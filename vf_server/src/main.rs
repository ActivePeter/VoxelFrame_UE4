#![feature(linked_list_remove)]

mod game;
#[macro_use]
pub mod game_chunk;
mod game_player_manager;
mod game_player;
mod base_type;
mod net;
mod conv;
mod send;
mod send_packer;
mod protos;
mod net_pack_convert;
mod part_server_sync;
mod game_flow;
mod game_entity;
mod async_task;
mod game_block;
mod log;
mod game_pack_distribute;
// mod base_func;

// use std::net::TcpListener;
use tokio::net::TcpListener;
use tokio::io::AsyncReadExt;
pub use tokio::sync::RwLock;
pub use std::sync::Arc;
pub use std::cell::RefCell;
pub use std::rc::Rc;
use std::process::{ChildStderr, Stdio};
use std::io::{Read, BufReader, BufRead};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    //创建游戏

    let game_main_loop_channels =
        game::main_loop().await;

    let listener = TcpListener::bind("127.0.0.1:7776").await?;
    println!("server launched");

    loop {
        let (mut socket, addr) = listener.accept().await?;
        let gmlc = game_main_loop_channels.clone();


        // let (mut rd, mut wr) = socket.into_split();
        // let game_handle=the_game.clone();
        tokio::spawn(async move {
            println!("one connection");
            // game_handle.write().await.spawn_player(socket,addr).await;

            //1.创建消息循环
            // let mut client = net::Client::create(socket, addr);
            let (mut r, mut w) = socket.into_split();
            let tx = net::start_rw_loop(gmlc, r, w, addr);
            // tx.sender.send(vec![0]).await
            // //2.new_player消息
            // gmlc.new_player_channel_tx.send(tx).await;
        });
        //     println!("client connected {0}", addr);
        //     let mut buf = [0; 1024];
        //     loop {
        //         let n = match rd.read(&mut buf).await {
        //             // socket closed
        //             Ok(n) if n == 0 => break,
        //             Ok(n) => n,
        //             Err(e) => {
        //                 eprintln!("failed to read from socket; err = {:?}", e);
        //                 return;
        //             }
        //         };
        //     }
        //     println!("client disconnect");
        // });
    }
}