// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
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
		}
	}
	//Chunk::Chunk()
	//{
	//}

	Chunk::~Chunk()
	{
	}
}