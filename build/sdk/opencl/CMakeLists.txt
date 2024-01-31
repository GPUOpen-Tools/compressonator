if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
    link_directories(${COMPRESSONATOR_ROOT_PATH}/../common/lib/ext/opencl/lib/x86_64)
else()
    link_directories(${COMPRESSONATOR_ROOT_PATH}/../common/lib/ext/opencl/lib/x86)
endif()

add_library(EncodeWith_OCL SHARED )

target_sources(EncodeWith_OCL PUBLIC

    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/cmp_gpu/opencl/compute_opencl.cpp
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/cmp_gpu/opencl/compute_opencl.h
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/cmp_gpu/opencl/copencl.cpp
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/cmp_gpu/opencl/copencl.h
)

target_include_directories(EncodeWith_OCL PRIVATE
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib         # compressonator.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_framework                 # compute_base.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_framework/common/half     # half.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_core/shaders              # common_def.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_core/source               # cmp_math_vec4.h
    ${COMPRESSONATOR_ROOT_PATH}/applications/_libs/cmp_math   #cmp_math_common.h
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/common  # tc_pluginapi.h
    ${COMPRESSONATOR_ROOT_PATH}/../common/lib/ext/opencl/include
)

target_link_libraries(EncodeWith_OCL 
PRIVATE
    CMP_Framework
    CMP_Compressonator
    OpenCL
)

set(BUILD_PLUGIN_TARGET ${CMAKE_BINARY_DIR}/bin/debug/plugin)

set_target_properties(EncodeWith_OCL PROPERTIES 
    FOLDER ${PROJECT_FOLDER_SDK_PLUGIN_DYNAMIC}
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
