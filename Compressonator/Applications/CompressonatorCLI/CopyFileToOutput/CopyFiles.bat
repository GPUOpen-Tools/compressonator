set BUILD_QTDIR=%1
set BUILD_OPENCV_DIR=%2
set BUILD_OUTDIR=%3
set BUILD_SOLUTIONDIR=%4
set _DEBUG=%5

echo ---------------------------------------
echo QT      : [%BUILD_QTDIR%]
echo OPENCV  : [%BUILD_OPENCV_DIR%]
echo OUTPUT  : [%BUILD_OUTDIR%]
echo SOLUTION: [%BUILD_SOLUTIONDIR%]
echo DBG/REL : [%_DEBUG%]
echo ---------------------------------------

echo on

XCopy /r /d /y ..\qt.conf %BUILD_OUTDIR%

XCopy /r /d /y "%BUILD_QTDIR%bin\win32\icudt54.dll"                      %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\icuin54.dll"                      %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\icuuc54.dll"                      %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\libGLESv2%_DEBUG%.dll"            %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Core%_DEBUG%.dll"              %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Gui%_DEBUG%.dll"               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_core249%_DEBUG%.dll"    %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_imgproc249%_DEBUG%.dll" %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_highgui249%_DEBUG%.dll" %BUILD_OUTDIR%

IF NOT EXIST %BUILD_OUTDIR%plugins mkdir %BUILD_OUTDIR%plugins 