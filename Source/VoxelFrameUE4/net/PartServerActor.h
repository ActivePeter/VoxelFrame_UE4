// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VF_Base.h"
#include "vf_NetworkManager.h"

#include "PartServerActor.generated.h"

UCLASS()
class VOXELFRAMEUE4_API APartServerActor : public AActor
{
	GENERATED_BODY()
		///////////////////////////////
		// data
protected:
	std::shared_ptr<VF::NetworkManager> network_manager;

	///////////////////////////////
	// func
public:
	// Sets default values for this actor's properties
	APartServerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
