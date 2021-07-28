// Copyright Epic Games, Inc. All Rights Reserved.

#include "VoxelFrameUE4GameMode.h"
#include "VoxelFrameUE4HUD.h"
#include "VoxelFrameUE4Character.h"
#include "UObject/ConstructorHelpers.h"

AVoxelFrameUE4GameMode::AVoxelFrameUE4GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AVoxelFrameUE4HUD::StaticClass();
}
