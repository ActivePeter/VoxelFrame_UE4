#include "operation_rec.h"

namespace VF
{
	namespace _operation_rec
	{
		//////defination templete
		// IVF_Obj /////////////////////////
		void OperationRec::IVF_Obj_init(ContextId id) {
			IVF_Obj::IVF_Obj_init(id);
		}
		void OperationRec::IVF_Obj_begin() {}
		void OperationRec::IVF_Obj_end() {}

		////////////////////////////////////


		void OperationRec::operation_done(
			OperationId id, OperationState state)
		{
			VF_LogWarning("operation_done,state:%s",
				state == OperationState::Fail ? "fail" : "succ"
			);
			if (state == OperationState::Fail)
			{
				this->operations[id]->cancel(*get_context());
			}
			this->operations.erase(id);
		}

	}
}