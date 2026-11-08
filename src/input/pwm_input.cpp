#include "input/pwm_input.h"

#include "hardware/sync.h"
#include "pico/time.h"

PwmInput *PwmInput::activeInstance = nullptr;

bool PwmInput::init(
    uint inputPin,
    std::uint16_t inputMinimumPulseUs,
    std::uint16_t inputMaximumPulseUs)
{
    if (inputMinimumPulseUs >= inputMaximumPulseUs ||
        (activeInstance != nullptr && activeInstance != this))
    {
        return false;
    }

    pin = inputPin;
    minimumPulseUs = inputMinimumPulseUs;
    maximumPulseUs = inputMaximumPulseUs;

    riseTimeUs = 0;
    lastUpdateUs = 0;
    pulseUs = 0;
    waitingForFall = false;
    valid = false;

    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_down(pin);

    activeInstance = this;
    gpio_set_irq_enabled_with_callback(
        pin,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &PwmInput::gpioIrqCallback);

    return true;
}

PwmInput::Sample PwmInput::sample() const
{
    const std::uint32_t interruptState = save_and_disable_interrupts();

    const Sample snapshot{
        pulseUs,
        lastUpdateUs,
        valid,
    };

    restore_interrupts(interruptState);
    return snapshot;
}

void PwmInput::gpioIrqCallback(uint gpio, std::uint32_t events)
{
    if (activeInstance != nullptr && gpio == activeInstance->pin)
    {
        activeInstance->onEdge(events);
    }
}

void PwmInput::onEdge(std::uint32_t events)
{
    const std::uint32_t nowUs = time_us_32();

    if ((events & (GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL)) ==
        (GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL))
    {
        // The pulse cannot be reconstructed unambiguously if both edge bits
        // accumulated before the callback ran. Fail safe until a clean pulse.
        lastUpdateUs = nowUs;
        waitingForFall = false;
        valid = false;
        return;
    }

    if ((events & GPIO_IRQ_EDGE_RISE) != 0u)
    {
        riseTimeUs = nowUs;
        waitingForFall = true;
    }

    if ((events & GPIO_IRQ_EDGE_FALL) == 0u)
    {
        return;
    }

    lastUpdateUs = nowUs;

    if (!waitingForFall)
    {
        valid = false;
        return;
    }

    waitingForFall = false;
    const std::uint32_t measuredPulseUs = nowUs - riseTimeUs;

    if (measuredPulseUs < minimumPulseUs ||
        measuredPulseUs > maximumPulseUs)
    {
        valid = false;
        return;
    }

    pulseUs = static_cast<std::uint16_t>(measuredPulseUs);
    valid = true;
}
