#include "Physic.h"
#include "GameContext.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

namespace VF
{
	namespace _Physic
	{
		void sys_checkPlayerRay2Chunk(AVoxelFrameUE4Character* player)
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

				auto& p = hitResult.Location;
				auto gridSide = _Base::checkPointOnBlockGridSide(p);
				FVector relatePos;

				{
					switch (gridSide)
					{
						//压缩代码
///////////////////////////////////////////////////////////////////////////////////						
#define sys_checkPlayerRay2Chunk_switch1(X,Y,Z)\
	case _Base::BlockGridSide::X:\
		if (dir.X < 0)\
		{\
			relatePos.X = round(p.X/VF_CommonBlockSize)*VF_CommonBlockSize;\
		}\
		else\
		{\
			relatePos.X = round(p.X/VF_CommonBlockSize)*VF_CommonBlockSize - VF_CommonBlockSize;\
		}\
		relatePos.Y = floor(p.Y/VF_CommonBlockSize)*VF_CommonBlockSize;\
		relatePos.Z = floor(p.Z/VF_CommonBlockSize)*VF_CommonBlockSize;\
		break
///////////////////////////////////////////////////////////////////////////////////
						sys_checkPlayerRay2Chunk_switch1(X, Y, Z);
						sys_checkPlayerRay2Chunk_switch1(Y, X, Z);
						sys_checkPlayerRay2Chunk_switch1(Z, Y, X);
					default:
						break;
					}
				}
				auto blockWorldPosF = relatePos;
				_Base::conv_point_from_ue_2_vf(blockWorldPosF);
				Type::Vec3I blockWorldPos(blockWorldPosF.X, blockWorldPosF.Y, blockWorldPosF.Z);
				auto positionInChunk = PositionInfoInChunk::fromVfPoint(blockWorldPos);

				auto chunkMesh = Cast<UProceduralMeshComponentWithBind>(hitResult.Component);
				auto chunk = (Chunk*)chunkMesh->bindedTo;
				auto blockId = chunk->readData(
					positionInChunk.p.X,
					positionInChunk.p.Y,
					positionInChunk.p.Z);

				/*UE_LOG(LogTemp, Warning, TEXT("block x y z %d %d %d %d"),
					positionInChunk.p.X,
					positionInChunk.p.Y,
					positionInChunk.p.Z, blockId);*/
				if (//true
					GameContext::getContext()->blockManager->getInfo(blockId).isEmptyBlock()
					//blockId
					)
				{
					auto p1 = FVector(
						relatePos.X + VF_CommonBlockSize / 2,
						relatePos.Y + VF_CommonBlockSize / 2,
						relatePos.Z + VF_CommonBlockSize / 2);
					//previewer->SetActorHiddenInGame(false);
					previewer->SetActorLocation(p1);
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
			hide:
				//UE_LOG(LogTemp, Warning, TEXT("no collision"));
				previewer->SetActorHiddenInGame(true);
			}
		}
	}
}
