#pragma once
#include "CoreMinimal.h"
#include "unordered_map"
#include "memory"
#include "VF_Base.h"

#include "Chunk.h"

#include "GameContext.h"

#include "parallel_hashmap/phmap.h"
namespace VF
{
	class ChunkManager
	{
		GameContext* context;
		/**
		 * 区块位置对应区块智能指针的map
		 */
		phmap::flat_hash_map<ChunkKey, std::shared_ptr<Chunk>> chunkKey2chunksMap;
		/**
		 * 可视区块的一组相对位置
		 */
		std::vector<ChunkKey> inRangeChunksRelativePos;
		/**
		 * 要被绘制的区块
		 */
		std::vector<std::shared_ptr<Chunk>> chunks2Draw;

		Type::Vec3I oldPlayerChunkPos;

		std::list<std::shared_ptr<Chunk>> chunks2Cook;
		FCriticalSection chunkCookMutex;
		//判断区块是否在可视范围内0
		bool isChunkInRange(int x, int y, int z,
			int centerX = 0, int centerY = 0, int centerZ = 0)
		{
			x = x - centerX;
			y = y - centerY;
			z = z - centerZ;
			return (x * x + y * y + z * z < VF_ChunkLoadRadius* VF_ChunkLoadRadius);
		}
		//判断区块是否在可视范围内1
		inline bool isChunkInRange(ChunkKey& ck,
			int centerX = 0, int centerY = 0, int centerZ = 0)
		{
			return isChunkInRange(ck.x(), ck.y(), ck.z(), centerX, centerY, centerZ);
		}

	public:
		ChunkManager();
		void init(GameContext* context1)
		{
			context = context1;
		}
		void constructMeshForChunk(Chunk& chunk);

		void checkPlayerChunkPosChange(const Type::Vec3F& curPlayerPos);

		void cookOneChunk();

		//通过Key获取chunk，若不存在则创建chunk
		std::shared_ptr<Chunk> getChunkOfKey(const ChunkKey& ck);

		void loop();
	};
}


