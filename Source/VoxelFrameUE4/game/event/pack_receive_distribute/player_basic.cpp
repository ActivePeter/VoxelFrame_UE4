#include "../PackReceiveEvent.h"
#include "game/main_player_Character.h"
#include "game/vf_entity.h"

namespace VF
{
	void pack_recv_handle(std::shared_ptr<PlayerBasic> player_basic, GameContext& context)
	{
		VF_Log("pack_recv PlayerBasic %d", player_basic->entity_id());
		context.main_player_server_eid = player_basic->entity_id();
		AVoxelFrameUE4Character** mp = nullptr;
		context.ecs.scene->randomAccessEntity(context.main_player_eid, mp);
		if (mp)
		{
			context.entity_manager->add_exist_entity(
				EntityRef(*mp, EntityType::T_Player, context.main_player_eid, context.main_player_server_eid));
		}
		else
		{
			VF_LogWarning("fatal err: no main player find");
		}
	}
}
