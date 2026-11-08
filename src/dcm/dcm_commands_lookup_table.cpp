#include "dcm/dcm_commands_lookup_table.h"

#include "dcm/dcm_commands.h"
#include "dcm/dcm_protocol.h"

namespace
{
    std::array<
        DCMCommandsLookupTable::CommandEntry,
        DCMCommandsLookupTable::MaximumCommands>
    makeCommands()
    {
        std::array<
            DCMCommandsLookupTable::CommandEntry,
            DCMCommandsLookupTable::MaximumCommands>
            result{};

        constexpr std::uint8_t commandId =
            static_cast<std::uint8_t>(
                DCMProtocol::CommandId::SetVehicleState);

        result[commandId] = {
            commandId,
            DCMCommands::setVehicleState,
            DCMProtocol::VehicleStatePayloadLength,
            false,
        };

        return result;
    }
}

namespace DCMCommandsLookupTable
{
    const std::array<CommandEntry, MaximumCommands> commands =
        makeCommands();
}
