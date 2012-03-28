/*
 This file is part of SWGANH. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2011 The SWG:ANH Team

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SWGANH_MESSAGES_CHAT_DELETE_PERSISTENT_MESSAGE_H_
#define SWGANH_MESSAGES_CHAT_DELETE_PERSISTENT_MESSAGE_H_

#include <cstdint>
#include "anh/byte_buffer.h"
#include "swganh/messages/base_swg_message.h"

namespace swganh {
namespace messages {

struct ChatDeletePersistentMessage : public swganh::messages::BaseSwgMessage<ChatDeletePersistentMessage> {
	static uint16_t opcount() { return 2; }
	static uint32_t opcode() { return 0x8F251641; }

	uint32_t mail_message_id;

	void onSerialize(anh::ByteBuffer& buffer) const {
		buffer.write(mail_message_id);
	}

	void onDeserialize(anh::ByteBuffer buffer) {
		mail_message_id = buffer.read<uint32_t>();
	}
};

}} // namespace swganh::messages

#endif // SWGANH_MESSAGES_CHAT_DELETE_PERSISTENT_MESSAGE_H_