link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64/Release
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64/Debug
    )

add_library(Plugin_CFilter SHARED)

target_sources(Plugin_CFilter PRIVATE

    BoxFilter.cpp
    BoxFilter.h
    ${PROJECT_SOURCE_DIR}/Applications/_Plugins/CMP_D3DX/Module.cpp
    ${PROJECT_SOURCE_DIR}/Applications/_Plugins/CMP_D3DX/Module.h
    ${PROJECT_SOURCE_DIR}/Applications/_Plugins/CMP_D3DX/D3DXModule.cpp
    ${PROJECT_SOURCE_DIR}/Applications/_Plugins/CMP_D3DX/D3DXModule.h
    ${PROJECT_SOURCE_DIR}/Applications/_Plugins/CMP_D3DX/D3D9Module.cpp
    ${PROJECT_SOURCE_DIR}/Applications/_Plugins/CMP_D3DX/D3D9Module.h
)

target_include_directories(Plugin_CFilter PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/../Common/Lib/Ext/apitrace/dxsdk/Include
    ${PROJECT_SOURCE_DIR}/Applications/_Plugins/CMP_D3DX
    #${PROJECT_SOURCE_DIR}/../Common/Lib/Ext/DirectX-SDK/June-2010/Include
)

target_link_libraries(Plugin_CFilter PRIVATE

    CMP_FileIO
    CompressonatorLIB
    Plugin_CmdLine
    Plugin_PluginManager
    Plugin_TCPluginAPI
)

set_target_properties(Plugin_CFilter PROPERTIES 
    FOLDER ${FOLDER_NAME}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )

