// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BladeZ : ModuleRules
{
	public BladeZ(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"BladeZ",
			"BladeZ/Variant_Platforming",
			"BladeZ/Variant_Platforming/Animation",
			"BladeZ/Variant_Combat",
			"BladeZ/Variant_Combat/AI",
			"BladeZ/Variant_Combat/Animation",
			"BladeZ/Variant_Combat/Gameplay",
			"BladeZ/Variant_Combat/Interfaces",
			"BladeZ/Variant_Combat/UI",
			"BladeZ/Variant_SideScrolling",
			"BladeZ/Variant_SideScrolling/AI",
			"BladeZ/Variant_SideScrolling/Gameplay",
			"BladeZ/Variant_SideScrolling/Interfaces",
			"BladeZ/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
