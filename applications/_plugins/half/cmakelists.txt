
add_library(Plugin_Half)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../../cmp_framework/common/half)

target_sources(Plugin_Half PRIVATE

    ${LEGACY_LOCATION}/elut.h
    ${LEGACY_LOCATION}/half.cpp
    ${LEGACY_LOCATION}/half.h
    ${LEGACY_LOCATION}/halfexport.h
    ${LEGACY_LOCATION}/halffunction.h
    ${LEGACY_LOCATION}/halflimits.h
    ${LEGACY_LOCATION}/tofloat.h
)

target_include_directories(Plugin_Half PUBLIC

    ./
    ${LEGACY_LOCATION}
)

set_target_properties(Plugin_Half PROPERTIES FOLDER ${FOLDER_NAME})
