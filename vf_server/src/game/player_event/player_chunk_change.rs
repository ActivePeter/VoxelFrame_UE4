use crate::game::player::{Player, PlayerId};
use crate::game::chunk::ChunkKey;
use crate::game::Game;
use std::collections::HashSet;
use crate::net_send;
pub(crate) async fn call(from:&ChunkKey,to:&ChunkKey,game:&Game,pid:PlayerId,new_interested:HashSet<ChunkKey>){
    // 找出这个player 未
    for ck in &new_interested{
        net_send::player_one_chunk_blockdata(game,pid,ck).await;
        net_send::player_one_chunk_entitydata(game,pid,ck).await;
    }
}