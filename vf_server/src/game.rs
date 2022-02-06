use crate::*;
use std::collections::{HashMap, HashSet};
use crate::game_player_manager::PlayerManager;
use tokio::net::TcpStream;
use std::any::Any;
use crate::game_player::{Player, PlayerId};
use crate::conv::point3f_2_chunkkey;
use crate::game_chunk::{ChunkKey, Chunk};
use std::net::SocketAddr;
use tokio::net::windows::named_pipe::PipeEnd::Client;
// use std::borrow::{Borrow, BorrowMut};
use std::thread::{spawn, LocalKey};
use tokio::sync::{mpsc, oneshot};
use tokio::task;
use crate::net::{ClientMsg, ClientMsgEnum, ClientDescription, ClientSender};
use std::borrow::Borrow;
use crate::protos::common::ClientType;
use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
use crate::part_server_sync::PartServerSync;
use crate::game_entity::{EntityId, EntityData};
use crate::net_pack_convert::MsgEnum;
use crate::net_pack_convert::MsgEnum::MainPlayerMoveCmd;
use crate::async_task::AsyncTaskManager;

pub type ClientId = usize;
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
    fn add_new_client(&mut self, mut sender:ClientSender, client_type:ClientType) ->ClientId{
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
    fn remove_client(&mut self,cid:ClientId){
        let ctype=self.id2client.get(&cid).unwrap().client_type;
        if ctype==ClientType_GameServer{
            self.partserver_clients.remove(&cid);
        }else{
            self.player_clients.remove(&cid);
        }
        self.id2client.remove(&cid);
    }
}
pub struct Game {
    //enityid_entity_map:

    //游戏逻辑
    pub entity_cnt: u32,
    pub chunks: HashMap<game_chunk::ChunkKey, game_chunk::Chunk>,
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
    // //若没有则进行初始化
    //创建chunk同时要将其加入到part server sync中
    pub async fn chunk_get_mut(&mut self, chunk_key: &ChunkKey) -> &mut Chunk {
        let a=self.chunks.contains_key(chunk_key);
        // let chunk=self.chunks.get_mut(chunk_key);
        // match chunk {
        //     None => {
            if(a){
                return self.chunks.get_mut(chunk_key).unwrap();
            }else{

                self.chunks.insert(chunk_key.clone(), game_chunk::Chunk::new_and_load(chunk_key));
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
        // task::spawn_local(async move{
        println!("game main loop task spawned");
        loop {
            let msg=  msg_channel_rx.recv().await.unwrap();
            match msg {
                ClientMsgEnum::ClientCommonMsg(common_msg) => {

                    match common_msg.msg_enum{
                        MsgEnum::ClientFirstConfirm(_) => {}
                        MsgEnum::EntityPos(_) => {}
                        MsgEnum::PlayerBasic(_) => {}
                        MsgEnum::ChunkPack(_) => {}
                        MsgEnum::ChunkEntityPack(_) => {}
                        MsgEnum::MainPlayerMoveCmd(cmd) => {
                            if common_msg.client_type==ClientType::ClientType_Player {
                                game_player::handle_MainPlayerMoveCmd(common_msg.client_id,
                                    &mut context, cmd).await;
                            }
                        }
                        MsgEnum::Rpl_SpawnEntityInPs(rpl) => {
                            if common_msg.client_type==ClientType_GameServer{
                                async_task::spawn_entity_in_ps_rpl(&mut context,rpl).await;
                            }
                        }
                    }
                }
                ClientMsgEnum::ClientConnect(m) => {
                    let id=
                        context.client_manager.add_new_client(m.sender,m.client_type);
                    m.response.send(id);
                    game_flow::after_client_connect(&mut context, id).await;
                }
                ClientMsgEnum::ClientDisconnect(m) => {
                    context.client_manager.remove_client(m.client_id);
                }
            }
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