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

pub type Point3f = ArrayVec<f32, 3>;

pub use tokio::io::{AsyncReadExt, AsyncWriteExt};
pub use tokio::net::TcpListener;

#[macro_export]
macro_rules! new {
    ($a:ident)=>{
        $a::new()
    };
    ($a:expr)=>{
           $a
    };
    ($a:ident $b:ident)=>{
        $a::new($b::new())
    };
    ($a:ident $($b:tt )*)=>{
           $a::new(new!($($b)*))

    };
}

/////////////////////////////////////////////////
pub type ArcRw<T> = Arc<RwLock<T>>;
#[macro_export]
macro_rules! ArcRw_new {
    ($a:ident)=>{
          new!(Arc RwLock $a)
    };
    ($a:expr)=>{
          new!(Arc RwLock $a)
    };
}
//////////////////////////////////////////////////

