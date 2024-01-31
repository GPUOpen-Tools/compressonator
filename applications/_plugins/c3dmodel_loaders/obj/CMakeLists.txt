
add_library(ModelLoader_obj SHARED)

file(GLOB_RECURSE SRCS

    "*.cpp"
    "*.h"
)

target_sources(ModelLoader_obj PRIVATE

    ${SRCS}
)

target_link_libraries(ModelLoader_obj PRIVATE
    CMP_Compressonator
    CMP_Framework
    CMP_Common
)

target_include_directories(ModelLoader_obj PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
)

target_compile_definitions(ModelLoader_obj PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(ModelLoader_obj PROPERTIES 
    FOLDER ${PROJECT_FOLDER_SDK_PLUGIN_DYNAMIC}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
)
