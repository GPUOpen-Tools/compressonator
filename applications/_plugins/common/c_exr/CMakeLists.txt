# =======================================================================

add_library(Plugin_Common_cExr INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../)

target_sources(Plugin_Common_cExr INTERFACE

    ${LEGACY_LOCATION}/cexr.cpp
    ${LEGACY_LOCATION}/cexr.h
    ${LEGACY_LOCATION}/namespacealias.h
)

target_include_directories(Plugin_Common_cExr INTERFACE
    ./
    ${LEGACY_LOCATION}
)

target_link_libraries(Plugin_Common_cExr INTERFACE

    CMP_FileIO
)

#set_target_properties(Plugin_Common_cExr PROPERTIES FOLDER ${FOLDER_NAME})
