
add_library(CMP_FileIO)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../_Plugins/Common)

target_sources(CMP_FileIO PRIVATE

    ${LEGACY_LOCATION}/CMP_FileIO.cpp
    ${LEGACY_LOCATION}/CMP_FileIO.h
)

target_include_directories(CMP_FileIO PUBLIC

    ${LEGACY_LOCATION}
    ./
)

set_target_properties(CMP_FileIO PROPERTIES FOLDER ${FOLDER_NAME})
