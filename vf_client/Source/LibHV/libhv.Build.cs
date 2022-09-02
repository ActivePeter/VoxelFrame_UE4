// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

//version 1.2.4
public class LibHV : ModuleRules
{
    public LibHV(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core" });

        PrivateDependencyModuleNames.AddRange(new string[] { });
        PublicIncludePaths.Add(
               "./LibHV/include"
       );
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {

            //  第三方静态库的路径
            PublicLibraryPaths.Add("./LibHV/lib");

            //  第三方静态库的名称
            //PublicAdditionalLibraries.Add("libuv_msvc_x64.lib");
            //PublicAdditionalLibraries.Add("libuv.lib");
            PublicAdditionalLibraries.Add("hv_static.lib");
            //PublicAdditionalLibraries.Add("hv.lib");
        }
        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        bEnableShadowVariableWarnings = false;
        bEnableUndefinedIdentifierWarnings = false;
        bEnableExceptions = true;

        Definitions.Add("_CRT_SECURE_NO_WARNINGS");
    }
}
