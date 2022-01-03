use crate::*;
use std::collections::{LinkedList, HashMap};
use crate::player::{Player, PlayerId};
use tokio::net::TcpStream;
use crate::client::{Client, ClientSender};


pub struct PlayerManager {
    id_cnt: player::PlayerId,
    pub playerid_2_player: HashMap<
        player::PlayerId,
        Player
    >,
    // pub msg_list: LinkedList<Vec<u8>>,
}
impl PlayerManager{
    pub fn create_once()->PlayerManager{
        return PlayerManager{
            id_cnt: 0,
            playerid_2_player: Default::default(),
            // msg_list: Default::default()
        }
    }

    pub fn create_player_and_bind_client(&mut self,client_sender:ClientSender) -> PlayerId {
        let player=
            self.playerid_2_player.entry(self.id_cnt)
                .or_insert(
                    Player::create_uninited()
                );
        player.player_id=self.id_cnt;
        self.id_cnt+=1;
        player.sender=Some(client_sender);

        return player.player_id;
    }

    pub fn get_player_handle(&mut self, playerid :&PlayerId) -> Option<&mut Player> {
        return self.playerid_2_player.get_mut(playerid);
    }
}