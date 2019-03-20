# Official SteamVR Unreal Plugin

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

