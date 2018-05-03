set BUILD_QTDIR=%1
set BUILD_OPENCV_DIR=%2
set BUILD_OUTDIR=%3
set BUILD_SOLUTIONDIR=%4
set BUILD_GLEWDIR=%5
set BUILD_VULKANDIR=%6
set BUILD_AGSDIR=%7
set _DEBUG=%8

echo ---------------------------------------
echo QT      : [%BUILD_QTDIR%]
echo OPENCV  : [%BUILD_OPENCV_DIR%]
echo OUTPUT  : [%BUILD_OUTDIR%]
echo SOLUTION: [%BUILD_SOLUTIONDIR%]
echo DBG/REL : [%_DEBUG%]
echo ---------------------------------------

echo on

XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\Documents\UserGuide.chm"       %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\Documents\GettingStarted.chm"  %BUILD_OUTDIR%

IF NOT EXIST %BUILD_OUTDIR%Projects mkdir %BUILD_OUTDIR%Projects
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\RunTime\BC7_Compression.cprj"  %BUILD_OUTDIR%Projects\
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\RunTime\BC6H_Compression.cprj" %BUILD_OUTDIR%Projects\

IF NOT EXIST %BUILD_OUTDIR%Images   mkdir %BUILD_OUTDIR%Images
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\RunTime\images\Balls.exr"  %BUILD_OUTDIR%Images\
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\RunTime\images\Boat.png"   %BUILD_OUTDIR%Images\
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\RunTime\images\Ruby2.bmp"  %BUILD_OUTDIR%Images\

XCopy /r /d /y .\qt.conf %BUILD_OUTDIR%

XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_core249%_DEBUG%.dll"    %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_imgproc249%_DEBUG%.dll" %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_highgui249%_DEBUG%.dll" %BUILD_OUTDIR%

XCopy /r /d /y "%BUILD_GLEWDIR%glew32.dll"                               %BUILD_OUTDIR%

XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\Applications\_Plugins\CGPUDecode\Vulkan\VK_ComputeShader\texture.vert.spv"       %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\Applications\_Plugins\CGPUDecode\Vulkan\VK_ComputeShader\texture.frag.spv"       %BUILD_OUTDIR%

XCopy /r /d /y "%COMPUTE%\*.*"                                           %BUILD_OUTDIR%\Compute\

IF NOT EXIST %BUILD_OUTDIR%plugins mkdir %BUILD_OUTDIR%plugins 
IF NOT EXIST %BUILD_OUTDIR%WelcomePage mkdir %BUILD_OUTDIR%WelcomePage 
XCopy /e /d /y "%BUILD_SOLUTIONDIR%..\WelcomePage" "%BUILD_OUTDIR%WelcomePage"


XCopy /r /d /y "%BUILD_QTDIR%bin\libGLESv2%_DEBUG%.dll"            %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\libEGL%_DEBUG%.dll"               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5Core%_DEBUG%.dll"              %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5Widgets%_DEBUG%.dll"           %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5Gui%_DEBUG%.dll"               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5WebEngineWidgets%_DEBUG%.dll"  %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5WebEngine%_DEBUG%.dll"         %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5WebEngineCore%_DEBUG%.dll"     %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\QtWebEngineProcess%_DEBUG%.exe"   %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5Quick%_DEBUG%.dll"             %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5Qml%_DEBUG%.dll"               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5Network%_DEBUG%.dll"           %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5WebChannel%_DEBUG%.dll"        %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5xml%_DEBUG%.dll"               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5Positioning%_DEBUG%.dll"       %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\Qt5OpenGL%_DEBUG%.dll"            %BUILD_OUTDIR%


IF NOT EXIST %BUILD_OUTDIR%plugins\platforms mkdir %BUILD_OUTDIR%plugins\platforms
XCopy /r /d /y "%BUILD_QTDIR%plugins\platforms\qwindows%_DEBUG%.dll" %BUILD_OUTDIR%plugins\platforms\

IF NOT EXIST %BUILD_OUTDIR%plugins\imageformats mkdir %BUILD_OUTDIR%plugins\imageformats
XCopy /r /d /y "%BUILD_QTDIR%plugins\imageformats\qtga%_DEBUG%.dll"  %BUILD_OUTDIR%plugins\imageformats\
XCopy /r /d /y "%BUILD_QTDIR%plugins\imageformats\qtiff%_DEBUG%.dll" %BUILD_OUTDIR%plugins\imageformats\
XCopy /r /d /y "%BUILD_QTDIR%plugins\imageformats\qjpeg%_DEBUG%.dll" %BUILD_OUTDIR%plugins\imageformats\

IF NOT EXIST %BUILD_OUTDIR%resources mkdir %BUILD_OUTDIR%resources
XCopy /s /r /d /y "%BUILD_QTDIR%resources\*" %BUILD_OUTDIR%resources\

IF NOT EXIST %BUILD_OUTDIR%translations mkdir %BUILD_OUTDIR%translations
XCopy /r /d /y "%BUILD_QTDIR%translations\qtwebengine_en.qm" %BUILD_OUTDIR%translations\


IF NOT EXIST %BUILD_OUTDIR%translations\qtwebengine_locales mkdir %BUILD_OUTDIR%translations\qtwebengine_locales
XCopy /s /r /d /y "%BUILD_QTDIR%translations\qtwebengine_locales\en-US.pak" %BUILD_OUTDIR%translations\qtwebengine_locales\

REM IF "%PLATFORM%" == "Win32" goto :copy32ags
REM IF "%PLATFORM%" == "x64" goto :copy64ags

REM :copy32ags
REM XCopy /r /d /y "%BUILD_AGSDIR%\amd_ags_x86.dll"               %BUILD_OUTDIR%
REM goto :copyend

REM :copy64ags
REM XCopy /r /d /y "%BUILD_AGSDIR%\amd_ags_x64.dll"               %BUILD_OUTDIR%

REM :copyend

echo "Dependencies copied done"




