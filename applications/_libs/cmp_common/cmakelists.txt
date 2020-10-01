
add_library(CMP_Common INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../../CMP_Framework)

target_sources(CMP_Common INTERFACE

    ${LEGACY_LOCATION}/Compute_Base.cpp
    ${LEGACY_LOCATION}/Compute_Base.h
    ${LEGACY_LOCATION}/Common/CMP_BoxFilter.cpp
    ${LEGACY_LOCATION}/Common/CMP_BoxFilter.h
    ${LEGACY_LOCATION}/Common/CMP_MIPS.cpp
    ${LEGACY_LOCATION}/Common/CMP_MIPS.h
    ${LEGACY_LOCATION}/Common/HDR_Encode.cpp
    ${LEGACY_LOCATION}/Common/HDR_Encode.h
)

target_include_directories(CMP_Common INTERFACE

    ./Common
    ./
    ${LEGACY_LOCATION}
    ${LEGACY_LOCATION}/Common
)

target_link_libraries(CMP_Common INTERFACE

    Plugin_Half
    Plugin_Common_KernelDef
    Plugin_Common_STBImage
    Plugin_CCMP_SDK
    Plugin_CCMP_Encode
)

# set_target_properties(CMP_Common PROPERTIES FOLDER ${FOLDER_NAME})
