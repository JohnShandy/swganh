// This file is part of SWGANH which is released under GPL v2.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_MESSAGES_GET_AUCTION_DETAILS_H_
#define SWGANH_MESSAGES_GET_AUCTION_DETAILS_H_

#include <cstdint>
#include "anh/byte_buffer.h"
#include "base_swg_message.h"

namespace swganh {
namespace messages {

    struct GetAuctionDetails : public BaseSwgMessage<GetAuctionDetails>
    {
    	static uint16_t Opcount() { return 2; }
    	static uint32_t Opcode() { return 0xD36EFAE4; }
    	
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

#endif // SWGANH_MESSAGES_GET_AUCTION_DETAILS_H_
