
use rand_seeder::Seeder;
use rand_seeder::rand_core::RngCore;         // for next_u32
use rand_pcg::Pcg64;
use std::f32::consts::PI;
use crate::game::chunk;
use crate::game::chunk::{VF_CHUNK_WIDTH, Chunk, ChunkKey};
use crate::conv;
use std::collections::HashMap;
use std::collections::hash_map::RandomState;            // or whatever you like

struct RandWithSeed{
    seed:String
}
impl RandWithSeed{
    pub fn new(seed:String) -> RandWithSeed {
        RandWithSeed{
            seed
        }
    }
    //最终映射到(0,1)
    pub fn rand2f_range0_1f(&self,a:f32,b:f32)->f32{
        let mut rng: Pcg64 =Seeder::from("this is seed").make_rng();
        let u64_= unsafe{
            let u32_a=std::mem::transmute::<f32, u32>(a)   ;
            let u32_b=std::mem::transmute::<f32,u32>(b);

            (u32_a as u64)|u32_b as u64
        };
        rng.advance(u64_ as u128);

        let v=rng.next_u32()as u16;
        v as f32/u16::MAX as f32
    }
    //计算随机方向的单位向量
    pub fn rand2f_unitvec(&self,a:f32,b:f32)->glam::Vec2{
        let rand_rad=self.rand2f_range0_1f(a,b)*2.0*PI;
        glam::Vec2::new(f32::cos(rand_rad),f32::sin(rand_rad))
    }
}
pub fn init_chunk_data(map: &mut HashMap<ChunkKey, Chunk>, c: &mut chunk::Chunk){
    for x in 0..VF_CHUNK_WIDTH {
        for z in 0..VF_CHUNK_WIDTH {
            let gx=x+c.chunk_key.x*VF_CHUNK_WIDTH;
            let gz=z+c.chunk_key.z*VF_CHUNK_WIDTH;
            let gyoff=c.chunk_key.y*VF_CHUNK_WIDTH;
            let divide_y=(noise_algrithm::perlin_noise(
                gx as f32
                    // /10.0
                    /VF_CHUNK_WIDTH as f32
                ,
                gz as f32
                    // /10.0
                    /VF_CHUNK_WIDTH as f32
            ) *20.0)as i32;

            for y in 0..VF_CHUNK_WIDTH {
                if y+gyoff<divide_y {
                    c.chunk_data[conv::p_int_2_index_in_chunk(x,y,z)] = 1;
                } else {
                    c.chunk_data[conv::p_int_2_index_in_chunk(x,y,z)] = 0;
                }
            }
        }
    }
    let mut cp =ChunkProccessor{
        map,
        c
    };
    cp.add_river();
}


struct ChunkBlockBoxIter{
        
}


struct ChunkProccessor<'a> {
    map: &'a mut HashMap<ChunkKey, Chunk>,
    c: &'a mut chunk::Chunk
}

impl<'a> ChunkProccessor<'a>{
    pub fn add_river(&mut self){
        // const RIVER_HEIGHT: i32 =10;
        // //判断周围区块已有水方块
        // //1.若有，则从该处开始flood fill

        // fn
        // self.c.chunk_key.get_beside(chunk::Side::XMinus1);
        // //2.若没有，则根据概率决定这个区块中小于坐标的位置是否有水，有则进行flood fill
        // // ,直到所有满足河流高度block格子都经过了flood fill判断


    }
}


mod noise_algrithm{
    use crate::game::chunk_terrain::RandWithSeed;

    pub fn perlin_noise(x:f32, y:f32) -> f32 {
        fn lerp(a:f32, b:f32, rat:f32) -> f32 {
            fn lerp_f(x:f32)->f32{
                // 6x^5 - 15x^4 + 10x^3
                6.0*x.powi(5) - 15.0*x.powi(4) + 10.0*x.powi(3)
            }
            a+(b-a)*lerp_f(rat)
        }
        let rander=RandWithSeed::new("this is seed".to_string());
        let p=glam::Vec2::new(x,y);
        let p_0_0=p.floor();
        let p_1_0=p_0_0+glam::Vec2::new(1.0,0.0);
        let p_0_1=p_0_0+glam::Vec2::new(0.0,1.0);
        let p_1_1=p_0_0+glam::Vec2::new(1.0,1.0);
        lerp(
            lerp(rander.rand2f_unitvec(p_0_0.x,p_0_0.y).dot(p-p_0_0),
                 rander.rand2f_unitvec(p_1_0.x,p_1_0.y).dot(p-p_1_0)
                 ,p.x-p_0_0.x),
            lerp(rander.rand2f_unitvec(p_0_1.x,p_0_1.y).dot(p-p_0_1),
                 rander.rand2f_unitvec(p_1_1.x,p_1_1.y).dot(p-p_1_1)
                 ,p.x-p_0_0.x),
            p.y-p_0_0.y
        )
    }
}