
add_library(Plugin_C3DModel_Loaders_obj)

file(GLOB_RECURSE SRCS

    "*.cpp"
    "*.h"
)

target_sources(Plugin_C3DModel_Loaders_obj PRIVATE

    ${SRCS}
)

target_link_libraries(Plugin_C3DModel_Loaders_obj PRIVATE

    CompressonatorLIB
    CMP_MeshCompressor
    CMP_MeshOptimizer
    
    Plugin_TCPluginAPI
    Plugin_PluginManager
    Plugin_GLTF
    Plugin_Common_UtilFuncs
)

target_include_directories(Plugin_C3DModel_Loaders_obj PUBLIC

    ./
)

set_target_properties(Plugin_C3DModel_Loaders_obj PROPERTIES FOLDER ${FOLDER_NAME})
