# Compressonator

Compressonator is a set of tools to allow artists and developers to more easily create compressed texture image assets and easily visualize the quality impact of various compression technologies.  It consists of a GUI application, a command line application and an SDK for easy integration into a developer tool chain.

Compressonator DXTn (S3TC)is developed for Microsoft Windows® platform.

Recent additions
------------------------------------------------------

Installer for prebuilt SDK Libs, GUI and Command line application

Support for ASTC and ETC2
Variable sized compression buffers
GPU based compressed image views using OpenGL and DirectX 11


Build Instructions for Windows 7 and up
------------------------------------------------------

You will need the following:

1.	Visual Studio 2015 to build the applications and libs

The directory of your workspace should be as follows

- Common
- Compressonator
- LICENSE
- README.md

----------------------- 
A: Build the libs using
----------------------- 
\Compressonator\VS2015\CompressonatorLib.sln

This solution will create a build folder that contains DLL and Libs under \Compressonator\Build\VS2015\(configuration)\(platform)\


--------------------------------------
B: Build the Command line tool using 
--------------------------------------
\Compressonator\Applications\CompressonatorCLI\VS2015\VS2015.sln

This solution will create a build folder that contains a 
CompressonatorCLI.exe and a new plugins folder under
\Compressonator\Build\VS2015\(configuration)\(platform)\

The command line tool has dependencies on Qt (V5.4 and up)

- icuin54.dll
- icuuc54.dll
- icudt54.dll
- Qt5Core.dll
- Qt5Gui.dll
- libGLESv2.dll
- opencv_core249.dll
- opencv_imgproc249.dll
- opencv_highgui249.dll
- glew32.dll
- vulkan-1.dll

The dlls above are copied from common to CompressonatorCLI.exe folder by building the "CopyScript" project in the VS2015.sln.

-----------------------------------------------------------
C: Build the GUI Tool using
------------------------------------------------------------
\Compressonator\Applications\CompressonatorGUI\VS2015\VS2015.sln

This solution will create a build folder that contains a 
Compressonator.exe and a plugins folder under
\Compressonator\Build\VS2015\(configuration)\(platform)\

If building in Debug_MD configuration, please make sure the working directory is in \Compressonator\Build\VS2015\Debug_MD\(platform)\

The GUI tool has dependencies on Qt(V5.4 and up) and CXL - These file are copied from common & CLX folders to the Compressonator.exe folder when the project solution builds "CopyScript" project.

