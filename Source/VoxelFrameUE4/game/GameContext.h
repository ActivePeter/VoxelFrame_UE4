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
		std::shared_ptr<ChunkManager> chunkManager;
		std::shared_ptr<MeshManager> meshManager;
		std::shared_ptr<_Block::BlockManager> blockManager;
		std::shared_ptr<_Block::BlockPreviewManager> blockPreviewManager;
		AWorldActor* worldActor;

		/*
		 *WorldActor ���õĺ�������������Ƿ�һ�о���
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