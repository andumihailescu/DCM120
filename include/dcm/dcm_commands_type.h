#pragma once

#include <cstdint>

struct ECUContext;

using CommandHandler = void (*)(
    const std::uint8_t *payload,
    std::uint8_t payloadLength,
    ECUContext *context);
