
add_library(Plugin_GLTF_DX12 INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../common/gltf/dx12)

target_sources(Plugin_GLTF_DX12 INTERFACE

    ${LEGACY_LOCATION}/gltfbboxpass.cpp
    ${LEGACY_LOCATION}/gltfbboxpass.h
    ${LEGACY_LOCATION}/gltfdepthpass.cpp
    ${LEGACY_LOCATION}/gltfdepthpass.h
    ${LEGACY_LOCATION}/gltfpbr.cpp
    ${LEGACY_LOCATION}/gltfpbr.h
    ${LEGACY_LOCATION}/gltftechnique.cpp
    ${LEGACY_LOCATION}/gltftechnique.h
    ${LEGACY_LOCATION}/gltfhelpers_dx12.cpp
    ${LEGACY_LOCATION}/gltfhelpers_dx12.h
    ${LEGACY_LOCATION}/gltfgetformat_dx12.cpp
    ${LEGACY_LOCATION}/gltfgetformat_dx12.h
)

target_include_directories(Plugin_GLTF_DX12 INTERFACE
    ./
    ${LEGACY_LOCATION}
)

target_link_libraries(Plugin_GLTF_DX12 INTERFACE

    Plugin_GLTF
)

# set_target_properties(Plugin_GLTF_DX12 PROPERTIES FOLDER ${FOLDER_NAME})
