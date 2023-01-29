pub mod chunk;
pub(crate) mod player;
pub(crate) mod entity;
pub(crate) mod block;
pub(crate) mod pack_distribute;
pub(crate) mod operation;
mod chunk_terrain;
mod terrain_gen;

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
use std::thread::{spawn, LocalKey, sleep};
use tokio::sync::{mpsc, oneshot, Mutex};
use tokio::task;
use crate::net::{ClientMsg, ClientMsgEnum, ClientDescription, ClientSender};
// use std::borrow::Borrow;
use crate::protos::common::ClientType;
use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
use crate::part_server_sync::PartServerSync;
use game::entity::{EntityId, EntityData};
use crate::net_pack::MsgEnum;
use crate::net_pack::MsgEnum::MainPlayerMoveCmd;
use crate::async_task::AsyncTaskManager;
use crate::game::player::PlayerManager;
use tokio::time::Duration;
use std::sync::atomic::AtomicI32;
use std::sync::atomic::Ordering::{Relaxed, Release, Acquire};
use crate::event::chunk_event;
use crate::game::chunk::{VF_CHUNK_WIDTH, ChunkLoadStage};
use crate::game::terrain_gen::tree::TerrainGenTree;
use std::cell::Cell;
use std::collections::hash_map::RandomState;

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
    _chunks: RefCell<HashMap<chunk::ChunkKey, chunk::Chunk>>,
    _next_entity_id: RefCell<u32>,
    _entities: RefCell<HashMap<EntityId, EntityData>>,
    // pub entities: HashMap<EntityId,<EntityData>>,
    _player_manager: RefCell<PlayerManager>,

    //网络相关
    _client_manager:RefCell<ClientManager>,
    _part_server_sync:RefCell<PartServerSync>,
    _async_task_manager:RefCell<AsyncTaskManager>,
}
unsafe impl Send for Game{}
unsafe impl Sync for Game{}

impl Game {
    pub fn new() -> Game {
        Game {
            _next_entity_id: RefCell::new(0),
            _chunks: Default::default(),
            _entities: Default::default(),
            _player_manager: RefCell::new(PlayerManager::create_once()),

            _client_manager:RefCell::new(ClientManager{
                next_client_id: 0,
                id2client: Default::default(),
                partserver_clients: Default::default(),
                player_clients: Default::default()
            }),
            _part_server_sync:RefCell::new(PartServerSync::new()),
            _async_task_manager: RefCell::new(
                AsyncTaskManager::new()
            ),
        }
    }
    pub(crate) fn tick(&mut self){

    }
    pub fn entity_get(&self, entity_id: &EntityId) -> Option<&EntityData> {
        return self.entities_ref().get(entity_id);
    }
    pub fn entity_get_mut(&mut self, entity_id: &EntityId) -> Option<&mut EntityData> {
        return self.entities_mut().get_mut(entity_id);
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

    pub(crate) fn entities_ref(&self) -> &HashMap<EntityId, EntityData, RandomState> {
        unsafe{& *self._entities.as_ptr()}
    }
    pub(crate) fn entities_mut(&self) -> &mut HashMap<EntityId, EntityData, RandomState> {
        unsafe{&mut *self._entities.as_ptr()}
    }
    pub(crate) fn part_server_mut(&self) -> &mut PartServerSync {
        unsafe{&mut *self._part_server_sync.as_ptr()}
    }
    pub(crate) fn part_server_ref(&self) -> & PartServerSync {
        unsafe{& *self._part_server_sync.as_ptr()}
    }
    pub fn chunks_ref(&self) -> & HashMap<ChunkKey, Chunk> {
        unsafe{& *self._chunks.as_ptr() }
    }
    pub fn chunks_mut(&self) -> &mut HashMap<ChunkKey, Chunk> { unsafe{&mut *self._chunks.as_ptr() } }

    pub fn client_man_ref(&self) -> &ClientManager { unsafe{& *self._client_manager.as_ptr() } }
    pub fn client_man_mut(&self) -> &ClientManager { unsafe{&mut *self._client_manager.as_ptr() } }

    pub fn player_man_ref(&self) -> &PlayerManager {
        unsafe{& *self._player_manager.as_ptr()}
    }
    pub fn player_man_mut(&self) -> &mut PlayerManager {
        unsafe{&mut *self._player_manager.as_ptr()}
    }
    pub fn async_task_man_mut(&self) -> &mut AsyncTaskManager {
        unsafe{&mut *self._async_task_manager.as_ptr()}
    }
    pub fn async_task_man_ref(&self) -> &AsyncTaskManager {
        unsafe{& *self._async_task_manager.as_ptr()}
    }
    pub fn next_entity_id_ref(&self) -> &u32 {
        unsafe{& *self._next_entity_id.as_ptr()}
    }
    pub fn next_entity_id_mut(&self) -> &mut u32 {
        unsafe{& mut *self._next_entity_id.as_ptr()}
    }

    // if it's new, we dont ask it to load the whole
    pub fn try_load_chunk_get_mut(&self, chunk_key:&ChunkKey) -> &mut Chunk {
        let a=self.chunks_ref().contains_key(chunk_key);

        if(a){
            return self.chunks_mut().get_mut(chunk_key).unwrap();
        }else{
            let ck=chunk::Chunk::new_and_load(self.chunks_mut(), chunk_key);
            self.chunks_mut().insert(chunk_key.clone(),ck );
            return self.chunks_mut().get_mut(chunk_key).unwrap();
        }
    }
    /*
        若没有则进行初始化
        创建chunk同时要将其加入到part server sync中
    */
    // ->返回区块
    pub async fn chunk_get_mut_loaded(&self, chunk_key: &ChunkKey) -> &mut Chunk {
        let need_gen={
            let ck = self.try_load_chunk_get_mut(chunk_key);
            if ck.load_stage == ChunkLoadStage::Pre {
                ck.load_stage = ChunkLoadStage::End;
                true
            }else{
                false
            }
        };
        if need_gen{
            // println!("chunk {} {} {} gen tree",chunk_key.x,chunk_key.y,chunk_key.z);
            self.terrain_gen_tree(chunk_key);
            // sync to part server
            part_server_sync::add_free_chunk(self,chunk_key.clone()).await;
        }
        // .add_free_chunk(chunk_key.clone());
        return self.try_load_chunk_get_mut(chunk_key);

        // }
        // Some(chunk) => {
        // }
        // }
        // let chunk = self.chunks
        //     .entry(*chunk_key)
        //     .or_insert(chunk::Chunk::new_and_load(chunk_key));

    }
    pub fn chunk_get(&self, chunk_key: &ChunkKey) -> Option<&Chunk> {
        return self.chunks_ref().get(chunk_key);
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
        msg_channel_tx:msg_channel_tx.clone(),
    };
    let tick_task_tx=msg_channel_tx;
    let tickcnt=Arc::new(AtomicI32::new(0));
    {
        let tickcnt=tickcnt.clone();
        tokio::spawn(async move{
            tickcnt.store(1,Release);
            tick_task_tx.send(ClientMsgEnum::Tick).await.unwrap();
            sleep(Duration::from_millis(20));
        });
    }
    //
    // let mut game = game::Game::create();
    // local.spawn_local(async move {
    tokio::spawn(async move {
        let mut context = game::Game::new();
        // let mut packhandlers=game_pack_distribute::create_packhandlers();
        // task::spawn_local(async move{
        println!("game main loop task spawned");
        loop {
            let msg=  msg_channel_rx.recv().await.unwrap();
            if let ClientMsgEnum::Tick=msg{
                tickcnt.store(0,Release);
                // chunk_event::sync_all_change_to_clients::call();
                for i in 0..50{
                    context.tick();
                    //新的超时
                    if tickcnt.load(Acquire)==1{
                        println!("too slow, ticks havent done, left {}",50-i);
                        break;
                    }
                }
            }else{
                pack_distribute::
                distribute_client_common_msg(&mut context,msg).await;
            }
        }
        println!("game main loop task end");
        // });
    });

    return game_channels;
}

// pub async fn test(mut rx:mpsc::Receiver<ClientSender>){
// let a:HashMap<EntityId,Rc<RefCell<EntityData>>>=Default::default();

// }