// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_MESSAGES_GAME_SCENE_CHANGED_H_
#define SWGANH_MESSAGES_GAME_SCENE_CHANGED_H_

#include <cstdint>
#include "anh/byte_buffer.h"
#include "base_swg_message.h"

namespace swganh {
namespace messages {

    struct GameSceneChanged : public BaseSwgMessage<GameSceneChanged>
    {
    	static uint16_t Opcount() { return 1; }
    	static uint32_t Opcode() { return 0xBBADAEB9; }

    	void OnSerialize(anh::ByteBuffer& buffer) const {}

    	void OnDeserialize(anh::ByteBuffer buffer) {}
    };

}} // namespace swganh::messages

#endif // SWGANH_MESSAGES_GAME_SCENE_CHANGED_H_
