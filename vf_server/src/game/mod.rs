pub mod chunk;
pub(crate) mod player;
pub(crate) mod game_flow;
pub(crate) mod entity;
pub(crate) mod block;
pub(crate) mod pack_distribute;
pub(crate) mod operation;
mod chunk_terrain;

pub use crate::*;
use std::collections::{HashMap, HashSet};
use tokio::net::TcpStream;
// use std::any::Any;
use game::player::{Player, PlayerId};
use crate::conv::point3f_2_chunkkey;
use game::chunk::{ChunkKey, Chunk};
use std::net::SocketAddr;
use tokio::net::windows::named_pipe::PipeEnd::Client;
// use std::borrow::{Borrow, BorrowMut};
use std::thread::{spawn, LocalKey};
use tokio::sync::{mpsc, oneshot};
use tokio::task;
use crate::net::{ClientMsg, ClientMsgEnum, ClientDescription, ClientSender};
// use std::borrow::Borrow;
use crate::protos::common::ClientType;
use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
use crate::part_server_sync::PartServerSync;
use game::entity::{EntityId, EntityData};
use crate::net_pack_convert::MsgEnum;
use crate::net_pack_convert::MsgEnum::MainPlayerMoveCmd;
use crate::async_task::AsyncTaskManager;
use crate::game::player::PlayerManager;

pub type ClientId = usize;
pub type ClientOperationId=u32;
// use crate::client::Client;

// tokio::task_local! {
//     static game_local: RefCell<Game>;
// }

// #[derive(Default, Debug)]
// struct ServerContext{
//     client_manager:ClientManager,
//     game:Game,
//     part_server_sync:PartServerSync,
// }
// impl ServerContext{
//     fn create() -> ServerContext {
//         ServerContext{
//             client_manager:ClientManager{
//                 next_client_id: 0,
//                 id2client: Default::default(),
//                 partserver_clients: Default::default(),
//                 player_clients: Default::default()
//             },
//             game:Game::create(),
//             part_server_sync:PartServerSync::create(),
//         }
//     }

// }
// #[derive(Default, Debug)]
pub struct ClientManager{
    next_client_id:ClientId,
    pub(crate) id2client :HashMap<ClientId,ClientDescription>,
    partserver_clients:HashSet<ClientId>,
    player_clients:HashSet<ClientId>,
}
impl ClientManager{
    pub fn get_player_sender(&self, player: &Player) -> ClientSender {
        return self.id2client.get(&(player.client_id)).unwrap().sender.clone();
    }

    //make sure the cid is valid before call
    pub fn get_sender(&self,cid:ClientId)->ClientSender{
        return self.id2client.get(&(cid)).unwrap().sender.clone();
    }
    pub fn add_new_client(&mut self, mut sender:ClientSender, client_type:ClientType) ->ClientId{
        // sender.id=self.next_client_id;
        self.id2client.insert(self.next_client_id,ClientDescription{
            client_type,
            sender,
            client_id: self.next_client_id
        });
        if(client_type==ClientType_GameServer){
            self.partserver_clients.insert(self.next_client_id);
        }else{
            self.player_clients.insert(self.next_client_id);
        }
        self.next_client_id+=1;

        return self.next_client_id-1;
    }
    pub fn remove_client(&mut self,cid:ClientId){
        let ctype=self.id2client.get(&cid).unwrap().client_type;
        if ctype==ClientType_GameServer{
            self.partserver_clients.remove(&cid);
        }else{
            self.player_clients.remove(&cid);
        }
        self.id2client.remove(&cid);
    }
    pub fn get_clienttype(&self,cid:ClientId)->Option<ClientType>{
        match self.id2client.get(&cid){
            None => {None}
            Some(c) => {
                Some(c.client_type)
            }
        }
    }
}
pub struct Game {
    //enityid_entity_map:

    //游戏逻辑
    pub entity_cnt: u32,
    pub chunks: HashMap<chunk::ChunkKey, chunk::Chunk>,
    pub entities: HashMap<EntityId, EntityData>,
    // pub entities: HashMap<EntityId,<EntityData>>,
    pub player_manager: PlayerManager,

    //网络相关
    pub client_manager:ClientManager,
    pub part_server_sync:PartServerSync,
    pub async_task_manager:AsyncTaskManager,
}

impl Game {
    pub fn create() -> Game {
        Game {
            entity_cnt: 0,
            chunks: Default::default(),
            entities: Default::default(),
            player_manager: PlayerManager::create_once(),

            client_manager:ClientManager{
                next_client_id: 0,
                id2client: Default::default(),
                partserver_clients: Default::default(),
                player_clients: Default::default()
            },
            part_server_sync:PartServerSync::create(),
            async_task_manager: AsyncTaskManager::create(),
        }
    }


    pub fn entity_get(&self, entity_id: &EntityId) -> Option<&EntityData> {
        return self.entities.get(entity_id);
    }
    pub fn entity_get_mut(&mut self, entity_id: &EntityId) -> Option<&mut EntityData> {
        return self.entities.get_mut(entity_id);
    }

    // fn  add_chunks_be_interested_by(
    //     &mut self, player:&Rc<RefCell<Player>>, player_entity:Rc<RefCell<EntityData>>){
    //
    //     // let p_ck=point3f_2_chunkkey(player_entity.borrow_mut()..position);
    //     //
    //     //  iter_relative_chunk_key_in_interest_range!(
    //     //     relate_ck,
    //     //     {
    //     //         let ck=p_ck.plus(relate_ck);
    //     //         let chunk=self.get_chunk(&ck);
    //     //         chunk.add_be_interested_by(player.borrow_mut().player_id.clone());
    //     //     }
    //     // )
    // }
    // fn add_player_entity_2_chunk(&mut self, player: &, entity: &EntityData) {
    //     //1.根据位置计算chunk_key
    //     let chunk_key = point3f_2_chunkkey(&entity.position);
    //     //2.获取区块
    //     let chunk = self.get_chunk(&chunk_key);
    //     //3.entity
    //     chunk.entities.push_back(entity.entity_id);
    //     //4.player
    //     chunk.players.push_back(player.player_id);
    //     // //5.interested_by
    //     // chunk.be_interested_by(player.player_id);
    // }
    //
    /*
        若没有则进行初始化
        创建chunk同时要将其加入到part server sync中
    */
    // ->返回区块
    pub async fn chunk_get_mut(&mut self, chunk_key: &ChunkKey) -> &mut Chunk {
        let a=self.chunks.contains_key(chunk_key);
        // let chunk=self.chunks.get_mut(chunk_key);
        // match chunk {
        //     None => {
        if(a){
            return self.chunks.get_mut(chunk_key).unwrap();
        }else{
            let ck=chunk::Chunk::new_and_load(&mut self.chunks, chunk_key);
            self.chunks.insert(chunk_key.clone(),ck );
            // .add_free_chunk(chunk_key.clone());
            part_server_sync::add_free_chunk(self,chunk_key.clone()).await;
            return self.chunks.get_mut(chunk_key).unwrap();
        }
        // }
        // Some(chunk) => {
        // }
        // }
        // let chunk = self.chunks
        //     .entry(*chunk_key)
        //     .or_insert(chunk::Chunk::new_and_load(chunk_key));

    }
    pub fn chunk_get(&self, chunk_key: &ChunkKey) -> Option<&Chunk> {
        return self.chunks.get(chunk_key);
    }
    // // pub fn get_chunk(&self, chunk_key:& ChunkKey) -> & Chunk {
    // //     let chunk=self.chunks
    // //         .entry(*chunk_key)
    // //         .or_insert(chunk::Chunk::new_and_load(chunk_key));
    // //     return chunk;
    // // }
}


pub struct GameMainLoopChannels {
    pub msg_channel_tx: mpsc::Sender<ClientMsgEnum>,
}

impl Clone for GameMainLoopChannels {
    fn clone(&self) -> Self {
        return GameMainLoopChannels {
            msg_channel_tx: self.msg_channel_tx.clone(),
        };
    }
}

pub async fn main_loop()
    -> GameMainLoopChannels
{
    println!("main_loop()");

    let (msg_channel_tx, mut msg_channel_rx):
        (mpsc::Sender<ClientMsgEnum>, mpsc::Receiver<ClientMsgEnum>)
        = mpsc::channel(10);


    let game_channels = GameMainLoopChannels {
        msg_channel_tx,
    };
    //
    // let mut game = game::Game::create();
    // local.spawn_local(async move {
    tokio::spawn(async move {
        let mut context =game::Game::create();
        // let mut packhandlers=game_pack_distribute::create_packhandlers();
        // task::spawn_local(async move{
        println!("game main loop task spawned");
        loop {
            let msg=  msg_channel_rx.recv().await.unwrap();

            pack_distribute::
            distribute_client_common_msg(&mut context,msg).await;
            // match msg {
            //     ClientStateMsg::ClientConnect(s)=>{
            //
            //     }
            // }

            // tokio::select! {
            //     // Some(client_sender) = new_player_channel_rx.recv() => {
            //     //     println!("new player in msg rx");
            //     //     // game.spawn_player(client_sender).await;
            //     // },
            //
            //     Some(msg)=msg_channel_rx.recv()=>{
            //         println!("client msg to game loop")
            //     },
            //     else => break,
            // }
        }
        println!("game main loop task end");
        // });
    });

    return game_channels;
}

// pub async fn test(mut rx:mpsc::Receiver<ClientSender>){
// let a:HashMap<EntityId,Rc<RefCell<EntityData>>>=Default::default();

// }