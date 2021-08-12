#pragma once
#define VF_ChunkLoadRadius (2)
#define VF_ChunkWidth (16)
#define VF_ChunkSize (VF_ChunkWidth * VF_ChunkWidth * VF_ChunkWidth)
#define VF_WorldScale (100)
#include "memory"
#include "vector"
#include "list"
#include "CoreMinimal.h"

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

	//基础函数 衔接ue和vf之间的一些差异
	namespace _Base
	{
		void conv_point_from_ue_2_vf(Type::Vec3F &p);
		void conv_point_from_vf_2_ue(Type::Vec3F &p);
	}
}