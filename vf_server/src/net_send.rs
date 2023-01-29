use crate::*;
use crate::game::{Game, ClientManager, chunk};
use crate::game::player::{Player, PlayerId};
use crate::game::entity::EntityData;
use crate::game::chunk::ChunkKey;


// pub mod to_player{
//     use crate::game::{ClientOperationId, ClientId};
//     use crate::game;
//     use crate::protos;
//     use crate::net_pack_convert;
//     use crate::net_pack_convert::PackIds;
//
//
// }
pub async fn player_basic(
    client_manager: &ClientManager,
    player: &Player,
    entity: &EntityData) {
    let packed = send_packer::pack_player_basic(entity);
    let sender = client_manager.get_player_sender(player);
    sender.send(packed).await;
}

pub async fn player_one_chunk_blockdata(
    game: &Game,
    pid:PlayerId,
    ckk:&ChunkKey
){
    let ck=game.chunk_get_mut_loaded(ckk).await;
    let packed=send_packer::pack_chunk_pack(ck);
    let sender = game.client_man_ref().get_player_sender(
        game.player_man_ref().playerid_2_player.get(&pid).unwrap());
    // player.sender.as_ref().unwrap().clone().
    sender.send(packed).await;
}

pub async fn player_one_chunk_entitydata(
    game: &Game,
    pid:PlayerId,
    ckk:&ChunkKey
){

    let ck=game.chunk_get_mut_loaded(ckk).await;
    let packed=send_packer::pack_chunk_entity_pack(
        ck,
        game.entities_ref()
    );
    let sender = game.client_man_ref().get_player_sender(
        game.player_man_ref().playerid_2_player.get(&pid).unwrap());
    // player.sender.as_ref().unwrap().clone().
    sender.send(packed).await;
}

pub async fn player_interested_chunk_block_data(
    client_manager: &ClientManager,
    player: &Player,
    entity: &EntityData,
    game: &Game,
) {
    println!("send player chunk data");
    //计算玩家区块
    let p_ck = conv::point3f_2_chunkkey(&entity.position);
    iter_relative_chunk_key_in_interest_range!(
        r_ck,
        {
            let cur_ck=p_ck.plus(r_ck);
            let ck=game.chunk_get_mut_loaded(&cur_ck).await;
            // println!("send player {} {} {} {}",cur_ck.x,cur_ck.y,cur_ck.z,ck.not_air_cnt());
            let packed=send_packer::pack_chunk_pack(ck);
            let sender = client_manager.get_player_sender(player);
            // player.sender.as_ref().unwrap().clone().
            sender.send(packed).await;
        }
    )
}

pub async fn player_interested_chunk_entity_data(
    client_manager: &ClientManager,
    player: &Player,
    entity: &EntityData,
    game: &Game,
) {
    let p_ck = conv::point3f_2_chunkkey(&entity.position);

    iter_relative_chunk_key_in_interest_range!(
        r_ck,
        {
            let cur_ck=p_ck.plus(r_ck);
            let packed=send_packer::pack_chunk_entity_pack(
                game.chunk_get_mut_loaded(&cur_ck).await,
                game.entities_ref()
            );
            let sender = client_manager.get_player_sender(player);
            // println!("send player {}",player.client_id);
            //send_packer::pack_chunk_pack(game.chunk_get(&cur_ck).unwrap());
            // player.sender.as_ref().unwrap().clone().
                sender.send(packed).await;
        }
    )
}