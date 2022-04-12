use crate::*;
use crate::game_player::Player;
use crate::game::{Game, ClientManager};
use crate::game_entity::EntityData;


pub mod to_part_server{
    use crate::game;
    use crate::protos::common::PutBlock;
    use crate::conv;
    use crate::part_server_sync;
    use crate::net_pack_convert;
    use crate::protos;
    use crate::net_pack_convert::PackIds;
    use crate::async_task::AsyncTaskId;
    use protobuf::SingularPtrField;

    pub async fn put_block(ctx: &mut game::Game,task_id:AsyncTaskId,
                        pb: PutBlock){
        //根据方块坐标获取partserver
        let ck
            =conv::point3i_2_chunkkey2(pb.x,pb.y,pb.z);
        let sender=part_server_sync::
            get_part_server_sender_of_chunk(ctx,ck).unwrap();

        //制作pack
        let mut pack =protos::common::Cmd_PutBlockInPs::new();
        pack.task_id=task_id;
        pack.put_block=SingularPtrField::some(pb);

        //转vec发送
        let vec=net_pack_convert::pack_to_bytes(
            pack,PackIds::ECmd_PutBlockInPs);
        sender.send(vec).await;
    }
}
pub mod to_player{
    use crate::game::{ClientOperationId, ClientId};
    use crate::game;
    use crate::protos;
    use crate::net_pack_convert;
    use crate::net_pack_convert::PackIds;

    pub async fn operation_failed(ctx: &mut game::Game,cid:ClientId,opid:ClientOperationId){
        let mut pack=protos::common::ClientOperationFailed::new();
        pack.operation_id=opid;
        let vec=net_pack_convert::pack_to_bytes(pack,PackIds::EClientOperationFailed);
        let sender=ctx.client_manager.get_sender(cid);
        sender.send(vec).await;
    }
    pub async fn operation_succ(ctx: &mut game::Game,cid:ClientId,opid:ClientOperationId){
        let mut pack=protos::common::ClientOperationSucc::new();
        pack.operation_id=opid;
        let vec=net_pack_convert::pack_to_bytes(pack,PackIds::EClientOperationSucc);
        let sender=ctx.client_manager.get_sender(cid);
        sender.send(vec).await;
    }
}
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