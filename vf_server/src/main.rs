#![feature(linked_list_remove)]
#![feature(generic_associated_types)]
#![feature(type_alias_impl_trait)]


// mod game;
#[macro_use]

mod base_type;
mod net;
mod conv;
mod net_send;
mod send_packer;
mod protos;
pub mod net_pack;
mod part_server_sync;

mod async_task;

mod log;
mod game;
pub mod netclient;
mod net_event;
mod event;
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

        tokio::spawn(async move {
            println!("one connection");
            let (mut r, mut w) = socket.into_split();
            let tx = net::start_rw_loop(gmlc, r, w, addr);
        });
    }
}