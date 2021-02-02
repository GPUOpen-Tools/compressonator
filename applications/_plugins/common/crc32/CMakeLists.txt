# =======================================================================

add_library(Plugin_Common_Crc32 INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../)

target_sources(Plugin_Common_Crc32 INTERFACE

    ${LEGACY_LOCATION}/crc32.h
)

target_include_directories(Plugin_Common_Crc32 INTERFACE
    ./
    ${LEGACY_LOCATION}
)

#set_target_properties(Plugin_Common_Crc32 PROPERTIES FOLDER ${FOLDER_NAME})
