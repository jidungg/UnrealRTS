// Copyright Epic Games, Inc. All Rights Reserved.

//using System.IO;
using UnrealBuildTool;

public class TestTopDown : ModuleRules
{
	public TestTopDown(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemSteam" });
        
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Steam", "lib", "steam_api64.lib"));
    }
}
