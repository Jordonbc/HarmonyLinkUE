// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class HarmonyLink : ModuleRules
{
	public HarmonyLink(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				"ThirdParty/HarmonyLinkLib/include"
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
                "Engine",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		
		// Platform-specific settings
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add(Path.Combine(PluginDirectory, "Source/ThirdParty/HarmonyLinkLib/bin/Win64/HarmonyLinkLib.lib"));
			RuntimeDependencies.Add("$(BinaryOutputDir)/HarmonyLinkLib.dll", Path.Combine(PluginDirectory, "Source/ThirdParty/HarmonyLinkLib/bin/Win64/HarmonyLinkLib.dll"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			PublicAdditionalLibraries.Add(Path.Combine(PluginDirectory, "Source/ThirdParty/HarmonyLinkLib/bin/Linux/libHarmonyLinkLib.so"));
			RuntimeDependencies.Add("$(BinaryOutputDir)/libHarmonyLinkLib.so", Path.Combine(PluginDirectory, "Source/ThirdParty/HarmonyLinkLib/bin/Linux/libHarmonyLinkLib.so"));
		}
		
		// I shall include this if anyone wishes to provide Mac binaries of HarmonyLink but these are not included by default as I don't own one.
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicAdditionalLibraries.Add(Path.Combine(PluginDirectory, "Source/ThirdParty/HarmonyLinkLib/bin/Mac/HarmonyLinkLib.dylib"));
			RuntimeDependencies.Add("$(BinaryOutputDir)/HarmonyLinkLib.dylib", Path.Combine(PluginDirectory, "Source/ThirdParty/HarmonyLinkLib/bin/Mac/HarmonyLinkLib.dylib"));
		}
	}
}
