use crate::*;
use std::mem;
use crate::chunk::Chunk;
use protobuf::Message;
use byteorder::{LittleEndian, ByteOrder, BigEndian};

pub fn pack_chunk_pack(chunk: &Chunk) -> Vec<u8> {
    // let _socket_lock = p_ptr.read().await.socket.upgrade().unwrap().clone();
    let mut proto_chunk = protos::common::ChunkPack::new();
    let mut msg_byte;
    // msg_byte.resize(4, 0);
    // let key = chunk_ptr.read().await.chunk_key.clone();
    proto_chunk.x = chunk.chunk_key.x;
    proto_chunk.y = chunk.chunk_key.y;
    proto_chunk.z =chunk.chunk_key.z;
    println!("{0} {1} {2}", proto_chunk.x, proto_chunk.y, proto_chunk.z);
    {
        proto_chunk.data =chunk.chunk_data.clone();
        msg_byte = proto_chunk.write_to_bytes().unwrap();
    }
    let msg_byte_len = msg_byte.len();
    let msg_head = &mut [0; 5];
    println!("msg body len {0}", (msg_byte_len - 4));
    BigEndian::write_u32(&mut msg_head[1..], (msg_byte_len) as u32);
    println!("msg body len read {0} {1} {2} {3} {4} {5}", BigEndian::read_u32(&msg_head[1..]),
             msg_head[0],
             msg_head[1],
             msg_head[2],
             msg_head[3],
             msg_head[4]
    );

    let mut a = msg_head.to_vec();
    a.append(&mut msg_byte);

    return a;
}