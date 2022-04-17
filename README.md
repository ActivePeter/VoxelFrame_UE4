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

[Details](./record.md)

- 2022/4/16

  方块材质的动态加载，拼接到atlas，匹配block的uv setter

  ![image-20220416194802420](https://hanbaoaaa.xyz/tuchuang/images/2022/04/16/image-20220416194802420.png)

- 2022/2/6-4/1（期间在考研复试，还有毕设

  - [x] 服务端定时检测entity移动状态，并批量同步给对区块感兴趣的player
  - [x] operation封装，可以进行服务端确认后回退操作
  - [x] 放置方块的服务端同步

- 2022/2/5

  将player移动指令发送到server，并转发给part server，同时做出相应动作

- 2022/1/3

  最近主要是在思考并发模型了，细节我记到别处去

- 2021/8/27

  - [x] 根据解析的数据创建chunk
  - [x] 封装可阻塞等待的队列
  - [x] ecs遍历移动检测

  

- 2021/8/24-26

  修复tcp接收bug，protobuf可以正常接收

  protoc文件生成 24-25

  protobuf解析(解析过程中修复了逻辑bug) 26

  list+mutex实现用于多线程间通信队列，用于各类线程向主线程派发事件 26

  

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

    

