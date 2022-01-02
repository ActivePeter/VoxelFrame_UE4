use crate::*;
use crate::client::Client;
use tokio::sync::mpsc::Sender;

pub type PlayerId = i32;
pub struct Player {
    pub player_id:PlayerId,
    pub entity_id:entity::EntityId,
    // pub client:Client,
    pub sender:Option<Sender<Vec<u8>>>
}
impl Player{
    pub fn create_uninited()->Player{
        Player{
            player_id: 0,
            entity_id: 0,
            sender:None,
            // client: Client::create_uninited(),
        }
    }
}