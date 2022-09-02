#pragma once

#include "VF_Base.h"
#include "vf_NetworkManager.h"
namespace VF
{
	//数据先经过rust服务端，然后再分发到ue服务端进行分区块计算，
	//若只有一个ue服务端，就只用一个
	class PartServer
	{
	public:
		void start();
	};
}