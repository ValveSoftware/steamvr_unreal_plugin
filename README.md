# Official SteamVR Unreal Plugin

## I. How to add this plugin to your UE4 Project
Pre-requisite: Your project must be a C++ enabled project. 

1. In your project's root directory, create a new folder if it doesn't exist:
Plugins

2. Under the new Plugins directory, create a subfolder called (this name is arbitrary, and doesn't affect compilation or functioning of the plugin):
SteamVR

3. Add a git submodule to your project:

* repo link (https): https://github.com/ValveSoftware/steamvr_unreal_plugin.git
* repo link (ssh): git@github.com:ValveSoftware/steamvr_unreal_plugin.git

* Git Tower users, follow these instructions: https://www.git-tower.com/help/win/submodules/add-clone
* SourceTree users, follow these instructions: https://confluence.atlassian.com/sourcetreekb/adding-a-submodule-subtree-with-sourcetree-785332086.html
* Git command line users, follow instructions here: https://git-scm.com/book/en/v2/Git-Tools-Submodules

4. Right-click on your .uproject file and select "Generate Visual Studio project files" (this requires the Epci Launcher to be installed on your machine)

5. Open your project, UE, should ask for a recompile of all the plugin submodules, click OK.

Reminder: You need to update submodules separately from your regular Fetch/Pulls of your project to update the plugin.

