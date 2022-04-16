#pragma once
#include "VF_Base.h"
#include "operation_rec.h"
#include "game/block/block.h"

namespace VF
{
	namespace _operation_rec
	{
		class PutBlockOperation :public IOperation
		{
		public:
			virtual void act(GameContext& ctx) override;
			virtual void cancel(GameContext& ctx) override;

			_Block::BlockDiscription to;
			_Block::BlockDiscription from;
			int x;
			int y;
			int z;

			PutBlockOperation(
				int x1, int y1, int z1,
				_Block::BlockDiscription to1)
				:x(x1), y(y1), z(z1), to(to1)
			{

			}
		};

	}
}