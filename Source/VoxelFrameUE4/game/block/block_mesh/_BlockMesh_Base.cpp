#include "_BlockMesh_Base.h"
#include "game/conv.h"
namespace VF
{
	namespace _Block
	{

		void BlockMesh_Base::conv_local_vertex_2_ue_world_pos(
			PositionInfoInChunk::OfIPos& block_p, VFVec3F& vertex)
		{
			auto block_world_pos = block_p.pos + _conv::origin_point_i_of_chunk(block_p.chunkKey);
			vertex.X += block_world_pos.X;
			vertex.Y += block_world_pos.Y;
			vertex.Z += block_world_pos.Z;

			std::swap(vertex.Y, vertex.Z);
			vertex *= VF_WorldScale;


			//VF_LogWarning("vertex %f %f %f",
			//	vertex.X, vertex.Y, vertex.Z
			//);
		}

	}
}