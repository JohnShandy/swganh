// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_MESSAGES_CHAT_ON_SEND_INSTANT_MESSAGE_H_
#define SWGANH_MESSAGES_CHAT_ON_SEND_INSTANT_MESSAGE_H_

#include <cstdint>
#include "anh/byte_buffer.h"
#include "base_swg_message.h"

namespace swganh {
namespace messages {

    struct ChatOnSendInstantMessage : public BaseSwgMessage<ChatOnSendInstantMessage>
    {
    	static uint16_t Opcount() { return 3; }
    	static uint32_t Opcode() { return 0x88DBB381; }

    	uint32_t success_flag; // 0x00 = success, 0x04 = failure
    	uint32_t sequence_number;

    	void OnSerialize(anh::ByteBuffer& buffer) const
    	{
    		buffer.write(success_flag);
    		buffer.write(sequence_number);
    	}

    	void OnDeserialize(anh::ByteBuffer buffer)
    	{
    		success_flag = buffer.read<uint32_t>();
    		sequence_number = buffer.read<uint32_t>();
    	}
    };

}} // namespace swganh::messages

#endif // SWGANH_MESSAGES_CHAT_ON_SEND_INSTANT_MESSAGE_H_
