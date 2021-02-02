
add_library(CMP_Common INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../../cmp_framework)

target_sources(CMP_Common INTERFACE

    ${LEGACY_LOCATION}/compute_base.cpp
    ${LEGACY_LOCATION}/compute_base.h
    ${LEGACY_LOCATION}/common/cmp_boxfilter.cpp
    ${LEGACY_LOCATION}/common/cmp_boxfilter.h
    ${LEGACY_LOCATION}/common/cmp_mips.cpp
    ${LEGACY_LOCATION}/common/cmp_mips.h
    ${LEGACY_LOCATION}/common/hdr_encode.cpp
    ${LEGACY_LOCATION}/common/hdr_encode.h
)

target_include_directories(CMP_Common INTERFACE

    ./common
    ./
    ${LEGACY_LOCATION}
    ${LEGACY_LOCATION}/common
)

target_link_libraries(CMP_Common INTERFACE

    Plugin_Half
    Plugin_Common_KernelDef
    Plugin_Common_STBImage
    Plugin_CCMP_SDK
    Plugin_CCMP_Encode
)

# set_target_properties(CMP_Common PROPERTIES FOLDER ${FOLDER_NAME})
