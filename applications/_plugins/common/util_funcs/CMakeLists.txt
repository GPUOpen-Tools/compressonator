# =======================================================================

add_library(Plugin_Common_UtilFuncs INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../)

target_sources(Plugin_Common_UtilFuncs INTERFACE

    ${LEGACY_LOCATION}/utilfuncs.cpp
    ${LEGACY_LOCATION}/utilfuncs.h
)

target_include_directories(Plugin_Common_UtilFuncs INTERFACE
    ./
    ${LEGACY_LOCATION}
)

#set_target_properties(Plugin_Common_UtilFuncs PROPERTIES FOLDER ${FOLDER_NAME})
