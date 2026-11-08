#include "drive/drive_controller.h"

#include <algorithm>

#include "pico/time.h"

bool DriveController::init(
    uint throttlePin,
    uint directionPin,
    std::uint16_t outputMinimumPulseUs,
    std::uint16_t outputNeutralPulseUs,
    std::uint16_t outputMaximumPulseUs,
    std::uint32_t framePeriodUs,
    DriveMode initialMode)
{
    initialized = false;
    currentMode = DriveModePolicy::normalize(initialMode);

    if (!throttleOutput.init(
            throttlePin,
            outputMinimumPulseUs,
            outputNeutralPulseUs,
            outputMaximumPulseUs,
            framePeriodUs))
    {
        return false;
    }

    minimumPulseUs = outputMinimumPulseUs;
    neutralPulseUs = outputNeutralPulseUs;
    maximumPulseUs = outputMaximumPulseUs;
    lastThrottlePulseUs = neutralPulseUs;
    transitionStartedUs = 0;
    transitionPhaseDurationUs = framePeriodUs;
    transitionPhase = DriveTransitionPhase::None;

    if (!directionOutput.init(
            directionPin,
            framePeriodUs,
            DriveModePolicy::directionFor(currentMode)))
    {
        throttleOutput.setNeutral();
        return false;
    }

    initialized = true;
    return true;
}

void DriveController::setMode(DriveMode mode)
{
    if (!initialized)
    {
        return;
    }

    const DriveMode requestedMode = DriveModePolicy::normalize(mode);

    if (requestedMode == currentMode)
    {
        return;
    }

    // Remove torque before changing or disabling the selected direction.
    throttleOutput.setNeutral();
    lastThrottlePulseUs = neutralPulseUs;

    currentMode = requestedMode;

    const MotorDirection requestedDirection =
        DriveModePolicy::directionFor(currentMode);

    if (requestedDirection == directionOutput.direction())
    {
        transitionPhase =
            DriveModePolicy::preserveDirectionSettling(
                transitionPhase);
        return;
    }

    // The throttle and direction pins use independent PWM slices. Wait one
    // complete period so the neutral compare value is guaranteed to have
    // latched before changing the direction compare value.
    transitionStartedUs = time_us_32();
    transitionPhase =
        DriveTransitionPhase::NeutralBeforeDirection;
}

DriveMode DriveController::mode() const
{
    return currentMode;
}

void DriveController::setThrottleRequestUs(
    std::uint16_t requestedPulseUs)
{
    setThrottleRequestUs(requestedPulseUs, time_us_32());
}

void DriveController::setThrottleRequestUs(
    std::uint16_t requestedPulseUs,
    std::uint32_t nowUs)
{
    if (!initialized)
    {
        return;
    }

    advanceTransition(nowUs);

    const std::uint16_t safeRequestedPulseUs = std::clamp(
        requestedPulseUs,
        minimumPulseUs,
        maximumPulseUs);
    const std::uint16_t outputPulseUs =
        DriveModePolicy::selectThrottlePulseUs(
            currentMode,
            safeRequestedPulseUs,
            neutralPulseUs,
            transitionPhase != DriveTransitionPhase::None);

    if (outputPulseUs == lastThrottlePulseUs)
    {
        return;
    }

    throttleOutput.setPulseUs(outputPulseUs);
    lastThrottlePulseUs = outputPulseUs;
}

std::uint16_t DriveController::throttlePulseUs() const
{
    return lastThrottlePulseUs;
}

MotorDirection DriveController::direction() const
{
    return directionOutput.direction();
}

bool DriveController::transitionInProgress() const
{
    return transitionPhase != DriveTransitionPhase::None;
}

void DriveController::advanceTransition(std::uint32_t nowUs)
{
    if (transitionPhase == DriveTransitionPhase::None ||
        !DriveModePolicy::settleElapsed(
            nowUs,
            transitionStartedUs,
            transitionPhaseDurationUs))
    {
        return;
    }

    if (transitionPhase ==
        DriveTransitionPhase::NeutralBeforeDirection)
    {
        directionOutput.setDirection(
            DriveModePolicy::directionFor(currentMode));
        transitionStartedUs = nowUs;
        transitionPhase =
            DriveModePolicy::nextTransitionPhase(transitionPhase);
        return;
    }

    transitionPhase =
        DriveModePolicy::nextTransitionPhase(transitionPhase);
}
