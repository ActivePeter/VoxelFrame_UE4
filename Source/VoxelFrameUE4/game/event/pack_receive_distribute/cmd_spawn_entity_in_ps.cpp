#include "../PackReceiveEvent.h"
namespace VF
{
	//server side
	void pack_recv_handle(std::shared_ptr<Cmd_SpawnEntityInPs> seip, GameContext& context)
	{
		VF_LogWarning("server recv: Cmd_SpawnEntityInPs");
		auto pos_vec = VFVec3F(seip->entity_pos().x(), seip->entity_pos().y(), seip->entity_pos().z());
		//auto ck = ChunkKey::from_vf_point(pos_vec);
		auto entity_ref = context.entity_manager->born_entity(seip->entity_pos().entity_id(), EntityType::T_Player, std::move(pos_vec));
		VFVec3F ue_loc = entity_ref.base_ptr->GetTransform().GetLocation();
		_base::conv_point_from_ue_2_vf(ue_loc);
		auto rpl = std::make_shared<Rpl_SpawnEntityInPs>();
		seip->mutable_entity_pos()->set_x(ue_loc.X);
		seip->mutable_entity_pos()->set_y(ue_loc.Y);
		seip->mutable_entity_pos()->set_z(ue_loc.Z);
		rpl->mutable_entity_pos()->CopyFrom(seip->entity_pos());
		rpl->set_task_id(seip->task_id());
		context.networkManager->send(
			PackContainer(PackIds::ERpl_SpawnEntityInPs, rpl)
		);
	}
}