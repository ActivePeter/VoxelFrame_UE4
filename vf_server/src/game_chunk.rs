use crate::*;
use std::collections::{LinkedList, HashMap};
use crate::game_player::PlayerId;
use crate::game::{Game, ClientId};
use crate::conv::point3f_2_chunkkey;
use crate::game_entity::EntityId;

pub const VF_CHUNK_LOAD_RADIUS: i32 = (4);
pub const VF_CHUNK_WIDTH: i32 = (32);
pub const VF_CHUNK_SIZE: i32 = (VF_CHUNK_WIDTH * VF_CHUNK_WIDTH * VF_CHUNK_WIDTH);

#[derive(PartialEq, Eq, Hash, Clone, Copy)]
pub struct ChunkKey {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}
impl ChunkKey{
    pub fn plus(self, ck: ChunkKey) -> ChunkKey {
        return ChunkKey {
            x: self.x + ck.x,
            y: self.y + ck.y,
            z: self.z + ck.z,
        };
    }
    pub fn is_in_range(&self) -> bool {
        return self.x * self.x + self.y * self.y + self.z * self.z < VF_CHUNK_LOAD_RADIUS * VF_CHUNK_LOAD_RADIUS;
    }
}
pub struct Chunk {
    pub chunk_key: ChunkKey,
    pub chunk_data: Vec<u8>,
    pub players: LinkedList<game_player::PlayerId>,
    pub entities: LinkedList<EntityId>,
    pub entity_update: protos::common::EntityPosUpdate,
    pub be_interested_by: LinkedList<game_player::PlayerId>,
    pub part_server_cid:Option<ClientId>,
}

impl Chunk{
    pub fn new_and_load(key: &ChunkKey) -> Chunk {
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
        chunk.load();

        return chunk;
    }
    pub fn load(&mut self) {
        for x in 0..VF_CHUNK_WIDTH {
            for y in 0..VF_CHUNK_WIDTH {
                for z in 0..VF_CHUNK_WIDTH {
                    if (y < VF_CHUNK_WIDTH / 2) {
                        self.chunk_data[conv::p_int_2_index_in_chunk(x,y,z)] = 1;
                    } else {
                        self.chunk_data[conv::p_int_2_index_in_chunk(x,y,z)] = 0;
                    }
                }
            }
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
}
// pub struct ChunkManager{
//     pub chunks: HashMap<game_chunk::ChunkKey, game_chunk::Chunk>,
// }
// impl ChunkManager{
//
// }
#[macro_export]
macro_rules! iter_relative_chunk_key_in_interest_range {
  ($chunk_name:ident ,$callback:block) => {
    for x in -game_chunk::VF_CHUNK_LOAD_RADIUS..game_chunk::VF_CHUNK_LOAD_RADIUS {
        for y in -game_chunk::VF_CHUNK_LOAD_RADIUS..game_chunk::VF_CHUNK_LOAD_RADIUS {
            for z in -game_chunk::VF_CHUNK_LOAD_RADIUS..game_chunk::VF_CHUNK_LOAD_RADIUS {
                let $chunk_name = game_chunk::ChunkKey { x, y, z };
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
