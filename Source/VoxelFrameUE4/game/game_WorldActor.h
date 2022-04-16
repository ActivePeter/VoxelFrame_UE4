// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "game/GameContext.h"

#include "game_WorldActor.generated.h"
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
	std::unique_ptr<VF::GameContext> context;
	VF::UProceduralMeshComponentWithBind* custom_mesh;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "PreLoadResource")
		UMaterialInstance* block_mat_parent;
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Consistent")
		UTexture2D* block_texture;

protected:
	/* The vertices of the mesh */
	TArray<FVector> vertices;
	/* The triangles of the mesh */
	TArray<int32> triangles;

	// UProceduralMeshComponent* customMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void generateCubeTest();


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
