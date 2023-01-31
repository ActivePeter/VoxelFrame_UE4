use crate::net::{ClientMsgEnum, ClientMsg};
use crate::net::net_pack::MsgEnum;
use crate::protos::common::ClientType;
use crate::{async_task, log};
use crate::net::part_server_sync;
use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
use crate::game::{Game, player, entity, block};
use crate::net::net_client;
use crate::game::player::PlayerConnectionHandler;
use crate::game::{player_event,chunk_event};
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
        ClientMsgEnum::ClientCommonMsg(mut msg) => {
            let mut continue_= player::handle_pack(context, &msg).await;
            // if continue_ { continue_ = entity::handle_pack(context, &common_msg).await }
            let clientid=msg.client_id;
            match msg.msg_enum {
                MsgEnum::PlayerRequestChunks(request)=>{
                    if msg.client_type==ClientType_Player{
                        player_event::request_chunks::call(clientid,context,request).await;
                    }
                }
                /// entity
                MsgEnum::EntityPosUpdate(epu)=>{

                    // println!("EntityPosUpdate From PartServer");
                    if msg.client_type==ClientType_GameServer{
                        chunk_event::entity_pos_update::on_ps_update(context, epu).await;
                    }

                    // false
                }
                MsgEnum::Rpl_SpawnEntityInPs(rpl) => {
                    // println!("Rpl_SpawnEntityInPs");
                    if msg.client_type==ClientType_GameServer{
                        chunk_event::entity_spawned::call(context, rpl).await;
                        // handle_spawn_entity_in_ps_rpl(
                        //     ).await;
                    }
                    // false
                }

                MsgEnum::PutBlock(pb)=>{
                    if msg.client_type==ClientType_Player{
                        //todo 加入一些权限检测啥的

                        //来自player，需要请求partserver putblock
                        player_event::cmd_put_block::call(context,msg.client_id,
                                                          pb).await;
                    }else{ println!("{} handle_PutBlock",log::get_info_type_str(log::InfoType::WrongMsgSource)) }
                    // false
                }
                MsgEnum::Rpl_PutBlockInPs(rpl)=>{
                    if msg.client_type==ClientType_GameServer{
                        player_event::cmd_put_block::reply(context,rpl).await;
                        // put_block::put_block_in_ps_rpl(
                        //    ).await;
                    }
                    // false
                }
                MsgEnum::PlayerPosUpdate(player_pos_update)=>{
                    player_event::player_pos_update::on_player_msg(context, player_pos_update).await;
                }
                _ => {
                    // true
                }
            };
            // if continue_ { continue_ = block::handle_pack(context, &common_msg).await }
            // game_player::handle_pack(common_msg).await;
            // match common_msg.msg_enum{
            //     _ => {
            //         async_task::match_client_rpl_msg(context, common_msg).await;
            //     }
            // }
        }
        ClientMsgEnum::ClientConnect(m) => {
            let id=
                context._client_manager.borrow_mut().add_new_client(m.sender, m.client_type);
            m.response.send(id).unwrap();
            if m.client_type==ClientType_GameServer{
                // println!("ClientType_GameServer");
                part_server_sync::on_partserver_connected(context,id).await;
                // _after_client_connect::after_client_connect_part_server(game,cid).await;
            }else if m.client_type==ClientType_Player{
                //获取玩家位置
                player_event::connected::on_player_connect_getcid(context,id).await;
            }else{
                println!("ClientConnect unknown type");
            }
            // game_flow::after_client_connect(context, id).await;
        }
        ClientMsgEnum::ClientDisconnect(m) => {
            net_client::NetClientOperator::new(context)
                .on_client_disconnect(m.client_id).await
            // context.client_manager.remove_client(m.client_id);
        }
        _=>{unreachable!()}
    }
}