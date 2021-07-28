#pragma once
#include "VF_Base.h"

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

		GameContext();
		//~GameContext();
	};
}

#include "MeshManager.h"
#include "ChunkManager.h"
#include "block/block.h"