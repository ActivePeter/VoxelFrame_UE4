#include "conv.h"
#include "VF_Base.h"

namespace VF
{
	namespace _conv
	{
		ChunkKey point_2_chunkkey(int x, int y, int z)
		{
			auto ck = ChunkKey(0, 0, 0);
			// println!("plen  {0}", p.len());
			ck.keyData.X = x / VF_ChunkWidth;
			ck.keyData.Y = y / VF_ChunkWidth;
			ck.keyData.Z = z / VF_ChunkWidth;

			if (x < 0) {
				ck.keyData.X -= 1;
			}
			if (y < 0) {
				ck.keyData.Y--;// = ck.y() - 1;
			}
			if (z < 0) {
				ck.keyData.Z--;//= ck.z() - 1;
			}
			return ck;
		}

		VFVec3F origin_point_f_of_chunk(const ChunkKey& ck)
		{
			return VFVec3F(ck.x() * VF_ChunkWidth, ck.y() * VF_ChunkWidth, ck.z() * VF_ChunkWidth);
		}

		VFVec3I origin_point_i_of_chunk(const ChunkKey& ck)
		{
			return VFVec3I(ck.x() * VF_ChunkWidth, ck.y() * VF_ChunkWidth, ck.z() * VF_ChunkWidth);
		}

		VFVec3F ue_point_of_vf_point(const VFVec3F& vfp)
		{
			return VFVec3F(vfp.X * VF_WorldScale, vfp.Z * VF_WorldScale, vfp.Y * VF_WorldScale);
		}

		VFVec3I ue_point_of_vf_point(const VFVec3I& vfp)
		{
			return VFVec3I(vfp.X * VF_WorldScale, vfp.Z * VF_WorldScale, vfp.Y * VF_WorldScale);
		}
	}
}