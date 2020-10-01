
add_library(Plugin_CMP_EncodeGPU INTERFACE)

add_subdirectory(DirectX)
#target_link_libraries(Plugin_CMP_GPU INTERFACE Plugin_CMP_GPU_DXC)

add_subdirectory(OpenCL)
#target_link_libraries(Plugin_CMP_GPU INTERFACE Plugin_CMP_GPU_OCL)

add_subdirectory(GpuHW)
