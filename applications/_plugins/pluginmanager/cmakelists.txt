# =======================================================================

add_library(Plugin_PluginManager)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../common)

target_sources(Plugin_PluginManager 
PRIVATE
    ${LEGACY_LOCATION}/pluginbase.h
    ${LEGACY_LOCATION}/plugininterface.h
    ${LEGACY_LOCATION}/pluginmanager.cpp
    ${LEGACY_LOCATION}/pluginmanager.h
    ${LEGACY_LOCATION}/texture.h
)

target_include_directories(Plugin_PluginManager 
PUBLIC
    ./
    ${LEGACY_LOCATION}
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
)

target_link_libraries(Plugin_PluginManager 
PUBLIC
    CompressonatorLIB
)

set_target_properties(Plugin_PluginManager PROPERTIES FOLDER ${FOLDER_NAME})
