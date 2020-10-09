# =======================================================================

add_library(Plugin_CmdLine)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../common)

target_sources(Plugin_CmdLine PRIVATE

    ${LEGACY_LOCATION}/cmdline.cpp
    ${LEGACY_LOCATION}/cmdline.h
    ${LEGACY_LOCATION}/textureio.h
    ${LEGACY_LOCATION}/textureio.cpp
    ${LEGACY_LOCATION}/mipstoqimage.cpp
    ${LEGACY_LOCATION}/mipstoqimage.h
)

target_include_directories(Plugin_CmdLine PUBLIC
    ./
    ${LEGACY_LOCATION}
    ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
)

target_link_libraries(Plugin_CmdLine

    PRIVATE
        CMP_Common
        CMP_FileIO
        CMP_MeshCompressor
        CMP_MeshOptimizer
        CompressonatorLIB
        GPU_Decode

        Plugin_CMesh_Mesh_Optimizer
        Plugin_TCPluginAPI
        
        Plugin_PluginManager
        Plugin_Common_ATIFormats
        Plugin_GLTF
        Plugin_Common_UtilFuncs
        Plugin_Common_KernelDef

        ExtOpenEXR
        ExtQt5

    PUBLIC
        GPU_Decode
)

set_target_properties(Plugin_CmdLine PROPERTIES FOLDER ${FOLDER_NAME})
