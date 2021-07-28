// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "game/GameContext.h"

#include "WorldActor.generated.h"
namespace VF
{
	template <typename IndexArrType>
	static void addOneTriangle2IndexArr(IndexArrType& arr, int index1, int index2, int index3)
	{
		arr.Add(index1);
		arr.Add(index2);
		arr.Add(index3);
	}
}



UCLASS()
class VOXELFRAMEUE4_API AWorldActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldActor();

	VF::GameContext context;
protected:
	/* The vertices of the mesh */
	TArray<FVector> vertices;
	/* The triangles of the mesh */
	TArray<int32> triangles;

	// UProceduralMeshComponent* customMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void generateCubeTest();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
