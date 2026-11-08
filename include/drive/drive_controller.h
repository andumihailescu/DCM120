#pragma once

#include <cstdint>

#include "drive/drive_mode.h"
#include "esc/esc_direction_output.h"
#include "esc/esc_output.h"

class DriveController
{
public:
    DriveController() = default;

    bool init(
        uint throttlePin,
        uint directionPin,
        std::uint16_t minimumPulseUs,
        std::uint16_t neutralPulseUs,
        std::uint16_t maximumPulseUs,
        std::uint32_t framePeriodUs,
        DriveMode initialMode = DriveModePolicy::InitialMode);

    void setMode(DriveMode mode);
    DriveMode mode() const;

    void setThrottleRequestUs(std::uint16_t requestedPulseUs);
    void setThrottleRequestUs(
        std::uint16_t requestedPulseUs,
        std::uint32_t nowUs);
    std::uint16_t throttlePulseUs() const;
    MotorDirection direction() const;
    bool transitionInProgress() const;

private:
    void advanceTransition(std::uint32_t nowUs);

    EscOutput throttleOutput;
    EscDirectionOutput directionOutput;

    std::uint16_t minimumPulseUs = 0;
    std::uint16_t neutralPulseUs = 0;
    std::uint16_t maximumPulseUs = 0;
    std::uint16_t lastThrottlePulseUs = 0;
    std::uint32_t transitionStartedUs = 0;
    std::uint32_t transitionPhaseDurationUs = 0;
    DriveMode currentMode = DriveModePolicy::InitialMode;
    DriveTransitionPhase transitionPhase =
        DriveTransitionPhase::None;
    bool initialized = false;
};
