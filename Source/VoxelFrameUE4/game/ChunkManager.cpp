
#include "ChunkManager.h"

namespace VF
{
	namespace _chunkman_ecs_sys
	{
		void _sys_common_update(ContextId& cid);
		void regist_all(GameContext& ctx, ChunkManager& cman)
		{
			auto& ecs = ctx.ecs;
			ecs.scene->addSys2Group(
				ecs.commonUpdateSys, _sys_common_update
			);
		}

		void _sys_common_update(ContextId& cid)
		{
			cid.get()->chunkManager->constructsome();
		}
	}
	ChunkManager::ChunkManager()
	{
		oldPlayerChunkPos = VFVec3I(9999, -9999, 9999);
		//1.构造视野区块数组
		{
			inRangeChunksRelativePos.resize(
				(2 * VF_ChunkLoadRadius + 1) *
				(2 * VF_ChunkLoadRadius + 1) *
				(2 * VF_ChunkLoadRadius + 1));
			int cnt = 0;
			for (int x = -VF_ChunkLoadRadius; x < VF_ChunkLoadRadius + 1; x++) {
				for (int y = -VF_ChunkLoadRadius; y < VF_ChunkLoadRadius + 1; y++) {
					for (int z = -VF_ChunkLoadRadius; z < VF_ChunkLoadRadius + 1; z++) {
						//如果在范围内。加入inRangeChunkPos
						if (isChunkInRange(x, y, z)) {
							//创建Key
							inRangeChunksRelativePos[cnt] = ChunkKey(x, y, z);
							//std::cout << "one in range pos\r\n";
							//printf("one in range pos %d %d %d\r\n", x, y, z);
							cnt++;
						}
					}
				}
			}
			inRangeChunksRelativePos.resize(cnt);
			chunks2Draw.resize(cnt);
		}
	}

	//defination templete
		// IVF_Obj /////////////////////////
	void ChunkManager::IVF_Obj_init(ContextId id) {
		IVF_Obj::IVF_Obj_init(id);
	}
	void ChunkManager::IVF_Obj_begin()
	{
		_chunkman_ecs_sys::regist_all(*this->get_context(), *this);
	}
	void ChunkManager::IVF_Obj_end() {}
	////////////////////////////////////

	//void ChunkManager::init(GameContext* context1)
	//{
	//	{
	//		context = context1;
	//		//chunkConstructThread = std::make_unique<ChunkConstructThread>(this);
	//		//FRunnableThread::Create(chunkConstructThread.get(),
	//		//	ANSI_TO_TCHAR("chunk_construct_thread"));
	//	}
	//}
	void constructMeshInOneDim(TextureManager& texture_man, int blockx, int blocky, int blockz,
		int blockx_p, int blocky_p, int blockz_p,
		//   uint8_t &block,
		//   uint8_t &block_p,
		_block::BlockTypeInfo& Info,
		_block::BlockTypeInfo& Info_p,
		_block::FaceDirection posDir,
		_block::FaceDirection negDir, Chunk& chunk)
	{

		/*VFVec3F chunkWorldPos;
		chunk.getChunkWorldPos(chunkWorldPos);*/

		//+1为空 当前为实心
		if (Info_p.isEmptyBlock() &&
			!Info.isEmptyBlock() &&
			Info.blockMesh->hasStandardFace(posDir))
		{
			auto block_pos = PositionInfoInChunk::OfIPos(chunk.chunkData.chunkKey, VFVec3I(blockx, blocky, blockz));
			//VF_assert(Info.blockUVSetter);
			Info.blockMesh->pushOneFaceVerticesAndIndices_selfPos(
				posDir, chunk.mesh_construct_data, block_pos,
				*Info.blockUVSetter, texture_man
			);
			//_block::pushOneFace2Chunk(chunkWorldPos, blockx, blocky, blockz, Info, posDir, chunk.vertices, chunk.triangles);
		}
		//x为空 x+1为实,添加朝x负向的面
		else if (Info.isEmptyBlock() &&
			!Info_p.isEmptyBlock() &&
			Info_p.blockMesh->hasStandardFace(negDir))
		{
			auto block_pos = PositionInfoInChunk::OfIPos(chunk.chunkData.chunkKey, VFVec3I(blockx_p, blocky_p, blockz_p));
			//VF_assert(Info.blockUVSetter);
			//if(Info.blockUVSetter)
			{
				Info_p.blockMesh->pushOneFaceVerticesAndIndices_selfPos(
					negDir, chunk.mesh_construct_data, block_pos,
					*Info_p.blockUVSetter, texture_man
				);
			}
			//_block::pushOneFace2Chunk(chunkWorldPos, blockx_p, blocky_p, blockz_p, Info_p, negDir, chunk.vertices, chunk.triangles);
		}
	}

	/*template <typename IndexArrType>
	static void addOneTriangle2IndexArr(IndexArrType& arr, int index1, int index2, int index3)
	{
		arr.Add(index1);
		arr.Add(index2);
		arr.Add(index3);
	}*/
	tl::optional<char> ChunkManager::try_read_blockdata_in_chunk(
		_type::Vec3I&& p, Chunk& chunk)
	{
		if (chunk.block_pos_valid(p))
		{
			return chunk.readData(p.X, p.Y, p.Z);
		}
		return tl::nullopt;
	}
	void ChunkManager::constructMeshForChunk(Chunk& chunk)
	{
		/*if (!chunk.mesh_construct_data.needConstruct)
		{
			return;
		}*/
		chunk.mesh_construct_data.before_construct();
		//chunk.needConstruct = false;
		auto ckkeyup_x = chunk.chunkData.chunkKey;
		auto ckkeyup_y = chunk.chunkData.chunkKey;
		auto ckkeyup_z = chunk.chunkData.chunkKey;
		ckkeyup_x.keyData.X += 1;
		ckkeyup_y.keyData.Y += 1;
		ckkeyup_z.keyData.Z += 1;
		auto chunkup_x = try_get_chunk_by_ck(ckkeyup_x);
		auto chunkup_y = try_get_chunk_by_ck(ckkeyup_y);
		auto chunkup_z = try_get_chunk_by_ck(ckkeyup_z);

		auto wrap_one_dir = [&](int x_y_z, int x, int y, int z,
			_type::Vec3I off,
			tl::optional<Chunk&>& chunkup)->char
		{
			char _block_p = 0;
			if (x_y_z == VF_ChunkWidth - 1)
			{
				//不在范围内+1不一定读到
				if (chunkup)
				{
					auto p = _type::Vec3I(x, y, z) + off;
					p.X %= VF_ChunkWidth; p.Y %= VF_ChunkWidth; p.Z %= VF_ChunkWidth;
					//在范围内+1必然读到
					auto _res = this->try_read_blockdata_in_chunk(
						std::move(p), *chunkup);
					//VF_assert(_res);
					if (_res)
					{
						_block_p = *_res;
					}
					else
					{
						VF_LogWarning("block_should be found %d %d %d", p.X, p.Y, p.Z);
					}
				}
				else
				{
					_block_p = (char)_block::StaticBlockID::Empty;
				}
			}
			else
			{
				auto p = _type::Vec3I(x, y, z) + off;
				//在范围内+1必然读到
				auto _res = try_read_blockdata_in_chunk(
					_type::Vec3I(x, y, z) + off, chunk);
				//VF_assert(_res);
				if (_res)
				{
					_block_p = *_res;
				}
				else
				{
					VF_LogWarning("block_should be found2 %d %d %d", p.X, p.Y, p.Z);
				}
			}
			return _block_p;
		};

		//边界需要判断边缘区块，若边缘区块不存在，后续区块载入时，提醒边缘区块更新
		for (int x = 0; x < VF_ChunkWidth; x++)
		{
			for (int y = 0; y < VF_ChunkWidth; y++)
			{
				for (int z = 0; z < VF_ChunkWidth; z++)
				{
					{
						char _block_p = wrap_one_dir(x, x, y, z, _type::Vec3I(1, 0, 0), chunkup_x);

						auto _block = chunk.readData(x, y, z);
						auto& Info = get_context()->blockManager->getInfo(_block);
						//auto _block_p = chunk.readData(x + 1, y, z);
						auto& Info_p_x = get_context()->blockManager->getInfo(_block_p);
						constructMeshInOneDim(
							*get_context()->taxture_man,
							x, y, z,
							x + 1, y, z,
							//   _block, _block_p,
							Info, Info_p_x,
							_block::FaceDirection::X_Positive, _block::FaceDirection::X_Negative, chunk);
					}
					{

						char _block_p = wrap_one_dir(y, x, y, z, _type::Vec3I(0, 1, 0), chunkup_y);

						auto _block = chunk.readData(x, y, z);
						auto& Info = get_context()->blockManager->getInfo(_block);
						//_block_p = chunk.readData(x, y + 1, z);
						auto& Info_p_y = get_context()->blockManager->getInfo(_block_p);
						constructMeshInOneDim(
							*get_context()->taxture_man,
							x, y, z,
							x, y + 1, z,
							//   _block, _block_p,
							Info, Info_p_y,
							_block::FaceDirection::Y_Positive, _block::FaceDirection::Y_Negative, chunk);
					}
					{
						char _block_p = wrap_one_dir(z, x, y, z, _type::Vec3I(0, 0, 1), chunkup_z);

						auto _block = chunk.readData(x, y, z);
						auto& Info = get_context()->blockManager->getInfo(_block);
						//_block_p = chunk.readData(x, y, z + 1);
						auto& Info_p_z = get_context()->blockManager->getInfo(_block_p);
						constructMeshInOneDim(
							*get_context()->taxture_man,
							x, y, z,
							x, y, z + 1,
							//   _block, _block_p,
							Info, Info_p_z,
							_block::FaceDirection::Z_Positive, _block::FaceDirection::Z_Negative, chunk);
					}
				}
			}
		}

		/*
		FVector b;
		chunk.getChunkWorldPos(b);
		chunk.vertices.Add(b + FVector(0, -100, 0));	 //lower left - 0
		chunk.vertices.Add(b + (FVector(0, -100, 100)));	 //lower left - 0
		chunk.vertices.Add(b + (FVector((0, 100, 0))));	 //lower left - 0
		chunk.vertices.Add(b + (FVector(0, 100, 100)));	 //lower left - 0

		chunk.vertices.Add(b + (FVector(200, -100, 0)));	 //lower left - 0
		chunk.vertices.Add(b + (FVector(100, -100, 100)));	 //lower left - 0
		chunk.vertices.Add(b + FVector(100, 100, 100));
		chunk.vertices.Add(b + FVector(100, 100, 0));

		//Back face of cube
		VF::addOneTriangle2IndexArr(chunk.triangles, 0, 2, 3);
		VF::addOneTriangle2IndexArr(chunk.triangles, 3, 1, 0);

		//Left face of cube
		VF::addOneTriangle2IndexArr(chunk.triangles, 0, 1, 4);
		VF::addOneTriangle2IndexArr(chunk.triangles, 4, 1, 5);

		//Front face of cube
		VF::addOneTriangle2IndexArr(chunk.triangles, 4, 5, 7);
		VF::addOneTriangle2IndexArr(chunk.triangles, 7, 5, 6);

		//Right face of cube
		VF::addOneTriangle2IndexArr(chunk.triangles, 7, 6, 3);
		VF::addOneTriangle2IndexArr(chunk.triangles, 3, 2, 7);

		//Top face
		VF::addOneTriangle2IndexArr(chunk.triangles, 1, 3, 5);
		VF::addOneTriangle2IndexArr(chunk.triangles, 6, 5, 3);

		//bottom face
		VF::addOneTriangle2IndexArr(chunk.triangles, 2, 0, 4);
		VF::addOneTriangle2IndexArr(chunk.triangles, 4, 7, 2);*/


	}


	void ChunkManager::add_chunk_2_construct(const ChunkKey& ck)
	{
		auto chunk_ = try_get_chunk_by_ck(ck);
		if (chunk_)
		{
			auto& chunk = *chunk_;
			if (chunk.add_construct_disc)
			{
				(*chunk.add_construct_disc).ms = _util::timestamp();
			}
			else
			{
				chunk.add_construct_disc =
					AddConstructDisc(_util::timestamp());
			}

			this->_chunks_2_construct.insert(ck);
		}
	}
	void ChunkManager::constructsome()
	{
		std::vector<ChunkKey> removed(this->_chunks_2_construct.size());
		//如果到时间，且没有正在构建，则调用asyncConstructMeshForChunk
		for (auto& ck : this->_chunks_2_construct)
		{
			auto c_ = this->try_get_chunk_by_ck(ck);
			if (c_)
			{
				auto& c = *c_;
				if (c.add_construct_disc
					//&& (*c.add_construct_disc).ms > 50 
					&& !c.is_constructing)
				{
					auto cptr = try_get_chunkptr_by_ck(ck);
					asyncConstructMeshForChunk(*cptr);
					c.is_constructing = true;
					c.add_construct_disc = tl::nullopt;
					removed.push_back(ck);
				}
			}
		}
		for (auto& ck : removed)
		{
			this->_chunks_2_construct.erase(ck);
		}
	}
	void ChunkManager::asyncConstructMeshForChunk(std::shared_ptr<Chunk>& chunk2Draw)
	{

		//chunkConstructThread->chunks2construct.en_queue(MoveTemp(chunk2Draw));
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
			[this, chunk2Draw]()
			{
				//VF_LogWarning("asyncConstructMeshForChunk");
				//assert(chunk2Draw.lock());
				constructMeshForChunk(*chunk2Draw);
				chunk2Draw->is_constructing = false;

				chunkCookMutex.Lock();
				chunks2Cook.emplace_back(chunk2Draw);
				chunkCookMutex.Unlock();
			}
		);
	}
	void ChunkManager::checkPlayerChunkPosChange(const VFVec3F& curPlayerPos)
	{
	}

	void ChunkManager::cookOneChunk()
	{
		/*AsyncTask(ENamedThreads::GameThread,
			[this]()
			{*/

		if (chunkCookMutex.TryLock())
		{
			int cnt = 0;
			while (chunks2Cook.size() > 0 && cnt < 10)//限制数量
			{
				cnt++;
				auto back = chunks2Cook.back();
				chunks2Cook.pop_back();



				if (back->meshId < 0)
				{
					//VF_LogWarning("createMeshAndGetId");
					//context->meshManager->customMesh = context->worldActor->CreateDefaultSubobject<UProceduralMeshComponent>("customMesh");
					back->meshId = get_context()->meshManager->createMeshAndGetId(
						back, VF_Tag_ChunkMesh, back->mesh_construct_data);
					get_context()->meshManager->getMeshById(back->meshId)->SetCollisionObjectType(VF_PhysicChannel_ChunkMesh);

				}
				else
				{

					get_context()->meshManager->updateMeshWithId(
						back->meshId,
						back->mesh_construct_data);
				}
			}

			chunkCookMutex.Unlock();
		}
	}

	std::shared_ptr<Chunk> ChunkManager::create_chunk(std::string& chunk_data, const ChunkKey& ck)
	{
		auto a = std::make_shared < Chunk>(ck, chunk_data);
		chunkKey2chunksMap[ck] = a;
		ChunkKey keys[3];
		//更新xyz负向区块
		keys[0] = ck;
		keys[0].keyData.X -= 1;
		keys[1] = ck;
		keys[1].keyData.Y -= 1;
		keys[2] = ck;
		keys[2].keyData.Z -= 1;
		for (auto& k : keys)
		{
			add_chunk_2_construct(k);
		}
		//auto b = (std::weak_ptr<Chunk>)a;
		//asyncConstructMeshForChunk(a);
		add_chunk_2_construct(ck);
		return a;
	}

	std::shared_ptr<Chunk> ChunkManager::getChunkOfKey(const ChunkKey& ck)
	{
		bool contain = chunkKey2chunksMap.contains(ck);
		VF_assert(contain);
		if (!contain)
		{
			return nullptr;
		}
		return chunkKey2chunksMap[ck];
	}
	std::shared_ptr<Chunk> ChunkManager::getChunkOfKey_not_sure(const ChunkKey& ck)
	{
		bool contain = chunkKey2chunksMap.contains(ck);
		//VF_assert(contain);
		if (!contain)
		{
			return nullptr;
		}
		return chunkKey2chunksMap[ck];
	}
	void ChunkManager::loop()
	{

	}

	//uint32 ChunkConstructThread::Run()
	//{
	//	/*AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
	//		[this, chunk2Draw]()
	//		{*/
	//	while (!this->stopping)
	//	{
	//		this->chunks2construct.if_empty_wait();

	//		while (!chunks2construct.empty())
	//		{
	//			auto chunk2Draw = chunks2construct.de_queue();
	//			VF_LogWarning("asyncConstructMeshForChunk");
	//			assert(chunk2Draw.lock());
	//			chunkManager->constructMeshForChunk(*chunk2Draw.lock());

	//			chunkManager->chunkCookMutex.Lock();
	//			chunkManager->chunks2Cook.emplace_back(chunk2Draw);
	//			chunkManager->chunkCookMutex.Unlock();
	//		}
	//		FPlatformProcess::Sleep(0.00101);
	//	}
	//	/*	}
	//	);*/
	//	return 1;
	//}
}
