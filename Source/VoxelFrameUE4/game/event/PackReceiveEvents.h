#pragma once
#include "./Event.h"
#include "ProtoPacks/chunk.pb.h"
namespace VF
{
	namespace _Event
	{
		class NewChunkLoadEvent :IEvent
		{
		public:
			// IEvent ////////////////////////////////
			void run() override;
			/////////////////////////////////////////

			// Self ////////////////////////////////
			std::shared_ptr<ChunkPack> chunk_pack;
			/////////////////////////////////////////
		};
	}
}

