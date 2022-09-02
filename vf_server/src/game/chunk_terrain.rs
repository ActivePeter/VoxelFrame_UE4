use rand_seeder::Seeder;
use rand_seeder::rand_core::RngCore;
// for next_u32
use rand_pcg::Pcg64;
use std::f32::consts::PI;
use crate::game::chunk;
use crate::game::chunk::{VF_CHUNK_WIDTH, Chunk, ChunkKey};
use crate::conv;
use std::collections::HashMap;
use std::collections::hash_map::RandomState;
use std::cmp::{min, max};
use glam::IVec3;            // or whatever you like

struct RandWithSeed {
    seed: String,
}

impl RandWithSeed {
    pub fn new(seed: String) -> RandWithSeed {
        RandWithSeed {
            seed
        }
    }
    //最终映射到(0,1)
    pub fn rand2f_range0_1f(&self, a: f32, b: f32) -> f32 {
        let mut rng: Pcg64 = Seeder::from("this is seed").make_rng();
        let u64_ = unsafe {
            let u32_a = std::mem::transmute::<f32, u32>(a);
            let u32_b = std::mem::transmute::<f32, u32>(b);

            (u32_a as u64) | u32_b as u64
        };
        rng.advance(u64_ as u128);

        let v = rng.next_u32() as u16;
        v as f32 / u16::MAX as f32
    }
    //计算随机方向的单位向量
    pub fn rand2f_unitvec(&self, a: f32, b: f32) -> glam::Vec2 {
        let rand_rad = self.rand2f_range0_1f(a, b) * 2.0 * PI;
        glam::Vec2::new(f32::cos(rand_rad), f32::sin(rand_rad))
    }
}

pub fn init_chunk_data(map: &mut HashMap<ChunkKey, Chunk>, c: &mut chunk::Chunk) {
    for x in 0..VF_CHUNK_WIDTH {
        for z in 0..VF_CHUNK_WIDTH {
            let gx = x + c.chunk_key.x * VF_CHUNK_WIDTH;
            let gz = z + c.chunk_key.z * VF_CHUNK_WIDTH;
            let gyoff = c.chunk_key.y * VF_CHUNK_WIDTH;
            let divide_y = (noise_algrithm::perlin_noise(
                gx as f32
                    // /10.0
                    / VF_CHUNK_WIDTH as f32
                ,
                gz as f32
                    // /10.0
                    / VF_CHUNK_WIDTH as f32,
            ) * 20.0) as i32+20;

            for y in 0..VF_CHUNK_WIDTH {
                if y + gyoff < divide_y {
                    c.chunk_data[conv::p_int_2_index_in_chunk(x, y, z)] = 1;
                } else {
                    c.chunk_data[conv::p_int_2_index_in_chunk(x, y, z)] = 0;
                }
            }
        }
    }
    let mut cp = ChunkProccessor {
        map,
        c,
    };
    cp
        .gen_rocks()
        .add_river()
    ;
}

//用于遍历一个区块中的方块坐标
struct ChunkBlockBoxIter {
    chunkkey: ChunkKey,
    pos_begin: glam::IVec3,
    //相对区块的坐标最小
    pos_end: glam::IVec3,
    //相对区块的坐标最大
    cur_pos: glam::IVec3,//当前遍历点
}

impl ChunkBlockBoxIter {
    pub fn new(chunkkey: ChunkKey,
               pos_begin: glam::IVec3,//相对区块的坐标最小
               pos_end: glam::IVec3,//相对区块的坐标最大
    ) -> ChunkBlockBoxIter {
        ChunkBlockBoxIter {
            chunkkey,
            pos_begin,
            pos_end,
            cur_pos: pos_begin,
        }
    }
    pub fn reset_x(&mut self){
        self.cur_pos.x=self.pos_begin.x;
    }
    pub fn reset_y(&mut self){
        self.cur_pos.y=self.pos_begin.y;
    }
    pub fn reset_z(&mut self){
        self.cur_pos.z=self.pos_begin.z;
    }
    pub fn plus_x(&mut self, v: i32) -> bool {
        if v > 0 {
            let max = max(self.pos_begin.x, self.pos_end.x);
            if self.cur_pos.x + v <= max {
                self.cur_pos.x += v;
                return true;
            }
        } else if v < 0 {
            let min = min(self.pos_begin.x, self.pos_end.x);
            if self.cur_pos.x + v >= min {
                self.cur_pos.x += v;
                return true;
            }
        }
        false
    }
    pub fn plus_y(&mut self, v: i32) -> bool {
        if v > 0 {
            let max = max(self.pos_begin.y, self.pos_end.y);
            if self.cur_pos.y + v <= max {
                self.cur_pos.y += v;
                return true;
            }
        } else if v < 0 {
            let min = min(self.pos_begin.y, self.pos_end.y);
            if self.cur_pos.y + v >= min {
                self.cur_pos.y += v;
                return true;
            }
        }
        false
    }
    pub fn plus_z(&mut self, v: i32) -> bool {
        if v > 0 {
            let max = max(self.pos_begin.z, self.pos_end.z);
            if self.cur_pos.z + v <= max {
                self.cur_pos.z += v;
                return true;
            }
        } else if v < 0 {
            let min = min(self.pos_begin.z, self.pos_end.z);
            if self.cur_pos.z + v >= min {
                self.cur_pos.z += v;
                return true;
            }
        }
        false
    }
    pub fn with_globalp_and_index(&mut self) -> (glam::IVec3, usize) {
        (self.chunkkey.get_world_pos()+self.cur_pos, conv::p_int_2_index_in_chunk(self.cur_pos.x, self.cur_pos.y, self.cur_pos.z))
    }
}

struct ChunkProccessor<'a> {
    map: &'a mut HashMap<ChunkKey, Chunk>,
    c: &'a mut chunk::Chunk,
}

impl<'a> ChunkProccessor<'a> {
    pub fn gen_rocks(&mut self) -> &mut ChunkProccessor<'a> {
        let mut piter =ChunkBlockBoxIter::new(self.c.chunk_key.clone(),
                                              IVec3::new(VF_CHUNK_WIDTH - 1, VF_CHUNK_WIDTH - 1, VF_CHUNK_WIDTH - 1),
                                              IVec3::new(0, 0, 0),
        );
        loop{//x
            loop{//z
                let mut ycnt=0;
                loop {//y
                    let (_,index)=piter.with_globalp_and_index();
                    if ycnt==3{
                        self.c.chunk_data[index as usize]=2;
                    }else{
                        let b=self.c.chunk_data[index as usize];
                        if b!=0{
                            ycnt+=1;
                        }
                    }
                    if !piter.plus_y(-1){
                        piter.reset_y();
                        break;
                    }
                }
                if !piter.plus_z(-1){
                    piter.reset_z();
                    break;
                }
            }
            if !piter.plus_x(-1){
                piter.reset_x();
                break;
            }
        }

        self
    }
    pub fn add_river(&mut self) -> &mut ChunkProccessor<'a> {
        const RIVER_HEIGHT:i32=14;

        let chunkbeginp=self.c.chunk_key.get_world_pos();
        if RIVER_HEIGHT>chunkbeginp.y&&RIVER_HEIGHT<chunkbeginp.y+VF_CHUNK_WIDTH{
            let mut piter =ChunkBlockBoxIter::new(self.c.chunk_key.clone(),
                                                  IVec3::new(VF_CHUNK_WIDTH - 1, RIVER_HEIGHT-chunkbeginp.y, VF_CHUNK_WIDTH - 1),
                                                  IVec3::new(0, 0, 0),
            );
            loop{//x
                loop{//z
                    let mut ycnt=0;
                    loop {//y
                        let (_,index)=piter.with_globalp_and_index();
                        if self.c.chunk_data[index as usize]==0{
                            self.c.chunk_data[index as usize]=3;
                        }
                        if !piter.plus_y(-1){
                            piter.reset_y();
                            break;
                        }
                    }
                    if !piter.plus_z(-1){
                        piter.reset_z();
                        break;
                    }
                }
                if !piter.plus_x(-1){
                    piter.reset_x();
                    break;
                }
            }
        }
        // const RIVER_HEIGHT: i32 =10;
        // //判断周围区块已有水方块
        // //1.若有，则从该处开始flood fill

        // fn
        // self.c.chunk_key.get_beside(chunk::Side::XMinus1);
        // //2.若没有，则根据概率决定这个区块中小于坐标的位置是否有水，有则进行flood fill
        // // ,直到所有满足河流高度block格子都经过了flood fill判断

        self
    }
}


mod noise_algrithm {
    use crate::game::chunk_terrain::RandWithSeed;

    pub fn perlin_noise(x: f32, y: f32) -> f32 {
        fn lerp(a: f32, b: f32, rat: f32) -> f32 {
            fn lerp_f(x: f32) -> f32 {
                // 6x^5 - 15x^4 + 10x^3
                6.0 * x.powi(5) - 15.0 * x.powi(4) + 10.0 * x.powi(3)
            }
            a + (b - a) * lerp_f(rat)
        }
        let rander = RandWithSeed::new("this is seed".to_string());
        let p = glam::Vec2::new(x, y);
        let p_0_0 = p.floor();
        let p_1_0 = p_0_0 + glam::Vec2::new(1.0, 0.0);
        let p_0_1 = p_0_0 + glam::Vec2::new(0.0, 1.0);
        let p_1_1 = p_0_0 + glam::Vec2::new(1.0, 1.0);
        lerp(
            lerp(rander.rand2f_unitvec(p_0_0.x, p_0_0.y).dot(p - p_0_0),
                 rander.rand2f_unitvec(p_1_0.x, p_1_0.y).dot(p - p_1_0)
                 , p.x - p_0_0.x),
            lerp(rander.rand2f_unitvec(p_0_1.x, p_0_1.y).dot(p - p_0_1),
                 rander.rand2f_unitvec(p_1_1.x, p_1_1.y).dot(p - p_1_1)
                 , p.x - p_0_0.x),
            p.y - p_0_0.y,
        )
    }
}