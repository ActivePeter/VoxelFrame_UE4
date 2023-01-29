use crate::game::chunk::{ChunkKey, VF_CHUNK_WIDTH};
use crate::game::Game;
use crate::game::chunk_terrain::RandWithSeed;
use crate::conv;
use crate::game::block::block_type::{BlockGrass, Block, BlockDirt, BlockAir, BlockRock};
use crate::game::block::block_type;
use crate::game::terrain_gen::model::Model;
use glam::IVec3;

impl Model{
    fn new_tree() -> Model {
        let x_w=7 as i32;
        let z_w=7 as i32;
        let h=15;
        let mut data =Vec::new();
        data.resize((x_w * z_w * h) as usize, BlockAir::block_type_id());
        let mut model=Model{
            data,
            x_w,
            z_w,
            h,
            center_x: 3,
            center_y: 0,
            center_z: 3
        };
        for x in 0..x_w{
            for z in 0..z_w{
                for y in h-5..h {
                    model.set_data_at(x,y,z,BlockRock::block_type_id());
                }
            }
        }
        for y in 0..h-4{
            let x=3;
            let z=3;
            model.set_data_at(x,y,z,BlockDirt::block_type_id());
        }
        model
    }
}

pub(crate) trait TerrainGenTree{
    fn terrain_gen_tree(&self,chunk_key:&ChunkKey);
}

impl TerrainGenTree for Game{
    fn terrain_gen_tree(&self,chunk_key:&ChunkKey) {
        let mut gen_one_tree=| x:i32,z:i32|{
            // println!("gen one tree {} {}",x,z);
            let mut tree_chunkkey=conv::point3i_2_chunkkey2(x,0,z);
            // println!("chunk {} {}",tree_chunkkey.x,tree_chunkkey.z);
            for chunk_y in -1..2{
                tree_chunkkey.y=chunk_y;
                let floor={
                    let chunk=self.try_load_chunk_get_mut(&tree_chunkkey);
                    chunk.find_floor(x,z)
                };
                if let Some((y,blocktype))=floor{
                    if  block_type::can_plant_tree(blocktype){
                        // println!("plant tree at {} {} {}",x,y,z);
                                 Model::new_tree().try_apply_to_world(self,x,y,z);
                        // prepare a tree model
                    }
                }
            }
        };
        //gen tree grid size
        let gen_tree_grid_size=19;
        let chunk_origin_p=chunk_key.get_world_pos();
        let chunk_end_p=chunk_origin_p+IVec3::new(VF_CHUNK_WIDTH,VF_CHUNK_WIDTH,VF_CHUNK_WIDTH);
        //chunk width is 32
        // find  19 sized grid in chunk
        // find >=chunk_orgin_p and %19==0
        fn find_in_range_exact_divide(begin:i32,end:i32,v:i32)->Vec<i32>{
            let mut ret=vec![];
            let mut cur=0;
            //find first
            for i in begin..end{
                if i%v==0{
                    ret.push(i);
                    cur=i+v;
                    break;
                }
            }
            while cur<end{
                ret.push(cur);
                cur+=v;
            }
            ret
        }
        let xs=find_in_range_exact_divide(chunk_origin_p.x,chunk_end_p.x,gen_tree_grid_size);
        let zs=find_in_range_exact_divide(chunk_origin_p.z,chunk_end_p.z,gen_tree_grid_size);
        // println!("find {} {}",xs.len(),zs.len());
        for x in &xs {
            for z in &zs{
                let mut x =*x as f32;
                let mut z =*z as f32;
                let rander = RandWithSeed::new("this is seed".to_string());
                let vec=rander.rand2f_unitvec(x,z);
                x+=vec.x*gen_tree_grid_size as f32 /2.0;
                z+=vec.y*gen_tree_grid_size as f32 /2.0;
                let x =x as i32;
                let z =z as i32;
                gen_one_tree(x,z);
            }
        }
    }
}