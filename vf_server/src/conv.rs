use crate::*;
use crate::base_type::Point3f;
use crate::game_chunk::{ChunkKey, VF_CHUNK_SIZE, VF_CHUNK_WIDTH};

pub fn point3f_2_chunkkey(p: &Point3f)->ChunkKey{
    let mut ck = ChunkKey {
        x: 0,
        y: 0,
        z: 0,
    };
    // println!("plen  {0}", p.len());
    ck.x = p[0] as i32 / VF_CHUNK_SIZE;
    ck.y = p[1] as i32 / VF_CHUNK_SIZE;
    ck.z = p[2] as i32 / VF_CHUNK_SIZE;

    if p[0] < 0.0 {
        ck.x = ck.x - 1;
    }
    if p[1] < 0.0 {
        ck.y = ck.y - 1;
    }
    if p[2] < 0.0 {
        ck.z = ck.z - 1;
    }
    return ck;
}
pub fn p_int_2_index_in_chunk(x: i32, y: i32, z: i32) -> usize {
    return (x + y * VF_CHUNK_WIDTH + z * VF_CHUNK_WIDTH * VF_CHUNK_WIDTH) as usize;
}