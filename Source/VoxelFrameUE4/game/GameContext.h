#pragma once


#include "VF_Base.h"
class AWorldActor;
namespace VF
{
	class ChunkManager;
	struct MeshManager;

	namespace _Block
	{
		class BlockManager;
		class BlockPreviewManager;
	}

	class GameContext
	{

	public:
		static GameContext* getContext();
		std::unique_ptr<ChunkManager> chunkManager;
		std::unique_ptr<MeshManager> meshManager;
		std::unique_ptr<_Block::BlockManager> blockManager;
		std::unique_ptr<_Block::BlockPreviewManager> blockPreviewManager;
		AWorldActor* worldActor;

		/*
		 *WorldActor 调用的函数，用来检查是否一切就绪
		 ***/
		void worldActorCallWhenBeginPlay();

		GameContext();
		~GameContext();
	};
}

#include "MeshManager.h"
#include "ChunkManager.h"
#include "block/block.h"
#include "VoxelFrameUE4/WorldActor.h"
#include "block/BlockPreviewManager.h"