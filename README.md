# Compressonator

Compressonator is a set of tools to allow artists and developers to more easily create compressed texture assets or model mesh optimizations and easily visualize the quality impact of various compression and rendering technologies.  It consists of a GUI application, a command line application and an SDK for easy integration into a developer tool chain.

Compressonator supports Microsoft Windows®, Linux and Mac builds.

For more details goto the Compressonator Wiki: https://github.com/GPUOpen-Tools/Compressonator/wiki 

Get Prebuilt Binaries and Installer here:
-----------------------------------------------------
https://github.com/GPUOpen-Tools/Compressonator/releases


## Contributors

Compressonator's GitHub repository (http://github.com/GPUOpen-Tools/Compressonator) is moderated by Advanced Micro Devices, Inc. as part of the GPUOpen initiative.

AMD encourages any and all contributors to submit changes, features, and bug fixes via Git pull requests to this repository.

Users are also encouraged to submit issues and feature requests via the repository's issue tracker.
  
Under development 
----------------------------------------------------
* Improvements to existing Compression Codecs
  * Fix for R & B channel swizzling

* Model Mesh Optimization
  * 3D triangle mesh optimization that improves on existing mesh preprocessing techniques
    * Vertex cache optimization
    * Overdraw optimization
    * Vertex prefetch cache optimization
    
* Model Mesh Compression using draco 
  * Compresss model for faster download 
    * Force mesh to be encoded as point cloud for processing
    * Set quantization bits for POSITION, NORMAL, TEX_CORD and other GENERIC attributes
    * Fast Decode 
    * see https://github.com/google/draco for more details on how the compression works
  
* 3D Model viewer features and modifications
  * Selectable 3D Model views with DX12, Vulkan and OpenGL
  * Multi GPU Node usage stats
  * Real time GPU profiler chart with Total GPU Time stats
  * Animation (on/off)
  * Glow
  * Spot lighting 
  * Shadow maps
  * Selection of tone mapper ("Timothy", "DX11DSK", "Reinhard", "Uncharted2Tonemap")
  * Exposure setting (0.0 to 1.0)
  * iblFactor (0.0 to 2.0)
  * Show Bounding Boxes
  * Show Sky dome
  * Fix Aspect Ratio
  * Wire mesh views
 
* 3D Model Image Diff viewer 
  * Selectable animation rates 
  * Manual mode to flip views
  * Option to enable or disable Textures rendering
 
  
 
 

Recent additions and changes
------------------------------------------------------
v2.7 release Nov 23 2017 https://github.com/GPUOpen-Tools/Compressonator/releases

* **Linux and Mac support**
  * updated  [builds](https://github.com/GPUOpen-Tools/Compressonator#build-instructions-for-linux-compressonatorcli-command-line-application) to use cmake and scripts

* **Preview of 3D Model viewer and asset compression**
  * View 3D Models with DX12 using glTF v2.0 file format https://www.khronos.org/gltf
  * Compress model textures
  * Image diff compressed model textures with original
  * Realtime 3D Model diff views

Gallery
-------
**2D Texture Compression**

![screenshot 1](https://github.com/GPUOpen-Tools/Compressonator/wiki/assets/compressonator_properties_view.jpg)

**glTF Model PBR Texture Compression**

![screenshot 2](https://github.com/GPUOpen-Tools/Compressonator/wiki/assets/gltfs3a.png)



Build Instructions for Windows 10 and up
------------------------------------------------------

To build the applications you will need to have Visual Studio 2015 installed and Qt V5.7 msvc2015 and msvc2015_64 opensource downloaded from [QT website](https://www.qt.io/download-open-source/) .

**Note**: The solution project assume QT is installed in default path (C:\Qt\5.7). If you use different path, please change the project property accordingly in the  Compressonator_Root.props file. 

The directory of your workspace should be as follows

- Common
- Compressonator
- LICENSE
- README.md


It is also recommended that you install and configure Visual Studio Qt5 Package extension from MSDN Visual Studio Gallery, and set in Qt Options dialog, Qt Default Version name V5.7 and path to default download path C:\Qt\5.7\msvc2015_64\bin\

https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408 

Build Applications using Batch Files
================================================================

Simply run one of the batch files from a window console.

To build the Compressonator GUI run BuildGUI.

To build the Compressonator Command line Tool run BuildCLI.



Manual Build Steps
================================================================

Go to the associated directory where Visual Studio Solution Files are located as indicated in steps A through C.

All of the Applications (B, C) apart from the SDK libs (A) can be built as either Release_MD or Debug_MD 32 Bit applications.


For Application builds make sure that the default startup projects are set as follows:

(B) CommandLine solution set to CompressonatorCLI
(C) GUI solution set to MainApp

Always clean and rebuild the projects

---------------------------- 
(A) Build the SDK libs using
---------------------------- 
\Compressonator\VS2015\CompressonatorLib.sln

This solution will create a build folder that contains DLL and Libs under \Compressonator\Build\VS2015\(configuration)\(platform)\

example: Compressonator\Build\VS2015\Release_MD\Win32\


--------------------------------------
(B) Build the Command line tool using 
--------------------------------------
\Compressonator\Applications\CompressonatorCLI\VS2015\VS2015.sln

This solution will create a build folder that contains a 
CompressonatorCLI.exe and a new plugins folder under
\Compressonator\Build\VS2015\(configuration)\(platform)\

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

-----------------------------------------------------------
(C) Build the GUI Tool using
------------------------------------------------------------
\Compressonator\Applications\CompressonatorGUI\VS2015\VS2015.sln

This solution will create a build folder that contains a 
Compressonator.exe and a plugins folder under
\Compressonator\Build\VS2015\(configuration)\(platform)\

If building in Debug_MD configuration, please make sure the working directory is in \Compressonator\Build\VS2015\Debug_MD\(platform)\

The GUI tool has dependencies on Qt(V5.7 and up) - These file are copied  to the Compressonator.exe folder when the project solution builds and run "CopyFiles.bat".

-----------------------------------------------------------
Build Instructions for Linux CompressonatorCLI command line application
-----------------------------------------------------------

**Note**: For all unix build with this scripts, Do NOT move the shell/batch scripts provided (run in the same folder (Compressonator/Applications/CompressonatorCLI/Make/))

For Ubuntu build (****only v14.04LTS and v16.04LTS build tested, WSL is tested as well):

run initsetup_ubuntu.sh to install the required packages for command line app:
- cmake at least 2.8.12
- gcc-6 and g++-6 (v6.3.0) 
- glew and opengl (libglew-dev libegl1-mesa-dev)
- qt5 at least 5.5.1
- openexr v2.2
- opencv v2.49
- Boost at least 1.59 (filesystem and system lib)
   
run buildCLI_ubuntu_cmake.sh to build (or rebuild) all the dependencies libraries with CompressonatorCLI executable generated in the same folder

**Note**: For more details on unix build, please refer to [readme.txt](https://github.com/GPUOpen-Tools/Compressonator/blob/master/Compressonator/Applications/CompressonatorCLI/Make/readme.txt)


Build Instructions for Linux Example command line application
------------------------------------------------------

To build the static Compressonator library or the Example command line applicatio under folder "Compressonator\Examples" you need to have the latest GCC **v6.3.0** toolchain installed (tested with 6.3.0 on WSL).

There are makefiles prepared for you.
* static library: makefile location "Compressonator/Make/ExampleMake/", output in "Compressonator/Build/Linux/libcompressonator.a"
* Example CLI: makefile location "Compressonator/Examples/Make", output in "Compressonator/Build/Linux/Example/cmpcli"

Example only take dds file as input. For more file format input, please try build CompressonatorCLI.
Usage for the Example CLI: ./cmpcli src.dds out.dds ATC_RGB 1

-------------------------------------
