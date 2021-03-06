// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_MESSAGES_ADD_ITEM_FAILED_MESSAGE_H_
#define SWGANH_MESSAGES_ADD_ITEM_FAILED_MESSAGE_H_

#include <cstdint>
#include "anh/byte_buffer.h"
#include "base_swg_message.h"

namespace swganh {
namespace messages {

    struct AddItemFailedMessage : public BaseSwgMessage<AddItemFailedMessage>
    {
    	static uint16_t Opcount() { return 1; }
    	static uint32_t Opcode() { return 0x69D3E1D2; }

    	uint64_t item_id;

    	void OnSerialize(anh::ByteBuffer& buffer) const
    	{
    		buffer.write(item_id);
    	}

    	void OnDeserialize(anh::ByteBuffer buffer)
    	{
    		item_id = buffer.read<uint64_t>();
    	}
    };

}} // namespace swganh::messages

#endif // SWGANH_MESSAGES_ADD_ITEM_FAILED_MESSAGE_H_