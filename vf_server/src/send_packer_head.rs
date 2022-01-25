use byteorder::{LittleEndian, ByteOrder, BigEndian};

pub enum PackIds {
    EChunkPack = 0,
    EPlayerBasic = 1,
    EChunkEntityPack =2,
}
pub fn make(pack_id:PackIds, pack_len:usize) -> [u8; 5] {
    let mut msg_head = [0; 5];
    msg_head[0]=pack_id as u8;
    BigEndian::write_u32(&mut msg_head[1..], (pack_len) as u32);
    return msg_head;
}