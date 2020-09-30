link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64/Release
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64/Debug
    )

add_library(Plugin_CMP_GPU_DXC SHARED )

target_sources(Plugin_CMP_GPU_DXC PUBLIC
    CDirectX.cpp
    CDirectX.h
    Compute_DirectX.cpp
    Compute_DirectX.h
)

target_link_libraries(Plugin_CMP_GPU_DXC 
PRIVATE
    CompressonatorLIB
    GPU_Decode
    ExtDirectX
    ExtDirectXTex
    Plugin_TCPluginAPI
    CMP_Framework
    Plugin_PluginManager
)

target_include_directories(Plugin_CMP_GPU_DXC PRIVATE
    ${PROJECT_SOURCE_DIR}/CMP_Framework             # compute_base.h
    ${PROJECT_SOURCE_DIR}/CMP_Core/shaders          # common_def.h
    ${PROJECT_SOURCE_DIR}/CMP_Core/source           # cmp_math_vec4.h
    ${PROJECT_SOURCE_DIR}/Applications/_Libs/CMP_Math #cmp_math_common.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex
)

set_target_properties(Plugin_CMP_GPU_DXC PROPERTIES 
    FOLDER ${FOLDER_NAME}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
