#include "BlockMesh_Common.h"
namespace VF
{
	namespace _Block
	{
		//// 不同种类的网格信息，表达在不同文件里，比较清晰
		//void BlockMesh_Common::setFaceVertexPosOnDir(VFArray<VFVec3F>& vertexPoses, _Block::FaceDirection dir)
		//{
		//	
		//}



		void BlockMesh_Common::addUpStandardVertexOfIndex(PositionInfoInChunk::OfIPos& blockp, VFVec3F& vertexPos, uint8_t index)
		{
			switch (index)
			{
				// case 0: nothing need to do
				//     vertex.addPosition(0,0,0);
				//     break;
			case 1:
				vertexPos += VFVec3F(1, 0, 0);
				//vertex.addPosition(1, 0, 0);
				break;
			case 2:
				//vertex.addPosition(
				vertexPos += VFVec3F(0, 1, 0);

				break;
			case 3:
				//vertex.addPosition(
				vertexPos += VFVec3F(0, 0, 1);
				break;
			case 4:
				//vertex.addPosition(
				vertexPos += VFVec3F(1, 1, 0);
				break;
			case 5:
				//vertex.addPosition(
				vertexPos += VFVec3F(0, 1, 1);
				break;
			case 6:
				//vertex.addPosition(
				vertexPos += VFVec3F(1, 0, 1);
				break;
			case 7:
				//vertex.addPosition(
				vertexPos += VFVec3F(1, 1, 1);
				break;
			}
			BlockMesh_Base::conv_local_vertex_2_ue_world_pos(blockp, vertexPos);
		}
		void BlockMesh_Common::setFaceVertices(PositionInfoInChunk::OfIPos& blockp, VFArray<_type::Vec3F>& vertexPoses, uint8_t _0, uint8_t _1, uint8_t _2,
			uint8_t _3)
		{
			{
				_type::Vec3F model(0, 0, 0);
				vertexPoses.Add(model);
				vertexPoses.Add(model);
				vertexPoses.Add(model);
				vertexPoses.Add(model);
				addUpStandardVertexOfIndex(blockp, vertexPoses[vertexPoses.Num() - 4], _0);
				addUpStandardVertexOfIndex(blockp, vertexPoses[vertexPoses.Num() - 3], _1);
				addUpStandardVertexOfIndex(blockp, vertexPoses[vertexPoses.Num() - 2], _2);
				addUpStandardVertexOfIndex(blockp, vertexPoses[vertexPoses.Num() - 1], _3);

				/*static int i = 0;
				i++;
				if (i <= 10)
				{
					VF_LogWarning(
						"%f %f, %f %f, %f %f, %f %f\n"
						,
						vertexPoses[vertexPoses.Num() - 4].X, vertexPoses[vertexPoses.Num() - 4].Y,
						vertexPoses[vertexPoses.Num() - 3].X, vertexPoses[vertexPoses.Num() - 3].Y,
						vertexPoses[vertexPoses.Num() - 2].X, vertexPoses[vertexPoses.Num() - 2].Y,
						vertexPoses[vertexPoses.Num() - 1].X, vertexPoses[vertexPoses.Num() - 1].Y
					);
				}*/
			}
		}
		void BlockMesh_Common::pushOneFaceVerticesAndIndices_selfPos(
			_Block::FaceDirection dir,
			MeshConstructData& mesh_construct, PositionInfoInChunk::OfIPos& block_p,
			BlockUVSetter_Base& uv_setter, TextureManager& texture_man)
		{
			// 8个点 对应8个索引
			switch (dir)
			{
			case _Block::FaceDirection::X_Positive:
				// 1 4 7 6
				setFaceVertices(block_p, mesh_construct.vertices, 1, 4, 7, 6);
				break;
			case _Block::FaceDirection::X_Negative:
				// 3 5 2 0
				setFaceVertices(block_p, mesh_construct.vertices, 3, 5, 2, 0);
				break;
			case _Block::FaceDirection::Y_Positive:
				// 2 5 7 4
				setFaceVertices(block_p, mesh_construct.vertices, 2, 5, 7, 4);
				break;
			case _Block::FaceDirection::Y_Negative:
				// 1 6 3 0
				setFaceVertices(block_p, mesh_construct.vertices, 1, 6, 3, 0);
				break;
			case _Block::FaceDirection::Z_Positive:
				// 6 7 5 3
				setFaceVertices(block_p, mesh_construct.vertices, 6, 7, 5, 3);
				break;
			case _Block::FaceDirection::Z_Negative:
				// 0 2 4 1
				setFaceVertices(block_p, mesh_construct.vertices, 0, 2, 4, 1);
				break;

			default:
				//std::cout << "no face enum matched" << std::endl;
				break;
			}
			//auto vertexIndexOffset=mesh_construct.vertices.Num();
			//setFaceVertexPosOnDir(mesh_construct.vertices, dir);
			//三角形点序号，对应刚添加的四个点
			mesh_construct.triangles.Add((unsigned int)(mesh_construct.vertices.Num() - 4));
			mesh_construct.triangles.Add((unsigned int)(mesh_construct.vertices.Num() - 3));
			mesh_construct.triangles.Add((unsigned int)(mesh_construct.vertices.Num() - 2));
			mesh_construct.triangles.Add((unsigned int)(mesh_construct.vertices.Num() - 4));
			mesh_construct.triangles.Add((unsigned int)(mesh_construct.vertices.Num() - 2));
			mesh_construct.triangles.Add((unsigned int)(mesh_construct.vertices.Num() - 1));

			uv_setter.setVertexUVOnDir(texture_man, mesh_construct.uvs, dir);
		}
		////这个应该可以拿到Base去
		//void BlockMesh_Common::getBlockValidTriangles(std::vector<VFVec3F>& vertexPoses, _Graph::Indices& indices)
		//{
		//	for (int a = 0; a < (int)(_Block::FaceDirection::End); a++)
		//	{
		//		this->pushOneFaceVerticesAndIndices_selfPos((_Block::FaceDirection)a, vertexPoses, indices, 0);
		//	}
		//}
		//这个应该也可以拿到Base去


		//这个拿不了

	}
}
