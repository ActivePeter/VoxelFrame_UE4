#include "MeshManager.h"
#include "../WorldActor.h"
namespace VF
{
	int MeshManager::createMeshAndGetId(const TArray<FVector>& vertices, const TArray<int32>& triangles)
	{
		int id = 0;
		if (recycledSectionIds.size() > 0)
		{
			//auto iter = recycledSectionIds.end();
			id = recycledSectionIds.back();
			recycledSectionIds.pop_back();
			//recycledSectionIds.erase(iter);
		}
		else
		{
			id = nextSectionId;

			auto newMesh = NewObject<UProceduralMeshComponent>(context->worldActor);
			newMesh->RegisterComponent();
			newMesh->bUseAsyncCooking = true;

			meshCompPool.emplace_back(newMesh);
			nextSectionId++;
		}
		assert(customMesh);
		assert(context->worldActor);
		auto mesh=meshCompPool[id];
		
		mesh->CreateMeshSection(0/*id*/, vertices, triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
		
		return id;
	}

	void MeshManager::updateMeshWithId(int id, const TArray<FVector>& vertices, const TArray<int32>& triangles)
	{
		auto mesh = meshCompPool[id];
		
		mesh->CreateMeshSection(0/*id*/, vertices, triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	}

	void MeshManager::delMeshWithId(int id)
	{
		recycledSectionIds.emplace_back(id);
		//mesh->SetVisibility()
	}
}