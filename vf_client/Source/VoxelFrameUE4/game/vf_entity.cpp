#include "vf_entity.h"

#include "GameContext.h"
#include "VF_EntityPlayerCpp.h"
#include "net/vf_NetworkManager.h"

namespace VF
{
	//defination templete
			// IVF_Obj /////////////////////////
	void EntityManager::IVF_Obj_init(ContextId id) {
		IVF_Obj::IVF_Obj_init(id);

	}
	void EntityManager::IVF_Obj_begin()
	{
		entity_load_class_models();
		init_ecs();
	}
	void EntityManager::IVF_Obj_end() {}

	void EntityManager::remove_enity(bool cmd_from_server, ServerEntityId seid)
	{
		auto iter = this->server_eid_2_entity_ref.find(seid);
		if (iter != this->server_eid_2_entity_ref.end())
		{
			UE_LOG(LogTemp, Warning, TEXT("remove_enity %d"), seid);
			auto er = iter->second;
			get_context()->ecs.scene->deleteEntity(er.eid);
			er.base_ptr->K2_DestroyActor();
			this->server_eid_2_entity_ref.erase(iter);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("remove_enity not found"));
		}
	}

	////////////////////////////////////


	void EntityRef::copy_to_proto_entity_pos(EntityPos& ep)
	{
		ep.set_t(type);
		ep.set_entity_id(seid);
		auto& trans = base_ptr->GetTransform();
		ep.set_x(trans.GetLocation().X / VF_WorldScale);
		ep.set_y(trans.GetLocation().Y / VF_WorldScale);
		ep.set_z(trans.GetLocation().Z / VF_WorldScale);

		ep.set_r_pitch(trans.Rotator().Pitch);
		ep.set_r_roll(trans.Rotator().Roll);
		ep.set_r_yaw(trans.Rotator().Yaw);

		ep.set_vx(base_ptr->GetVelocity().X / VF_WorldScale);
		ep.set_vy(base_ptr->GetVelocity().Y / VF_WorldScale);
		ep.set_vz(base_ptr->GetVelocity().Z / VF_WorldScale);
	}

	void EntityManager::init_ecs()
	{
		/*get_context()->ecs.scene->addSys2Group(
			get_context()->ecs.commonUpdateSys, _ecs_sys::sync_controlled_entity
		);*/
		if (get_context()->type == ClientType::PartServer)
			//if (0)
		{
			get_context()->ecs.scene->addSys2Group(
				get_context()->ecs.commonUpdateSys, _ecs_sys::ps_check_entity_move
			);
			get_context()->ecs.scene->addSys2Group(
				get_context()->ecs.tick_end_sys, _ecs_sys::ps_send_all_in_queue_EntityPosUpdate
			);
		}
		else
		{
			get_context()->ecs.scene->addSys2Group(
				get_context()->ecs.commonUpdateSys, _ecs_sys::pl_sync_entity
			);
		}
	}
	////////////////////////////
	//  entity resource load
	////////////////////////////
#define __load(__path__, __cpp_parent__, __enum__)\
{\
	UClass* entity = LoadClass<__cpp_parent__>(nullptr,\
		TEXT(__path__));\
	if (!entity)\
	{\
		VF_LogWarning("entity_load_class_models failed")\
			return;\
	}\
	get_context()->entity_manager->add_entity_class(\
	__enum__, entity);\
}

	void EntityManager::entity_load_class_models()
	{
		__load(
			"Blueprint'/Game/vf_entity/vf_player/VF_EntityPlayer.VF_EntityPlayer_C'",
			AVF_EntityPlayerCpp,
			EntityType::T_Player
		)

	}

#define __born_entity_switch(__EntityType__)\
	case EntityType::__EntityType__:	\

	EntityRef EntityManager::born_entity(ServerEntityId seid, EntityType type, VFVec3F&& ue_pos)
	{
		VF_LogWarning("spawn entity in server:%d", seid);
		EntityRef ref;
		auto param = FActorSpawnParameters();
		param.bNoFail = true;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//_base::conv_point_from_vf_2_ue(vf_pos);//conved to ue pos
		//vf_pos *= VF_WorldScale;
		switch (type)
		{
		case EntityType::T_Player:
		{
			auto ptr =
				get_context()->worldActor->GetWorld()
				->SpawnActor<AVF_EntityPlayerCpp>(entity_class[int(type)], ue_pos, FRotator::ZeroRotator, param);

			if (ptr)
			{
				ptr->SpawnDefaultController();
				auto eid = get_context()->ecs.scene->createEntity()
					.addComponent(ptr)//保存entityplayer指针
					.addComponent((AVF_EntityBase*)ptr)
					//.addEmptyComponent<EntityTag_Common>()
					.addComponent(EntityMoveRecord(ptr->GetTransform().Rotator(), ptr->GetTransform().GetLocation()))
					.addComponent(ServerEntityIdCotainer(seid))
					.addComponent(EntitySyncPosData(ptr->GetTransform().Rotator(),
						ptr->GetTransform().GetLocation()))
					.entityId;

				ref = EntityRef(ptr, type, eid, seid);
				server_eid_2_entity_ref[seid] = ref;
			}
			else
			{
				VF_LogWarning("fatal err: fail to born actor")
			}
		}
		break;
		}
		return ref;
	}

	void EntityManager::add_exist_entity(EntityRef ref)
	{
		this->server_eid_2_entity_ref[ref.seid] = ref;
		VF_LogWarning("add_exist_entity %d", ref.seid);
	}

	///////////////////////////////////////////////////////////////////




	void EntityManager::born_entities_in_chunk(
		const ChunkKey&& ck,
		const google::protobuf::RepeatedPtrField<EntityPos>& entities)
	{
		auto& con = (*get_context());
		for (auto& entity : entities)
		{
			auto ue_pos = VFVec3F(entity.x(), entity.y(), entity.z());
			VFVec3F_vf2ue(ue_pos);

			//若为main player id
			if (entity.entity_id() == con.main_player_server_eid && get_context()->type == ClientType::Player)
			{
				VF_Log("mainplayer entity info get:\n  chunk %d %d %d,eid %d,pid %d",
					ck.x(),
					ck.y(),
					ck.z(),
					entity.entity_id(),
					con.main_player_server_eid);

				auto comp = con.main_player_get_all_comps();
				if (comp.character)
				{


					AVoxelFrameUE4Character* character = *comp.character;
					//VF_Log("before main player first_position_set");
					character->first_position_set(
						std::move(ue_pos)
					);
				}

			}
			else//其余entity
			{
				VF_LogWarning("one_entity_borned");
				//VFVec3F vec = VFVec3F::ZeroVector;
				auto ref = born_entity(entity.entity_id(), EntityType::T_Player,
					std::move(ue_pos));
				if (ref.base_ptr)
				{
					/*ref.base_ptr->SetActorLocation(
						VFVec3F(
							entity.x(), entity.y(), entity.z()
						)
					);*/
				}
			}
		}

	}

	void EntityManager::add_entity_class(EntityType type, UClass* uclass)
	{
		if (entity_class.size() < (int)type + 1)
		{
			entity_class.resize((int)type + 1);
		}
		entity_class[(int)type] = uclass;
	}


	namespace _ecs_sys
	{
		/*void client_entity_deadreckoning(ContextId& id, AVF_EntityBase*& entity, EntityDeadReckonData& dr)
		{
			entity->AddMovementInput()
		}*/

		/*void sync_controlled_entity(ControlledEntityTag& tag) {
		}*/
		void pl_sync_entity(ContextId& id, AVF_EntityBase*& entity, ServerEntityIdCotainer& seid,
			EntitySyncPosData& espd)
		{
			if (seid.seid == id.get()->main_player_server_eid)
			{//main player 策略

			}
			else//其余entity
			{
				auto delta = espd.target_p - entity->GetActorLocation();
				if (delta.Size() > 500)
				{
					entity->SetActorLocation(espd.target_p);
				}
				else if (delta.Size() > 100)
				{
					auto lo = espd.target_p;
					//VF_LogWarning("sync other entity %d,cur entity %d", seid, id.get()->main_player_server_eid);
					//VF_LogWarning("delta %.2f %.2f %.2f %.2f", delta.Size(), lo.X, lo.Y, lo.Z);
					entity->AddMovementInput(delta, id.get()->deltatime * 1000);
					//entity->SetActorLocation(espd.target_p);
				}
				else
				{
					entity->SetActorLocation(espd.target_p);
				}
			}
			espd.new_tar_sync_ms += id.get()->deltatime;
		}

		//todo 执行这个函数过程中记录下所有需要发送的entity.一次性发送
		void ps_check_entity_move(
			ContextId& id, AVF_EntityBase*& entity, ServerEntityIdCotainer& seid,
			EntityMoveRecord& move_rec)
		{
			//VF_LogWarning("ps_check_entity_move");
			auto delta_rotate = entity->GetTransform().Rotator() - move_rec.last_rotate;
			if ((entity->GetTransform().GetLocation() - move_rec.last_p).Size() > VF_Sync_Threshold_Move
				|| fabsf(delta_rotate.Roll) > VF_Sync_Threshold_Rotate
				|| fabsf(delta_rotate.Pitch) > VF_Sync_Threshold_Rotate
				|| fabsf(delta_rotate.Yaw) > VF_Sync_Threshold_Rotate
				)
			{
				//VF_LogWarning("ps_check_entity_move %d", seid);
				id.get()->entity_manager->queue_EntityPosUpdate.insert(seid.seid);

				move_rec.last_rotate = entity->GetTransform().Rotator();
				move_rec.last_p = entity->GetTransform().GetLocation();
			}
		}

		void ps_send_all_in_queue_EntityPosUpdate(ContextId& id)
		{
			auto& queue = id.get()->entity_manager->queue_EntityPosUpdate;
			if (queue.size() > 0)
			{
				auto pack = std::make_shared<EntityPosUpdate>();
				pack->mutable_entity_pos()->Reserve(queue.size());
				for (auto& seid : queue)
				{
					auto& eref = id.get()->entity_manager->server_eid_2_entity_ref[seid];
					EntityPos* ep = pack->mutable_entity_pos()->Add();
					eref.copy_to_proto_entity_pos(*ep);
				}

				//VF_LogWarning("ps_send_all_in_queue_EntityPosUpdate");
				id.get()->networkManager->send(PackContainer(PackIds::EEntityPosUpdate, pack));
				//end
				queue.clear();
			}
		}
	}
}
