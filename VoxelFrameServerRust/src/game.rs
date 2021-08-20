use crate::base::*;
use std::ptr::null;
use std::rc::Weak;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
pub struct BaseEntityData {
    // #[serde(rename = "Pos")]
    pub position: Point3f,
    // #[serde(rename = "Rotation")]
    // pub rotation: ArrayVec<f32, 2>,
    // #[serde(rename = "Motion")]
    // pub velocity: ArrayVec<f64, 3>,
}

impl BaseEntityData {
    pub fn new() -> Self {
        BaseEntityData {
            position: Point3f::new()
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pub trait ITick {
    fn tick(&mut self);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pub struct Game {
    pub chunk_manager: ChunkManager,
    pub player_manager: PlayerManager,
    ticks: i64,
    // pub ck: LinkedList<Chunk>,
}

impl Game {
    pub fn new() -> Game {
        return Game {
            chunk_manager: ChunkManager::new(),
            player_manager: PlayerManager::new(),
            ticks: 0,
            // ck: Default::default(),
        };
        // {
        //     let mut g = game.write().unwrap();
        //     g.chunkManager.write().unwrap().game = sync::Arc::downgrade(&game.clone());
        // }
        // game.chunkManager.setGame(sync::Arc::downgrade(&game));
        // return game;
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
