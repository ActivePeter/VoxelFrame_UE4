pub use std::*;
pub use std::sync::Arc;
pub use tokio::sync::RwLock;
pub use std::collections::*;
pub use std::borrow::Borrow;
pub use std::convert::TryInto;

pub use arrayvec::ArrayVec;
pub use crate::game::*;
pub use crate::chunk::*;
pub use crate::player::*;
pub use std::sync::Weak as SyncWeak;

pub use crate::*;

/**type def**/
pub type Point3f = ArrayVec<f32, 3>;

pub use std::rc::Rc;
pub use tokio::io::{AsyncReadExt, AsyncWriteExt};
pub use tokio::net::TcpListener;

#[macro_export]
macro_rules! new {
    ($a:ident)=>{
          $a::new()
    };
    ($a:ident,$b:ident)=>{
          $a::new($b::new())
    };
    ($a:ident,$b:expr)=>{
          $a::new($b)
    };

    ($a:ident,$($b:tt )*)=>{
           $a::new(new!($($b)*))

    };
    // ($a:ident,$b:ident,$c:ident)=>{
    //       $a::new($b::new($c::new()))
    // };
    // ($a:ident,$b:ident,$c:expr)=>{
    //       $a::new($b::new($c))
    // };
}

/////////////////////////////////////////////////
pub type ArcRw<T> = Arc<RwLock<T>>;
#[macro_export]
macro_rules! ArcRw_new {
    ($a:expr)=>{
          Arc::new(RwLock::new($a))
        // new!(Arc,RwLock,$a)
    };
}
//////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pub struct BaseEntityData {
//     // #[serde(rename = "Pos")]
//     pub position: Point3f,
//     // #[serde(rename = "Rotation")]
//     // pub rotation: ArrayVec<f32, 2>,
//     // #[serde(rename = "Motion")]
//     // pub velocity: ArrayVec<f64, 3>,
// }
//
// impl BaseEntityData {
//     pub fn new() -> Self {
//         let mut bed = BaseEntityData {
//             position: Point3f::new()
//         };
//         bed.position.push(0.0);
//         bed.position.push(0.0);
//         bed.position.push(0.0);
//         return bed;
//     }
// }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pub trait ITick {
    fn tick(&mut self);
}
