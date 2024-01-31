
add_library(ModelViewer_Vulkan SHARED)



 file(GLOB_RECURSE GLTF_VULKAN_UTIL
     ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf/vulkan/*.cpp
     ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf/vulkan/*.h
 )

file(GLOB_RECURSE VULKAN_UTIL
    util/*.cpp
    util/*.h
)

 file(GLOB_RECURSE MODEL_VIEWER
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/model_viewer/*.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/model_viewer/*.cpp
    )

target_sources(ModelViewer_Vulkan PRIVATE
    ${VULKAN_UTIL}
    ${GLTF_VULKAN_UTIL}
    ${MODEL_VIEWER}
    vulkan_device.cpp
    vulkan_device.h
    vulkan_main.cpp
    vulkan_main.h
    vulkan_renderer.cpp
    vulkan_renderer.h
)


target_include_directories(ModelViewer_Vulkan PUBLIC
    ./
    ./util
     ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
     ${PROJECT_SOURCE_DIR}/cmp_framework/common/
     ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
     ${PROJECT_SOURCE_DIR}/applications/_plugins/common
     ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf
     ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf/vulkan
     ${PROJECT_SOURCE_DIR}/applications/_plugins/common/model_viewer
     ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
     ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
     ${PROJECT_SOURCE_DIR}/applications/_plugins/c3dmodel_viewers/vulkan/util
     ${PROJECT_SOURCE_DIR}/applications/_plugins/c3dmodel_viewers/gltf_dx12_ex/dx12util
     ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui 
     ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
     ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(ModelViewer_Vulkan

    PRIVATE
        ExtVulkan
        ExtGLM
        ExtGlslang
        ExtQt5Widgets

        extern_imgui
        CMP_Compressonator
        CMP_Framework
        CMP_GpuDecode
        CMP_GUI_Gltf
        CMP_Common              # CModel Data
)

target_compile_definitions(ModelViewer_Vulkan PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(ModelViewer_Vulkan PROPERTIES 
    FOLDER ${PROJECT_FOLDER_APPS_PLUGIN_DYNAMIC}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
