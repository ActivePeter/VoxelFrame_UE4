use crate::game;
use crate::protos::common::{PutBlock, Rpl_PutBlockInPs};
use crate::async_task::AsyncTask;
use crate::send;
use std::mem::take;

pub async fn put_block_in_ps(
    ctx: &mut game::Game,
    cid: game::ClientId, pb: PutBlock) {
    let task_id=ctx.async_task_manager.add_task(
        AsyncTask::EPutBlockInPs(cid)
    );
    send::to_part_server::put_block(ctx,task_id,cid,pb).await;
}

pub async fn put_block_in_ps_rpl(ctx: &mut game::Game, rpl: Rpl_PutBlockInPs) {
    let task=ctx.async_task_manager.finish_task(rpl.task_id);
    match task{
        AsyncTask::EPutBlockInPs(cid) => {
            if rpl.put_succ==0 {
                //放置失败

            }
        }
        _ => {}
    }
}