// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
namespace VF
{
	namespace _Chunk
	{
		Type::Vec3I getChunkPositionOfAPoint(const Type::Vec3F pos)
		{
			Type::Vec3I chunkP;
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
			return chunkP;
		}
	}
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
	Chunk::Chunk(const ChunkKey& ck)
	{
		chunkData.chunkKey = ck;
		for (int x = 0; x < VF_ChunkWidth; x++)
		{
			for (int y = 0; y < VF_ChunkWidth; y++)
			{
				for (int z = 0; z < VF_ChunkWidth; z++)
				{
					if (y > VF_ChunkWidth / 2)
					{
						if (x == 8 && z == 8)
						{
							chunkData.dataSet[inChunk_blockPos2blockIndex(x, y, z)] = 1;
						}
						else
						{
							chunkData.dataSet[inChunk_blockPos2blockIndex(x, y, z)] = 0;
						}
					}
					else
					{
						chunkData.dataSet[inChunk_blockPos2blockIndex(x, y, z)] = 1;
					}
				}
			}
		}
	}
	//Chunk::Chunk()
	//{
	//}

	Chunk::~Chunk()
	{
	}
}