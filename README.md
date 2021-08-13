# VoxelFrame_UE4
## Introduction（介绍

- I likes playing minecraft very much and it's always been my dream to write a mc.

  我非常喜欢玩minecraft，写一个mc是我长久以来一直想做到的事

![image-20210609143052106](https://hanbaoaaa.xyz/tuchuang/images/2021/06/09/image-20210609143052106.png)

## Organization（组织

- [VoxelFrame (github.com)](https://github.com/VoxelFrame)   

  To store related projects.

  用于存储相关项目

## Language（语言

- c++(client), rust(server) ~~golang~~ 

## Engine Version

- ue4.26

## Related Projects（相关项目

- #### paecs（entity component system)

  [https://github.com/ActivePeter/paecs](https://github.com/ActivePeter/paecs)

  I'm also trying to implement a ecs lib and use it in my mc

  我也在尝试实现一个ecs库并用在我的mc里

- #### raw opengl version

  [https://github.com/ActivePeter/VoxelFrame](https://github.com/ActivePeter/VoxelFrame)

- #### unity version

  [VoxelFrame/VoxelFrame_Abandoned (github.com)](https://github.com/VoxelFrame/VoxelFrame_Abandoned)

## Progress（進度

- 2021/8/14

  - 发光方块，用来作为以后选择方块用的

    ![image-20210813232434571](https://hanbaoaaa.xyz/tuchuang/images/2021/08/13/image-20210813232434571.png)

- 2021/8/13

  - 区块构建的卡顿修复，原因是在一个mesh component下create会复制之前的数据，

  - 无限加载

    ![](./image/Compress_20210813_155849.gif)

    

