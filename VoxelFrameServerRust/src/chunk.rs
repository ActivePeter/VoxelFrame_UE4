use crate::base::*;
use std::iter::Map;
use std::cell::RefCell;


pub const VF_CHUNK_LOAD_RADIUS: i32 = (4);
pub const VF_CHUNK_WIDTH: i32 = (32);
pub const VF_CHUNK_SIZE: i32 = (VF_CHUNK_WIDTH * VF_CHUNK_WIDTH * VF_CHUNK_WIDTH);

/////////////////////////////////////////////////////////////////
pub struct ChunkManager {
    // pub game:sync::Weak<RwLock<Game>>,
    // chunk: Chunk,
    chunks: HashMap<ChunkKey, ArcRw<Chunk>>,
}

impl ChunkManager {
    pub fn new() -> ChunkManager {
        return ChunkManager {
            // game:Default::default(),
            chunks: Default::default()
        };
    }
    pub fn get_chunk_by_chunk_key(&mut self, ck: ChunkKey) -> Arc<RwLock<Chunk>> {
        // let chunk_op;
        // {
        // {
        //     let chunk_op = self.chunks.get_mut(ck);
        //     // }
        //     if let Some(chunk) = chunk_op {
        //         return chunk;
        //     }
        // }
        // // else {
        // let mut new_chunk = Chunk::new();
        // let mchunk = &mut new_chunk;
        // self.chunks.insert(*ck, new_chunk);
        let mchunk = self.chunks.entry(ck).or_insert(Arc::new(RwLock::from(Chunk::new(&ck))));
        return mchunk.clone();
    }
// pub fn setGame(&mut self, game:sync::Weak<RwLock<Game>>){
//     self.game= game;
// }
// pub fn get_chunk(&self, chunk_key: &ChunkKey) {
//     let get = self.chunks.get(chunk_key);
// }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pub struct Chunk {
    pub chunk_key: ChunkKey,
    pub chunk_data: Vec<u8>,
}

impl Chunk {
    pub fn new(key: &ChunkKey) -> Chunk {
        let mut v = Vec::new();
        v.resize(VF_CHUNK_SIZE as usize, 0);

        return Chunk { chunk_data: v, chunk_key: key.clone() };
    }

    pub async fn load(&mut self) {
        for x in 0..VF_CHUNK_WIDTH {
            for y in 0..VF_CHUNK_WIDTH {
                for z in 0..VF_CHUNK_WIDTH {
                    if (y < VF_CHUNK_WIDTH / 2) {
                        self.chunk_data[Chunk::conv_p_2_index(x, y, z)] = 1;
                    } else {
                        self.chunk_data[Chunk::conv_p_2_index(x, y, z)] = 0;
                    }
                }
            }
        }
    }

    fn conv_p_2_index(x: i32, y: i32, z: i32) -> usize {
        return (x + y * VF_CHUNK_WIDTH + z * VF_CHUNK_WIDTH * VF_CHUNK_WIDTH) as usize;
    }
}

// impl MsgDataOwner for Chunk {
//     async fn send(&mut self, p_ptr: ArcRw<Player>) {}
// }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#[derive(PartialEq, Eq, Hash, Clone, Copy)]
pub struct ChunkKey {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}


impl ChunkKey {
    fn from_world_point() {}

    pub(crate) fn is_in_range(&self) -> bool {
        return self.x * self.x + self.y * self.y + self.z * self.z < VF_CHUNK_LOAD_RADIUS * VF_CHUNK_LOAD_RADIUS;
    }

    //加载范围内的区块相对坐标
    pub fn in_range_relative<F: FnMut(ChunkKey)>(mut callback: F) {
        // pub fn in_range_relative(callback: impl FnMut(ChunkKey)) {
        for x in -VF_CHUNK_LOAD_RADIUS..VF_CHUNK_LOAD_RADIUS {
            for y in -VF_CHUNK_LOAD_RADIUS..VF_CHUNK_LOAD_RADIUS {
                for z in -VF_CHUNK_LOAD_RADIUS..VF_CHUNK_LOAD_RADIUS {
                    let ck = ChunkKey { x, y, z };
                    if ck.is_in_range() {
                        callback(ck);
                    }
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
macro_rules! chunk_key_in_range_relative {
  ($chunk_name:ident ,$callback:block) => {
    for x in -VF_CHUNK_LOAD_RADIUS..VF_CHUNK_LOAD_RADIUS {
        for y in -VF_CHUNK_LOAD_RADIUS..VF_CHUNK_LOAD_RADIUS {
            for z in -VF_CHUNK_LOAD_RADIUS..VF_CHUNK_LOAD_RADIUS {
                let $chunk_name = ChunkKey { x, y, z };
                if $chunk_name.is_in_range() {
                    // $callback(ck);
                    $callback
                }
            }
        }
    }
  }
}