use crate::base::*;
use protobuf::Message;
use crate::send::SendFilter;

// pack ids
enum PackIds {
    TotalChunk = 0,
    EntityMove,
    _End,
}

struct PackDescription {
    from: PlayerId,
}

pub fn handle_pack_buff(pid: PlayerId, pack_id: u8, buff: &[u8]) {
    if pack_id >= PackIds::_End as u8 {
        panic!("error: pack_id is out of range");
    }
    let pack_id_enum: PackIds = unsafe { std::mem::transmute(pack_id) };
    match pack_id_enum
    {
        PackIds::EntityMove => {
            // let mut pack: protos::common::EntityMove;
            // pack.merge_from_bytes(buff);
            let proto_pack = protos::common::EntityMove::parse_from_bytes(buff).unwrap();
            handle_entity_move_pack(pid, proto_pack, buff);
        }
        _ => {}
    }
}

fn handle_entity_move_pack(pid: PlayerId, pack: protos::common::EntityMove, rawbuf: &[u8]) {
    //玩家坐标，更新区块状态，给其余玩家全部发送
    if pack.t == protos::common::EntityType::T_Player {
        let sendFilter = SendFilter::new().add_player_2_blacklist(pid);
        send::data2all_with_filter(&sendFilter, rawbuf);
        println!("player move {0},{1},{2}", pack.x, pack.y, pack.z);
        // GAME_CONTEXT.lock().await.player_manager.
    }
}