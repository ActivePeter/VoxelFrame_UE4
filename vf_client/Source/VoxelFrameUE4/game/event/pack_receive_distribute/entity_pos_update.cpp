#include "../PackReceiveEvent.h"
#include "game/main_player.h"

namespace VF
{
	void pack_recv_handle(std::shared_ptr<EntityPosUpdate> epu, GameContext& context)
	{
		//VF_LogWarning("pack rec EntityPosUpdate");
		for (auto& e : epu->entity_pos())
		{
			if (e.entity_id() != context.main_player_server_eid)
			{
				//VF_LogWarning("entity %d %d", e.entity_id(), context.main_player_server_eid);
				auto a = context.entity_manager->server_eid_2_entity_ref.find(e.entity_id());
				if (a != context.entity_manager->server_eid_2_entity_ref.end())
				{
					auto& edata = a->second;

					edata.base_ptr->SetActorLocation(VFVec3F(e.x(), e.y(), e.z()) * VF_WorldScale);
					edata.base_ptr->GetRootComponent()->ComponentVelocity = VFVec3F(e.vx(), e.vy(), e.vz()) * VF_WorldScale;
					/*VF_LogWarning("  p1 %.2f %.2f %.2f, p2 %.2f %.2f %.2f",
						edata.base_ptr->GetTransform().GetLocation().X,
						edata.base_ptr->GetTransform().GetLocation().Y,
						edata.base_ptr->GetTransform().GetLocation().Z,
						e.x() * VF_WorldScale,
						e.y() * VF_WorldScale,
						e.z() * VF_WorldScale
					);*/
					edata.base_ptr->SetActorRotation(FRotator(e.r_pitch(), e.r_yaw(), e.r_roll()));
					EntitySyncPosData* espd = nullptr;
					context.ecs.scene->randomAccessEntity(edata.eid, espd);
					if (espd)
					{
						//VF_LogWarning("target %.2f %.2f %.2f",)
						espd->target_p = VFVec3F(e.x(), e.y(), e.z()) * VF_WorldScale;
						//VF_LogWarning("entity have EntitySyncPosData %d", edata.seid);
					}
					else
					{
						VF_LogWarning("entity dont have EntitySyncPosData %d", edata.seid);
					}
					//edata.base_ptr->SetActorLocation(VFVec3F(e.x(), e.y(), e.z()) * VF_WorldScale);
					//edata.base_ptr->GetCharacterMovement()->Velocity = VFVec3F(e.vx(), e.vy(), e.vz()) * VF_WorldScale;

				//edata.base_ptr->AddMovementInput(
				//	FVector(e.x(), e.y(), e.z()) * VF_WorldScale - edata.base_ptr->GetTransform().GetLocation());
				}
				else
				{
					//VF_LogWarning("  entity not found %d", e.entity_id());
					context.entity_manager->born_entity(e.entity_id(), e.t(), VFVec3F(e.x(), e.y(), e.z()) * VF_WorldScale);
				}
			}
			else
			{//sync main player
				_main_player::sync_from_server(context, e);
			}
		}

	}
}
