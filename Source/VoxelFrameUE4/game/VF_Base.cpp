#include "VF_Base.h"

#include "google/protobuf/message.h"
#include "google/protobuf/message_lite.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"
#include "google/protobuf/stubs/stl_util.h"

namespace VF
{
	namespace _base
	{
		//废弃函数
		void conv_point_from_ue_2_vf(_type::Vec3F& p)
		{
			p = p / VF_WorldScale;
			auto bak = p.Y;
			p.Y = p.Z;
			p.Z = bak;
		}
		//废弃函数
		void conv_point_from_vf_2_ue(_type::Vec3F& p)
		{
			p = p * VF_WorldScale;
			auto bak = p.Y;
			p.Y = p.Z;
			p.Z = bak;
		}

		//检测点是否在方块边界上
		BlockGridSide checkPointUEOnBlockGridSide(const _type::Vec3F& p)
		{
			if (
				abs(p.X - roundf(p.X)) < VF_RoughEpsilon
				)
			{
				return BlockGridSide::X;
			}
			else if (
				abs(p.Y - roundf(p.Y)) < VF_RoughEpsilon
				)
			{
				return BlockGridSide::Y;
			}
			else if (
				abs(p.Z - roundf(p.Z)) < VF_RoughEpsilon
				)
			{
				return BlockGridSide::Z;
			}
			else
			{
				return BlockGridSide::No;
			}
		}

	}

	namespace _util {
		bool is_rough_zero(const FVector& vector)
		{
			return vector.Size() < VF_RoughEpsilon;
		}
		bool is_rough_zero(float f)
		{
			return abs(f) < VF_RoughEpsilon;
		}

		void _img_process::set_bgra_one_pixel(std::vector<uint8_t>& data,
			int w, int h, int x, int y, uint8_t r,
			uint8_t g, uint8_t b, uint8_t a)
		{
			if (x < 0 || x >= w || y < 0 || y >= h)
			{
				return;
			}
			data[(x + y * w) * 4] = b;
			data[(x + y * w) * 4 + 1] = g;
			data[(x + y * w) * 4 + 2] = r;
			data[(x + y * w) * 4 + 3] = a;
		}

		//void _img::copy_stbipp_image_2_utexture2d(
		//	const stbipp::Image& img, UTexture2D& target, int x, int y)
		//{
		//	int write_w = img.width() - x;
		//	int write_h = img.height() - y;
		//	write_w = std::min(write_w, target.GetSizeX());
		//	write_h = std::min(write_h, target.GetSizeY());

		//	auto tar_image_data = target.PlatformData->Mips[0].BulkData;
		//	auto locked = (uint32_t*)tar_image_data.Lock(LOCK_READ_WRITE);
		//	for (int rowIndex = 0; rowIndex < write_h/*other.m_height*/; ++rowIndex)
		//	{
		//		for (int columnIndex = 0; columnIndex < write_w/*other.m_width*/; ++columnIndex)
		//		{

		//			//(*this)(columnIndex + x, rowIndex + y) = static_cast<Color>(other(columnIndex, rowIndex));
		//		}
		//	}
		//	tar_image_data.Unlock();
		//	target.UpdateResource();
		//}
	}
}
