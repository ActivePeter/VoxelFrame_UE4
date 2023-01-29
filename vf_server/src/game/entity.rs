use crate::*;
use crate::game::{Game, player, ClientId};
use crate::protos::common::{EntityType, ClientType};
use std::collections::LinkedList;
use crate::net_pack::{pack_to_bytes, PackIds, pack_to_bytes2, MsgEnum, PackPriority};
use protobuf::Clear;
use crate::game::player::{PlayerId, Player};
use crate::net::ClientMsg;
use crate::protos::common::ClientType::ClientType_GameServer;
use crate::async_task::AsyncTask;
use crate::base_type::point3f_new2;
use game::chunk;
use crate::game::chunk::{chunks_add_be_interested, chunks_remove_be_interested, ChunkKey};

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
        game.entities_mut().entry(*game.next_entity_id_ref())
            .or_insert(
                EntityData {
                    entity_id: *game.next_entity_id_ref(),
                    position: base_type::point3f_new2(0.0,300.0,0.0),
                    entity_type: EntityType::T_Player,
                }
            );
    // let entity=

    let entity_id = *game.next_entity_id_ref();
    *game.next_entity_id_mut() += 1;

    return entity_id;
}

// pub fn entity_spawn_cont(game: &mut Game) {}
pub async fn entity_move_change_chunk(ctx:&Game,from:ChunkKey,to:ChunkKey,eid:EntityId,isplayer:bool){

    let pid=ctx._player_manager.borrow().get_player_by_eid(eid).player_id;
    {
        let chunk = ctx.chunk_get_mut_loaded(&from).await;
        chunk.del_entity(eid);

        if isplayer {
            chunk.del_player_only(pid);
        }
    }
    {
        let chunk = ctx.chunk_get_mut_loaded(&to).await;
        chunk.entities.push_back(eid);
        if isplayer {
            chunk.players.push_back(pid);
        }
    }
}
// //update entity pos and send to interested players
// pub async fn update_entity_pos(game: &mut Game, epu: protos::common::EntityPosUpdate,
//                                except_enabled:bool,except_player_id:PlayerId) {
//
//
// }

pub async fn spawn_entity_in_ps(
    game:&mut Game,
    epos:protobuf::SingularPtrField<protos::common::EntityPos>){
    let task_id=game._async_task_manager.borrow_mut().add_task(AsyncTask::ESpawnEntityInPs);
    let ck;
    {
        let epos_ref=epos.as_ref().unwrap();
        ck=conv::point3f_2_chunkkey(&point3f_new2(epos_ref.x,epos_ref.y,epos_ref.z));
    }
    let sender=part_server_sync::get_part_server_sender_of_chunk(game,ck).await.unwrap();
    let mut cmd=protos::common::Cmd_SpawnEntityInPs::new();
    cmd.task_id=task_id;
    println!("spawn_entity_in_ps_cmd{}",cmd.task_id);
    cmd.entity_pos=epos;
    // println!("send ECmd_SpawnEntityInPs");
    sender.send(net_pack::pack_to_bytes(
        cmd,
        PackIds::ECmd_SpawnEntityInPs),PackIds::ECmd_SpawnEntityInPs.default_priority()).await;
    // sender.send()
}
pub fn pack_serialize_remove_entity(eid:EntityId, remove_type:protos::common::RemoveEntityType) -> (Vec<u8>, PackPriority) {
    let mut pack =protos::common::RemoveEntity::new();
    pack.entity_id=eid;
    pack.remove_type=remove_type;
    (net_pack::pack_to_bytes(pack, PackIds::ERemoveEntity),PackIds::ERemoveEntity.default_priority())
}
pub struct EntityOperator<'a>{
    ctx:&'a Game
}
impl EntityOperator<'_> {
    pub fn new(ctx:&Game) -> EntityOperator {
        EntityOperator{
            ctx,
        }
    }
    fn remove_entity_indata(&mut self,eid:EntityId){
        self.ctx.entities_mut().remove(&eid);
    }
    // async fn send2all_remove_entity_in_game(&mut self,eid)
    pub async fn remove_player_entity_in_game(&mut self,p:&Player){
        println!("remove_player_entity_in_game");
        //2.移除chunk中的entity
        chunk::ChunkOperator::new(self.ctx)
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