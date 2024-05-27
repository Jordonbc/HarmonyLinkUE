// Copyright (C) 2024 Jordon Brooks

using UnrealBuildTool;
using System.IO;
using Internal;

public class HarmonyLinkLib : ModuleRules
{
	public HarmonyLinkLib(ReadOnlyTargetRules Target) : base(Target)
	{
		Console.WriteLine("Building HarmonyLinkLib");
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		Type = ModuleType.External;

		// Add the standard library
        bUseRTTI = true;
        bEnableExceptions = true;

        // Optionally, if you need C++17 features
        CppStandard = CppStandardVersion.Cpp17;
        
		string includePath = Path.Combine(ModuleDirectory, "include");
        Console.WriteLine("Include Path: " + includePath);
        PublicIncludePaths.Add(includePath);

		PublicDefinitions.Add("HARMONYLINKLIB_STATIC=1");
        
		string platformString = Target.Platform.ToString();
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDefinitions.Add("BUILD_WINDOWS=1");
			string dllPath = Path.Combine(ModuleDirectory, "lib", platformString, "HarmonyLinkLibStatic.lib");
			Console.WriteLine("DLL Path: " + dllPath);
			PublicAdditionalLibraries.Add(dllPath);
			//RuntimeDependencies.Add(dllPath);
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			Console.WriteLine("Building Linux");
			PublicDefinitions.Add("BUILD_LINUX=1");
			string libPath = Path.Combine(ModuleDirectory, "bin", platformString, "libHarmonyLinkLibShared.so");
			Console.WriteLine("Library Path: " + libPath);
			PublicAdditionalLibraries.Add(libPath);
			
			// Add the C++ standard library explicitly
			//string toolchainLibPath = "E:/UnrealToolChains/v22_clang-16.0.6-centos7/x86_64-unknown-linux-gnu/lib";
			//PublicSystemLibraryPaths.Add(toolchainLibPath);
			RuntimeDependencies.Add(libPath);
		}
		// I shall include this if anyone wishes to provide Mac binaries of HarmonyLink but these are not included by default as I don't own one.
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicDefinitions.Add("BUILD_MACOS=1");
			string dynlibPath = Path.Combine(ModuleDirectory, "lib", platformString, "libHarmonyLinkLibStatic.a");
			Console.WriteLine("Dynamic Library Path: " + dynlibPath);
			RuntimeDependencies.Add(dynlibPath);
		}
	}
}
