#![feature(map_try_insert)]
#[macro_use]
extern crate lazy_static;

// extern crate protobuf;
//
pub mod game;
#[macro_use]
pub mod chunk;
pub mod player;
#[macro_use]
pub mod base;

mod send;
mod protos;
mod receive;

use crate::base::*;
use tokio::sync::MutexGuard;

lazy_static! {
     pub static ref  GAME_CONTEXT:Arc<tokio::sync::Mutex<Game>>=Arc::new(tokio::sync::Mutex::new(Game::new()));
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {


    // let a = ArcRw_new!(Chunk);
    let listener = TcpListener::bind("127.0.0.1:7776").await?;
    // thread::spawn(|| {
    //     get_game_context().start();
    //     loop {
    //         for i in 1..50 {
    //             get_game_context().tick();
    //         }
    //         thread::sleep(time::Duration::from_millis(1));
    //     }
    // });
    println!("server launched");
    loop {
        let (mut socket, addr) = listener.accept().await?;
        let (mut rd, mut wr) = socket.into_split();
        // socket.write_all()
        tokio::spawn(async move {
            println!("client connected {0}", addr);
            let socket_lock = ArcRw_new!(wr);// Arc::new(RwLock::new(socket));
            let player = GAME_CONTEXT.lock().await.player_manager.
                add_player().await;
            player.write().await.socket = Arc::downgrade(&socket_lock);
            async_player_check_chunk_load(player).await;

            let mut buf = [0; 1024];
            // In a loop, read data from the socket and write the data back.
            loop {
                let n = match rd.read(&mut buf).await {
                    // socket closed
                    Ok(n) if n == 0 => break,
                    Ok(n) => n,
                    Err(e) => {
                        eprintln!("failed to read from socket; err = {:?}", e);
                        return;
                    }
                };

                // // Write the data back
                // if let Err(e) = socket_lock.write().await.write_all(&buf[0..n]).await {
                //     eprintln!("failed to write to socket; err = {:?}", e);
                //     return;
                // }
            }

            println!("client disconnect");
        });
    }
}
