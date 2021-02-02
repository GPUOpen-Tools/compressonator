
add_library(Plugin_CGPUDecode INTERFACE)

if (OPTION_CMP_DIRECTX)
    add_subdirectory(directx)
    target_link_libraries(Plugin_CGPUDecode INTERFACE Plugin_CGPUDecode_DirectX)
endif()

if (OPTION_CMP_OPENGL)
    add_subdirectory(opengl)
    target_link_libraries(Plugin_CGPUDecode INTERFACE Plugin_CGPUDecode_OpenGL)
endif()

if (OPTION_CMP_VULKAN)
    add_subdirectory(vulkan)
    target_link_libraries(Plugin_CGPUDecode INTERFACE Plugin_CGPUDecode_Vulkan)
endif()
