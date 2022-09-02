#include "../PackReceiveEvent.h"
#include "game/Chunk.h"


namespace VF
{
	void pack_recv_handle(std::shared_ptr<ChunkPack> chunk_pack, GameContext& context)
	{
		auto& cp = chunk_pack;

		auto new_chunk = context.chunkManager->create_chunk(
			*cp->mutable_data(),
			ChunkKey(cp->x(), cp->y(), cp->z())
		);
		//std::string hh;
		//cp->mutable_data()->swap(hh);

		//ck.chunkData.dataSet
		/*VF_Log("pack_recv ChunkPack %d %d %d, %d %d",
			cp->x(), cp->y(), cp->z(), cp->mutable_data()->size(), new_chunk->chunkData.dataSet.size()
		);*/
	}
}
