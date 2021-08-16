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

		void BlockPreviewManager::setPosition(Type::Vec3I blockWorldPos1, std::weak_ptr<Chunk> chunkBlockIn1)
		{
			blockWorldPos = blockWorldPos1;
			this->blockPutPreviewer->SetActorLocation(Type::Vec3F(
				blockWorldPos.X * VF_CommonBlockSize + VF_CommonBlockSize / 2,
				blockWorldPos.Z * VF_CommonBlockSize + VF_CommonBlockSize / 2,
				blockWorldPos.Y * VF_CommonBlockSize + VF_CommonBlockSize / 2));
			chunkBlockIn = chunkBlockIn1;
		}
		void BlockPreviewManager::putBlock()
		{
			if (!blockPutPreviewer->IsHidden())
			{
				auto positionInChunk = PositionInfoInChunk::fromVfPoint(blockWorldPos);
				chunkBlockIn.lock()->setData(1,
					positionInChunk.p.X,
					positionInChunk.p.Y,
					positionInChunk.p.Z);

				chunkBlockIn.lock()->needConstruct = true;
				GameContext::getContext()->chunkManager->asyncConstructMeshForChunk(chunkBlockIn);
			}
		}
	}
}