// pub mod async_task;
pub mod entity;
// pub mod block;

use std::collections::HashMap;
use crate::{protos, async_task, game};
use crate::game::{Game, ClientId, ClientOperationId};
use crate::conv;
use crate::base_type::point3f_new2;
use crate::part_server_sync;
use crate::net_pack;
use crate::net_pack::{PackIds, MsgEnum};
use crate::net_pack::MsgEnum::EntityPos;
use crate::net::ClientMsg;
use crate::protos::common::ClientType::ClientType_GameServer;
use crate::log;
use crate::protos::common::PutBlock;


pub type AsyncTaskId=u32;
pub struct AsyncTaskManager{
    id2wait_rpl_task:HashMap<AsyncTaskId,AsyncTask>,
    next_id:AsyncTaskId,
}
impl AsyncTaskManager{
    pub fn new() -> AsyncTaskManager {
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
    EPutBlockInPs(ClientId,ClientOperationId,Box<PutBlock>),

    FinishTaskFailed,
}

// pub async fn match_client_rpl_msg(ctx:&mut game::Game,common_msg: ClientMsg){
//
//     match common_msg.msg_enum{
//
//         _ => {}
//     }
// }

// pub use crate::async_task;