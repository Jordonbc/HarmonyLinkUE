// Copyright (C) 2024 Jordon Brooks

using UnrealBuildTool;
using System.IO;
using Internal;

public class HarmonyLinkLib : ModuleRules
{
	public HarmonyLinkLib(ReadOnlyTargetRules Target) : base(Target)
	{
		//Console.WriteLine("Building HarmonyLinkLib");
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		Type = ModuleType.External;

		// Add the standard library
        //bUseRTTI = true;
        //bEnableExceptions = true;

        // Optionally, if you need C++17 features
        //CppStandard = CppStandardVersion.Cpp17;
        
		string includePath = Path.Combine(ModuleDirectory, "include");
        //Console.WriteLine("Include Path: " + includePath);
        PublicIncludePaths.Add(includePath);
        
		string platformString = Target.Platform.ToString();
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDefinitions.Add("HARMONYLINKLIB_STATIC=1");
			PublicDefinitions.Add("BUILD_WINDOWS=1");
			string libpath = Path.Combine(ModuleDirectory, "lib", platformString, "HarmonyLinkLibStatic.lib");

			PublicAdditionalLibraries.Add(libpath);

			//string dllPath = Path.Combine(ModuleDirectory, "bin", platformString, "HarmonyLinkLibShared.dll");
			//string dllTargetPath = "$(TargetOutputDir)/HarmonyLinkLibShared.dll";

			//Console.WriteLine("DLL Path: " + dllPath);
			//RuntimeDependencies.Add(dllTargetPath, dllPath);

			//PublicDelayLoadDLLs.Add("HarmonyLinkLibShared.dll");
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			PublicDefinitions.Add("HARMONYLINKLIB_STATIC=1");
			//Console.WriteLine("Building Linux");
			PublicDefinitions.Add("BUILD_LINUX=1");
			string dllPath = Path.Combine(ModuleDirectory, "lib", platformString, "libHarmonyLinkLibStatic_clang++.a");
            //string dllTargetPath = "$(TargetOutputDir)/libHarmonyLinkLibShared.so";
            //Console.WriteLine("Library Path: " + libPath);
            PublicAdditionalLibraries.Add(dllPath);

			// Ensure the proper linking of standard C++ libraries
            //PublicSystemLibraries.Add("stdc++");
            //PublicSystemLibraries.Add("c++abi");
            //PublicSystemLibraries.Add("m"); // Math library
            //PublicSystemLibraries.Add("pthread"); // POSIX threads library

            // Add the C++ standard library explicitly if needed
            PublicSystemLibraries.Add("c++");

            // Ensure linking with libc and other necessary libraries
            //PublicSystemLibraries.Add("dl"); // Dynamic linking loader
            //PublicSystemLibraries.Add("rt"); // Real-time library

            // Add any other libraries that might be needed
            //PublicSystemLibraries.Add("gcc_s"); // GCC support library
            //PublicSystemLibraries.Add("gcc"); // GCC compiler support library
        }
	}
}
