#pragma once
#include "VF_Base.h"
#include "functional"
#include "parallel_hashmap/phmap.h"
namespace VF
{
	//记录游戏中对数据的操作
	namespace _operation_rec
	{
		using OperationId = uint32_t;
		enum class OperationState
		{
			Succ,
			Fail
		};
		class IOperation
		{
		protected:
			bool operated = false;

			void IOperation_do_act(std::function<void()> cb)
			{
				if (!this->IOperation::operated)
				{
					this->IOperation::operated = true;

					cb();
				}
			}
			void IOperation_do_cancel(std::function<void()> cb)
			{
				if (this->IOperation::operated)
				{
					this->IOperation::operated = false;

					cb();
				}
			}
		public:
			int operation_id = 0;
			virtual void act(GameContext& ctx) = 0;
			virtual void cancel(GameContext& ctx) = 0;
			virtual ~IOperation() {};
		};


		class OperationRec :public IVF_Obj
		{
			//// IVF_Obj interface ///////////////////////
		public:
			//初始化：override时需要调用父类
			virtual void IVF_Obj_init(ContextId id) override;
			virtual void IVF_Obj_begin() override;
			virtual void IVF_Obj_end() override;

			/// <summary>
			/// 自带函数
			/// </summary>
			template <class ImplType>
			struct OperationCarray
			{
				ImplType& ref;
				OperationId id;
			};

			// 创建新的操作
			template <class ImplType, class... ArgTypes>
			//std::pair<ImplType&, OperationId >
			OperationCarray<ImplType>
				new_operation(ArgTypes&&... _Args)
			{
				auto p = std::make_unique<ImplType>(
					std::forward<ArgTypes>(_Args)...);
				operations[next_op_id] = std::move(p);
				auto id = next_op_id;
				next_op_id++;

				auto& _pp = operations[id];
				auto& pp = *static_cast<ImplType*>(_pp.get());

				return { pp,id };
				//return { pp,id };
			}
			void operation_done(OperationId id, OperationState state);
		private:
			OperationId next_op_id = 0;
			phmap::flat_hash_map<OperationId, std::unique_ptr<IOperation>> operations;
		};


	}
}