use crate::*;
use std::collections::{LinkedList, HashMap};
use crate::player::Player;
use tokio::net::TcpStream;



pub struct PlayerManager {
    id_cnt: player::PlayerId,
    pub playerid_2_player: HashMap<
        player::PlayerId,
        Rc<RefCell<Player>>
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

    pub fn create_player(&mut self) -> Rc<RefCell<Player>> {
        let player=
            self.playerid_2_player.entry(self.id_cnt)
                .or_insert(
                    Rc::new(
                        RefCell::from(
                            Player::create_uninited()
                        )
                    )
                );
        player.borrow_mut().player_id=self.id_cnt;
        self.id_cnt+=1;

        return player.clone();
    }
}