link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/opencl/lib/x86_64
    )

add_library(EncodeWith_OCL SHARED )

target_sources(EncodeWith_OCL PUBLIC

    compute_opencl.cpp
    compute_opencl.h
    copencl.cpp
    copencl.h
)

target_include_directories(EncodeWith_OCL PRIVATE
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib         # compressonator.h
    ${PROJECT_SOURCE_DIR}/cmp_framework                 # compute_base.h
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half     # half.h
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders              # common_def.h
    ${PROJECT_SOURCE_DIR}/cmp_core/source               # cmp_math_vec4.h
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math   #cmp_math_common.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common  # tc_pluginapi.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/opencl/include
)

target_link_libraries(EncodeWith_OCL 
PRIVATE
    CMP_Framework
    CMP_Compressonator
    OpenCL
)

set(BUILD_PLUGIN_TARGET ${CMAKE_BINARY_DIR}/bin/debug/plugin)

set_target_properties(EncodeWith_OCL PROPERTIES 
    FOLDER "Plugin_Dynamic/GPU_EncodeWith"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
