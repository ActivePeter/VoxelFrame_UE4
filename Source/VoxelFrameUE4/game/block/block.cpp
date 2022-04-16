#include "block.h"
#include "game/conv.h"
#include "game/GameContext.h"
#include "game/operation_rec/chunk_block.h"
#include "net/vf_NetworkManager.h"
#include "net/vf_pack.h"

//#include "app.h"
namespace VF
{
	namespace _Block
	{
		// void CommonBlockMesh::setFaceUVsByTextureIndex(Mesh &mesh, int textureIndex)
		// {
		//     float uvs[8];
		//     auto size = mesh.vertices.size();
		//     auto &tm = *App::getInstance().graphPtr->_textureManagerPtr;
		//     tm.getBlockFaceUVsByTextureIndex(uvs, textureIndex);
		//     mesh.vertices[size - 4].setUV(uvs[0], uvs[1]);
		//     mesh.vertices[size - 3].setUV(uvs[2], uvs[3]);
		//     mesh.vertices[size - 2].setUV(uvs[4], uvs[5]);
		//     mesh.vertices[size - 1].setUV(uvs[6], uvs[7]);
		// }

		//注册所有方块
		BlockManager::BlockManager()
		{
			//registerBlockAll(*this);
			// CommonBlockMeshs.resize(255);
		}

		//void pushOneFace2Chunk(const VFVec3F& chunkMeshPos, int blockx, int blocky, int blockz, _Block::BlockTypeInfo& info, _Block::FaceDirection dir,
		//	VFArray<VFVec3F>& vertexPoses,
		//	VFArray<int32>& indices, VFArray<FVector2D>& uvs, TextureManager& texture_man)
		//{

		//	VFArray<VFVec3F> poses;
		//	

		//	
		//	// mesh.appendVetexPoses(poses);
		//	// this->setVertexUVOnDir(dir, mesh);
		//	// blockUVSetter->setVertexUVOnDir(dir, mesh);
		//}

		//////////////////////////////
		///
		///     BlockDataChanger
		///   
		//////////////////////////////

		//返回原先的
		BlockDiscription BlockDataChanger::set_block_data(int x, int y, int z, BlockDiscription block_disc)
		{
			BlockDiscription ret;
			{
				auto ck =
					_conv::point_2_chunkkey(x, y, z);
				x -= ck.x() * VF_ChunkWidth;
				y -= ck.y() * VF_ChunkWidth;
				z -= ck.z() * VF_ChunkWidth;
				auto chunk =
					this->ctx.chunkManager->getChunkOfKey(ck);
				//VF_assert(chunk);
				if (chunk)
				{
					ret = chunk->readData(x, y, z);
					chunk->setData(block_disc, x, y, z);
					chunk->call_after_edit();
					this->cks[ck] = 1;
				}
			}
			return ret;
		}

		void BlockDataChanger::update_chunk_mesh()
		{
			for (auto& ck : this->cks)
			{
				auto chunk = this->ctx.chunkManager->getChunkOfKey(ck.first);
				if (chunk)
				{
					VF_LogWarning("update one chunk mesh");
					ctx.chunkManager->asyncConstructMeshForChunk(chunk);
				}
			}
			this->cks.clear();
		}

		void change_one_block_and_send_2_server(
			GameContext& ctx, Chunk& chunk,
			_type::Vec3I chunk_block_pos,
			BlockDiscription block)
		{
			{
				auto pb = std::make_shared<PutBlock>();
				auto block_world_pos = chunk.chunkData.chunkKey.keyData * VF_ChunkWidth
					+ chunk_block_pos;

				auto new_op = ctx.operation_rec->
					new_operation<_operation_rec::PutBlockOperation>(
						block_world_pos.X,
						block_world_pos.Y,
						block_world_pos.Z,
						block
						);
				VF_LogWarning("put block operation %d %d %d"
					//"%d %d %d"
					//"%d %d %d"
					,
					//block_world_pos.X,
					block_world_pos.Y,
					//block_world_pos.Z,
					chunk.chunkData.chunkKey.y(),
					chunk_block_pos.Y
				);
				new_op.ref.act(ctx);

				pb->set_block_id(block);
				pb->set_x(block_world_pos.X);
				pb->set_y(block_world_pos.Y);
				pb->set_z(block_world_pos.Z);
				pb->set_operation_id(new_op.id);

				PackContainer pc(PackIds::EPutBlock, pb);
				ctx.networkManager->send(std::move(pc));
			}
		}

		//void pushOneFace2Mesh(int blockx, int blocky, int blockz, TypeInfo& Info, FaceDirection dir, _Graph::Mesh& mesh)
		//{
		//	// printf("dir: %d \r\n", dir);
		//	//Vertex vertex;
		//	//添加方块坐标和区块坐标（网格坐标）的偏移
		//	//vertex.setPosition(mesh.mesh_position.x + blockx,
		//	//mesh.mesh_position.y + blocky,
		//	//mesh.mesh_position.z + blockz);
		//	//加入序列
		//	//for (int i = 0; i < 4; i++)
		//	//{
		//	//mesh.vertices.push_back(vertex);
		//	//}
		//	//这一块要为了后续程序统一做出整改
		//	// 构造网格时传入一个空vector<VertexPos>（临时
		//	// 构造过程中往vector填入内容，
		//	// 最后构造结束后再把数据拷贝到vertex数组里
		//	std::vector<VFVec3F> poses;
		//	Info.blockMesh->pushOneFaceVerticesAndIndices_selfPos(dir, poses, mesh.indices, mesh.vertices.size());
		//	//加上网格以及方块的坐标，转换到世界坐标上
		//	for (auto& i : poses)
		//	{
		//		i += VFVec3F(
		//			mesh.mesh_position.x + blockx,
		//			mesh.mesh_position.y + blocky,
		//			mesh.mesh_position.z + blockz);
		//	}
		//	mesh.appendVetexPoses(poses);
		//	// this->setVertexUVOnDir(dir, mesh);
		//	Info.blockUVSetter->setVertexUVOnDir(dir, mesh);

		//	// printf("%d %d\r\n", mesh.vertices.size(), mesh.indices.size());
		//}
	}
}
