use crate::game::{Game, player, chunk};
use crate::async_task::AsyncTask;
use crate::{protos, conv};
use crate::game::entity::{entity_move_change_chunk};
use crate::game::player::PlayerId;
use std::collections::LinkedList;
use crate::protos::common::EntityType;
use crate::net_pack::{pack_to_bytes2, PackIds};
use crate::event::chunk_event;
use protobuf::Clear;

pub(crate) async fn call(game:&mut Game,rpl:protos::common::Rpl_SpawnEntityInPs){
    println!("spawn_entity_in_ps_rpl{}",rpl.task_id);
    let a=game.async_task_man_mut().finish_task(rpl.task_id);
    match a {
        AsyncTask::FinishTaskFailed => {
            return;
        }
        _=>{}
    }
    let entitypos=rpl.entity_pos.unwrap();
    // if entitypos.t==protos::common::EntityType::T_Player {
    //player 生成完成
    if entitypos.t==protos::common::EntityType::T_Player {
        let player=game.player_man_ref().get_player_by_eid(entitypos.entity_id).clone();

        //  3.update chunk
        let mut epu =protos::common::EntityPosUpdate::new();
        epu.mut_entity_pos().push(entitypos);
        // println!("epu cnt {}",epu.entity_pos.len());
        chunk_event::entity_pos_update::call(game, epu,
                                                true, player.player_id).await;


        //todo: 出生点区块坐标可能变化，还未做相应的处理
        player::send_player_basic_and_chunk(
            game,player.player_id,player.entity_id).await;

    }else{
        let mut epu =protos::common::EntityPosUpdate::new();
        epu.mut_entity_pos().push(entitypos);
        chunk_event::entity_pos_update::call(game, epu,
                                                false, 0).await;
    }
}

fn on_put_block() {

}
