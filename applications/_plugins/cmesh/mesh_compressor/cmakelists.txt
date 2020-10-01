
add_library(Plugin_CMesh_Mesh_Compressor)

target_sources(Plugin_CMesh_Mesh_Compressor
    PRIVATE
        mesh_compressor.cpp
        mesh_compressor.h
)

target_link_libraries(Plugin_CMesh_Mesh_Compressor
    PRIVATE
        CMP_MeshCompressor
        Plugin_TCPluginAPI
        Plugin_PluginManager
)

target_include_directories(Plugin_CMesh_Mesh_Compressor PUBLIC

    ./
)

set_target_properties(Plugin_CMesh_Mesh_Compressor PROPERTIES FOLDER ${FOLDER_NAME})
