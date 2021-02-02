
add_library(GPUDecode_Vulkan SHARED)

file(GLOB_RECURSE SHADERS
    *.spv
)

# file(GLOB_RECURSE SOURCES
#     *.cpp
#     *.h
#     *.hpp
#     *.inl
# )

target_sources(GPUDecode_Vulkan PRIVATE
#    ${SOURCES}
${PROJECT_SOURCE_DIR}/applications/_plugins/cgpudecode/vulkan/cvulkan.cpp
${PROJECT_SOURCE_DIR}/applications/_plugins/cgpudecode/vulkan/cvulkan.h
${PROJECT_SOURCE_DIR}/applications/_plugins/cgpudecode/vulkan/vulkandebug.cpp
${PROJECT_SOURCE_DIR}/applications/_plugins/cgpudecode/vulkan/vulkandebug.h
${PROJECT_SOURCE_DIR}/applications/_plugins/cgpudecode/vulkan/vulkanswapchain.hpp
${PROJECT_SOURCE_DIR}/applications/_plugins/cgpudecode/vulkan/vulkantools.cpp
${PROJECT_SOURCE_DIR}/applications/_plugins/cgpudecode/vulkan/vulkantools.h
${PROJECT_SOURCE_DIR}/applications/_plugins/cgpudecode/vulkan/gpu_vulkan.cpp
${PROJECT_SOURCE_DIR}/applications/_plugins/cgpudecode/vulkan/gpu_vulkan.h
)

target_include_directories(GPUDecode_Vulkan PRIVATE
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
)

target_link_libraries(GPUDecode_Vulkan PRIVATE
    CMP_Common
    CMP_Compressonator
    CMP_GpuDecode

    ExtVulkan
    ExtGLM
)

target_compile_definitions(GPUDecode_Vulkan PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(GPUDecode_Vulkan PROPERTIES 
    FOLDER "Plugin_Dynamic/GPU_DecodeView"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
