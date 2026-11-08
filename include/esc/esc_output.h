#pragma once

#include <cstdint>

#include "hardware/pwm.h"

class EscOutput
{
public:
    EscOutput() = default;

    bool init(
        uint pin,
        std::uint16_t minimumPulseUs,
        std::uint16_t neutralPulseUs,
        std::uint16_t maximumPulseUs,
        std::uint32_t framePeriodUs);

    void setPulseUs(std::uint16_t pulseUs);
    void setNeutral();

private:
    uint slice = 0;
    uint channel = 0;
    std::uint16_t minimumPulseUs = 0;
    std::uint16_t neutralPulseUs = 0;
    std::uint16_t maximumPulseUs = 0;
    std::uint32_t systemClockHz = 0;
    std::uint32_t dividerTimes16 = 0;
    bool initialized = false;
};
