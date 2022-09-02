#pragma once
#include "Chunk.h"
#include "VF_Base.h"
#include "ProtoPacks/common.pb.h"
#include "parallel_hashmap/phmap.h"
#include "VF_EntityBase.h"
#include "ProtoPacks/common.pb.h"
#include "paecs/paecs.h"

namespace VF
{
	class GameContext;

	using ServerEntityId = uint32_t;
	struct ServerEntityIdCotainer
	{
		ServerEntityId seid;
		ServerEntityIdCotainer(ServerEntityId seid1) :seid(seid1) {}
	};

	//ecs tags
	//struct ControlledEntityTag {};
	//struct GameEntityTag {};

	//struct EntityTag_Common {};
	struct EntityMoveRecord
	{
		FRotator last_rotate;//ue_p
		FVector last_p;//ue_p
		EntityMoveRecord(FRotator lr, FVector lp) :
			last_rotate(lr), last_p(lp) {}
	};
	struct EntitySyncPosData
	{
		FRotator target_ro;//ue_ro
		FVector target_p;//ue_p
		float new_tar_sync_ms;//set back to 0 when new pos in
		EntitySyncPosData(
			FRotator tr, FVector tp)
			:target_ro(tr),
			target_p(tp),
			new_tar_sync_ms(0) {}
	};

	struct MainPlayerSyncPosData
	{

	};
	//struct EntityDeadReckonData
	//{
	//	FVector target_p;//ue_p
	//	FVector begin_p;
	//	float time_spent;
	//};
	struct EntityRef
	{
		AVF_EntityBase* base_ptr;
		EntityType type;
		paecs::EntityID eid;
		ServerEntityId seid;

		EntityRef()
		{
			base_ptr = nullptr;
		}
		EntityRef(AVF_EntityBase* b, EntityType t, paecs::EntityID eid1,
			ServerEntityId seid1
		) :base_ptr(b), type(t), eid(eid1), seid(seid1) {}

		void copy_to_proto_entity_pos(EntityPos& pos);
	};

	class EntityManager :public IVF_Obj
	{
		std::vector<UClass*> entity_class;

		void init_ecs();
		void entity_load_class_models();
	public:
		phmap::flat_hash_map<ServerEntityId, EntityRef> server_eid_2_entity_ref;

		//for server
		phmap::flat_hash_set<ServerEntityId> queue_EntityPosUpdate;

		// IVF_Obj interface ///////////////////////
	public:
		//初始化：override时需要调用父类
		virtual void IVF_Obj_init(ContextId id) override;
		virtual void IVF_Obj_begin() override;
		virtual void IVF_Obj_end() override;
		//////////////////////////////////////////
	public:
		void remove_enity(bool cmd_from_server, ServerEntityId seid);
		EntityRef born_entity(ServerEntityId seid, EntityType type, VFVec3F&& vf_pos);
		void add_exist_entity(EntityRef ref);
		void born_entities_in_chunk(
			const ChunkKey&& ck,
			const google::protobuf::RepeatedPtrField< ::EntityPos>& entities);
		void add_entity_class(EntityType type, UClass* uclass);
	};
	namespace _ecs_sys
	{
		void pl_sync_entity(ContextId& id, AVF_EntityBase*& entity, ServerEntityIdCotainer& seid,
			//EntityTag_Common& common,
			EntitySyncPosData& espd);
		void ps_send_all_in_queue_EntityPosUpdate(ContextId& id);
		void ps_check_entity_move(
			ContextId& id, AVF_EntityBase*& entity, ServerEntityIdCotainer& seid,
			EntityMoveRecord& move_rec
			//,EntityTag_Common& common
		);
		//void sync_controlled_entity(ControlledEntityTag& tag);
	}


}
