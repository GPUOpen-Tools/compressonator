
if (OPTION_CMP_DIRECTX )
link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64/Release
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64/Debug
    )
endif()

add_library(Image_FilterFX SHARED)

target_sources(Image_FilterFX PRIVATE

    filterfx.cpp
    filterfx.h
)

target_include_directories(Image_FilterFX PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/apitrace/dxsdk/Include
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib         # compressonator.h
    ${PROJECT_SOURCE_DIR}/cmp_framework                 # compute_base.h
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half     # half.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common  # tc_pluginapi.h
)

target_link_libraries(Image_FilterFX PRIVATE
    CMP_Compressonator

)

target_compile_definitions(Image_FilterFX PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(Image_FilterFX PROPERTIES 
    FOLDER "Plugin_Dynamic/ImageFx"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )

