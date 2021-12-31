开发具体过程记录

#### 2021/8/27-28

- [x] 根据解析的数据创建chunk

  细节：解析的区块数据在proto中是string类型。赋值会进行深拷贝（[C++ std::string写时复制与深浅拷贝 - 不写诗的诗人小安 - 博客园 (cnblogs.com)](https://www.cnblogs.com/anhongyu/p/14108117.html)）所以我使用了swap，直接将其内存交换给chunk,避免了不必要的内存创建和拷贝，减少内存波动

- [ ] 人物移动数据，同步给服务器

  - [x] 客户端发送需要先解决一个线程间通信以及socket阻塞读取的问题

    所以先实现一个简易的安全队列

    ```c++
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
    	[weak_this]()
    	{
    		while (1)
    		{
    			auto _this = weak_this.lock();
    			if (!(_this && _this->running))
    			{
    				break;
    			}
    			_this->msg_send_queue.if_empty_wait();
    			while (!_this->msg_send_queue.empty())
    			{
    				auto msg = _this->msg_send_queue.de_queue();
    				//auto stream= std::ostream();
    				auto msg_string = msg->SerializeAsString(
    				int32 offset = 0;
    				while (offset < msg_string.size()) {
    					auto sent_size = 0;
    					_this->socket->Send(
    						(uint8_t*)msg_string.data() + offset,
    						msg_string.size(),
    						sent_size);
    					offset += sent_size;
    				}
    			}
          	}
    	});
    ```

    传入一个智能指针，这样析构之后，weak获取到的变成null，便结束进程了

  - [x] 然后是移动检测，我用ecs来做

  - [x] 下一步是封装移动的protobuf包

  - [ ] 下一步是服务器接收后，转发给感兴趣的所有用户，

- [ ] 服务器检测到人物区块变化，发送新的区块

#### 2021/8/24-26

修复tcp接收bug，protobuf可以正常接收

下一步：

 - [x] protoc文件生成 24-25

 - [x] protobuf解析(解析过程中修复了逻辑bug) 26

 - [x] ue client 包解析完后，将数据作为事件派发给游戏，游戏循环对事件进行响应和处理

   考虑一个问题：使用ecs还是普通的list来存放事件

   算了，，，还是直接一个list存得了，也就取出来要锁一下  26

   

#### 8/17

- [ ] 开始rust服务器部分
- [x] 方块破坏

#### 8/16

- [x] 方块放置
- [x] 指针优化。取消裸指针

### 8.15

射线碰撞具体逻辑

- [x] 1.判断是否是区块mesh

- [x] 2.通过区块mesh获取到chunk

- [ ] 3.判断是否为方块边界，

  - [x] 是

    直接计算出可放置方块位置

  - [ ] 否(暂不考虑)

    计算出可放置方块位置

- [x] 4.判断可放置方块位置是否可放置(有无方块，有无物体)

