use std::collections::HashMap;
use crate::{protos, game_chunk};
use crate::game::Game;
use crate::conv;
use crate::base_type::point3f_new2;
use crate::part_server_sync;
use crate::net_pack_convert;
use crate::net_pack_convert::PackIds;
use crate::game_player;

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
        return self.id2wait_rpl_task.remove(&task_id).unwrap();
    }
}
pub enum AsyncTask{
    ESpawnEntityInPs,
}

pub async fn spawn_entity_in_ps(
    game:&mut Game,
    epos:protobuf::SingularPtrField<protos::common::EntityPos>){
    let task_id=game.async_task_manager
        .add_task(AsyncTask::ESpawnEntityInPs);
    let ck;
    {
        let epos_ref=epos.as_ref().unwrap();
        ck=conv::point3f_2_chunkkey(&point3f_new2(epos_ref.x,epos_ref.y,epos_ref.z));
    }
    let sender=part_server_sync::get_part_server_sender_of_chunk(game,ck).unwrap();
    let mut cmd=protos::common::Cmd_SpawnEntityInPs::new();
    cmd.task_id=task_id;
    cmd.entity_pos=epos;
    let vec= net_pack_convert::pack_to_bytes(cmd,PackIds::ECmd_SpawnEntityInPs);
    // println!("send ECmd_SpawnEntityInPs");
    sender.send(vec).await;
    // sender.send()
}
pub async fn spawn_entity_in_ps_rpl(game:&mut Game,rpl:protos::common::Rpl_SpawnEntityInPs){
    let a=game.async_task_manager.finish_task(rpl.task_id);
    let entitypos=rpl.entity_pos.unwrap();
    if entitypos.t==protos::common::EntityType::T_Player {
        //player 生成完成
        //  1.设置player位置
        game.entity_get_mut(&entitypos.entity_id).unwrap()
            .set_positon(entitypos.x,entitypos.y,entitypos.z);
        //  2.发送player所有数据
        let pid=game.player_manager.get_player_by_eid(entitypos.entity_id);
        //todo: 出生点区块坐标可能变化，还未做相应的处理
        game_player::after_player_data_all_load(
            game,pid.player_id,pid.entity_id).await;
    }
}