#include "BlockPreviewManager.h"
#include "../GameContext.h"
namespace VF
{
	namespace _Block
	{
		bool BlockPreviewManager::checkInited()
		{
			return true;
		}
		void BlockPreviewManager::init(AStaticMeshActor* blockPutPreviewer1)
		{
			this->blockPutPreviewer = blockPutPreviewer1;
		}

		void BlockPreviewManager::setTargetBlockPosition(Type::Vec3I targetBlockChunkPos1, std::weak_ptr<Chunk> chunkTargetBlockIn1)
		{
			this->targetBlockChunkPos = targetBlockChunkPos1;

			this->chunkTargetBlockIn = chunkTargetBlockIn1;
			targeting = true;
		}

		void BlockPreviewManager::setPutBlockPosition(Type::Vec3I putBlockChunkPos1,
			std::weak_ptr<Chunk> chunkPutBlockIn1)
		{
			this->putBlockChunkPos = putBlockChunkPos1;
			this->chunkPutBlockIn = chunkPutBlockIn1;

			auto posOfChunkPutBlockIn = chunkPutBlockIn1.lock()->chunkData.chunkKey.keyData;
			auto putBlockWorldPosUE = Type::Vec3F(
				(putBlockChunkPos.X + posOfChunkPutBlockIn.X * VF_ChunkWidth) * VF_WorldScale + VF_WorldScale / 2,
				(putBlockChunkPos.Z + posOfChunkPutBlockIn.Z * VF_ChunkWidth) * VF_WorldScale + VF_WorldScale / 2,
				(putBlockChunkPos.Y + posOfChunkPutBlockIn.Y * VF_ChunkWidth) * VF_WorldScale + VF_WorldScale / 2);
			this->blockPutPreviewer->SetActorLocation(putBlockWorldPosUE);
		}
		void BlockPreviewManager::destroyBlock()
		{
			if (targeting)
			{

				//auto positionInChunk = PositionInfoInChunk::fromVfPoint(blockWorldPos);
				chunkPutBlockIn.lock()->setData(0,
					targetBlockChunkPos.X,
					targetBlockChunkPos.Y,
					targetBlockChunkPos.Z);

				chunkTargetBlockIn.lock()->needConstruct = true;
				GameContext::getContext()->chunkManager->asyncConstructMeshForChunk(chunkTargetBlockIn);
			}

		}
		void BlockPreviewManager::putBlock()
		{
			if (!blockPutPreviewer->IsHidden())
			{
				//auto positionInChunk = PositionInfoInChunk::fromVfPoint(blockWorldPos);
				chunkPutBlockIn.lock()->setData(1,
					putBlockChunkPos.X,
					putBlockChunkPos.Y,
					putBlockChunkPos.Z);

				chunkPutBlockIn.lock()->needConstruct = true;
				GameContext::getContext()->chunkManager->asyncConstructMeshForChunk(chunkPutBlockIn);
			}
		}
	}
}