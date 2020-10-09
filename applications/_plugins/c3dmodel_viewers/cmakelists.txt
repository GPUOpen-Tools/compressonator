
add_library(Plugin_C3DModel_Viewers INTERFACE)

if (OPTION_CMP_DIRECTX)
    add_subdirectory(gltf_dx12_ex)
    target_link_libraries(Plugin_C3DModel_Viewers INTERFACE

        Plugin_C3DModel_Viewers_glTF_DX12_EX
    )
endif()

if (CMP_HOST_WINDOWS)
if (OPTION_CMP_OPENGL)
    add_subdirectory(gltf_opengl)
    target_link_libraries(Plugin_C3DModel_Viewers INTERFACE

        Plugin_C3DModel_Viewers_glTF_OpenGL
    )
endif()
endif()

if (OPTION_CMP_VULKAN)
    add_subdirectory(vulkan)
    target_link_libraries(Plugin_C3DModel_Viewers INTERFACE

        Plugin_C3DModel_Viewers_Vulkan
    )
endif()

