
set(FOLDER_NAME _Libs)

add_subdirectory(cmp_math)
add_subdirectory(cmp_mesh)
#if (OPTION_BUILD_DRACO)
#add_subdirectory(cmp_meshcompressor)
#endif()
add_subdirectory(cmp_meshoptimizer)
add_subdirectory(gpu_decode)
