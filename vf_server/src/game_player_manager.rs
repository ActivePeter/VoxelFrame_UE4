use crate::*;
use std::collections::{LinkedList, HashMap};
use crate::game_player::{Player, PlayerId};
use tokio::net::TcpStream;
use crate::game::ClientId;


pub struct PlayerManager {
    id_cnt: game_player::PlayerId,
    pub playerid_2_player: HashMap<
        game_player::PlayerId,
        Player
    >,
    pub clientid_2_playerid:HashMap<ClientId,PlayerId>,
    // pub msg_list: LinkedList<Vec<u8>>,
}
impl PlayerManager{
    pub fn create_once()->PlayerManager{
        return PlayerManager{
            id_cnt: 0,
            playerid_2_player: Default::default(),
            clientid_2_playerid:Default::default(),
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

    pub fn get_player_handle(&mut self, playerid :&PlayerId) -> Option<&mut Player> {
        return self.playerid_2_player.get_mut(playerid);
    }

    pub fn get_player_by_cid(&self, cid:ClientId) -> &Player {
        let pid=self.clientid_2_playerid.get(&cid).unwrap();
        return self.playerid_2_player.get(pid).unwrap();
    }
}