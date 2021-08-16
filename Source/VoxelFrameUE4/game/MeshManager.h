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

		std::weak_ptr<void> bindedTo;
		void init()
		{
			memset(&bindedTo, 0, sizeof(bindedTo));
		}
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

		int createMeshAndGetId(std::weak_ptr<void> bindedTo, const FName& tag, const TArray<FVector>& Vertices, const TArray<int32>& Triangles);
		void updateMeshWithId(int id, const TArray<FVector>& Vertices, const TArray<int32>& Triangles);
		void delMeshWithId(int id);

		void init(GameContext* context1)
		{
			context = context1;
		}
	};
}
