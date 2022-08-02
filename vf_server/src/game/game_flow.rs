use crate::game::{ClientId, Game, ClientManager};
use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
use crate::send;
use crate::part_server_sync;

mod _after_client_connect{

    use crate::game::{ClientId, Game, ClientManager, game_entity, game_chunk};
    use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
    use crate::{send};
    use crate::protos;
    use crate::part_server_sync;
    use crate::async_task;
    use std::fmt::Debug;


    // pub(crate) async fn after_client_connect_part_server(
    //     game:&mut Game,cid:ClientId) {
    //     part_server_sync::add_part_server(game,cid).await;
    //     // game.part_server_sync.add_part_server(client_disc.client_id);
    // }
    // pub(crate) async fn after_client_connect_player(game:&mut Game,client_id:ClientId) {
    //
    // }
}