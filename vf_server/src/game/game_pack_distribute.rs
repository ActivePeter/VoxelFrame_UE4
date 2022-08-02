use crate::net::{ClientMsgEnum, ClientMsg};
use crate::net_pack_convert::MsgEnum;
use crate::protos::common::ClientType;
use crate::{async_task, part_server_sync};
use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
use crate::game::{Game, game_player, game_entity, game_block, game_flow};
use crate::netclient;
use crate::game::game_player::PlayerConnectionHandler;
// use std::alloc::Global;

// pub struct PackHandler{
//     handle_common: fn()->void
//
// }
// pub fn create_packhandlers() -> PackHandlers {
//     let mut v: Vec<PackHandler> = vec![];
//
//     return v;
// }
// async fn hhh(){
//
// }
pub async fn distribute_client_common_msg(context:&mut Game,msg:ClientMsgEnum){
    // let h=hhh;
    // h().await;

    match msg {
        ClientMsgEnum::ClientCommonMsg(mut common_msg) => {
            let mut continue_=game_player::handle_pack(context,&common_msg).await;
            if continue_ { continue_ = game_entity::handle_pack(context, &common_msg).await }
            if continue_ { continue_ = game_block::handle_pack(context,&common_msg).await }
            // game_player::handle_pack(common_msg).await;
            // match common_msg.msg_enum{
            //     _ => {
            //         async_task::match_client_rpl_msg(context, common_msg).await;
            //     }
            // }
        }
        ClientMsgEnum::ClientConnect(m) => {
            let id=
                context.client_manager.add_new_client(m.sender,m.client_type);
            m.response.send(id);
            if m.client_type==ClientType_GameServer{
                // println!("ClientType_GameServer");
                part_server_sync::on_partserver_connected(context,id).await;
                // _after_client_connect::after_client_connect_part_server(game,cid).await;
            }else if m.client_type==ClientType_Player{
                //获取玩家位置
                PlayerConnectionHandler::new(context).on_player_connect_getcid(id).await;
                // println!("ClientType_Player");
                // _after_client_connect::after_client_connect_player(game, cid).await;
            }else{
                println!("ClientConnect unknown type");
            }
            // game_flow::after_client_connect(context, id).await;
        }
        ClientMsgEnum::ClientDisconnect(m) => {
            netclient::NetClientOperator::new(context)
                .on_client_disconnect(m.client_id).await
            // context.client_manager.remove_client(m.client_id);
        }
    }
}