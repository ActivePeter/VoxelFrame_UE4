use crate::base::*;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::{TcpListener, TcpStream};
use std::sync::Mutex;
use crate::base::collections::hash_map::OccupiedError;
use tokio::net::tcp::{WriteHalf, OwnedWriteHalf};
use crate::conv::conv_point3f_2_chunk_key;
// use std::rc::Weak;

pub type PlayerId = i32;

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
    pub new_in_world: bool,
    pub id: PlayerId,
    // data: BaseEntityData,
    // chunks_sent: HashMap<ChunkKey, RwLock<SyncWeak<Chunk>>>,
    pub chunk_info: PlayerChunkInfo,
    pub socket: SyncWeak<RwLock<OwnedWriteHalf>>,
    pub entity_data: entity::EntityData,
}


impl Player {
    pub(crate) fn new(id_cnt: i32) -> ArcRw<Player> {
        let p = ArcRw_new!(Player {
            new_in_world: true,
            id: id_cnt,
            // data: BaseEntityData::new(),
            // chunks_sent: Default::default(),
            chunk_info: PlayerChunkInfo::new(),
            socket:Default::default(),


            entity_data:Default::default()
        });

        // p.write().unwrap().check_chunk_load();

        return p;
    }
    fn get_chunk_key(&self) -> ChunkKey {
        return conv_point3f_2_chunk_key(&self.data.position);
    }
}

impl ITick for Player {
    fn tick(&mut self) {
        // self.check_chunk_load();
        // todo!()
    }
}

// pub async fn player_first_in_world(p_ptr: ArcRw<Player>) {
//     async_player_check_chunk_load().await;
// }

//call when first init& position change.
pub async fn async_player_check_chunk_load(p_ptr: ArcRw<Player>) {
    //首次进入游戏 或 区块坐标变化
    //首先计算player区块坐标


    if p_ptr.read().await.new_in_world {
        // let mut p = p_ptr.write().await;

        p_ptr.write().await.new_in_world = false;
        //0.清空not sent
        // 因为之前未发送的已经无效了
        // self.chunk_info.chunks_not_loaded.clear();

        //1、遍历感兴趣范围的map里的chunk
        let mut new_chunks_sent: HashMap<ChunkKey, SyncWeak<RwLock<Chunk>>> = Default::default();
        let mut chunks_not_sent: LinkedList<ArcRw<Chunk>> = LinkedList::new();

        // let mut test: LinkedList<&Chunk> = LinkedList::new();
        // let mut mng = get_game_context();
        // mng.get_test();

        //计算玩家区块
        let p_ck = p_ptr.read().await.get_chunk_key();

        // 从原先的已发送队列中拿出有效的

        chunk_key_in_range_relative!(
                relate_ck,//相对位置
                {
                    let ck=p_ck.plus(relate_ck);

                    let p=p_ptr.read().await;
                    let find=p.chunk_info.chunks_sent.get(&ck).clone();

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
                            .push_back(GAME_CONTEXT.
                            lock().await.
                            chunk_manager.get_chunk_by_chunk_key(&ck));
                    }
                }
            );
        // 取代旧的已发送的，去除掉已经不在范围内的已发送的
        p_ptr.write().await.chunk_info.chunks_sent =
            new_chunks_sent
                // new_chunks_sent_lock.read().unwrap()
                .clone();


        //2、遍历not sent
        //未发送的区块，进行加载和加入player的发送队列（当前线程
        let mut a = 0;
        for chunk in chunks_not_sent
        {
            // let chunk = chunks_not_sent.front().unwrap().clone();
            let p_clone = p_ptr.clone();
            tokio::spawn(async move {
                chunk.write().await.load().await;
                // chunk.read().await.send(p_clone).await;
                send::chunk_2_player(chunk, p_clone).await;
                println!("sent cnt:{0}", a);
                a = a + 1;
            });
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
