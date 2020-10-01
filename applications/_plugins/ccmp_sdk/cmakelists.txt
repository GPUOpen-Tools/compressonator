
add_library(Plugin_CCMP_SDK)

add_subdirectory(BC1)
add_subdirectory(BC2)
add_subdirectory(BC3)
add_subdirectory(BC4)
add_subdirectory(BC5)
add_subdirectory(BC6)
add_subdirectory(BC7)

target_include_directories(Plugin_CCMP_SDK PUBLIC
    ./
)

target_link_libraries(Plugin_CCMP_SDK PRIVATE

    CompressonatorLIB
    CMP_Core
    CMP_Math

    Plugin_CMesh_Mesh_Optimizer
    Plugin_PluginManager
    Plugin_Common_HPCCompress

    CMP_Mesh
    CMP_MeshOptimizer
    GPU_Decode
)

set_target_properties(Plugin_CCMP_SDK PROPERTIES FOLDER ${FOLDER_NAME})
