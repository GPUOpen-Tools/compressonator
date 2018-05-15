# Compressonator

Compressonator is a set of tools to allow artists and developers to more easily create compressed texture assets or model mesh optimizations and easily visualize the quality impact of various compression and rendering technologies.  It consists of a GUI application, a command line application and an SDK for easy integration into a developer tool chain.

Compressonator supports Microsoft Windows®, Linux and Mac builds.

For more details goto the online Compressonator Documents: http://compressonator.readthedocs.io/en/latest/ 

Get Prebuilt Binaries and Installer here:
-----------------------------------------------------
https://github.com/GPUOpen-Tools/Compressonator/releases


## Contributors

Compressonator's GitHub repository (http://github.com/GPUOpen-Tools/Compressonator) is moderated by Advanced Micro Devices, Inc. as part of the GPUOpen initiative.

AMD encourages any and all contributors to submit changes, features, and bug fixes via Git pull requests to this repository.

Users are also encouraged to submit issues and feature requests via the repository's issue tracker.
  
Under development for 3.1 release
----------------------------------------------------
* Radeon Powered Compression with Compute and Packed Math Libraries for CPU and GPU

* Improvements to existing Compression Codecs
  * Fix for R & B channel swizzling
  * updated ETC Codec with support for SRGB, EAC and punchthrough Alpha
 
* CLI support for Model Mesh Compression and Model Optimization

* Model Mesh Compression 
  * Support for GLTF 2.0 compression extensions (KHR_draco_mesh_compression, ...)
    
* Selectable 3D Model viewers
  * Support for the Vulkan® and OpenGL® Viewer code introduced in 3.0
 
* 3D Model Image Diff viewer 
  * Selectable animation rates 
  * Manual mode to flip views
 

Recent 3.0 additions and changes
------------------------------------------------------

* Model Mesh Optimization for OBJ and GLTF 2.0 files
  * 3D triangle mesh optimization that improves on existing mesh preprocessing techniques
    * Vertex cache optimization
    * Overdraw optimization
    * Vertex prefetch cache optimization
    
* Model Mesh Compression for OBJ files 
  * Compresss models for faster download using Draco .drc files
    * Set quantization bits for POSITION, NORMAL, TEX_CORD and other GENERIC attributes
  
* DirectX® 12 3D Model viewer features and modifications
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
 
* 2D Image viewer update
  * Switch overlaid views of processed (compressed), Original and or Pixel Image Differance views with simple key stokes (O, P or D) and toggle the views at any time from Origin to Processed or Original to Image Diff using space bar

Gallery
-------
**2D Texture Compression**

![screenshot 1](https://github.com/GPUOpen-Tools/Compressonator/blob/master/docs/source/media/image51.png)

**glTF Model PBR Texture Compression**

![screenshot 2](https://github.com/GPUOpen-Tools/Compressonator/blob/master/docs/source/media/image96.png)


Build Instructions for Compressonator SDK
-----------------------------------------
Use the following project files to build binaries as needed for use in your own applications or sdk

* Texture Compression Codecs: Prebuilt Binaries for Compressonator VS2015 Libs are supplied when using the installer
\Compressonator\VS2015\CompressonatorLib.sln

This solution will create a build folder that contains DLL and Libs under \Compressonator\Build\VS2015\(configuration)\(platform)\
example: Compressonator\Build\VS2015\Release_MD\Win32\

* Mesh Optimization Lib: (Interfaces to this lib will change in future releases)
\Compressonator\VS2015\CMP_MeshOptimizer

* Mesh Compression  Lib: (Interfaces to this lib will change in future releases)
\Compressonator\VS2015\CMP_MeshCompressor


Build Instructions for Compressonator GUI and CLI applications on Windows 10 and up
------------------------------------------------------------------------------------

To build the applications you will need to have Visual Studio 2015 installed, Vulkan SDK version 1.0.68.0 from [Vulkan LunarG website](https://vulkan.lunarg.com/sdk/home), glslang.lib and spirv-tools.lib built from the Vulkan SDK installed. Qt V5.7 msvc2015 and msvc2015_64 opensource downloaded from [QT website](https://www.qt.io/download-open-source/) .

**Note**: Compressonator project solution files assume QT is installed in default path (C:\Qt\Qt5.7.1\5.7). and Vulkan SDK Environment variable (VULKAN_SDK) for path to the source code is set. Users must build the Vulkan SDK binaries prior to building the GUI and CLI applications. If you use different path, please change the project property accordingly in the  Compressonator_Root.props file.

The directory of your workspace should be as follows

- Common
- Compressonator
- LICENSE
- README.md


It is also recommended that you install and configure Visual Studio Qt5 Package extension from MSDN Visual Studio Gallery, and set in Qt Options dialog, Qt Default Version name V5.7 and path to default download path C:\Qt\Qt5.7.1\5.7\msvc2015_64\bin\

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
