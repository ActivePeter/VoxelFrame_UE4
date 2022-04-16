namespace VF
{
	namespace _Block
	{
		class BlockUVSetter_Base;
	}
}
#pragma once
// #include "../enum.h"
#include "../block.h"
#include "game/texture_man.h"
/////////////////////////////////////////
namespace VF
{
	namespace _Block
	{
		class BlockUVSetter_Base
		{
		public:
			void setFaceUVsByTextureIndex(TextureManager& texture_man, VFArray<FVector2D>& uvs, int textureIndex);
			virtual void setVertexUVOnDir(TextureManager& texture_man, VFArray<FVector2D>& uvs, _Block::FaceDirection dir) = 0;
		};
	}
}
