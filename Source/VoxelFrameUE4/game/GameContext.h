#pragma once


#include "VF_Base.h"
#include "paecs/paecs.h"
#include "event/Event.h"

class AWorldActor;
namespace VF
{
	class ChunkManager;
	struct MeshManager;
	class NetworkManager;

	namespace _Block
	{
		class BlockManager;
		class BlockPreviewManager;
	}

	struct Ecs
	{
		std::unique_ptr<paecs::Scene> scene;
		paecs::SysGroup commonUpdateSys;
		void init()
		{
			scene = std::make_unique<paecs::Scene>();
		}
	};

	class GameContext
	{

	public:
		static GameContext* getContext();
		std::unique_ptr<ChunkManager> chunkManager;
		std::unique_ptr<MeshManager> meshManager;
		std::unique_ptr<_Block::BlockManager> blockManager;
		std::unique_ptr<_Block::BlockPreviewManager> blockPreviewManager;
		std::unique_ptr < NetworkManager> networkManager;

		Ecs ecs;
		AWorldActor* worldActor;

		//std::list<std::shared_ptr<_Event::IEvent>> events;
		_Event::EventList event_list;

		/*
		 *WorldActor 调用的函数，用来检查是否一切就绪
		 ***/
		void worldActorCallWhenBeginPlay();

		GameContext();
		~GameContext();
	};
}

#include "MeshManager.h"
#include "ChunkManager.h"
#include "block/block.h"
#include "VoxelFrameUE4/WorldActor.h"
#include "block/BlockPreviewManager.h"
