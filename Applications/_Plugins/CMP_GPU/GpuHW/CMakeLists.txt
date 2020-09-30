
add_library(Plugin_CMP_GPU_HW SHARED )

target_sources(Plugin_CMP_GPU_HW PUBLIC
    Compute_GpuHW.cpp
    Compute_GpuHW.h
    CGpuHW.cpp
    CGpuHW.h
    ${PROJECT_SOURCE_DIR}/External/glad/include/glad/glad.h
    ${PROJECT_SOURCE_DIR}/External/glad/src/glad.c
)

target_include_directories(Plugin_CMP_GPU_HW PRIVATE
    ${PROJECT_SOURCE_DIR}/CMP_Framework             # compute_base.h
    ${PROJECT_SOURCE_DIR}/CMP_Core/shaders          # common_def.h
    ${PROJECT_SOURCE_DIR}/CMP_Core/source           # cmp_math_vec4.h
    ${PROJECT_SOURCE_DIR}/Applications/_Libs/CMP_Math #cmp_math_common.h
    ${PROJECT_SOURCE_DIR}/External/glad/include/
    ${PROJECT_SOURCE_DIR}/External/glad/include/glad
)

target_link_libraries(Plugin_CMP_GPU_HW 
PRIVATE
    ExtGLM 
    ExtGLFW
    CMP_Framework
    CompressonatorLIB
    Plugin_PluginManager
    Plugin_TCPluginAPI
    GPU_Decode
)

set(BUILD_PLUGIN_TARGET ${CMAKE_BINARY_DIR}/bin/debug/plugin)

set_target_properties(Plugin_CMP_GPU_HW PROPERTIES 
    FOLDER ${FOLDER_NAME}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
