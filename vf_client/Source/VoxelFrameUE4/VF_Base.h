#pragma once
#define VF_ChunkLoadRadius (4)
#define VF_ChunkWidth (32)
#define VF_ChunkSize (VF_ChunkWidth * VF_ChunkWidth * VF_ChunkWidth)
#define VF_WorldScale (100)
#define VF_CommonBlockSize (VF_WorldScale)
#define VF_Tag_ChunkMesh (FName("chunk_mesh"))

#define VF_RoughEpsilon (0.0001)
#define VF_Sync_Threshold_Rotate (0.0087266462599)
#define VF_Sync_Threshold_Move (1)

#define VF_PhysicChannel_ChunkMesh ECollisionChannel::ECC_GameTraceChannel2
#include "memory"
#include "vector"
#include "list"
#include "queue"
#include "CoreMinimal.h"
#include <cassert>
#include <mutex>
#include "tl/optional.hpp"

#include "google/protobuf/message.h"
#include "stbipp/Image.hpp"

//#define VF_Namespace_Ecs namespace VF{namespace _Ecs{
#define VF_Namespace_Ecs_Sys namespace VF{namespace _Ecs{namespace _Sys{
#define VF_Namespace_Ecs_Entity namespace VF{namespace _Ecs{namespace _Entity{

#define VF_LogWarning(Str,...) UE_LOG(LogTemp, Warning, TEXT(Str),__VA_ARGS__)
#define VF_Log(Str,...) UE_LOG(LogTemp, Log, TEXT(Str),__VA_ARGS__)
#define VF_LogErr(Str,...) UE_LOG(LogTemp,ERROR , TEXT(Str),__VA_ARGS__)

//输入为false是输出error
#define VF_assert ensure

namespace VF
{
	class GameContext;
	//类型
	//using ContextId = uint8_t;

	struct ContextId
	{
		uint8_t id;
		GameContext* get();
	};
	namespace _type
	{
		template <typename T>
		using Array = TArray<T>;

		using Vec3F = FVector;
		using Vec3I = FIntVector;

		//#define
	}
	template <typename T>
	using VFArray = TArray<T>;
	using VFVec3F = FVector;
	using VFVec3I = FIntVector;
	inline void VFVec3F_vf2ue(VFVec3F& vfvec)
	{
		vfvec *= VF_WorldScale;
		std::swap(vfvec.Y, vfvec.Z);
	}
	inline void VFVec3F_ue2vf(VFVec3F& vfvec)
	{
		vfvec /= VF_WorldScale;
		std::swap(vfvec.Y, vfvec.Z);
	}

	template <typename T>
	struct VFRect
	{
		T x1;
		T y1;
		T w;
		T h;
		VFRect(float _x1, float _y1, float _w, float _h)
			:x1(_x1), y1(_y1), w(_w), h(_h) {}
		T get_x2()
		{
			return x1 + w;
		}
		T get_y2()
		{
			return y1 + h;
		}
	};
	namespace _VFRect
	{
		FVector2D get_top_right(const VFRect<float>& rect);
		FVector2D get_bottom_right(const VFRect<float>& rect);
		FVector2D get_top_left(const VFRect<float>& rect);
		FVector2D get_bottom_left(const VFRect<float>& rect);
	}
	//基础函数 
	namespace _base
	{
		void conv_point_from_ue_2_vf(_type::Vec3F& p);
		void conv_point_from_vf_2_ue(_type::Vec3F& p);

		//检测点是否在方块边界上
		enum class BlockGridSide
		{
			X,
			Y,
			Z,
			No
		};
		BlockGridSide checkPointUEOnBlockGridSide(const _type::Vec3F& p);
	}
	namespace _util {
		uint64_t timestamp();
		bool is_rough_zero(const FVector& vector);
		bool is_rough_zero(float vector);

		namespace _img_process
		{
			void set_bgra_one_pixel(std::vector<uint8_t>& data,
				int w, int h, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		}

		/*namespace _img {
			void copy_stbipp_image_2_utexture2d(
				const stbipp::Image& img, UTexture2D& target, int x, int y);
		}*/
	}
}

#include "game/IVF_Obj.h"