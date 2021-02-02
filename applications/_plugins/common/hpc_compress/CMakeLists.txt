# =======================================================================

add_library(Plugin_Common_HPCCompress INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../)

target_sources(Plugin_Common_HPCCompress INTERFACE

    ${LEGACY_LOCATION}/hpc_compress.h
)

target_include_directories(Plugin_Common_HPCCompress INTERFACE
    ./
    ${LEGACY_LOCATION}
)

#set_target_properties(Plugin_Common_HPCCompress PROPERTIES FOLDER ${FOLDER_NAME})
