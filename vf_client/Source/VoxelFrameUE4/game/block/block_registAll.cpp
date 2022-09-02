#include "block.h"
#include "block_mesh/BlockMesh_Common.h"
#include "block_uv/BlockUVSetter_UP_Side_Bottom.h"
#include "game/GameContext.h"

namespace VF
{
	namespace _block
	{
		//defination templete
		// IVF_Obj /////////////////////////
		void BlockManager::IVF_Obj_init(ContextId id) {
			IVF_Obj::IVF_Obj_init(id);
		}
		void BlockManager::IVF_Obj_begin() {}
		void BlockManager::IVF_Obj_end() {}
		////////////////////////////////////

		void BlockManager::after_block_texture_loaded()
		{
			auto& texture_man = get_context()->taxture_man;
			//0.空气方块
			this->addEmptyBlock(BlockTypeInfo());
			//1.草方块
			{
				auto up = texture_man->get_index_of_block_face_name("Grass_Up");
				auto side = texture_man->get_index_of_block_face_name("Grass_Side");
				auto bottom = texture_man->get_index_of_block_face_name("Grass_Bottom");
				VF_LogWarning("dirt faces %d %d %d", up, side, bottom);
				if (up != -1 && side != -1 && bottom != -1)
				{
					this->addBlock(BlockTypeInfo(
						std::make_shared<BlockUVSetter_UP_Side_Bottom>(up, side, bottom),
						std::make_shared<BlockMesh_Common>()));
				}
				auto up_rect = texture_man->get_uv_rect_of_block_face_index(up);
				//auto side_rect = texture_man->get_uv_rect_of_block_face_index(side);


				VF_LogWarning("rect %f %f %f %f", up_rect.x1, up_rect.y1, up_rect.get_x2(), up_rect.get_y2());
			}
			//2.rock方块
			{
				auto up = texture_man->get_index_of_block_face_name("Rock");
				auto side = texture_man->get_index_of_block_face_name("Rock");
				auto bottom = texture_man->get_index_of_block_face_name("Rock");
				VF_LogWarning("rock faces %d %d %d", up, side, bottom);

				if (up != -1 && side != -1 && bottom != -1)
				{
					this->addBlock(BlockTypeInfo(
						std::make_shared<BlockUVSetter_UP_Side_Bottom>(up, side, bottom),
						std::make_shared<BlockMesh_Common>()));
				}
				auto up_rect = texture_man->get_uv_rect_of_block_face_index(up);
				//auto side_rect = texture_man->get_uv_rect_of_block_face_index(side);


				VF_LogWarning("rect %f %f %f %f", up_rect.x1, up_rect.y1, up_rect.get_x2(), up_rect.get_y2());
			}
			//3.水方块
			{
				auto up = texture_man->get_index_of_block_face_name("Water");
				auto side = texture_man->get_index_of_block_face_name("Water");
				auto bottom = texture_man->get_index_of_block_face_name("Water");
				VF_LogWarning("water faces %d %d %d", up, side, bottom);

				if (up != -1 && side != -1 && bottom != -1)
				{
					this->addBlock(BlockTypeInfo(
						std::make_shared<BlockUVSetter_UP_Side_Bottom>(up, side, bottom),
						std::make_shared<BlockMesh_Common>()));
				}
				auto up_rect = texture_man->get_uv_rect_of_block_face_index(up);
				//auto side_rect = texture_man->get_uv_rect_of_block_face_index(side);


				VF_LogWarning("rect %f %f %f %f", up_rect.x1, up_rect.y1, up_rect.get_x2(), up_rect.get_y2());
			}
		}
	}
}
