use crate::*;
use tokio::net::tcp::{OwnedReadHalf, OwnedWriteHalf};
use tokio::net::TcpStream;
use std::net::SocketAddr;
use crate::game_player::Player;
use tokio::sync::{mpsc, oneshot};
use crate::protos::common::{ClientFirstConfirm, EntityPos, PlayerBasic, ChunkPack, ChunkEntityPack, ClientType};
use crate::net_pack_convert::MsgEnum;
use crate::net::msg_pack_make::MsgPackMaker;
use crate::protos::common::ClientType::{ClientType_Player, ClientType_GameServer};
use crate::game::{ClientId, GameMainLoopChannels};
// use std::alloc::Global;
use tokio::io::AsyncWriteExt;

//当前文件，表示读写循环

pub struct ClientDescription{
    pub client_type: ClientType,
    pub sender: ClientSender,
    pub client_id: ClientId,
}

//来自客户端的消息
// 接收消息后转发给主循环的数据结构
pub struct ClientMsg {
    pub client_type: ClientType,
    pub client_id: ClientId,
    pub msg_enum: MsgEnum,
}

pub enum ClientMsgEnum {
    ClientCommonMsg(ClientMsg),
    ClientConnect(ClientConnect),
    ClientDisconnect(ClientDisconnect),
}

pub struct ClientConnect {
    pub sender: ClientSender,
    pub client_type: ClientType,
    pub response: oneshot::Sender<ClientId>,
}

pub struct ClientDisconnect {
    pub client_id:ClientId,
}

//一个客户端需要的所有东西
// 1.sender
// 2.client type
#[derive(Clone, Debug)]
pub struct ClientSender{
    // pub id:ClientId,
    sender:mpsc::Sender<Vec<u8>>,
}
impl ClientSender{
    pub async fn send(&self,vec:Vec<u8>){
        // println!("send cid {}",self.id);
        self.sender.send(vec).await;
    }
}
// pub type ClientSender = mpsc::Sender<Vec<u8>>;

//启动一个客户端的读写循环
pub fn start_rw_loop(
    mainloop_chan: GameMainLoopChannels,
    mut rd: OwnedReadHalf,
    mut wr: OwnedWriteHalf,
    addr: SocketAddr) {
    println!("start_rw_loop {0}", addr);

    //服务端通过这个通道给客户端发消息
    // 消息数据为bytes，即在主循环中就进行封包
    let (net_tx, mut net_rx): (mpsc::Sender<Vec<u8>>, mpsc::Receiver<Vec<u8>>) =
        mpsc::channel(100);
    // tokio::spawn(async move{
    //     while let Some(i) = rx.recv().await {
    //         println!("got = {}", i);
    //     });
    let addr2=addr.clone();
    //send wait loop
    tokio::spawn(async move {
        loop {
            let i=net_rx.recv().await;
            match i {
                None => {
                    break;
                }
                Some(j) => {

                    // println!("one send msg {}",addr2);
                    wr.write_all(j.as_slice()).await;
                    wr.flush().await;
                }
            }
            // => {
            // },
            // tokio::select! {
            //         Some(i) =
            //         else => break,
            //     }
        }
        // while let Some(i) = rx.recv().await {
        //     println!("one send msg");
        //     wr.write_all(i.as_slice()).await;
        //     wr.flush().await;
        // }

        println!("send loop end");
    });

    //rec loop
    tokio::spawn(async move {
        // let client=player.write().await.client;
        let mut receive_handler = ReceiveHandler::create(ClientSender{
            // id: 0,
            sender: net_tx,
        }, mainloop_chan,addr);

        let mut buf = [0; 1024];
        loop {
            let n = match rd.read(&mut buf).await {
                // socket closed
                Ok(n) if n == 0 => break,
                Ok(n) => n,
                Err(e) => {
                    eprintln!("failed to read from socket; err = {:?}", e);
                    break;
                }
            };
            // println!("client read size{}", n);
            receive_handler.handle_a_buffset(&buf, n).await;
        }
        //发给主循环通信结束的信息
        receive_handler.disconnect().await;
        println!("client disconnect,rec loop end");
    });
    // return ClientSender::new(tx.clone());
}

//接受消息输入到此，先使用packer进行解包
//解完后由receive handler处理，
//  记录接收消息过程中的一些状态

struct ReceiveHandler {
    kernel: ReceiveHandlerKernel,
    msg_maker: MsgPackMaker,
}

impl ReceiveHandler {
    pub fn create(sender: ClientSender,
                  gmlc: GameMainLoopChannels,
    addr:SocketAddr) -> Self {
        return Self {
            // next_client_id: 0
            kernel: ReceiveHandlerKernel::create(sender, gmlc,addr),
            msg_maker: Default::default(),
        };
    }
    pub async fn handle_a_buffset(&mut self, buffset: &[u8], len: usize) {
        self.msg_maker.handle_a_buffset(
            buffset, len,
            &mut self.kernel,
        ).await;
    }

    pub async fn disconnect(&mut self){
        self.kernel.disconnect().await;
    }
}

pub struct ReceiveHandlerKernel {
    client_type: Option<ClientType>,
    client_sender: ClientSender,
    client_id: ClientId,
    mainloop_chan: GameMainLoopChannels,
    addr:SocketAddr,
}

impl ReceiveHandlerKernel {
    pub fn create(sender: ClientSender, gmlc: GameMainLoopChannels,addr:SocketAddr) -> Self {
        return Self {
            // next_client_id: 0
            client_sender: sender,
            client_type: None,
            client_id: 0,
            mainloop_chan: gmlc,
            addr,
        };
    }
    pub async fn disconnect(&mut self){
        match self.client_type {
            None => {}
            Some(_) => {
                self.mainloop_chan.msg_channel_tx.send(ClientMsgEnum::ClientDisconnect(
                    ClientDisconnect{
                        client_id:self.client_id
                    }
                )).await;
            }
        }
    }
    async fn handle_one_msg(&mut self, msg: MsgEnum) {
        match self.client_type {
            None => {
                //未加载客户端状态
                match msg {
                    MsgEnum::ClientFirstConfirm(confirm) => {
                        if confirm.client_type == ClientType_Player {
                            println!("client first confirm {0} {1}", "ClientType_Player",self.addr);
                        } else if confirm.client_type == ClientType_GameServer {
                            println!("client first confirm {0} {1}",
                                     "ClientType_GameServer",self.addr);
                        } else {
                            println!("client first confirm unknown type");
                        }
                        self.client_type = Some(confirm.client_type);
                        let (resp_tx, resp_rx):
                            (oneshot::Sender<ClientId>, oneshot::Receiver<ClientId>) = oneshot::channel();

                        self.mainloop_chan.msg_channel_tx.send(
                            ClientMsgEnum::ClientConnect(ClientConnect {
                                sender: self.client_sender.clone(),
                                client_type: confirm.client_type,
                                response: resp_tx,
                            })).await;
                        let res_client_id = resp_rx.await.unwrap();
                        println!("get client id {}",res_client_id);
                        // self.client_sender.id=res_client_id;
                        self.client_id = res_client_id;
                    }
                    _ => {}
                }
            }
            Some(ctype) => {
                //已加载客户端状态
                self.mainloop_chan.msg_channel_tx.send(
                    ClientMsgEnum::ClientCommonMsg(ClientMsg {
                        client_type: ctype,
                        client_id: self.client_id,
                        msg_enum: msg,
                    })
                ).await;
            }
        }
    }
}

mod msg_pack_make {
    use byteorder::{LittleEndian, BigEndian, ByteOrder};
    use crate::net_pack_convert::{MsgEnum, bytes_to_pack};
    use crate::net::{ReceiveHandlerKernel};
    use tokio::macros::support::Future;

    const MsgPackHeadSize: u8 = 5;

    //描述数据包头
    #[derive(Default, Debug)]
    struct MsgPackHead {
        pack_id: u8,
        pack_len: u32,
    }

    //进行数据的解包
    #[derive(Default, Debug)]
    pub struct MsgPackMaker {
        pack_head: MsgPackHead,
        //描述包头
        head_buff: [u8; 5],
        head_rec_cnt: u8,
        //描述包体
        body_buff: Vec<u8>,
        body_rec_cnt: u32,
    }

    impl MsgPackMaker {
        pub fn create() -> Self {
            return Self::default();
        }
        pub async fn handle_a_buffset
        (&mut self, buffset: &[u8], _byte_cnt: usize,
         receive_handler: &mut ReceiveHandlerKernel)
        // where F: FnMut(s:MsgEnum)
        {
            let mut handled_offset = 0;

            while handled_offset < _byte_cnt {
                let byte_cnt_left = _byte_cnt - handled_offset;
                //头本次还是未收全
                if self.head_rec_cnt < MsgPackHeadSize {
                    if byte_cnt_left + (self.head_rec_cnt as usize) < MsgPackHeadSize as usize {
                        for i in 0..byte_cnt_left {
                            self.head_buff[(self.head_rec_cnt as usize) + i]
                                = buffset[handled_offset + i];
                        }
                        self.head_rec_cnt += byte_cnt_left as u8;
                    }//头本次收全
                    else {
                        let cpylen = MsgPackHeadSize - self.head_rec_cnt;
                        for i in 0..cpylen {
                            self.head_buff[(self.head_rec_cnt + i) as usize] =
                                buffset[handled_offset + i as usize];
                        }
                        handled_offset += (MsgPackHeadSize - self.head_rec_cnt) as usize;
                        self.calc_pack_head();
                        self.head_rec_cnt = MsgPackHeadSize;
                        //扩大缓冲区
                        if (self.pack_head.pack_len > self.body_buff.len() as u32) {
                            self.body_buff.resize(self.pack_head.pack_len as usize, 0);
                        }
                        // continue;
                    }
                }

                // 1.剩余数据小于需要读的字节数量(不够
                if byte_cnt_left <
                    (self.pack_head.pack_len - self.body_rec_cnt) as usize {
                    self.write_data_2_body(&buffset[handled_offset..], byte_cnt_left);
                    return;
                } else {
                    //完成读包
                    let len = self.pack_head.pack_len - self.body_rec_cnt;
                    self.write_data_2_body(&buffset[handled_offset..], len as usize);
                    handled_offset += len as usize;

                    //对包进行解析
                    let a = bytes_to_pack(self.pack_head.pack_id as i32,
                                          &self.body_buff.as_slice()[..self.pack_head.pack_len as usize]);
                    match a {
                        None => {
                            println!("no msg type matched");
                        }
                        Some(msg_enum) => {
                            receive_handler.handle_one_msg(msg_enum).await;
                            // callback(receive_handler, msg_enum).await;
                        }
                    }
                    self.reset();
                }
            }
        }
        fn reset(&mut self) {
            self.head_rec_cnt = 0;
            self.body_rec_cnt = 0;
        }
        fn write_data_2_body(&mut self, buffset: &[u8], byte_cnt_left: usize) {
            for i in 0..byte_cnt_left {
                self.body_buff[self.body_rec_cnt as usize + i] =
                    buffset[i];
            }
        }
        fn calc_pack_head(&mut self) {
            self.pack_head.pack_id = self.head_buff[0];
            self.pack_head.pack_len = BigEndian::read_u32(&self.head_buff[1..]);
        }
    }
}
