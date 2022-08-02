// Fill out your copyright notice in the Description page of Project Settings.

#include "game_WorldActor.h"
#include "ProceduralMeshComponent.h"
#include "VF_EntityPlayerCpp.h"
#include "DataBridgeInstance.h"

// Sets default values
AWorldActor::AWorldActor()
{
	//the following must be at here
	//custom_mesh = this->CreateDefaultSubobject<VF::UProceduralMeshComponentWithBind>("customMesh");
	this->PrimaryActorTick.bCanEverTick = true;
	this->SetActorTickEnabled(true);
}

// Called when the game starts or when spawned
void AWorldActor::BeginPlay()
{
	Super::BeginPlay();

	//GetWorld()->GetGameInstance()
	auto db = Cast<UDataBridgeInstance, UGameInstance>(GetWorld()->GetGameInstance());
	if (db->IsServer)
	{
		context = std::make_unique<VF::GameContext>(
			VF::ClientType::PartServer, this);
		VF_LogWarning("init as part_server");
	}
	else
	{
		context = std::make_unique<VF::GameContext>(
			VF::ClientType::Player, this);
		VF_LogWarning("init as player");
	}

	while (context.get()->context_id.get() != context.get())
	{
		VF_LogWarning("_error context id set");
	}
	//context->setContext(context);
	context->world_actor_call_when_BeginPlay();
	//generateCubeTest();
	//customMesh.bU
}

void AWorldActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	context->world_actor_call_when_EndPlay();
	context = nullptr;
}

// Called every frame
void AWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	static float time_sum = 0;
	time_sum += DeltaTime * 1000;
	context->deltatime = DeltaTime;
	//VF_LogWarning("AWorldActor::Tick");
	while (context->event_list->size() > 0)
	{
		auto a = context->event_list->pop();
		a->run(*context);
	}
	/*for (auto& event : context->event_list)
	{
		event.run();
	}*/

	context->chunkManager->cookOneChunk();

	context->ecs.commonUpdateSys.runAll();
	if (time_sum > 100)
	{
		context->ecs.update_sys_100ms.runAll();
		time_sum = 0;
		/*if (context->type == VF::ClientType::PartServer)
		{
			VF_LogWarning("100ms sv");
		}
		else
		{
			VF_LogWarning("100ms cl");
		}*/
	}
	context->ecs.tick_end_sys.runAll();
}

void AWorldActor::generateCubeTest()
{
	//6 sides on cube, 4 verts each (corners)

	//These are relative locations to the placed Actor in the world
	vertices.Add(FVector(0, -100, 0));	 //lower left - 0
	vertices.Add(FVector(0, -100, 100)); //upper left - 1
	vertices.Add(FVector(0, 300, 0));	 //lower right - 2
	vertices.Add(FVector(0, 100, 100));	 //upper right - 3

	vertices.Add(FVector(200, -100, 0));   //lower front left - 4
	vertices.Add(FVector(100, -100, 100)); //upper front left - 5

	vertices.Add(FVector(100, 100, 100)); //upper front right - 6
	vertices.Add(FVector(100, 100, 0));	  //lower front right - 7

	//Back face of cube
	VF::addOneTriangle2IndexArr(triangles, 0, 2, 3);
	VF::addOneTriangle2IndexArr(triangles, 3, 1, 0);

	//Left face of cube
	VF::addOneTriangle2IndexArr(triangles, 0, 1, 4);
	VF::addOneTriangle2IndexArr(triangles, 4, 1, 5);

	//Front face of cube
	VF::addOneTriangle2IndexArr(triangles, 4, 5, 7);
	VF::addOneTriangle2IndexArr(triangles, 7, 5, 6);

	//Right face of cube
	VF::addOneTriangle2IndexArr(triangles, 7, 6, 3);
	VF::addOneTriangle2IndexArr(triangles, 3, 2, 7);

	//Top face
	VF::addOneTriangle2IndexArr(triangles, 1, 3, 5);
	VF::addOneTriangle2IndexArr(triangles, 6, 5, 3);

	//bottom face
	VF::addOneTriangle2IndexArr(triangles, 2, 0, 4);
	VF::addOneTriangle2IndexArr(triangles, 4, 7, 2);

	/*TArray<FLinearColor> VertexColors;
	VertexColors.Add(FLinearColor(0.f, 0.f, 1.f));
	VertexColors.Add(FLinearColor(1.f, 0.f, 0.f));
	VertexColors.Add(FLinearColor(1.f, 0.f, 0.f));
	VertexColors.Add(FLinearColor(0.f, 1.f, 0.f));
	VertexColors.Add(FLinearColor(0.5f, 1.f, 0.5f));
	VertexColors.Add(FLinearColor(0.f, 1.f, 0.f));
	VertexColors.Add(FLinearColor(1.f, 1.f, 0.f));
	VertexColors.Add(FLinearColor(0.f, 1.f, 1.f));*/

	//context.meshManager->customMesh->CreateMeshSection(0, vertices, triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	//context->meshManager->createMeshAndGetId(this, FName("test"), vertices, triangles);
	// customMesh->section
	//customMesh->CreateMeshSection_LinearColor(0, vertices, triangles, TArray<FVector>(), TArray<FVector2D>(), VertexColors, TArray<FProcMeshTangent>(), true);
}

