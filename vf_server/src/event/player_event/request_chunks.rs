use crate::protos::common;
use crate::game::{ClientId, Game};
use crate::game::chunk::ChunkKey;
use crate::net_send;
pub(crate) async fn call(clientid:ClientId,game:&Game,pack:common::PlayerRequestChunks){
    // let sender=game.client_man_ref().get_sender(clientid);
    let pid=game.player_man_ref().clientid_2_playerid.get(&clientid);
    match pid{
        None => {eprintln!("client not exist {}",clientid)}
        Some(pid) => {
            for ck in &pack.chunk_keys{
                let ck=ChunkKey::from_proto_chunkkey(ck);
                net_send::player_one_chunk_blockdata(game,*pid,&ck).await;
            }
        }
    }
}