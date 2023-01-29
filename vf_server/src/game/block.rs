use crate::protos::common::{ClientType, PutBlock};
use crate::protos::common::ClientType::{ClientType_Player, ClientType_GameServer};
use crate::game::{ClientId, Game};
use crate::{game, net_send};
use crate::log;
// use crate::send;
use crate::async_task;
use crate::net::ClientMsg;
use crate::net_pack::MsgEnum;
use game::operation::OperationResult;
// use crate::game::game_entity::update_entity_pos;

pub(crate) mod block_type {
    pub(crate) type BlockTypeId =u8;

    pub(crate) fn can_plant_tree(typeid:BlockTypeId) -> bool {
        typeid==BlockDirt::block_type_id()||
            typeid==BlockGrass::block_type_id()
    }

    pub(crate) trait Block{
        fn block_type_id()->BlockTypeId;
    }
    pub(crate) struct BlockAir{}
    impl Block for BlockAir{
        fn block_type_id() -> BlockTypeId {
            0
        }
    }
    pub(crate) struct BlockDirt{}
    impl Block for BlockDirt{
        fn block_type_id() -> BlockTypeId {
            1
        }
    }
    pub(crate) struct BlockGrass{}
    impl Block for BlockGrass{
        fn block_type_id() -> BlockTypeId {
            2
        }
    }
    pub(crate) struct BlockRock{}
    impl Block for BlockRock{
        fn block_type_id() -> BlockTypeId {
            3
        }
    }
    pub(crate) struct BlockWater{}
    impl Block for BlockWater{
        fn block_type_id() -> BlockTypeId {
            4
        }
    }
}

pub(crate) mod put_block{
    use crate::*;
    use crate::{game, log, conv, protos, net_pack};
    use crate::protos::common::{ClientType, PutBlock, Rpl_PutBlockInPs};
    use crate::game::{ClientId, part_server_sync, Game};
    use crate::protos::common::ClientType::ClientType_Player;
    use crate::async_task::AsyncTask;
    use protobuf::SingularPtrField;
    use crate::net_pack::PackIds;
    use crate::game::operation::OperationResult;
    use crate::game::chunk::Chunk;
    use crate::base_type:: point3f_new2;


    // pub mod chunksync{
    //     use crate::protos::common::PutBlock;
    //     use crate::conv;
    //     use crate::game::Game;

    pub async fn notifyinterest_putblock(ctx:&mut Game,putter_cid:ClientId,pb:Box<PutBlock>){
        let ck =conv::point3i_2_chunkkey2(pb.x,pb.y,pb.z);
        let chunk=ctx.chunk_get(&ck).unwrap();
        let v=
            net_pack::packbox_to_bytes(pb, PackIds::EPutBlock);
        for pid in &chunk.be_interested_by{
            let cid=ctx.player_man_ref().playerid_2_player.get(pid).unwrap()
                .client_id;
            if cid!=putter_cid {
                ctx.client_man_ref().get_sender(cid)
                    .send(v.clone(),PackIds::EPutBlock.default_priority()).await;
            }
        }
    }
    // }
    // pub(crate) async fn putblock_in_chunk(
    //     ctx: &mut game::Game,
    //     pb:Box<PutBlock>,puttercid:ClientId){
    //     //根据方块坐标获取partserver
    //     let ck
    //         =conv::point3i_2_chunkkey2(pb.x,pb.y,pb.z);
    //     let chunk=ctx.chunk_get_mut(&ck).await;
    //     chunk.set_block_at(
    //         conv::chunkpos_of_worldpos(
    //             Point3i::new(pb.x,pb.y,pb.z))
    //         ,pb.block_id as u8);
    //
    //     notifyinterest_putblock(ctx,puttercid,pb).await;
    //     // let chunk=
    // }

    async fn put_block_in_ps(
        ctx: &mut game::Game,
        cid: game::ClientId, pb: PutBlock) {
        let task_id=ctx.async_task_man_mut().add_task(
            AsyncTask::EPutBlockInPs(cid,pb.get_operation_id(),Box::new(pb.clone()))
        );

        //根据方块坐标获取partserver
        let ck
            =conv::point3i_2_chunkkey2(pb.x,pb.y,pb.z);
        let sender=part_server_sync::
        get_part_server_sender_of_chunk(ctx,ck).await.unwrap();

        //制作pack
        let mut pack =protos::common::Cmd_PutBlockInPs::new();
        pack.task_id=task_id;
        pack.put_block=SingularPtrField::some(pb);

        sender.serialize_and_send(pack,PackIds::ECmd_PutBlockInPs)
            .await;
            // //转vec发送
            // let vec=net_pack_convert::pack_to_bytes(
            //     pack,PackIds::ECmd_PutBlockInPs);
            // sender.send(vec).await;

    }


    //
    // pub async fn handle_PutBlock(ctx:&mut game::Game,ct:ClientType,cid:ClientId,pb:PutBlock) {
    //     if ct==ClientType_Player{
    //         //todo 加入一些权限检测啥的
    //
    //         //来自player，需要请求partserver putblock
    //         put_block_in_ps(ctx,cid,pb).await;
    //     }else{
    //         println!("{} handle_PutBlock",log::get_info_type_str(log::InfoType::WrongMsgSource))
    //     }
    // }
}
