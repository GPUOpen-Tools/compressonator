
add_library(Plugin_C3DModel_Loaders_drc)

target_sources(Plugin_C3DModel_Loaders_drc PRIVATE

    ModelLoader_drc.cpp
    ModelLoader_drc.h
)

target_link_libraries(Plugin_C3DModel_Loaders_drc PRIVATE

    CompressonatorLIB
    CMP_MeshCompressor
    CMP_MeshOptimizer

    Plugin_PluginManager
    Plugin_TCPluginAPI
)

target_include_directories(Plugin_C3DModel_Loaders_drc PUBLIC

	./
)

set_target_properties(Plugin_C3DModel_Loaders_drc PROPERTIES FOLDER ${FOLDER_NAME})
