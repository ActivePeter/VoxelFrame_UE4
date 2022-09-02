#pragma once
#include "VF_Base.h"
#include "parallel_hashmap/phmap.h"
namespace VF
{
	//struct BlockMaterialModelStatic
	//{
	//	static BlockMaterialModelStatic instance;
	//	//ConstructorHelpers::FObjectFinderOptional<UTexture> TextureFinder;
	//	ConstructorHelpers::FObjectFinderOptional<UMaterial> MaterialFinder;
	//	UMaterial* Material;
	//	BlockMaterialModelStatic()
	//		: //TextureFinder(TEXT("Texture2D'/Game/Textures/2DBackground.2DBackground'"))
	//		MaterialFinder(TEXT("Material'/Game/Content/vf_Material/ChunkBlockMaterial.ChunkBlockMaterial'"))
	//	{
	//		Material = MaterialFinder.Get();
	//	}
	//};

	class TextureManager :public IVF_Obj
	{
		////////////////////////////////////////////
			// copy templete

			// IVF_Obj interface ///////////////////////
	private:
		//材质文件名对应的index
		phmap::flat_hash_map<std::string, int> block_face_name_2_index;
	public:
		//初始化：override时需要调用父类
		virtual void IVF_Obj_init(ContextId id) override;
		virtual void IVF_Obj_begin() override;
		virtual void IVF_Obj_end() override;

		//加载和构建材质
		void load_and_make_texture();
		//UMaterialInstanceDynamic* block_atlas_mat = nullptr;
		//给区块mesh绑定的material
		UTexture2D* block_atlas_texture = nullptr;
		int block_atlas_w = 0;
		int block_atlas_h = 0;
		//获取对应方块材质名的uv值
		int get_index_of_block_face_name(const std::string& str);
		VFRect<float> get_uv_rect_of_block_face_name(const std::string& str);
		VFRect<float> get_uv_rect_of_block_face_index(int index);
	};
}
