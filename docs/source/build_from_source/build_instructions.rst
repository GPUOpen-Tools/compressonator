Build from GitHub Sources
+++++++++++++++++++++++++

Compressonator comes with pre-built binaries for all of our stable releases: https://github.com/GPUOpen-Tools/Compressonator/releases 
which includes command line tools, GUI and binaries for use in developer applications.

The following build instructions are provided for developers interested in building the latest sources available on GitHub https://github.com/GPUOpen-Tools/Compressonator


Build Instructions for Compressonator SDK Libs
==============================================

Use the following project files to build binaries as needed for use in your own applications or link to your own SDK

Texture Compression Codecs 
--------------------------
Prebuilt Binaries for Compressonator VS2015 Libs are supplied when using the installer
"/Compressonator/VS2015/CompressonatorLib.sln"

This solution will create a build folder that contains DLL and Libs under \Compressonator\Build\VS2015\(configuration)\(platform)\
example: Compressonator/Build/VS2015/Release_MD/Win32\

Mesh Optimization Lib
---------------------
(Interfaces to this lib will change in future releases)
"/Compressonator/VS2015/CMP_MeshOptimizer"

Mesh Compression  Lib
---------------------
(Interfaces to this lib will change in future releases)
"/Compressonator/VS2015/CMP_MeshCompressor"


Build Instructions for Compressonator GUI and CLI applications on Windows 10 and up
===================================================================================

To build the applications you will need to have Visual Studio 2015 installed, Vulkan SDK version 1.0.68.0 from [Vulkan LunarG website](https://vulkan.lunarg.com/sdk/home), glslang.lib and spirv-tools.lib built from the Vulkan SDK installed. Qt V5.7 msvc2015 and msvc2015_64 opensource downloaded from [QT website](https://www.qt.io/download-open-source/) .

**Note**: Compressonator GUI project solution files assume QT is installed in default path (C:/Qt/Qt5.7.1/5.7). and Vulkan SDK Environment variable (VULKAN_SDK) for path to the source code is set. Users must build the Vulkan SDK binaries prior to building the GUI and CLI applications. If you use different path, please change the project property accordingly in the  Compressonator_Root.props file.

The directory of your workspace should be as follows

- Common
- Compressonator
- LICENSE
- README.md


It is also recommended that you install and configure Visual Studio Qt5 Package extension from MSDN Visual Studio Gallery, and set in Qt Options dialog, Qt Default Version name V5.7 and path to default download path C:\Qt\Qt5.7.1\5.7\msvc2015_64\bin\

https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408 

using Batch Files
=================

Simply run one of the batch files from a window console.

To build the Compressonator GUI run BuildGUI.

To build the Compressonator Command line Tool run BuildCLI.



using Visual Studio solution files
==================================

Go to the associated directory where Visual Studio Solution files are located as indicated below.

All of the Applications apart from the SDK libs can be built as either Release_MD or Debug_MD applications.

Also make sure that the default startup projects are set as follows:

CommandLine solution set to CompressonatorCLI
GUI solution set to MainApp

Always clean and rebuild the projects


Command line tool 
------------------

use: /Compressonator/Applications/CompressonatorCLI/VS2015/VS2015.sln

This solution will create a build folder that contains a 
CompressonatorCLI.exe and a new plugins folder under
/Compressonator/Build/VS2015/(configuration)/(platform)/

The command line tool has dependencies on the following:

Qt (V5.7 and up)
- Qt5Core.dll
- Qt5Gui.dll
- qt.conf

OpenGL
- glew32.dll
- libGLESv2.dll

OpenCV
- opencv_core249.dll
- opencv_imgproc249.dll
- opencv_highgui249.dll

Required only when using Vulkan plugin
- vulkan-1.dll 
- texture.vert.spv
- texture.frag.spv

Optional for image loading and saving
- qtga.dll
- qtiff.dll
 
The dlls above are copied to CompressonatorCLI.exe folder by "CopyFiles.bat" in the VS2015.sln.


GUI Tool 
--------
use: /Compressonator/Applications/CompressonatorGUI/VS2015/VS2015.sln

This solution will create a build folder that contains a 
Compressonator.exe and a plugins folder under
/Compressonator/Build/VS2015/(configuration)/(platform)\

If building in Debug_MD configuration, please make sure the working directory is in /Compressonator/Build/VS2015/Debug_MD/(platform)/

The GUI tool has dependencies on Qt(V5.7 and up) - These file are copied  to the Compressonator.exe folder when the project solution builds and run "CopyFiles.bat".


Build Instructions for Linux CompressonatorCLI command line application
=======================================================================

Note: For all unix build with this scripts, Do NOT move the shell/batch scripts provided (run in the same folder (Compressonator/Applications/CompressonatorCLI/Make/))

For Ubuntu build (only v14.04LTS and v16.04LTS build tested, WSL is tested as well):

run initsetup_ubuntu.sh to install the required packages for command line app:
- cmake at least 2.8.12
- gcc-6 and g++-6 (v6.3.0) 
- glew and opengl (libglew-dev libegl1-mesa-dev)
- qt5 at least 5.5.1
- openexr v2.2
- opencv v2.49
- Boost at least 1.59 (filesystem and system lib)
   
run buildCLI_ubuntu_cmake.sh to build (or rebuild) all the dependencies libraries with CompressonatorCLI executable generated in the same folder

Note: For more details on unix build, please refer to [readme.txt](https://github.com/GPUOpen-Tools/Compressonator/blob/master/Compressonator/Applications/CompressonatorCLI/Make/readme.txt)


Build Instructions for Linux Example command line application
=============================================================

To build the static Compressonator library or the Example command line applicatio under folder "Compressonator\Examples" you need to have the latest GCC **v6.3.0** toolchain installed (tested with 6.3.0 on WSL).

There are makefiles prepared for you.
static library: makefile location "Compressonator/Make/ExampleMake/", output in "Compressonator/Build/Linux/libcompressonator.a"
Example CLI: makefile location "Compressonator/Examples/Make", output in "Compressonator/Build/Linux/Example/cmpcli"

Example only take dds file as input. For more file format input, please try build CompressonatorCLI.
Usage for the Example CLI: ./cmpcli src.dds out.dds ATC_RGB 1
