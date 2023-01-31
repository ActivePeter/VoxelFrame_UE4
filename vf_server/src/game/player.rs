use crate::*;
use tokio::sync::mpsc::Sender;
use crate::game::{ClientId, Game, entity, chunk};
use crate::protos::common;
use crate::net::net_pack::{PackIds, MsgEnum};
use crate::game::entity::EntityId;
use crate::protos::common::ClientType;
use crate::net::{ClientMsgEnum, ClientMsg};
use crate::game::player_event;
use std::collections::HashMap;
use crate::game::chunk::ChunkKey;
use crate::net::net_send;

pub type PlayerId = i32;

// #[derive(Clone)]
pub struct Player {
    pub player_id:PlayerId,
    pub entity_id: EntityId,
    pub client_id:ClientId,
    // pub current_chunk:ChunkKey,
    // pub client:Client,
    // pub sender:Option<ClientSender>
}
impl Player{
    pub fn create_uninited()->Player{
        Player{
            player_id: 0,
            entity_id: 0,
            client_id:0,
        }
    }
}


pub struct PlayerManager {
    id_cnt: PlayerId,
    pub playerid_2_player: HashMap<
        PlayerId,
        Player
    >,
    pub clientid_2_playerid:HashMap<ClientId,PlayerId>,
    entityid_2_playerid:HashMap<EntityId,PlayerId>,
    // pub msg_list: LinkedList<Vec<u8>>,
}
impl PlayerManager{
    pub fn create_once()->PlayerManager{
        return PlayerManager{
            id_cnt: 0,
            playerid_2_player: Default::default(),
            clientid_2_playerid:Default::default(),
            entityid_2_playerid:Default::default(),
            // msg_list: Default::default()
        }
    }

    pub fn create_player_and_bind_client(&mut self,client_id:ClientId) -> PlayerId {
        let player=
            self.playerid_2_player.entry(self.id_cnt)
                .or_insert(
                    Player::create_uninited()
                );
        player.player_id=self.id_cnt;
        self.id_cnt+=1;
        player.client_id=client_id;
        self.clientid_2_playerid.insert(client_id,player.player_id);
        return player.player_id;
    }

    pub fn get_player_mut(&mut self, playerid :&PlayerId) -> Option<&mut Player> {
        return self.playerid_2_player.get_mut(playerid);
    }

    pub fn set_player_entity_id(&mut self,playerid :PlayerId,entity_id:EntityId){
        self.entityid_2_playerid.insert(entity_id,playerid);
        self.get_player_mut(&playerid).unwrap().entity_id=entity_id;
    }

    pub fn get_player_by_cid(&self, cid:ClientId) -> &Player {
        let pid=self.clientid_2_playerid.get(&cid).unwrap();
        return self.playerid_2_player.get(pid).unwrap();
    }
    pub fn get_player_by_eid(&self, eid:EntityId) -> Option<&PlayerId> {
        self.entityid_2_playerid.get(&eid)
    }
    //unwrap
    pub fn get_player_by_eid_unwrap(&self, eid:EntityId) ->&Player{
        let pid=self.entityid_2_playerid.get(&eid).unwrap();
        return self.playerid_2_player.get(pid).unwrap();
    }
}
pub struct PlayerManOperator<'a>{
    ctx:&'a Game
}
impl PlayerManOperator<'_> {
    pub fn new(ctx:& Game) -> PlayerManOperator {
        PlayerManOperator{
            ctx,
        }
    }
    fn remove_player_in_data(&mut self,pid:PlayerId){
        let r=self.ctx.player_man_mut().playerid_2_player.remove(&pid);
        match r{
            None => {}
            Some(p) => {
                self.ctx.player_man_mut().clientid_2_playerid.remove(&p.client_id);
                self.ctx.player_man_mut().entityid_2_playerid.remove(&p.entity_id);
            }
        }

    }

}
pub struct PlayerConnectionHandler<'a>{
    ctx:&'a mut Game
}
impl PlayerConnectionHandler<'_> {
    pub fn new(ctx:& mut Game) -> PlayerConnectionHandler {
        PlayerConnectionHandler{ctx}
    }
    pub async fn on_player_disconnect(&mut self,cid:ClientId){
        println!("on_player_disconnect");
        let p = (self.ctx.player_man_ref().get_player_by_cid(cid));
        entity::EntityOperator::new(self.ctx)
            .remove_player_entity_in_game(p).await;

        PlayerManOperator::new(self.ctx)
            .remove_player_in_data(p.player_id);
        // self.ctx.player_manager.get_player_mut()
    }
}

pub async fn send_player_basic_and_chunk(game:&Game,playerid:PlayerId,player_entity_id:EntityId){
    {
        let player= game.player_man_ref().playerid_2_player.get(&playerid).unwrap();
        let entity=game.entity_get(&player_entity_id).unwrap();
        // 1.player基本信息（player_entity_id
        net_send::player_basic(game.client_man_ref(), player, entity).await;
        // // 2.区块地形
        // net_send::player_interested_chunk_block_data(game.client_man_ref(),
        //                                              player, entity, game
        // ).await;
        // // 3.感兴趣区块的entity数据
        // net_send::player_interested_chunk_entity_data(game.client_man_ref(),
        //                                               player, entity, game
        // ).await;
    }
}
pub async fn handle_pack(context:&mut Game,msg:& ClientMsg) -> bool {
    return match &msg.msg_enum {
        MsgEnum::MainPlayerMoveCmd(cmd)
        => {
            if msg.client_type==ClientType::ClientType_Player{
                let cpcmd=cmd.clone();
                player_event::cmd_move::call(msg.client_id, context, cpcmd).await;

            }
            // println!("MainPlayerMoveCmd");
            false
        }
        MsgEnum::MainPlayerJumpCmd(cmd)=> {
            if msg.client_type==ClientType::ClientType_Player {
                let cpcmd = cmd.clone();
                player_event::cmd_jump::call(msg.client_id,
                                             context, cpcmd).await;
            }

            // println!("MainPlayerJumpCmd");
                false

        }
        _ => {
            true
        }
    }
}


