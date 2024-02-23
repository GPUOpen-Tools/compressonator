Build from GitHub Sources
+++++++++++++++++++++++++

The simplest way to get Compressonator is to use any of the pre-built binaries available on the GPUOpen-Tools GitHub page.
These include the command line tool, GUI application, and binaires for installing the SDK for developers.

This page will serve as a guide for the more advanced user who desires the flexibility of building Compressonator directly from the source code.

For either case, whether you want pre-built binaries or to download the source for the latest release of Compressonator, you can use
the following link:

https://github.com/GPUOpen-Tools/Compressonator/releases

While the releases page should provide you with a stable version of Compressonator to use, there are occasionally small updates made
between official releases that fix minor bugs. So if you need the absolute latest public source code, you can run the following command:

.. code-block:: console

    git clone --recursive https://github.com/GPUOpen-Tools/Compressonator.git


Prerequisites
==============================================

There are a few dependencies that must be installed before Compressonator can be built. The following are applicable for both 
Windows and Linux builds:

- CMake 3.15 or above
- Vulkan SDK version 1.2.141.2 or above is required
- Python 3.6 or above
- Qt 5.12.6 is recommended
- OpenCV 4.2.0 is recommended

Additionally, on Windows there are a couple of extra dependencies you will want to ensure are installed. The first is the Windows 10 SDK
version 10.0.19041.0 or later. You can typically download this through the Visual Studio Installer, but it can also be found via the 
`Windows SDK Archive`_.
Also, you will want to make sure that you are using toolset version 142 when compiling the code in Visual Studio. This is the default for
Visual Studio 2019, but for newer versions you will need to install the correct toolset.

.. _Windows SDK Archive: https://developer.microsoft.com/en-us/windows/downloads/sdk-archive/

Building the SDK and Other Libraries
==============================================

There are Visual Studio project files provided in the "compressonator/build_sdk" folder that can be used to build the SDK
(also called cmp_compressonatorlib), the Core, and the Framework.

There is a batch script provided in the "compressonator/build" folder named "windows_build_sdk.bat" if you'd prefer to run an automated
script to build all the SDK libraries. Be warned that this script builds all variations of the SDK libraries (in both Release and Debug modes),
so it can take quite a while to fully build.

The script can be called from the root Compressonator folder like:

.. code-block:: console

    call build/windows_build_sdk.bat

Building the GUI and CLI Applications
==============================================================

Similar to the SDK libraries, there is a simple script you can call on Windows that will setup the environment and start the builds
for the Compressonator applications. It does make some assumptions about the specific versions of Qt and the Vulkan SDK that are installed,
so it might not work without some tweaking.  The batch script is located at "build/windows_build_apps.bat".

If you'd prefer more customization, or wish to build from Linux, the steps to generate the projects and build the apps will be detailed
in the remainder of this section.

Preparing the Environment
--------------------------

**NOTE**: On Linux you will want to start by calling the "initsetup_ubuntu.sh" script in the "build" folder to download and install any
other dependencies you might be missing.

The first step is to execute the "fetch_dependencies.py" script located in the "build" folder, using a command like:

.. code-block:: console

    python fetch_dependencies.py

This will download all of the external libraries used by Compressonator into a "common" folder at the same level as the root Compressonator
folder.

Generating Project Files
--------------------------

The next step is to use CMake to generate the project files. But before you can do that a few environment variables need to be set.

There are a few libraries that Compressonator expects the user to install on their own and then tell Compressonator where to find them. Those libraries are: the Vulkan SDK, Qt5, and OpenCV. To let Compressonator know where to find these libraries we use environment variables. The variables are **VULKAN_DIR**, **QT_DIR**, and **OPENCV_DIR** respectively.

They can be set by doing running commands like the following:

*Windows* 

.. code-block:: console

    set VULKAN_DIR=C:\VulkanSDK\1.2.141.2\
    set QT_DIR=C:\Qt\Qt5.12.6\5.12.6\msvc2017_64\
    set OPENCV_DIR=C:\opencv\

*Linux*

.. code-block:: console

    export VULKAN_DIR=/opt/VulkanSDK/1.2.141.2/
    export QT_DIR=/opt/Qt/Qt5.9.2/5.9.2/gcc_64/
    export OPENCV_DIR=/opt/opencv/


With all of that said, you are now ready to run CMake and generate the project files. An example of the CMake command to run on both
Windows and Linux can be found below (run from the root compressonator folder that contains the CMakeLists.txt file):

*Windows*

.. code-block:: console

    cmake -G "Visual Studio 16 2019" .

If you are instead using Visual Studio 17 2022 (or newer) you will need to change the value passed to the -G option, you will also need
to explicitly set the toolset version used to v142 via the -T option ("-T v142").

*Linux*

.. code-block:: console

    cmake .

Building Compressonator
--------------------------

Assuming the CMake command finished without incident, everything is now set up to build Compressonator!

If you are on Windows you can open the "compressonator.sln" file and build using Visual Studio. If you'd rather use the command line, or
are running Linux, the commands to build would look something like:

*Windows*

.. code-block:: console

    msbuild /m:4 /t:build /p:Configuration=release /p:Platform=x64 compressonator.sln

*Linux*

.. code-block:: console

    make

Optional Build Settings
==============================================

Compressonator provides many optional flags you can use to customize your build when running the CMake command. You can check the root
CMakeLists.txt file for everything that exists, but in this section we will point out a few that might be of interest.

Some of these will let you choose to build only certain parts of the project, while others will allow you to reenable features that are
no longer officially supported and are turned off by default.

- **OPTION_ENABLE_ALL_APPS** Allows you to enable or disable building of the CLI and GUI applications. This is ON by default.
- **OPTION_BUILD_APPS_CMP_CLI** Enable only the CLI application for building.
- **OPTION_BUILD_APPS_CMP_GUI** Enable only the GUI application for building.
- **OPTION_BUILD_DRACO** Enable using the Draco library for compressing and decompressing 3D meshes. This is OFF by default.
- **OPTION_BUILD_ASTC** Enable the ASTC codec. This is OFF by default.

Building the Documentation
==============================================================

The documentation is written using the reStructuredText_ markup syntax. There are batch files and scripts provided that will run commands
to build the documentation into HTML pages for easy viewing. These scripts use Sphinx_ to build the documentation, so you must install
that first before you can use them.

.. _reStructuredText: https://docutils.sourceforge.io/rst.html
.. _Sphinx: https://www.sphinx-doc.org/en/master/usage/installation.html

Once Sphinx is installed, you can run the following from the root Compressonator folder for Windows:

.. code-block:: console

    call build/windows_build_docs.bat

For Linux users you need to call the make files directly in the "compressonator/docs/" folder, something like:

.. code-block:: console

    set -x
    cd compressonator/docs
    make -j 4 clean
    make -j 4 html