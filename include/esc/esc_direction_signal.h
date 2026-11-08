#pragma once

#include <cstdint>

enum class MotorDirection : std::uint8_t
{
    Reverse,
    Forward,
};

namespace EscDirectionSignal
{
    inline constexpr std::uint16_t ReversePulseUs = 1000;
    inline constexpr std::uint16_t ForwardPulseUs = 2000;
    inline constexpr MotorDirection InitialDirection =
        MotorDirection::Forward;

    constexpr std::uint16_t pulseUs(MotorDirection direction)
    {
        return direction == MotorDirection::Reverse
                   ? ReversePulseUs
                   : ForwardPulseUs;
    }

    static_assert(pulseUs(MotorDirection::Reverse) == 1000);
    static_assert(pulseUs(MotorDirection::Forward) == 2000);
}
