// Copyright Amr Hamed

using UnrealBuildTool;
using System.Collections.Generic;

public class SkateboardingSimTarget : TargetRules
{
	public SkateboardingSimTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "SkateboardingSim" } );
	}
}
