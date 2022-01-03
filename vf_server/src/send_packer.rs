use crate::*;
use std::mem;
use crate::chunk::Chunk;
use protobuf::{Message, CodedOutputStream, RepeatedField};
use byteorder::{LittleEndian, ByteOrder, BigEndian};
use crate::entity::{EntityData, EntityId};
use crate::send_packer_head::PackIds;
use crate::game::Game;
use std::collections::HashMap;
use crate::protos::common::EntityPos;

mod util{
    use crate::{send_packer_head, protos};
    use crate::send_packer_head::PackIds;
    use protobuf::{CodedOutputStream, RepeatedField};
    // use std::alloc::Global;

    pub fn proto2buf<T: ::protobuf::Message>(proto_pack: T,pack_id:PackIds) -> Vec<u8> {
        //消息体 长度
        let msg_body_len=proto_pack.compute_size() as usize;
        //消息头
        let msg_head=send_packer_head::make(
            pack_id, msg_body_len);
        //给拷贝准备好空间
        let mut final_vec =msg_head.to_vec();
        final_vec.resize(5 + msg_body_len, 0);
        // let final_vec_slice=final_vec.as_mut_slice();
        // 流输出器
        let mut stream =
            CodedOutputStream::bytes(&mut final_vec[5..]);
        proto_pack.write_to(&mut stream);
        stream.flush();
        println!("player basic packed bodylen:{0}"
                 ,msg_body_len);
        return final_vec
    }

    pub fn make_repeated<T:Clone>(model:T,n:usize)
        ->RepeatedField::<T>{

        let mut vec=Vec::new();
        // vec.push(T::new());
        vec.resize(
            n,model);
        //长度对齐
        return RepeatedField::<T>::from_vec(vec);

    }
}

pub fn pack_chunk_pack(chunk: &Chunk) -> Vec<u8> {
    // let _socket_lock = p_ptr.read().await.socket.upgrade().unwrap().clone();
    let mut proto_chunk = protos::common::ChunkPack::new();
    // let mut msg_byte;
    // msg_byte.resize(4, 0);
    // let key = chunk_ptr.read().await.chunk_key.clone();
    proto_chunk.x = chunk.chunk_key.x;
    proto_chunk.y = chunk.chunk_key.y;
    proto_chunk.z =chunk.chunk_key.z;
    // println!("{0} {1} {2}", proto_chunk.x, proto_chunk.y, proto_chunk.z);
    {
        proto_chunk.data =chunk.chunk_data.clone();
        // msg_byte = proto_chunk.write_to_bytes().unwrap();
    }
    let final_vec=
        util::proto2buf(proto_chunk,PackIds::ChunkPack);
    // let msg_byte_len = msg_byte.len();
    //
    // //制作包头
    // let msg_head=send_packer_head::make(
    //     PackIds::ChunkPack,msg_byte_len);
    // let mut a = msg_head.to_vec();
    // a.append(&mut msg_byte);

    return final_vec;
}

pub fn pack_player_basic(entity:&EntityData) -> Vec<u8> {
    //装填数据
    let mut proto_pack =protos::common::PlayerBasic::new();
    proto_pack.entity_id=entity.entity_id;

    let final_vec=
        util::proto2buf(proto_pack,PackIds::PlayerBasic);

    // for a in final_vec.iter(){
    //     print!("{} ",a);
    // }
    // println!("|");
    // for a in body.iter(){
    //     print!("{} ",a);
    // }
    // println!("|");
    return final_vec;
}

pub fn pack_chunk_entity_pack(
    chunk: &Chunk, entities:&HashMap<EntityId, EntityData>)
    -> Vec<u8> {

    let mut proto_pack = protos::common::ChunkEntityPack::new();
    proto_pack.x=chunk.chunk_key.x;
    proto_pack.y=chunk.chunk_key.y;
    proto_pack.z=chunk.chunk_key.z;

    EntityPos::new();
    // let mut entity_pos_vec:
    //     Vec<protos::common::EntityPos>=Vec::new();
    // entity_pos_vec.resize(
    //     chunk.entities.len(),EntityPos::new());
    // //长度对齐
    // proto_pack.entity_pos=
    //     RepeatedField::<EntityPos>::from_vec(entity_pos_vec);

    proto_pack.entity_pos=
        util::make_repeated(
            EntityPos::new(),entities.len());

    let mut i =0;
    //拷贝到proto构造器
    for eid in chunk.entities.iter(){
        let e=entities.get(eid).unwrap();
        let cur=proto_pack.entity_pos.get_mut(i).unwrap();
        cur.x=e.position[0];
        cur.y=e.position[1];
        cur.z=e.position[2];
        i=i+1;
    }

    let final_vec=
        util::proto2buf(proto_pack,PackIds::ChunkEntityPack);

    return final_vec;
}