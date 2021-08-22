#pragma once
#include "../IManager.h"
#include "Engine/StaticMeshActor.h"
#include  "VF_Base.h"
#include  "../Chunk.h"
namespace VF
{
	namespace _Block
	{
		class BlockPreviewManager :public IManager
		{
		public:
			AStaticMeshActor* blockPutPreviewer;


			std::weak_ptr<Chunk> chunkPutBlockIn;
			Type::Vec3I putBlockChunkPos;

			std::weak_ptr<Chunk> chunkTargetBlockIn;
			Type::Vec3I targetBlockChunkPos;
			bool targeting = false;

			void init(AStaticMeshActor* blockPutPreviewer);

			void setTargetBlockPosition(Type::Vec3I targetBlockChunkPos1, std::weak_ptr<Chunk> chunkTargetBlockIn1);
			void setPutBlockPosition(Type::Vec3I putBlockChunkPos1, std::weak_ptr<Chunk> chunkPutBlockIn1);
			void destroyBlock();

			void putBlock();
			bool checkInited() override;
		};
	}
}
