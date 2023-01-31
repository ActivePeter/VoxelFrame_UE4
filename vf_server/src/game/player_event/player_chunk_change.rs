use crate::*;
use crate::game::player::{Player, PlayerId};
use crate::game::chunk::ChunkKey;
use crate::game::{Game, chunk};
use std::collections::HashSet;
use net::net_send;
pub(crate) async fn call(from:Option<ChunkKey>,to:&ChunkKey,game:&Game,pid:PlayerId){
    // chunk changed
    if let Some(from)=from{
        // remove old chunk
        chunk::chunks_remove_be_interested(game,pid,from).await;
    }
    // add new chunk
    chunk::chunks_add_be_interested(game,pid,to).await;
}