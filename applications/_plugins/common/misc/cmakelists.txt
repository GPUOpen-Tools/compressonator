# =======================================================================

add_library(Plugin_Common_Misc INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ..)

target_sources(Plugin_Common_Misc INTERFACE

    ${LEGACY_LOCATION}/misc.cpp
    ${LEGACY_LOCATION}/misc.h
)

target_include_directories(Plugin_Common_Misc INTERFACE
    ./
    ${LEGACY_LOCATION}/
)

#set_target_properties(Plugin_Common_Misc PROPERTIES FOLDER ${FOLDER_NAME})
