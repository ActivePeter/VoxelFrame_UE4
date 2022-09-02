#include "texture_man.h"

#include "GameContext.h"
#include "HAL/FileManagerGeneric.h"
#include "stbipp/Image.hpp"
#include "stbipp/ImageImporter.hpp"
#include "ImageUtils.h"
//#include "Components/PrimitiveComponent.h"

namespace VF
{
	//BlockMaterialModelStatic BlockMaterialModelStatic::instance;

	const int BlockTextureWidth = 32;
	////defination templete
			// IVF_Obj /////////////////////////
	void TextureManager::IVF_Obj_init(ContextId id) {
		IVF_Obj::IVF_Obj_init(id);
	}
	void TextureManager::IVF_Obj_begin()
	{
		load_and_make_texture();
	}
	void TextureManager::IVF_Obj_end()
	{

	}

	UTexture2D* test_texture(uint8_t* from, int w, int h)
	{
		//int32 TextureHeight = 1024;
		//int32 TextureWidth = 1024;
		UTexture2D* TheTexture2D = UTexture2D::CreateTransient(w, h, PF_B8G8R8A8);
		{
			//uint8* Pixels = new uint8[TextureWidth * TextureHeight * 4];
			//for (int32 y = 0; y < TextureHeight; y++)
			//{
			//	for (int32 x = 0; x < TextureWidth; x++)
			//	{
			//		int32 curPixelIndex = ((y * TextureWidth) + x);
			//		int32 color = x;
			//		while (color > 255)
			//			color -= 255;
			//		Pixels[4 * curPixelIndex] = color;
			//		Pixels[4 * curPixelIndex + 1] = color;
			//		Pixels[4 * curPixelIndex + 2] = color;
			//		Pixels[4 * curPixelIndex + 3] = 255;
			//	}
			//}
			void* TextureData = TheTexture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, from, sizeof(uint8) * w * h * 4);
			//delete Pixels;
		}
		TheTexture2D->PlatformData->Mips[0].BulkData.Unlock();//TheTexture2D->PlatformData = new FTexturePlatformData();	// Then we initialize the PlatformData

#define UpdateResource UpdateResource
		TheTexture2D->UpdateResource();
		return TheTexture2D;
	}

	void TextureManager::load_and_make_texture()
	{
		FString material_dir = FPaths::ProjectDir();
		material_dir += "Content/vf_Material/";
		auto block_texture_dir = material_dir + "block_texture/";

		TArray<FString> block_textures;
		FFileManagerGeneric::Get().FindFiles(block_textures,
			*block_texture_dir, TEXT(".png"));
		VF_LogWarning("load texture at %s %d",
			*block_texture_dir, block_textures.Num());

		//1.根据图片数目确定整个图的大小
		int w, h;
		{
			w = 1;
			while (w * w < block_textures.Num())
			{
				w++;
			}
			//到这里确定可以容纳方块数的正方形宽度
			h = block_textures.Num() / w;
			if (block_textures.Num() % w != 0)
			{
				//不能整除，高度+1
				h++;
			}
		}
		//2.生成整体图像缓冲
		//stbipp::Image _atlas(w * BlockTextureWidth, h * BlockTextureWidth);
		std::vector<uint8_t> _atlas;
		_atlas.resize(w * BlockTextureWidth * h * BlockTextureWidth * 4);
		//3.读取所有材质并写入到缓冲
		int block_cnt = 0;
		for (int k = 0; k < block_textures.Num(); k++)
		{
			auto& fname = block_textures[k];
			if (fname.EndsWith(".png"))
			{
				auto fullpath = block_texture_dir + fname;
				//VF_LogWarning("texture file %s", *fname);
				//TArray<uint8_t> data;
				//std::string cfname(TCHAR_TO_ANSI(*fname));
				std::string cfullpath(TCHAR_TO_ANSI(*fullpath));
				//stbipp::Image image;
				int img_w, img_h;
				auto imgdata = stbipp::loadUCharImage(cfullpath, img_w, img_h, stbipp::ImageFormat::RGBA32);
				VF_assert(img_w == BlockTextureWidth);
				VF_assert(img_h == BlockTextureWidth);
				if (img_w == BlockTextureWidth &&
					img_h == BlockTextureWidth
					)
				{
					int offset_x = (block_cnt % w) * BlockTextureWidth;
					int offset_y = (block_cnt / w) * BlockTextureWidth;
					for (int x = 0; x < img_w; x++)
					{
						for (int y = 0; y < img_h; y++)
						{
							_util::_img_process::set_bgra_one_pixel(
								_atlas, w * BlockTextureWidth, h * BlockTextureWidth, x + offset_x, y + offset_y,
								imgdata[y * img_w * 4 + x * 4],
								imgdata[y * img_w * 4 + x * 4 + 1],
								imgdata[y * img_w * 4 + x * 4 + 2],
								imgdata[y * img_w * 4 + x * 4 + 3]
							);
							////rgb->bgr
							//_atlas.push_back(imgdata[y * img_w * 4 + x * 4 + 2]);
							//_atlas.push_back(imgdata[y * img_w * 4 + x * 4 + 1]);
							//_atlas.push_back(imgdata[y * img_w * 4 + x * 4]);
							//_atlas.push_back(imgdata[y * img_w * 4 + x * 4 + 3]);
						}
					}
					{//注册到map
						int name_end;
						//fname.FindLastChar('/', name_begin);
						fname.FindLastChar('.', name_end);
						std::string name(name_end, ' ');
						for (int j = 0; j < name_end; j++)
						{
							name[j] = fname[j];
						}
						VF_LogWarning("texture file %s", name.c_str());
						this->block_face_name_2_index[name] = block_cnt;
					}
					block_cnt++;
				}
				stbipp::freeStbData(imgdata);
			}

			//auto succ = stbipp::loadImage(cfullpath, image, stbipp::ImageFormat::RGBA32);
			//VF_assert(succ);
			//if (succ)
			//{
			//	int name_end;
			//	//fname.FindLastChar('/', name_begin);
			//	fname.FindLastChar('.', name_end);
			//	std::string name(name_end, ' ');
			//	for (int j = 0; j < name_end; j++)
			//	{
			//		name[j] = fname[j];
			//	}
			//	VF_LogWarning("texture file %s", name.c_str());
			//	this->block_face_name_2_index[name] = i;
			//	_atlas.copyDataFrom(image, (i % w) * BlockTextureWidth, (i / w) * BlockTextureWidth);
			//	i++;
			//}
			//FFileHelper::LoadFileToArray(data,"");
		}

		VF_LogWarning("atlas w:%d, atlas h:%d, block cnt:%d", w, h, block_cnt);
		//TArray<FColor> colors;
		//colors.Reserve(_atlas.m_data.size());
		//for (auto color : _atlas.m_data)
		//{
		//	//FColor ;
		//	colors.Push(FColor(0, 255, 0, 0));
		//}
		//4.将图片生成material，
		block_atlas_texture =
			test_texture(_atlas.data(), w * BlockTextureWidth, h * BlockTextureWidth);
		/*FImageUtils::CreateTexture2D(
			w, h, colors, get_context()->worldActor,
			"block_atlas", EObjectFlags::RF_NoFlags,
			FCreateTexture2DParameters());*/
		block_atlas_texture->Filter = TEnumAsByte<TextureFilter>(TextureFilter::TF_Nearest);
		//#define UpdateResource UpdateResource
		block_atlas_texture->UpdateResource();
		block_atlas_texture->AddToRoot();

		this->block_atlas_w = w;
		this->block_atlas_h = h;
		//this->block_atlas_mat =
			//UMaterialInstanceDynamic::Create(NULL, get_context()->worldActor);
		//block_atlas_mat->SetTextureParameterValue(FName("block_texture"), block_atlas_texture);
		//block_atlas_mat->SetTe
		//get_context()->worldActor->block_texture = test_texture();
		//block_atlas_texture = test_texture();

		get_context()->blockManager->after_block_texture_loaded();
		get_context()->worldActor->bpv_texture_rendered = this->block_atlas_texture;
		//FFileHelper::LoadFileToArray(FPaths::ProjectDir);
		//FFileHelper::LoadFileToArray(data,TEXT(""), 0);
	}

	int TextureManager::get_index_of_block_face_name(const std::string& str)
	{
		auto find = this->block_face_name_2_index.find(str);
		VF_assert(find != block_face_name_2_index.end());
		if (find == block_face_name_2_index.end())
		{
			for (auto& elem : block_face_name_2_index)
			{
				VF_LogWarning("exist %s", *FString(elem.first.c_str()));
			}
			VF_LogWarning("look for %s", *FString(str.c_str()));
			return -1;
		}
		return this->block_face_name_2_index[str];
	}

	VFRect<float> TextureManager::get_uv_rect_of_block_face_name(
		const std::string& str)
	{
		auto find = this->block_face_name_2_index.find(str);
		VF_assert(find != block_face_name_2_index.end());
		if (find == block_face_name_2_index.end())
		{
			return VFRect<float>(0, 0, 0, 0);
		}
		auto index = this->block_face_name_2_index[str];

		return get_uv_rect_of_block_face_index(index);
	}
	VFRect<float> TextureManager::get_uv_rect_of_block_face_index(
		int index)
	{
		auto uvw = 1.0f / this->block_atlas_w;
		auto uvh = 1.0f / block_atlas_h;
		return
			VFRect<float>(
				(index % block_atlas_w) * 1.0 / block_atlas_w
				,
				(index / block_atlas_w) * 1.0 / block_atlas_h
				, uvw, uvh
				);
	}

	////////////////////////////////////
}
