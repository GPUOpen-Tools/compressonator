link_directories(
    ${COMPRESSONATOR_ROOT_PATH}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/Release
    ${COMPRESSONATOR_ROOT_PATH}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/Debug
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
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/cmp_gpu/directx/cdirectx.cpp
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/cmp_gpu/directx/cdirectx.h
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/cmp_gpu/directx/compute_directx.cpp
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/cmp_gpu/directx/compute_directx.h
)

target_link_libraries(EncodeWith_DXC 
PRIVATE
    CMP_Compressonator
    CMP_Framework
    ExtDirectX
)

target_include_directories(EncodeWith_DXC PRIVATE
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib         # compressonator.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_framework                 # compute_base.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_framework/common/half     # half.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_core/shaders              # common_def.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_core/source               # cmp_math_vec4.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_core/shaders/compiled     # static compiler shaders 
    ${COMPRESSONATOR_ROOT_PATH}/applications/_libs/cmp_math   # cmp_math_common.h
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/common  # tc_pluginapi.h
    ${COMPRESSONATOR_ROOT_PATH}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex  
)

set_target_properties(EncodeWith_DXC PROPERTIES 
    FOLDER ${PROJECT_FOLDER_SDK_PLUGIN_DYNAMIC}
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
