use crate::game::{ClientId, Game, entity, chunk_event, player};
use crate::protos;

pub(crate) async fn on_player_connect_getcid(
    game:&mut Game,cid:ClientId){
    //////////////////////////////////////
    //  player 进来后所有数据操作

    // //1.获取player码以及绑定tcp
    let playerid =
        (game)._player_manager.borrow_mut().create_player_and_bind_client(cid);
    //
    //
    // entity=(game).spawn_entity_for_player(&player);

    //2.出生entity 这个过程是产生entity，
    let player_entity_id = entity::entity_spawn(game);

    //2.5产生完entity id 就与player绑定
    game._player_manager.borrow_mut().set_player_entity_id(playerid, player_entity_id);

    //3.ps 创建entity
    let mut epos =protobuf::SingularPtrField::some( protos::common::EntityPos::new());
    let entity=game.entity_get(&player_entity_id).unwrap();
    {
        let epos_ref=epos.as_mut().unwrap();
        epos_ref.x = entity.position[0];
        epos_ref.y = entity.position[1];
        epos_ref.z = entity.position[2];
        epos_ref.entity_id = player_entity_id;
        epos_ref.t = protos::common::EntityType::T_Player;
    }
    //
    // chunk_event::entity_pos_update::on_entity_spawned(
    //     game,&entitypos).await;

    //todo: 出生点区块坐标可能变化，还未做相应的处理
    player::send_player_basic_and_chunk(
        game,playerid,player_entity_id).await;
}