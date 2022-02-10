use crate::*;
use crate::game::Game;
use crate::protos::common::EntityType;
use std::collections::LinkedList;
use crate::game_chunk::ChunkKey;
use crate::net_pack_convert::{pack_to_bytes, PackIds, pack_to_bytes2};
use protobuf::Clear;
use crate::game_player::PlayerId;

//type
pub type EntityId = u32;

pub struct EntityData {
    pub entity_id: EntityId,
    pub position: base_type::Point3f,
    pub entity_type: EntityType,
}

impl EntityData {
    pub fn hello(&mut self) {}
    pub fn set_positon(&mut self, x: f32, y: f32, z: f32) {
        self.position[0] = x;
        self.position[1] = y;
        self.position[2] = z;
    }
}

//entity related operations
pub fn entity_spawn(game: &mut Game) -> u32 {
    let entity =
        game.entities.entry(game.entity_cnt)
            .or_insert(
                EntityData {
                    entity_id: game.entity_cnt,
                    position: base_type::point3f_new(),
                    entity_type: EntityType::T_Player,
                }
            );
    // let entity=

    let entity_id = game.entity_cnt;
    game.entity_cnt += 1;

    return entity_id;
}

pub fn entity_spawn_cont(game: &mut Game) {}

//update entity pos and send to interested players
pub async fn update_entity_pos(game: &mut Game, epu: protos::common::EntityPosUpdate,
                               except_enabled:bool,except_player_id:PlayerId) {
    // println!("update_entity_pos");
    let mut changed_chunks = LinkedList::new();
    //1.一个chunk的entity变更一起算
    for a in epu.entity_pos {
        match game.entities.get_mut(&a.entity_id) {
            None => {
                println!("update_entity_pos no entity matched {}",a.entity_id);
            }
            Some(game_entity) => {
                // println!("update_entity_pos matched {}",a.entity_id);
                game_entity.set_positon(a.x, a.y, a.z);
                let ck = conv::point3f_2_chunkkey(&game_entity.position);
                let chunk = game.chunk_get_mut(&ck).await;
                chunk.entity_update.entity_pos.push(a);
                changed_chunks.push_back(ck);
            }
        }
    }
    //2.发送给感兴趣的players
    for ck in &changed_chunks {
        let chunk =game.chunks.get_mut(ck).unwrap();
            //game.chunk_get(ck).unwrap();// (&ck).await;
        for p in &chunk.be_interested_by {
            if except_enabled && p==&except_player_id{
                continue;
            }
            if(except_enabled){
                for eu in &chunk.entity_update.entity_pos{
                    print!("{} ",eu.entity_id);
                }
                println!("send to p{}",p);
            }
            // for eu in &chunk.entity_update.entity_pos{
            //     print!("{} ",eu.entity_id);
            // }
            // println!("send to p{}",p);
            let cid = game.player_manager.playerid_2_player.get(&p).unwrap().client_id;
            // let a=chunk.entity_update.borrow();
            let bytes=pack_to_bytes2(
                &chunk.entity_update, PackIds::EEntityPosUpdate);
            game.client_manager.id2client.get(&cid).unwrap()
                .sender.send(bytes).await;
            // println!("sent ck entity add to player");
        }
        chunk.entity_update.clear();
    }

}