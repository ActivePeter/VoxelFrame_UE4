#ifndef BlockEnums
#define BlockEnums
namespace VF
{
	namespace _Block
	{
		enum class FaceDirection
		{
			X_Positive = 0,
			X_Negative,
			Y_Positive,
			Y_Negative,
			Z_Positive,
			Z_Negative,
			End,
		};
	}
}
#endif
#pragma once
/**
 * 即最普通的方块状态，撑满一格
*/
#include "block_mesh/_BlockMesh_Base.h"
#include "block_uv/_BlockUVSetter_Base.h"
#include "VF_Base.h"
#include "game/Chunk.h"

namespace VF
{
	class GameContext;

	namespace _Block
	{
		// #pragma once

		// class CommonBlockMeshModel
		// {
		// private:
		//     int []
		//     /* data */
		// public:
		// };

		//标准顺序

		// uint32_t _0_0_0 = getIndexByPos(x + 0, y + 0, z + 0);
		// uint32_t _1_0_0 = getIndexByPos(x + 1, y + 0, z + 0);
		// uint32_t _0_1_0 = getIndexByPos(x + 0, y + 1, z + 0);
		// uint32_t _0_0_1 = getIndexByPos(x + 0, y + 0, z + 1);
		// uint32_t _1_1_0 = getIndexByPos(x + 1, y + 1, z + 0);
		// uint32_t _0_1_1 = getIndexByPos(x + 0, y + 1, z + 1);
		// uint32_t _1_0_1 = getIndexByPos(x + 1, y + 0, z + 1);
		// uint32_t _1_1_1 = getIndexByPos(x + 1, y + 1, z + 1);
		//

		///////////////////////////
		using BlockDiscription = int;


		class BlockManager;

		/**
			存储所有方块信息，
			若empty，则uvsetter和blockmesh是没有的
		*/
		class BlockTypeInfo
		{

			bool isEmptyBlockFlag = false;

		public:
			std::shared_ptr<BlockUVSetter_Base> blockUVSetter;
			std::shared_ptr<BlockMesh_Base> blockMesh;
			/**
			 * 判断是否为空方快
			*/
			inline bool isEmptyBlock()
			{
				return isEmptyBlockFlag;
			}
			inline void set_isEmptyBlockFlag(bool a)
			{
				isEmptyBlockFlag = a;
			}
			static BlockTypeInfo newEmptyBlock()
			{
				BlockTypeInfo info;
				info.isEmptyBlockFlag = true;
				return info;
			}

			/**
			 * 默认构造函数
			*/
			BlockTypeInfo()
			{
			}

			/*
				模板构造函数
			*/
			template <typename BlockUVSetterType, typename BlockMeshType>
			BlockTypeInfo(std::shared_ptr<BlockUVSetterType> blockUVSetter, std::shared_ptr<BlockMeshType> blockMesh)
			{
				this->blockUVSetter = blockUVSetter; // static_cast<BlockUVSetter_Base>(blockUVSetter);
				this->blockMesh = blockMesh;
				//static_cast<BlockMesh_Base>(blockMesh);
			}
		};

		class BlockManager :public IVF_Obj
		{
		private:
			/**
			 * 以方块id的顺序存储方块信息，可以快速的随机访问
			*/
			std::vector<BlockTypeInfo> typeInfos;

			// IVF_Obj interface ///////////////////////
		public:
			//初始化：override时需要调用父类
			virtual void IVF_Obj_init(ContextId id) override;
			virtual void IVF_Obj_begin() override;
			virtual void IVF_Obj_end() override;


			/**
			 * block manager构造函数
			*/
			BlockManager();
			/**
			 * 添加block信息（在注册block时调用
			*/
			void addBlock(const BlockTypeInfo& Info)
			{
				typeInfos.push_back(Info);
			}

			/**
			 * 添加一个emptyblock（在注册block时调用
			*/
			void addEmptyBlock(const BlockTypeInfo& Info)
			{
				typeInfos.push_back(Info);
				//设置为空方块属性
				typeInfos.back().set_isEmptyBlockFlag(true);
			}
			/**
			 * 根据blockId获取Info
			*/
			BlockTypeInfo& getInfo(int blockId)
			{
				return typeInfos[blockId];
			}

			/**
			 * 在材质文件加载完之后,被texture manager 调用
			 */
			void after_block_texture_loaded();
		};

		// /**************************************
		//  * 往区块网格添加一个方块面的函数
		//  * 包含了三角形的顺序信息（indices
		//  * 顶点信息（vertices
		//  * 材质信息（uv
		//  *
		//  * 网格是区块网格，所以要加上方块的坐标作为顶点的偏移
		//  * ***************************************/
		/*void pushOneFace2Chunk(const VFVec3F& chunkMeshPos, int blockx, int blocky, int blockz, _Block::BlockTypeInfo& info, _Block::FaceDirection dir,
			VFArray<VFVec3F>& vertexPoses,
			VFArray<int32>& indices, VFArray<FVector2D>& uvs, TextureManager& texture_man);*/
			//void pushOneFace2Mesh(int blockx, int blocky, int blockz, Info& Info, _Block::FaceDirection dir, _Graph::Mesh& mesh);

		struct BlockDataChanger
		{
			GameContext& ctx;
			std::vector<ChunkKey> changed_chunks;
			phmap::flat_hash_map<ChunkKey, char> cks;
			BlockDataChanger(GameContext& ctx1) :ctx(ctx1)
			{

			}

			BlockDiscription set_block_data(
				int x, int y, int z,
				BlockDiscription block_disc);
			void update_chunk_mesh();
		};

		void change_one_block_and_send_2_server(
			GameContext& ctx,
			Chunk& chunk, _type::Vec3I chunk_block_pos,
			BlockDiscription block
		);

	}
}
