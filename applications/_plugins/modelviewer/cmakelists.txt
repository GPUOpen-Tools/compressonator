# =======================================================================

add_library(Plugin_Model_Viewer)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../common/model_viewer)

target_sources(Plugin_Model_Viewer PRIVATE

    ${LEGACY_LOCATION}/modeltexture.cpp
    ${LEGACY_LOCATION}/modeltexture.h
)

target_include_directories(Plugin_Model_Viewer PUBLIC
    ./
    ${LEGACY_LOCATION}/
)

target_link_libraries(Plugin_Model_Viewer PRIVATE

    CompressonatorLIB

    Plugin_PluginManager

    Plugin_Common_UtilFuncs
    Plugin_GLTF
)

set_target_properties(Plugin_Model_Viewer PROPERTIES FOLDER ${FOLDER_NAME})
