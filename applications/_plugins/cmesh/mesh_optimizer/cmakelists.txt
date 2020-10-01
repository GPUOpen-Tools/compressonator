
add_library(Plugin_CMesh_Mesh_Optimizer)

target_sources(Plugin_CMesh_Mesh_Optimizer
    PRIVATE
        mesh_optimizer.h
        mesh_optimizer.cpp
)

target_include_directories(Plugin_CMesh_Mesh_Optimizer
    INTERFACE
        ./
)

target_link_libraries(Plugin_CMesh_Mesh_Optimizer
    PRIVATE
        CompressonatorLIB
        CMP_MeshOptimizer

        Plugin_PluginManager
        Plugin_TCPluginAPI
)

set_target_properties(Plugin_CMesh_Mesh_Optimizer PROPERTIES FOLDER ${FOLDER_NAME})
