use crate::*;
use std::collections::{LinkedList, HashMap};
use game::{Game, ClientId, player, chunk,chunk_terrain};
use crate::conv::point3f_2_chunkkey;
use crate::game::entity::{EntityId, EntityData};
use crate::game::player::{PlayerId, Player};
use crate::net_pack_convert::PackIds;
use crate::protos::common::RemoveEntityType;
use crate::net::ClientSender;
use crate::base_type::Point3i;
use std::collections::hash_map::RandomState;


pub const VF_CHUNK_LOAD_RADIUS: i32 = (4);
pub const VF_CHUNK_WIDTH: i32 = (32);
pub const VF_CHUNK_SIZE: i32 = (VF_CHUNK_WIDTH * VF_CHUNK_WIDTH * VF_CHUNK_WIDTH);

#[derive(PartialEq, Eq, Hash, Clone, Copy)]
pub struct ChunkKey {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}
pub enum Side{
    YPlus1,
    YMinus1,
    XPlus1,
    XMinus1,
    ZPlus1,
    ZMinus1,
}
impl ChunkKey{
    pub fn plus(self, ck: ChunkKey) -> ChunkKey {
        return ChunkKey {
            x: self.x + ck.x,
            y: self.y + ck.y,
            z: self.z + ck.z,
        };
    }
    pub fn new(x:i32,y:i32,z:i32)->ChunkKey{
        ChunkKey{
            x,
            y,
            z
        }
    }
    pub fn is_in_range(&self) -> bool {
        return self.x * self.x + self.y * self.y + self.z * self.z < VF_CHUNK_LOAD_RADIUS * VF_CHUNK_LOAD_RADIUS;
    }
    pub fn get_beside(&self, side:Side) -> ChunkKey {
        let mut ck =self.clone();
        match side{
            Side::YPlus1 => {ck.y+=1;}
            Side::YMinus1 => {ck.y-=1;}
            Side::XPlus1 => {ck.x+=1;}
            Side::XMinus1 => {ck.x-=1;}
            Side::ZPlus1 => {ck.z+=1;}
            Side::ZMinus1 => {ck.z-=1;}
        }
        ck
    }
    pub fn get_world_pos(&self)->glam::IVec3{
        glam::IVec3::new(self.x*VF_CHUNK_WIDTH,self.y*VF_CHUNK_WIDTH,self.z*VF_CHUNK_WIDTH)
    }
}
pub struct Chunk {
    pub chunk_key: ChunkKey,
    pub chunk_data: Vec<u8>,
    pub players: LinkedList<player::PlayerId>,
    pub entities: LinkedList<EntityId>,
    pub entity_update: protos::common::EntityPosUpdate,
    pub be_interested_by: LinkedList<player::PlayerId>,
    pub part_server_cid:Option<ClientId>,
}

impl Chunk{
    pub fn new_and_load(map: &mut HashMap<ChunkKey, Chunk>, key: &ChunkKey) -> Chunk {
        let mut v = Vec::new();
        v.resize(VF_CHUNK_SIZE as usize, 0);

        let mut chunk =Chunk {
            chunk_data: v,
            chunk_key: key.clone(),
            players: Default::default(),
            entities: Default::default(),
            be_interested_by: Default::default(),
            part_server_cid:None,
            entity_update: Default::default()
        };
        chunk_terrain::init_chunk_data(map,&mut chunk);
        // chunk.load();
        return chunk;
    }
    pub fn load(&mut self) {
        for x in 0..VF_CHUNK_WIDTH {
            for y in 0..VF_CHUNK_WIDTH {
                for z in 0..VF_CHUNK_WIDTH {
                    if y < VF_CHUNK_WIDTH / 2 {
                        self.chunk_data[conv::p_int_2_index_in_chunk(x,y,z)] = 1;
                    } else {
                        self.chunk_data[conv::p_int_2_index_in_chunk(x,y,z)] = 0;
                    }
                }
            }
        }
    }
    pub fn set_block_at(&mut self,p:Point3i,block:u8){
        if conv::p_int_2_index_in_chunk(p.x, p.y, p.z)<self.chunk_data.len() {
            self.chunk_data[conv::p_int_2_index_in_chunk(p.x,p.y,p.z)]=block;
        }
    }
    pub fn add_be_interested_by(&mut self, pid:PlayerId){
        if !self.be_interested_by.contains(&pid) {
            self.be_interested_by.push_back(pid);
        }
    }
    pub fn del_be_interested_by(&mut self, pid:PlayerId){
        // let index=self.be_interested_by.(&pid);
        let mut index:usize =0 ;
        let mut found=false;
        for p in self.be_interested_by.iter() {
            if *p==pid{
                found=true;
                break;
            }
           index+=1;
        }
        if found {
            self.be_interested_by.remove(index);
            // self.be_interested_by.push_back(pid);
        }
    }
    pub fn del_entity(&mut self,eid:EntityId){
        let mut index:usize =0 ;
        let mut found=false;
        for p in self.entities.iter() {
            if *p==eid{
                found=true;
                break;
            }
            index+=1;
        }
        if found {
            self.entities.remove(index);
            // self.be_interested_by.push_back(pid);
        }else{
            println!("del entity not found");
        }
    }
    pub fn del_player_only(&mut self,pid:PlayerId){
        let mut index:usize =0 ;
        let mut found=false;
        for p in self.players.iter() {
            if *p==pid{
                found=true;
                break;
            }
            index+=1;
        }
        if found {
            self.players.remove(index);
            // self.be_interested_by.push_back(pid);
        }else{
            println!("del player2 not found");
        }
    }
    pub fn del_player(&mut self,del_entity:bool,p1:&Player){
        if(del_entity){
            self.del_entity(p1.entity_id);
        }
        let mut index:usize =0 ;
        let mut found=false;
        for p in self.players.iter() {
            if *p==p1.player_id{
                found=true;
                break;
            }
            index+=1;
        }
        if found {
            self.players.remove(index);
            // self.be_interested_by.push_back(pid);
        }else{
            println!("del player not found");
        }
    }
}
// pub struct ChunkManager{
//     pub chunks: HashMap<chunk::ChunkKey, chunk::Chunk>,
// }
// impl ChunkManager{
//
// }
#[macro_export]
macro_rules! iter_relative_chunk_key_in_interest_range {
  ($chunk_name:ident ,$callback:block) => {
    for x in -chunk::VF_CHUNK_LOAD_RADIUS..chunk::VF_CHUNK_LOAD_RADIUS {
        for y in -chunk::VF_CHUNK_LOAD_RADIUS..chunk::VF_CHUNK_LOAD_RADIUS {
            for z in -chunk::VF_CHUNK_LOAD_RADIUS..chunk::VF_CHUNK_LOAD_RADIUS {
                let $chunk_name = chunk::ChunkKey { x, y, z };
                if $chunk_name.is_in_range() {
                    // $callback(ck);
                    $callback
                }
            }
        }
    }
  }
}
//chunk related operations
//chunk 加入玩家
pub async fn chunk_add_player(game:&mut Game,
                    playerid: PlayerId,
                    player_entity_id: EntityId,
) {
    let entity = game.entity_get(&player_entity_id).unwrap();
    //1.根据位置计算chunk_key
    let chunk_key = point3f_2_chunkkey(&entity.position);
    //2.获取区块
    let chunk = game.chunk_get_mut(&chunk_key).await;
    //3.entity
    chunk.entities.push_back(player_entity_id);
    //4.player
    chunk.players.push_back(playerid);
}
pub async fn chunks_remove_be_interested(
    game:&mut Game,
    playerid: PlayerId,
    center_ck:ChunkKey
) {
    let p_ck = center_ck;

    iter_relative_chunk_key_in_interest_range!(
            relate_ck,
            {
                let ck=p_ck.plus(relate_ck);
                let chunk=game.chunk_get_mut(&ck).await;
                chunk.del_be_interested_by(playerid);
            }
        )
}
pub async fn chunks_add_be_interested2(
    game:&mut Game,
    playerid: PlayerId,
    center_ck:ChunkKey
) {
    let p_ck = center_ck;

    iter_relative_chunk_key_in_interest_range!(
            relate_ck,
            {
                let ck=p_ck.plus(relate_ck);
                let chunk=game.chunk_get_mut(&ck).await;
                chunk.add_be_interested_by(playerid);
            }
        )
}
pub async fn chunks_add_be_interested(
    game:&mut Game,
    playerid: PlayerId,
    player_entity_id: EntityId,
) {
    let entity = game.entity_get(&player_entity_id).unwrap();
    let p_ck = point3f_2_chunkkey(&entity.position);

    iter_relative_chunk_key_in_interest_range!(
            relate_ck,
            {
                let ck=p_ck.plus(relate_ck);
                let chunk=game.chunk_get_mut(&ck).await;
                chunk.add_be_interested_by(playerid);
            }
        )
}

pub struct ChunkOperator<'a>{
    ctx:&'a mut Game
}
impl ChunkOperator<'_> {
    pub fn new(ctx:& mut Game) -> ChunkOperator {
        ChunkOperator{
            ctx,
        }
    }
    pub async fn chunks_remove_interested(&mut self,p:&Player){
        let entity=self.ctx.entity_get(&p.entity_id).unwrap();

        let p_ck = point3f_2_chunkkey(&entity.position);
        iter_relative_chunk_key_in_interest_range!(
            relate_ck,
            {
                let ck=p_ck.plus(relate_ck);
                let chunk=self.ctx.chunk_get_mut(&ck).await;
                chunk.del_be_interested_by(p.player_id);
            }
        )
    }

    pub async fn remove_player(&mut self,p:&Player){
        println!("chunk remove_player");
        {//1.移除原本被其感兴趣的区块中的感兴趣信息
            self.chunks_remove_interested(p).await;
        }
        let entity = self.ctx.entity_get(&p.entity_id).unwrap();
        //2.根据位置计算chunk_key
        let chunk_key = point3f_2_chunkkey(&entity.position);
        {
            //3.获取区块
            let chunk = self.ctx.chunk_get_mut(&chunk_key).await;
            //4.移除数据
            chunk.del_player(true, p);
        }
        let chunk = self.ctx.chunk_get(&chunk_key).unwrap();

        //5.给感兴趣的单位发送
        let v=game::entity::pack_serialize_remove_entity(
            p.entity_id,RemoveEntityType::disco
        );
        for pid in &chunk.be_interested_by{
            let _p=self.ctx.player_manager.playerid_2_player.get(pid).unwrap();
            // self.ctx.client_manager.get_player_sender(p)
            //     .serialize_and_send(protos::common::Remove,PackIds);
            self.ctx.client_manager.get_player_sender(_p)
                .send(v.clone()).await
        }
        //6.给partsever发送
        let ps=part_server_sync::get_part_server_sender_of_chunk(
            self.ctx,chunk_key
        );
        match ps{
            None => {}
            Some(pss) => {
                pss.send(v.clone()).await;
            }
        }
        // self.ctx.client_manager.get_sender()
    }
    // async fn send_change_2_interested_player(){
    //
    // }
}