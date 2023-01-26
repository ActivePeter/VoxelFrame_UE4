use crate::*;
use tokio::sync::mpsc::Sender;
use crate::game::{ClientId, Game, entity, chunk};
use crate::protos::common;
use crate::net_pack_convert::{PackIds, MsgEnum};
use crate::game::entity::EntityId;
use crate::protos::common::ClientType;
use crate::net::{ClientMsgEnum, ClientMsg};
use crate::event::player_event;
use std::collections::HashMap;

pub type PlayerId = i32;

#[derive(Clone)]
pub struct Player {
    pub player_id:PlayerId,
    pub entity_id: EntityId,
    pub client_id:ClientId,
    // pub client:Client,
    // pub sender:Option<ClientSender>
}
impl Player{
    pub fn create_uninited()->Player{
        Player{
            player_id: 0,
            entity_id: 0,
            client_id:0,
            // sender:None,
            // client: Client::create_uninited(),
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

    //unwrap
    pub fn get_player_by_eid(&self,eid:EntityId)->&Player{
        let pid=self.entityid_2_playerid.get(&eid).unwrap();
        return self.playerid_2_player.get(pid).unwrap();
    }
}
pub struct PlayerManOperator<'a>{
    ctx:&'a mut Game
}
impl PlayerManOperator<'_> {
    pub fn new(ctx:& mut Game) -> PlayerManOperator {
        PlayerManOperator{
            ctx,
        }
    }
    fn remove_player_in_data(&mut self,pid:PlayerId){
        let r=self.ctx.player_manager.playerid_2_player.remove(&pid);
        match r{
            None => {}
            Some(p) => {
                self.ctx.player_manager.clientid_2_playerid.remove(&p.client_id);
                self.ctx.player_manager.entityid_2_playerid.remove(&p.entity_id);
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
        let p = (*self.ctx.player_manager.get_player_by_cid(cid)).clone();
        entity::EntityOperator::new(self.ctx)
            .remove_player_entity_in_game(&p).await;

        PlayerManOperator::new(self.ctx)
            .remove_player_in_data(p.player_id);
        // self.ctx.player_manager.get_player_mut()
    }
    pub async fn on_player_connect_getcid(&mut self,cid:ClientId){
        //////////////////////////////////////
        //  player 进来后所有数据操作

        // //1.获取player码以及绑定tcp
        let playerid =
            (self.ctx).player_manager.create_player_and_bind_client(cid);
        //
        //
        // entity=(game).spawn_entity_for_player(&player);

        //2.出生entity 这个过程是产生entity，
        let player_entity_id = entity::entity_spawn(self.ctx);

        //2.5产生完entity id 就与player绑定
        self.ctx.player_manager.set_player_entity_id(playerid,player_entity_id);

        //3.ps 创建entity
        let mut epos =protobuf::SingularPtrField::some( protos::common::EntityPos::new());
        let entity=self.ctx.entity_get(&player_entity_id).unwrap();
        {
            let epos_ref=epos.as_mut().unwrap();
            epos_ref.x = entity.position[0];
            epos_ref.y = entity.position[1];
            epos_ref.z = entity.position[2];
            epos_ref.entity_id = player_entity_id;
            epos_ref.t = protos::common::EntityType::T_Player;
        }

        //4.将player id 和entity id 加入区块
        {
            chunk::chunks_add_be_interested(self.ctx, playerid, player_entity_id).await;
            chunk::chunk_add_player(self.ctx, playerid, player_entity_id).await;

        }

        entity::spawn_entity_in_ps(self.ctx, epos).await;


        // println!("before add player -----------------------------------");

        // // println!("aft add player -----------------------------------");
        // //4.刷新被感兴趣的区块
        // game_chunk::chunks_add_be_interested(game, playerid, player_entity_id).await;

        // println!("aft1 add player -----------------------------------");
        ////////////////////////////////////
        //  player 进来后所有数据发送
        // println!("send player -----------------------------------");
        // 5.发送玩家进入后的全部内容
        // after_player_data_all_load(game,playerid,player_entity_id).await;
    }
}

pub async fn send_player_basic_and_chunk(game:&Game,playerid:PlayerId,player_entity_id:EntityId){
    {
        let player=
            game.player_manager.playerid_2_player.get(&playerid).unwrap();
        let entity=game.entity_get(&player_entity_id).unwrap();
        // 1.player基本信息（player_entity_id
        send::player_basic(&game.client_manager, player, entity).await;
        // 2.区块地形
        send::player_interested_chunk_block_data(&game.client_manager,
                                                 player,entity,game
        ).await;
        // 3.感兴趣区块的entity数据
        send::player_interested_chunk_entity_data(&game.client_manager,
                                                  player,entity,game
        ).await;
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


