#[macro_use]
extern crate lazy_static;

//
pub mod game;
#[macro_use]
pub mod chunk;
pub mod player;
#[macro_use]
pub mod base;

use crate::base::*;
use tokio::sync::MutexGuard;





lazy_static! {
     pub static ref  GAME_CONTEXT:Arc<tokio::sync::Mutex<Game>>=Arc::new(tokio::sync::Mutex::new(Game::new()));
}
pub async fn get_game_context() -> tokio::sync::MutexGuard<'static, Game> {
    return GAME_CONTEXT.lock().await;
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

        tokio::spawn(async move {
            println!("client connected {0}", addr);
            let socket_lock = Arc::new(RwLock::new(socket));
            let player = GAME_CONTEXT.lock().await.player_manager.
                add_player();
            player.write().await.socket = Arc::downgrade(&socket_lock);
            async_player_check_chunk_load(player).await;

            let mut buf = [0; 1024];
            // In a loop, read data from the socket and write the data back.
            loop {
                let n = match socket_lock.write().await.read(&mut buf).await {
                    // socket closed
                    Ok(n) if n == 0 => break,
                    Ok(n) => n,
                    Err(e) => {
                        eprintln!("failed to read from socket; err = {:?}", e);
                        return;
                    }
                };

                // Write the data back
                if let Err(e) = socket_lock.write().await.write_all(&buf[0..n]).await {
                    eprintln!("failed to write to socket; err = {:?}", e);
                    return;
                }
            }

            println!("client disconnect");
        });
    }
}
