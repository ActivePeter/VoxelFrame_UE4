#pragma once
#include "ProtoPacks/common.pb.h"
class GameContext;
namespace VF{
void pack_recv_handle(std::shared_ptr<ChunkPack> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<PlayerBasic> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<ChunkEntityPack> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<MainPlayerMoveCmd> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<Cmd_SpawnEntityInPs> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<EntityPosUpdate> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<PutBlock> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<Cmd_PutBlockInPs> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<ClientOperationFailed> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<ClientOperationSucc> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<MainPlayerJumpCmd> pack, GameContext& context);
void pack_recv_handle(std::shared_ptr<RemoveEntity> pack, GameContext& context);

#define MACRO_pack_recv_distribute case EChunkPack:\
                pack_recv_handle(std::static_pointer_cast<ChunkPack>(this->pack_container.proto_pack),context);\
                break;\
case EPlayerBasic:\
                pack_recv_handle(std::static_pointer_cast<PlayerBasic>(this->pack_container.proto_pack),context);\
                break;\
case EChunkEntityPack:\
                pack_recv_handle(std::static_pointer_cast<ChunkEntityPack>(this->pack_container.proto_pack),context);\
                break;\
case EMainPlayerMoveCmd:\
                pack_recv_handle(std::static_pointer_cast<MainPlayerMoveCmd>(this->pack_container.proto_pack),context);\
                break;\
case ECmd_SpawnEntityInPs:\
                pack_recv_handle(std::static_pointer_cast<Cmd_SpawnEntityInPs>(this->pack_container.proto_pack),context);\
                break;\
case EEntityPosUpdate:\
                pack_recv_handle(std::static_pointer_cast<EntityPosUpdate>(this->pack_container.proto_pack),context);\
                break;\
case EPutBlock:\
                pack_recv_handle(std::static_pointer_cast<PutBlock>(this->pack_container.proto_pack),context);\
                break;\
case ECmd_PutBlockInPs:\
                pack_recv_handle(std::static_pointer_cast<Cmd_PutBlockInPs>(this->pack_container.proto_pack),context);\
                break;\
case EClientOperationFailed:\
                pack_recv_handle(std::static_pointer_cast<ClientOperationFailed>(this->pack_container.proto_pack),context);\
                break;\
case EClientOperationSucc:\
                pack_recv_handle(std::static_pointer_cast<ClientOperationSucc>(this->pack_container.proto_pack),context);\
                break;\
case EMainPlayerJumpCmd:\
                pack_recv_handle(std::static_pointer_cast<MainPlayerJumpCmd>(this->pack_container.proto_pack),context);\
                break;\
case ERemoveEntity:\
                pack_recv_handle(std::static_pointer_cast<RemoveEntity>(this->pack_container.proto_pack),context);\
                break;
}