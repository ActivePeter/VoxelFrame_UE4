#![feature(linked_list_remove)]

mod game;
#[macro_use]
pub mod chunk;
mod game_player_manager;
mod player;
mod entity;
mod base_type;
mod client;
mod conv;
mod send;
mod send_packer;
mod protos;
mod send_packer_head;
// mod base_func;

// use std::net::TcpListener;
use tokio::net::TcpListener;
use tokio::io::AsyncReadExt;
pub use tokio::sync::RwLock;
pub use std::sync::Arc;
pub use std::cell::RefCell;
pub use std::rc::Rc;
#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    //创建游戏

    let game_main_loop_channels=
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
            let mut client =client::Client::create(socket,addr);
            let tx= client::start_rw_loop(client);
            //2.new_player消息
            gmlc.new_player_channel_tx.send(tx).await;
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