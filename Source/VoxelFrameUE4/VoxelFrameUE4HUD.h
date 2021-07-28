// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VoxelFrameUE4HUD.generated.h"

UCLASS()
class AVoxelFrameUE4HUD : public AHUD
{
	GENERATED_BODY()

public:
	AVoxelFrameUE4HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

