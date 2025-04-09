//

// Define this to build plugin as an engine plugin, even when build is executed from .uproject
// Todo: can remove code for IsEnginePlugin() == false (including CopyToProjectBinaries function)
#define FORCE_ENGINE_PLUGIN_BUILD

namespace UnrealBuildTool.Rules
{
	using System;
	using System.IO;
	using System.Collections.Generic;

	public class SDL : ModuleRules
	{
		// UE does not copy third party dlls to the output directory automatically.
		// Link statically so you don't have to do it manually.
		// to be delete private bool LinkThirdPartyStaticallyOnWindows = false;

		private bool IsEnginePlugin()
		{
#if FORCE_ENGINE_PLUGIN_BUILD
			return true;
#else
			return Path.GetFullPath(ModuleDirectory).Contains("\\Engine\\Plugins\\");
#endif
		}

		private string ModulePath
        {
            get { return ModuleDirectory; }
        }

        private string ThirdPartyPath
        {
            get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
        }

        private string BinariesPath
        {
            get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Binaries/")); }
        }

        public string GetUProjectPath()
        {
            return Path.Combine(ModuleDirectory, "../../../..");
        }

        private void CopyToProjectBinaries(string Filepath, ReadOnlyTargetRules Target)
        {
            System.Console.WriteLine("UE4 is: " + Path.GetFullPath(GetUProjectPath()));

            string BinariesDir = Path.Combine(GetUProjectPath(), "Binaries", Target.Platform.ToString());
            string Filename = Path.GetFileName(Filepath);

            //convert relative path 
            string FullBinariesDir = Path.GetFullPath(BinariesDir);

            if (!Directory.Exists(FullBinariesDir))
            {
                Directory.CreateDirectory(FullBinariesDir);
            }

            string FullBinaryFilepath = Path.Combine(FullBinariesDir, Filename);
            if (!File.Exists(FullBinaryFilepath))
            {
                System.Console.WriteLine("SDL: Copied from " + Filepath + ", to " + FullBinaryFilepath);
                File.Copy(Filepath, FullBinaryFilepath, true);
            }
        }

        public SDL(ReadOnlyTargetRules Target) : base(Target)
		{
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
 
            PublicDependencyModuleNames.AddRange(
				new string[]
				{
                    "Core",
                    "CoreUObject",
                    "Engine",
				});


            string LibraryPlatformFolder = string.Empty;
            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                LibraryPlatformFolder = "Win64";
            }
            else if (Target.Platform == UnrealTargetPlatform.Mac)
            {
                LibraryPlatformFolder = "osx";
            }
            else if (Target.Platform == UnrealTargetPlatform.Linux)
            {
                LibraryPlatformFolder = "linux64";
            }

            string IncludePath = Path.Combine(ThirdPartyPath, "SDL2", "Include");
            PublicIncludePaths.Add(IncludePath);

            string LibraryPath = Path.Combine(ThirdPartyPath, "SDL2", "Lib", LibraryPlatformFolder);
            string DLLPath = Path.Combine(ThirdPartyPath, "SDL2", "Bin", LibraryPlatformFolder);

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                //Lib
                PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "SDL2.lib"));

                //DLL
                string PluginDLLPath = Path.Combine(DLLPath, "SDL2.dll");

				if (IsEnginePlugin())
				{
					// Copy the dll next to the plugin
					RuntimeDependencies.Add("$(BinaryOutputDir)/SDL2.dll", PluginDLLPath);
				}
				else
				{
					System.Console.WriteLine("Project plugin detected, using dll at " + PluginDLLPath);

					// This copy DLL from Plugin\ThirdParty\SDL2\Bin\ to UE4Project\Binaries\Win64\
					CopyToProjectBinaries(PluginDLLPath, Target);

					string ProjectDLLFile = Path.GetFullPath(Path.Combine(GetUProjectPath(), "Binaries", LibraryPlatformFolder, "SDL2.dll"));
					// Request copy DLL from UE4Project\Binaries\Win64\ to UE4Build\Binaries\Win64\
					RuntimeDependencies.Add(ProjectDLLFile);
				}

                // Delay load DLLs - typically used for External(third party) modules
                // We need to delay the Lib in order to Build, otherwise it will lock SDL2.dll from the UE4Project\Binaries\Win64\
                PublicDelayLoadDLLs.Add("SDL2.dll");
            }
        }
	}

}
