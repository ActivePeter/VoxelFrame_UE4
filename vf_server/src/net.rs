use crate::*;
use tokio::net::tcp::{OwnedReadHalf, OwnedWriteHalf};
use tokio::net::TcpStream;
use std::net::SocketAddr;
use crate::player::Player;
use tokio::sync::mpsc;
use tokio::sync::mpsc::{Sender, Receiver};
use tokio::io::AsyncWriteExt;
use crate::protos::common::{ClientFirstConfirm, EntityPos, PlayerBasic, ChunkPack, ChunkEntityPack};

//当前文件，表示读写循环

//表示连入客户端类型
enum ClientType{
    Player=0,
    PartServer,
    Empty
}

//用于携带消息包
enum MsgEnum{
    ClientFirstConfirm(ClientFirstConfirm),
    EntityPos(EntityPos),
    PlayerBasic(PlayerBasic),
    ChunkPack(ChunkPack),
    ChunkEntityPack(ChunkEntityPack),
}

//来自客户端的消息
// 接收消息后转发给主循环的数据结构
pub struct ClientMsg{
    pub client_type:ClientType,
    pub client_id:u32,
    pub msg_enum:MsgEnum
}

//包含tcp链接后的所有原始数据
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

}

pub fn start_rw_loop(client:Client)
                     -> ClientSender {
    println!("start_rw_loop {0}", client.addr);
    let mut rd =client.rd;
    let mut wr=client.wr;

    //服务端通过这个通道给客户端发消息
    let (tx, mut rx):(Sender<Vec<u8>>,Receiver<Vec<u8>>) = mpsc::channel(10);
    // tokio::spawn(async move{
    //     while let Some(i) = rx.recv().await {
    //         println!("got = {}", i);
    //     });

    //send wait loop
    tokio::spawn(async move{
        loop {
            tokio::select! {
                    Some(i) = rx.recv() => {
                        println!("one send msg");
                        wr.write_all(i.as_slice()).await;
                        wr.flush().await;
                    },
                    else => break,
                }
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
        let mut receive_handler=ReceiveHandler::create();
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
            println!("client read size{}",n)
        }
        println!("client disconnect,rec loop end");
    });
    return ClientSender::new(tx.clone());
}

//接受消息输入到此，先使用packer进行解包
//解完后由receive handler处理，
struct ReceiveHandler{
    msg_pack_maker:msg_pack_make::MsgPackMaker
}
impl ReceiveHandler{
    pub fn create()->Self{
        return Self{
            msg_pack_maker:msg_pack_make::MsgPackMaker::create()
        }
    }
    pub fn handle_a_buffset(&mut self,buffset:&[u8],len:usize){
        self.msg_pack_maker.handle_a_buffset(buffset,len,||{

        });
    }
}

mod proto_util {
    use crate::{send_packer_head, protos};
    use crate::send_packer_head::PackIds;
    use protobuf::{CodedOutputStream, RepeatedField};
    use crate::net::MsgEnum;
    // use std::alloc::Global;

    pub fn buf2proto(buf :Vec<u8>,pack_id:u8) {
        MsgEnum::try_from()
        // //消息体 长度
        // let msg_body_len = proto_pack.compute_size() as usize;
        // //消息头
        // let msg_head = send_packer_head::make(
        //     pack_id, msg_body_len);
        // //给拷贝准备好空间
        // let mut final_vec = msg_head.to_vec();
        // final_vec.resize(5 + msg_body_len, 0);
        // // let final_vec_slice=final_vec.as_mut_slice();
        // // 流输出器
        // let mut stream =
        //     CodedOutputStream::bytes(&mut final_vec[5..]);
        // proto_pack.write_to(&mut stream);
        // stream.flush();
        // println!("player basic packed bodylen:{0}"
        //          , msg_body_len);
        // return final_vec
    }
}
mod msg_pack_make{
    use byteorder::{LittleEndian, BigEndian, ByteOrder};
    const MsgPackHeadSize:u8=5,
    //描述数据包头
    #[derive(Default, Debug)]
    struct MsgPackHead{
        pack_id:u8,
        pack_len:u32
    }

    //进行数据的解包
    #[derive(Default, Debug)]
    pub struct MsgPackMaker{
        pack_head:MsgPackHead,
        //描述包头
        head_buff:[u8;5],
        head_rec_cnt:u8,
        //描述包体
        body_buff:Vec<u8>,
        body_rec_cnt:u32,
    }
    impl MsgPackMaker{
        pub fn create()->Self{
            return Self::default();
        }
        pub fn handle_a_buffset<F>
            (&mut self,buffset:&[u8],_byte_cnt:usize, callback:F)
            where F:FnMut()
        {
            let mut handled_offset =0;

            while handled_offset<len {
                let byte_cnt_left = _byte_cnt - handled_offset;
                //头本次还是未收全
                if self.head_rec_cnt<MsgPackHeadSize {
                    for i in 0..byte_cnt_left {
                        self.head_buff[self.head_rec_cnt+i]=buffset[handled_offset+i];
                    }
                    self.head_rec_cnt+=byte_cnt_left;
                }
                //头本次收全
                else{
                    let cpylen=MsgPackHeadSize-self.head_rec_cnt;
                    for i in 0..cpylen {
                        self.head_buff[self.head_rec_cnt+i]=buffset[handled_offset+i];
                    }
                    handled_offset+=MsgPackHeadSize-self.head_rec_cnt;
                    self.calc_pack_head();
                    self.head_rec_cnt=MsgPackHeadSize;
                    //扩大缓冲区
                    if(self.pack_head.pack_len> self.body_buff.len() as u32){
                        self.body_buff.resize(self.body_buff.len());
                    }
                    continue;
                }
                // 1.剩余数据小于需要读的字节数量(不够
                if byte_cnt_left<
                    (self.pack_head.pack_len - self.body_rec_cnt) as usize {
                    self.write_data_2_body(&buffset[handled_offset..], byte_cnt_left);
                    return false;
                }
                else{
                    //完成读包
                    let len=self.pack_head.pack_len-self.body_rec_cnt;
                    self.write_data_2_body(&buffset[handled_offset..],len);
                    handled_offset+=len;

                    //对包进行解析
                }
            }
        }
        fn write_data_2_body(&mut self,buffset:&[u8],byte_cnt_left:usize){
            for i in 0..byte_cnt_left {
                self.body_buff[self.body_rec_cnt+i]=
                    buffset[i];
            }
        }
        fn calc_pack_head(&mut self){
            self.pack_head.pack_id=self.head_buff[0];
            self.pack_head.pack_len= BigEndian::read_u32(&self.head_buff[1..]);
        }
    }
}
