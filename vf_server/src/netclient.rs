use crate::game::{Game, ClientId};
use crate::protos::common::ClientType::ClientType_Player;
use crate::part_server_sync;
use crate::game::game_player::PlayerConnectionHandler;

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
        let ctype=self.ctx.client_manager.get_clienttype(cid).unwrap();
        if ctype==ClientType_Player {
            PlayerConnectionHandler::new(self.ctx).on_player_disconnect(cid).await;

        }else{
            part_server_sync::on_partserver_disconnect(self.ctx,cid).await;
        }
    }
}
