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
		std::unordered_set<int> recycledSectionIds;
		int nextSectionId = 1;

		int createMesh_andGetId(const TArray<FVector>& Vertices, const TArray<int32>& Triangles);
		void updateMesh_withId(int id, const TArray<FVector>& Vertices, const TArray<int32>& Triangles);

		void init(GameContext* context1)
		{
			context = context1;
		}
	};
}
