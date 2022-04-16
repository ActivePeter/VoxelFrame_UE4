
#include "ChunkManager.h"

namespace VF
{
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
	void ChunkManager::IVF_Obj_begin() {}
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
		_Block::BlockTypeInfo& Info,
		_Block::BlockTypeInfo& Info_p,
		_Block::FaceDirection posDir,
		_Block::FaceDirection negDir, Chunk& chunk)
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
			//_Block::pushOneFace2Chunk(chunkWorldPos, blockx, blocky, blockz, Info, posDir, chunk.vertices, chunk.triangles);
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
			//_Block::pushOneFace2Chunk(chunkWorldPos, blockx_p, blocky_p, blockz_p, Info_p, negDir, chunk.vertices, chunk.triangles);
		}
	}

	/*template <typename IndexArrType>
	static void addOneTriangle2IndexArr(IndexArrType& arr, int index1, int index2, int index3)
	{
		arr.Add(index1);
		arr.Add(index2);
		arr.Add(index3);
	}*/

	void ChunkManager::constructMeshForChunk(Chunk& chunk)
	{
		if (!chunk.mesh_construct_data.needConstruct)
		{
			return;
		}
		chunk.mesh_construct_data.before_construct();
		//chunk.needConstruct = false;

		//chunk.triangles.SetNum(0);
		//chunk.vertices.SetNum(0);
		//chunk.triangles
			//vertices.clear();

			//auto blockManager = App::getInstance().gamePtr->blockManager;
		//遍历区块
		//VF_LogWarning("construct chunk %d %d %d",
		//	chunk.chunkData.chunkKey.x(),
		//	chunk.chunkData.chunkKey.y(),
		//	chunk.chunkData.chunkKey.z()
		//);

		for (int x = 0; x < VF_ChunkWidth - 1; x++)
		{
			for (int y = 0; y < VF_ChunkWidth - 1; y++)
			{
				for (int z = 0; z < VF_ChunkWidth - 1; z++)
				{
					//标准方块8个点序列对应的在区块中的索引
					// uint32_t _8points[8] = {
					//     getIndexByPos(x + 0, y + 0, z + 0),
					//     getIndexByPos(x + 1, y + 0, z + 0),
					//     getIndexByPos(x + 0, y + 1, z + 0),
					//     getIndexByPos(x + 0, y + 0, z + 1),
					//     getIndexByPos(x + 1, y + 1, z + 0),
					//     getIndexByPos(x + 0, y + 1, z + 1),
					//     getIndexByPos(x + 1, y + 0, z + 1),
					//     getIndexByPos(x + 1, y + 1, z + 1),
					// };

					auto _block = chunk.readData(x, y, z);
					auto& Info = get_context()->blockManager->getInfo(_block);

					auto _block_p = chunk.readData(x + 1, y, z);
					auto& Info_p_x = get_context()->blockManager->getInfo(_block_p);
					constructMeshInOneDim(
						*get_context()->taxture_man,
						x, y, z,
						x + 1, y, z,
						//   _block, _block_p,
						Info, Info_p_x,
						_Block::FaceDirection::X_Positive, _Block::FaceDirection::X_Negative, chunk);

					_block_p = chunk.readData(x, y + 1, z);
					auto& Info_p_y = get_context()->blockManager->getInfo(_block_p);
					constructMeshInOneDim(
						*get_context()->taxture_man,
						x, y, z,
						x, y + 1, z,
						//   _block, _block_p,
						Info, Info_p_y,
						_Block::FaceDirection::Y_Positive, _Block::FaceDirection::Y_Negative, chunk);

					_block_p = chunk.readData(x, y, z + 1);
					auto& Info_p_z = get_context()->blockManager->getInfo(_block_p);
					constructMeshInOneDim(
						*get_context()->taxture_man,
						x, y, z,
						x, y, z + 1,
						//   _block, _block_p,
						Info, Info_p_z,
						_Block::FaceDirection::Z_Positive, _Block::FaceDirection::Z_Negative, chunk);
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
	void ChunkManager::asyncConstructMeshForChunk(std::shared_ptr<Chunk>& chunk2Draw)
	{
		//chunkConstructThread->chunks2construct.en_queue(MoveTemp(chunk2Draw));
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
			[this, chunk2Draw]()
			{
				//VF_LogWarning("asyncConstructMeshForChunk");
				assert(chunk2Draw.lock());
				constructMeshForChunk(*chunk2Draw);

				chunkCookMutex.Lock();
				chunks2Cook.emplace_back(chunk2Draw);
				chunkCookMutex.Unlock();
			}
		);
	}
	void ChunkManager::checkPlayerChunkPosChange(const VFVec3F& curPlayerPos)
	{
		//auto curPlayerChunkPos = PositionInfoInChunk::fromVfPoint(curPlayerPos).chunkKey.keyData;
		//if (curPlayerChunkPos != oldPlayerChunkPos)
		//{
		//	for (int i = 0; i < chunks2Draw.size(); i++) {
		//		auto chunk2draw = chunks2Draw[i].lock();
		//		if (chunk2draw) {

		//			//不在范围内,即旧区块,从graph的绘制列表中拿出
		//			//并加入销毁列表
		//			if (!isChunkInRange(chunk2draw->chunkData.chunkKey, curPlayerChunkPos.X, curPlayerChunkPos.Y, curPlayerChunkPos.Z)) {
		//				/*App::getInstance().graphPtr->meshes2draw.erase(chunks2Draw[i].get());
		//				chunksDestroyQuene.push_back(chunks2Draw[i]);*/
		//				auto key = chunk2draw->chunkData.chunkKey;
		//				//UE_LOG(LogTemp, Warning, TEXT("mesh del %d %d %d"), key.x(), key.y(), key.z());
		//				context->meshManager->delMeshWithId(chunk2draw->meshId);
		//				chunk2draw->unbindMesh();
		//			}
		//			else {
		//				/*printf("%-2d %-2d %-2d,", player.chunkX - chunks2Draw[i]->key.x, player.chunkY - chunks2Draw[i]->key.y, player.chunkZ - chunks2Draw[i]->key.z);*/
		//			}
		//		}
		//	}
		//	//遍历球体范围的区块
		//	for (int i = 0; i < inRangeChunksRelativePos.size(); i++) {
		//		auto& cur = inRangeChunksRelativePos[i];
		//		auto chunk2Draw = getChunkOfKey(
		//			ChunkKey(
		//				curPlayerChunkPos.X + cur.x(),
		//				curPlayerChunkPos.Y + cur.y(),
		//				curPlayerChunkPos.Z + cur.z()
		//			)
		//		);
		//		chunks2Draw[i] = chunk2Draw;
		//		//chunkPtr = chunk2Draw.get(;

		//		//App::getInstance().graphPtr->meshes2draw.emplace(chunks2Draw[i].get());

		//		asyncConstructMeshForChunk(chunk2Draw);

		//		//threadPool2BuildChunkMeshes->enqueue(
		//		//	[](std::shared_ptr<Chunk> chunkPtr) { // Chunk *chunkPtr)
		//		//		// chunkPtr->dataMut.lock();
		//		//		chunkPtr->constructMesh();
		//		//		// chunkPtr->dataMut.unlock();
		//		//	},
		//		//	chunk2Draw);
		//		//chunks2Draw[i]->constructMesh();
		//	}

		//}
		//oldPlayerChunkPos = curPlayerChunkPos;
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
		//auto b = (std::weak_ptr<Chunk>)a;
		asyncConstructMeshForChunk(a);
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
