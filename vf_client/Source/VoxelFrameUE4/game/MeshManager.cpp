#include "MeshManager.h"
#include "game_WorldActor.h"
namespace VF
{
	UProceduralMeshComponentWithBind* MeshManager::getMeshById(int id)
	{
		return meshCompPool[id];
	}

	int MeshManager::createMeshAndGetId(
		std::weak_ptr<void> bindedTo, const FName& tag,
		MeshConstructData& construct_data)
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

			auto newMesh = NewObject<UProceduralMeshComponentWithBind>(get_context()->worldActor);
			newMesh->init();

			/*newMesh->bindedTo.reset();*/

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



		//vers.Add(FVector(0.000000, 0.000000, 0.000000));
		//vers.Add(FVector(0.000000, 100, 0.000000));
		//vers.Add(FVector(100, 100, 0));
		//vers.Add(FVector(100, 0, 0));
		//vertices.SetNum(4);
		VFArray<int32> tris;
		tris.Add(0); tris.Add(1); tris.Add(2);
		tris.Add(1); tris.Add(2); tris.Add(3);
		//triangles.SetNum(6);

		mesh->CreateMeshSection(0/*id*/,
			//vers, tris,
			construct_data.vertices, construct_data.triangles,
			TArray<FVector>(), construct_data.uvs, TArray<FColor>(),
			TArray<FProcMeshTangent>(), true);

		//VF_assert(get_context()->taxture_man->block_atlas_mat);
		//if (get_context()->taxture_man->block_atlas_mat)
		{
			// Grab material
			//auto Material = mesh->GetMaterial(0);
			//VF_assert(get_context()->taxture_man->block_atlas_texture);
			//if (Material == NULL)
			//{
			//if (get_context()->taxture_man->block_atlas_texture)
			{
				auto Ma =
					UMaterialInstanceDynamic::Create(
						get_context()->worldActor->block_mat_parent,
						get_context()->worldActor);
				Ma->SetTextureParameterValue(TEXT("Texture"),
					get_context()->taxture_man->block_atlas_texture
					//get_context()->worldActor->block_texture
				);
				mesh->SetMaterial(0,
					Ma
					//	get_context()->worldActor->block_mat_parent
				);
			}

			//Material = Ma;
			//UMaterial::GetDefaultMaterial(MD_Surface);
		//}

		//mesh->SetMaterial(0, Material);
		//auto mp = Cast<UMaterialInstanceDynamic>(mesh->GetMaterial(0));
		//VF_assert(mp);
		//if (mp)
		//{
			//mp->SetTextureParameterValue(FName("DynamicTexture"),
				//get_context()->taxture_man->block_atlas_texture);
		//}
		//mesh->GetMaterial(0);
		//mesh->CreateAndSetMaterialInstanceDynamicFromMaterial(0, get_context()->taxture_man->block_atlas_mat);
		//mesh->SetMaterial(0, get_context()->taxture_man->block_atlas_mat);
		}

		return id;
	}

	void MeshManager::updateMeshWithId(int id, MeshConstructData& construct_data)
	{
		auto mesh = meshCompPool[id];
		mesh->bUseAsyncCooking = true;
		mesh->CreateMeshSection(0/*id*/, construct_data.vertices, construct_data.triangles,
			TArray<FVector>(), construct_data.uvs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	}

	void MeshManager::delMeshWithId(int id)
	{
		recycledSectionIds.emplace_back(id);
		//mesh->SetVisibility()
	}

	//defination templete
		// IVF_Obj /////////////////////////
	void MeshManager::IVF_Obj_init(ContextId id) {
		IVF_Obj::IVF_Obj_init(id);
	}
	void MeshManager::IVF_Obj_begin() {}
	void MeshManager::IVF_Obj_end() {}
	////////////////////////////////////

	//void MeshManager::begin()
	//{

	//	this->context = ;
	//	/*this->customMesh = context->worldActor->custom_mesh;
	//	this->customMesh->bUseAsyncCooking = false;
	//	this->customMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
	//}
}
