#include "MeshManager.h"

namespace VF
{
	int MeshManager::createMesh_andGetId(const TArray<FVector>& vertices, const TArray<int32>& triangles)
	{
		int id = 0;
		if (recycledSectionIds.size() > 0)
		{
			auto iter = recycledSectionIds.end();
			id = *iter;
			recycledSectionIds.erase(iter);
		}
		else
		{
			id = nextSectionId;
			nextSectionId++;
		}
		customMesh->CreateMeshSection(id, vertices, triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

		return id;
	}

	void MeshManager::updateMesh_withId(int id, const TArray<FVector>& vertices, const TArray<int32>& triangles)
	{
		customMesh->CreateMeshSection(id, vertices, triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	}
}