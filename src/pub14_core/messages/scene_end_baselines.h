// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_MESSAGES_SCENE_END_BASELINES_H_
#define SWGANH_MESSAGES_SCENE_END_BASELINES_H_

#include <cstdint>
#include "anh/byte_buffer.h"
#include "base_swg_message.h"

namespace swganh {
namespace messages {
    
    struct SceneEndBaselines : public BaseSwgMessage<SceneEndBaselines>
    {
        static uint16_t Opcount() { return 2; }
        static uint32_t Opcode() { return 0x2C436037; }
        
        uint64_t object_id;
        
        void OnSerialize(anh::ByteBuffer& buffer) const
        {
        	buffer.write(object_id);
        }

        void OnDeserialize(anh::ByteBuffer buffer) 
        {
        	object_id = buffer.read<uint64_t>();
        }
    };

}}  // namespace swganh::messages

#endif  // SWGANH_MESSAGES_SCENE_END_BASELINES_H_
