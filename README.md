
# Qt Creator Klocwork Plugin

This plugin provides the ability to perform analyses of your code within Qt Creator using the Klocwork static code analyser.
Klocwork is a commercial static analyser by Rogue Wave Software that detects a wide range of vulnerabilities, both through its interprocedural control and data flow engine and XPath-like semantic pattern checks.
Essentially, you can detect runtime bugs like null pointer dereferences, uninitialised variables, memory leaks, buffer overflows, as well as comply with industry standards such as MISRA C/C++.

[More information on Klocwork](http://www.klocwork.com/products-services/klocwork)

[More information on Rogue Wave Software](http://www.roguewave.com/products-services)

## Installation

1. Edit klocwork.pro and set the QTCREATOR_SOURCES and IDE_BUILD_TREE variables. QTCREATOR_SOURCES should point to the trunk of the Qt Creator source code, and IDE_BUILD_TREE should point to the directory that these sources were built in.
2. Run "qmake -r" from the plugin source directory
3. Run "make"
4. Launch Qt Creator from IDE_BUILD_TREE and the plugin should be installed.

Alternatively, follow the above steps and then copy the libKlocwork.xxx library file to the plugins library directory for an existing version of Qt Creator. Note that you need to be using compatible versions of Qt Creator.

Built and tested against Qt Creator version 3.8.41 and Qt version 5.4.2

## Usage

1. Install the Klocwork Command Line Tools (get these from Rogue Wave / local partner)
2. In Qt Creator, go to the IDE Preferences page and select the Analyser page and the Klocwork tab. Enter the kwcheck binary location and the license server/port. [Optional] Enable Continuous Analysis.
3. Go to the Projects page and select the Klocwork tab for your project. Enter here the build specification location (see below for more information), the Klocwork server project/host/port
4. Hit Ctrl+Alt+Meta+K to analyse the currently selected file, or go to Tools->Klocwork->Analyse Current File

### Generating a Klocwork Build Specification File

[Document for Eclipse (setup is similar)](https://developer.klocwork.com/documentation/klocwork/en/current/getting-started-klocwork-desktop-cc-plugin-eclipse#Captureyourbuildsettings)

In a nutshell, Klocwork uses a build specification to capture the necessary information about how to build a project. It does this by capturing your build command and evnironment.
Hence, if you run "make" as your build command, prepend this with "kwinject" and Klocwork will listen to your build command and generate a build specification. The result being

<klocwork command line tools>/bin/kwinject -u [-o <build_specificaiton_location>] make                // or any other build command

The [-o <build_specificaiton_location>] is optional. Default is "kwinject.out". Specify this file to the Klocwork plugin as per step 3 above under Usage.

Hint: you do not need to build your entire project - the "-u" switch tells kwinject to update, so that each time you build (presumably incrementally) Klocwork will keep adding knowledge about you build to the build specification.
What this does mean, is that in order for Klocwork to analyse a file, that file's compilation/linkage must have been captured with kwinject and present in the build specificaiton.

## Copyright / License

Copyright (C) 2015 Emenda Nordic AB
Author: Jacob Lärfors / jacob.larfors@emenda.eu

GNU Lesser General Public License Usage
This software may be used under the terms of the GNU Lesser General Public License version 2.1 or version 3 as published by the Free Software Foundation and appearing in the file LICENSE.LGPLv21 and LICENSE.LGPLv3 included in the packaging of this file.
Please review the following information to ensure the GNU Lesser General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html and http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
