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

XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\Documents\UserGuide.chm"       %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\Documents\GettingStarted.chm"  %BUILD_OUTDIR%

IF NOT EXIST %BUILD_OUTDIR%Projects mkdir %BUILD_OUTDIR%Projects
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\RunTime\BC7_Compression.cprj"  %BUILD_OUTDIR%Projects\
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\RunTime\BC6H_Compression.cprj" %BUILD_OUTDIR%Projects\

IF NOT EXIST %BUILD_OUTDIR%Images   mkdir %BUILD_OUTDIR%Images
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\RunTime\images\Balls.exr"  %BUILD_OUTDIR%Images\
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\RunTime\images\Boat.png"   %BUILD_OUTDIR%Images\
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\..\..\RunTime\images\Ruby2.bmp"  %BUILD_OUTDIR%Images\

XCopy /r /d /y ..\qt.conf %BUILD_OUTDIR%

XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_core249%_DEBUG%.dll"    %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_imgproc249%_DEBUG%.dll" %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_OPENCV_DIR%opencv_highgui249%_DEBUG%.dll" %BUILD_OUTDIR%

XCopy /r /d /y "%BUILD_QTDIR%bin\win32\icudt54.dll"                      %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\icuin54.dll"                      %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\icuuc54.dll"                      %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\libGLESv2%_DEBUG%.dll"            %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\libEGL%_DEBUG%.dll"               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Core%_DEBUG%.dll"              %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Widgets%_DEBUG%.dll"           %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Gui%_DEBUG%.dll"               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5WebKitWidgets%_DEBUG%.dll"     %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5WebKit%_DEBUG%.dll"            %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Sensors%_DEBUG%.dll"           %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Positioning%_DEBUG%.dll"       %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Quick%_DEBUG%.dll"             %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Qml%_DEBUG%.dll"               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Network%_DEBUG%.dll"           %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Multimedia%_DEBUG%.dll"        %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5WebChannel%_DEBUG%.dll"        %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5Sql%_DEBUG%.dll"               %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5MultimediaWidgets%_DEBUG%.dll" %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5OpenGL%_DEBUG%.dll"            %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5PrintSupport%_DEBUG%.dll"      %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_QTDIR%bin\win32\Qt5xml%_DEBUG%.dll"               %BUILD_OUTDIR%

IF NOT EXIST %BUILD_OUTDIR%plugins mkdir %BUILD_OUTDIR%plugins 
IF NOT EXIST %BUILD_OUTDIR%WelcomePage mkdir %BUILD_OUTDIR%WelcomePage 
XCopy /e /d /y "%BUILD_SOLUTIONDIR%..\WelcomePage" "%BUILD_OUTDIR%WelcomePage"

IF NOT EXIST %BUILD_OUTDIR%plugins\platforms mkdir %BUILD_OUTDIR%plugins\platforms
XCopy /r /d /y "%BUILD_QTDIR%plugins\platforms\qwindows%_DEBUG%.dll" %BUILD_OUTDIR%plugins\platforms\

IF NOT EXIST %BUILD_OUTDIR%plugins\imageformats mkdir %BUILD_OUTDIR%plugins\imageformats
XCopy /r /d /y "%BUILD_QTDIR%plugins\imageformats\qtga%_DEBUG%.dll"  %BUILD_OUTDIR%plugins\imageformats\
XCopy /r /d /y "%BUILD_QTDIR%plugins\imageformats\qtiff%_DEBUG%.dll" %BUILD_OUTDIR%plugins\imageformats\

IF [%_DEBUG%] == [] GOTO UseRelease
echo copy: "%BUILD_SOLUTIONDIR%..\CXL\Lib\qcustomplotd.dll"
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\qcustomplotd.dll"                %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\qscintilla2d.dll"                %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\CXLBaseTools-d.dll"              %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\CXLApiClasses-d.dll"             %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\CXLOSAPIWrappers-d.dll"          %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\CXLOSWrappers-d.dll"             %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\CXLApplicationComponents-d.dll"  %BUILD_OUTDIR%
GOTO Next
:UseRelease
echo copy: "%BUILD_SOLUTIONDIR%..\CXL\Lib\qcustomplot.dll"
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\qcustomplot.dll"                %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\qscintilla2.dll"                %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\CXLBaseTools.dll"              %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\CXLApiClasses.dll"             %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\CXLOSAPIWrappers.dll"          %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\CXLOSWrappers.dll"             %BUILD_OUTDIR%
XCopy /r /d /y "%BUILD_SOLUTIONDIR%..\CXL\Lib\CXLApplicationComponents.dll"  %BUILD_OUTDIR%
:Next
