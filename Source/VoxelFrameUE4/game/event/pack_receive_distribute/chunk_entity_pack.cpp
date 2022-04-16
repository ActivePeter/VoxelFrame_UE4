#include "../PackReceiveEvent.h"
#include "game/Chunk.h"

namespace VF
{
	void pack_recv_handle(std::shared_ptr<ChunkEntityPack> chunk_entity_pack, GameContext& context)
	{
		//VF_Log("pack_recv ChunkEntityPack");
		auto& entities = chunk_entity_pack->entity_pos();
		if (entities.size() > 0)
		{
			context.entity_manager->born_entities_in_chunk(
				ChunkKey(chunk_entity_pack->x(), chunk_entity_pack->y(), chunk_entity_pack->z()),
				(*chunk_entity_pack).entity_pos());
		}
	}
}
