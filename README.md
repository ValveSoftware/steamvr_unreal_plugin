# SteamVR Input for Unreal 1.5-sdk1.5.17

| Branch | Notes | Download |
| ------------- | ------------- | ------------- |
| [develop](https://github.com/ValveSoftware/steamvr_unreal_plugin) | Active development branch using UE 4.15, may be unstable | N.A. |
| [4.22](https://github.com/ValveSoftware/steamvr_unreal_plugin/tree/4.22) | For projects using UE 4.22 | [Zip](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.22.zip) <br> [Tarball](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.22.tar.gz) |
| [4.21](https://github.com/ValveSoftware/steamvr_unreal_plugin/tree/4.21) | For projects using UE 4.21 | [Zip](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.21.zip) <br> [Tarball](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.21.tar.gz) |
| [4.20](https://github.com/ValveSoftware/steamvr_unreal_plugin/tree/4.20) | For projects using UE 4.20 | [Zip](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.20.zip) <br> [Tarball](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.20.tar.gz) |
| [4.19](https://github.com/ValveSoftware/steamvr_unreal_plugin/tree/4.19) | For projects using UE 4.19 | [Zip](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.19.zip) <br> [Tarball](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.19.tar.gz) |
| [4.18](https://github.com/ValveSoftware/steamvr_unreal_plugin/tree/4.18) | For projects using UE 4.18 | [Zip](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.18.zip) <br> [Tarball](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.18.tar.gz) |
| [4.17](https://github.com/ValveSoftware/steamvr_unreal_plugin/tree/4.17) | For projects using UE 4.17 | [Zip](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.17.zip) <br> [Tarball](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.17.tar.gz) |
| [4.16](https://github.com/ValveSoftware/steamvr_unreal_plugin/tree/4.16) | For projects using UE 4.16 | [Zip](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.16.zip) <br> [Tarball](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.16.tar.gz) |
| [4.15](https://github.com/ValveSoftware/steamvr_unreal_plugin/tree/4.15) | For projects using UE 4.15 | [Zip](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.15.zip) <br> [Tarball](https://github.com/ValveSoftware/steamvr_unreal_plugin/archive/v1.5_4.15.tar.gz) |


## I. Setting up a new project for use with the SteamVR Input Plugin
If you are not adding the plugin to an existing project, create a new Blueprint project using the Virtual Reality template.  Starter content is not necessary.  

For Blueprint projects to use the plugin directly from GitHub, you need to convert it to a C++ project.  To convert it:
* Select to File -> New C++ Class
* In the Choose Parent Class dialog, choose Game Mode Base
* Give the class a name and click Create.  The engine will compile the code and open the solution in Visual Studio
* Close Visual Studio and the Unreal Editor
* Add the SteamVR Input plugin using the steps below


## I. How to add this plugin to your UE4 Project
1. Download the SteamVR Input plugin from GitHub
	* Go to https://github.com/ValveSoftware/steamvr_unreal_plugin
	* Click the "Clone or download" and select Download Zip
	
2. In your project's root directory, create a new folder if it doesn't exist:
Plugins

3. Unzip the downloaded file directly to the new Plugins folder.  There should now be a folder called steamvr_unreal_plugin-master; you can rename this if you'd like.  

5. In the root folder of your project, right-click on your .uproject file and select "Generate Visual Studio project files" (this requires the Epic Launcher to be installed on your machine)

6. Finally, double-click the *.uproject file at the root of your project to compile and open the project.  If you are prompted to rebuild any files select yes

Reminder: You need to update submodules separately from your regular Fetch/Pulls of your project to update the plugin.
