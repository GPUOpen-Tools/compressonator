set BUILD_QTDIR=%1
set BUILD_OPENCV_DIR=%2
set BUILD_OUTDIR=%3
set BUILD_SOLUTIONDIR=%4
set BUILD_GLEWDIR=%5
set BUILD_VULKANDIR=%6
set COMPUTE=%7
set PLATFORM=%8
set _DEBUG=%9

echo ---------------------------------------
echo QT      : [%BUILD_QTDIR%]
echo OPENCV  : [%BUILD_OPENCV_DIR%]
echo OUTPUT  : [%BUILD_OUTDIR%]
echo SOLUTION: [%BUILD_SOLUTIONDIR%]
echo DBG/REL : [%_DEBUG%]
echo COMPUTE : [%COMPUTE%]
echo PLATFORM : [%PLATFORM%]
echo ---------------------------------------

echo on

XCopy /r /d /y ..\qt.conf %BUILD_OUTDIR%


XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_core249%_DEBUG%.dll"            %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_imgproc249%_DEBUG%.dll"         %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_highgui249%_DEBUG%.dll"         %BUILD_OUTDIR%

XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\Header\GPU_Decode\VK_ComputeShader\texture.vert.spv"       %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\Header\GPU_Decode\VK_ComputeShader\texture.frag.spv"       %BUILD_OUTDIR%
XCopy /r /d /y "%COMPUTE%\*.*"                                           %BUILD_OUTDIR%\Compute\


IF NOT EXIST %BUILD_OUTDIR%plugins mkdir %BUILD_OUTDIR%plugins 

IF "%PLATFORM%" == "Win32" goto :copy32

IF "%PLATFORM%" == "x64" goto :copy64

:copy32
XCopy /r /d /y "%BUILD_QTDIR%msvc2015\bin\libGLESv2%_DEBUG%.dll"            %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%msvc2015\bin\Qt5Core%_DEBUG%.dll"              %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%msvc2015\bin\Qt5Gui%_DEBUG%.dll"               %BUILD_OUTDIR%

IF NOT EXIST %BUILD_OUTDIR%plugins\imageformats mkdir %BUILD_OUTDIR%plugins\imageformats
XCopy /r /d /y "%BUILD_QTDIR%msvc2015\plugins\imageformats\qtga%_DEBUG%.dll"  %BUILD_OUTDIR%plugins\imageformats\
XCopy /r /d /y "%BUILD_QTDIR%msvc2015\plugins\imageformats\qtiff%_DEBUG%.dll" %BUILD_OUTDIR%plugins\imageformats\

IF NOT EXIST %BUILD_OUTDIR%plugins\platforms mkdir %BUILD_OUTDIR%plugins\platforms
XCopy /r /d /y "%BUILD_QTDIR%msvc2015\plugins\platforms\qwindows%_DEBUG%.dll" %BUILD_OUTDIR%plugins\platforms\

XCopy /r /d /y "%BUILD_GLEWDIR%x86\glew32.dll"                               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_VULKANDIR%lib32\vulkan-1.dll"                         %BUILD_OUTDIR%
goto :copyend

:copy64
XCopy /r /d /y "%BUILD_QTDIR%msvc2015_64\bin\libGLESv2%_DEBUG%.dll"            %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%msvc2015_64\bin\Qt5Core%_DEBUG%.dll"              %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%msvc2015_64\bin\Qt5Gui%_DEBUG%.dll"               %BUILD_OUTDIR%

IF NOT EXIST %BUILD_OUTDIR%plugins\imageformats mkdir %BUILD_OUTDIR%plugins\imageformats
XCopy /r /d /y "%BUILD_QTDIR%msvc2015_64\plugins\imageformats\qtga%_DEBUG%.dll"  %BUILD_OUTDIR%plugins\imageformats\
XCopy /r /d /y "%BUILD_QTDIR%msvc2015_64\plugins\imageformats\qtiff%_DEBUG%.dll" %BUILD_OUTDIR%plugins\imageformats\

IF NOT EXIST %BUILD_OUTDIR%plugins\platforms mkdir %BUILD_OUTDIR%plugins\platforms
XCopy /r /d /y "%BUILD_QTDIR%msvc2015_64\plugins\platforms\qwindows%_DEBUG%.dll" %BUILD_OUTDIR%plugins\platforms\

XCopy /r /d /y "%BUILD_GLEWDIR%x64\glew32.dll"                               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_VULKANDIR%lib\vulkan-1.dll"                           %BUILD_OUTDIR%
goto :copyend

:copyend
echo "DLL copied done"