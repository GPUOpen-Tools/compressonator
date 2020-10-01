
add_library(Plugin_GLTF_DX12 INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../Common/gltf/DX12)

target_sources(Plugin_GLTF_DX12 INTERFACE

    ${LEGACY_LOCATION}/GltfBBoxPass.cpp
    ${LEGACY_LOCATION}/GltfBBoxPass.h
    ${LEGACY_LOCATION}/GltfDepthPass.cpp
    ${LEGACY_LOCATION}/GltfDepthPass.h
    ${LEGACY_LOCATION}/GltfPbr.cpp
    ${LEGACY_LOCATION}/GltfPbr.h
    ${LEGACY_LOCATION}/GltfTechnique.cpp
    ${LEGACY_LOCATION}/GltfTechnique.h
    ${LEGACY_LOCATION}/glTFHelpers_DX12.cpp
    ${LEGACY_LOCATION}/glTFHelpers_DX12.h
    ${LEGACY_LOCATION}/gltfGetFormat_DX12.cpp
    ${LEGACY_LOCATION}/gltfGetFormat_DX12.h
)

target_include_directories(Plugin_GLTF_DX12 INTERFACE
    ./
    ${LEGACY_LOCATION}
)

target_link_libraries(Plugin_GLTF_DX12 INTERFACE

    Plugin_GLTF
)

# set_target_properties(Plugin_GLTF_DX12 PROPERTIES FOLDER ${FOLDER_NAME})
