REM gets the output dir
set BUILD_OUTDIR=%1

REM get the batch files dir 
SET mypath=%~dp0
echo %mypath:~0,-1%

IF NOT EXIST %BUILD_OUTDIR%Compute mkdir %BUILD_OUTDIR%Compute

del %BUILD_OUTDIR%\Compute\BC7_Encode_Kernel.cpp.cmp

XCopy /r /d /y "%mypath:~0,-1%\BC7_Encode_Kernel.cpp"  %BUILD_OUTDIR%\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC7_Encode_Kernel.h"  %BUILD_OUTDIR%\Compute\

XCopy /r /d /y "%mypath:~0,-1%\..\..\Common\Common_Def.h"        %BUILD_OUTDIR%\Compute\

echo "Dependencies copied done"




