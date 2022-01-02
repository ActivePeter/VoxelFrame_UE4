use crate::*;
use std::collections::HashMap;
use crate::game_player_manager::PlayerManager;
use tokio::net::TcpStream;
use std::any::Any;
use crate::player::Player;
use crate::conv::point3f_2_chunkkey;
use crate::entity::{EntityData, EntityId};
use crate::chunk::{ChunkKey, Chunk};
use std::net::SocketAddr;
use tokio::net::windows::named_pipe::PipeEnd::Client;
use std::borrow::Borrow;

// use crate::client::Client;

pub struct Game {
    //enityid_entity_map:
    entity_cnt: u32,
    chunks: HashMap<chunk::ChunkKey,chunk::Chunk>,
    // entities: HashMap<EntityId,Arc<RwLock<EntityData>>>,
    entities: HashMap<EntityId,Rc<RefCell<EntityData>>>,
    player_manager:PlayerManager
}
impl Game{
    pub fn create()->Game{
        Game{
            entity_cnt:0,
            chunks:Default::default(),
            entities:Default::default(),
            player_manager:PlayerManager::create_once()
        }
    }

    pub async fn spawn_player(&mut self,socket:TcpStream,addr:SocketAddr){
        let player;
        let entity;

        //1.获取player码以及绑定tcp
        (player)=(self).player_manager.create_player();

        //2.出生entity
        entity=(self).spawn_entity_for_player(player.borrow());

        //3.刷新被感兴趣的区块
        self.add_chunks_be_interested_by(player.borrow(),&entity);

        //4.创建读取循环 和 发送循环
        let mut client =client::Client::create(socket,addr);
        let tx= client.start_rw_loop(player.clone());
        player.borrow_mut().sender=Some(tx);

        //5.发送玩家进入后的全部内容
        {
            // 1.player基本信息（player_entity_id
            send::player_basic(player.clone(),entity.clone());
            // 2.区块地形
            send::player_interested_chunk_block_data(
                player.clone(),entity.clone(),self
            );
            // 3.感兴趣区块的entity数据
            send::player_interested_chunk_entity_data(
                player.clone(),entity.clone(),self
            );
        }
    }
    fn add_chunks_be_interested_by(&mut self,player:&Rc<RefCell<Player>>,player_entity:&Rc<RefCell<EntityData>>){
        let p_ck=point3f_2_chunkkey(&player_entity.borrow_mut().position);

         iter_relative_chunk_key_in_interest_range!(
            relate_ck,
            {
                let ck=p_ck.plus(relate_ck);
                let chunk=self.get_chunk(&ck);
                chunk.add_be_interested_by(player.borrow_mut().player_id.clone());
            }
        )
    }
    fn spawn_entity_for_player(&mut self, player:&Rc<RefCell<Player>>) -> Rc<RefCell<EntityData>> {
        //1.加入entities列表
        let entity=
            self.entities.entry(self.entity_cnt)
                .or_insert(
                    Rc::new(RefCell::from(
                        entity::EntityData{
                            entity_id: self.entity_cnt,
                            position: base_type::point3f_new(),
                            entity_type: 0
                        }
                    ))
                ).clone();
        // let entity=
        player.borrow_mut().entity_id=self.entity_cnt;
        self.entity_cnt+=1;

        //2.加入区块
        self.add_player_entity_2_chunk(player,entity.clone());

        return entity.clone();
    }
     fn add_player_entity_2_chunk(&mut self,player:&Rc<RefCell<Player>>,entity:Rc<RefCell<EntityData>>){
        //1.根据位置计算chunk_key
        let chunk_key=point3f_2_chunkkey(&entity.borrow_mut().position);
        //2.获取区块
        let chunk=self.get_chunk(&chunk_key);
        //3.entity
        chunk.entities.push_back(entity.borrow_mut().entity_id);
        //4.player
        chunk.players.push_back(player.borrow_mut().player_id);
        // //5.interested_by
        // chunk.be_interested_by(player.player_id);
    }

    //若没有则进行初始化
    pub fn get_chunk(&mut self, chunk_key:& ChunkKey) -> & mut Chunk {
        let chunk=self.chunks
            .entry(*chunk_key)
            .or_insert(chunk::Chunk::new_and_load(chunk_key));
        return chunk;
    }
    // pub fn get_chunk(&self, chunk_key:& ChunkKey) -> & Chunk {
    //     let chunk=self.chunks
    //         .entry(*chunk_key)
    //         .or_insert(chunk::Chunk::new_and_load(chunk_key));
    //     return chunk;
    // }
}