
add_library(Plugin_CMP_EncodeGPU INTERFACE)

if (OPTION_CMP_DIRECTX)
add_subdirectory(directx)
endif()

add_subdirectory(opencl)

add_subdirectory(gpuhw)
