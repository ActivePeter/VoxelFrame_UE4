use byteorder::{LittleEndian, ByteOrder, BigEndian};
use crate::protos::common;
use protobuf::{CodedOutputStream, parse_from_bytes, Message, ProtobufResult, ProtobufError};
pub(crate) type PackPriority=i32;
#[derive(Clone)]
pub enum PackIds {
    EChunkPack = 0,
    EPlayerBasic = 1,
    EChunkEntityPack =2,
    EClientFirstConfirm=3,
    EEntityPos=4,
    EMainPlayerMoveCmd=5,
    ECmd_SpawnEntityInPs=6,
    ERpl_SpawnEntityInPs=7,
    EEntityPosUpdate=8,
    EPutBlock=9,
    ECmd_PutBlockInPs=10,
    ERpl_PutBlockInPs=11,
    EClientOperationFailed=12,
    EClientOperationSucc=13,
    EMainPlayerJumpCmd=14,
    ERemoveEntity=15,
    EPlayerRequestChunks=16,
    EPlayerPosUpdate=17,
}
impl PackIds{
    pub(crate) fn default_priority(&self) -> PackPriority {
        let high=1;
        let common=10;
        let low=20;
        // the bigger, the later
        match self{
            PackIds::EChunkPack => {low}
            PackIds::EPlayerBasic => {common}
            PackIds::EChunkEntityPack => {common}
            PackIds::EClientFirstConfirm => {common}
            PackIds::EEntityPos => {high}
            PackIds::EMainPlayerMoveCmd => {high}
            PackIds::ECmd_SpawnEntityInPs => {high}
            PackIds::ERpl_SpawnEntityInPs => {high}
            PackIds::EEntityPosUpdate => {high}
            PackIds::EPutBlock => {common}
            PackIds::ECmd_PutBlockInPs => {common}
            PackIds::ERpl_PutBlockInPs => {common}
            PackIds::EClientOperationFailed => {common}
            PackIds::EClientOperationSucc => {common}
            PackIds::EMainPlayerJumpCmd => {high}
            PackIds::ERemoveEntity => {common}
            PackIds::EPlayerRequestChunks => {common}
            PackIds::EPlayerPosUpdate => {high}
        }
    }
}

//用于携带消息包
#[derive(Debug)]
pub enum MsgEnum {
    ClientFirstConfirm(common::ClientFirstConfirm),
    EntityPos(common::EntityPos),
    PlayerBasic(common::PlayerBasic),
    ChunkPack(common::ChunkPack),
    ChunkEntityPack(common::ChunkEntityPack),
    MainPlayerMoveCmd(common::MainPlayerMoveCmd),
    Rpl_SpawnEntityInPs(common::Rpl_SpawnEntityInPs),
    EntityPosUpdate(common::EntityPosUpdate),
    PutBlock(common::PutBlock),
    Rpl_PutBlockInPs(common::Rpl_PutBlockInPs),
    MainPlayerJumpCmd(common::MainPlayerJumpCmd),
    PlayerRequestChunks(common::PlayerRequestChunks),
    PlayerPosUpdate(common::PlayerPosUpdate)
}



macro_rules! one_pack {
  ($msg:ty,$msg_enum:expr,$data_slice:ident ) => {

    let a:ProtobufResult<$msg>=Message::parse_from_bytes($data_slice);
      match a{
        Ok(v)=>{return Some($msg_enum(v));}
        Err(e)=>{panic!("{}, {}",e,$data_slice.len())}
    }
    // let r= Message::parse_from_bytes($data_slice).unwrap();

  }
}
fn get_pack_id_i32(id:PackIds) -> i32 {
    return id as i32;
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
        // println!("EPlayerBasic");
        one_pack!(common::PlayerBasic,MsgEnum::PlayerBasic,data_slice);

    }else if msg_pack_id==PackIds::EMainPlayerMoveCmd as i32{
        one_pack!(common::MainPlayerMoveCmd,MsgEnum::MainPlayerMoveCmd,data_slice);

    }else if msg_pack_id==PackIds::ERpl_SpawnEntityInPs as i32{
        one_pack!(common::Rpl_SpawnEntityInPs,MsgEnum::Rpl_SpawnEntityInPs,data_slice);

    }else if msg_pack_id==PackIds::EEntityPosUpdate as i32{
        one_pack!(common::EntityPosUpdate,MsgEnum::EntityPosUpdate,data_slice);

    }else if msg_pack_id==PackIds::EPutBlock as i32{
        one_pack!(common::PutBlock,MsgEnum::PutBlock,data_slice);

    }else if msg_pack_id==PackIds::ERpl_PutBlockInPs as i32{
        one_pack!(common::Rpl_PutBlockInPs,MsgEnum::Rpl_PutBlockInPs,data_slice);

    }else if msg_pack_id==PackIds::EMainPlayerJumpCmd as i32{
        one_pack!(common::MainPlayerJumpCmd,MsgEnum::MainPlayerJumpCmd,data_slice);

    }else if msg_pack_id==PackIds::EPlayerRequestChunks as i32{
        one_pack!(common::PlayerRequestChunks,MsgEnum::PlayerRequestChunks,data_slice);
    }else if msg_pack_id==PackIds::EPlayerPosUpdate as i32{
        one_pack!(common::PlayerPosUpdate,MsgEnum::PlayerPosUpdate,data_slice);
    }

    return None;
}