#include "dcm/dcm_commands.h"

#include <cstddef>

#include "dcm/dcm_protocol.h"
#include "drive/drive_controller.h"
#include "ecu_context/ecu_context.h"

namespace DCMCommands
{
    void setVehicleState(
        const std::uint8_t *payload,
        std::uint8_t payloadLength,
        ECUContext *context)
    {
        if (payload == nullptr ||
            payloadLength !=
                DCMProtocol::VehicleStatePayloadLength ||
            context == nullptr ||
            context->drive == nullptr)
        {
            return;
        }

        constexpr std::size_t driveModeIndex =
            static_cast<std::size_t>(
                DCMProtocol::VehicleStateField::DriveMode);

        context->drive->setMode(
            DCMProtocol::decodeDriveMode(
                payload[driveModeIndex]));
    }
}
