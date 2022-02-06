use crate::*;
use tokio::sync::mpsc::Sender;
use crate::game::{ClientId, Game};
use crate::game_entity::EntityId;
use crate::protos::common;
use crate::net_pack_convert::PackIds;

pub type PlayerId = i32;
pub struct Player {
    pub player_id:PlayerId,
    pub entity_id: EntityId,
    pub client_id:ClientId,
    // pub client:Client,
    // pub sender:Option<ClientSender>
}
impl Player{
    pub fn create_uninited()->Player{
        Player{
            player_id: 0,
            entity_id: 0,
            client_id:0,
            // sender:None,
            // client: Client::create_uninited(),
        }
    }
}

pub async fn after_player_data_all_load(game:&Game,playerid:PlayerId,player_entity_id:EntityId){
    {
        let player=
            game.player_manager.playerid_2_player.get(&playerid).unwrap();
        let entity=game.entity_get(&player_entity_id).unwrap();
        // 1.player基本信息（player_entity_id
        send::player_basic(&game.client_manager, player, entity).await;
        // 2.区块地形
        send::player_interested_chunk_block_data(&game.client_manager,
                                                 player,entity,game
        ).await;
        // 3.感兴趣区块的entity数据
        send::player_interested_chunk_entity_data(&game.client_manager,
                                                  player,entity,game
        ).await;
    }
}

pub async fn handle_MainPlayerMoveCmd(cid:ClientId,game:&mut Game,pmcmd:common::MainPlayerMoveCmd){
    {
        let player = game.player_manager.get_player_by_cid(cid);
        if (player.entity_id != pmcmd.entity_id) {
            println!("wrong player");
            return;
        }
    }
    //1.找到player所在区块，
    let ck=conv::point3f_2_chunkkey(&game.entities.get(&pmcmd.entity_id).unwrap().position);
    //2.确认区块是那个服务端控制的
    let cidop=game.part_server_sync.get_part_server_cid_of_chunk(ck);
    //3.转发
    match cidop {
        None => {}
        Some(cid) => {
            game.client_manager.get_sender(cid).send(
                net_pack_convert::pack_to_bytes(pmcmd,PackIds::EMainPlayerMoveCmd)
            ).await;
        }
    }
}