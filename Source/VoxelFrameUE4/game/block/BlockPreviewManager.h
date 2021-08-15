#pragma once
#include "../IManager.h"
#include "Engine/StaticMeshActor.h"

namespace VF
{
	namespace _Block
	{
		class BlockPreviewManager :public IManager
		{
		public:
			AStaticMeshActor* blockPutPreviewer;
			void init(AStaticMeshActor* blockPutPreviewer);
			bool checkInited() override;
		};
	}
}
