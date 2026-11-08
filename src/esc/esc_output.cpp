#include "esc/esc_output.h"

#include <algorithm>
#include <limits>

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "esc/pwm_timing.h"

bool EscOutput::init(
    uint outputPin,
    std::uint16_t outputMinimumPulseUs,
    std::uint16_t outputNeutralPulseUs,
    std::uint16_t outputMaximumPulseUs,
    std::uint32_t framePeriodUs)
{
    if (outputMinimumPulseUs > outputNeutralPulseUs ||
        outputNeutralPulseUs > outputMaximumPulseUs ||
        outputMaximumPulseUs >= framePeriodUs ||
        framePeriodUs == 0u ||
        framePeriodUs > static_cast<std::uint32_t>(
                            std::numeric_limits<std::uint16_t>::max()) +
                            1u)
    {
        return false;
    }

    systemClockHz = clock_get_hz(clk_sys);
    dividerTimes16 = PwmTiming::divider16(systemClockHz);

    if (dividerTimes16 < PwmTiming::MinimumDivider16 ||
        dividerTimes16 > PwmTiming::MaximumDivider16)
    {
        return false;
    }

    const std::uint32_t periodCounts = PwmTiming::microsecondsToCounts(
        systemClockHz,
        dividerTimes16,
        framePeriodUs);
    const std::uint32_t maximumPulseCounts =
        PwmTiming::microsecondsToCounts(
            systemClockHz,
            dividerTimes16,
            outputMaximumPulseUs);

    if (periodCounts == 0u || periodCounts > 65536u ||
        maximumPulseCounts >= periodCounts)
    {
        return false;
    }

    minimumPulseUs = outputMinimumPulseUs;
    neutralPulseUs = outputNeutralPulseUs;
    maximumPulseUs = outputMaximumPulseUs;

    gpio_init(outputPin);
    gpio_pull_down(outputPin);
    gpio_put(outputPin, false);
    gpio_set_dir(outputPin, GPIO_OUT);

    slice = pwm_gpio_to_slice_num(outputPin);
    channel = pwm_gpio_to_channel(outputPin);

    pwm_set_enabled(slice, false);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv_int_frac4(
        &config,
        dividerTimes16 >> 4u,
        dividerTimes16 & 0x0Fu);
    pwm_config_set_wrap(
        &config,
        static_cast<std::uint16_t>(periodCounts - 1u));
    pwm_init(slice, &config, false);

    const std::uint32_t neutralCounts = PwmTiming::microsecondsToCounts(
        systemClockHz,
        dividerTimes16,
        neutralPulseUs);
    pwm_set_chan_level(slice, channel, neutralCounts);
    pwm_set_counter(slice, 0u);
    gpio_set_function(outputPin, GPIO_FUNC_PWM);
    pwm_set_enabled(slice, true);

    initialized = true;
    return true;
}

void EscOutput::setPulseUs(std::uint16_t outputPulseUs)
{
    if (!initialized)
    {
        return;
    }

    const std::uint16_t safePulseUs = std::clamp(
        outputPulseUs,
        minimumPulseUs,
        maximumPulseUs);

    const std::uint32_t pulseCounts = PwmTiming::microsecondsToCounts(
        systemClockHz,
        dividerTimes16,
        safePulseUs);
    pwm_set_chan_level(slice, channel, pulseCounts);
}

void EscOutput::setNeutral()
{
    setPulseUs(neutralPulseUs);
}
