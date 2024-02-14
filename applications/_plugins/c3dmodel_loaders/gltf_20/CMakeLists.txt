
add_library(ModelLoader_gltf SHARED)

target_sources(ModelLoader_gltf PRIVATE
    gltf_20.cpp
    gltf_20.h
)

target_link_libraries(ModelLoader_gltf PRIVATE
    CMP_Compressonator
    CMP_Framework
    CMP_GUI_Gltf
    CMP_Common              # CModel Data
)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets HINTS ${QT_DIR})

target_include_directories(ModelLoader_gltf PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm/
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
)

target_compile_definitions(ModelLoader_gltf PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(ModelLoader_gltf PROPERTIES 
    FOLDER ${PROJECT_FOLDER_SDK_PLUGIN_DYNAMIC}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
)
