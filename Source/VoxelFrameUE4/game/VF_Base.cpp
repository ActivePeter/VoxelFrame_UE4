#include "VF_Base.h"
namespace VF
{
    namespace _Base
    {
        void conv_point_from_ue_2_vf(Type::Vec3F &p)
        {
            p = p / 100;
            auto bak = p.Y;
            p.Y = p.X;
            p.X = bak;
        }
        void conv_point_from_vf_2_ue(Type::Vec3F &p)
        {
            p = p * 100;
            auto bak = p.Y;
            p.Y = p.X;
            p.X = bak;
        }
    }
}