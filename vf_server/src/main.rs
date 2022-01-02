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
    let mut the_game = game::Game::create();

    let listener = TcpListener::bind("127.0.0.1:7776").await?;
    println!("server launched");
    loop {
        let (mut socket, addr) = listener.accept().await?;
        // let (mut rd, mut wr) = socket.into_split();
        the_game.spawn_player(socket,addr);
        // tokio::spawn(async move {
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