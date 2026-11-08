#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "dcm/dcm_commands_type.h"

namespace DCMCommandsLookupTable
{
    inline constexpr std::size_t MaximumCommands = 256;

    struct CommandEntry
    {
        std::uint8_t commandId = 0;
        CommandHandler handler = nullptr;
        std::uint8_t payloadLength = 0;
        bool hasConcatenatedData = false;
    };

    extern const std::array<CommandEntry, MaximumCommands> commands;
}
