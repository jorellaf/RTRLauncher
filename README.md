# Launcher for Rome: Total Realism VIII
Official launcher for the [_Rome: Total Realism VIII_ (RTR8)](http://www.twcenter.net/forums/forumdisplay.php?26) modification for the _Rome: Total War - Alexander_ expansion.

---

### Install Instructions
Download the contents of the `release` folder and paste the _contents_ into your RTR8 installation root folder so that the `RTRLauncher.exe` file is located in `~/RomeTW-Alexander/RTR/launcher/RTRLauncher.exe`. There is a shortcut located inside the `release` folder which you can easily copy and paste it to your desktop or wherever you wish.

#### Requirements
* You need to have installed your copy of RTR8 inside the folder where your _Rome: Total War - Alexander_ expansion is located.
    * More specifically, the RTR8 path must be `~/RomeTW-Alex/RTR/`, where the executable for the _Rome: Total War - Alexander_ expansion is located in `~/RomeTW-Alexander/RomeTW-ALX.exe`. If the launcher does not find the executable file with that specific name, it will display an error.
* The folder for the mod must be named `RTR`. Any other name will cause the launcher to display an error (this behaviour will be modified in a future update).
* The launcher requires a valid `launcher.dat` file (UTF-8 encoded) to run successfully.
    * The default RTR8 `launcher.dat` file can be found at `release/launcher/launcher.dat`. The file contains instructions for the syntax.

### Features
* Run the _Rome: Total Realism VIII_ mod without the need of creating a shortcut with command line arguments manually.
* Select a number of technical settings when running the game (e.g. run the game windowed, disable sounds, etc).
    * Additional minor developer options, e.g. being able to manually or automattically check for changes in the campaign map files, and force the generation of an updated world file.
* Change many common in-game preferences without needing to alter the preferences file.
    * Automatic generation of a preferences file, in case one is missing.
    * If a preferences file exists, all the preferences will be maintained as they are unless you change them in the launcher interface.
    * Automatically increase the maximum resolution of the game to 8K UHD to allow the game to run better in larger screens.
* All options explained in detail through tooltips.

### Additional Credits
* The Rome: Total Realism VIII Logo: [lanjane](https://gitlab.rtrproject.com/Lanjane).
* The [Europa Barbarorum 1](https://www.europabarbarorum.com/EB1/) team's Trivial Script launcher for showing what a good Rome: Total mod launcher could be capable of.

### License
This software was created using Qt 5.12.2 and is licensed under the GNU General Public License version 3. See the license file for details.
