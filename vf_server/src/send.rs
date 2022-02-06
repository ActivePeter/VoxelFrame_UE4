use crate::*;
use crate::game_player::Player;
use crate::game::{Game, ClientManager};
use crate::game_entity::EntityData;


pub async fn player_basic(
    client_manager: &ClientManager,
    player: &Player,
    entity: &EntityData) {
    let packed = send_packer::pack_player_basic(entity);
    let sender = client_manager.get_player_sender(player);
    sender.send(packed).await;
}

pub async fn player_interested_chunk_block_data(
    client_manager: &ClientManager,
    player: &Player,
    entity: &EntityData,
    game: &Game,
) {
    //计算玩家区块
    let p_ck = conv::point3f_2_chunkkey(&entity.position);

    iter_relative_chunk_key_in_interest_range!(
        r_ck,
        {
            let cur_ck=p_ck.plus(r_ck);
            let packed=send_packer::pack_chunk_pack(game.chunk_get(&cur_ck).unwrap());
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
                game.chunk_get(&cur_ck).unwrap(),
                &game.entities
            );
            let sender = client_manager.get_player_sender(player);
            // println!("send player {}",player.client_id);
            //send_packer::pack_chunk_pack(game.chunk_get(&cur_ck).unwrap());
            // player.sender.as_ref().unwrap().clone().
                sender.send(packed).await;
        }
    )
}