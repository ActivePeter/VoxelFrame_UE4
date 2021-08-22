#include "./NetworkManager.h"
namespace VF
{

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
			VF_LogWarning("server connected");
			AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
				[this]()
				{
					TArray<uint8> receiveData;
					receiveData.SetNum(1024);
					while (1)
					{
						int32 byte_cnt = 0;
						auto succ = socket->Recv(
							receiveData.GetData(), receiveData.Num(), byte_cnt//,ESocketReceiveFlags::
						);
						if (succ)
						{
							this->handle_data(receiveData, byte_cnt);
						}
						else
						{
							VF_LogWarning("socket disconnected.");
							break;
						}
					}

					//while()
				}
			);
		}
	}


	NetworkManager::~NetworkManager()
	{
		if (socket && socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
		{
			socket->Close();
		}
	}

	bool NetworkManager::handle_data(TArray<uint8>& received_data, int _byte_cnt)
	{
		uint32_t handled_offset = 0;
		while (handled_offset != _byte_cnt)
		{
			auto byte_cnt_left = _byte_cnt - handled_offset;
			//头上一次未接收全
			if (data_handle_record.head_rec_cnt < DataHandleRecord::Data_Head_Size)
			{
				//头本次还是未收全
				if (byte_cnt_left + data_handle_record.head_rec_cnt < DataHandleRecord::Data_Head_Size)
				{
					memcpy(data_handle_record.get_head_write_pos(),
						received_data.GetData(),
						//接收大小
						byte_cnt_left);
					data_handle_record.head_rec_cnt += byte_cnt_left;

					return true;
					//return false;
				}
				//头本次收全
				else
				{
					handled_offset += DataHandleRecord::Data_Head_Size - data_handle_record.head_rec_cnt;

					memcpy(data_handle_record.get_head_write_pos(),
						received_data.GetData(),
						//剩余大小
						DataHandleRecord::Data_Head_Size - data_handle_record.head_rec_cnt);
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
				//memcpy(data_handle_record,)
				data_handle_record.write_data_2_body(received_data.GetData(), byte_cnt_left);
				handled_offset += byte_cnt_left;
			}
			else
				//完成读包
			{
				auto len = data_handle_record.pack_head.pack_len -
					data_handle_record.body_rec_cnt;
				data_handle_record.write_data_2_body(received_data.GetData(), len);
				handled_offset += len;

				//处理包
				// ...

				//重置状态
				data_handle_record.reset();
			}
		}
		//根据
		return true;
	}
}
