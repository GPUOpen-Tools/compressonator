
add_library(Plugin_C3DModel_Viewers INTERFACE)

if (OPTION_CMP_DIRECTX)
	add_subdirectory(glTF_DX12_EX)
	target_link_libraries(Plugin_C3DModel_Viewers INTERFACE

		Plugin_C3DModel_Viewers_glTF_DX12_EX
	)
endif()

if (OPTION_CMP_OPENGL)
	add_subdirectory(glTF_OpenGL)
	target_link_libraries(Plugin_C3DModel_Viewers INTERFACE

		Plugin_C3DModel_Viewers_glTF_OpenGL
	)
endif()

if (OPTION_CMP_VULKAN)
	add_subdirectory(Vulkan)
	target_link_libraries(Plugin_C3DModel_Viewers INTERFACE

		Plugin_C3DModel_Viewers_Vulkan
	)
endif()

