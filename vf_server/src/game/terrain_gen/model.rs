use crate::game::block::block_type::BlockTypeId;
use crate::game::Game;
use glam::IVec3;
use crate::conv;
use crate::game::chunk::{VF_CHUNK_WIDTH, ChunkKey, Chunk, ChunkLoadStage};

pub(crate) struct Model{
    pub(crate) data:Vec<u8>,
    pub(crate) x_w:i32,
    pub(crate) z_w:i32,
    pub(crate) h:i32,
    pub(crate) center_x:i32,//中心点相对整个区域的位置
    pub(crate) center_y:i32,
    pub(crate) center_z:i32,
}
impl Model{
    #[inline]
    pub(crate) fn set_data_at(&mut self,x:i32,y:i32,z:i32,data:BlockTypeId){
        self.data[(x + y * self.x_w + z * self.x_w * self.h)as usize]=data;
    }
    #[inline]
    pub(crate) fn get_data_at(&self,p:IVec3)->BlockTypeId{
        // println!("get_data_at {} {} {}",p.x,p.y,p.z);
        self.data[(p.x + p.y * self.x_w + p.z * self.x_w * self.h) as usize]
    }
    pub(crate) fn try_apply_to_world(&self, game:&Game, x:i32, y:i32, z:i32) -> bool {
        //中心点绝对-0，0，0绝对=中心点相对
        // =》0，0，0绝对=中心点绝对-中心点相抵
        let g_0_0_0=IVec3::new(
            x-self.center_x,
            y-self.center_y,
            z-self.center_z);
        let g_1_1_1=g_0_0_0+IVec3::new(self.x_w,self.h,self.z_w);
        //计算关联区块范围，
        let ck_000=conv::point3i_2_chunkkey2(g_0_0_0.x,g_0_0_0.y,g_0_0_0.z);
        // println!("{} {} {} ",g_0_0_0.x,g_0_0_0.y,g_0_0_0.z);
        // println!("{} {} {} ",ck_000.x,ck_000.y,ck_000.z);
        let ck_111=conv::point3i_2_chunkkey2(g_1_1_1.x-1,g_1_1_1.y-1,g_1_1_1.z-1);
        fn foreach<F>(game:&Game, g_0_0_0:&IVec3, g_1_1_1:&IVec3, ck_000:&ChunkKey, ck_111:&ChunkKey, f: F) where
        // 闭包没有输入值和返回值。
            F: Fn(&IVec3,&mut Chunk)->bool{
            for ckx in ck_000.x..ck_111.x+1{
                for cky in ck_000.y..ck_111.y+1{
                    for ckz in ck_000.z..ck_111.z+1{
                        fn get_range(rangebegin:i32,rangeend:i32,ck:i32)->(i32,i32){
                            (if rangebegin<ck*VF_CHUNK_WIDTH{
                                ck*VF_CHUNK_WIDTH
                            }else{
                                rangebegin
                            },if rangeend>=(ck+1)*VF_CHUNK_WIDTH{
                                (ck+1)*VF_CHUNK_WIDTH
                            }else{
                                rangeend
                            })
                        }
                        //calc the block range
                        let (beginx,endx)=get_range(g_0_0_0.x,g_1_1_1.x,ckx);
                        let (beginy,endy)=get_range(g_0_0_0.y,g_1_1_1.y,cky);
                        let (beginz,endz)=get_range(g_0_0_0.z,g_1_1_1.z,ckz);
                        let chunk=game.try_load_chunk_get_mut(&ChunkKey{
                            x:ckx,
                            y:cky,
                            z:ckz
                        });
                        // println!("range {} {} {} {}",g_0_0_0,g_1_1_1,beginy,endy);
                        for bx in beginx..endx{
                            for by in beginy..endy{
                                for bz in beginz..endz{
                                    let global_bp=IVec3::new(bx,by,bz);
                                    if !f(&global_bp,chunk){
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        //遍历区块范围
        foreach(game, &g_0_0_0, &g_1_1_1, &ck_000, &ck_111, |global_bp, chunk|{
            if chunk.load_stage==ChunkLoadStage::End{
                return false;
            }
            let to_model_p=*global_bp-g_0_0_0;
            if self.get_data_at(to_model_p)!=0{
                if chunk.block_get_at_idx(conv::p_int_2_index_in_chunk(to_model_p.x,to_model_p.y,to_model_p.z))!=0{
                    // panic!();
                    //冲突
                    return false;
                }
            }
            true
        });
        //遍历区块范围
        foreach(game, &g_0_0_0, &g_1_1_1, &ck_000, &ck_111, |global_bp, chunk|{
            // chunk.set_block_at(&IVec3::new(20,20,20),1);
            let to_model_p=*global_bp-g_0_0_0;
            let data=self.get_data_at(to_model_p);
            if data!=0{
                // println!("setblock {} {} {}",chunk.chunk_key.x,chunk.chunk_key.y,chunk.chunk_key.z);
                chunk.set_block_at(global_bp,data);
            }
            true
        });
        true
    }
}