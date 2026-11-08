#pragma once

#include <cstdint>

struct PwmSample
{
    std::uint16_t pulseUs = 0;
    std::uint32_t lastUpdateUs = 0;
    bool valid = false;
};
