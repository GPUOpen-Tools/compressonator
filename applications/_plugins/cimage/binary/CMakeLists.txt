cmake_minimum_required(VERSION 3.10)

add_library(Image_BINARY STATIC "")

target_sources(Image_BINARY
    PRIVATE
    binary.cpp
    binary.h
)

target_include_directories(Image_BINARY
    PRIVATE
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    ${PROJECT_SOURCE_DIR}/applications/_plugins/cimage/brlg
)

target_link_libraries(Image_BINARY 
    PRIVATE
    CMP_Compressonator
)

set_target_properties(Image_BINARY PROPERTIES 
    FOLDER "Plugin_Static/ImageIO"
)
