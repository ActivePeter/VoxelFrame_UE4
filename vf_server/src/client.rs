use crate::*;
use tokio::net::tcp::{OwnedReadHalf, OwnedWriteHalf};
use tokio::net::TcpStream;
use std::net::SocketAddr;
use crate::player::Player;
use tokio::sync::mpsc;
use tokio::sync::mpsc::{Sender, Receiver};
use tokio::io::AsyncWriteExt;

pub struct Client {
    pub rd:OwnedReadHalf,
    pub wr:OwnedWriteHalf,
    pub addr:SocketAddr,
    // disconnect:bool,
}
impl Client{
    pub fn create(socket:TcpStream, addr:SocketAddr) -> Client {
        let (rd,wr)=socket.into_split();
        return Client{
            rd:rd,
            wr:wr,
            addr:addr
        };
    }
    // pub fn create_uninited()->Client{
    //     Client{
    //         rd:Default::default(),
    //         wr: Default::default(),
    //         // disconnect:false,
    //         addr: None
    //     }
    // }
    // pub fn start_rw_loop(&mut self){
    //     // //write
    //     // tokio::spawn(async move {
    //     //     loop {
    //     //         if(self.disconnect){
    //     //             break;
    //     //         }
    //     //     }
    //     //     println!("write loop end");
    //     // });
    //
    //     //read
    //
    // }
    // pub fn bind_socket(&mut self,socket:TcpStream,addr:SocketAddr){
    //     let (rd,wr)=socket.into_split();
    //     self.rd=rd;
    //     self.wr=wr;
    //     self.addr=Some(addr);
    // }
    pub fn start_rw_loop(mut self, player:Rc<RefCell<Player>>)
        -> Sender<Vec<u8>> {
        println!("client connected {0}", self.addr);
        let mut rd =self.rd;
        let mut wr=self.wr;

        let (tx, mut rx):(Sender<Vec<u8>>,Receiver<Vec<u8>>) = mpsc::channel(100);
        // tokio::spawn(async move{
        //     while let Some(i) = rx.recv().await {
        //         println!("got = {}", i);
        //     });

        tokio::spawn(async move{
            while let Some(i) = rx.recv().await {
                wr.write_all(i.as_slice());
                wr.flush();
            }
        });

        tokio::spawn(async move {
            // let client=player.write().await.client;

            let mut buf = [0; 1024];
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
            }
            println!("client disconnect,rec loop end");
        });
        return tx;
    }
}

