use crate::game::{Game, part_server_sync,game_player,game_entity};
use crate::{protos, conv, net_pack_convert};
use crate::async_task::AsyncTask;
use crate::base_type::point3f_new2;
use crate::net_pack_convert::PackIds;


// pub async fn spawn_entity_in_ps_rpl