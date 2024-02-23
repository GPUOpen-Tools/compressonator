
add_library(Mesh_Compressor SHARED)

target_sources(Mesh_Compressor
    PRIVATE
    mesh_compressor.cpp
    mesh_compressor.h
)

target_link_libraries(Mesh_Compressor
    PRIVATE
    CMP_Framework
    # CMP_MeshCompressor
)

target_include_directories(Mesh_Compressor PUBLIC
    PRIVATE
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_mesh
    # ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshcompressor/draco/src
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
)


target_compile_definitions(Mesh_Compressor PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(Mesh_Compressor PROPERTIES 
        FOLDER ${PROJECT_FOLDER_SDK_PLUGIN_DYNAMIC}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
)
