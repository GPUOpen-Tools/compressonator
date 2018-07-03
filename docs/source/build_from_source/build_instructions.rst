Build from GitHub Sources
+++++++++++++++++++++++++

Compressonator comes with pre-built binaries for all of our stable releases: https://github.com/GPUOpen-Tools/Compressonator/releases 
which includes command line tools, GUI and binaries for use in developer applications.

The following build instructions are provided for developers interested in building the latest sources available on GitHub https://github.com/GPUOpen-Tools/Compressonator or from the source code downloaded from the releases page. 

**Note**: Git submodule contents in Compressonator/Common/Lib folder will not be automatically included in downloaded source code. You can either navigate to all the "url" listed in https://github.com/GPUOpen-Tools/Compressonator/blob/master/.gitmodules and download zip from each of the url and extract them into the Compressonator/Common/Lib folder OR if you have Git installed in your system, run the Git command "git clone --recursive https://github.com/GPUOpen-Tools/Compressonator.git" to get all the source code including the submodules contents.


Build Instructions for Compressonator SDK Libs
==============================================

Use the following project files to build binaries as needed for use in your own applications or link to your own SDK

Texture Compression Codecs 
--------------------------
Prebuilt Binaries for Compressonator VS2015 Libs are supplied when using the installer in https://github.com/GPUOpen-Tools/Compressonator/releases OR you can build from "/Compressonator/VS2015/CompressonatorLib.sln"

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

1. To build the applications you will need to have Visual Studio 2015 installed, Vulkan SDK version 1.0.68.0 from [Vulkan LunarG website](https://vulkan.lunarg.com/sdk/home). Only Vulkan SDK version 1.0.68.0 is tested in the build.

2. After Vulkan SDK installed, you will need to build glslang.lib, SPIRV.lib, HLSL.lib, OGLCompiler.lib, OSDependent.lib, SPVRemapper.lib, SPIRV-Tools-opt.lib, SPIRV-Tools.lib from the Vulkan SDK installed using all the CMakeList.txt scripts provided in the default installation path C:\\VulkanSDK\\1.0.68.0\\glslang and C:\\VulkanSDK\\1.0.68.0\\spirv-tools folders. 

3. To build the Vulkan-related libs in step 2, first download cmake tools from https://cmake.org/download/) , set the environment PATH to include <cmake_install_path>\\bin  which contains cmake.exe. You will need to download python 2.7 as well (from https://www.python.org/downloads/) for glslang and spirv-tools build later. 

4. Then, copy and paste the following commands (these commands are for x64bit build, for 32bit build, change the build folder name from VS2015_64 to VS2015) in a window batch file, save the batch file in the default installation path C:\\VulkanSDK\\1.0.68.0\\glslang and run the batch file.

set CurrDir=%CD%

for %%* in (.) do set CurrDirName=%%~nx*

IF EXIST %CurrDir%\VS2015_64 (rmdir VS2015_64 /s /q)

mkdir VS2015_64

cd VS2015_64

cmake -G "Visual Studio 14 2015 Win64" ..\\..\\%CurrDirName%

cd %CurrDir%

5. After the batch file run success (make sure there is no cmake errors or missing dependencies), open the glslang.sln file in the VS2015_64 folder with Microsoft Visual Studio 2015 and build the solution in Debug or Release mode (for Compressonator Debug_MD or Release_MD).

6. Repeat creating the same batch file in the default installation path C:\\VulkanSDK\\1.0.68.0\\spirv-tools and run the batch file in the path and then build the spirv-tools.sln file in the VS2015_64 or VS2015 folder.

**Note**: Compressonator GUI project expects these glslang libs and spirv-tools libs are built in the C:\\VulkanSDK\\1.0.68.0\\<glslang or spirv-tools>\\<VS2015_64 or VS2015> subfolders. If you have a different libs output paths, please remember to update all the paths macros "Compressonator_GLSLANG_" accordingly in the Compressonator/Compressonator_Root.props file.

7. The build will also requires QT V5.7 msvc2015 or/and msvc2015_64 opensource downloaded from [QT website](https://download.qt.io/official_releases/qt/5.7/5.7.1/) .

8. It is also recommended that you install and configure Visual Studio Qt5 Package extension from MSDN Visual Studio Gallery https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408 , and set in Qt Options dialog, Qt Default Version name V5.7 and path to default download path C:\\Qt\\Qt5.7.1\\5.7\\msvc2015_64\\bin\\

**Note**: Compressonator GUI project solution files assume QT is installed in default path (C:/Qt/Qt5.7.1/5.7). and Vulkan SDK Environment variable (VULKAN_SDK) for path to the source code is set. Users must build the Vulkan SDK binaries prior to building the GUI and CLI applications. If you use different path, please change the project property accordingly in the  Compressonator_Root.props file.

9. Also Compressonator Applications are using the Windows SDK version 10.0.15063.0. You can either install the required version of Windows SDK or change the SDK version to any Window 10 version in the project property pages or by right-clicking the solution and selecting "Retarget solution".

10. The directory of your workspace should be as follows

- Common
- Compressonator
- docs
- LICENSE
- README.md

using Batch Files
-----------------

After completing all the steps to install Vulkan SDK, Qt and projects set up as described above,

simply run one of the batch files from a window console:

To build the Compressonator GUI tool, run BuildGUI.bat.

To build the Compressonator Command line tool, run BuildCLI.bat.

OR

using Visual Studio solution files for:
---------------------------------------

After completing all the steps to install Vulkan SDK, Qt and projects set up as described above,

go to the associated directory where Visual Studio Solution files are located as indicated below.

All of the Applications apart from the SDK libs can be built as either Release_MD or Debug_MD applications.

Also make sure that the default startup projects are set as follows:

CompressonatorCLI VS2015.sln set to CompressonatorCLI project
CompressonatorGUI VS2015.sln set to MainApp project

Always clean and rebuild the projects. 

More details about:

Command line tool 
------------------

use: /Compressonator/Applications/CompressonatorCLI/VS2015/VS2015.sln

startup project set to CompressonatorCLI

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

startup project set to MainApp

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
