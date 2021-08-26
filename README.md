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

- 2021/8/24-25

  修复tcp接收bug，protobuf可以正常接收

  下一步：

   - [x] protoc文件生成

   - [ ] protobuf解析

   - [ ] ue client 包解析完后，将数据作为事件派发给游戏，游戏循环对事件进行响应和处理

     考虑一个问题：使用ecs还是普通的list来存放事件|准备用ecs

     需要添加一个特性:函数调用时返回entity id，以便动态的删除,这里只要用个trick，把entityid也一起作为组件，便可以获取到了

     然后要考虑到一个问题，往ecs中添加组件，必然会操作内存，那么怎么保证线程安全又不太降低效率

     - 实现一个队列，队列长度弄成原子性的，
       - 操作步骤：
         - 加入节点{上一个节点的下一个节点为此，此时可能上一个节点正在被取出{头节点（指针)变为下一个节点}}，长度+1（原子性）
         - 拷贝节点，长度-1

     

- 2021/8/18-23

  rust server 的基本结构，protobuf打包发出

  

- 2021/8/17

  方块放置完成

  ![image-20210816195208272](https://hanbaoaaa.xyz/tuchuang/images/2021/08/16/image-20210816195208272.png)

- 2021/8/16

  基本完成放置方块预览

  ![image-20210815195843489](https://hanbaoaaa.xyz/tuchuang/images/2021/08/15/image-20210815195843489.png)

- 2021/8/15

  射线碰撞检测，

  ![image-20210815030953538](https://hanbaoaaa.xyz/tuchuang/images/2021/08/15/image-20210815030953538.png)

- 2021/8/14

  - 发光方块，用来作为以后选择方块用的

    ![image-20210813232434571](https://hanbaoaaa.xyz/tuchuang/images/2021/08/13/image-20210813232434571.png)

- 2021/8/13

  - 区块构建的卡顿修复，原因是在一个mesh component下create会复制之前的数据，

  - 无限加载

    

