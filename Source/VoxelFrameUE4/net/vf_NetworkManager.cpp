#include "./vf_NetworkManager.h"
//#include "ProtoPacks/chunk.pb.h"
#include "../game/GameContext.h"
#include "vf_pack.h"
#ifdef _MSC_VER
#pragma comment(lib,"ws2_32.lib")
#pragma comment (lib,"Advapi32.lib")
#pragma comment (lib,"Iphlpapi.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "userenv.lib")
#endif 
namespace VF
{


	// IVF_Obj /////////////////////////
	void NetworkManager::IVF_Obj_init(ContextId id) {
		IVF_Obj::IVF_Obj_init(id);
		VF_LogWarning("new network manager");
	}
	void NetworkManager::IVF_Obj_begin()
	{

	}
	void NetworkManager::IVF_Obj_end() {}
	////////////////////////////////////

	void NetworkManager::send(
		PackContainer&& msg)
	{
		msg_send_queue.Enqueue(std::move(msg));
	}

	//#pragma optimize( "", off )
	void NetworkManager::connectServer(
		std::shared_ptr<NetworkManager> netManager, std::string ip, int port)
	{
		int connfd = cli.createsocket(port, ip.c_str());
		if (connfd < 0) {
			VF_LogWarning("failed connect");
			return;
		}
		cli.onConnection = [netManager](const hv::SocketChannelPtr& channel) {
			std::string peeraddr = channel->peeraddr();
			if (channel->isConnected()) {
				netManager->running = true;
				VF_LogWarning("connected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
				AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
					[netManager]()
					{
						VF_LogWarning("newwork send loop start");
						while (1)
						{
							auto& _this = netManager;
							if (!_this)
							{
								break;
							}
							//auto _this = weak_this.lock();
							if (!(_this->running))
							{
								break;
							}
							//阻塞等待新的要发送的消息
							//VF_LogWarning(
							//	"newwork send loop"
							//	" before wait");
							//_this->msg_send_queue.if_empty_wait();
							/*if (!(_this->running))
							{
								break;
							}*/
							//VF_LogWarning(
							//	"newwork send loop"
							//	" after wait");

							while (!_this->msg_send_queue.IsEmpty())
							{
								//停止处理数据
								if (!(_this->running))
								{
									break;
								}
								PackContainer msg;
								_this->msg_send_queue.Dequeue(msg);
								//auto stream= std::ostream();
								auto msg_string = _vf_pack::conv_pack_to_bytes(msg);
								//_this->channel->write(msg_string);


								_this->cli.send(msg_string);
							}
						}
						VF_LogWarning("newwork send loop end");

					});
			}
			else {
				VF_LogWarning("disconnected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
			}
		};
		cli.onMessage = [netManager](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
			if (netManager->running)
			{
				netManager->handle_data((const char*)buf->data(), buf->size());
			}
		};
		cli.start();
		VF_LogWarning("cli start");

	}

	void NetworkManager::close()
	{
		VF_LogWarning("NetworkManager::close()");
		if (this->running)
		{
			//msg_send_queue.wake();
			this->running = false;
			cli.closesocket();
		}
	}

	//#pragma optimize( "", on )

	NetworkManager::~NetworkManager()
	{
		this->data_handle_record.body_buff.resize(0);
		VF_LogWarning("NetworkManager end");//析构前task未结束，需要注意
		this->close();
	}

	//#pragma optimize( "", off )
	bool NetworkManager::handle_data(const char* received_data, int _byte_cnt)
		//bool NetworkManager::handle_data(std::vector<uint8_t>& received_data, int _byte_cnt)
	{

		uint32_t handled_offset = 0;
		while ((int)handled_offset < _byte_cnt)
		{

			auto byte_cnt_left = _byte_cnt - handled_offset;
			//头上一次未接收全

			//VF_LogWarning("liio head");
			if (data_handle_record.head_rec_cnt < DataHandleRecord::Data_Head_Size)
			{
				//头本次还是未收全
				if (byte_cnt_left + data_handle_record.head_rec_cnt < DataHandleRecord::Data_Head_Size)
				{
					//VF_LogWarning("handle case 1");
					//memcpy(data_handle_record.get_head_write_pos(),
					//	received_data.data(),
					//	//接收大小
					//	byte_cnt_left);
					for (uint32_t i = 0; i < byte_cnt_left; i++)
					{
						assert(data_handle_record.head_rec_cnt + i < 5);
						data_handle_record.head_buff[data_handle_record.head_rec_cnt + i] = received_data[handled_offset + i];
					}
					data_handle_record.head_rec_cnt += byte_cnt_left;

					return true;
					//return false;
				}
				//头本次收全
				else
				{
					//VF_LogWarning("handle case 2");
					/*VF_LogWarning("handle case 2 %d %d %d %d %d %d",
						received_data[0],
						received_data[1],
						received_data[2],
						received_data[3],
						received_data[4],
						DataHandleRecord::Data_Head_Size - data_handle_record.head_rec_cnt);*/
					for (uint32_t i = 0; i < (uint32_t)(DataHandleRecord::Data_Head_Size - data_handle_record.head_rec_cnt); i++)
					{
						assert(data_handle_record.head_rec_cnt + i < 5);
						data_handle_record.head_buff[data_handle_record.head_rec_cnt + i] = received_data[handled_offset + i];
					}
					handled_offset += DataHandleRecord::Data_Head_Size - data_handle_record.head_rec_cnt;

					data_handle_record.calc_buf_2_pack_head();

					//memcpy(data_handle_record.get_head_write_pos(),
					//	received_data.data(),
					//	//剩余大小
					//	DataHandleRecord::Data_Head_Size - data_handle_record.head_rec_cnt);
					data_handle_record.head_rec_cnt = DataHandleRecord::Data_Head_Size;

					data_handle_record.update_body_buff_size();
					//准备好vector空间
					continue;
				}

			}
			//处理body pack
			// 1.剩余数据小于需要读的包数量(不够
			if (byte_cnt_left < data_handle_record.pack_head.pack_len -
				data_handle_record.body_rec_cnt)
			{
				//VF_LogWarning("handle case 3");
				//memcpy(data_handle_record,)
				data_handle_record.write_data_2_body(received_data + handled_offset, byte_cnt_left);
				//handled_offset += byte_cnt_left;
				return true;
			}
			else
				//完成读包
			{

				auto len = data_handle_record.pack_head.pack_len -
					data_handle_record.body_rec_cnt;
				//VF_LogWarning("handle case 4 %d %d", len, data_handle_record.pack_head.pack_len);
				data_handle_record.write_data_2_body(received_data + handled_offset, len);
				handled_offset += len;

				//处理包
				// ...
				_HandlePack::handle_pack_buff(
					get_context_id(),
					data_handle_record.pack_head.pack_id,
					data_handle_record.body_buff,
					data_handle_record.pack_head.pack_len);
				//GameContext::ecs
				//重置状态
				//VF_LogWarning("end one pack %d %d %d", data_handle_record.body_rec_cnt, data_handle_record.head_rec_cnt, total++);
				data_handle_record.reset();
				//VF_LogWarning("end one pack %d %d", data_handle_record.body_rec_cnt, data_handle_record.head_rec_cnt);
				/*while (1)
				{
				}*/
			}

		}


		//根据
		return true;
	}
	//#pragma optimize( "", on )
}
