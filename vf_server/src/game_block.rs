use crate::protos::common::{ClientType, PutBlock};
use crate::protos::common::ClientType::{ClientType_GameServer, ClientType_Player};
use crate::game::ClientId;
use crate::game;
use crate::log;
use crate::send;
use crate::async_task;
pub async fn handle_PutBlock(ctx:&mut game::Game,ct:ClientType,cid:ClientId,pb:PutBlock) {
    if ct==ClientType_Player{
        //todo 加入一些权限检测啥的

        //来自player，需要请求partserver putblock
        async_task::block::put_block_in_ps(ctx,cid,pb).await;
    }else{
        println!("{} handle_PutBlock",log::get_info_type_str(log::InfoType::WrongMsgSource))
    }
}
