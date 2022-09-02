#pragma once

#include "VF_Base.h"

namespace VF
{
	class IVF_Obj
	{
	private:
	protected:
		GameContext* get_context()
		{
			return context_id.get();
		}
		ContextId get_context_id()
		{
			return context_id;
		}
		ContextId context_id;
	public:
		// IVF_Obj interface ///////////////////////
		//初始化：override时需要调用父类
		virtual void IVF_Obj_init(ContextId id) {
			context_id = id;
		}
		virtual void IVF_Obj_begin() = 0;
		virtual void IVF_Obj_end() = 0;
		virtual ~IVF_Obj() {};
		////////////////////////////////////////////


		////////////////////////////////////////////
		// copy templete

		//// IVF_Obj interface ///////////////////////
		//public:
		//	//初始化：override时需要调用父类
		//virtual void IVF_Obj_init(ContextId id) override;
		//virtual void IVF_Obj_begin() override;
		//virtual void IVF_Obj_end() override;
		//
		//////defination templete
		////// IVF_Obj /////////////////////////
		////void ::IVF_Obj_init(ContextId id) {
		////	IVF_Obj::IVF_Obj_init(id);
		////}
		////void ::IVF_Obj_begin(){}
		////void ::IVF_Obj_end(){}
		////////////////////////////////////////

		////////////////////////////////////////////
	};

}
