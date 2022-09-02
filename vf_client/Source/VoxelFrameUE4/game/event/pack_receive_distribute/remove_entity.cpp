#include "../PackReceiveEvent.h"
#include "game/GameContext.h"
#include "game/Chunk.h"
#include "net/vf_NetworkManager.h"

namespace VF
{
	void pack_recv_handle(std::shared_ptr<RemoveEntity> pack, GameContext& context)
	{
		context.entity_manager->remove_enity(true, pack->entity_id());
	}
}
