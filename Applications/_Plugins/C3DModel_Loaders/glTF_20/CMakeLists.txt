
add_library(Plugin_C3DModel_Loaders_glTF_20)

target_sources(Plugin_C3DModel_Loaders_glTF_20 PRIVATE

    glTF_20.cpp
    glTF_20.h
)

target_link_libraries(Plugin_C3DModel_Loaders_glTF_20 PRIVATE

    CompressonatorLIB
    CMP_MeshCompressor
    CMP_MeshOptimizer
    
    Plugin_Common_Misc
    Plugin_GLTF
    Plugin_PluginManager
    Plugin_TCPluginAPI
)

target_include_directories(Plugin_C3DModel_Loaders_glTF_20 PUBLIC

	./
)

set_target_properties(Plugin_C3DModel_Loaders_glTF_20 PROPERTIES FOLDER ${FOLDER_NAME})
