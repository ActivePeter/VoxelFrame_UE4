#include "GameContext.h"

#include "Kismet/GameplayStatics.h"

namespace VF
{
	static GameContext* context;

	GameContext* GameContext::getContext()
	{
		assert(context);
		return context;
	}
	GameContext::GameContext()
	{
		chunkManager = std::make_shared<ChunkManager>();
		chunkManager->init(this);

		meshManager = std::make_shared<MeshManager>();
		meshManager->init(this);

		blockManager = std::make_shared<_Block::BlockManager>();
		blockManager->registAll();

		blockPreviewManager = std::make_shared<_Block::BlockPreviewManager>();

		context = this;
	}
	GameContext::~GameContext()
	{
		context = nullptr;
	}
	void GameContext::worldActorCallWhenBeginPlay()
	{
		TArray<AActor*> blockPutPreviewerArr;
		UGameplayStatics::GetAllActorsWithTag(worldActor->GetWorld(),"blockPutPreviewer", blockPutPreviewerArr);
		assert(blockPutPreviewerArr.Num() == 1);
		blockPreviewManager->init(Cast<AStaticMeshActor>(blockPutPreviewerArr[0]));
		assert(blockPreviewManager->checkInited());
	}
}
