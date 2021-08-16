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
			Type::Vec3I blockWorldPos;
			std::weak_ptr<Chunk> chunkBlockIn;
			void init(AStaticMeshActor* blockPutPreviewer);
			void setPosition(Type::Vec3I blockWorldPos, std::weak_ptr<Chunk> chunkBlockIn1);
			void putBlock();
			bool checkInited() override;
		};
	}
}
