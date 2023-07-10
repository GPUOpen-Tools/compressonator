if (OPTION_CMP_DIRECTX )
link_directories( ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64 )
endif()

add_library(Image_MipMap SHARED)

target_sources(Image_MipMap PRIVATE

    boxfilter.cpp
    boxfilter.h
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/cmp_boxfilter.cpp
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/cmp_boxfilter.h
)

target_include_directories(Image_MipMap PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex
  
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib         # compressonator.h
    ${PROJECT_SOURCE_DIR}/cmp_framework                 # compute_base.h
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half     # half.h
    
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common  # tc_pluginapi.h
)

target_link_libraries(Image_MipMap PRIVATE
    CMP_Compressonator
)

target_compile_definitions(Image_MipMap PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(Image_MipMap PROPERTIES 
    FOLDER "Plugin_Dynamic/ImageFx"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
)
