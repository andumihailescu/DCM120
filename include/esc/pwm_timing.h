#pragma once

#include <cstdint>

namespace PwmTiming
{
    inline constexpr std::uint32_t TargetCounterHz = 1000000u;
    inline constexpr std::uint32_t DividerScale = 16u;
    inline constexpr std::uint32_t MinimumDivider16 = 16u;
    inline constexpr std::uint32_t MaximumDivider16 = 4095u;

    constexpr std::uint32_t divider16(std::uint32_t systemClockHz)
    {
        return static_cast<std::uint32_t>(
            (static_cast<std::uint64_t>(DividerScale) * systemClockHz +
             (TargetCounterHz / 2u)) /
            TargetCounterHz);
    }

    constexpr std::uint32_t microsecondsToCounts(
        std::uint32_t systemClockHz,
        std::uint32_t dividerTimes16,
        std::uint32_t microseconds)
    {
        if (dividerTimes16 == 0u)
        {
            return 0u;
        }

        const std::uint64_t denominator =
            static_cast<std::uint64_t>(dividerTimes16) * 1000000u;
        const std::uint64_t numerator =
            static_cast<std::uint64_t>(DividerScale) *
            systemClockHz *
            microseconds;

        return static_cast<std::uint32_t>(
            (numerator + (denominator / 2u)) / denominator);
    }

    static_assert(divider16(125000000u) == 2000u);
    static_assert(divider16(150000000u) == 2400u);
    static_assert(divider16(200000000u) == 3200u);
    static_assert(microsecondsToCounts(150000000u, 2400u, 20000u) ==
                  20000u);
    static_assert(microsecondsToCounts(150000000u, 2400u, 1500u) ==
                  1500u);
}
