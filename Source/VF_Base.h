#pragma once
#define VF_ChunkLoadRadius (2)
#define VF_ChunkWidth (16)
#define VF_ChunkSize (VF_ChunkWidth*VF_ChunkWidth*VF_ChunkWidth)
#define VF_WorldScale (100)
#include "memory"
#include "vector"
#include "CoreMinimal.h"

namespace VF
{
	namespace Type
	{
		template<typename T>
		using Array = TArray<T>;

		using Vec3F = FVector;
		using Vec3I = FIntVector;
		//#define 
	}
}