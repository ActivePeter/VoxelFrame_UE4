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
	}

	class GameContext
	{
	public:
		std::shared_ptr<ChunkManager> chunkManager;
		std::shared_ptr<MeshManager> meshManager;
		std::shared_ptr<_Block::BlockManager> blockManager;
		AWorldActor* worldActor;
		GameContext();
		//~GameContext();
	};
}

#include "MeshManager.h"
#include "ChunkManager.h"
#include "block/block.h"
#include "VoxelFrameUE4/WorldActor.h"