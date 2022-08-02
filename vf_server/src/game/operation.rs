use crate::game::{ClientId, ClientOperationId, Game};
use crate::protos;
use crate::net_pack_convert::PackIds;

pub enum OperationResult{
    Succ=0,
    Fail=1,
}
pub async fn send_player_operation_result(
    ctx: &mut Game,cid:ClientId,opid:ClientOperationId,res:OperationResult
){
    match res {
        OperationResult::Succ => {

            let mut pack=protos::common::ClientOperationSucc::new();
            pack.operation_id=opid;
            // let vec=net_pack_convert::pack_to_bytes(pack,PackIds::EClientOperationSucc);
            // let sender=
            ctx.client_manager.get_sender(cid)
                .serialize_and_send(pack,PackIds::EClientOperationSucc).await;
            // sender.send(vec).await;
        }
        OperationResult::Fail => {

            let mut pack=protos::common::ClientOperationFailed::new();
            pack.operation_id=opid;
            ctx.client_manager.get_sender(cid)
                .serialize_and_send(pack,PackIds::EClientOperationFailed).await;
            // sender.send(vec).await;
        }
    }
}