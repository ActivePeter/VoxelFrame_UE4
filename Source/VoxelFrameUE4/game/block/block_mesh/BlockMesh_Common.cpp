#include "BlockMesh_Common.h"
namespace VF
{
	namespace _Block
	{
		// 不同种类的网格信息，表达在不同文件里，比较清晰
		void BlockMesh_Common::setFaceVertexPosOnDir(Type::Array<Type::Vec3F>& vertexPoses, _Block::FaceDirection dir)
		{
			// 8个点 对应8个索引
			switch (dir)
			{
			case _Block::FaceDirection::X_Positive:
				// 1 4 7 6
				setFaceVertices(vertexPoses, 1, 4, 7, 6);
				break;
			case _Block::FaceDirection::X_Negative:
				// 3 5 2 0
				setFaceVertices(vertexPoses, 3, 5, 2, 0);
				break;
			case _Block::FaceDirection::Y_Positive:
				// 2 5 7 4
				setFaceVertices(vertexPoses, 2, 5, 7, 4);
				break;
			case _Block::FaceDirection::Y_Negative:
				// 1 6 3 0
				setFaceVertices(vertexPoses, 1, 6, 3, 0);
				break;
			case _Block::FaceDirection::Z_Positive:
				// 6 7 5 3
				setFaceVertices(vertexPoses, 6, 7, 5, 3);
				break;
			case _Block::FaceDirection::Z_Negative:
				// 0 2 4 1
				setFaceVertices(vertexPoses, 0, 2, 4, 1);
				break;

			default:
				//std::cout << "no face enum matched" << std::endl;
				break;
			}
		}

		void BlockMesh_Common::addUpStandardVertexOfIndex(Type::Vec3F& vertexPos, uint8_t index)
		{
			switch (index)
			{
				// case 0: nothing need to do
				//     vertex.addPosition(0,0,0);
				//     break;
			case 1:
				vertexPos += Type::Vec3F(1, 0, 0);
				//vertex.addPosition(1, 0, 0);
				break;
			case 2:
				//vertex.addPosition(
				vertexPos += Type::Vec3F(0, 1, 0);

				break;
			case 3:
				//vertex.addPosition(
				vertexPos += Type::Vec3F(0, 0, 1);
				break;
			case 4:
				//vertex.addPosition(
				vertexPos += Type::Vec3F(1, 1, 0);
				break;
			case 5:
				//vertex.addPosition(
				vertexPos += Type::Vec3F(0, 1, 1);
				break;
			case 6:
				//vertex.addPosition(
				vertexPos += Type::Vec3F(1, 0, 1);
				break;
			case 7:
				//vertex.addPosition(
				vertexPos += Type::Vec3F(1, 1, 1);
				break;
			}
		}
		void BlockMesh_Common::pushOneFaceVerticesAndIndices_selfPos(
			_Block::FaceDirection dir,
			Type::Array<Type::Vec3F>& vertexPoses,
			Type::Array<int32>& indices, unsigned int vertexIndexOffset)
		{
			setFaceVertexPosOnDir(vertexPoses, dir);
			//三角形点序号，对应刚添加的四个点
			indices.Add(vertexIndexOffset + (unsigned int)(vertexPoses.Num() - 4));
			indices.Add(vertexIndexOffset + (unsigned int)(vertexPoses.Num() - 3));
			indices.Add(vertexIndexOffset + (unsigned int)(vertexPoses.Num() - 2));
			indices.Add(vertexIndexOffset + (unsigned int)(vertexPoses.Num() - 4));
			indices.Add(vertexIndexOffset + (unsigned int)(vertexPoses.Num() - 2));
			indices.Add(vertexIndexOffset + (unsigned int)(vertexPoses.Num() - 1));
		}
		////这个应该可以拿到Base去
		//void BlockMesh_Common::getBlockValidTriangles(std::vector<Type::Vec3F>& vertexPoses, _Graph::Indices& indices)
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
