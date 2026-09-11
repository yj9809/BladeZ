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
			"NavigationSystem",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"Niagara",
			"GameplayCameras",
			"LevelSequence",
			"MovieScene",
			"LoadingScreenModule"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "EngineCameras" });

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
			"BladeZ/Variant_SideScrolling/UI",
		});

	}
}
