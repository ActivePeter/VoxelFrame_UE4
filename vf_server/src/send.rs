use crate::*;
use crate::player::Player;
use crate::entity::EntityData;
use crate::game::Game;

pub async fn player_basic(
    player: &Player,
    entity: &EntityData) {
    let packed = send_packer::pack_player_basic(entity);
    player.sender.as_ref().unwrap().clone().sender.
        send(packed).await;
}

pub async fn player_interested_chunk_block_data(
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
            player.sender.as_ref().unwrap().clone().
                sender.send(packed).await;
        }
    )
}

pub async fn player_interested_chunk_entity_data(
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
            //send_packer::pack_chunk_pack(game.chunk_get(&cur_ck).unwrap());
            player.sender.as_ref().unwrap().clone().
                sender.send(packed).await;
        }
    )
}