// This file is part of SWGANH which is released under GPL v2.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_MESSAGES_CHAT_REQUEST_PERSISTENT_MESSAGE_H_
#define SWGANH_MESSAGES_CHAT_REQUEST_PERSISTENT_MESSAGE_H_

#include <cstdint>
#include "anh/byte_buffer.h"
#include "base_swg_message.h"

namespace swganh {
namespace messages {

    struct ChatRequestPersistentMessage : public BaseSwgMessage<ChatRequestPersistentMessage>
    {
    	static uint16_t Opcount() { return 3; }
    	static uint32_t Opcode() { return 0x07E3559F; }

    	uint32_t spacer;
    	uint32_t mail_message_id;

    	void OnSerialize(anh::ByteBuffer& buffer) const
    	{
    		buffer.write(spacer);
    		buffer.write(mail_message_id);
    	}

    	void OnDeserialize(anh::ByteBuffer buffer)
    	{
    		spacer = buffer.read<uint32_t>();
    		mail_message_id = buffer.read<uint32_t>();
    	}
    };

}} // namespace swganh::messages

#endif // SWGANH_MESSAGES_CHAT_REQUEST_PERSISTENT_MESSAGE_H_
