#pragma once

#include "VF_Base.h"
#include "ProceduralMeshComponent.h"
#include "unordered_set"

#include "GameContext.h"

namespace VF
{
	class UProceduralMeshComponentWithBind :public UProceduralMeshComponent
	{
	public:
		void* bindedTo;
	};
	struct MeshManager {
		GameContext* context;

		UProceduralMeshComponentWithBind* customMesh;

		/*销毁后可再利用的网格插件*/
		std::list<int> recycledSectionIds;

		/*网格组件池*/
		std::vector<UProceduralMeshComponentWithBind*> meshCompPool;

		int nextSectionId = 0;

		UProceduralMeshComponentWithBind* getMeshById(int id);

		int createMeshAndGetId(void* bindedTo, const FName& tag, const TArray<FVector>& Vertices, const TArray<int32>& Triangles);
		void updateMeshWithId(void* bindedTo, int id, const TArray<FVector>& Vertices, const TArray<int32>& Triangles);
		void delMeshWithId(int id);

		void init(GameContext* context1)
		{
			context = context1;
		}
	};
}
