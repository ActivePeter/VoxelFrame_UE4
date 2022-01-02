use crate::*;

//type
pub type EntityId = u32;
pub type EntityType=u32;
pub struct EntityData {
    pub entity_id: EntityId,
    pub position: base_type::Point3f,
    pub entity_type:EntityType,
}
impl EntityData{

}
