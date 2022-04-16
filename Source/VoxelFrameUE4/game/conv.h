#pragma once

#include "VF_Base.h"
#include "game/Chunk.h"

namespace VF
{
	namespace _conv
	{
		ChunkKey point_2_chunkkey(int x, int y, int z);

		VFVec3F origin_point_f_of_chunk(const ChunkKey& ck);
		VFVec3I origin_point_i_of_chunk(const ChunkKey& ck);

		VFVec3F ue_point_of_vf_point(const VFVec3F& vfp);
		VFVec3I ue_point_of_vf_point(const VFVec3I& vfp);
	}
}
