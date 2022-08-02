//class BlockMesh_Base;
//#ifndef ___BLOCKMESH_BASE_H__
//#define ___BLOCKMESH_BASE_H__
#pragma once

#include "../block.h"
#include "VF_Base.h"
#include "game/Chunk.h"
#include "game/mesh.h"
#include "game/texture_man.h"

namespace VF
{
	namespace _block
	{
		class BlockUVSetter_Base;
		//void conv_block_vector_from_selfpos_2_ue_world_pos(FVector& selfpos, );
		//void conv_block_vector_from_selfpos_2_ue_world_pos(FVector& selfpos, PositionInfoInChunk);
		class BlockMesh_Base
		{
		public:
			/**
			 * 获取r3d collider shape
			*/
			//virtual rp3d::BoxShape* getBlockColliderShape() = 0;

			/**
			 * 获取方块所有的网格三角形(以vertex序列和index序列表示)
			*/
			//virtual void getBlockValidTriangles(std::vector<Type::Vec3F>& vertexPoses, _Graph::Indices& indices) = 0;
			void conv_local_vertex_2_ue_world_pos(PositionInfoInChunk::OfIPos& block_p, VFVec3F& vertex);
			/**
			 * 获取某一面的顶点信息到序列里
			 *  （相对方块自身的坐标
			*/
			virtual void pushOneFaceVerticesAndIndices_selfPos(
				_block::FaceDirection dir,
				MeshConstructData& mesh_construct, PositionInfoInChunk::OfIPos& block_p_in_chunk,
				BlockUVSetter_Base& uv_setter, TextureManager& texture_man) = 0;

			/**************************************
			 * 方块基础信息1,判断某个方向是否有标准类型面
			 * 以便后续有特殊类型方块可以继承和覆盖
			 * ***************************************/
			virtual bool hasStandardFace(_block::FaceDirection dir) = 0;
		};
	}
}

//#endif // ___BLOCKMESH_BASE_H__