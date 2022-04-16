namespace VF
{
	class GameContext;
}

#ifndef __GameContext_H_
#define __GameContext_H_

#include "main_player.h"
#include "VF_Base.h"
#include "paecs/paecs.h"
#include "event/Event.h"
//#include "base/SafeQuene.h"
#include "vf_entity.h"
#include "block/BlockPreviewManager.h"
#include "operation_rec/operation_rec.h"
#include "texture_man.h"


class AWorldActor;
namespace VF
{

	class ChunkManager;
	struct MeshManager;
	class NetworkManager;
	namespace _main_player
	{
		struct AllCompPtr;
	}

	namespace _Block
	{
		class BlockManager;
		class BlockPreviewManager;
	}

	struct Ecs
	{
		std::unique_ptr<paecs::Scene> scene;
		paecs::SysGroup commonUpdateSys;
		paecs::SysGroup update_sys_100ms;
		paecs::SysGroup tick_end_sys;
		paecs::SysGroup game_end_sys;
		void init();
	};

	enum class ClientType
	{
		Player,
		PartServer,
	};

	class PlayerOnly :public IVF_Obj
	{
		// IVF_Obj interface ///////////////////////
	public:
		//初始化：override时需要调用父类
		virtual void IVF_Obj_init(ContextId id) override;
		virtual void IVF_Obj_begin() override;
		virtual void IVF_Obj_end() override;

	};
	class ServerOnly :public IVF_Obj
	{
		// IVF_Obj interface ///////////////////////
	public:
		//初始化：override时需要调用父类
		virtual void IVF_Obj_init(ContextId id) override;
		virtual void IVF_Obj_begin() override;
		virtual void IVF_Obj_end() override;

	};


	class GameContext :std::enable_shared_from_this<GameContext>
	{
	private:
		std::list<std::shared_ptr<IVF_Obj>> vf_objs;
	public:
		ContextId context_id;
		static GameContext* get_context(ContextId context_id);

		std::shared_ptr<ChunkManager> chunkManager;
		std::shared_ptr<MeshManager> meshManager;
		std::shared_ptr<_Block::BlockManager> blockManager;
		std::shared_ptr<_Block::BlockPreviewManager> block_preview_manager;
		std::shared_ptr<NetworkManager> networkManager;
		std::shared_ptr<EntityManager> entity_manager;
		std::shared_ptr<PlayerOnly> player_only;
		std::shared_ptr<ServerOnly> server_only;
		std::shared_ptr<_operation_rec::OperationRec> operation_rec;
		std::shared_ptr<TextureManager> taxture_man;

		ClientType type;
		Ecs ecs;
		AWorldActor* worldActor;
		float deltatime;

		//main player
		paecs::EntityID main_player_eid;
		ServerEntityId main_player_server_eid;
		_main_player::AllCompPtr main_player_get_all_comps();

		//std::list<std::shared_ptr<_Event::IEvent>> events;
		_Event::EventList event_list;
		//SafeQueue<> rec_pack_queue;
	private:
		template <class VF_Obj_Type, class... ArgTypes>
		std::shared_ptr<VF_Obj_Type> make_obj(ArgTypes&&... _Args)
		{
			auto p = std::make_shared<VF_Obj_Type>(
				std::forward<_Types>(_Args)...);
			vf_objs.push_back(p);
			return p;
		}
	public:
		/*
		 *WorldActor 调用的函数，用来检查是否一切就绪
		 ***/
		void world_actor_call_when_BeginPlay();
		void world_actor_call_when_EndPlay();

		GameContext(ClientType client_type, AWorldActor* world_actor);
		~GameContext();
	};


}

#include "MeshManager.h"
#include "ChunkManager.h"
#include "block/block.h"
#include "game_WorldActor.h"




#endif

