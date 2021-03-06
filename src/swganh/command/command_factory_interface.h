// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_COMMAND_COMMAND_FACTORY_INTERFACE_H_
#define SWGANH_COMMAND_COMMAND_FACTORY_INTERFACE_H_

#include <functional>
#include <memory>

#include "anh/hash_string.h"

namespace swganh {
namespace app {
    class SwganhKernel;
}

namespace messages {
namespace controllers {
    class CommandQueueEnqueue;
}}

namespace object {
    class ObjectController;
}

namespace command {
    
    class CommandInterface;
    struct CommandProperties;

    typedef std::function<
        std::shared_ptr<CommandInterface> (
            swganh::app::SwganhKernel* kernel,
            const CommandProperties& properties)
    > CommandCreator;

    /**
     * Used by the CommandService to create command instances for incoming requests.
     */
    class CommandFactoryInterface
    {
    public:
        virtual ~CommandFactoryInterface() {}

        /**
         * Adds a creator for a given command type. Only the most recently added creator for
         * a type is used.
         *
         * @param command The name/crc of the command.
         * @param creator The creator associated with the specified command.
         */
        virtual void AddCommandCreator(anh::HashString command, CommandCreator&& creator) = 0;

        /**
         * Removes the creator for a given command type if one is set.
         *
         * @param command The name/crc of the command.
         */
        virtual void RemoveCommandCreator(anh::HashString command) = 0;

        /**
         * Creates a new instance of a command.
         *
         * @param command The name/crc of the command.
         * @return New instance of the specified command.
         */
        virtual std::shared_ptr<CommandInterface> CreateCommand(anh::HashString command) = 0;
    };

}}

#endif  // SWGANH_COMMAND_COMMAND_FACTORY_INTERFACE_H_
