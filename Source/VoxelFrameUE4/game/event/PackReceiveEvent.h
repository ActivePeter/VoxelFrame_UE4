#pragma once
#include "./Event.h"
#include "net/vf_pack.h"
#include "game/GameContext.h"
#include "ProtoPacks/common.pb.h"
#include "pack_receive_distribute/pack_recv_handle_inc.h"
namespace VF
{

	////chunk_pack
	//void pack_recv_handle(std::shared_ptr <ChunkPack> chunk_pack, GameContext& context);

	////player_basic
	//void pack_recv_handle(std::shared_ptr<PlayerBasic> player_basic, GameContext& context);

	////chunk_entity_pack
	//void pack_recv_handle(std::shared_ptr<ChunkEntityPack> chunk_entity_pack, GameContext& context);

	////main_player_move
	//void pack_recv_handle(std::shared_ptr<MainPlayerMoveCmd> mpmc, GameContext& context);

	////cmd_spawn_entity.cpp
	//void pack_recv_handle(std::shared_ptr<Cmd_SpawnEntityInPs> seip, GameContext& context);

	////entity_pos_update.cpp
	//void pack_recv_handle(std::shared_ptr<EntityPosUpdate> epu, GameContext& context);

	////put_block.cpp
	//void pack_recv_handle(std::shared_ptr<PutBlock> epu, GameContext& context);
	//void pack_recv_handle(std::shared_ptr<Cmd_PutBlockInPs> epu, GameContext& context);

	////operation.cpp
	//void pack_recv_handle(
	//	std::shared_ptr<ClientOperationFailed> epu, GameContext& context);
	//void pack_recv_handle(
	//	std::shared_ptr<ClientOperationSucc> cof,
	//	GameContext& context);


	namespace _Event
	{
		//class NewChunkLoadEvent :public IEvent
		//{
		//public:
		//	// IEvent ////////////////////////////////
		//	void run() override;
		//	/////////////////////////////////////////

		//	// Self ////////////////////////////////
		//	std::shared_ptr<ChunkPack> chunk_pack;
		//	/////////////////////////////////////////

		//	static auto create(const std::shared_ptr<ChunkPack>& chunk_pack)
		//	{
		//		auto a = std::make_shared<NewChunkLoadEvent>();
		//		a->chunk_pack = chunk_pack;
		//		//return (std::shared_ptr<IEvent>)a;
		//		return a;
		//		//return static_cast< >(a);
		//	}
		//};
		class PackRecieveEvent :public IEvent, public IVF_Obj
		{
		public:
			// IEvent ////////////////////////////////
			void run(GameContext& context) override;
			/////////////////////////////////////////

			// IVF_Obj interface ///////////////////////
			//初始化：override时需要调用父类
			virtual void IVF_Obj_init(ContextId id) override;
			virtual void IVF_Obj_begin() override;
			virtual void IVF_Obj_end() override;
			////////////////////////////////////////////

			// Self ////////////////////////////////
			PackContainer pack_container;

			/////////////////////////////////////////
			static auto create(ContextId id, const PackContainer&& pack_container)
			{

				auto a = std::make_shared<PackRecieveEvent>();
				a->pack_container = std::move(pack_container);
				/*a->proto_pack = proto_pack;
				a->pack_id = pack_id;*/
				//return (std::shared_ptr<IEvent>)a;
				return a;
				//return static_cast< >(a);
			}
		};
	}
}

