#pragma once

#include <cstdint>

#include "input/pwm_sample.h"

namespace ThrottlePolicy
{
    constexpr bool isFresh(
        const PwmSample &sample,
        std::uint32_t nowUs,
        std::uint32_t timeoutUs)
    {
        // return sample.valid &&
        //        (nowUs - sample.lastUpdateUs <= timeoutUs);
        return (nowUs - sample.lastUpdateUs <= timeoutUs);
    }

    constexpr std::uint16_t selectOutputPulse(
        const PwmSample &sample,
        std::uint32_t nowUs,
        std::uint32_t timeoutUs,
        std::uint16_t neutralPulseUs)
    {
        return isFresh(sample, nowUs, timeoutUs)
                   ? sample.pulseUs
                   : neutralPulseUs;
    }

    static_assert(isFresh(PwmSample{1500, 100, true}, 200, 100));
    static_assert(!isFresh(PwmSample{1500, 100, true}, 201, 100));
    static_assert(isFresh(
        PwmSample{1500, 0xFFFFFFF0u, true},
        0x00000010u,
        32u));
    // static_assert(!isFresh(PwmSample{1500, 100, false}, 100, 100));
}
