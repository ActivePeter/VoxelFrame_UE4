#include "_BlockUVSetter_Base.h"
namespace VF
{
	namespace _block
	{
		//对应基础类型方块的网格设置uv
		void BlockUVSetter_Base::setFaceUVsByTextureIndex(TextureManager& texture_man, VFArray<FVector2D>& uvs, int textureIndex)
		{
			auto rect = texture_man.get_uv_rect_of_block_face_index(textureIndex);
			uvs.Push(FVector2D(rect.x1, rect.y1));
			uvs.Push(FVector2D(rect.x1, rect.get_y2()));
			uvs.Push(FVector2D(rect.get_x2(), rect.get_y2()));
			uvs.Push(FVector2D(rect.get_x2(), rect.y1));

			//uvs.Push(FVector2D(0, 0));
			//uvs.Push(FVector2D(0, 0.5));
			//uvs.Push(FVector2D(0.5, 0.5));
			//uvs.Push(FVector2D(0.5, 0));
		}
	}
}