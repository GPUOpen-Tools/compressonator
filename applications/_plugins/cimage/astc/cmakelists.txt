cmake_minimum_required(VERSION 3.10)

add_library(Image_ASTC STATIC "")

target_sources(Image_ASTC
    PRIVATE
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/tc_pluginapi.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/tc_plugininternal.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/tc_plugininternal.cpp
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/utilfuncs.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/utilfuncs.cpp
    ./castc.cpp
    ./castc.h
    )

target_include_directories(Image_ASTC
    PRIVATE
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
     ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    )

if (UNIX)
    target_compile_definitions(Image_ASTC PRIVATE _LINUX)
endif()

set_target_properties(Image_ASTC PROPERTIES FOLDER "Plugin_Static/ImageIO")

