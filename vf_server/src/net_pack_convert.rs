use byteorder::{LittleEndian, ByteOrder, BigEndian};
use crate::protos::common;
use protobuf::{CodedOutputStream, parse_from_bytes, Message, ProtobufResult, ProtobufError};

pub enum PackIds {
    EChunkPack = 0,
    EPlayerBasic = 1,
    EChunkEntityPack =2,
    EClientFirstConfirm=3,
    EEntityPos=4,
    EMainPlayerMoveCmd=5,
}
//用于携带消息包
pub enum MsgEnum {
    ClientFirstConfirm(common::ClientFirstConfirm),
    EntityPos(common::EntityPos),
    PlayerBasic(common::PlayerBasic),
    ChunkPack(common::ChunkPack),
    ChunkEntityPack(common::ChunkEntityPack),
    MainPlayerMoveCmd(common::MainPlayerMoveCmd),
}

fn make_pack_head(pack_id: PackIds, pack_len: usize) -> [u8; 5] {
    let mut msg_head = [0; 5];
    msg_head[0] = pack_id as u8;
    BigEndian::write_u32(&mut msg_head[1..], (pack_len) as u32);
    return msg_head;
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
    proto_pack.write_to(&mut stream);
    stream.flush();
    // println!("player basic packed bodylen:{0}"
    //          , msg_body_len);
    return final_vec;
}

macro_rules! one_pack {
  ($msg:ty,$msg_enum:expr,$data_slice:ident ) => {
    let r:$msg = Message::parse_from_bytes($data_slice).unwrap();
    return Some($msg_enum(r));
  }
}

pub fn bytes_to_pack(msg_pack_id: i32, data_slice: &[u8])
                          -> Option<MsgEnum> {
    if msg_pack_id==PackIds::EChunkPack as i32 {
        one_pack!(common::ChunkPack,MsgEnum::ChunkPack,data_slice);
    } else if msg_pack_id==PackIds::EChunkEntityPack as i32{
        one_pack!(common::ChunkEntityPack,MsgEnum::ChunkEntityPack,data_slice);
    }else if msg_pack_id==PackIds::EClientFirstConfirm as i32{
        one_pack!(common::ClientFirstConfirm,MsgEnum::ClientFirstConfirm,data_slice);
    }else if msg_pack_id==PackIds::EEntityPos as i32{
        one_pack!(common::EntityPos,MsgEnum::EntityPos,data_slice);
    }else if msg_pack_id==PackIds::EPlayerBasic as i32{
        one_pack!(common::PlayerBasic,MsgEnum::PlayerBasic,data_slice);
    }else if msg_pack_id==PackIds::EMainPlayerMoveCmd as i32{
        one_pack!(common::MainPlayerMoveCmd,MsgEnum::MainPlayerMoveCmd,data_slice);
    }
    return None;
}