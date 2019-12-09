// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class IslandMainEditor : ModuleRules
{
	public IslandMainEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                "IslandMainEditor/Public"
            });

        PrivateIncludePaths.AddRange(
            new string[] 
            {
				"IslandMainEditor/Private"
			});

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine","IslandMainProject"
            });
        PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
				"UnrealEd"
            });

        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {
            });

        DynamicallyLoadedModuleNames.AddRange(
            new string[] 
            {
            });

	}
}
