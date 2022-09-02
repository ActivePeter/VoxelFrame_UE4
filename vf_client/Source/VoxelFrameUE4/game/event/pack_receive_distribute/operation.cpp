#include "../PackReceiveEvent.h"
#include "game/GameContext.h"

namespace VF
{
	void pack_recv_handle(
		std::shared_ptr<ClientOperationFailed> cof,
		GameContext& context)
	{

		context.operation_rec->operation_done(
			cof->operation_id(),
			_operation_rec::OperationState::Fail
		);
	}
	void pack_recv_handle(
		std::shared_ptr<ClientOperationSucc> cof,
		GameContext& context)
	{
		context.operation_rec->operation_done(
			cof->operation_id(),
			_operation_rec::OperationState::Succ
		);
	}
}
