
add_library(EncodeWith_GPU SHARED )

target_sources(EncodeWith_GPU PUBLIC
    compute_gpuhw.cpp
    compute_gpuhw.h
    cgpuhw.cpp
    cgpuhw.h
    ${PROJECT_SOURCE_DIR}/external/glad/include/glad/glad.h
    ${PROJECT_SOURCE_DIR}/external/glad/src/glad.c
)

target_include_directories(EncodeWith_GPU PRIVATE
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib         # compressonator.h
    ${PROJECT_SOURCE_DIR}/cmp_framework                 # compute_base.h
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half     # half.h
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders              # common_def.h
    ${PROJECT_SOURCE_DIR}/cmp_core/source               # cmp_math_vec4.h
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math   # cmp_math_common.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common  # tc_pluginapi.h
    ${PROJECT_SOURCE_DIR}/external/glad/include/
    ${PROJECT_SOURCE_DIR}/external/glad/include/glad
)

target_link_libraries(EncodeWith_GPU 
PRIVATE
    ExtGLM 
    ExtGLFW
    CMP_Framework
    CMP_Compressonator
)

set(BUILD_PLUGIN_TARGET ${CMAKE_BINARY_DIR}/bin/debug/plugin)

set_target_properties(EncodeWith_GPU PROPERTIES 
    FOLDER "Plugin_Dynamic/GPU_EncodeWith"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
