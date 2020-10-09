
add_library(Plugin_GLTF_Vulkan INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../common/gltf/vulkan)

target_sources(Plugin_GLTF_Vulkan INTERFACE

    ${LEGACY_LOCATION}/gltfbboxpassvk.cpp
    ${LEGACY_LOCATION}/gltfbboxpassvk.h
    ${LEGACY_LOCATION}/gltfdepthpassvk.cpp
    ${LEGACY_LOCATION}/gltfdepthpassvk.h
    ${LEGACY_LOCATION}/gltfpbrvk.cpp
    ${LEGACY_LOCATION}/gltfpbrvk.h
    ${LEGACY_LOCATION}/gltftechnique.cpp
    ${LEGACY_LOCATION}/gltftechnique.h
    ${LEGACY_LOCATION}/gltfhelpers_vulkan.cpp
    ${LEGACY_LOCATION}/gltfhelpers_vulkan.h
)

target_include_directories(Plugin_GLTF_Vulkan INTERFACE
    ./
    ${LEGACY_LOCATION}
)

target_link_libraries(Plugin_GLTF_Vulkan INTERFACE

    Plugin_GLTF
)

# set_target_properties(Plugin_GLTF_Vulkan PROPERTIES FOLDER ${FOLDER_NAME})
