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
	struct MeshManager :public IVF_Obj {
		UProceduralMeshComponentWithBind* customMesh;

		/*销毁后可再利用的网格插件*/
		std::list<int> recycledSectionIds;

		/*网格组件池*/
		std::vector<UProceduralMeshComponentWithBind*> meshCompPool;

		int nextSectionId = 0;

		UProceduralMeshComponentWithBind* getMeshById(int id);

		int createMeshAndGetId(
			std::weak_ptr<void> bindedTo, const FName& tag,
			MeshConstructData& construct_data);
		void updateMeshWithId(int id,
			MeshConstructData& construct_data);
		void delMeshWithId(int id);

		// IVF_Obj interface ///////////////////////
	public:
		//初始化：override时需要调用父类
		virtual void IVF_Obj_init(ContextId id) override;
		virtual void IVF_Obj_begin() override;
		virtual void IVF_Obj_end() override;



		void begin();
	};
}
