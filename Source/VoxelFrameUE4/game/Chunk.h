// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "VF_Base.h"
#include "parallel_hashmap/phmap.h"

namespace VF
{
	//const int ChunkLoadRange;
	struct ChunkKey
	{
		Type::Vec3I keyData;
		bool operator==(const ChunkKey& o) const
		{
			return keyData == o.keyData;
		}
		friend size_t hash_value(const ChunkKey& p)
		{
			return phmap::HashState().combine(0, p.keyData.X, p.keyData.Y, p.keyData.Z);
		}

		inline int32 x()
		{
			return keyData.X;
		}
		inline int32 y()
		{
			return keyData.Y;
		}
		inline int32 z()
		{
			return keyData.Z;
		}
		ChunkKey(int x, int y, int z)
		{
			keyData = { x,y,z };
		}
		ChunkKey() {}
	};
	struct ChunkData
	{
		ChunkKey chunkKey;
		char dataSet[VF_ChunkSize];
	};

	/**
	 * 区块。地面网格的最小单位，地区数据传输的最小单位
	 */
	struct /*VOXELFRAMEUE4_API*/ Chunk
	{
		ChunkData chunkData;
		//about mesh
		int meshId = -1;

		/**
		 * 顶点
		 */
		TArray<FVector> vertices;

		/**
		 * 顶点索引三角形
		 */
		TArray<int32> triangles;

		/**
		 * 需要构造
		 */
		bool needConstruct = true;
		void unbindMesh()
		{
			meshId = -1;
		}
		inline void getChunkWorldPos(Type::Vec3F& return_pos)
		{
			return_pos.X = chunkData.chunkKey.x() * (VF_ChunkWidth);
			return_pos.Y = chunkData.chunkKey.y() * (VF_ChunkWidth);
			return_pos.Z = chunkData.chunkKey.z() * (VF_ChunkWidth);
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
		Chunk(const ChunkKey& ck);
		//public:
		//Chunk();
		~Chunk();
	};

	namespace PositionInfoInChunk
	{
		template <typename VecType>
		struct Result
		{
			ChunkKey chunkKey;
			VecType p;
		};
		template <typename VecType>
		Result<VecType> fromVfPoint(const VecType& pos)
		{
			Result<VecType> r;

			auto& chunkP = r.chunkKey.keyData;
			{ //1. recalc chunk pos
				if (pos.X >= 0) {
					chunkP.X = (int)pos.X / (VF_ChunkWidth);
				}
				else {
					chunkP.X = ((int)pos.X / (VF_ChunkWidth)) - 1;
				}
				if (pos.Y >= 0) {
					chunkP.Y = (int)pos.Y / (VF_ChunkWidth);
				}
				else {
					chunkP.Y = ((int)pos.Y / (VF_ChunkWidth)) - 1;
				}
				if (pos.Z >= 0) {
					chunkP.Z = (int)pos.Z / (VF_ChunkWidth);
				}
				else {
					chunkP.Z = ((int)pos.Z / (VF_ChunkWidth)) - 1;
				}
			}
			r.p = pos - VecType(chunkP.X * VF_ChunkWidth, chunkP.Y * VF_ChunkWidth, chunkP.Z * VF_ChunkWidth);
			return r;
		}
	}
}