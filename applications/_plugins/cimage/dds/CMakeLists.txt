cmake_minimum_required(VERSION 3.10)

add_library(Image_DDS STATIC "")

target_sources(Image_DDS 
    PRIVATE
    dds.cpp
    dds.h
    dds_dx10.cpp
    dds_dx10.h
    dds_file.cpp
    dds_file.h
    dds_helpers.cpp
    dds_helpers.h
    )

target_include_directories(Image_DDS
    PRIVATE
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    )

target_link_libraries(Image_DDS 
    PRIVATE
    CMP_Compressonator
    )

set_target_properties(Image_DDS PROPERTIES 
    FOLDER "Plugin_Static/ImageIO"
    )
