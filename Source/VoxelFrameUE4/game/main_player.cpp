#include "main_player.h"
#include "GameContext.h"
#include "main_player_Character.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "net/vf_NetworkManager.h"

namespace VF
{
	namespace _main_player
	{
		void init(GameContext& context1, AVoxelFrameUE4Character* player)
		{
			if (context1.type == ClientType::Player)
			{
				auto& ecs = context1.ecs;
				NetSyncData net_sync_data;
				player->ecsId = ecs.scene->createEntity()
					.addComponent(static_cast<AVF_EntityBase*>(player))
					.addComponent(player)
					.addComponent(net_sync_data)
					//.addComponent(EntitySyncPosData(player->GetTransform().Rotator(),
					//	player->GetTransform().GetLocation()))
					//.addEmptyComponent<EntityTag_Common>()

					//.addEmptyComponent<GameEntityTag>()
					//.addEmptyComponent<ServerEntityId>()
					//.addEmptyComponent<VF::_Ecs::Moveable>()
					.entityId;
				context1.main_player_eid = player->ecsId;

				ecs.scene->addSys2Group(ecs.commonUpdateSys, _ecs_sys::main_player_update);
				ecs.scene->addSys2Group(ecs.game_end_sys, _ecs_sys::main_player_update);
				//ecs.scene->addSys2Group(ecs.commonUpdateSys, _ecs_sys::player_movement_sync);
			}
		}

		AllCompPtr get_mainplayer_comps_by_eid(paecs::Scene& ecs, paecs::EntityID& eid)
		{
			AllCompPtr all;

			if (ecs.randomAccessEntity(eid, all.character))
			{
				return all;
			}
			else
			{
				VF_LogWarning("failed to get main player");
			}
			return all;
		}

		void sync_from_server(GameContext& context, const EntityPos& ep)
		{
			NetSyncData* net_sync_data = nullptr;
			AVF_EntityBase** eptr = nullptr;
			auto server_pos = FVector(ep.x(), ep.y(), ep.z()) * VF_WorldScale;
			auto succ = context.ecs.scene->randomAccessEntity(context.main_player_eid, net_sync_data, eptr);
			VF_assert(succ);
			if (!succ)
			{
				return;
			}

			auto e = *eptr;
			//暂时直接同步坐标试试
			//VF_LogWarning("synced from server %f %f %f", ep.x(), ep.y(), ep.z());
			e->SetActorLocation(FVector(ep.x(), ep.y(), ep.z()) * VF_WorldScale);
			e->GetRootComponent()->ComponentVelocity = VFVec3F(ep.vx(), ep.vy(), ep.vz()) * VF_WorldScale;
			//auto curms = FDateTime::Now().GetTimeOfDay().GetTotalMilliseconds();
			//if ((curms - net_sync_data->last_sync_time) > 200)
			{

				////计算距离，
				//auto distance = FVector::Dist(server_pos, cm->movement_vec);
				//cm->last_sync_time = curms;
				//if (cm->history->empty())
				//{//历史坐标为空，直接更新坐标
				//	VF_LogWarning("history empty,directly set");
				//	auto delta = server_pos - (*eptr)->GetActorLocation();
				//	(*eptr)->AddMovementInput(delta, delta.Size());
				//	return;
				//}
				//auto begin = cm->history->begin();
				//std::list<PlayerTrans>::iterator closest = begin;
				//float closest_distance = (begin->pos - server_pos).Size();
				//++begin;
				//auto cnt = 0;
				//while (begin != cm->history->end())
				//{
				//	auto d = (begin->pos - server_pos).Size();
				//	if (d < closest_distance)
				//	{
				//		closest_distance = d;
				//		closest = begin;
				//	}
				//	cnt++;
				//	++begin;
				//}
				//VF_LogWarning("correct by %d record", cnt);
				////找到跟服务器发来的数据最近的之前的记录点，将偏移作为最新坐标的修正。
				//auto delta = server_pos - closest->pos;
				//(*eptr)->AddMovementInput(delta, delta.Size());
				////移除此点之前的所有记录值
				//cm->history->clear();
			}

		}
		namespace _ecs_sys
		{
			void main_player_update(ContextId& cid, AVoxelFrameUE4Character*& player, NetSyncData& net_sync_data)
			{
				using namespace _main_player_update;

				player_movement_sync(cid, player, net_sync_data);
				checkPlayerRay2Chunk(cid, player);
			}

		}
		namespace _main_player_update {

			void player_movement_sync(ContextId& cid, AVoxelFrameUE4Character*& player, NetSyncData& _net_sync_data)
			{
				NetSyncData net_sync_data;
				std::swap(net_sync_data, _net_sync_data);//将空数据换入

				//如果有移动或旋转操作
				if (!_util::is_rough_zero(net_sync_data.move_forward) ||
					!_util::is_rough_zero(net_sync_data.move_right) ||
					net_sync_data.rotated)
				{
					auto mpmc = std::make_shared<MainPlayerMoveCmd>();
					mpmc->set_rotate_yaw(player->GetTransform().Rotator().Yaw);
					mpmc->set_move_forward(net_sync_data.move_forward);
					mpmc->set_move_right(net_sync_data.move_right);
					cid.get()->networkManager->send(
						PackContainer(PackIds::EMainPlayerMoveCmd, mpmc));

					////record trans
					//movement.history->push_front(PlayerTrans(
					//	ptrans.GetLocation(),
					//	ptrans.Rotator()
					//));
					////VF_LogWarning("send player move");
				}
				//如果有跳跃操作
				if (net_sync_data.jump_cmd)
				{
					auto jump_cmd = std::make_shared<MainPlayerJumpCmd>();
					jump_cmd->set_start_or_end(net_sync_data.jump_cmd);
					cid.get()->networkManager->send(
						PackContainer(PackIds::EMainPlayerJumpCmd, jump_cmd)
					);

				}
			}
			void checkPlayerRay2Chunk(ContextId& cid, AVoxelFrameUE4Character*& player)
			{
				FVector start, end;
				//{
				start = player->GetFirstPersonCameraComponent()->GetComponentTransform().GetLocation();
				auto dir = player->GetFirstPersonCameraComponent()->GetComponentTransform().GetRotation().Vector();
				auto range = 1000;
				end = start + dir * 1000;
				//}

				FCollisionQueryParams collisonQueryParams(TEXT("QueryParams"), true, player);
				collisonQueryParams.bTraceComplex = true;
				collisonQueryParams.bReturnPhysicalMaterial = false;

				FHitResult hitResult;
				auto world = cid.get()->worldActor->GetWorld();
				world->LineTraceSingleByChannel(
					hitResult,
					start, end,
					ECollisionChannel::ECC_Visibility,
					collisonQueryParams);
				auto& previewer = cid.get()->block_preview_manager->blockPutPreviewer;

				if (hitResult.IsValidBlockingHit() &&
					hitResult.Component->ComponentHasTag(VF_Tag_ChunkMesh))
				{

					auto& pUE = hitResult.Location;
					auto pVF = pUE / VF_WorldScale;
					auto gridSide = _base::checkPointUEOnBlockGridSide(pUE);

					FIntVector putBlockPos;
					FIntVector targetBlockPos;

					{
						switch (gridSide)
						{
							//压缩代码
							///////////////////////////////////////////////////////////////////////////////////
#define sys_checkPlayerRay2Chunk_switch1(X, Y, Z)        \
    case _base::BlockGridSide::X:                        \
        if (dir.X < 0)                                   \
        {                                                \
            targetBlockPos.X = round(pVF.X) - 1;         \
            putBlockPos.X = targetBlockPos.X + 1;        \
        }                                                \
        else                                             \
        {                                                \
            targetBlockPos.X = round(pVF.X);             \
            putBlockPos.X = targetBlockPos.X - 1;        \
        }                                                \
        targetBlockPos.Y = putBlockPos.Y = floor(pVF.Y); \
        targetBlockPos.Z = putBlockPos.Z = floor(pVF.Z); \
        break
				///////////////////////////////////////////////////////////////////////////////////
							sys_checkPlayerRay2Chunk_switch1(X, Y, Z);
							sys_checkPlayerRay2Chunk_switch1(Y, X, Z);
							sys_checkPlayerRay2Chunk_switch1(Z, Y, X);
						default:
							break;
						}
					}
					//以上处理完还是yz没交换的ue版
					Swap(pVF.Y, pVF.Z);
					Swap(targetBlockPos.Y, targetBlockPos.Z);
					Swap(putBlockPos.Y, putBlockPos.Z);

					auto putBlockChunkPosInfo = PositionInfoInChunk::OfIPos::from_vf_pos(putBlockPos);
					auto targetBlockChunkPosInfo = PositionInfoInChunk::OfIPos::from_vf_pos(targetBlockPos);

					auto chunkOfPutBlock = cid.get()->chunkManager->getChunkOfKey_not_sure(putBlockChunkPosInfo.chunkKey);
					auto chunkOfTargetBlock = cid.get()->chunkManager->getChunkOfKey_not_sure(targetBlockChunkPosInfo.chunkKey);
					//VF_assert(chunkOfPutBlock);
					//VF_assert(chunkOfTargetBlock);
					if (chunkOfPutBlock && chunkOfTargetBlock)
					{
						cid.get()->block_preview_manager->setTargetBlockPosition(
							targetBlockChunkPosInfo.pos, chunkOfTargetBlock);
						/*UE_LOG(LogTemp, Warning, TEXT("block x y z %d %d %d %d"),
											positionInChunk.p.X,
											positionInChunk.p.Y,
											positionInChunk.p.Z, blockId);*/

						auto putBlockId = chunkOfPutBlock->readData(
							putBlockChunkPosInfo.pos.X,
							putBlockChunkPosInfo.pos.Y,
							putBlockChunkPosInfo.pos.Z);
						if ( //true
							cid.get()->blockManager->getInfo(putBlockId).isEmptyBlock()
							//blockId
							)
						{
							//auto p1 = FVector(
							//	relatePos.X + VF_CommonBlockSize / 2,
							//	relatePos.Y + VF_CommonBlockSize / 2,
							//	relatePos.Z + VF_CommonBlockSize / 2);
							////previewer->SetActorHiddenInGame(false);
							//previewer->SetActorLocation(p1);
							cid.get()->block_preview_manager->setPutBlockPosition(
								putBlockChunkPosInfo.pos, chunkOfPutBlock);

							previewer->UpdateOverlaps();
							TArray<UPrimitiveComponent*> comps;
							previewer->GetOverlappingComponents(comps);
							if (comps.Num() == 0)
								//||
								//(overlaps.Num() == 1 && overlaps[0].OverlapInfo.GetComponent()->ComponentTags.Contains(VF_Tag_ChunkMesh)))
							{

								//UE_LOG(LogTemp, Warning, TEXT("no overlap"));
								previewer->SetActorHiddenInGame(false);
							}
							else
							{

								//UE_LOG(LogTemp, Warning, TEXT("overlap"));
								goto hide;
							}
						}
						else
						{
							goto hide;
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT(
							"target block chunk not found\n"
							"  %d %d %d"
						), putBlockChunkPosInfo.chunkKey.x(), putBlockChunkPosInfo.chunkKey.y(), putBlockChunkPosInfo.chunkKey.z());
						goto hide;
					}
					//UE_LOG(LogTemp, Warning, TEXT("label %s"), *hitResult.Actor->GetActorLabel());
				}
				else
				{
					cid.get()->block_preview_manager->targeting = false;
				hide:
					//UE_LOG(LogTemp, Warning, TEXT("no collision"));
					previewer->SetActorHiddenInGame(true);
				}
			}
		}
	}
}

