use crate::game::{Game, ClientId};
use crate::protos::common::ClientType::{ClientType_Player, ClientType_GameServer};
use crate::net::part_server_sync;
use crate::game::player::{PlayerConnectionHandler, Player, PlayerId};
use std::collections::{HashMap, HashSet, LinkedList};
use crate::net::{ClientSender, ClientDescription};
use crate::protos::common::ClientType;
use crate::game::chunk::ChunkKey;
use std::collections::linked_list::Iter;


pub(crate) struct ClientManager{
    next_client_id:ClientId,
    pub(crate) id2client :HashMap<ClientId,ClientDescription>,
    partserver_clients:HashSet<ClientId>,
    player_clients:HashSet<ClientId>,
}
impl ClientManager{
    pub(crate) fn new() -> ClientManager {
        ClientManager{
            next_client_id: 0,
            id2client: Default::default(),
            partserver_clients: Default::default(),
            player_clients: Default::default()
        }
    }
    pub fn get_player_sender(&self, player: &Player) -> &ClientSender {
        return &self.id2client.get(&(player.client_id)).unwrap().sender;
    }

    //make sure the cid is valid before call
    pub fn get_sender_ref(&self, cid:ClientId) -> &ClientSender {
        return &self.id2client.get(&(cid)).unwrap().sender;
    }
    pub fn get_sender(&self,cid:ClientId)->ClientSender{
        return self.id2client.get(&(cid)).unwrap().sender.clone();
    }
    pub fn add_new_client(&mut self, mut sender:ClientSender, client_type:ClientType) ->ClientId{
        // sender.id=self.next_client_id;
        self.id2client.insert(self.next_client_id,ClientDescription{
            client_type,
            sender,
            client_id: self.next_client_id
        });
        if client_type==ClientType_GameServer {
            self.partserver_clients.insert(self.next_client_id);
        }else{
            self.player_clients.insert(self.next_client_id);
        }
        self.next_client_id+=1;

        return self.next_client_id-1;
    }
    pub fn remove_client(&mut self,cid:ClientId){
        let ctype=self.id2client.get(&cid).unwrap().client_type;
        if ctype==ClientType_GameServer{
            self.partserver_clients.remove(&cid);
        }else{
            self.player_clients.remove(&cid);
        }
        self.id2client.remove(&cid);
    }
    pub fn get_clienttype(&self,cid:ClientId)->Option<ClientType>{
        match self.id2client.get(&cid){
            None => {None}
            Some(c) => {
                Some(c.client_type)
            }
        }
    }
}

pub struct NetClientOperator<'a>{
    ctx:&'a mut Game
}
impl NetClientOperator<'_> {
    pub fn new(ctx:& mut Game) -> NetClientOperator {
        NetClientOperator{
            ctx,
        }
    }
    pub async fn on_client_disconnect(&mut self,cid:ClientId){
        println!("on_client_disconnect");
        let ctype=self.ctx.client_man_ref().get_clienttype(cid).unwrap();
        if ctype==ClientType_Player {
            PlayerConnectionHandler::new(self.ctx).on_player_disconnect(cid).await;

        }else{
            part_server_sync::on_partserver_disconnect(self.ctx,cid).await;
        }
    }
}
