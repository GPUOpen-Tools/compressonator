# =======================================================================

add_library(Plugin_GLTF)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../common)

target_sources(Plugin_GLTF
    PRIVATE
        ${LEGACY_LOCATION}/gltf/defines.h
        ${LEGACY_LOCATION}/gltf/error.cpp
        ${LEGACY_LOCATION}/gltf/error.h
        ${LEGACY_LOCATION}/gltf/gltfcommon.cpp
        ${LEGACY_LOCATION}/gltf/gltfcommon.h
        ${LEGACY_LOCATION}/gltf/gltffeatures.h
        ${LEGACY_LOCATION}/gltf/gltfhelpers.cpp
        ${LEGACY_LOCATION}/gltf/gltfhelpers.h
        ${LEGACY_LOCATION}/gltf/gltfstructures.h
        ${LEGACY_LOCATION}/gltf/stb_image_write.h
        ${LEGACY_LOCATION}/gltf/stb_image.h
        ${LEGACY_LOCATION}/gltf/tiny_gltf2_utils.cpp
        ${LEGACY_LOCATION}/gltf/tiny_gltf2_utils.h
        ${LEGACY_LOCATION}/gltf/tiny_gltf2.h
)

target_link_libraries(Plugin_GLTF 
    PRIVATE
        CompressonatorLIB
        Plugin_CmdLine

    PUBLIC
        Plugin_TCPluginAPI
        Plugin_Json
        CMP_MeshOptimizer
        CMP_MeshCompressor
        ExtGLM
)

target_include_directories(Plugin_GLTF 
    PUBLIC
        ./
        ./gltf/
        ${LEGACY_LOCATION}
        ${LEGACY_LOCATION}/gltf
)

set_target_properties(Plugin_GLTF PROPERTIES FOLDER ${FOLDER_NAME})

if (OPTION_CMP_DIRECTX)
add_subdirectory(dx12)
endif()

add_subdirectory(vulkan)
