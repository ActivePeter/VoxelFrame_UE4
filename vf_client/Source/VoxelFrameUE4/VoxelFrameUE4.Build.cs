// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelFrameUE4 : ModuleRules
{
    public VoxelFrameUE4(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateIncludePaths.AddRange(
            new string[]
            {
                "./VoxelFrameUE4/paecslib/src",
                "./VoxelFrameUE4",
                "./VoxelFrameUE4/3rd"
            }
        );
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine", "InputCore",
                "HeadMountedDisplay", "ProceduralMeshComponent",
                "Sockets","Networking",
                
                //third party
                "Protobuf","LibHV",
            }
        );
    }
}
