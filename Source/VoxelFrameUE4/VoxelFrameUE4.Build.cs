// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelFrameUE4 : ModuleRules
{
    public VoxelFrameUE4(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "ProceduralMeshComponent" });
    }
}
