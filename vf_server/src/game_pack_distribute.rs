use crate::net::{ClientMsgEnum};
use crate::net_pack_convert::MsgEnum;
use crate::protos::common::ClientType;
use crate::{game_player, game_entity, game_block, async_task, game_flow};
use crate::protos::common::ClientType::ClientType_GameServer;
use crate::game::Game;

pub async fn distribute_client_common_msg(context:&mut Game,msg:ClientMsgEnum){

    match msg {
        ClientMsgEnum::ClientCommonMsg(common_msg) => {
            match common_msg.msg_enum{
                MsgEnum::MainPlayerMoveCmd(cmd) => {
                    if common_msg.client_type==ClientType::ClientType_Player {
                        game_player::handle_pack::
                            handle_MainPlayerMoveCmd(common_msg.client_id,
                                                              context, cmd).await;
                    }
                }
                MsgEnum::MainPlayerJumpCmd(cmd) => {
                    if common_msg.client_type==ClientType::ClientType_Player {
                        game_player::handle_pack::
                            handle_MainPlayerJumpCmd(common_msg.client_id,
                                                              context, cmd).await;
                    }
                }
                MsgEnum::EntityPosUpdate(epu)=>{
                    if common_msg.client_type==ClientType_GameServer{
                        game_entity::update_entity_pos(context,epu,
                                                       false,0).await;
                    }
                }
                MsgEnum::PutBlock(pb)=>{
                    game_block::handle_PutBlock(
                        context,
                        common_msg.client_type,common_msg.client_id,
                        pb
                    ).await;
                }
                _ => {
                    async_task::match_client_rpl_msg(context, common_msg).await;
                }
            }
        }
        ClientMsgEnum::ClientConnect(m) => {
            let id=
                context.client_manager.add_new_client(m.sender,m.client_type);
            m.response.send(id);
            game_flow::after_client_connect(context, id).await;
        }
        ClientMsgEnum::ClientDisconnect(m) => {
            context.client_manager.remove_client(m.client_id);
        }
    }
}