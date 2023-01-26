use crate::game::{ClientId, Game};
use crate::protos::common;
use crate::{conv, net_pack_convert};
use crate::net_pack_convert::PackIds;

pub(crate) async fn call(cid:ClientId, game:&mut Game, pmcmd:common::MainPlayerJumpCmd){
    //TODO 这里1。2是假的，目前是one server 模式
    //0.验证pack 有效性
    if(pmcmd.entity_id!=game.player_manager.get_player_by_cid(cid).entity_id){
        println!("wrong cmd eid {} {}",pmcmd.entity_id,game.player_manager.get_player_by_cid(cid).entity_id);
        return;
    }
    //1.找到player所在区块，
    let ck=conv::point3f_2_chunkkey(&game.entities.get(&pmcmd.entity_id).unwrap().position);
    //2.确认区块是那个服务端控制的
    let cidop=game.part_server_sync.get_part_server_cid_of_chunk(ck);

    //3.转发
    match cidop {
        None => {}
        Some(cid) => {
            game.client_manager.get_sender(cid).send(
                net_pack_convert::pack_to_bytes(pmcmd,PackIds::EMainPlayerJumpCmd)
            ).await;
        }
    }
}
