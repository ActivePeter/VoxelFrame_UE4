use crate::game::{ClientId, Game, ClientManager};
use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
use crate::send;

pub async fn after_client_connect(game:&mut Game,cid:ClientId){
    let disc=game.client_manager.id2client.get(&cid).unwrap();
    // println!("after_client_connect");
    if disc.client_type==ClientType_GameServer{
        // println!("ClientType_GameServer");
        _after_client_connect::after_client_connect_part_server(game,cid).await;
    }else if disc.client_type==ClientType_Player{
        //获取玩家位置
        // println!("ClientType_Player");
        _after_client_connect::after_client_connect_player(game, cid).await;
    }
}
mod _after_client_connect{
    use crate::game::{ClientId, Game, ClientManager};
    use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
    use crate::{send, game_chunk};

    use crate::part_server_sync;

    pub(crate) async fn after_client_connect_part_server(
        game:&mut Game,cid:ClientId) {
        part_server_sync::add_part_server(game,cid).await;
        // game.part_server_sync.add_part_server(client_disc.client_id);
    }
    pub(crate) async fn after_client_connect_player(game:&mut Game,client_id:ClientId) {
        //////////////////////////////////////
        //  player 进来后所有数据操作

        // //1.获取player码以及绑定tcp
        let playerid =
            (game).player_manager.create_player_and_bind_client(client_id);
        //
        //
        // entity=(game).spawn_entity_for_player(&player);

        //2.出生entity 这个过程是产生entity，
        let player_entity_id = game.entity_spawn();

        //2.5产生完entity id 就与player绑定
        {
            let p = game.player_manager.get_player_handle(&playerid).unwrap();
            p.entity_id = player_entity_id;
        }
        println!("before add player -----------------------------------");
        //3.将player id 和entity id 加入区块
        // game.add_player_entity_2_chunk(&player, entity);
        {
            game_chunk::chunk_add_player(game, playerid, player_entity_id).await;
        }
        println!("aft add player -----------------------------------");
        //4.刷新被感兴趣的区块
        game_chunk::chunks_add_be_interested(game, playerid, player_entity_id).await;

        println!("aft1 add player -----------------------------------");
        ////////////////////////////////////
        //  player 进来后所有数据发送
        println!("send player -----------------------------------");
        // 5.发送玩家进入后的全部内容
        {
            let player=
                game.player_manager.playerid_2_player.get(&playerid).unwrap();
            let entity=game.entity_get(&player_entity_id).unwrap();
            // 1.player基本信息（player_entity_id
            send::player_basic(&game.client_manager, player, entity).await;
            // 2.区块地形
            send::player_interested_chunk_block_data(&game.client_manager,
                                                     player,entity,game
            ).await;
            // 3.感兴趣区块的entity数据
            send::player_interested_chunk_entity_data(&game.client_manager,
                                                      player,entity,game
            ).await;
        }
    }
}