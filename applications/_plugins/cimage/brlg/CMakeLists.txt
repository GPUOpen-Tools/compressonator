cmake_minimum_required(VERSION 3.10)

add_library(Image_BRLG STATIC "")

target_sources(Image_BRLG
    PRIVATE
    brlg.cpp
    brlg.h
)

target_include_directories(Image_BRLG
    PRIVATE
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    )

target_link_libraries(Image_BRLG 
    PRIVATE
    CMP_Compressonator
    )

set_target_properties(Image_BRLG PROPERTIES 
    FOLDER "Plugin_Static/ImageIO"
    )
