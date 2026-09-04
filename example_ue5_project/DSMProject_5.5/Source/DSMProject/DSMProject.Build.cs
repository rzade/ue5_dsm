// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DSMProject : ModuleRules
{
	public DSMProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG",
            "OnlineSubsystem",
            "Json",
            "JsonUtilities",
            "Http",
            "OnlineSubsystemUtils",
            "OnlineSubsystemNull",
            "ApplicationCore",
            "Slate",
            "SlateCore"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { 
            "Slate",
            "SlateCore"
        });

        PrivateDependencyModuleNames.Add("OnlineSubsystem");
    }
}
