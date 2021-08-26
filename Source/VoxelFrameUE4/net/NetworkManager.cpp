#include "./NetworkManager.h"
#include "ProtoPacks/chunk.pb.h"
#include "../game/GameContext.h"

namespace VF
{
	//#pragma optimize( "", off )
	void NetworkManager::connectServer()
	{
		//socket
		socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->
			CreateSocket(NAME_Stream, TEXT("default"), false);
		//address
		FIPv4Address ip(127, 0, 0, 1);
		auto addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->
			CreateInternetAddr(); addr->SetIp(ip.Value); addr->SetPort(7776);

		socket->Connect(*addr);
		if (!socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
		{
			//UE_LOG(LogTemp, Warning, TEXT("server not connected"));
			VF_LogWarning("server not connected%d", 1);
		}
		else
		{
			VF_LogWarning("server connected1");


			AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
				[this]()
				{
					VF_LogWarning("receive thread started");
					running = true;
					//TArray<uint8> receiveData;
					//receiveData.SetNum(1024);
					std::vector<uint8_t> buff;
					buff.resize(1024);
					//uint8_t buff2[1024];
					//uint8_t* buff3 = (uint8_t*)malloc(1024 * sizeof(uint8_t));
					while (1)
					{
						//VF_LogWarning("receive thread loop");
						int32 byte_cnt = 0;
						auto succ = socket->Recv(
							buff.data(), buff.size(), byte_cnt//,ESocketReceiveFlags::
						);
						if (succ)
						{
							//VF_LogWarning("start handling %d datas", byte_cnt);
							this->handle_data(buff, byte_cnt);
							//VF_LogWarning("after handling %d datas", byte_cnt);
						}
						else
						{
							VF_LogWarning("socket disconnected.");
							break;
						}
					}
					VF_LogWarning("task end.");

					//while (1)
					//{

					//}
				}
			);


		}
	}
	//#pragma optimize( "", on )

	NetworkManager::~NetworkManager()
	{
		this->data_handle_record.body_buff.resize(0);
		VF_LogWarning("NetworkManager end");
		running = false;
		if (socket && socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
		{
			socket->Close();
		}

	}

	//#pragma optimize( "", off )
	bool NetworkManager::handle_data(std::vector<uint8_t>& received_data, int _byte_cnt)
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
				data_handle_record.write_data_2_body(received_data.data(), byte_cnt_left);
				handled_offset += byte_cnt_left;
				return true;
			}
			else
				//完成读包
			{

				auto len = data_handle_record.pack_head.pack_len -
					data_handle_record.body_rec_cnt;
				//VF_LogWarning("handle case 4 %d %d", len, data_handle_record.pack_head.pack_len);
				data_handle_record.write_data_2_body(received_data.data(), len);
				handled_offset += len;

				//处理包
				// ...
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
