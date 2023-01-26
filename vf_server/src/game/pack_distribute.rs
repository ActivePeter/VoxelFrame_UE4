use crate::net::{ClientMsgEnum, ClientMsg};
use crate::net_pack_convert::MsgEnum;
use crate::protos::common::ClientType;
use crate::{async_task, part_server_sync, log};
use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
use crate::game::{Game, player, entity, block};
use crate::netclient;
use crate::game::player::PlayerConnectionHandler;
use crate::event::player_event;
use crate::event::chunk_event;
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
            match &msg.msg_enum {

                /// entity
                MsgEnum::EntityPosUpdate(epu)=>{

                    // println!("EntityPosUpdate From PartServer");
                    if msg.client_type==ClientType_GameServer{
                        chunk_event::entity_pos_update::call(context, epu.clone(),
                                                                false, 0).await;
                    }

                    false
                }
                MsgEnum::Rpl_SpawnEntityInPs(rpl) => {
                    // println!("Rpl_SpawnEntityInPs");
                    if msg.client_type==ClientType_GameServer{
                        chunk_event::entity_spawned::call(context, rpl.clone()).await;
                        // handle_spawn_entity_in_ps_rpl(
                        //     ).await;
                    }
                    false
                }

                MsgEnum::PutBlock(pb)=>{
                    if msg.client_type==ClientType_Player{
                        //todo 加入一些权限检测啥的

                        //来自player，需要请求partserver putblock
                        player_event::cmd_put_block::call(context,msg.client_id,
                                                          pb.clone()).await;
                    }else{ println!("{} handle_PutBlock",log::get_info_type_str(log::InfoType::WrongMsgSource)) }
                    false
                }
                MsgEnum::Rpl_PutBlockInPs(rpl)=>{
                    if msg.client_type==ClientType_GameServer{
                        player_event::cmd_put_block::reply(context,rpl.clone()).await;
                        // put_block::put_block_in_ps_rpl(
                        //    ).await;
                    }
                    false
                }
                _ => {
                    true
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
                context.client_manager.add_new_client(m.sender,m.client_type);
            m.response.send(id).unwrap();
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
        _=>{unreachable!()}
    }
}