#include "can/can_manager.h"

#include <cstring>

CANManager::CANManager()
    : m_can()
{
}

CANManager::CANManager(
    spi_inst_t *spi,
    uint8_t chip_select_pin,
    uint8_t mosi_pin,
    uint8_t miso_pin,
    uint8_t clock_pin,
    uint32_t spi_clock_hz)
    : m_can(
          spi,
          chip_select_pin,
          mosi_pin,
          miso_pin,
          clock_pin,
          spi_clock_hz)
{
}

bool CANManager::init()
{
    m_initialized = false;

    m_last_error = m_can.reset();
    if (m_last_error != MCP2515::ERROR_OK)
    {
        return false;
    }

    m_last_error = m_can.setBitrate(CAN_1000KBPS, MCP_8MHZ);
    if (m_last_error != MCP2515::ERROR_OK)
    {
        return false;
    }

    m_last_error = m_can.setNormalMode();
    if (m_last_error != MCP2515::ERROR_OK)
    {
        return false;
    }

    m_initialized = true;
    return true;
}

bool CANManager::tryReceive(can_frame &frame)
{
    frame = {};

    if (!m_initialized)
    {
        m_last_error = MCP2515::ERROR_FAILINIT;
        return false;
    }

    m_last_error = m_can.readMessage(&frame);
    return m_last_error == MCP2515::ERROR_OK;
}

bool CANManager::sendFrame(
    canid_t can_id,
    const uint8_t *data,
    uint8_t length)
{
    if (length > CAN_MAX_DLEN ||
        (data == nullptr && length != 0u) ||
        !isValidTransmitId(can_id))
    {
        m_last_error = MCP2515::ERROR_FAILTX;
        return false;
    }

    if (!m_initialized)
    {
        m_last_error = MCP2515::ERROR_FAILINIT;
        return false;
    }

    can_frame frame{};
    frame.can_id = can_id;
    frame.can_dlc = length;

    if (length != 0u)
    {
        std::memcpy(frame.data, data, length);
    }

    m_last_error = m_can.sendMessage(&frame);
    return m_last_error == MCP2515::ERROR_OK;
}

bool CANManager::isInitialized() const
{
    return m_initialized;
}

MCP2515::ERROR CANManager::lastError() const
{
    return m_last_error;
}

bool CANManager::isValidTransmitId(canid_t can_id)
{
    if ((can_id & CAN_ERR_FLAG) != 0u)
    {
        return false;
    }

    if ((can_id & CAN_EFF_FLAG) != 0u)
    {
        constexpr canid_t allowed_bits =
            CAN_EFF_FLAG | CAN_RTR_FLAG | CAN_EFF_MASK;
        return (can_id & ~allowed_bits) == 0u;
    }

    constexpr canid_t allowed_bits = CAN_RTR_FLAG | CAN_SFF_MASK;
    return (can_id & ~allowed_bits) == 0u;
}
