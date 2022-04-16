#pragma once


#include "VF_Base.h"
#include "ProtoPacks/common.pb.h"

namespace VF
{

	enum PackIds
	{
		EChunkPack = 0,
		EPlayerBasic = 1,
		EChunkEntityPack = 2,
		EClientFirstConfirm = 3,
		EEntityPos = 4,
		EMainPlayerMoveCmd = 5,
		ECmd_SpawnEntityInPs = 6,
		ERpl_SpawnEntityInPs = 7,
		EEntityPosUpdate = 8,
		EPutBlock = 9,
		ECmd_PutBlockInPs = 10,
		ERpl_PutBlockInPs = 11,
		EClientOperationFailed = 12,
		EClientOperationSucc = 13,
		EMainPlayerJumpCmd = 14,
	};

	class PackContainer
	{
	public:
		std::shared_ptr<google::protobuf::Message> proto_pack;
		PackIds pack_id;

		PackContainer()
		{
			proto_pack = nullptr;
			pack_id = PackIds::EChunkPack;
		}
		PackContainer(
			PackIds pack_id1,
			std::shared_ptr<google::protobuf::Message>pack_msg)
		{
			proto_pack = pack_msg;
			pack_id = pack_id1;
		}

	};
	struct PackHead
	{
		uint8_t pack_id;
		uint32_t pack_len;
		PackHead() {}
		PackHead(uint8_t id, uint8_t len) :pack_id(id), pack_len(len) {}
	};

#define PackHead_Len (5)


#define _func_name_PackContainer_make__(__packname__)\
	PackContainer PackContainer_make(std::shared_ptr<__packname__> p)

	_func_name_PackContainer_make__(PlayerBasic);
	_func_name_PackContainer_make__(ChunkPack);
	_func_name_PackContainer_make__(ChunkEntityPack);
	_func_name_PackContainer_make__(ClientFirstConfirm);
	_func_name_PackContainer_make__(EntityPos);

	namespace _vf_pack
	{
		std::string conv_pack_to_bytes(PackContainer& pack);


		bool write_pack_head_to_bytes(const PackHead&& head, std::string& bytes);
	}

	namespace _HandlePack
	{
		void handle_pack_buff(ContextId cid, uint8_t pack_id, std::vector<uint8_t>& pack_buff, uint32_t pack_len);
	}
}