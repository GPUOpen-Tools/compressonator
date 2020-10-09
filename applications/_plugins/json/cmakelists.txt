# =======================================================================

add_library(Plugin_Json INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../common)

target_sources(Plugin_Json INTERFACE

    ${LEGACY_LOCATION}/json/json.h
)

target_include_directories(Plugin_Json INTERFACE
    json
    ./
    ${LEGACY_LOCATION}
)

#set_target_properties(Plugin_Json PROPERTIES FOLDER ${FOLDER_NAME})
