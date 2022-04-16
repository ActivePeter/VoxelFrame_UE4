#include "../PackReceiveEvent.h"
#include "game/GameContext.h"
#include "game/Chunk.h"
#include "net/vf_NetworkManager.h"

namespace VF
{
	void pack_recv_handle(std::shared_ptr<PutBlock> pb, GameContext& context)
	{

		{//来自服务端或其他player的方块更新
			auto pi = PositionInfoInChunk::OfIPos::from_vf_pos(
				VFVec3I(pb->x(), pb->y(), pb->z()));
			auto chunk = context.chunkManager->getChunkOfKey(pi.chunkKey);
			if (chunk)
			{
				chunk->setData(1,
					pi.pos.X,
					pi.pos.Y,
					pi.pos.Z);

				chunk->call_after_edit();
				context.chunkManager->asyncConstructMeshForChunk(chunk);
			}
			else
			{
				VF_LogWarning("pl chunk of block not exist");
			}
		}
	}
	void VF::pack_recv_handle(std::shared_ptr<Cmd_PutBlockInPs> cmd, GameContext& context)
	{
		auto& pb = cmd->put_block();
		auto pi = PositionInfoInChunk::OfIPos::from_vf_pos(
			VFVec3I(pb.x(), pb.y(), pb.z()));
		auto chunk = context.chunkManager->getChunkOfKey(pi.chunkKey);
		if (chunk)
		{
			chunk->setData(pb.block_id(),
				pi.pos.X,
				pi.pos.Y,
				pi.pos.Z);

			chunk->call_after_edit();
			context.chunkManager->asyncConstructMeshForChunk(chunk);


			//send back
			auto rpl = std::make_shared<Rpl_PutBlockInPs>();
			rpl->set_task_id(cmd->task_id());
			rpl->set_put_succ(1);

			//rpl->set_allocated_put_block(cmd->mutable_put_block());
			PackContainer pc(PackIds::ERpl_PutBlockInPs, rpl);
			context.networkManager->send(std::move(pc));
			//context.networkManager->send();
		}
		else
		{
			VF_LogWarning("ps chunk of block not exist");
		}
	}
}
