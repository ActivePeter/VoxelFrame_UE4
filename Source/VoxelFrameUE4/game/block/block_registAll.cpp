#include "block.h"
#include "block_mesh/BlockMesh_Common.h"
#include "block_uv/BlockUVSetter_UP_Side_Bottom.h"

namespace VF
{
	namespace _Block
	{
		void BlockManager::registAll()
		{
			this->addEmptyBlock(BlockTypeInfo());

			this->addBlock(BlockTypeInfo(
				std::make_shared<BlockUVSetter_UP_Side_Bottom>(),
				std::make_shared<BlockMesh_Common>()));
		}
	}
}