set BUILD_QTDIR=%1
set BUILD_OPENCV_DIR=%2
set BUILD_OUTDIR=%3
set BUILD_ROOT=%4
set BUILD_GLEWDIR=%5
set BUILD_VULKANDIR=%6
set _DEBUG=%7

echo ---------------------------------------
echo QT                 : [%BUILD_QTDIR%]
echo OPENCV             : [%BUILD_OPENCV_DIR%]
echo OUTPUT             : [%BUILD_OUTDIR%]
echo ROOT               : [%BUILD_ROOT%]
echo GLEW               : [%BUILD_GLEWDIR%]
echo VULKAN             : [%BUILD_VULKANDIR%]
echo DEBUG              : [%_DEBUG%]
echo ---------------------------------------

echo on

XCopy /r /d /y ..\qt.conf %BUILD_OUTDIR%

XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_core249%_DEBUG%.dll"            %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_imgproc249%_DEBUG%.dll"         %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_highgui249%_DEBUG%.dll"         %BUILD_OUTDIR%

XCopy /r /d /y "%BUILD_ROOT%\Applications\_Plugins\CGPUDecode\Vulkan\VK_ComputeShader\texture.vert.spv"       %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_ROOT%\Applications\_Plugins\CGPUDecode\Vulkan\VK_ComputeShader\texture.frag.spv"       %BUILD_OUTDIR%

REM IF NOT EXIST %BUILD_OUTDIR%Compute mkdir %BUILD_OUTDIR%Compute 
REM XCopy /r /d /y "%BUILD_ROOT%\Compute\*.*"                                               %BUILD_OUTDIR%\Compute\

IF NOT EXIST %BUILD_OUTDIR%plugins mkdir %BUILD_OUTDIR%plugins 

XCopy /r /d /y "%BUILD_QTDIR%\bin\libGLESv2%_DEBUG%.dll"            %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%\bin\Qt5Core%_DEBUG%.dll"              %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%\bin\Qt5Gui%_DEBUG%.dll"               %BUILD_OUTDIR%

IF NOT EXIST %BUILD_OUTDIR%plugins\imageformats mkdir %BUILD_OUTDIR%plugins\imageformats

XCopy /r /d /y "%BUILD_QTDIR%\plugins\imageformats\qtga%_DEBUG%.dll"  %BUILD_OUTDIR%plugins\imageformats\
XCopy /r /d /y "%BUILD_QTDIR%\plugins\imageformats\qtiff%_DEBUG%.dll" %BUILD_OUTDIR%plugins\imageformats\
XCopy /r /d /y "%BUILD_QTDIR%plugins\imageformats\qjpeg%_DEBUG%.dll" %BUILD_OUTDIR%plugins\imageformats\

IF NOT EXIST %BUILD_OUTDIR%plugins\platforms mkdir %BUILD_OUTDIR%plugins\platforms

XCopy /r /d /y "%BUILD_QTDIR%\plugins\platforms\qwindows%_DEBUG%.dll" %BUILD_OUTDIR%plugins\platforms\

XCopy /r /d /y "%BUILD_GLEWDIR%\glew32.dll"    %BUILD_OUTDIR%

echo "DLL copied done"