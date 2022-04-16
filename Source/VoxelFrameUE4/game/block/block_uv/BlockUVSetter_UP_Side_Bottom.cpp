#include "BlockUVSetter_UP_Side_Bottom.h"

void VF::_Block::BlockUVSetter_UP_Side_Bottom::setVertexUVOnDir(TextureManager& texture_man, VFArray<FVector2D>& uvs, _Block::FaceDirection dir)
{
	{
		// printf("child is called\r\n");
		//auto size = mesh.vertices.size();
		switch (dir)
		{
		case _Block::FaceDirection::X_Positive: //侧面
		case _Block::FaceDirection::X_Negative:
		case _Block::FaceDirection::Z_Negative:
		case _Block::FaceDirection::Z_Positive:
			setFaceUVsByTextureIndex(texture_man, uvs, sideIndex);
			//根据index 还有材质的总数，就可以算出材质具体的位置，然后对应顶点进行配置即可
			/* code */
			break;
		case _Block::FaceDirection::Y_Negative: //底面
			setFaceUVsByTextureIndex(texture_man, uvs, bottomIndex);
			break;
		case _Block::FaceDirection::Y_Positive: //上面
			setFaceUVsByTextureIndex(texture_man, uvs, topIndex);
			break;
		}
	}
}
