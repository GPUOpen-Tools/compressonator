
if (OPTION_CMP_DIRECTX )
    link_directories(
        ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/Release
        ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/Debug
    )
endif()

add_library(cmp_plugin SHARED)

target_sources(cmp_plugin PRIVATE
    prototype.cpp
    prototype.h
)

target_include_directories(cmp_plugin PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/apitrace/dxsdk/Include
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders              # cmp fidelity fx shaders
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib         # compressonator.h
    ${PROJECT_SOURCE_DIR}/cmp_framework                 # compute_base.h
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half     # half.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common  # tc_pluginapi.h
)

target_link_libraries(cmp_plugin PRIVATE
    CMP_Compressonator
    d3d11
    windowscodecs;
)

target_compile_definitions(cmp_plugin PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(cmp_plugin PROPERTIES 
    FOLDER ${FOLDER_NAME}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$(Configuration)"
)

