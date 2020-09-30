# =======================================================================

add_library(Plugin_PluginManager)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../Common)

target_sources(Plugin_PluginManager 
PRIVATE
    ${LEGACY_LOCATION}/PluginBase.h
    ${LEGACY_LOCATION}/PluginInterface.h
    ${LEGACY_LOCATION}/PluginManager.cpp
    ${LEGACY_LOCATION}/PluginManager.h
    ${LEGACY_LOCATION}/Texture.h
)

target_include_directories(Plugin_PluginManager 
PUBLIC
    ./
    ${LEGACY_LOCATION}
    ${PROJECT_SOURCE_DIR}/CMP_CompressonatorLib
    ${PROJECT_SOURCE_DIR}/CMP_Framework/Common
    ${PROJECT_SOURCE_DIR}/CMP_Framework/Common/half
)

target_link_libraries(Plugin_PluginManager 
PUBLIC
    CompressonatorLIB
)

set_target_properties(Plugin_PluginManager PROPERTIES FOLDER ${FOLDER_NAME})
