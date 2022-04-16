#include "../PackReceiveEvent.h"
#include "game/VF_EntityPlayerCpp.h"

namespace VF
{
	//server side
	void pack_recv_handle(std::shared_ptr<MainPlayerMoveCmd> mpmc, GameContext& context)
	{
		//VF_LogWarning("server recv: player_move");
		auto contain = context.entity_manager->server_eid_2_entity_ref.contains(mpmc->entity_id());
		VF_assert(contain);
		if (contain)
		{
			auto a = context.entity_manager->server_eid_2_entity_ref[mpmc->entity_id()];
			if (a.base_ptr)
			{
				auto trans = a.base_ptr->GetActorTransform();
				auto r = FRotator(trans.Rotator().Pitch, mpmc->rotate_yaw(), trans.Rotator().Roll);
				trans.SetRotation(r.Quaternion());
				a.base_ptr->SetActorTransform(trans);

				a.base_ptr->AddMovementInput(a.base_ptr->GetActorForwardVector(), mpmc->move_forward());
				a.base_ptr->AddMovementInput(a.base_ptr->GetActorRightVector(), mpmc->move_right());
			}
		}
	}
	void pack_recv_handle(std::shared_ptr<MainPlayerJumpCmd> mpjc, GameContext& context)
	{
		VF_LogWarning("server recv: player_jump");
		auto contain = context.entity_manager->server_eid_2_entity_ref.contains(mpjc->entity_id());
		VF_assert(contain);
		if (contain)//服务端有这个eid
		{
			auto a = context.entity_manager->server_eid_2_entity_ref[mpjc->entity_id()];
			if (a.base_ptr)//根据eid找到了实体
			{
				AVF_EntityPlayerCpp** entity_player_ptr_ptr;
				auto res =
					context.ecs.scene->randomAccessEntity(
						a.eid, entity_player_ptr_ptr
					);
				VF_assert(res);
				if (res)//确认为player entity
				{
					auto entity_player_ptr = (*entity_player_ptr_ptr);
					if (mpjc->start_or_end())
					{
						entity_player_ptr->Jump();
					}
					else
					{
						entity_player_ptr->StopJumping();
					}
				}
			}
		}
	}
}
