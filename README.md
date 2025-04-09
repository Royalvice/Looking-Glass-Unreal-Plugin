# Looking Glass Plugin for Unreal Engine

## Obtaining the source code

Actually, this is the standard procedure suitable for any Github repository. If you already did that before, it's unlikely that you'll find anything new in this section.

`git clone --recursive git@github.com:Looking-Glass/UnrealSDK.git`

For older git version

- `git clone git@github.com:Looking-Glass/UnrealSDK.git`
- `cd UnrealSDK`
- `git submodule init`
- `git submodule update`

Alternatively you may get the snapshot of the repository using downloading of zip file from Github instead of using `git clone`.

## How to build the plugin from the source code

- Pre-requisites: you should have Visual Studio 2019 installed.
- Download the project from Github (see above).
- Find the UnrealSDK.uproject file in the root folder of repository.
- If you have multiple Unreal engine versions installed, right click on this file, select "Switch unreal engine version" option and select the one which you want to use with this plugin.
- Right click on .uproject file again, choose "Generate Visual Studio project files". After this, you'll find UnrealSDK.sln file right next to the .uproject file.
- Open the .sln file (e.g. by double-clicking on it), you'll have Visual Studio started, with the solution file loaded.
- Now, you may build the project, or run it directly from the Visual Studio.

## How to use pre-built version of the plugin

- As a pre-requisite, you should have an existing project for Unreal engine.
- Download the latest release for your Unreal engine version [here](https://github.com/Looking-Glass/UnrealPlugin/releases)
- Create Plugins folder in your project (it may already exist if you have any project-level plugins).
- Create LookingGlass folder inside of Plugins. Actual folder name doesn't matter.
- Unzip the archive file with precompiled plugin into this folder.
- Now you may run your project. The LookingGlass plugin should be automatically enabled, unless if was explicitly disabled before, so there's no needs to enable it, as the engine always using all of the project-space plugins. You may see the "LookingGlass" button on the editor's toolbar.
- If the button didn't appear, go to the menu, Edit | Plugins | Project | 3D Screen | LookingGlass, and check the "Enabled" option. The engine will prompt to restart the editor after that.

## How to package the plugin

Packaging is needed for plugin's distribution accross different machines, e.g. providing builds over the Internet. The easiest way to package is to run `BuildPlugin.bat` file in the root folder of the project. This file will build and package the plugin for multiple engine versions, so if you don't need that - you may customize it to work with the version you needed.

Alternatively, there's a standard way of packaging the plugin from the editor. Just doing the same actions as for enabling the plugin (to open the plugin's page in engine settings): go to the menu, Edit | Plugins | Project | 3D Screen | LookingGlass. There you'll see the "Package" option, click on it. The editor will actomatically execute all needed actions and pass the plugin over the Visual Studio to build it.
