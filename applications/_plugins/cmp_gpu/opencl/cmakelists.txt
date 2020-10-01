link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/opencl/lib/x86_64
    )

add_library(Plugin_CMP_GPU_OCL SHARED )

target_sources(Plugin_CMP_GPU_OCL PUBLIC

    Compute_OpenCL.cpp
    Compute_OpenCL.h
    COpenCL.cpp
    COpenCL.h
)

target_include_directories(Plugin_CMP_GPU_OCL PRIVATE
    ${PROJECT_SOURCE_DIR}/CMP_Framework             # compute_base.h
    ${PROJECT_SOURCE_DIR}/CMP_Core/shaders          # common_def.h
    ${PROJECT_SOURCE_DIR}/CMP_Core/source           # cmp_math_vec4.h
    ${PROJECT_SOURCE_DIR}/Applications/_Libs/CMP_Math #cmp_math_common.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/opencl/include
)

target_link_libraries(Plugin_CMP_GPU_OCL 
PRIVATE
    CMP_Framework
    CompressonatorLIB
    Plugin_PluginManager
    Plugin_TCPluginAPI
    GPU_Decode
    OpenCL
)

set(BUILD_PLUGIN_TARGET ${CMAKE_BINARY_DIR}/bin/debug/plugin)

set_target_properties(Plugin_CMP_GPU_OCL PROPERTIES 
    FOLDER ${FOLDER_NAME}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
