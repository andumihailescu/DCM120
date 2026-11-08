#pragma once

#include <cstdint>

#include "esc/esc_direction_signal.h"
#include "esc/esc_output.h"

class EscDirectionOutput
{
public:
    EscDirectionOutput() = default;

    bool init(
        uint pin,
        std::uint32_t framePeriodUs,
        MotorDirection initialDirection =
            EscDirectionSignal::InitialDirection);

    void setDirection(MotorDirection direction);
    MotorDirection direction() const;

private:
    EscOutput pwmOutput;
    MotorDirection currentDirection =
        EscDirectionSignal::InitialDirection;
    bool initialized = false;
};
