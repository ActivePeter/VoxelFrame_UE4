// pub mod async_task;
pub mod entity;
pub mod block;

use std::collections::HashMap;
use crate::{protos, game_chunk, async_task, game};
use crate::game::{Game, ClientId, ClientOperationId};
use crate::conv;
use crate::base_type::point3f_new2;
use crate::part_server_sync;
use crate::net_pack_convert;
use crate::net_pack_convert::{PackIds, MsgEnum};
use crate::game_player;
use crate::game_entity;
use crate::net_pack_convert::MsgEnum::EntityPos;
use crate::net::ClientMsg;
use crate::protos::common::ClientType::ClientType_GameServer;
use crate::log;


pub type AsyncTaskId=u32;
pub struct AsyncTaskManager{
    id2wait_rpl_task:HashMap<AsyncTaskId,AsyncTask>,
    next_id:AsyncTaskId,
}
impl AsyncTaskManager{
    pub fn create() -> AsyncTaskManager {
        AsyncTaskManager{
            id2wait_rpl_task: Default::default(),
            next_id:0,
        }
    }
    pub fn add_task(&mut self,task:AsyncTask)->AsyncTaskId{
        self.id2wait_rpl_task.insert(self.next_id,task);
        self.next_id+=1;
        return self.next_id-1;
    }
    pub fn finish_task(&mut self, task_id:AsyncTaskId) -> AsyncTask {
        match self.id2wait_rpl_task.remove(&task_id) {
            Some(task) => task,
            None => {
                println!("{} finish_task: task_id not found",
                    log::get_info_type_str(log::InfoType::FinishAsyncTaskFailed)
                );
                AsyncTask::FinishTaskFailed },
        }
        // return self.id2wait_rpl_task.remove(&task_id);
    }
}
pub enum AsyncTask{
    ESpawnEntityInPs,
    EPutBlockInPs(ClientId,ClientOperationId),

    FinishTaskFailed,
}

pub async fn match_client_rpl_msg(ctx:&mut game::Game,common_msg: ClientMsg){

    match common_msg.msg_enum{
        MsgEnum::Rpl_SpawnEntityInPs(rpl) => {
            if common_msg.client_type==ClientType_GameServer{
                async_task::entity::spawn_entity_in_ps_rpl(
                    ctx,rpl).await;
            }
        }
        MsgEnum::Rpl_PutBlockInPs(rpl)=>{
            if common_msg.client_type==ClientType_GameServer{
                async_task::block::put_block_in_ps_rpl(
                    ctx,rpl).await;
            }
        }
        _ => {}
    }
}

// pub use crate::async_task;