#include "vf_pack.h"

#include "../game/event/PackReceiveEvent.h"
#include "../game/GameContext.h"

#define switch_pack(__pack__) case E##__pack__:\
	{\
		auto cp = std::make_shared<__pack__>();\
		cp->ParseFromArray(pack_buff.data(), pack_len);\
		pass_to_game(cid,cp, E##__pack__);\
	}\
	break;
//	//Detail::handle_##__pack__(cp);

#define _func_PackContainer_make__(__packname__)\
	_func_name_PackContainer_make__(__packname__)\
	{return PackContainer(PackIds::E##__packname__, p);}

namespace VF
{
	_func_PackContainer_make__(PlayerBasic);
	_func_PackContainer_make__(ChunkPack);
	_func_PackContainer_make__(ChunkEntityPack);
	_func_PackContainer_make__(ClientFirstConfirm);
	_func_PackContainer_make__(EntityPos);
	namespace _vf_pack
	{
		std::string conv_pack_to_bytes(PackContainer& pack)
		{
			std::string msg_string;

			msg_string.resize(PackHead_Len);
			//msg->SerializeAsString();
			pack.proto_pack->AppendToString(&msg_string);

			_vf_pack::write_pack_head_to_bytes(
				PackHead(pack.pack_id, msg_string.size() - PackHead_Len),
				msg_string);

			return msg_string;
		}
		bool write_pack_head_to_bytes(const PackHead&& head, std::string& bytes)
		{
			if (bytes.size() < PackHead_Len)
			{
				return false;
			}
			bytes[0] = head.pack_id;
			bytes[1] = head.pack_len >> 24 & 0xff;
			bytes[2] = head.pack_len >> 16 & 0xff;
			bytes[3] = head.pack_len >> 8 & 0xff;
			bytes[4] = head.pack_len & 0xff;
			return true;
		}
	}
	namespace _HandlePack
	{
		PackContainer PackContainer_make(std::shared_ptr<PlayerBasic> p)
		{
			return PackContainer(PackIds::EPlayerBasic, p);
		}
		//namespace Detail
		//{
			//prot
			//void handle_ChunkPack(std::shared_ptr<ChunkPack> cp)
			//{
			//	////1.转成proto，
			//	//auto cp = std::make_shared<ChunkPack>();
			//	//cp->ParseFromArray(pack_buff.data(), pack_len);

			//	//2.传给event_list
			//	GameContext::getContext()->event_list.push(_Event::NewChunkLoadEvent::create(cp));
			//}
			/*void handle_PlayerBasic(std::shared_ptr<PlayerBasic> pbp)
			{
				GameContext::getContext()->event_list.push(_Event::PackRecieveEvent::create(pbp,PackIds::EPlayerBasic));
			}*/
			//void handle_ChunkEntityPack()
		//}
		inline void pass_to_game(ContextId cid, const std::shared_ptr<google::protobuf::Message>& pack, PackIds pack_id)
		{
			if(!cid.get())
			{
				return;
			}
			cid.get()->
				event_list->push(
					_Event::PackRecieveEvent::create(cid,
						PackContainer(pack_id, pack)));
		}
		void handle_pack_buff(ContextId cid, uint8_t pack_id, std::vector<uint8_t>& pack_buff, uint32_t pack_len)
		{
			//VF_LogWarning("handle_pack_buff %d",pack_id);
			switch (pack_id)
			{
				/*case EChunkPack:
					auto cp = std::make_shared<ChunkPack>();
					cp->ParseFromArray(pack_buff.data(), pack_len);
					pass_to_game(cp, EChunkPack);*/
				switch_pack(ChunkPack);
				switch_pack(ChunkEntityPack);
				switch_pack(RemoveEntity);

				//default:
					//VF_LogWarning("No matching pack id. %d", pack_id);
			}
			if (cid.get()&&cid.get()->type == ClientType::PartServer)
			{
				switch (pack_id)
				{
					switch_pack(MainPlayerMoveCmd);
					switch_pack(MainPlayerJumpCmd);
					switch_pack(Cmd_SpawnEntityInPs);
					switch_pack(Cmd_PutBlockInPs);
				}
			}
			else//player
			{
				switch (pack_id)
				{
					switch_pack(PlayerBasic);
					switch_pack(EntityPosUpdate);
					switch_pack(PutBlock);
					switch_pack(ClientOperationFailed);
					switch_pack(ClientOperationSucc);
				}
			}
		}
	}
}