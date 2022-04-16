// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DataBridgeInstance.generated.h"

/**
 *
 */
UCLASS()
class VOXELFRAMEUE4_API UDataBridgeInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "MyCharacter")
		float MyFloat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataBridgeInstance")
		bool IsServer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataBridgeInstance")
		bool TestUDataBridgeInstance;
	/*UFUNCTION(BlueprintImplementableEvent, Category = "DataBridgeInstance")
		void data_bridge_test(const FString& Message);*/



};
