#include "BlockPreviewManager.h"

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
	}
}