
add_library(Plugin_Half)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../../CMP_Framework/Common/half)

target_sources(Plugin_Half PRIVATE

    ${LEGACY_LOCATION}/eLut.h
    ${LEGACY_LOCATION}/half.cpp
    ${LEGACY_LOCATION}/half.h
    ${LEGACY_LOCATION}/halfExport.h
    ${LEGACY_LOCATION}/halfFunction.h
    ${LEGACY_LOCATION}/halfLimits.h
    ${LEGACY_LOCATION}/toFloat.h
)

target_include_directories(Plugin_Half PUBLIC

    ./
    ${LEGACY_LOCATION}
)

set_target_properties(Plugin_Half PROPERTIES FOLDER ${FOLDER_NAME})
