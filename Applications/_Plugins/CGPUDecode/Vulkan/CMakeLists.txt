
add_library(Plugin_CGPUDecode_Vulkan)

file(GLOB_RECURSE SHADERS

    *.spv
)

file(GLOB_RECURSE SOURCES

    *.cpp
    *.h
    *.hpp
    *.inl
)

target_sources(Plugin_CGPUDecode_Vulkan PRIVATE

    ${SOURCES}
)

target_include_directories(Plugin_CGPUDecode_Vulkan PRIVATE

   ./
)

target_link_libraries(Plugin_CGPUDecode_Vulkan PRIVATE

    CMP_Common
    CompressonatorLIB

    Plugin_PluginManager
    Plugin_TCPluginAPI
    GPU_Decode

    ExtVulkan
    ExtGLM
)

set_target_properties(Plugin_CGPUDecode_Vulkan PROPERTIES FOLDER ${FOLDER_NAME})
