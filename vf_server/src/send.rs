use crate::*;
use crate::player::Player;
use crate::entity::EntityData;
use crate::game::Game;

pub fn player_basic(
    player:Rc<RefCell<Player>>,
    entity:Rc<RefCell<EntityData>>){

}

pub fn player_interested_chunk_block_data(
    player:Rc<RefCell<Player>>,
    entity:Rc<RefCell<EntityData>>,
    game:&mut Game
){
    //计算玩家区块
    let p_ck=conv::point3f_2_chunkkey(&entity.borrow_mut().position);

    iter_relative_chunk_key_in_interest_range!(
        r_ck,
        {
            let cur_ck=p_ck.plus(r_ck);
            let packed=send_packer::pack_chunk_pack(game.get_chunk(&cur_ck));
            player.borrow_mut().sender.as_ref().unwrap().send(packed);
        }
    )
}

pub fn player_interested_chunk_entity_data(
    player:Rc<RefCell<Player>>,
    entity:Rc<RefCell<EntityData>>,
    game:&Game
){

}