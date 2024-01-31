# this must be before add_executable
# link_directories(
#      ${PROJECT_EXTERNAL_LIBDIR}/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64
#     )

add_library(ModelViewer_DX12 SHARED)

file(GLOB_RECURSE GLTF_DX12_SRCS
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf/DX12/*.cpp
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf/DX12/*.h
)

file(GLOB_RECURSE DX12UTIL_SRCS
    dx12util/*.cpp
    dx12util/*.h
)

target_sources(ModelViewer_DX12 PRIVATE
    ${GLTF_DX12_SRCS}
    ${DX12UTIL_SRCS}
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/qtimgui/imgui_dx12.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/qtimgui/imgui_dx12renderer.cpp
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/qtimgui/imgui_dx12renderer.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/model_viewer/modeltexture.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/model_viewer/modeltexture.cpp
    gltf_dx12_ex.h
    gltf_dx12_ex.cpp
    gltf_dx12deviceex.h
    gltf_dx12deviceex.cpp
    gltf_dx12rendererex.h
    gltf_dx12rendererex.cpp
)

target_include_directories(ModelViewer_DX12 PUBLIC
    ./
    ./media
    ./dx12util
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/d3dx12
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf/dx12
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/model_viewer
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/qtimgui
    ${PROJECT_SOURCE_DIR}/external/imgui

    ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
    ${Qt5Gui_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui 
)

target_link_libraries(ModelViewer_DX12 PRIVATE
    CMP_Compressonator
    CMP_Framework
    CMP_GUI_Gltf
    CMP_GpuDecode
    extern_imgui
    CMP_Common              # CModel Data
)

target_compile_definitions(ModelViewer_DX12 PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(ModelViewer_DX12 PROPERTIES 
    AUTOMOC ON
    FOLDER ${PROJECT_FOLDER_APPS_PLUGIN_DYNAMIC}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
)
