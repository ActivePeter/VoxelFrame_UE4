use crate::protos::common::{ClientType, PutBlock};
use crate::protos::common::ClientType::{ClientType_Player, ClientType_GameServer};
use crate::game::{ClientId, Game};
use crate::{game, send};
use crate::log;
// use crate::send;
use crate::async_task;
use crate::net::ClientMsg;
use crate::net_pack_convert::MsgEnum;
use game::operation::OperationResult;
// use crate::game::game_entity::update_entity_pos;

mod put_block{
    use crate::*;
    use crate::{game, log, conv, protos, net_pack_convert};
    use crate::protos::common::{ClientType, PutBlock, Rpl_PutBlockInPs};
    use crate::game::{ClientId, part_server_sync, Game};
    use crate::protos::common::ClientType::ClientType_Player;
    use crate::async_task::AsyncTask;
    use protobuf::SingularPtrField;
    use crate::net_pack_convert::PackIds;
    use crate::game::operation::OperationResult;
    use crate::game::game_chunk::Chunk;
    use crate::base_type::{Point3i, point3f_new2};


    // pub mod chunksync{
    //     use crate::protos::common::PutBlock;
    //     use crate::conv;
    //     use crate::game::Game;

    pub async fn notifyinterest_putblock(ctx:&mut Game,putter_cid:ClientId,pb:Box<PutBlock>){
        let ck =conv::point3i_2_chunkkey2(pb.x,pb.y,pb.z);
        let chunk=ctx.chunks.get(&ck).unwrap();
        let v=
            net_pack_convert::packbox_to_bytes(pb,PackIds::EPutBlock);
        for pid in &chunk.be_interested_by{
            let cid=ctx.player_manager.playerid_2_player.get(pid).unwrap()
                .client_id;
            if cid!=putter_cid {
                ctx.client_manager.get_sender(cid)
                    .send(v.clone()).await;
            }
        }
    }
    // }
    async fn putblock_in_chunk(
        ctx: &mut game::Game,
        pb:Box<PutBlock>,puttercid:ClientId){
        //根据方块坐标获取partserver
        let ck
            =conv::point3i_2_chunkkey2(pb.x,pb.y,pb.z);
        let chunk=ctx.chunk_get_mut(&ck).await;
        chunk.set_block_at(
            conv::chunkpos_of_worldpos(
                Point3i::new(pb.x,pb.y,pb.z))
            ,pb.block_id as u8);

        notifyinterest_putblock(ctx,puttercid,pb).await;
        // let chunk=
    }

    async fn put_block_in_ps(
        ctx: &mut game::Game,
        cid: game::ClientId, pb: PutBlock) {
        let task_id=ctx.async_task_manager.add_task(
            AsyncTask::EPutBlockInPs(cid,pb.get_operation_id(),Box::new(pb.clone()))
        );

        //根据方块坐标获取partserver
        let ck
            =conv::point3i_2_chunkkey2(pb.x,pb.y,pb.z);
        let sender=part_server_sync::
        get_part_server_sender_of_chunk(ctx,ck).unwrap();

        //制作pack
        let mut pack =protos::common::Cmd_PutBlockInPs::new();
        pack.task_id=task_id;
        pack.put_block=SingularPtrField::some(pb);

        sender.serialize_and_send(pack,PackIds::ECmd_PutBlockInPs)
            .await;
            // //转vec发送
            // let vec=net_pack_convert::pack_to_bytes(
            //     pack,PackIds::ECmd_PutBlockInPs);
            // sender.send(vec).await;

    }

    pub async fn put_block_in_ps_rpl(ctx: &mut game::Game, rpl: Rpl_PutBlockInPs) {

        let task=ctx.async_task_manager.finish_task(rpl.task_id);
        match task{
            AsyncTask::EPutBlockInPs(cid,opid,pb) => {
                if rpl.put_succ==0 {
                    //放置失败
                    game::operation::send_player_operation_result(
                        ctx,cid,opid,
                        OperationResult::Fail
                    ).await
                    // send::to_player::operation_failed(ctx,cid,opid).await;
                }else{
                    //放置成功
                    putblock_in_chunk(ctx,pb,cid).await;
                    // println!("put succ");
                    game::operation::send_player_operation_result(
                        ctx,cid,opid,
                        OperationResult::Succ
                    ).await;
                }
            }
            _ => {
                println!("err:task not found");
            }
        }
    }

    pub async fn handle_PutBlock(ctx:&mut game::Game,ct:ClientType,cid:ClientId,pb:PutBlock) {
        if ct==ClientType_Player{
            //todo 加入一些权限检测啥的

            //来自player，需要请求partserver putblock
            put_block_in_ps(ctx,cid,pb).await;
        }else{
            println!("{} handle_PutBlock",log::get_info_type_str(log::InfoType::WrongMsgSource))
        }
    }
}

pub async fn handle_pack(context:&mut Game,msg:& ClientMsg) -> bool {
    return match &msg.msg_enum {
        MsgEnum::PutBlock(pb)=>{
            put_block::handle_PutBlock(
                context,
                msg.client_type,msg.client_id,
                pb.clone()
            ).await;
            false
        }
        MsgEnum::Rpl_PutBlockInPs(rpl)=>{
            if msg.client_type==ClientType_GameServer{
                put_block::put_block_in_ps_rpl(
                    context,rpl.clone()).await;
            }
            false
        }
        _ => {
            true
        }
    }
}