use crate::async_task::AsyncTask;
use crate::{game, conv, protos};
use crate::game::operation::OperationResult;
use crate::protos::common::{Rpl_PutBlockInPs, PutBlock};
use crate::game::part_server_sync;
use protobuf::SingularPtrField;
use crate::net_pack::PackIds;
use crate::event::chunk_event;

pub(crate) async fn call(
    ctx: &mut game::Game,
    cid: game::ClientId, pb: PutBlock) {
        let task_id=ctx.async_task_man_mut().add_task(
            AsyncTask::EPutBlockInPs(cid,pb.get_operation_id(),Box::new(pb.clone()))
        );

        //根据方块坐标获取partserver
        let ck
            =conv::point3i_2_chunkkey2(pb.x,pb.y,pb.z);
        let sender=part_server_sync::
        get_part_server_sender_of_chunk(ctx,ck).await.unwrap();

        //制作pack
        let mut pack =protos::common::Cmd_PutBlockInPs::new();
        pack.task_id=task_id;
        pack.put_block=SingularPtrField::some(pb);

        sender.serialize_and_send(pack,PackIds::ECmd_PutBlockInPs)
            .await;
}

pub(crate) async fn reply(ctx: &mut game::Game, rpl: Rpl_PutBlockInPs){
    {

        let task=ctx.async_task_man_mut().finish_task(rpl.task_id);
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
                    chunk_event::block_put::call(ctx,pb,cid).await;
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
}
