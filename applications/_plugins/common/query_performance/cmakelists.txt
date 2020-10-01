# =======================================================================

add_library(Plugin_Common_QueryPerformance INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../)

target_sources(Plugin_Common_QueryPerformance INTERFACE

    ${LEGACY_LOCATION}/query_timer.cpp
    ${LEGACY_LOCATION}/query_timer.h
)

target_include_directories(Plugin_Common_QueryPerformance INTERFACE
    ./
    ${LEGACY_LOCATION}
)

#set_target_properties(Plugin_Common_QueryPerformance PROPERTIES FOLDER ${FOLDER_NAME})
