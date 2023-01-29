use crate::{net_pack, conv};
use crate::net_pack::PackIds;
use crate::game::{ClientId, Game};
use crate::protos::common;

pub(crate) async fn call(cid:ClientId, game:&mut Game, pmcmd:common::MainPlayerMoveCmd) {

    //TODO 这里1。2是假的，目前是one server 模式
    //0.验证pack 有效性
    if pmcmd.entity_id!=game.player_man_ref().get_player_by_cid(cid).entity_id {
        println!("wrong cmd eid {} {}",pmcmd.entity_id,game.player_man_ref().get_player_by_cid(cid).entity_id);
        return;
    }
    //1.找到player所在区块，
    let ck=conv::point3f_2_chunkkey(&game.entities_ref().get(&pmcmd.entity_id).unwrap().position);
    //2.确认区块是那个服务端控制的
    let cidop=game.part_server_ref().get_part_server_cid_of_chunk(ck);

    //3.转发
    match cidop {
        None => {}
        Some(cid) => {
            game.client_man_ref().get_sender(cid).send(
                net_pack::pack_to_bytes(
                    pmcmd,PackIds::EMainPlayerMoveCmd),PackIds::EMainPlayerMoveCmd.default_priority()
            ).await;
        }
    }
}