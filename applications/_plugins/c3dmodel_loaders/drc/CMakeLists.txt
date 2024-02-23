
add_library(ModelLoader_drc SHARED)

target_sources(ModelLoader_drc PRIVATE

    modelloader_drc.cpp
    modelloader_drc.h
)

target_link_libraries(ModelLoader_drc PRIVATE
    CMP_Compressonator
    CMP_Framework
)

# Find the QtWidgets library
find_package(Qt5 REQUIRED COMPONENTS Core Widgets HINTS ${QT_DIR})

target_include_directories(ModelLoader_drc PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
)

target_compile_definitions(ModelLoader_drc PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(ModelLoader_drc PROPERTIES 
    FOLDER ${PROJECT_FOLDER_SDK_PLUGIN_DYNAMIC}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
)
