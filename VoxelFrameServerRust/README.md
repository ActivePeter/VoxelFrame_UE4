server of VoxelFrame, written in rust.



## run

cargo run



开发思路：

8/18

首先是玩家进入，给他分配了一个坐标，（addPlayer）然后加载周围的chunk（player.tick），然后发回给玩家,

