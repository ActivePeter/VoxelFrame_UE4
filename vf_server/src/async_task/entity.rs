use crate::game::{Game, part_server_sync, player, entity};
use crate::{protos, conv, net_pack};
use crate::async_task::AsyncTask;
use crate::base_type::point3f_new2;
use crate::net_pack::PackIds;


// pub async fn spawn_entity_in_ps_rpl