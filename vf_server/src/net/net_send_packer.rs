use crate::*;
use std::mem;
use protobuf::{Message, CodedOutputStream, RepeatedField, SingularPtrField};
use byteorder::{LittleEndian, ByteOrder, BigEndian};
use crate::game::Game;
use std::collections::HashMap;
use crate::protos::common::EntityPos;
use crate::net::net_pack::{PackIds, PackPriority};
use crate::protos::common;
use crate::game::chunk::Chunk;
use crate::game::entity::{EntityData, EntityId};
use crate::net::net_pack;

mod util{
    use protobuf::{CodedOutputStream, RepeatedField};
    // use std::alloc::Global;



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



fn make_pack_head(pack_id: PackIds, pack_len: usize) -> [u8; 5] {
    let mut msg_head = [0; 5];
    msg_head[0] = pack_id as u8;
    BigEndian::write_u32(&mut msg_head[1..], (pack_len) as u32);
    return msg_head;
}

pub fn pack_to_bytes2<T: ::protobuf::Message>(proto_pack: &T, pack_id: PackIds) -> Vec<u8> {
    //消息体 长度
    let msg_body_len = proto_pack.compute_size() as usize;
    //消息头
    let msg_head = make_pack_head(
        pack_id, msg_body_len);
    //给拷贝准备好空间
    let mut final_vec = msg_head.to_vec();
    final_vec.resize(5 + msg_body_len, 0);
    // let final_vec_slice=final_vec.as_mut_slice();
    // 流输出器
    let mut stream =
        CodedOutputStream::bytes(&mut final_vec[5..]);
    proto_pack.write_to(&mut stream).unwrap();
    stream.flush().unwrap();
    // println!("player basic packed bodylen:{0}"
    //          , msg_body_len);
    return final_vec;
}
pub fn packbox_to_bytes<T: ::protobuf::Message>(proto_pack: Box<T>, pack_id: PackIds) -> Vec<u8> {
    //消息体 长度
    let msg_body_len = proto_pack.compute_size() as usize;
    //消息头
    let msg_head = make_pack_head(
        pack_id, msg_body_len);
    //给拷贝准备好空间
    let mut final_vec = msg_head.to_vec();
    final_vec.resize(5 + msg_body_len, 0);
    // let final_vec_slice=final_vec.as_mut_slice();
    // 流输出器
    let mut stream =
        CodedOutputStream::bytes(&mut final_vec[5..]);
    proto_pack.write_to(&mut stream).unwrap();
    stream.flush().unwrap();
    // println!("player basic packed bodylen:{0}"
    //          , msg_body_len);
    return final_vec;
}
pub fn pack_to_bytes<T: ::protobuf::Message>(proto_pack: T, pack_id: PackIds) -> Vec<u8> {
    //消息体 长度
    let msg_body_len = proto_pack.compute_size() as usize;
    //消息头
    let msg_head = make_pack_head(
        pack_id, msg_body_len);
    //给拷贝准备好空间
    let mut final_vec = msg_head.to_vec();
    final_vec.resize(5 + msg_body_len, 0);
    // let final_vec_slice=final_vec.as_mut_slice();
    // 流输出器
    let mut stream =
        CodedOutputStream::bytes(&mut final_vec[5..]);
    proto_pack.write_to(&mut stream).unwrap();
    stream.flush().unwrap();
    // println!("player basic packed bodylen:{0}"
    //          , msg_body_len);
    return final_vec;
}

pub fn pack_chunk_pack(chunk: &Chunk) -> (Vec<u8>,PackPriority) {
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
        pack_to_bytes(
            proto_chunk,PackIds::EChunkPack);
    // let msg_byte_len = msg_byte.len();
    //
    // //制作包头
    // let msg_head=send_packer_head::make(
    //     PackIds::ChunkPack,msg_byte_len);
    // let mut a = msg_head.to_vec();
    // a.append(&mut msg_byte);

    return (final_vec,PackIds::EChunkPack.default_priority());
}

pub fn pack_player_basic(entity:&EntityData) -> (Vec<u8>,PackPriority) {
    //装填数据
    let mut proto_pack =protos::common::PlayerBasic::new();

    proto_pack.pos=SingularPtrField::some(entity.gen_entity_pos_pack());
    // proto_pack.entity_id=entity.entity_id;

    let final_vec= pack_to_bytes(
            proto_pack,PackIds::EPlayerBasic);

    // for a in final_vec.iter(){
    //     print!("{} ",a);
    // }
    // println!("|");
    // for a in body.iter(){
    //     print!("{} ",a);
    // }
    // println!("|");
    return (final_vec,PackIds::EPlayerBasic.default_priority());
}

pub fn pack_chunk_entity_pack(
    chunk: &Chunk, entities:&HashMap<EntityId, EntityData>)
    -> (Vec<u8>,PackPriority) {

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
            EntityPos::new(),chunk.entities.len());
    {//拷贝entity数据到数组中
        let mut i=0;
        for eid in chunk.entities.iter(){
            proto_pack.entity_pos[i].entity_id=*eid;
            println!("pack_chunk_entity_pack add eid{}",*eid);
            let entity=entities.get(eid).unwrap();
            proto_pack.entity_pos[i].x=entity.position[0];
            proto_pack.entity_pos[i].y=entity.position[1];
            proto_pack.entity_pos[i].z=entity.position[2];
            i+=1;
        }
    }
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

    let final_vec= pack_to_bytes(
        proto_pack,PackIds::EChunkEntityPack);
        // util::proto2buf(proto_pack,PackIds::EChunkEntityPack);

    return (final_vec,PackIds::EChunkEntityPack.default_priority());
}

