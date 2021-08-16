#pragma once
#define VF_ChunkLoadRadius (4)
#define VF_ChunkWidth (32)
#define VF_ChunkSize (VF_ChunkWidth * VF_ChunkWidth * VF_ChunkWidth)
#define VF_WorldScale (100)
#define VF_CommonBlockSize (VF_WorldScale)
#define VF_Tag_ChunkMesh (FName("chunk_mesh"))

#define VF_RoughEpsilon (0.0001)

#define VF_PhysicChannel_ChunkMesh ECollisionChannel::ECC_GameTraceChannel2
#include "memory"
#include "vector"
#include "list"
#include "CoreMinimal.h"
#include <cassert>
namespace VF
{
	namespace Type
	{
		template <typename T>
		using Array = TArray<T>;

		using Vec3F = FVector;
		using Vec3I = FIntVector;

		//#define
	}

	//基础函数 
	namespace _Base
	{
		void conv_point_from_ue_2_vf(Type::Vec3F& p);
		void conv_point_from_vf_2_ue(Type::Vec3F& p);

		//检测点是否在方块边界上
		enum class BlockGridSide
		{
			X,
			Y,
			Z,
			No
		};
		BlockGridSide checkPointOnBlockGridSide(const Type::Vec3F& p);
	}

}