
add_library(Plugin_CCMP_SDK)

add_subdirectory(bc1)
add_subdirectory(bc2)
add_subdirectory(bc3)
add_subdirectory(bc4)
add_subdirectory(bc5)
add_subdirectory(bc6)
add_subdirectory(bc7)

if (OPTION_CMP_GTC)
add_subdirectory(gtc)
endif()

if (OPTION_CMP_APC)
add_subdirectory(apc)
endif()

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
