#pragma once
#include "CoreMinimal.h"
#include "unordered_map"
#include "memory"
#include "VF_Base.h"
#include "Chunk.h"
#include "GameContext.h"
//#include "base/SafeQuene.h"
#include "parallel_hashmap/phmap.h"

namespace VF
{
	class ChunkConstructThread;
	class ChunkManager :public IVF_Obj
	{
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
		std::vector<std::weak_ptr<Chunk>> chunks2Draw;
		phmap::flat_hash_set<ChunkKey> _chunks_2_construct;
		_type::Vec3I oldPlayerChunkPos;



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
		tl::optional<std::shared_ptr<Chunk>&> try_get_chunkptr_by_ck(const ChunkKey& ck)
		{
			auto find = this->chunkKey2chunksMap.find(ck);
			if (find == this->chunkKey2chunksMap.end())
			{
				return tl::nullopt;
			}
			return find->second;
		}
		tl::optional<Chunk&> try_get_chunk_by_ck(const ChunkKey& ck)
		{
			auto find = this->chunkKey2chunksMap.find(ck);
			if (find == this->chunkKey2chunksMap.end())
			{
				return tl::nullopt;
			}
			return *find->second;
		}
		void constructMeshForChunk(Chunk& chunk);
		tl::optional<char> try_read_blockdata_in_chunk(
			_type::Vec3I&& p, Chunk& chunk);
	public:
		//var
		FCriticalSection chunkCookMutex;
		std::list<std::shared_ptr<Chunk>> chunks2Cook;
		//std::unique_ptr<ChunkConstructThread> chunkConstructThread;

		//func
		// IVF_Obj interface ///////////////////////
	public:
		//初始化：override时需要调用父类
		virtual void IVF_Obj_init(ContextId id) override;
		virtual void IVF_Obj_begin() override;
		virtual void IVF_Obj_end() override;


		ChunkManager();
		void constructsome();
		void add_chunk_2_construct(const ChunkKey& ck);
		void asyncConstructMeshForChunk(std::shared_ptr<Chunk>& chunk2Draw);
		//输入须先转换为vf坐标
		void checkPlayerChunkPosChange(const _type::Vec3F& curPlayerPos);
		void cookOneChunk();
		std::shared_ptr<Chunk>  create_chunk(std::string& chunk_data, const ChunkKey& ck);
		//通过Key获取chunk，若不存在则创建chunk
		std::shared_ptr<Chunk> getChunkOfKey(const ChunkKey& ck);
		std::shared_ptr<Chunk> getChunkOfKey_not_sure(const ChunkKey& ck);
		void loop();
	};

	//class ChunkConstructThread :public FRunnable
	//{
	//public:
	//	//  var
	//	SafeQueue<std::weak_ptr<Chunk>> chunks2construct;
	//	ChunkManager* chunkManager;
	//	bool stopping = false;
	//	//  func
	//	ChunkConstructThread(ChunkManager* cm)
	//	{
	//		chunkManager = cm;
	//	}
	//	virtual bool Init() override { return true; }
	//	virtual uint32 Run() override;

	//	virtual void Stop() override
	//	{
	//		stopping = true;
	//	}

	//	virtual void Exit() override { }
	//	~ChunkConstructThread() {}
	//};
}


