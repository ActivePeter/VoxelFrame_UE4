#pragma once
#include "VF_Base.h"
#include "_BlockMesh_Base.h"
namespace VF
{
	namespace _Block
	{
		class BlockMesh_Common : public BlockMesh_Base
		{
			friend class Manager;

			/* BlockMesh_Base *******************************************************/
			//void getBlockValidTriangles(std::vector<_type::Vec3F>& vertexPoses, _Graph::Indices& indices) override;

			void pushOneFaceVerticesAndIndices_selfPos(
				_Block::FaceDirection dir,
				MeshConstructData& mesh_construct, PositionInfoInChunk::OfIPos& block_p_in_chunk,
				BlockUVSetter_Base& uv_setter, TextureManager& texture_man) override;

			bool hasStandardFace(_Block::FaceDirection dir) override
			{
				return true;
			}

			/**************************************************************************/
		protected:
			/**
 * 根据四个序号 初始化（获取到vertices里）一面的顶点坐标
*/
			void setFaceVertices(PositionInfoInChunk::OfIPos& blockp, VFArray<_type::Vec3F>& vertexPoses, uint8_t _0, uint8_t _1, uint8_t _2, uint8_t _3);

			/* data */
			// void setFaceUVsByTextureIndex(
			//     Mesh &mesh, int textureIndex);

			/**************************************
			 * 方块基础信息3
			 * 这个函数包含方块 顶点相对坐标 和 对应的index
			 * 参考markdown文件夹里的 方块顶点顺序.md
			 * ***************************************/
			void addUpStandardVertexOfIndex(PositionInfoInChunk::OfIPos& blockp, _type::Vec3F& vertexPos, uint8_t index);

		public:
			/**
			 * 空的Info的构造函数
			*/
			BlockMesh_Common() {}

			/**************************************
			 * 方块基础信息2
			 * 这个函数包含方块每个面的顶点坐标顺序
			 * 以便后续有特殊类型方块可以继承和覆盖
			 * ***************************************/
			 //void setFaceVertexPosOnDir(VFArray<_type::Vec3F>& vertexPoses, _Block::FaceDirection dir);
			 // virtual void setVertexUVOnDir(_Block::FaceDirection dir, Mesh &mesh) {}


		};
	}
}
