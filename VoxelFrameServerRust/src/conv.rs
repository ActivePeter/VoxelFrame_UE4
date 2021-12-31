use crate::base::*;

pub fn conv_point3f_2_chunk_key(p: &Point3f) -> ChunkKey {
    let mut ck = ChunkKey {
        x: 0,
        y: 0,
        z: 0,
    };
    println!("plen  {0}", p.len());
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