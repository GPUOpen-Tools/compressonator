
add_library(Mesh_Optimizer SHARED)

target_sources(Mesh_Optimizer
    PRIVATE
        mesh_optimizer.h
        mesh_optimizer.cpp
)

target_include_directories(Mesh_Optimizer
    INTERFACE
        ./
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common
)

target_link_libraries(Mesh_Optimizer
    PRIVATE
        CMP_Compressonator
        CMP_Framework
        CMP_MeshOptimizer
)

target_compile_definitions(Mesh_Optimizer PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(Mesh_Optimizer PROPERTIES 
        FOLDER "Plugin_Dynamic/Mesh_Compression"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
        )
