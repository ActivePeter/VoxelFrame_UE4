// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
#include "game/conv.h"

namespace VF
{
	static inline int inChunk_blockPos2blockIndex(int x, int y, int z)
	{
		return x + y * VF_ChunkWidth + z * VF_ChunkWidth * VF_ChunkWidth;
	}
	static inline void  inChunk_blockIndex2blockPos(int index, int& returnX, int& returnY, int& returnZ)
	{
		returnX = index % VF_ChunkWidth;
		returnY = (index / VF_ChunkWidth) % VF_ChunkWidth;
		returnZ = index / VF_ChunkWidth / VF_ChunkWidth;
	}

	ChunkKey ChunkKey::from_ue_point(VFVec3F ue_p)
	{
		return from_vf_point(ue_p / VF_WorldScale);
	}

	ChunkKey ChunkKey::from_vf_point(VFVec3F vf_p)
	{
		auto ck = ChunkKey(0, 0, 0);
		// println!("plen  {0}", p.len());
		ck.keyData.X = vf_p.X / VF_ChunkWidth;
		ck.keyData.Y = vf_p.Y / VF_ChunkWidth;
		ck.keyData.Z = vf_p.Z / VF_ChunkWidth;

		if (vf_p.X < 0.0) {
			ck.keyData.X = ck.x() - 1;
		}
		if (vf_p.Y < 0.0) {
			ck.keyData.Y = ck.y() - 1;
		}
		if (vf_p.Z < 0.0) {
			ck.keyData.Z = ck.z() - 1;
		}
		return ck;
	}


	Chunk::Chunk(const ChunkKey& ck, std::string& chunk_data)
	{
		chunkData.dataSet.swap(chunk_data);
		chunkData.dataSet.resize(VF_ChunkSize);
		chunkData.chunkKey = ck;
		/*for (int x = 0; x < VF_ChunkWidth; x++)
		{
			for (int y = 0; y < VF_ChunkWidth; y++)
			{
				for (int z = 0; z < VF_ChunkWidth; z++)
				{
					if (y > VF_ChunkWidth / 2)
					{
						if (x == 8 && z == 8)
						{
							chunkData.dataSet[x + y * VF_ChunkWidth + z * VF_ChunkWidth * VF_ChunkWidth] = 1;
						}
						else
						{
							chunkData.dataSet[x + y * VF_ChunkWidth + z * VF_ChunkWidth * VF_ChunkWidth] = 0;
						}
					}
					else
					{
						chunkData.dataSet[x + y * VF_ChunkWidth + z * VF_ChunkWidth * VF_ChunkWidth] = 1;
					}
				}
			}
		}*/
	}
	//Chunk::Chunk()
	//{
	//}

	Chunk::~Chunk()
	{
	}

	namespace PositionInfoInChunk
	{
		OfIPos OfIPos::from_vf_pos(const VFVec3I& pos)
		{
			auto ck = _conv::point_2_chunkkey(pos.X, pos.Y, pos.Z);
			return OfIPos(ck, pos - _conv::origin_point_i_of_chunk(ck));
		}

		OfFPos OfFPos::from_vf_pos(const VFVec3F& pos)
		{
			auto ck = _conv::point_2_chunkkey(pos.X, pos.Y, pos.Z);
			return OfFPos(ck, pos - _conv::origin_point_f_of_chunk(ck));
		}
	}
}
