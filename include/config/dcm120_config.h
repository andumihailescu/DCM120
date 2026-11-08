#pragma once

#include <cstdint>

#ifndef DCM120_SPEED_INPUT_PIN
#define DCM120_SPEED_INPUT_PIN 15
#endif

#ifndef DCM120_ESC_OUTPUT_PIN
#define DCM120_ESC_OUTPUT_PIN 14
#endif

#ifndef DCM120_ESC_DIRECTION_PIN
#define DCM120_ESC_DIRECTION_PIN 13
#endif

#ifndef DCM120_CAN_RX_PIN
#define DCM120_CAN_RX_PIN 16
#endif

#ifndef DCM120_CAN_CS_PIN
#define DCM120_CAN_CS_PIN 17
#endif

#ifndef DCM120_CAN_SCK_PIN
#define DCM120_CAN_SCK_PIN 18
#endif

#ifndef DCM120_CAN_TX_PIN
#define DCM120_CAN_TX_PIN 19
#endif

namespace DCM120Config
{
    namespace Pins
    {
        inline constexpr unsigned int SpeedInput = DCM120_SPEED_INPUT_PIN;
        inline constexpr unsigned int EscOutput = DCM120_ESC_OUTPUT_PIN;
        inline constexpr unsigned int EscDirection =
            DCM120_ESC_DIRECTION_PIN;
        inline constexpr unsigned int CanRx = DCM120_CAN_RX_PIN;
        inline constexpr unsigned int CanCs = DCM120_CAN_CS_PIN;
        inline constexpr unsigned int CanSck = DCM120_CAN_SCK_PIN;
        inline constexpr unsigned int CanTx = DCM120_CAN_TX_PIN;
    }

    namespace RcPwm
    {
        inline constexpr std::uint16_t MinimumPulseUs = 900;
        inline constexpr std::uint16_t NeutralPulseUs = 1500;
        inline constexpr std::uint16_t MaximumPulseUs = 2100;
        inline constexpr std::uint32_t FramePeriodUs = 20000;
        inline constexpr std::uint32_t SignalTimeoutUs = 100000;

        static_assert(MinimumPulseUs < NeutralPulseUs);
        static_assert(NeutralPulseUs < MaximumPulseUs);
        static_assert(MaximumPulseUs < FramePeriodUs);
    }

    namespace Can
    {
        inline constexpr std::uint32_t SpiClockHz = 10000000;
        inline constexpr std::uint32_t RetryIntervalUs = 1000000;
        inline constexpr std::uint8_t FramesPerUpdate = 8;
    }

    namespace Safety
    {
        inline constexpr std::uint32_t WatchdogTimeoutMs = 250;
    }

    static_assert(Pins::SpeedInput != Pins::EscOutput);
    static_assert(Pins::SpeedInput != Pins::EscDirection);
    static_assert(Pins::SpeedInput != Pins::CanRx);
    static_assert(Pins::SpeedInput != Pins::CanCs);
    static_assert(Pins::SpeedInput != Pins::CanSck);
    static_assert(Pins::SpeedInput != Pins::CanTx);
    static_assert(Pins::EscOutput != Pins::EscDirection);
    static_assert(Pins::EscOutput != Pins::CanRx);
    static_assert(Pins::EscOutput != Pins::CanCs);
    static_assert(Pins::EscOutput != Pins::CanSck);
    static_assert(Pins::EscOutput != Pins::CanTx);
    static_assert(Pins::EscDirection != Pins::CanRx);
    static_assert(Pins::EscDirection != Pins::CanCs);
    static_assert(Pins::EscDirection != Pins::CanSck);
    static_assert(Pins::EscDirection != Pins::CanTx);
    static_assert(Pins::CanRx != Pins::CanCs);
    static_assert(Pins::CanRx != Pins::CanSck);
    static_assert(Pins::CanRx != Pins::CanTx);
    static_assert(Pins::CanCs != Pins::CanSck);
    static_assert(Pins::CanCs != Pins::CanTx);
    static_assert(Pins::CanSck != Pins::CanTx);

    static_assert(Pins::SpeedInput < 30u);
    static_assert(Pins::EscOutput < 30u);
    static_assert(Pins::EscDirection < 30u);
    static_assert(Pins::CanRx < 30u);
    static_assert(Pins::CanCs < 30u);
    static_assert(Pins::CanSck < 30u);
    static_assert(Pins::CanTx < 30u);

    // EscOutput instances configure an entire PWM slice during init. Keep the
    // independent throttle and direction outputs on separate slices so one
    // cannot reset the other's compare register. This mapping is valid for
    // the GPIO range accepted above on RP2350.
    static_assert(
        ((Pins::EscOutput >> 1u) & 7u) !=
            ((Pins::EscDirection >> 1u) & 7u),
        "ESC throttle and direction GPIOs must use different PWM slices");

    // SPI0 alternate-function mappings available on RP2350 GPIOs.
    static_assert(
        Pins::CanRx == 0u || Pins::CanRx == 4u ||
            Pins::CanRx == 16u || Pins::CanRx == 20u,
        "DCM120_CAN_RX_PIN must be a valid SPI0 RX GPIO");
    static_assert(
        Pins::CanSck == 2u || Pins::CanSck == 6u ||
            Pins::CanSck == 18u || Pins::CanSck == 22u,
        "DCM120_CAN_SCK_PIN must be a valid SPI0 SCK GPIO");
    static_assert(
        Pins::CanTx == 3u || Pins::CanTx == 7u ||
            Pins::CanTx == 19u || Pins::CanTx == 23u,
        "DCM120_CAN_TX_PIN must be a valid SPI0 TX GPIO");
}
