#include "app/app.h"

#include "config/dcm120_config.h"
#include "dcm/dcm_command_dispatcher.h"
#include "input/throttle_policy.h"
#include "hardware/spi.h"
#include "pico/time.h"

App::App()
    : can(
          spi0,
          static_cast<std::uint8_t>(DCM120Config::Pins::CanCs),
          static_cast<std::uint8_t>(DCM120Config::Pins::CanTx),
          static_cast<std::uint8_t>(DCM120Config::Pins::CanRx),
          static_cast<std::uint8_t>(DCM120Config::Pins::CanSck),
          DCM120Config::Can::SpiClockHz)
{
}

bool App::init()
{
    // Establish a neutral drive mode before resetting/enabling the optional
    // CAN controller. Its constructor only prepares the SPI GPIO peripheral.
    if (!drive.init(
            DCM120Config::Pins::EscOutput,
            DCM120Config::Pins::EscDirection,
            DCM120Config::RcPwm::MinimumPulseUs,
            DCM120Config::RcPwm::NeutralPulseUs,
            DCM120Config::RcPwm::MaximumPulseUs,
            DCM120Config::RcPwm::FramePeriodUs,
            DriveModePolicy::InitialMode))
    {
        return false;
    }

    if (!speedInput.init(
            DCM120Config::Pins::SpeedInput,
            DCM120Config::RcPwm::MinimumPulseUs,
            DCM120Config::RcPwm::MaximumPulseUs))
    {
        drive.setMode(DriveMode::Neutral);
        return false;
    }

    context.can = &can;
    context.drive = &drive;
    canReady = can.init();
    lastCanInitAttemptUs = time_us_32();
    return true;
}

void App::update()
{
    const PwmInput::Sample input = speedInput.sample();
    // Read time after the IRQ-protected snapshot so lastUpdateUs can never be
    // newer than nowUs because of an edge arriving between the two reads.
    const std::uint32_t nowUs = time_us_32();
    const std::uint16_t requestedPulseUs =
        ThrottlePolicy::selectOutputPulse(
            input,
            nowUs,
            DCM120Config::RcPwm::SignalTimeoutUs,
            DCM120Config::RcPwm::NeutralPulseUs);

    drive.setThrottleRequestUs(requestedPulseUs, nowUs);

    serviceCan();
}

void App::setDriveMode(DriveMode mode)
{
    drive.setMode(mode);
}

DriveMode App::driveMode() const
{
    return drive.mode();
}

void App::setMotorDirection(MotorDirection direction)
{
    setDriveMode(
        direction == MotorDirection::Reverse
            ? DriveMode::Reverse
            : DriveMode::Forward);
}

MotorDirection App::motorDirection() const
{
    return drive.direction();
}

void App::serviceCan()
{
    if (!canReady)
    {
        // A controller reset is synchronous and includes the MCP2515 startup
        // delay. Never add that latency while a non-neutral throttle command
        // is active; local failsafe behavior has priority over CAN recovery.
        if (drive.throttlePulseUs() !=
            DCM120Config::RcPwm::NeutralPulseUs)
        {
            return;
        }

        const std::uint32_t nowUs = time_us_32();

        if (nowUs - lastCanInitAttemptUs >=
            DCM120Config::Can::RetryIntervalUs)
        {
            canReady = can.init();
            lastCanInitAttemptUs = time_us_32();
        }

        return;
    }

    can_frame frame{};

    for (std::uint8_t index = 0;
         index < DCM120Config::Can::FramesPerUpdate;
         ++index)
    {
        if (!can.tryReceive(frame))
        {
            if (can.lastError() == MCP2515::ERROR_NOMSG)
            {
                break;
            }

            // A malformed frame is discarded by the driver. Continue so a
            // valid frame in the other MCP2515 receive buffer is still read.
            continue;
        }

        DCMCommandDispatcher::dispatch(frame, &context);
    }
}
