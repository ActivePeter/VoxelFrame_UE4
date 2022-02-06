use crate::*;
use crate::game::Game;
use crate::protos::common::EntityType;

//type
pub type EntityId = u32;

pub struct EntityData {
    pub entity_id: EntityId,
    pub position: base_type::Point3f,
    pub entity_type:EntityType,
}
impl EntityData{
    pub fn hello(&mut self){

    }
    pub fn set_positon(&mut self,x:f32,y:f32,z:f32){
        self.position[0]=x;
        self.position[1]=y;
        self.position[2]=z;
    }
}

//entity related operations
pub fn entity_spawn(game:&mut Game) -> u32 {
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
pub fn entity_spawn_cont(game:&mut Game){

}