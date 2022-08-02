use crate::*;
use crate::game::{Game, game_player, ClientId};
use crate::protos::common::{EntityType, ClientType};
use std::collections::LinkedList;
use crate::net_pack_convert::{pack_to_bytes, PackIds, pack_to_bytes2, MsgEnum};
use protobuf::Clear;
use crate::game::game_player::{PlayerId, Player};
use crate::net::ClientMsg;
use crate::protos::common::ClientType::ClientType_GameServer;
use crate::async_task::AsyncTask;
use crate::base_type::point3f_new2;
use game::game_chunk;
use crate::game::game_chunk::{chunks_add_be_interested, chunks_remove_be_interested, ChunkKey};

//type
pub type EntityId = u32;

pub struct EntityData {
    pub entity_id: EntityId,
    pub position: base_type::Point3f,
    pub entity_type: EntityType,
}

impl EntityData {
    pub fn hello(&mut self) {}
    pub fn set_positon(&mut self, x: f32, y: f32, z: f32) {
        self.position[0] = x;
        self.position[1] = y;
        self.position[2] = z;
    }
}

//entity related operations
pub fn entity_spawn(game: &mut Game) -> u32 {
    let entity =
        game.entities.entry(game.entity_cnt)
            .or_insert(
                EntityData {
                    entity_id: game.entity_cnt,
                    position: base_type::point3f_new(),
                    entity_type: EntityType::T_Player,
                }
            );
    // let entity=

    let entity_id = game.entity_cnt;
    game.entity_cnt += 1;

    return entity_id;
}

// pub fn entity_spawn_cont(game: &mut Game) {}
pub async fn entity_move_change_chunk(ctx:&mut Game,from:ChunkKey,to:ChunkKey,eid:EntityId,isplayer:bool){

    let pid=ctx.player_manager.get_player_by_eid(eid).player_id;
    {
        let chunk = ctx.chunk_get_mut(&from).await;
        chunk.del_entity(eid);

        if isplayer {
            chunk.del_player_only(pid);
        }
    }
    {
        let chunk = ctx.chunk_get_mut(&to).await;
        chunk.entities.push_back(eid);
        if isplayer {
            chunk.players.push_back(pid);
        }
    }
}
//update entity pos and send to interested players
pub async fn update_entity_pos(game: &mut Game, epu: protos::common::EntityPosUpdate,
                               except_enabled:bool,except_player_id:PlayerId) {
    // println!("update_entity_pos");
    let mut changed_chunks = LinkedList::new();
    //1.一个chunk的entity变更一起算
    for a in epu.entity_pos {
        match game.entities.get_mut(&a.entity_id) {
            None => {
                println!("update_entity_pos no entity matched {}",a.entity_id);
            }
            Some(game_entity) => {
                let ck1 = conv::point3f_2_chunkkey(&game_entity.position);
                // println!("update_entity_pos matched {}",a.entity_id);
                game_entity.set_positon(a.x, a.y, a.z);
                let ck = conv::point3f_2_chunkkey(&game_entity.position);
                let isplayer=game_entity.entity_type==EntityType::T_Player;
                if isplayer {
                    if ck1!=ck {
                        let pid=game.player_manager.get_player_by_eid(a.entity_id).player_id;
                        game_chunk::chunks_remove_be_interested(game, pid, ck1)
                            .await;
                        game_chunk::chunks_add_be_interested2(game,pid,ck)
                            .await;
                    }
                }
                //实体区块变更
                if ck1!=ck {
                    entity_move_change_chunk(
                        game,ck1,ck,a.entity_id,isplayer).await;
                    let chunk = game.chunk_get_mut(&ck1).await;
                    chunk.entity_update.entity_pos.push(a.clone());
                    changed_chunks.push_back(ck1);
                }
                let mut chunk = game.chunk_get_mut(&ck).await;
                if(chunk.entity_update.entity_pos.len()>0){
                    println!("before_push chunk entity_update len {}",chunk.entity_update.entity_pos.len());
                }
                chunk.entity_update.entity_pos.push(a);
                changed_chunks.push_back(ck);
            }
        }
    }
    //2.遍历变更的区块，
    for ck in &changed_chunks {
        let chunk =game.chunks.get_mut(ck).unwrap();
            //game.chunk_get(ck).unwrap();// (&ck).await;
        for p in &chunk.be_interested_by {
            //对变更区块感兴趣的player p
            if except_enabled && p==&except_player_id{
                continue;
            }
            if(except_enabled){
                for eu in &chunk.entity_update.entity_pos{
                    print!("{} ",eu.entity_id);
                }
                println!("send to p{}",p);
            }
            // for eu in &chunk.entity_update.entity_pos{
            //     print!("{} ",eu.entity_id);
            // }
            // println!("send to p{}",p);
            let cid = game.player_manager.playerid_2_player.get(&p).unwrap().client_id;
            // let a=chunk.entity_update.borrow();
            let bytes=pack_to_bytes2(
                &chunk.entity_update, PackIds::EEntityPosUpdate);
            game.client_manager.id2client.get(&cid).unwrap()
                .sender.send(bytes).await;
            // println!("sent ck entity add to player");
        }
        chunk.entity_update.clear();
    }

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
    println!("spawn_entity_in_ps_cmd{}",cmd.task_id);
    cmd.entity_pos=epos;
    let vec=
        net_pack_convert::pack_to_bytes(
            cmd,
            PackIds::ECmd_SpawnEntityInPs);
    // println!("send ECmd_SpawnEntityInPs");
    sender.send(vec).await;
    // sender.send()
}
async fn handle_spawn_entity_in_ps_rpl (game:&mut Game,rpl:protos::common::Rpl_SpawnEntityInPs){
    println!("spawn_entity_in_ps_rpl{}",rpl.task_id);
    let a=game.async_task_manager.finish_task(rpl.task_id);
    match a {
        AsyncTask::FinishTaskFailed => {
            return;
        }
        _=>{}
    }
    let entitypos=rpl.entity_pos.unwrap();
    // if entitypos.t==protos::common::EntityType::T_Player {
        //player 生成完成
        if entitypos.t==protos::common::EntityType::T_Player {
            let pid=game.player_manager.get_player_by_eid(entitypos.entity_id).clone();

            //  3.update chunk
            let mut epu =protos::common::EntityPosUpdate::new();
            epu.mut_entity_pos().push(entitypos);
            // println!("epu cnt {}",epu.entity_pos.len());
            update_entity_pos(game,epu,
                              true,pid.player_id).await;


            //todo: 出生点区块坐标可能变化，还未做相应的处理
            game_player::send_player_basic_and_chunk(
                game,pid.player_id,pid.entity_id).await;

        }else{
            let mut epu =protos::common::EntityPosUpdate::new();
            epu.mut_entity_pos().push(entitypos);
            update_entity_pos(game,epu,
                                           false,0).await;
        }

    // }

}

pub async fn handle_pack(context:&mut Game,msg:& ClientMsg) -> bool {

    // println!("handle_pack entity");
    return match &msg.msg_enum {
        MsgEnum::EntityPosUpdate(epu)=>{

            println!("EntityPosUpdate From PartServer");
            if msg.client_type==ClientType_GameServer{
                update_entity_pos(context,epu.clone(),
                                  false,0).await;
            }

            false
        }
        MsgEnum::Rpl_SpawnEntityInPs(rpl) => {
            // println!("Rpl_SpawnEntityInPs");
            if msg.client_type==ClientType_GameServer{
                handle_spawn_entity_in_ps_rpl(
                    context,rpl.clone()).await;
            }
            false
        }
        _ => {
            true
        }
    }
}
pub fn pack_serialize_remove_entity(eid:EntityId,remove_type:protos::common::RemoveEntityType) -> Vec<u8> {
    let mut pack =protos::common::RemoveEntity::new();
    pack.entity_id=eid;
    pack.remove_type=remove_type;
    net_pack_convert::pack_to_bytes(pack,PackIds::ERemoveEntity)
}
pub struct EntityOperator<'a>{
    ctx:&'a mut Game
}
impl EntityOperator<'_> {
    pub fn new(ctx:& mut Game) -> EntityOperator {
        EntityOperator{
            ctx,
        }
    }
    fn remove_entity_indata(&mut self,eid:EntityId){
        self.ctx.entities.remove(&eid);
    }
    // async fn send2all_remove_entity_in_game(&mut self,eid)
    pub async fn remove_player_entity_in_game(&mut self,p:&Player){
        println!("remove_player_entity_in_game");
        //2.移除chunk中的entity
        game_chunk::ChunkOperator::new(self.ctx)
            .remove_player(p).await;
        //1.移除数据中的entity
        self.remove_entity_indata(p.entity_id);
    }
    pub async fn remove_notplayer_entity_in_game(&mut self,eid:EntityId){
        //1.通知所有interested的client全都移除
        
        //2.移除数据中的entity
        self.remove_entity_indata(eid);
        // self.ctx.player_manager.get_player_mut()
    }
}