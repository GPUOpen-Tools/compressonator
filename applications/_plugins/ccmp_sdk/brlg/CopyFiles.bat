REM gets the output dir
echo --BRLG--
set BUILD_OUTDIR=%1
echo %BUILDDIR%
REM get the batch files dir 
SET mypath=%~dp0
echo %mypath:~0,-1%

IF NOT EXIST %BUILD_OUTDIR%Compute mkdir %BUILD_OUTDIR%Compute

del %BUILD_OUTDIR%\Compute\GT_Encode_Kernel.cpp.cmp

XCopy /r /d /y "%mypath:~0,-1%\BRLG_Encode_Kernel.h"    %BUILD_OUTDIR%\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BRLG_Encode_Kernel.cpp"  %BUILD_OUTDIR%\Compute\

XCopy /r /d /y "%mypath:~0,-1%\..\..\Common\Common_Def.h"        %BUILD_OUTDIR%\Compute\

echo "Dependencies copied done"




