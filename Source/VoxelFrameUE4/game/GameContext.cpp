#include "GameContext.h"

namespace VF
{
	GameContext::GameContext()
	{
		chunkManager = std::make_shared<ChunkManager>();
		chunkManager->init(this);

		meshManager = std::make_shared<MeshManager>();
		meshManager->init(this);

		blockManager = std::make_shared<_Block::BlockManager>();
		blockManager->registAll();
	}
}