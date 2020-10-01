
add_library(Plugin_CGPUDecode_DirectX)

file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.h
)

target_sources(Plugin_CGPUDecode_DirectX PRIVATE
    ${SOURCES}
    ddsview.fx
)

target_link_libraries(Plugin_CGPUDecode_DirectX PRIVATE
    CompressonatorLIB
    GPU_Decode
    ExtDirectX
    ExtDirectXTex
    Plugin_TCPluginAPI
)

target_include_directories(Plugin_CGPUDecode_DirectX PRIVATE
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex
)

set_target_properties(Plugin_CGPUDecode_DirectX PROPERTIES FOLDER ${FOLDER_NAME})
