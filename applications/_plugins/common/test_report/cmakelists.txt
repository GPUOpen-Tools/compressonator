# =======================================================================

add_library(Plugin_Common_TestReport INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../)

target_sources(Plugin_Common_TestReport INTERFACE

    ${LEGACY_LOCATION}/testreport.h
)

target_include_directories(Plugin_Common_TestReport INTERFACE
    ./
    ${LEGACY_LOCATION}
)


#set_target_properties(Plugin_PSNR PROPERTIES FOLDER ${FOLDER_NAME})
