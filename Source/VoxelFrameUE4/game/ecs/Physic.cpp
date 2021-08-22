#include "Physic.h"
#include "../GameContext.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

namespace VF
{
	namespace _Ecs
	{
		namespace _Sys
		{
			namespace _Physic
			{
				void checkPlayerRay2Chunk(AVoxelFrameUE4Character*& player)
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
					auto world = GameContext::getContext()->worldActor->GetWorld();
					world->LineTraceSingleByChannel
					(
						hitResult,
						start, end,
						ECollisionChannel::ECC_Visibility,
						collisonQueryParams
					);
					auto& previewer = GameContext::getContext()->blockPreviewManager->blockPutPreviewer;

					if (hitResult.IsValidBlockingHit() &&
						hitResult.Component->ComponentHasTag(VF_Tag_ChunkMesh)
						)
					{

						auto& pUE = hitResult.Location;
						auto pVF = pUE / VF_WorldScale;
						auto gridSide = _Base::checkPointUEOnBlockGridSide(pUE);

						FIntVector putBlockPos;
						FIntVector targetBlockPos;

						{
							switch (gridSide)
							{
								//压缩代码
		///////////////////////////////////////////////////////////////////////////////////						
#define sys_checkPlayerRay2Chunk_switch1(X,Y,Z)\
	case _Base::BlockGridSide::X:\
		if (dir.X < 0)\
		{\
			targetBlockPos.X=round(pVF.X)-1;\
			putBlockPos.X = targetBlockPos.X+1;\
		}\
		else\
		{\
			targetBlockPos.X=round(pVF.X);\
			putBlockPos.X = targetBlockPos.X-1;\
		}\
		targetBlockPos.Y =putBlockPos.Y= floor(pVF.Y);\
		targetBlockPos.Z= putBlockPos.Z= floor(pVF.Z);\
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

						auto putBlockChunkPosInfo = PositionInfoInChunk::fromVfPoint(putBlockPos);
						auto targetBlockChunkPosInfo = PositionInfoInChunk::fromVfPoint(targetBlockPos);

						auto chunkOfPutBlock = GameContext::getContext()->chunkManager->getChunkOfKey(putBlockChunkPosInfo.chunkKey);
						auto chunkOfTargetBlock = GameContext::getContext()->chunkManager->getChunkOfKey(targetBlockChunkPosInfo.chunkKey);



						GameContext::getContext()->blockPreviewManager->setTargetBlockPosition(
							targetBlockChunkPosInfo.p, chunkOfTargetBlock);
						/*UE_LOG(LogTemp, Warning, TEXT("block x y z %d %d %d %d"),
							positionInChunk.p.X,
							positionInChunk.p.Y,
							positionInChunk.p.Z, blockId);*/


						auto putBlockId = chunkOfPutBlock.lock()->readData(
							putBlockChunkPosInfo.p.X,
							putBlockChunkPosInfo.p.Y,
							putBlockChunkPosInfo.p.Z);
						if (//true
							GameContext::getContext()->blockManager->getInfo(putBlockId).isEmptyBlock()
							//blockId
							)
						{
							//auto p1 = FVector(
							//	relatePos.X + VF_CommonBlockSize / 2,
							//	relatePos.Y + VF_CommonBlockSize / 2,
							//	relatePos.Z + VF_CommonBlockSize / 2);
							////previewer->SetActorHiddenInGame(false);
							//previewer->SetActorLocation(p1);
							GameContext::getContext()->blockPreviewManager->setPutBlockPosition(putBlockChunkPosInfo.p, chunkOfPutBlock);

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


						//UE_LOG(LogTemp, Warning, TEXT("label %s"), *hitResult.Actor->GetActorLabel());
					}
					else
					{
						GameContext::getContext()->blockPreviewManager->targeting = false;
					hide:
						//UE_LOG(LogTemp, Warning, TEXT("no collision"));
						previewer->SetActorHiddenInGame(true);
					}
				}
			}
		}
	}

}
