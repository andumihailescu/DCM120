#include "dcm/dcm_command_dispatcher.h"

#include <cstddef>
#include <cstdint>

#include "dcm/dcm_commands_lookup_table.h"
#include "dcm/dcm_protocol.h"
#include "mcp2515/can.h"

namespace DCMCommandDispatcher
{
    bool dispatch(
        const can_frame &frame,
        ECUContext *context)
    {
        if (context == nullptr ||
            frame.can_id != DCMProtocol::CommandCanId ||
            frame.can_dlc == 0 ||
            frame.can_dlc > CAN_MAX_DLEN)
        {
            return false;
        }

        const std::uint8_t commandId = frame.data[0];
        const std::uint8_t payloadLength =
            static_cast<std::uint8_t>(frame.can_dlc - 1u);
        const auto &command =
            DCMCommandsLookupTable::commands[commandId];

        if (command.commandId != commandId ||
            command.handler == nullptr ||
            command.payloadLength != payloadLength)
        {
            return false;
        }

        command.handler(
            &frame.data[1],
            payloadLength,
            context);
        return true;
    }
}
