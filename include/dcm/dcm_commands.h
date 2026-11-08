#pragma once

#include <cstdint>

struct ECUContext;

namespace DCMCommands
{
    void setVehicleState(
        const std::uint8_t *payload,
        std::uint8_t payloadLength,
        ECUContext *context);
}
