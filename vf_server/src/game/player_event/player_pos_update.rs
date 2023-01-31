use crate::*;
use crate::game::Game;
use protos::common::PlayerPosUpdate;
use net::net_send;
use net::net_send_packer;
use net::net_pack;
use net::net_pack::PackIds;
use crate::net::net_send_packer::pack_to_bytes;
use game::chunk_event;

pub(crate) async fn on_player_msg(game:&Game, p:PlayerPosUpdate){

    //1. find pid, might be invalid eid
    if let Some(player)=game.player_man_mut().get_player_by_eid(p.get_entity_pos().entity_id){
        let eid=game.player_man_mut().get_player_mut(player).unwrap().entity_id;

        // update position on this server
        chunk_event::entity_pos_update::on_pl_update(
            game,&p
        ).await;

        let ck=game.entity_get(&eid).unwrap().calc_chunk_key();
        let data=pack_to_bytes(p,PackIds::EPlayerPosUpdate);
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