# Compressonator

Compressonator is a set of tools to allow artists and developers to more easily create compressed texture image assets and easily visualize the quality impact of various compression technologies.  It consists of a GUI application, a command line application and an SDK for easy integration into a developer tool chain.

Compressonator DXTn (S3TC)is developed for Microsoft Windows® platform.

For more details goto the Compressonator Wiki: https://github.com/GPUOpen-Tools/Compressonator/wiki 

Recent additions and changes
------------------------------------------------------
v2.5 release Mar 20 2017 

Build Instructions for Windows 7 and up
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

-------------------------------------




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

