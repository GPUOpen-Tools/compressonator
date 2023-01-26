link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/Release
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/Debug
    )

add_library(EncodeWith_DXC SHARED )

# Experimental static DXC code
# add_custom_command(TARGET EncodeWith_DXC
#     PRE_BUILD
#     COMMENT "Generating HLSL shaders..."
#     COMMAND "${PROJECT_SOURCE_DIR}/cmp_core/shaders/compilebcn_shaders.cmd"
#     WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/cmp_core/shaders/"
#     USES_TERMINAL)

target_sources(EncodeWith_DXC PUBLIC
    cdirectx.cpp
    cdirectx.h
    compute_directx.cpp
    compute_directx.h
)

target_link_libraries(EncodeWith_DXC 
PRIVATE
    CMP_Compressonator
    CMP_Framework
    ExtDirectX
)

target_include_directories(EncodeWith_DXC PRIVATE
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib         # compressonator.h
    ${PROJECT_SOURCE_DIR}/cmp_framework                 # compute_base.h
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half     # half.h
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders              # common_def.h
    ${PROJECT_SOURCE_DIR}/cmp_core/source               # cmp_math_vec4.h
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders/compiled     # static compiler shaders 
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math   # cmp_math_common.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common  # tc_pluginapi.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex  
)

set_target_properties(EncodeWith_DXC PROPERTIES 
    FOLDER "Plugin_Dynamic/GPU_EncodeWith"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
