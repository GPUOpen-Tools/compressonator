# =======================================================================

add_library(Plugin_Common_ATIFormats)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../common)

target_sources(Plugin_Common_ATIFormats PRIVATE

    ${LEGACY_LOCATION}/atiformats.cpp
    ${LEGACY_LOCATION}/atiformats.h
)

target_include_directories(Plugin_Common_ATIFormats PUBLIC
    ./
    ${LEGACY_LOCATION}
)

target_link_libraries(Plugin_Common_ATIFormats PRIVATE
    CompressonatorLIB
    Plugin_PluginManager
)

set_target_properties(Plugin_Common_ATIFormats PROPERTIES FOLDER ${FOLDER_NAME})
