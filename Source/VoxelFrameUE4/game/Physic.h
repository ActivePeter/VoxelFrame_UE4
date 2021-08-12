#pragma once
#include "VF_Base.h"
namespace VF
{
    namespace _Physic
    {
        /**
    * 根据点和射线方向分析碰撞到的方块信息
    * */
        bool getBlockRayColidInfoOfPoint(Type::Vec3F rayDirection, Type::Vec3F colidP);
    }
}