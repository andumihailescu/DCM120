#include "esc/esc_direction_output.h"

bool EscDirectionOutput::init(
    uint outputPin,
    std::uint32_t framePeriodUs,
    MotorDirection initialDirection)
{
    const std::uint16_t initialPulseUs =
        EscDirectionSignal::pulseUs(initialDirection);

    if (!pwmOutput.init(
            outputPin,
            EscDirectionSignal::ReversePulseUs,
            initialPulseUs,
            EscDirectionSignal::ForwardPulseUs,
            framePeriodUs))
    {
        return false;
    }

    currentDirection =
        initialPulseUs == EscDirectionSignal::ReversePulseUs
            ? MotorDirection::Reverse
            : MotorDirection::Forward;
    initialized = true;
    return true;
}

void EscDirectionOutput::setDirection(MotorDirection direction)
{
    if (!initialized)
    {
        return;
    }

    const std::uint16_t pulseUs =
        EscDirectionSignal::pulseUs(direction);
    pwmOutput.setPulseUs(pulseUs);
    currentDirection =
        pulseUs == EscDirectionSignal::ReversePulseUs
            ? MotorDirection::Reverse
            : MotorDirection::Forward;
}

MotorDirection EscDirectionOutput::direction() const
{
    return currentDirection;
}
