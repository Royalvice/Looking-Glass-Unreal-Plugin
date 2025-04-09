// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealSDKEditorTarget : TargetRules
{
    public UnrealSDKEditorTarget(TargetInfo Target) : base(Target)
	{
#if UE_5_4_OR_LATER
		DefaultBuildSettings = BuildSettingsVersion.Latest;
        WindowsPlatform.bStrictConformanceMode = true;
        CppStandard = CppStandardVersion.Cpp20;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#else
		DefaultBuildSettings = BuildSettingsVersion.V2;
#endif
        Type = TargetType.Editor;
		bUsePCHFiles = false;
		ExtraModuleNames.Add("UnrealSDK");
	}
}
