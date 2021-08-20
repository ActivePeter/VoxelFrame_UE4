use crate::base::*;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::{TcpListener, TcpStream};
use std::sync::Mutex;
use crate::get_game_context;
// use std::rc::Weak;

type PlayerId = i32;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pub struct PlayerChunkInfo {
    //用于存储上一次发送的区块，避免重复发送
    chunks_sent: HashMap<ChunkKey, SyncWeak<RwLock<Chunk>>>,
}

impl PlayerChunkInfo {
    fn new() -> Self {
        Self {
            chunks_sent: Default::default(),
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pub struct Player {
    new_in_world: bool,
    id: PlayerId,
    data: BaseEntityData,
    // chunks_sent: HashMap<ChunkKey, RwLock<SyncWeak<Chunk>>>,
    chunk_info: PlayerChunkInfo,
    pub socket: SyncWeak<RwLock<TcpStream>>,
}


impl Player {
    fn new(id_cnt: i32) -> ArcRw<Player> {
        let p = new!(Arc RwLock Player {
            new_in_world: true,
            id: id_cnt,
            data: BaseEntityData::new(),
            // chunks_sent: Default::default(),
            chunk_info: PlayerChunkInfo::new(),
            socket:Default::default()
        });

        // p.write().unwrap().check_chunk_load();

        return p;
    }
}

impl ITick for Player {
    fn tick(&mut self) {
        // self.check_chunk_load();
        // todo!()
    }
}

//call when first init& position change.
pub async fn async_player_check_chunk_load(p_ptr: ArcRw<Player>) {
    //首次进入游戏 或 区块坐标变化
    //首先计算player区块坐标
    let mut p = p_ptr.write().await;
    if p.new_in_world {
        p.new_in_world = false;
        //0.清空not sent
        // 因为之前未发送的已经无效了
        // self.chunk_info.chunks_not_loaded.clear();
        //1、遍历感兴趣范围的map里的chunk
        let mut new_chunks_sent: HashMap<ChunkKey, SyncWeak<RwLock<Chunk>>> = Default::default();
        let mut chunks_not_sent: LinkedList<ArcRw<Chunk>> = LinkedList::new();

        // let mut test: LinkedList<&Chunk> = LinkedList::new();
        // let mut mng = get_game_context();
        // mng.get_test();
        // 从原先的已发送队列中拿出有效的
        chunk_key_in_range_relative!(
                ck,
                {
                    let find = p.chunk_info.chunks_sent.get(&ck);
                    if let Some(s) = find
                    {
                        // new_chunks_sent_lock.write().unwrap().
                        new_chunks_sent.
                            insert(ck, s.clone());
                    } else {
                        //未发送的，加入到未发送序列
                        // let chunk =;
                        // chunks_not_sent

                        chunks_not_sent
                            // .write().unwrap()
                            .push_back(GAME_CONTEXT.lock().await.chunk_manager.get_chunk_by_chunk_key(ck));
                    }
                }
            );
        // 取代旧的已发送的，去除掉已经不在范围内的已发送的
        p.chunk_info.chunks_sent =
            new_chunks_sent
                // new_chunks_sent_lock.read().unwrap()
                .clone();

        //2、遍历not sent
        //未发送的区块，进行加载和加入player的发送队列（当前线程
        for chunk in chunks_not_sent {
            let p_clone = p_ptr.clone();
            tokio::spawn(async move {
                chunk.write().await.load().await;
                chunk.write().await.send(p_clone).await;
            });
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pub struct PlayerManager {
    id_cnt: PlayerId,
    pub player_id2detail: HashMap<PlayerId, ArcRw<Player>>,
    pub msg_list: LinkedList<Arc<Vec<u8>>>,
}

impl PlayerManager {
    pub fn new() -> PlayerManager {
        return PlayerManager {
            id_cnt: 0,
            player_id2detail: Default::default(),
            msg_list: Default::default(),
        };
    }
    pub fn add_player(&mut self) -> ArcRw<Player> {
        let a = self.player_id2detail.insert(
            self.id_cnt,
            Player::new(self.id_cnt),
        ).unwrap();
        self.id_cnt += 1;

        // let op = self.player_id2detail.get(&self.id_cnt);

        return a;
    }
    // pub fn setGame(&mut self, game:sync::Weak<RwLock<Game>>){
    //     self.game= game;
    // }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
