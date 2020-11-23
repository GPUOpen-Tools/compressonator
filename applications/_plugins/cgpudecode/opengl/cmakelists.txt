
add_library(GPUDecode_OpenGL SHARED)

target_sources(GPUDecode_OpenGL PUBLIC

    copengl.cpp
    copengl.h
    gpu_opengl.cpp
    gpu_opengl.h
    shader.h
)


target_include_directories(GPUDecode_OpenGL PRIVATE
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
)

target_link_libraries(GPUDecode_OpenGL 

    PRIVATE
        CMP_Compressonator
        CMP_Framework
        CMP_GpuDecode

        ExtOpenGL
        ExtGLEW

    INTERFACE
        ExtGLEW
)

target_compile_definitions(GPUDecode_OpenGL PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(GPUDecode_OpenGL PROPERTIES 
    FOLDER "Plugin_Dynamic/GPU_DecodeView"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
