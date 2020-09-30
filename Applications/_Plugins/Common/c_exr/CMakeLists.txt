# =======================================================================

add_library(Plugin_Common_cExr INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../)

target_sources(Plugin_Common_cExr INTERFACE

    ${LEGACY_LOCATION}/cExr.cpp
    ${LEGACY_LOCATION}/cExr.h
    ${LEGACY_LOCATION}/namespaceAlias.h
)

target_include_directories(Plugin_Common_cExr INTERFACE
    ./
    ${LEGACY_LOCATION}
)

target_link_libraries(Plugin_Common_cExr INTERFACE

    CMP_FileIO
)

#set_target_properties(Plugin_Common_cExr PROPERTIES FOLDER ${FOLDER_NAME})
