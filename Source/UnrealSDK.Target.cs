// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealSDKTarget : TargetRules
{
    public UnrealSDKTarget(TargetInfo Target) : base(Target)
	{
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        WindowsPlatform.bStrictConformanceMode = true;
        CppStandard = CppStandardVersion.Cpp20;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        Type = TargetType.Game;
		bUsePCHFiles = false;
		ExtraModuleNames.Add("UnrealSDK");
    }
}
