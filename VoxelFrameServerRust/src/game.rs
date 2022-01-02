use crate::base::*;
use std::ptr::null;
use std::rc::Weak;
use tokio::net::tcp::OwnedWriteHalf;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pub struct Game {
    pub chunk_manager: ChunkManager,
    pub player_manager: PlayerManager,
    pub
    ticks: i64,
    entity_cnt: u64,
    // pub ck: LinkedList<Chunk>,
}

impl Game {
    pub fn new() -> Game {
        return Game {
            chunk_manager: ChunkManager::new(),
            player_manager: PlayerManager::new(),
            ticks: 0,
            entity_cnt: 0,
            // ck: Default::default(),
        };
        // {
        //     let mut g = game.write().unwrap();
        //     g.chunkManager.write().unwrap().game = sync::Arc::downgrade(&game.clone());
        // }
        // game.chunkManager.setGame(sync::Arc::downgrade(&game));
        // return game;
    }
    fn new_entity(&mut self) -> entity::EntityData {
        let entity_data = entity::EntityData {
            entity_id: 0,
            position: Default::default(),
        };
        self.entity_cnt += 1;
        return entity_data;
    }
    pub async fn spawn_player_in_game(&mut self, socketWr: &ArcRw<OwnedWriteHalf>) {
        //1.playerManager.add_player
        let player_ptr = self.player_manager.add_player().await;
        //1.1 bind socket
        player_ptr.write().await.socket = Arc::downgrade(socketWr);
        //1.3 gen_entity_for_player
        self.gen_entity_for_player(&player_ptr).await;
    }
    pub async fn gen_entity_for_player(&mut self, player_ptr: &ArcRw<Player>) {
        //分配实体id
        let entity_data = self.new_entity();
        player_ptr.write().await.entity_data = entity_data;
    }
    pub fn start(&self) {}
}

impl ITick for Game {
    fn tick(&mut self) {
        // todo!()
        self.ticks += 1;

        for (_, p) in &mut self.player_manager.player_id2detail {
            // p.tick();
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
            player_id2detail: HashMap::new(),
            msg_list: Default::default(),
        };
    }
    pub async fn add_player(&mut self) -> ArcRw<Player> {
        println!("player{0} added.", self.id_cnt);
        let a = Player::new(self.id_cnt);
        // let a =
        self.player_id2detail.insert(self.id_cnt, a.clone());
        // self.player_id2detail.insert(
        //     self.id_cnt,
        //     a.clone(),
        // ).unwrap();
        // println!("error {}", a);

        let op = self.player_id2detail.get(&self.id_cnt).unwrap();
        self.id_cnt += 1;


        println!("player id:{0}", op.read().await.id);

        return a;
    }
    // pub fn setGame(&mut self, game:sync::Weak<RwLock<Game>>){
    //     self.game= game;
    // }
}