# =======================================================================

add_library(Plugin_TCPluginAPI)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../common)

target_sources(Plugin_TCPluginAPI PRIVATE
    ${LEGACY_LOCATION}/modeldata.cpp
    ${LEGACY_LOCATION}/modeldata.h
    ${LEGACY_LOCATION}/tc_pluginapi.h
    ${LEGACY_LOCATION}/tc_plugininternal.cpp
    ${LEGACY_LOCATION}/tc_plugininternal.h
)

target_include_directories(Plugin_TCPluginAPI PUBLIC
    ./
    ${LEGACY_LOCATION}
)

target_link_libraries(Plugin_TCPluginAPI
PRIVATE
    CMP_FileIO
    CompressonatorLIB
    CMP_MeshOptimizer
    Plugin_PluginManager

INTERFACE
    CMP_MeshOptimizer
    Plugin_PluginManager
)

set_target_properties(Plugin_TCPluginAPI PROPERTIES FOLDER ${FOLDER_NAME})
