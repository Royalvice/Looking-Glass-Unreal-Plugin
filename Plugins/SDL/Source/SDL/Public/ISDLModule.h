#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "CoreMinimal.h"

#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"

class FSDLLoader
{
public:
	bool LoadDLL(const FString& PluginBaseDir)
	{
		bool bIsLoaded = false;

		FString PlatformName;
#if PLATFORM_WINDOWS
		PlatformName = "Win64";
#elif PLATFORM_LINUX
		PlatformName = "linux64";
#elif PLATFORM_MAC
		PlatformName = "osx";
#endif // PLATFORM_WINDOWS

#if PLATFORM_WINDOWS
		// Load DLL only in Editor, with build it will be copied to bin folder automaticly
		// It needed for build in editor
		const FString SDL2Dll = TEXT("SDL2.dll");

		// Pick the DLL from plugin's binaries directory (copied there from Build.cs using "RuntimeDependencies" function)
		FString DllPath = FPaths::Combine(PluginBaseDir, TEXT("Binaries"), PlatformName);

		FPlatformProcess::PushDllDirectory(*DllPath);
		FString DllFilePath = FPaths::Combine(DllPath, SDL2Dll);
		SDL2DLL = FPlatformProcess::GetDllHandle(*DllFilePath);
		FPlatformProcess::PopDllDirectory(*DllPath);

		bIsLoaded = SDL2DLL != nullptr;
#endif

		return bIsLoaded;
	}

	bool ReleaseDLL()
	{
		if (SDL2DLL != nullptr)
		{
			FPlatformProcess::FreeDllHandle(SDL2DLL);

			return true;
		}

		return false;
	}

protected:
	void* SDL2DLL = nullptr;
};


/**
 * The public interface to this module.  In most cases, this interface is only public to sibling modules
 * within this plugin.
 */
class ISDLModule : public IModuleInterface
{

public:

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline ISDLModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ISDLModule>("SDL");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("SDL");
	}
};

