#pragma once

#include "VF_Base.h"
#include "ProceduralMeshComponent.h"
#include "unordered_set"

#include "GameContext.h"

namespace VF
{
	struct MeshManager {
		GameContext* context;

		UProceduralMeshComponent* customMesh;

		/*销毁后可再利用的网格插件*/
		std::list<int> recycledSectionIds;

		/*网格组件池*/
		std::vector<UProceduralMeshComponent*> meshCompPool;

		int nextSectionId = 0;

		int createMeshAndGetId(const TArray<FVector>& Vertices, const TArray<int32>& Triangles);
		void updateMeshWithId(int id, const TArray<FVector>& Vertices, const TArray<int32>& Triangles);
		void delMeshWithId(int id);

		void init(GameContext* context1)
		{
			context = context1;
		}
	};
}
