#pragma once

#include <cstdint>

#include "hardware/gpio.h"
#include "input/pwm_sample.h"

class PwmInput
{
public:
    using Sample = PwmSample;

    PwmInput() = default;

    bool init(
        uint pin,
        std::uint16_t minimumPulseUs,
        std::uint16_t maximumPulseUs);

    Sample sample() const;

private:
    static void gpioIrqCallback(uint gpio, std::uint32_t events);
    void onEdge(std::uint32_t events);

    static PwmInput *activeInstance;

    uint pin = 0;
    std::uint16_t minimumPulseUs = 0;
    std::uint16_t maximumPulseUs = 0;

    volatile std::uint32_t riseTimeUs = 0;
    volatile std::uint32_t lastUpdateUs = 0;
    volatile std::uint16_t pulseUs = 0;
    volatile bool waitingForFall = false;
    volatile bool valid = false;
};
