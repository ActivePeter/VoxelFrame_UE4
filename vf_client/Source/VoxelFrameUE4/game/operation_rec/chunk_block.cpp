#include "chunk_block.h"

#include "game/GameContext.h"

namespace VF
{
	namespace _operation_rec
	{
		void PutBlockOperation::act(GameContext& ctx)
		{
			this->IOperation_do_act([&ctx, this]()
				{
					auto changer = _block::BlockDataChanger(ctx);
					this->from = changer
						.set_block_data(
							this->x, this->y, this->z,
							this->to);
					VF_LogWarning("block from:%d to:%d",
						from, to
					);
					changer.update_chunk_mesh();
				});
		}
		void PutBlockOperation::cancel(GameContext& ctx)
		{
			this->IOperation_do_cancel([&ctx, this]()
				{
					_block::BlockDataChanger(ctx)
						.set_block_data(
							this->x, this->y, this->z,
							this->from);
				});
		}
	}
}
