REM gets the output dir
set BUILD_OUTDIR=%1

REM get the batch files dir 
SET mypath=%~dp0
echo %mypath:~0,-1%

IF NOT EXIST %BUILD_OUTDIR%Compute mkdir %BUILD_OUTDIR%Compute

REM Build Vulkan Shader Binary
REM "%VULKAN_SDK%"\bin\glslangvalidator -V %mypath:~0,-1%\BC6H.comp -o %BUILD_OUTDIR%\Compute\BC6H.spv
REM IF %ERRORLEVEL% GTR 0 exit 123
REM XCopy /r /d /y "%mypath:~0,-1%\BC6H_compress.hlsl"  %BUILD_OUTDIR%\Compute\
REM XCopy /r /d /y "%mypath:~0,-1%\BC6H_compress_blit.hlsl"  %BUILD_OUTDIR%\Compute\

XCopy /r /d /y "%mypath:~0,-1%\BC6H_Encode_Kernel.h"  %BUILD_OUTDIR%\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC6H_Encode_Kernel.cpp"  %BUILD_OUTDIR%\Compute\

XCopy /r /d /y "%mypath:~0,-1%\..\..\Common\Common_KernelDef.h"  %BUILD_OUTDIR%\Compute\
XCopy /r /d /y "%mypath:~0,-1%\..\..\Common\Common_Def.h"        %BUILD_OUTDIR%\Compute\

echo "Dependencies copied done"




