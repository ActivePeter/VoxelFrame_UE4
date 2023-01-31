use crate::game::Game;
use crate::protos::common::PlayerPosUpdate;
use crate::net_send;
use crate::net_send_packer;
use crate::net_pack;
use crate::net_pack::PackIds;

pub(crate) async fn call(game:&Game,p:PlayerPosUpdate){

    //1. find pid
    if let Some(player)=game.player_man_mut().get_player_by_eid(p.get_entity_pos().entity_id){
        let eid=game.player_man_mut().get_player_mut(player).unwrap().entity_id;
        // update position on this server
        let entity=game.entities_mut().get_mut(&eid).unwrap();
        entity.set_position_by_data(p.get_entity_pos());
        let ck=entity.calc_chunk_key();
        let data=net_pack::pack_to_bytes(p,PackIds::EPlayerPosUpdate);
        let iter=net_send::SenderIterBuilder::new()
            .set_part_server(true)
            .set_except(*player)
            .set_range_chunk(ck)
            .build(game);
        for sender in iter{
            // println!("PlayerPosUpdate");
            sender.send(data.clone(),PackIds::EPlayerPosUpdate.default_priority()).await;
        }
    }
}