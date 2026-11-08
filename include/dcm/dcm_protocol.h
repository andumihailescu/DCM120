#pragma once

#include <cstddef>
#include <cstdint>

#include "drive/drive_mode.h"

namespace DCMProtocol
{
    inline constexpr std::uint16_t ResponseCanId = 0x120;
    inline constexpr std::uint16_t CommandCanId = 0x121;

    enum class CommandId : std::uint8_t
    {
        NotImplemented = 0x00,
        SoftwareVersion = 0x01,
        Reserved = 0x02,
        SetVehicleState = 0x03,
    };

    enum class VehicleStateField : std::size_t
    {
        DriveMode = 0,
    };

    inline constexpr std::uint8_t VehicleStatePayloadLength = 1;

    enum class WireDriveMode : std::uint8_t
    {
        Forward = 0x00,
        Neutral = 0x01,
        Reverse = 0x02,
    };

    constexpr DriveMode decodeDriveMode(std::uint8_t value)
    {
        switch (static_cast<WireDriveMode>(value))
        {
        case WireDriveMode::Forward:
            return DriveMode::Forward;
        case WireDriveMode::Reverse:
            return DriveMode::Reverse;
        case WireDriveMode::Neutral:
        default:
            return DriveMode::Neutral;
        }
    }

    static_assert(CommandCanId == 0x121);
    static_assert(
        static_cast<std::uint8_t>(CommandId::SetVehicleState) ==
        0x03);
    static_assert(decodeDriveMode(0x00) == DriveMode::Forward);
    static_assert(decodeDriveMode(0x01) == DriveMode::Neutral);
    static_assert(decodeDriveMode(0x02) == DriveMode::Reverse);
    static_assert(decodeDriveMode(0xFF) == DriveMode::Neutral);
}
