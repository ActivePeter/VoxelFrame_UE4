#pragma once
//#include "hv/TcpClient.h"
#include "VF_Base.h"
#include "Networking.h"
//#include "base/SafeQuene.h"
//#include "Protobuf.h"
#include <google/protobuf/message.h>
#include "vf_pack.h"
THIRD_PARTY_INCLUDES_START
#include "hv/TcpClient.h"
THIRD_PARTY_INCLUDES_END
//#include "NetThreads.h"

namespace VF
{

	//处理过程中的数据
	// 1.剩余长度
	// 2.总长度
	//
	/*union PackHead
	{
		uint8_t buff[5];*/

		//};
	struct DataHandleRecord
	{
		uint8_t head_buff[5];
		static const uint8_t Data_Head_Size = 5;
		PackHead pack_head;
		uint8_t head_rec_cnt = 0;
		std::vector<uint8_t> body_buff;
		uint32_t body_rec_cnt = 0;

		void calc_buf_2_pack_head()
		{
			//有对齐问题，所以直接解析
			pack_head.pack_id = head_buff[0];
			pack_head.pack_len =
				static_cast<uint32_t>(head_buff[1]) << 24 |
				static_cast<uint32_t>(head_buff[2]) << 16 |
				static_cast<uint32_t>(head_buff[3]) << 8 |
				static_cast<uint32_t>(head_buff[4]);
		}
		//接受完头后就调用
		void update_body_buff_size()
		{
			//只增不减
			if (body_buff.size() < pack_head.pack_len)
			{
				body_buff.resize(pack_head.pack_len);
			}
		}
		void reset()
		{
			head_rec_cnt = 0;
			body_rec_cnt = 0;
			//body_data.resize(0);// = 0;
		}
		void* get_head_write_pos()
		{
			return static_cast<void*>((uint8_t*)(&pack_head) + head_rec_cnt);
		}
		void write_data_2_body(const void* src, uint32_t len)
		{
			memcpy(body_buff.data() + body_rec_cnt, src, len);
			body_rec_cnt += len;
		}
	};


	class NetworkManager :
		std::enable_shared_from_this<NetworkManager>,
		public IVF_Obj
	{
		bool running = false;
		TQueue<PackContainer> msg_send_queue;
		//SafeQueue<PackContainer> msg_send_queue;
		int total = 0;

		hv::TcpClient cli;
		std::string buf;
		//uv_stream_t* handle;
		//uv_tcp_t* tcp_handle;
		std::thread t3;
		static GameContext* uv_get_context(void* data)
		{
			return ((ContextId*)(data))->get();
		}
		//FSocket* socket = nullptr;
		//brynet::net::TcpConnection::Ptr connection;
		//std::unique_ptr<hv::TcpClient> tcp_client;
		//hv::SocketChannelPtr channel;

	//// IVF_Obj interface ///////////////////////
	public:
		//初始化：override时需要调用父类
		virtual void IVF_Obj_init(ContextId id) override;
		virtual void IVF_Obj_begin() override;
		virtual void IVF_Obj_end() override;
		//////////////////////////////////////////

	public:
		//std::unique_ptr<RecvThread> recvThread;
		void send(PackContainer&& msg);
		void connectServer(
			std::shared_ptr<NetworkManager> netManager,
			std::string ip, int port);
		void close();
		~NetworkManager();
		DataHandleRecord data_handle_record;
		bool handle_data(const char* received_data, int byte_cnt);
	};
}
