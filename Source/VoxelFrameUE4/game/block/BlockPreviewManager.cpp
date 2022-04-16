#include "BlockPreviewManager.h"
#include "../GameContext.h"
#include "Kismet/GameplayStatics.h"
#include "net/vf_NetworkManager.h"
#include "game/operation_rec/chunk_block.h"


namespace VF
{
	namespace _Block
	{
		void BlockPreviewManager::IVF_Obj_init(ContextId ID)
		{
			IVF_Obj::IVF_Obj_init(ID);
			VF_LogWarning("BlockPreviewManager cid %d %d", ID.id, context_id.id);
		}

		void BlockPreviewManager::IVF_Obj_begin()
		{
			VF_LogWarning("BlockPreviewManager begin cid %d", context_id.id);
			auto w = this->context_id.get()->worldActor;
			TArray<AActor*> OutActors;
			UGameplayStatics::GetAllActorsOfClass(w->GetWorld(), AStaticMeshActor::StaticClass(), OutActors);
			check(OutActors.Num() > 0);
			this->blockPutPreviewer = Cast<AStaticMeshActor>(OutActors[0]);
		}

		void BlockPreviewManager::IVF_Obj_end()
		{
		}

		void BlockPreviewManager::setTargetBlockPosition(VFVec3I targetBlockChunkPos1, std::shared_ptr<Chunk> chunkTargetBlockIn1)
		{
			this->targetBlockChunkPos = targetBlockChunkPos1;

			this->chunkTargetBlockIn = chunkTargetBlockIn1;
			targeting = true;
		}

		void BlockPreviewManager::setPutBlockPosition(VFVec3I putBlockChunkPos1,
			std::shared_ptr<Chunk> chunkPutBlockIn1)
		{
			this->putBlockChunkPos = putBlockChunkPos1;
			this->chunkPutBlockIn = chunkPutBlockIn1;

			auto posOfChunkPutBlockIn = chunkPutBlockIn1->chunkData.chunkKey.keyData;
			auto putBlockWorldPosUE = VFVec3F(
				(putBlockChunkPos.X + posOfChunkPutBlockIn.X * VF_ChunkWidth) * VF_WorldScale + VF_WorldScale / 2,
				(putBlockChunkPos.Z + posOfChunkPutBlockIn.Z * VF_ChunkWidth) * VF_WorldScale + VF_WorldScale / 2,
				(putBlockChunkPos.Y + posOfChunkPutBlockIn.Y * VF_ChunkWidth) * VF_WorldScale + VF_WorldScale / 2);
			this->blockPutPreviewer->SetActorLocation(putBlockWorldPosUE);
		}

		void BlockPreviewManager::destroyBlock()
		{
			if (targeting)
			{
				_Block::change_one_block_and_send_2_server(
					*this->get_context(),
					*this->chunkTargetBlockIn,
					this->targetBlockChunkPos,
					0
				);
			}

		}
		void BlockPreviewManager::putBlock()
		{
			if (!blockPutPreviewer->IsHidden())
			{
				_Block::change_one_block_and_send_2_server(
					*this->get_context(),
					*this->chunkPutBlockIn,
					this->putBlockChunkPos,
					1
				);

				////auto positionInChunk = PositionInfoInChunk::fromVfPoint(blockWorldPos);
				////chunkPutBlockIn->setData(1,
				////	putBlockChunkPos.X,
				////	putBlockChunkPos.Y,
				////	putBlockChunkPos.Z);
				////chunkTargetBlockIn->call_after_edit();
				////GameContext::get_context(context_id)->chunkManager->asyncConstructMeshForChunk(chunkPutBlockIn);
				//
				//
				//
				//
				////sync to net
				//auto pb = std::make_shared<PutBlock>();
				//auto block_world_pos = chunkPutBlockIn->chunkData.chunkKey.keyData * VF_ChunkSize
				//	+ putBlockChunkPos;
				//
				//auto new_op = this->get_context()->operation_rec->
				//	new_operation<_operation_rec::PutBlockOperation>(
				//		block_world_pos.X,
				//		block_world_pos.Y,
				//		block_world_pos.Z,
				//		1
				//		);
				//new_op.ref.act(*this->get_context());
				//pb->set_block_id(1);
				//pb->set_x(block_world_pos.X);
				//pb->set_y(block_world_pos.Y);
				//pb->set_z(block_world_pos.Z);
				//pb->set_operation_id(new_op.id);
				//PackContainer pc(PackIds::EPutBlock, pb);
				//get_context()->networkManager->send(std::move(pc));
			}
		}
	}
}
