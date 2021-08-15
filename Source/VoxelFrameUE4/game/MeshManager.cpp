#include "MeshManager.h"
#include "../WorldActor.h"
namespace VF
{
	UProceduralMeshComponentWithBind* MeshManager::getMeshById(int id)
	{
		return meshCompPool[id];
	}
	int MeshManager::createMeshAndGetId(void* bindedTo, const FName& tag, const TArray<FVector>& vertices, const TArray<int32>& triangles)
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

			auto newMesh = NewObject<UProceduralMeshComponentWithBind>(context->worldActor);
			//newMesh->meshId = id;


			newMesh->RegisterComponent();
			newMesh->bUseAsyncCooking = true;

			meshCompPool.emplace_back(newMesh);
			nextSectionId++;
		}
		assert(customMesh);
		assert(context->worldActor);
		auto mesh = meshCompPool[id];
		assert(mesh);
		//设置绑定到的对象指针
		mesh->bindedTo = bindedTo;
		//设置新的绑定的tag
		mesh->ComponentTags.SetNum(0);
		mesh->ComponentTags.Add(tag);
		mesh->CreateMeshSection(0/*id*/, vertices, triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

		return id;
	}

	void MeshManager::updateMeshWithId(void* bindedTo, int id, const TArray<FVector>& vertices, const TArray<int32>& triangles)
	{
		auto mesh = meshCompPool[id];
		mesh->bUseAsyncCooking = true;
		mesh->bindedTo = bindedTo;
		mesh->CreateMeshSection(0/*id*/, vertices, triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	}

	void MeshManager::delMeshWithId(int id)
	{
		recycledSectionIds.emplace_back(id);
		//mesh->SetVisibility()
	}
}