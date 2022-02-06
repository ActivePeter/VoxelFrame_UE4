use std::collections::{HashSet, HashMap};
use crate::game_chunk::ChunkKey;
use crate::game::{ClientId, Game, ClientManager};
use crate::send_packer;
use crate::net::{ClientSender, ClientDescription};
use tokio::sync::mpsc;

pub struct PartServerSync{
    pub free_chunks:HashSet<ChunkKey>,
    // part_servers:HashMap<ClientId,PartServer>,
    pub part_server:Option<PartServer>,
}
impl PartServerSync{
    pub fn create() -> PartServerSync {
        PartServerSync{
            free_chunks:Default::default(),
            part_server:None,
        }
    }

    pub fn get_part_server_cid_of_chunk(&self, ck:ChunkKey) -> Option<ClientId> {
        match &self.part_server{
            None => {
                return None
            }
            Some(ps )=> {
                return Some(ps.client_id);
            }
        }
    }
}

pub fn get_part_server_sender_of_chunk(game:&mut Game,ck:ChunkKey)->Option<ClientSender>{
    match game.part_server_sync.get_part_server_cid_of_chunk(ck){
        None => {
            return None;
        }
        Some(cid) => {
            return Some(game.client_manager.get_sender(cid));
        }
    }
}

pub async fn add_part_server(game:&mut Game, cid:ClientId){
    let cm=&mut game.client_manager;
    let pss=&mut game.part_server_sync;
    // println!("aps");
    match &pss.part_server{
        None => {
            pss.part_server=Some(PartServer{
                client_id: cid,
                chunks: Default::default()
            });
            let sender=cm.get_sender(cid);
            bind_all_free_chunks_2_part_server(game,sender).await;
        }
        Some(_) => {
            println!("currently only support one part server")
        }
    }
}

pub async fn bind_all_free_chunks_2_part_server(game: &mut Game, send:ClientSender){
    // println!("safc");
    // let cm=&mut game.client_manager;
    if(!game.part_server_sync.free_chunks.is_empty()){
            // game.part_server_sync.free_chunks.retain(|&k| ->bool{
            //     send_packer::pack_chunk_pack(game.chunk_get(&k).unwrap());
            //     return false;
            // })

        for i in &game.part_server_sync.free_chunks{
            game.part_server_sync.part_server.as_mut().unwrap().chunks.insert(i.clone());
            let pack=send_packer::pack_chunk_pack(game.chunk_get(i).unwrap());
            //发送
            send.send(pack).await;
            // game.part_server_sync.free_chunks.remove(&i);
        }
        game.part_server_sync.free_chunks.clear();
    }
}

pub async fn add_free_chunk(game:&mut Game, ck:ChunkKey){
    println!("add_free_chunk");
    let mut cid:ClientId=0;
    let mut has_part_server =false;
    match &mut game.part_server_sync.part_server {
        None => {
            println!("currently no part server to take");
            game.part_server_sync.free_chunks.insert(ck);
        }
        Some( ps) => {
            ps.chunks.insert(ck);
            cid=ps.client_id;
            has_part_server=true;
        }
    }
    if(has_part_server){
        println!("has server then send {}",cid);
        //发送给partserver
        let sender=
            game.client_manager.get_sender(cid);
        sender.send(send_packer::pack_chunk_pack(
            game.chunk_get(&ck).unwrap())).await;
    }
}

pub struct PartServer{
    pub client_id:ClientId,
    pub chunks:HashSet<ChunkKey>,
}
