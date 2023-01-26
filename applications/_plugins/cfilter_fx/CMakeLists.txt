
if (OPTION_CMP_DIRECTX )
link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/Release
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/Debug
    )
endif()

add_library(Image_FilterFX SHARED)

target_sources(Image_FilterFX PRIVATE
    filterfx.cpp
    filterfx.h
    gpuresources.cpp
    gpuresources.h
    gpuresources_cas.cpp
    gpuresources_cas.h
    gpuresources_dx11.cpp
    gpuresources_dx11.h
    gpuresources_fsr.cpp
    gpuresources_fsr.h
)

# Experimental FXC code
add_custom_command(TARGET Image_FilterFX
    PRE_BUILD
    COMMENT "Generating FX shaders..."
    COMMAND "${PROJECT_SOURCE_DIR}/cmp_core/shaders/compilefx_shaders.cmd"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/cmp_core/shaders/"
    USES_TERMINAL)

target_include_directories(Image_FilterFX PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/apitrace/dxsdk/Include
    ${PROJECT_SOURCE_DIR}/cmp_core/source               # math and vector
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders              # hlml sources
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders/compiled     # compiled fx shaders
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib         # compressonator.h
    ${PROJECT_SOURCE_DIR}/cmp_framework                 # compute_base.h
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half     # half.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common  # tc_pluginapi.h
)

add_compile_definitions(USE_FSR)     # Super Resolution Upscaling

target_link_libraries(Image_FilterFX PRIVATE
    CMP_Compressonator
    d3d11
    windowscodecs;
)

target_compile_definitions(Image_FilterFX PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(Image_FilterFX PROPERTIES 
    FOLDER "Plugin_Dynamic/ImageFx"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )

