use crate::*;
use std::collections::HashMap;
use crate::game_player_manager::PlayerManager;
use tokio::net::TcpStream;
use std::any::Any;
use crate::player::{Player, PlayerId};
use crate::conv::point3f_2_chunkkey;
use crate::entity::{EntityData, EntityId};
use crate::chunk::{ChunkKey, Chunk};
use std::net::SocketAddr;
use tokio::net::windows::named_pipe::PipeEnd::Client;
// use std::borrow::{Borrow, BorrowMut};
use std::thread::{spawn, LocalKey};
use tokio::sync::{mpsc, oneshot};
use crate::client::ClientSender;
use tokio::task;

// use crate::client::Client;

// tokio::task_local! {
//     static game_local: RefCell<Game>;
// }

pub struct Game {
    //enityid_entity_map:
    pub entity_cnt: u32,
    pub chunks: HashMap<chunk::ChunkKey, chunk::Chunk>,
    pub entities: HashMap<EntityId, EntityData>,
    // pub entities: HashMap<EntityId,<EntityData>>,
    pub player_manager: PlayerManager,
}

impl Game {
    pub fn create() -> Game {
        Game {
            entity_cnt: 0,
            chunks: Default::default(),
            entities: Default::default(),
            player_manager: PlayerManager::create_once(),
        }
    }

    //entity related operations
    fn entity_spawn(&mut self) -> u32 {
        let entity =
            self.entities.entry(self.entity_cnt)
                .or_insert(
                    entity::EntityData {
                        entity_id: self.entity_cnt,
                        position: base_type::point3f_new(),
                        entity_type: 0,
                    }
                );
        // let entity=

        let entity_id = self.entity_cnt;
        self.entity_cnt += 1;

        return entity_id;
    }
    fn entity_get(&self, entity_id: &EntityId) -> Option<&EntityData> {
        return self.entities.get(entity_id);
    }
    fn entity_get_mut(&mut self, entity_id: &EntityId) -> Option<&mut EntityData> {
        return self.entities.get_mut(entity_id);
    }

    //chunk related operations
    //chunk 加入玩家
    fn chunk_add_player(&mut self,
                        playerid: PlayerId,
                        player_entity_id: EntityId,
    ) {
        let entity = self.entity_get(&player_entity_id).unwrap();
        //1.根据位置计算chunk_key
        let chunk_key = point3f_2_chunkkey(&entity.position);
        //2.获取区块
        let chunk = self.chunk_get_mut(&chunk_key);
        //3.entity
        chunk.entities.push_back(player_entity_id);
        //4.player
        chunk.players.push_back(playerid);
    }
    fn chunks_add_be_interested(
        &mut self,
        playerid: PlayerId,
        player_entity_id: EntityId,
    ) {
        let entity = self.entity_get(&player_entity_id).unwrap();
        let p_ck = point3f_2_chunkkey(&entity.position);

        iter_relative_chunk_key_in_interest_range!(
            relate_ck,
            {
                let ck=p_ck.plus(relate_ck);
                let chunk=self.chunk_get_mut(&ck);
                chunk.add_be_interested_by(playerid);
            }
        )
    }
    //基本函数
    pub async fn spawn_player(&mut self,client_sender:ClientSender) {
        // //1.获取player码以及绑定tcp
        let playerid =
            (self).player_manager.create_player_and_bind_client(client_sender);
        //
        //
        // entity=(self).spawn_entity_for_player(&player);

        //2.出生entity 这个过程是产生entity，
        let player_entity_id = self.entity_spawn();

        //2.5产生完entity id 就与player绑定
        {
            let p = self.player_manager.get_player_handle(&playerid).unwrap();
            p.entity_id = player_entity_id;
        }

        //3.将player id 和entity id 加入区块
        // self.add_player_entity_2_chunk(&player, entity);
        {
            self.chunk_add_player(playerid, player_entity_id);
        }

        //4.刷新被感兴趣的区块
        self.chunks_add_be_interested(playerid, player_entity_id);

        // 5.发送玩家进入后的全部内容
        {
            let player=
                self.player_manager.playerid_2_player.get(&playerid).unwrap();
            let entity=self.entity_get(&player_entity_id).unwrap();
            // 1.player基本信息（player_entity_id
            send::player_basic(player,entity).await;
            // 2.区块地形
            send::player_interested_chunk_block_data(
                player,entity,self
            ).await;
            // 3.感兴趣区块的entity数据
            send::player_interested_chunk_entity_data(
                player,entity,self
            ).await;
        }
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
    pub fn chunk_get_mut(&mut self, chunk_key: &ChunkKey) -> &mut Chunk {
        let chunk = self.chunks
            .entry(*chunk_key)
            .or_insert(chunk::Chunk::new_and_load(chunk_key));
        return chunk;
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
    pub new_player_channel_tx: mpsc::Sender<ClientSender>,
}

impl Clone for GameMainLoopChannels {
    fn clone(&self) -> Self {
        return GameMainLoopChannels {
            new_player_channel_tx: self.new_player_channel_tx.clone()
        };
    }
}

pub async fn main_loop() -> GameMainLoopChannels {
    println!("main_loop()");

    let (new_player_channel_tx, mut new_player_channel_rx):
        (mpsc::Sender<ClientSender>, mpsc::Receiver<ClientSender>)
        = mpsc::channel(10);
    //
    let game_channels = GameMainLoopChannels {
        new_player_channel_tx,
    };
    //
    let mut game = game::Game::create();
    let local = task::LocalSet::new();
    // local.spawn_local(async move {
    tokio::spawn(async move {

        // task::spawn_local(async move{
        println!("game main loop task spawned");
        loop {
            tokio::select! {
                Some(client_sender) = new_player_channel_rx.recv() => {
                    println!("new player in msg rx");
                    game.spawn_player(client_sender).await;
                },
                else => break,
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