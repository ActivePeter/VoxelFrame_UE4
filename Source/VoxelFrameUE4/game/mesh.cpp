#include "mesh.h"

namespace VF
{
	void MeshConstructData::before_construct()
	{

		{
			vertices.SetNum(0);
			triangles.SetNum(0);
			uvs.SetNum(0);
			needConstruct = false;
		}
	}

}
