#pragma once
#include "VF_Base.h"

#include "VoxelFrameUE4/VoxelFrameUE4Character.h"

namespace VF
{
	namespace _Ecs
	{
		namespace _Sys
		{
			namespace _Physic
			{
				//regist when player begin
				void checkPlayerRay2Chunk(AVoxelFrameUE4Character*& player);
			}
		}
	}
}