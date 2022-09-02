
namespace VF
{
	namespace _Event
	{
		class IEvent;
		class EventList;
	}
}

#ifndef __Event_H_
#define __Event_H_
#include "VF_Base.h"
namespace VF
{
	class GameContext;
}

namespace VF
{
	namespace _Event
	{
		class IEvent
		{
		public:
			virtual void run(GameContext& context) {};
		};

		class EventList
		{
		protected:
			std::list<std::shared_ptr<IEvent>> events;
			std::mutex list_mutex;
		public:


			void push(const std::shared_ptr<IEvent>& event)
			{
				list_mutex.lock();
				events.push_front(event);
				list_mutex.unlock();
			}
			auto pop()
			{
				auto a = events.back();
				list_mutex.lock();
				events.pop_back();
				list_mutex.unlock();
				return a;
			}
			auto size()
			{
				return events.size();
			}
		};
	}
}
#endif