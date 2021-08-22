#include "VF_Base.h"
namespace VF
{
	namespace _Base
	{
		void conv_point_from_ue_2_vf(Type::Vec3F& p)
		{
			p = p / VF_WorldScale;
			auto bak = p.Y;
			p.Y = p.Z;
			p.Z = bak;
		}
		void conv_point_from_vf_2_ue(Type::Vec3F& p)
		{
			p = p * VF_WorldScale;
			auto bak = p.Y;
			p.Y = p.Z;
			p.Z = bak;
		}

		//检测点是否在方块边界上
		BlockGridSide checkPointUEOnBlockGridSide(const Type::Vec3F& p)
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
}