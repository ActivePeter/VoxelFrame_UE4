use crate::*;
use crate::game::{Game, chunk};
use crate::game::player::{Player, PlayerId};
use crate::game::entity::EntityData;
use crate::game::chunk::ChunkKey;
use crate::net_pack::PackIds;
use crate::game::chunk_send::SendChunkTask;
use std::collections::HashSet;
use crate::net::ClientSender;
use std::collections::linked_list::Iter;
use crate::net_client::ClientManager;


pub(crate) struct SenderIter<'a>{
    game:&'a Game,
    builder:SenderIterBuilder,
    in_chunk_player_iter:Option<Iter<'a,PlayerId>>,
}

impl<'a> Iterator for SenderIter<'a>{
    type Item = &'a ClientSender;

    fn next(&mut self) -> Option<&'a ClientSender> {
        if self.builder.part_server_need{
            self.builder.part_server_need=false;
            let cid=self.game.part_server_ref().part_server.as_ref().unwrap().client_id;

            return Some(self.game.client_man_ref().get_sender_ref(cid));
        }
        let ck=self.game.chunk_get(&self.builder.range_chunk).unwrap();
        if self.in_chunk_player_iter.is_none(){
            self.in_chunk_player_iter=Some(ck.be_interested_by.iter());
        }
        while let Some(v)=self.in_chunk_player_iter.as_mut().unwrap().next(){
            if self.builder.except_list.contains(v){
                continue;
            }
            return Some(&self.game.client_man_ref().get_player_sender(
                self.game.player_man_mut().get_player_mut(v).unwrap()
            ));
        }
        None
    }
}
pub(crate) struct SenderIterBuilder{
    range_chunk:ChunkKey,
    except_list:HashSet<PlayerId>,
    part_server_need:bool,
}
impl<'a> SenderIterBuilder{
    pub(crate) fn new() -> SenderIterBuilder {
        SenderIterBuilder{
            range_chunk: ChunkKey::new(0,0,0),
            except_list: Default::default(),
            part_server_need: false
        }
    }
    pub(crate) fn build(self, game:&'a Game) -> SenderIter<'a> {
        SenderIter{
            game,
            builder: self,
            in_chunk_player_iter: None
        }
    }
    pub(crate) fn set_range_chunk(mut self, ck:ChunkKey) -> SenderIterBuilder {
        self.range_chunk=ck;
        self
    }
    pub(crate) fn set_except(mut self, pid:PlayerId) -> SenderIterBuilder {
        self.except_list.insert(pid);
        self
    }
    pub(crate) fn set_part_server(mut self, send:bool) -> SenderIterBuilder {
        self.part_server_need=send;
        self
    }
}
// pub mod to_player{
//     use crate::game::{ClientOperationId, ClientId};
//     use crate::game;
//     use crate::protos;
//     use crate::net_pack_convert;
//     use crate::net_pack_convert::PackIds;
//
//
// }
pub(crate) async fn player_basic(
    client_manager: &ClientManager,
    player: &Player,
    entity: &EntityData) {
    let (packed,p) = net_send_packer::pack_player_basic(entity);
    let sender = client_manager.get_player_sender(player);
    sender.send(packed,p).await;
}

pub async fn player_one_chunk_blockdata(
    game: &Game,
    pid:PlayerId,
    ckk:&ChunkKey
){
    let ck=game.chunk_get_mut_loaded(ckk).await;
    let (packed,p)= net_send_packer::pack_chunk_pack(ck);
    let player=game.player_man_ref().playerid_2_player.get(&pid).unwrap();
    game._chunk_sender.send(SendChunkTask::new(
        packed,ckk.clone(),player.client_id,
        game.client_man_ref().get_player_sender(player).clone()
    )).await;
    // let sender = game.client_man_ref().get_player_sender(
    //     game.player_man_ref().playerid_2_player.get(&pid).unwrap());
    // // player.sender.as_ref().unwrap().clone().
    // sender.send(packed,p).await;
}

pub async fn player_one_chunk_entitydata(
    game: &Game,
    pid:PlayerId,
    ckk:&ChunkKey
){

    let ck=game.chunk_get_mut_loaded(ckk).await;
    let (packed,p)= net_send_packer::pack_chunk_entity_pack(
        ck,
        game.entities_ref()
    );
    let sender = game.client_man_ref().get_player_sender(
        game.player_man_ref().playerid_2_player.get(&pid).unwrap());
    // player.sender.as_ref().unwrap().clone().
    sender.send(packed,p).await;
}

pub(crate) async fn player_interested_chunk_block_data(
    client_manager: &ClientManager,
    player: &Player,
    entity: &EntityData,
    game: &Game,
) {
    println!("send player chunk data");
    //计算玩家区块
    let p_ck = conv::point3f_2_chunkkey(&entity.position);
    iter_relative_chunk_key_in_interest_range!(
        r_ck,
        {
            let cur_ck=p_ck.plus(r_ck);
            let ck=game.chunk_get_mut_loaded(&cur_ck).await;
            // println!("send player {} {} {} {}",cur_ck.x,cur_ck.y,cur_ck.z,ck.not_air_cnt());
            let (packed,p)=net_send_packer::pack_chunk_pack(ck);
            game._chunk_sender.send(SendChunkTask::new(
                packed,cur_ck,player.client_id,client_manager.get_player_sender(player).clone()
            )).await;
            // let sender = client_manager.get_player_sender(player);
            // // player.sender.as_ref().unwrap().clone().
            // sender.send(packed,p).await;
        }
    )
}

pub(crate) async fn player_interested_chunk_entity_data(
    client_manager: &ClientManager,
    player: &Player,
    entity: &EntityData,
    game: &Game,
) {
    let p_ck = conv::point3f_2_chunkkey(&entity.position);

    iter_relative_chunk_key_in_interest_range!(
        r_ck,
        {
            let cur_ck=p_ck.plus(r_ck);
            let (packed,p)=net_send_packer::pack_chunk_entity_pack(
                game.chunk_get_mut_loaded(&cur_ck).await,
                game.entities_ref()
            );
            let sender = client_manager.get_player_sender(player);
            sender.send(packed,p).await;
        }
    )
}