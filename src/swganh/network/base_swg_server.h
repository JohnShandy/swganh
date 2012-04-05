// This file is part of SWGANH which is released under GPL v2.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_NETWORK_BASE_SWG_SERVER_H_
#define SWGANH_NETWORK_BASE_SWG_SERVER_H_

#include <functional>
#include <map>
#include <memory>

#ifdef WIN32
#include <concurrent_unordered_map.h>
#else
#include <tbb/concurrent_unordered_map.h>

namespace Concurrency {
    using ::tbb::concurrent_unordered_map;
}

#endif

#include "anh/byte_buffer.h"
#include "anh/hash_string.h"
#include "anh/network/soe/server.h"

namespace anh {

    class ByteBuffer;

}  // namespace anh::network::soe


namespace swganh {
namespace network {

    class BaseSwgServer : public anh::network::soe::Server
    {
    public:
        BaseSwgServer(boost::asio::io_service& io_service);

        template<typename ConnectionType, typename MessageType>
        struct GenericMessageHandler
        {
            typedef std::function<void (
                const std::shared_ptr<ConnectionType>&, MessageType)
            > HandlerType;
        };

        typedef std::function<void (
            const std::shared_ptr<anh::network::soe::Session>&,
            anh::ByteBuffer message)
        > SwgMessageHandler;

        typedef std::runtime_error HandlerAlreadyDefined;
        typedef std::runtime_error UnidentifiedMessageReceived;
        typedef std::runtime_error ValidClientRequired;

        void HandleMessage(
            const std::shared_ptr<anh::network::soe::Session>& connection,
            anh::ByteBuffer message);

        /**
         * Register's a message handler for processing SWG protocol messages.
         *
         * This overload accepts a member function and a pointer (either naked or smart)
         * and converts the request to the proper connection and message types.
         *
         * \code{.cpp}
         *
         *  RegisterMessageHandler(&MyClass::HandleSomeMessage, this);
         *
         * \param memfunc A member function that can process a concrete SWG protocol message.
         * \param instance An instance of a class that implements memfunc.
         */
        template<typename T, typename U, typename ConnectionType, typename MessageType>
        void RegisterMessageHandler(void (T::*memfunc)(const std::shared_ptr<ConnectionType>&, MessageType), U instance)
        {
            RegisterMessageHandler<ConnectionType, MessageType>(std::bind(memfunc, instance, std::placeholders::_1, std::placeholders::_2));
        }
        
        /**
         * Register's a message handler for processing SWG protocol messages.
         *
         * This handler automatically converts the request to the proper connection 
         * and message types.
         *
         * \param handler A std::function object representing the handler.
         */
        template<typename ConnectionType, typename MessageType>
        void RegisterMessageHandler(
            typename GenericMessageHandler<ConnectionType, MessageType>::HandlerType&& handler)
        {
            auto shared_handler = std::make_shared<typename GenericMessageHandler<ConnectionType, MessageType>::HandlerType>(std::move(handler));

            auto wrapped_handler = [this, shared_handler] (
                const std::shared_ptr<anh::network::soe::Session>& client,
                anh::ByteBuffer message)
            {
                MessageType tmp;
                tmp.Deserialize(std::move(message));

                (*shared_handler)(std::static_pointer_cast<ConnectionType>(client), std::move(tmp));
            };

            RegisterMessageHandler(MessageType::Opcode(), std::move(wrapped_handler));
        }

        /**
         * Register's a message handler for processing SWG protocol messages.
         *
         * This is the low level registration and should be used when wanting to bypass
         * automatic message conversion.
         *
         * \param handler_id The SWG protocol id for the handler's message type.
         * \param handler The SWG protocol handler.
         */
        void RegisterMessageHandler(
            anh::HashString handler_id,
            SwgMessageHandler&& handler);

        bool HasHandler(anh::HashString handler_id);

    private:
        typedef Concurrency::concurrent_unordered_map<
            anh::HashString,
            SwgMessageHandler
        > MessageHandlerMap;

        MessageHandlerMap message_handlers_;
    };

}}  // namespace swganh::network

#endif  // SWGANH_NETWORK_BASE_SWG_SERVER_H_
