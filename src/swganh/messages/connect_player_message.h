// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_MESSAGES_CONNECT_PLAYER_MESSAGE_H_
#define SWGANH_MESSAGES_CONNECT_PLAYER_MESSAGE_H_

#include <cstdint>
#include "anh/byte_buffer.h"
#include "base_swg_message.h"

namespace swganh {
namespace messages {

    struct ConnectPlayerMessage : public BaseSwgMessage<ConnectPlayerMessage>
    {
    	static uint16_t Opcount() { return 2; }
    	static uint32_t Opcode() { return 0x2E365218; }

    	uint32_t unknown; // always 0

    	ConnectPlayerMessage()
    		: unknown(0)
    	{}

    	void OnSerialize(anh::ByteBuffer& buffer) const
    	{
    		buffer.write(unknown);
    	}

    	void OnDeserialize(anh::ByteBuffer buffer)
    	{
    		unknown = buffer.read<uint32_t>();
    	}
    };

}} // namespace swganh::messages

#endif // SWGANH_MESSAGES_CONNECT_PLAYER_MESSAGE_H_
