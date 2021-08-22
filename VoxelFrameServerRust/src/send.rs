use crate::base::*;
use protobuf::{Message, CodedOutputStream};
use byteorder::{BigEndian, ByteOrder};
// use protos::generated_with_native::

pub async fn chunk_2_player(chunk_ptr: ArcRw<Chunk>, p_ptr: ArcRw<Player>) {
    let _socket_lock = p_ptr.read().await.socket.upgrade().unwrap().clone();

    let mut proto_chunk = protos::chunk::Chunk::new();
    let mut msg_byte;
    // msg_byte.resize(4, 0);
    let key = chunk_ptr.read().await.chunk_key.clone();
    proto_chunk.x = key.x;
    proto_chunk.y = key.y;
    proto_chunk.z = key.z;
    {
        //锁住chunk，提取数据
        let mut chunk_lock = chunk_ptr.write().await;
        proto_chunk.data =
            // chunk_ptr.read().await.chunk_data.clone();
            mem::take(&mut chunk_lock.chunk_data);
        msg_byte = proto_chunk.write_to_bytes().unwrap();
        //归还数据给chunk
        chunk_lock.chunk_data = proto_chunk.data;
    }
    let msg_byte_len = msg_byte.len();
    let msg_head = &mut [0; 4];
    // println!("msg_head len", )
    BigEndian::write_u32(msg_head, (msg_byte_len - 4) as u32);

    // len_bytes.join(body_byte);
    {
        let mut socket = _socket_lock.write().await;
        socket.write_all(msg_head).await.unwrap();//长度
        socket.write_all(msg_byte.as_slice()).await.unwrap();//数据
    }
}