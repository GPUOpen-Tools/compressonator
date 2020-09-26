
add_library(Plugin_GLTF_Vulkan INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../Common/gltf/Vulkan)

target_sources(Plugin_GLTF_Vulkan INTERFACE

    ${LEGACY_LOCATION}/GltfBBoxPassVK.cpp
	${LEGACY_LOCATION}/GltfBBoxPassVK.h
	${LEGACY_LOCATION}/GltfDepthPassVK.cpp
	${LEGACY_LOCATION}/GltfDepthPassVK.h
	${LEGACY_LOCATION}/GltfPbrVK.cpp
	${LEGACY_LOCATION}/GltfPbrVK.h
	${LEGACY_LOCATION}/GltfTechnique.cpp
	${LEGACY_LOCATION}/GltfTechnique.h
	${LEGACY_LOCATION}/glTFHelpers_Vulkan.cpp
	${LEGACY_LOCATION}/glTFHelpers_Vulkan.h
)

target_include_directories(Plugin_GLTF_Vulkan INTERFACE
    ./
	${LEGACY_LOCATION}
)

target_link_libraries(Plugin_GLTF_Vulkan INTERFACE

	Plugin_GLTF
)

# set_target_properties(Plugin_GLTF_Vulkan PROPERTIES FOLDER ${FOLDER_NAME})
