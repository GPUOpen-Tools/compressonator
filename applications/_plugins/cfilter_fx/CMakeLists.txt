
if (OPTION_CMP_DIRECTX )
link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64/Release
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64/Debug
    )
endif()

add_library(Plugin_CFilter_Fx SHARED)

target_sources(Plugin_CFilter_Fx PRIVATE

    filterfx.cpp
    filterfx.h
)

target_include_directories(Plugin_CFilter_Fx PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/apitrace/dxsdk/Include
)

target_link_libraries(Plugin_CFilter_Fx PRIVATE

    CMP_FileIO
    CompressonatorLIB
    Plugin_CmdLine
    Plugin_PluginManager
    Plugin_TCPluginAPI
)

set_target_properties(Plugin_CFilter_Fx PROPERTIES 
    FOLDER ${FOLDER_NAME}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )

