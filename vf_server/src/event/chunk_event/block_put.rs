use crate::conv;
use crate::protos::common::PutBlock;
use crate::game::{ClientId, Game};
use crate::base_type::Point3i;
use crate::game::block::put_block::notifyinterest_putblock;

pub(crate) async fn call(
                         ctx: &mut Game,
                         pb:Box<PutBlock>,puttercid:ClientId){
    //根据方块坐标获取partserver
    let ck
        =conv::point3i_2_chunkkey2(pb.x,pb.y,pb.z);
    let chunk=ctx.chunk_get_mut(&ck).await;
    chunk.set_block_at(
        conv::chunkpos_of_worldpos(
            Point3i::new(pb.x,pb.y,pb.z))
        ,pb.block_id as u8);

    notifyinterest_putblock(ctx,puttercid,pb).await;
    // let chunk=
}