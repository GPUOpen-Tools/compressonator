
link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>
    )

add_library(GPUDecode_DirectX SHARED)

file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.h
)

target_sources(GPUDecode_DirectX PRIVATE
    ${SOURCES}
    ddsview.fx
)

target_link_libraries(GPUDecode_DirectX PRIVATE
        CMP_Compressonator
        CMP_Framework
        CMP_GpuDecode
        ExtDirectXTex
)

target_include_directories(GPUDecode_DirectX PRIVATE
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex
)

target_compile_definitions(GPUDecode_DirectX PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(GPUDecode_DirectX PROPERTIES 
    FOLDER "Plugin_Dynamic/GPU_DecodeView"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
