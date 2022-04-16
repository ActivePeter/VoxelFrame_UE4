#pragma once

#include "VF_Base.h"

namespace VF
{
	struct MeshConstructData
	{
		/**
		 * 顶点
		 */
		TArray<FVector> vertices;

		/**
		 * 顶点索引三角形
		 */
		TArray<int32> triangles;

		/*
		 * uv
		 */
		VFArray<FVector2D>uvs;

		/**
		 * 在修改chunk数据后置为true
		 */
		bool needConstruct = true;

		//MeshConstructData() {}
		void before_construct();
	};
}