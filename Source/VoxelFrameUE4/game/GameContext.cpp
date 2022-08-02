#include "GameContext.h"

#include "Kismet/GameplayStatics.h"
#include "paecs/paecs.h"
#include "VoxelFrameUE4/net/vf_NetworkManager.h"

//#include "ecs/ChunkEvent.h"

namespace VF
{
	/// //////////////////////////////////////////////////////////


	static const int context_max = 10;

	static GameContext* context_pool[context_max];

	static ContextId put_next_context(GameContext* context)
	{
		static uint8_t cnt = 0;
		static uint8_t next_context_id = 0;

		ContextId cid;
		if (cnt > context_max || next_context_id > context_max || context_pool[0] == 0)
		{
			VF_LogWarning("context cnt reset");
			cnt = 0;
			next_context_id = 0;
		}
		context_pool[next_context_id] = context;

		cid.id = next_context_id;

		next_context_id++;
		cnt++;
		if (next_context_id >= context_max)
		{
			next_context_id = 0;
		}

		return cid;
	}
	static void remove_context(ContextId contextid)
	{
		//context_pool[contextid.id] = nullptr;
		memset(context_pool, 0, sizeof(context_pool));
	}
	/////////////////////////////////////////////////////////////////////////////

	// IVF_Obj /////////////////////////
	void PlayerOnly::IVF_Obj_init(ContextId id) {
		IVF_Obj::IVF_Obj_init(id);
	}
	void PlayerOnly::IVF_Obj_begin() {}
	void PlayerOnly::IVF_Obj_end() {}
	////////////////////////////////////

	// IVF_Obj /////////////////////////
	void ServerOnly::IVF_Obj_init(ContextId id) {
		IVF_Obj::IVF_Obj_init(id);
	}
	void ServerOnly::IVF_Obj_begin() {}
	void ServerOnly::IVF_Obj_end() {}
	////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	//std::unordered_map<char, GameContext*> context_map;
	//static GameContext* g_context = nullptr;
	GameContext* GameContext::get_context(ContextId context_id)
	{
		if (context_id.id >= context_max)
		{
			return nullptr;
		}
		return context_pool[context_id.id];
	}
	GameContext* ContextId::get()
	{
		return GameContext::get_context(*this);
	}

	FVector2D _VFRect::get_top_right(const VFRect<float>& rect)
	{
		return FVector2D(rect.x1 + rect.w, rect.y1);
	}

	FVector2D _VFRect::get_bottom_right(const VFRect<float>& rect)
	{
		return FVector2D(rect.x1 + rect.w, rect.y1 + rect.h);
	}

	FVector2D _VFRect::get_top_left(const VFRect<float>& rect)
	{
		return FVector2D(rect.x1, rect.y1);
	}

	FVector2D _VFRect::get_bottom_left(const VFRect<float>& rect)
	{
		return FVector2D(rect.x1, rect.y1 + rect.h);
	}

	GameContext::GameContext(ClientType client_type, AWorldActor* world_actor)
	{
		context_id = put_next_context(this);
		while (context_id.get() != this)
		{
			VF_LogWarning("error context id set");
		}
		this->worldActor = world_actor;
		this->type = client_type;

		//加载各类全局ecs系统
		ecs.init();

		ecs.scene->registSingleton<ContextId>([this](ContextId& cid)
			{
				cid.id = this->context_id.id;
			});

		this->event_list = std::make_shared< _Event::EventList>();
		chunkManager = make_obj<ChunkManager>();
		meshManager = make_obj<MeshManager>();
		blockManager = make_obj<_block::BlockManager>();
		entity_manager = make_obj<EntityManager>();
		block_preview_manager = make_obj<_block::BlockPreviewManager>();
		networkManager = make_obj<NetworkManager>();
		operation_rec = make_obj<_operation_rec::OperationRec>();
		taxture_man = make_obj<TextureManager>();
		//TArray<AActor*> blockPutPreviewerArr;
		//UGameplayStatics::GetAllActorsWithTag(
			//context->worldActor->GetWorld(),
			//"blockPutPreviewer", blockPutPreviewerArr);
		//assert(blockPutPreviewerArr.Num() == 1);
		/*block_preview_manager.init(
			Cast<AStaticMeshActor>(blockPutPreviewerArr[0]));*/
			//assert(blockPreviewManager->checkInited());
		if (type == ClientType::PartServer)
		{
			server_only = make_obj<ServerOnly>();
		}
		else if (type == ClientType::Player)
		{
			player_only = make_obj<PlayerOnly>();
			//blockPreviewManager = std::make_unique<_block::BlockPreviewManager>();
		}

		for (auto& i : vf_objs)
		{
			i->IVF_Obj_init(this->context_id);
		}
	}
	GameContext::~GameContext()
	{
		memset(context_pool, 0, sizeof(context_pool));
		//remove_context(context_id);
		//context = nullptr;
	}
	void GameContext::world_actor_call_when_BeginPlay()
	{
		//context.chunkManager = std::make_shared<VF::ChunkManager>();
		for (auto& i : vf_objs)
		{
			i->IVF_Obj_begin();
		}

		networkManager->connectServer(networkManager, "127.0.0.1"/*FIPv4Address(127, 0, 0, 1)*/, 7776);
		{//首次连接后，发送
			auto cfs = std::make_shared<ClientFirstConfirm>();
			if (type == ClientType::PartServer)
			{
				VF_LogWarning("send ClientType_GameServer");
				cfs->set_client_type(ClientType_GameServer);
			}
			else
			{
				VF_LogWarning("send ClientType_Player");
				cfs->set_client_type(ClientType_Player);
			}
			networkManager->send(PackContainer_make(cfs));
		}
	}

	void GameContext::world_actor_call_when_EndPlay()
	{
		networkManager->close();

		remove_context(this->context_id);
	}

	void Ecs::init()
	{
		scene = std::make_unique<paecs::Scene>();

	}


	_main_player::AllCompPtr GameContext::main_player_get_all_comps()
	{
		return _main_player::get_mainplayer_comps_by_eid((*this->ecs.scene), main_player_eid);
	}
}
