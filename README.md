# Compressonator

Compressonator is a set of tools to allow artists and developers to more easily create compressed texture image assets and easily visualize the quality impact of various compression technologies.  It consists of a GUI application, a command line application and an SDK for easy integration into a developer tool chain.

Compressonator DXTn (S3TC)is developed for Microsoft Windows® platform.

Recent additions and changes since v2.3 release
------------------------------------------------------

Added ASTC speed setting according to Quality

Quality Setting
<0.2 = VeryFast  (default)
<0.4 = Fast
<0.6 = Medium
<0.8 = Thorough
else Exhaustive

Added Multithreaded ASTC block processing

Support mipmap generation on KTX output

Add TGA image plugin

Enable GUI decompress view with vulkan

Added GPU based compressed image views using OpenGL, DirectX 11 and Vulkan


Build Instructions for Windows 7 and up
------------------------------------------------------

To build the applications you will need to have Visual Studio 2015 installed.

The directory of your workspace should be as follows

- Common
- Compressonator
- LICENSE
- README.md


It is also recommended that you install and configure Visual Studio Qt5 Package extension from MSDN Visual Studio Gallery, and set in Qt Options dialog, Qt Default Version name V5.5 and path to Common\Lib\Ext\Qt\5.5\bin

https://visualstudiogallery.msdn.microsoft.com/c89ff880-8509-47a4-a262-e4fa07168408 

-------------------------------------




Build Applications using Batch Files
================================================================

Simply run one of the batch files from a window console.

To build the Compressonator GUI run BuildGUI
To build the Compressonator Command line Tool run BuildCLI



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
(C) Build the GUI Tool using
------------------------------------------------------------
\Compressonator\Applications\CompressonatorGUI\VS2015\VS2015.sln

This solution will create a build folder that contains a 
Compressonator.exe and a plugins folder under
\Compressonator\Build\VS2015\(configuration)\(platform)\

If building in Debug_MD configuration, please make sure the working directory is in \Compressonator\Build\VS2015\Debug_MD\(platform)\

The GUI tool has dependencies on Qt(V5.4 and up) and CXL - These file are copied from common & CLX folders to the Compressonator.exe folder when the project solution builds "CopyScript" project.

