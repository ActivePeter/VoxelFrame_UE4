use crate::game::chunk::ChunkKey;
use std::collections::{HashMap, LinkedList, BTreeMap, HashSet};
use crate::game::ClientId;
use pa_queue_and_chan::priotity_async_mpsc_chan::Sender;
use crate::net::net_pack::{PackPriority, PackIds};
use tokio::sync::mpsc::error::TryRecvError;
use std::time::Duration;
use crate::net::ClientSender;
use std::cell::{RefCell, UnsafeCell, SyncUnsafeCell};
use std::sync::{Arc, Weak};

pub(crate) struct SendChunkTask{
    data:Vec<u8>,
    ck:ChunkKey,// same chunk can be de-weight
    cid:ClientId,
    sender:Option<ClientSender>
}
impl SendChunkTask{
    pub fn new(data:Vec<u8>,
               ck:ChunkKey,// same chunk can be de-weight
               cid:ClientId,
               sender:ClientSender) -> SendChunkTask {
        SendChunkTask{
            data,
            ck,
            cid,
            sender: Some(sender)
        }
    }
}

struct OneClientRecord{
    chunkdata:HashMap<ChunkKey,Arc<SyncUnsafeCell<Vec<u8>>>>,
}

pub(crate) struct ChunkSendWorker{
    cliend_2_chunk_datas:HashMap<ClientId,OneClientRecord>,
    victim_queue:LinkedList<VictimQueueElem>,
}
struct VictimQueueElem{
    cid:ClientId,
    ck:ChunkKey,
    sender:ClientSender,
    data:Weak<SyncUnsafeCell<Vec<u8>>>
}
unsafe impl Sync for VictimQueueElem{}
unsafe impl Send for VictimQueueElem{}
unsafe impl Sync for ChunkSendWorker{}
unsafe impl Send for ChunkSendWorker{}
impl ChunkSendWorker{
    // fn send_one_client_data(&mut self,client:ClientId){
    //     self.cl
    // }
    async fn take_and_send_one(&mut self){
        while let Some(elem)=self.victim_queue.pop_front(){
            // println!("take one {} {}",elem.data.strong_count(),elem.data.weak_count());
            if elem.data.strong_count()==1{
                let one_client_record=self.cliend_2_chunk_datas.get_mut(&elem.cid).unwrap();
                let mut data =one_client_record.chunkdata.remove(&elem.ck).unwrap();
                let mut take=vec![];
                std::mem::swap(unsafe{&mut *data.get() },&mut take);
                //send
                elem.sender.send(take,PackIds::EChunkPack.default_priority()).await;
                // println!("send one");
                return;
            }
        }
    }
    pub(crate) async fn spawn() -> tokio::sync::mpsc::Sender<SendChunkTask> {
        let (tx, mut rx)= tokio::sync::mpsc::channel::<SendChunkTask>(100);
        tokio::spawn(async move{
            let mut worker=ChunkSendWorker{
                cliend_2_chunk_datas: Default::default(),
                victim_queue: Default::default()
            };
            'outer: loop{
                loop {// all take out
                    match rx.try_recv(){
                        Ok(mut task) => {
                            let mut data =Arc::new(SyncUnsafeCell::new(vec![]));
                            worker.victim_queue.push_back(VictimQueueElem{
                                cid: task.cid,
                                ck: task.ck.clone(),
                                sender: task.sender.take().unwrap(),
                                data: Arc::downgrade(&data)
                            });
                            std::mem::swap(unsafe{&mut* data.get() },&mut task.data);
                            if let Some(record)=worker.cliend_2_chunk_datas.get_mut(&task.cid){
                                record.chunkdata.insert(task.ck,data);
                            }else{
                                let mut ret=OneClientRecord{
                                    chunkdata: Default::default()
                                };
                                ret.chunkdata.insert(task.ck,data);
                                worker.cliend_2_chunk_datas.insert(task.cid,ret);
                            };
                            continue;
                        }
                        Err(e) => {
                            match e{
                                TryRecvError::Empty => {
                                    break;
                                }
                                TryRecvError::Disconnected => {
                                    break 'outer;
                                }
                            }
                        }
                    }
                }
                // take out an oldest msg
                worker.take_and_send_one().await;
                tokio::time::sleep(Duration::from_millis(10)).await;
            }
        });
        tx
    }
}