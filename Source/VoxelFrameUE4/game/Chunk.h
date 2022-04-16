// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "VF_Base.h"
#include "parallel_hashmap/phmap.h"
#include "mesh.h"

namespace VF
{
	//const int ChunkLoadRange;
	struct ChunkKey
	{
		_type::Vec3I keyData;
		bool operator==(const ChunkKey& o) const
		{
			return keyData == o.keyData;
		}
		friend size_t hash_value(const ChunkKey& p)
		{
			return phmap::HashState().combine(0, p.keyData.X, p.keyData.Y, p.keyData.Z);
		}

		inline int32 x() const
		{
			return keyData.X;
		}
		inline int32 y() const
		{
			return keyData.Y;
		}
		inline int32 z() const
		{
			return keyData.Z;
		}
		ChunkKey(int x, int y, int z)
		{
			keyData = { x,y,z };
		}
		ChunkKey() {}

		static ChunkKey from_ue_point(VFVec3F ue_p);
		static ChunkKey from_vf_point(VFVec3F vf_p);
	};
	struct ChunkData
	{
		ChunkKey chunkKey;
		//char dataSet[VF_ChunkSize];
		std::string dataSet;
	};

	/**
	 * 区块。地面网格的最小单位，地区数据传输的最小单位
	 */
	struct /*VOXELFRAMEUE4_API*/ Chunk
	{
		ChunkData chunkData;
		//about mesh
		int meshId = -1;

		MeshConstructData mesh_construct_data;

		void unbindMesh()
		{
			meshId = -1;
		}
		inline void getChunkWorldPos(_type::Vec3F& return_pos)
		{
			return_pos.X = chunkData.chunkKey.x() * (VF_ChunkWidth);
			return_pos.Y = chunkData.chunkKey.y() * (VF_ChunkWidth);
			return_pos.Z = chunkData.chunkKey.z() * (VF_ChunkWidth);
		}
		inline void setData(int data1, int x, int y, int z)
		{
			chunkData.dataSet[x + y * VF_ChunkWidth + z * VF_ChunkWidth * VF_ChunkWidth] = data1;
		}
		inline char readData(int x, int y, int z)
		{
			assert(x + y * VF_ChunkWidth + z * VF_ChunkWidth * VF_ChunkWidth < VF_ChunkSize);
			return chunkData.dataSet[x + y * VF_ChunkWidth + z * VF_ChunkWidth * VF_ChunkWidth];
		}
		inline char readData(int index)
		{
			assert(index < VF_ChunkSize);
			return chunkData.dataSet[index];
		}
		inline void call_after_edit()
		{
			mesh_construct_data.needConstruct = true;
		}
		Chunk(const ChunkKey& ck, std::string& chunk_data);
		Chunk(const ChunkKey& ck) {}
		Chunk() {}
		//public:
		//Chunk();
		~Chunk();
	};

	namespace PositionInfoInChunk
	{
		//template <typename VecType>
		struct OfIPos
		{
			ChunkKey chunkKey;
			VFVec3I pos;//in chunk pos
			OfIPos(ChunkKey& ck1, VFVec3I pos1) :chunkKey(ck1), pos(pos1) {}
			static OfIPos from_vf_pos(const VFVec3I& pos);
		};

		struct OfFPos
		{
			ChunkKey chunkKey;
			VFVec3F pos;
			OfFPos(ChunkKey& ck1, VFVec3F pos1) :chunkKey(ck1), pos(pos1) {}
			static OfFPos from_vf_pos(const VFVec3F& pos);
		};
		//template <typename VecType>
		//Type<VecType> fromVfPoint(const VecType& pos)
		//{
		//	Result<VecType> r;

		//	auto& chunkP = r.chunkKey.keyData;
		//	{ //1. recalc chunk pos
		//		if (pos.X >= 0) {
		//			chunkP.X = (int)pos.X / (VF_ChunkWidth);
		//		}
		//		else {
		//			chunkP.X = ((int)pos.X / (VF_ChunkWidth)) - 1;
		//		}
		//		if (pos.Y >= 0) {
		//			chunkP.Y = (int)pos.Y / (VF_ChunkWidth);
		//		}
		//		else {
		//			chunkP.Y = ((int)pos.Y / (VF_ChunkWidth)) - 1;
		//		}
		//		if (pos.Z >= 0) {
		//			chunkP.Z = (int)pos.Z / (VF_ChunkWidth);
		//		}
		//		else {
		//			chunkP.Z = ((int)pos.Z / (VF_ChunkWidth)) - 1;
		//		}
		//	}
		//	r.p = pos - VecType(chunkP.X * VF_ChunkWidth, chunkP.Y * VF_ChunkWidth, chunkP.Z * VF_ChunkWidth);
		//	return r;
		//}
	}
}