#pragma once

#include <cstdint>

#include "mcp2515/mcp2515.h"

// Command-agnostic transport for raw classic-CAN frames.
class CANManager
{
public:
    static constexpr uint32_t DefaultSpiClockHz = 10'000'000u;

    // Uses the Raspberry Pi Pico default SPI0 pins: MISO GP16, CS GP17,
    // SCK GP18 and MOSI GP19.
    CANManager();

    // Allows a board integration to override every SPI pin without changing
    // the MCP2515 driver.
    CANManager(
        spi_inst_t *spi,
        uint8_t chip_select_pin,
        uint8_t mosi_pin,
        uint8_t miso_pin,
        uint8_t clock_pin,
        uint32_t spi_clock_hz = DefaultSpiClockHz);

    // Configures an MCP2515 with an 8 MHz crystal for a 1 Mbit/s CAN bus.
    // Returns false at the first failed initialization stage.
    bool init();

    // Reads at most one raw frame. False also means no frame is pending; use
    // lastError() when the distinction between ERROR_NOMSG and an error matters.
    bool tryReceive(can_frame &frame);

    // CAN IDs use the can_frame flag layout from mcp2515/can.h. Standard,
    // extended and remote-request frames are accepted; error frames are not.
    bool sendFrame(
        canid_t can_id,
        const uint8_t *data,
        uint8_t length);

    bool isInitialized() const;
    MCP2515::ERROR lastError() const;

private:
    static bool isValidTransmitId(canid_t can_id);

    MCP2515 m_can;
    bool m_initialized = false;
    MCP2515::ERROR m_last_error = MCP2515::ERROR_FAILINIT;
};
