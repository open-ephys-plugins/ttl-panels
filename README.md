# TTL Panels

## Overview

This plugin offers a set of widgets to make TTL event debugging easier:

* A toggle panel manually generates TTL events.

* A display panel that shows the current state of TTL inputs.


## Installation

This plugin will soon be available via the Open Ephys GUI Plugin Installer. To access the Plugin Installer, press **ctrl-P** or **⌘P** from inside the GUI. Once the installer is loaded, browse to the "TTL Panels" plugin and click "Install."


## Development history

This plugin was originally developed by Christopher Thomas in the Womelsdorf Laboratory at Vanderbilt University. The original repository can be found at https://github.com/att-circ-contrl/PluginTTLDebug


## Usage

### Toggle Panel

* The toggle panel creates 32 buttons that can be toggled to change the state of TTL lines on each incoming data stream. There is also a text box for manually entering the next TTL word.

### Display Panel

* The display panel passively displays the state of 32 TTL lines on each incoming data stream, as well as the most recent TTL word.



## Building from source

First, follow the instructions on [this page](https://open-ephys.github.io/gui-docs/Developer-Guide/Compiling-the-GUI.html) to build the Open Ephys GUI.

Then, clone this repository into a directory at the same level as the `plugin-GUI`, e.g.:
 
```
Code
├── plugin-GUI
│   ├── Build
│   ├── Source
│   └── ...
├── OEPlugins
│   └── ttl-panels
│       ├── Build
│       ├── Source
│       └── ...
```

### Windows

**Requirements:** [Visual Studio](https://visualstudio.microsoft.com/) and [CMake](https://cmake.org/install/)

From the `Build` directory, enter:

```bash
cmake -G "Visual Studio 17 2022" -A x64 ..
```

Next, launch Visual Studio and open the `OE_PLUGIN_ttl-panels.sln` file that was just created. Select the appropriate configuration (Debug/Release) and build the solution.

Selecting the `INSTALL` project and manually building it will copy the `.dll` and any other required files into the GUI's `plugins` directory. The next time you launch the GUI from Visual Studio, the TTL Toggle Panel and TTL Display Panel plugins should be available.


### Linux

**Requirements:** [CMake](https://cmake.org/install/)

From the `Build` directory, enter:

```bash
cmake -G "Unix Makefiles" ..
cd Debug
make -j
make install
```

This will build the plugin and copy the `.so` file into the GUI's `plugins` directory. The next time you launch the GUI compiled version of the GUI, the TTL Toggle Panel and TTL Display Panel plugins should be available.


### macOS

**Requirements:** [Xcode](https://developer.apple.com/xcode/) and [CMake](https://cmake.org/install/)

From the `Build` directory, enter:

```bash
cmake -G "Xcode" ..
```

Next, launch Xcode and open the `ttl-panels.xcodeproj` file that now lives in the “Build” directory.

Running the `ALL_BUILD` scheme will compile the plugin; running the `INSTALL` scheme will install the `.bundle` file to `/Users/<username>/Library/Application Support/open-ephys/plugins-api8`. The TTL Toggle Panel and TTL Display Panel plugins should be available the next time you launch the GUI from Xcode.
