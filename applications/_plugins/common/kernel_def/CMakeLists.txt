# =======================================================================

add_library(Plugin_Common_KernelDef INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../)

target_sources(Plugin_Common_KernelDef INTERFACE

    ${LEGACY_LOCATION}/common_kerneldef.h
)

target_include_directories(Plugin_Common_KernelDef INTERFACE
    ./
    ${LEGACY_LOCATION}
)

target_link_libraries(Plugin_Common_KernelDef INTERFACE

    CMP_Math
    CMP_Core
)

#set_target_properties(Plugin_Common_KernelDef PROPERTIES FOLDER ${FOLDER_NAME})
