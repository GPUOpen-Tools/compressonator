set BUILD_OUTDIR=%1
set BUILD_PROJECTDIR=%2

echo ---------------------------------------
echo OUTPUT  : [%BUILD_OUTDIR%]
echo SOLUTION: [%BUILD_PROJECTDIR%]
echo ---------------------------------------

echo on
IF NOT EXIST %BUILD_OUTDIR%shaders mkdir %BUILD_OUTDIR%shaders
XCopy /r /d /y "%BUILD_PROJECTDIR%..\DX12Util\shaders\blend.hlsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\DX12Util\shaders\bloom.hlsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\DX12Util\shaders\blur.hlsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\DX12Util\shaders\downscale.hlsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\DX12Util\shaders\downscaleThreshold.hlsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\DX12Util\shaders\glTF20_EX.hlsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\DX12Util\shaders\shadows.hlsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\DX12Util\shaders\SkyDome.hlsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\DX12Util\shaders\Tonemapping.hlsl"       %BUILD_OUTDIR%shaders\

IF NOT EXIST %BUILD_OUTDIR%media mkdir %BUILD_OUTDIR%media
IF NOT EXIST %BUILD_OUTDIR%media\envmap mkdir %BUILD_OUTDIR%media\envmap
XCopy /r /d /y "%BUILD_PROJECTDIR%..\media\envmap\brdf.dds"       %BUILD_OUTDIR%media\envmap\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\media\envmap\diffuse.dds"       %BUILD_OUTDIR%media\envmap\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\media\envmap\specular.dds"       %BUILD_OUTDIR%media\envmap\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\media\envmap\readme.txt"         %BUILD_OUTDIR%media\envmap\

echo "glTF_DX12_EX dependencies copy done"
