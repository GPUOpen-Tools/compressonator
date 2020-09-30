
add_library(Plugin_CImage_ASTC)

target_sources(Plugin_CImage_ASTC PRIVATE

    cASTC.h
    cASTC.cpp
)

target_link_libraries(Plugin_CImage_ASTC PRIVATE

    CompressonatorLIB
    CMP_FrameworkLIB

    Plugin_TCPluginAPI
    Plugin_Common_UtilFuncs
)

set_target_properties(Plugin_CImage_ASTC PROPERTIES FOLDER ${FOLDER_NAME})
