#pragma once
#include "Engine/StaticMeshActor.h"
#include  "VF_Base.h"
#include  "../Chunk.h"

namespace VF
{
	namespace _Block
	{

		class BlockPreviewManager :public IVF_Obj
		{
		public:
			AStaticMeshActor* blockPutPreviewer;


			std::shared_ptr<Chunk> chunkPutBlockIn;
			_type::Vec3I putBlockChunkPos;

			std::shared_ptr<Chunk> chunkTargetBlockIn;
			_type::Vec3I targetBlockChunkPos;
			bool targeting = false;

			// IVF_Obj interface ///////////////////////
			//初始化：override时需要调用父类
			virtual void IVF_Obj_init(ContextId id) override;
			virtual void IVF_Obj_begin() override;
			virtual void IVF_Obj_end() override;
			////////////////////////////////////////////

			//////////////////////////
			void init(ContextId context_id1);

			void setTargetBlockPosition(_type::Vec3I targetBlockChunkPos1, std::shared_ptr<Chunk> chunkTargetBlockIn1);
			void setPutBlockPosition(_type::Vec3I putBlockChunkPos1, std::shared_ptr<Chunk> chunkPutBlockIn1);
			void destroyBlock();

			void putBlock();
		};
	}
}
