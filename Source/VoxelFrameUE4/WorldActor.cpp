// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldActor.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AWorldActor::AWorldActor()
{
	context = std::make_shared<VF::GameContext>();
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	context->meshManager->customMesh = CreateDefaultSubobject<VF::UProceduralMeshComponentWithBind>("customMesh");
	//context->meshManager->customMesh->bUseAsyncCooking = false;
	//context->meshManager->customMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//context.chunkManager = std::make_shared<VF::ChunkManager>();
	context->worldActor = this;
}

// Called when the game starts or when spawned
void AWorldActor::BeginPlay()
{
	Super::BeginPlay();
	context->worldActorCallWhenBeginPlay();
	//generateCubeTest();
	//customMesh.bU
}

// Called every frame
void AWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	context->chunkManager->cookOneChunk();
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
	context->meshManager->createMeshAndGetId(this, FName("test"), vertices, triangles);
	// customMesh->section
	//customMesh->CreateMeshSection_LinearColor(0, vertices, triangles, TArray<FVector>(), TArray<FVector2D>(), VertexColors, TArray<FProcMeshTangent>(), true);
}
