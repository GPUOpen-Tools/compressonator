REM ====================================
REM Hybrid Codecs: Full support in v4.0
REM ====================================

REM gets the output dir
set BUILD_OUTDIR=%1

REM get the batch files dir 
SET mypath=%~dp0
echo %mypath:~0,-1%

IF NOT EXIST "%outpath%"\Plugins mkdir %BUILD_OUTDIR%Plugins
IF NOT EXIST "%outpath%"\Plugins\Compute mkdir %BUILD_OUTDIR%Plugins\Compute

REM ToDo: Build Vulkan based shaders
REM "%VULKAN_SDK%"\bin\glslangvalidator -V %mypath:~0,-1%\BC1... -o %BUILD_OUTDIR%\Plugins\Compute\BC1....spv
REM IF %ERRORLEVEL% GTR 0 exit 123

REM Remove any OpenCL compiled Binaries
REM 
del %BUILD_OUTDIR%Plugins\Compute\BC1_Encode_kernel.cpp.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC1_Encode_kernel.hlsl.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC2_Encode_kernel.cpp.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC2_Encode_kernel.hlsl.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC3_Encode_kernel.cpp.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC3_Encode_kernel.hlsl.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC4_Encode_kernel.cpp.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC4_Encode_kernel.hlsl.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC5_Encode_kernel.cpp.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC5_Encode_kernel.hlsl.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC6_Encode_kernel.cpp.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC6_Encode_kernel.hlsl.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC6_Encode_kernel.hlsl.0.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC6_Encode_kernel.hlsl.1.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC7_Encode_kernel.cpp.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC7_Encode_kernel.hlsl.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC7_Encode_kernel.hlsl.0.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC7_Encode_kernel.hlsl.1.cmp
del %BUILD_OUTDIR%Plugins\Compute\BC7_Encode_kernel.hlsl.2.cmp

XCopy /r /d /y "%mypath:~0,-1%\Common_Def.h"            %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BCn_Common_kernel.h"     %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC1_Encode_kernel.h"     %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC1_Encode_kernel.hlsl"  %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC1_Encode_kernel.cpp"   %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC2_Encode_kernel.h"     %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC2_Encode_kernel.hlsl"  %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC2_Encode_kernel.cpp"   %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC3_Encode_kernel.h"     %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC3_Encode_kernel.hlsl"  %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC3_Encode_kernel.cpp"   %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC4_Encode_kernel.h"     %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC4_Encode_kernel.hlsl"  %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC4_Encode_kernel.cpp"   %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC5_Encode_kernel.h"     %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC5_Encode_kernel.hlsl"  %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC5_Encode_kernel.cpp"   %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC6_Encode_kernel.h"     %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC6_Encode_kernel.hlsl"  %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC6_Encode_kernel.cpp"   %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC7_Encode_kernel.h"     %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC7_Encode_kernel.hlsl"  %BUILD_OUTDIR%Plugins\Compute\
XCopy /r /d /y "%mypath:~0,-1%\BC7_Encode_kernel.cpp"   %BUILD_OUTDIR%Plugins\Compute\

echo "Dependencies copied done"




