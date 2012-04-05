// This file is part of SWGANH which is released under GPL v2.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_MESSAGES_CLIENT_PERMISSIONS_MESSAGE_H_
#define SWGANH_MESSAGES_CLIENT_PERMISSIONS_MESSAGE_H_

#include <cstdint>
#include "anh/byte_buffer.h"
#include "base_swg_message.h"

namespace swganh {
namespace messages {
    
    struct ClientPermissionsMessage : public BaseSwgMessage<ClientPermissionsMessage>
    {
        static uint16_t Opcount() { return 4; }
        static uint32_t Opcode() { return 0xE00730E5; }
        
        uint8_t galaxy_available;
        uint8_t available_character_slots;
        uint8_t unlimited_characters;
        
        void OnSerialize(anh::ByteBuffer& buffer) const
        {
        	buffer.write(galaxy_available);
        	buffer.write(available_character_slots);
        	buffer.write(unlimited_characters);
        }

        void OnDeserialize(anh::ByteBuffer buffer)
        {
        	galaxy_available = buffer.read<uint8_t>();
        	available_character_slots = buffer.read<uint8_t>();
        	unlimited_characters = buffer.read<uint8_t>();
        }
    };

}}  // namespace swganh::messages

#endif  // SWGANH_MESSAGES_CLIENT_PERMISSIONS_MESSAGE_H_
