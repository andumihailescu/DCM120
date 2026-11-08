#pragma once

#include <cstdint>

#include "esc/esc_direction_signal.h"

enum class DriveMode : std::uint8_t
{
    Neutral,
    Forward,
    Reverse,
};

enum class DriveTransitionPhase : std::uint8_t
{
    None,
    NeutralBeforeDirection,
    DirectionSettling,
};

namespace DriveModePolicy
{
    inline constexpr DriveMode InitialMode = DriveMode::Neutral;

    constexpr DriveMode normalize(DriveMode mode)
    {
        switch (mode)
        {
        case DriveMode::Forward:
        case DriveMode::Reverse:
            return mode;
        case DriveMode::Neutral:
        default:
            return DriveMode::Neutral;
        }
    }

    constexpr std::uint16_t selectThrottlePulseUs(
        DriveMode mode,
        std::uint16_t requestedPulseUs,
        std::uint16_t neutralPulseUs,
        bool transitionInProgress = false)
    {
        const bool throttleInhibited =
            normalize(mode) == DriveMode::Neutral ||
            transitionInProgress;

        return throttleInhibited
                   ? neutralPulseUs
                   : requestedPulseUs;
    }

    constexpr bool settleElapsed(
        std::uint32_t nowUs,
        std::uint32_t settleStartedUs,
        std::uint32_t settleDurationUs)
    {
        return nowUs - settleStartedUs >= settleDurationUs;
    }

    constexpr DriveTransitionPhase nextTransitionPhase(
        DriveTransitionPhase phase)
    {
        switch (phase)
        {
        case DriveTransitionPhase::NeutralBeforeDirection:
            return DriveTransitionPhase::DirectionSettling;
        case DriveTransitionPhase::DirectionSettling:
            return DriveTransitionPhase::None;
        case DriveTransitionPhase::None:
        default:
            return DriveTransitionPhase::None;
        }
    }

    constexpr DriveTransitionPhase preserveDirectionSettling(
        DriveTransitionPhase phase)
    {
        return phase == DriveTransitionPhase::DirectionSettling
                   ? phase
                   : DriveTransitionPhase::None;
    }

    constexpr MotorDirection directionFor(DriveMode mode)
    {
        return normalize(mode) == DriveMode::Reverse
                   ? MotorDirection::Reverse
                   : MotorDirection::Forward;
    }

    static_assert(
        selectThrottlePulseUs(DriveMode::Neutral, 1000, 1500) ==
        1500);
    static_assert(
        selectThrottlePulseUs(DriveMode::Neutral, 2000, 1500) ==
        1500);
    static_assert(
        selectThrottlePulseUs(
            DriveMode::Forward,
            2000,
            1500,
            true) == 1500);
    static_assert(settleElapsed(20100, 100, 20000));
    static_assert(!settleElapsed(20099, 100, 20000));
    static_assert(settleElapsed(0x00000010u, 0xFFFFFFF0u, 32u));
    static_assert(
        nextTransitionPhase(
            DriveTransitionPhase::NeutralBeforeDirection) ==
        DriveTransitionPhase::DirectionSettling);
    static_assert(
        nextTransitionPhase(DriveTransitionPhase::DirectionSettling) ==
        DriveTransitionPhase::None);
    static_assert(
        preserveDirectionSettling(
            DriveTransitionPhase::DirectionSettling) ==
        DriveTransitionPhase::DirectionSettling);
    static_assert(
        preserveDirectionSettling(
            DriveTransitionPhase::NeutralBeforeDirection) ==
        DriveTransitionPhase::None);
    static_assert(directionFor(DriveMode::Forward) ==
                  MotorDirection::Forward);
    static_assert(directionFor(DriveMode::Neutral) ==
                  MotorDirection::Forward);
    static_assert(directionFor(DriveMode::Reverse) ==
                  MotorDirection::Reverse);
}
