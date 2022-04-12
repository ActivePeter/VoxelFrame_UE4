use crate::game;
use crate::protos::common::{PutBlock, Rpl_PutBlockInPs};
use crate::async_task::AsyncTask;
use crate::send;
use std::mem::take;

pub async fn put_block_in_ps(
    ctx: &mut game::Game,
    cid: game::ClientId, pb: PutBlock) {
    let task_id=ctx.async_task_manager.add_task(
        AsyncTask::EPutBlockInPs(cid,pb.get_operation_id())
    );
    send::to_part_server::put_block(ctx,task_id,pb).await;
}

pub async fn put_block_in_ps_rpl(ctx: &mut game::Game, rpl: Rpl_PutBlockInPs) {

    let task=ctx.async_task_manager.finish_task(rpl.task_id);
    match task{
        AsyncTask::EPutBlockInPs(cid,opid) => {
            if rpl.put_succ==0 {
                //放置失败
                send::to_player::operation_failed(ctx,cid,opid).await;
            }else{
                //放置成功
                // println!("put succ");
                send::to_player::operation_succ(ctx,cid,opid).await;
            }
        }
        _ => {
            println!("err:task not found");
        }
    }
}