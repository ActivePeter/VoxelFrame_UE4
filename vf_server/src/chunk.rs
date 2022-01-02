use crate::*;
use std::collections::LinkedList;
use crate::player::PlayerId;

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
    pub players: LinkedList<player::PlayerId>,
    pub entities: LinkedList<entity::EntityId>,
    pub be_interested_by: LinkedList<player::PlayerId>,
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
            be_interested_by: Default::default()
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