use crate::game::{Game, chunk};
use crate::{protos, conv};
use crate::game::player::PlayerId;
use std::collections::{LinkedList, HashSet};
use crate::game::entity::entity_move_change_chunk;
use crate::protos::common::EntityType;
use crate::net_pack::{PackIds, pack_to_bytes2};
use protobuf::Clear;
use crate::game::chunk::{ChunkLoadStage, ChunkKey};
use crate::event::player_event;

pub(crate) async fn call(game: &Game, epu: protos::common::EntityPosUpdate,
                         except_enabled:bool, except_player_id:PlayerId){
    // println!("update_entity_pos");
    let mut changed_chunks = LinkedList::new();
    //1.一个chunk的entity变更一起算
    for a in epu.entity_pos {
        match game.entities_mut().get_mut(&a.entity_id) {
            None => {
                println!("update_entity_pos no entity matched {}",a.entity_id);
            }
            Some(game_entity) => {
                let ck1 = conv::point3f_2_chunkkey(&game_entity.position);
                // println!("update_entity_pos matched {}",a.entity_id);
                game_entity.set_positon(a.x, a.y, a.z);
                let ck = conv::point3f_2_chunkkey(&game_entity.position);
                let isplayer=game_entity.entity_type==EntityType::T_Player;
                if isplayer {
                    if ck1!=ck {
                        let pid=game.player_man_ref().get_player_by_eid(a.entity_id).player_id;
                        // let mut collect_old=HashSet::new();
                        // let mut collect_news =HashSet::new();
                        chunk::chunks_remove_be_interested(game, pid, ck1)
                            .await;
                        chunk::chunks_add_be_interested2(game, pid, ck)
                            .await;
                        // //这里可以发现新加的chunks，
                        // //player 变更区块。需要将其没有的新区块发过去
                        // player_event::player_chunk_change::call(
                        //     &ck1,&ck,game,pid,collect_news
                        // ).await;
                    }
                }
                //实体区块变更
                if ck1!=ck {
                    entity_move_change_chunk(
                        game,ck1,ck,a.entity_id,isplayer).await;
                    let chunk = game.chunk_get_mut_loaded(&ck1).await;
                    chunk.entity_update.entity_pos.push(a.clone());
                    changed_chunks.push_back(ck1);
                }
                let mut chunk = game.chunk_get_mut_loaded(&ck).await;
                if(chunk.entity_update.entity_pos.len()>0){
                    println!("before_push chunk entity_update len {}",chunk.entity_update.entity_pos.len());
                }
                chunk.entity_update.entity_pos.push(a);
                changed_chunks.push_back(ck);
            }
        }
    }
    //2.//chunk view 遍历变更的区块，
    for ck in &changed_chunks {
        let chunk =game.try_load_chunk_get_mut(ck);
        assert!(chunk.load_stage==ChunkLoadStage::End);
        //game.chunk_get(ck).unwrap();// (&ck).await;
        for p in &chunk.be_interested_by {
            //对变更区块感兴趣的player p
            if except_enabled && p==&except_player_id{
                continue;
            }
            if(except_enabled){
                for eu in &chunk.entity_update.entity_pos{
                    print!("{} ",eu.entity_id);
                }
                println!("send to p{}",p);
            }
            // for eu in &chunk.entity_update.entity_pos{
            //     print!("{} ",eu.entity_id);
            // }
            // println!("send to p{}",p);
            let cid = game.player_man_ref().playerid_2_player.get(&p).unwrap().client_id;
            // let a=chunk.entity_update.borrow();
            let bytes=pack_to_bytes2(
                &chunk.entity_update, PackIds::EEntityPosUpdate);

            game.client_man_ref().id2client.get(&cid).unwrap()
                .sender.send(bytes,PackIds::EEntityPosUpdate.default_priority()).await;
            // println!("sent ck entity add to player");
        }
        chunk.entity_update.clear();
    }
}