# =======================================================================

add_library(Plugin_Common_STBImage INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../)

target_sources(Plugin_Common_STBImage INTERFACE

    ${LEGACY_LOCATION}/stb_image.h
)

target_include_directories(Plugin_Common_STBImage INTERFACE
    ./
    ${LEGACY_LOCATION}
)

#set_target_properties(Plugin_Common_STBImage PROPERTIES FOLDER ${FOLDER_NAME})