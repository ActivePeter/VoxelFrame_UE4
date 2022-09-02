#pragma once
#include "../block.h"
#include "_BlockUVSetter_Base.h"
//////////////////////////////////
namespace VF
{
	namespace _block
	{
		//还可以在实例化的时候自动将自己的材质预先加入到要构建的材质列表中，并且记录自己的材质对应的索引
		// 最后所有block加载完
		// 进行材质合并

		class BlockUVSetter_UP_Side_Bottom : public BlockUVSetter_Base
		{
		private:
			//需要有一个在材质中的位置
			//因为所有方块会被拼到一个材质里
			//index
			int topIndex;
			int sideIndex;
			int bottomIndex;

		public:
			BlockUVSetter_UP_Side_Bottom(int top_index, int side_index, int bottom_index)
				:topIndex(top_index), sideIndex(side_index), bottomIndex(bottom_index) {}
			//BlockUVSetter_UP_Side_Bottom(std::string top, std::string side, std::string bottom)
			//{
			//    topIndex = App::getInstance().graphPtr->_textureManagerPtr->registBlockFaceTexture(top);
			//    sideIndex = App::getInstance().graphPtr->_textureManagerPtr->registBlockFaceTexture(side);
			//    bottomIndex = App::getInstance().graphPtr->_textureManagerPtr->registBlockFaceTexture(bottom);
			//}
			//// void registTexture()
			//// {
			////     auto &textureManager = *(App::getInstance().graphPtr->_textureManagerPtr);
			////     topIndex = textureManager.registBlockFaceTexture("grass_top");
			////     sideIndex = textureManager.registBlockFaceTexture("grass_side");
			////     bottomIndex = textureManager.registBlockFaceTexture("grass_bottom");
			//// }
			////重写父类
			virtual void setVertexUVOnDir(TextureManager& texture_man, VFArray<FVector2D>& uvs, _block::FaceDirection dir) override;
		};
	}
}
