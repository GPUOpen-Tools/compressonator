set BUILD_OUTDIR=%1
set BUILD_PROJECTDIR=%2

echo ---------------------------------------
echo OUTPUT  : [%BUILD_OUTDIR%]
echo SOLUTION: [%BUILD_PROJECTDIR%]
echo ---------------------------------------

echo on

echo on
IF NOT EXIST %BUILD_OUTDIR%shaders mkdir %BUILD_OUTDIR%shaders
XCopy /r /d /y "%BUILD_PROJECTDIR%..\Util\shaders\glTF20-frag_vk.glsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\Util\shaders\glTF20-vert_vk.glsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\Util\shaders\shadows-frag_vk.glsl"       %BUILD_OUTDIR%shaders\
XCopy /r /d /y "%BUILD_PROJECTDIR%..\Util\shaders\shadows-vert_vk.glsl"       %BUILD_OUTDIR%shaders\

echo "glTF_Vulkan dependencies copy done"
