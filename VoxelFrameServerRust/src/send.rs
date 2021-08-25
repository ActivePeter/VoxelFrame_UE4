use crate::base::*;
use protobuf::{Message, CodedOutputStream};
use byteorder::{LittleEndian, ByteOrder, BigEndian};
use tokio::io::AsyncWrite;
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
    // ;
    // len_bytes.join(body_byte);
    let mut a = msg_head.to_vec();
    a.append(&mut msg_byte);

    {
        println!("before socket locked");
        let mut socket = _socket_lock.write().await;
        println!("socket locked");
        // socket.poll_write().is_ready()
        // let mut offset = 0;
        // let alen = a.len();
        // while offset < alen {
        //     let r = socket.write(&a.as_slice()[offset..]).await;
        //     match r {
        //         Ok(n) => {
        //             offset += n;
        //         }
        //         Err(e) => println!("error: {:?}", e),
        //     }
        // }
        socket.write_all(a.as_slice()).await.unwrap();
        socket.flush().await.unwrap();
        // socket.write_all(msg_head).await.unwrap();//长度
        // socket.write_all(msg_byte.as_slice()).await.unwrap();//数据

        println!("one was sent");
    }
}