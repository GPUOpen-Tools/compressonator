cmake_minimum_required(VERSION 2.8.11)
set(SAMPLES_DATA_DIR ${SAMPLES_DATA_DIR} "${PROJECT_SOURCE_DIR}/API-Samples/data")
set(SHADER_FILES ${SHADER_FILES} "")
include_directories( ${SAMPLES_DATA_DIR} ${GLSLANG_SPIRV_INCLUDE_DIR} ${GLMINC_PREFIX})

# Additional includes for spirv-tools
include_directories(${SPIRV_TOOLS_INCLUDE_DIR})
                    
# function to build a single-source-file sample that uses glsl shaders;
#   - assumes SAMPLE_GLSL_VERT_SHADERS and SAMPLE_GLSL_FRAG_SHADERS is set to
#     the list of shader files of that type, with .vert and .frag omitted
#   - uses glslangValidator to convert glsl to spirv
function(sampleExtGLSLShaders SNAME)
    set(SAMPLE_NAME ${SNAME})
    set(SHADER_DIR ${SAMPLES_DATA_DIR}/${VULKAN_VERSION})
    foreach(SFILE ${SAMPLE_GLSL_VERT_SHADERS})
        add_custom_command(OUTPUT ${SHADER_DIR}/${SFILE}-vert.spv
            COMMAND ${GLSLANG_VALIDATOR} -s -V ${SHADER_DIR}/${SFILE}.vert
            COMMAND ${MOVE_CMD} vert.spv ${SHADER_DIR}/${SFILE}-vert.spv
            DEPENDS ${SHADER_DIR}/${SFILE}.vert ${GLSLANG_VALIDATOR}
        )
        set(SHADER_FILES ${SHADER_FILES} ${SHADER_DIR}/${SFILE}-vert.spv)
    endforeach(SFILE)
    foreach(SFILE ${SAMPLE_GLSL_FRAG_SHADERS})
        add_custom_command(OUTPUT ${SHADER_DIR}/${SFILE}-frag.spv
            COMMAND ${GLSLANG_VALIDATOR} -s -V ${SHADER_DIR}/${SFILE}.frag
            COMMAND ${MOVE_CMD} frag.spv ${SHADER_DIR}/${SFILE}-frag.spv
            DEPENDS ${SHADER_DIR}/${SFILE}.frag ${GLSLANG_VALIDATOR}
        )
        set(SHADER_FILES ${SHADER_FILES} ${SHADER_DIR}/${SFILE}-frag.spv)
    endforeach(SFILE)
    add_executable(${SAMPLE_NAME} ${SAMPLE_NAME}/${SAMPLE_NAME}.cpp ${SHADER_FILES})
    target_link_libraries(${SAMPLE_NAME} ${VULKAN_LOADER} ${UTILS_NAME} ${GLSLANG_LIBRARIES} ${PTHREAD})
endfunction(sampleExtGLSLShaders)

# function to build a single-source-file sample that uses spirv shaders
#   - assumes SAMPLE_SPIRV_SHADERS is set to list to list of spirv shsders
#     used by the sample, with the .spv omitted
function(sampleExtSPIRVShaders SNAME)
    set(SAMPLE_NAME ${SNAME})
    set(SHADER_DIR ${SAMPLES_DATA_DIR}/${VULKAN_VERSION})
    foreach(SFILE ${SAMPLE_SPIRV_SHADERS})
        set(SHADER_FILES ${SHADER_FILES} ${SHADER_DIR}/${SFILE}.spv)
    endforeach(SFILE)
    add_executable(${SAMPLE_NAME} ${SAMPLE_NAME}/${SAMPLE_NAME}.cpp ${SHADER_FILES})
    target_link_libraries(${SAMPLE_NAME} ${VULKAN_LOADER} ${UTILS_NAME} ${GLSLANG_LIBRARIES} ${PTHREAD})
endfunction(sampleExtSPIRVShaders)

# function to build a simple single-source-file sample
#   - assumes S_TARGETS set to list of build targets
function(sampleWithSingleFile)
    foreach(TARG ${S_TARGETS})
        set(SAMPLE_NAME ${TARG})
        if(UNIX)
            add_executable(${SAMPLE_NAME} ${SAMPLE_NAME}/${SAMPLE_NAME}.cpp)
            target_link_libraries(${SAMPLE_NAME} ${UTILS_NAME} ${GLSLANG_LIBRARIES} ${XCB_LIBRARIES} ${VULKAN_LOADER} ${PTHREAD} ${SPIRV_TOOLS_LIBRARIES})
        else()
            add_executable(${SAMPLE_NAME} WIN32 ${SAMPLE_NAME}/${SAMPLE_NAME}.cpp)
            target_link_libraries(${SAMPLE_NAME} ${UTILS_NAME} ${GLSLANG_LIBRARIES} ${VULKAN_LOADER} ${WINLIBS} ${SPIRV_TOOLS_LIBRARIES})
        endif()
    endforeach(TARG)
endfunction(sampleWithSingleFile)

# function to build a simple single-source-file sample
#   - assumes that code is standalone and doesn't depend on utilitites
#   - assumes S_TARGETS set to list of build targets
function(sampleInlineGLSLShaders)
    foreach(TARG ${S_TARGETS})
        set(SAMPLE_NAME ${TARG})
        if(UNIX)
            add_executable(${SAMPLE_NAME} ${SAMPLE_NAME}/${SAMPLE_NAME}.cpp)
            target_link_libraries(${SAMPLE_NAME} ${GLSLANG_LIBRARIES} ${XCB_LIBRARIES} ${VULKAN_LOADER} ${PTHREAD})
        else()
            add_executable(${SAMPLE_NAME} WIN32 ${SAMPLE_NAME}/${SAMPLE_NAME}.cpp)
            target_link_libraries(${SAMPLE_NAME} ${GLSLANG_LIBRARIES} ${VULKAN_LOADER} ${WINLIBS})
        endif()
    endforeach(TARG)
endfunction(sampleInlineGLSLShaders)

if(WIN32)
    set (MOVE_CMD "move")
    set (VULKAN_LOADER_NAME "vulkan-${MAJOR}")
else()
    set (MOVE_CMD "mv")
    set (PTHREAD "pthread")
    set (VULKAN_LOADER_NAME "vulkan")
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/layers")
    set (VULKAN_LOADER ${VULKAN_LOADER_NAME})
else()
    find_library(VULKAN_LOADER NAMES ${VULKAN_LOADER_NAME}
                 HINTS "${PROJECT_SOURCE_DIR}/../${BINDATA_DIR}" )
endif()

set (LIBGLM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libs)

if(NOT WIN32)
    include_directories(
        ${XCB_INCLUDE_DIRS}
    )
    link_libraries(${XCB_LIBRARIES} ${VULKAN_LOADER} m )
endif()
if(WIN32)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES")

    # If MSVC, disable some signed/unsigned mismatch warnings.
    if (MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4267")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267")
    endif()

else()
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
endif()

if (NOT WIN32)
    # extra setup for out-of-tree builds
    if (NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        add_custom_target(samples-binary-dir-symlinks ALL
            COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/run_all_samples.sh
            COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/get-short-descripts.sh
            VERBATIM
            )
    endif()
else()
    if (NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lunarg.ppm LUNARG)
        FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/run_all_samples.ps1 RUN_ALL)
        add_custom_target(samples-binary-dir-symlinks ALL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RUN_ALL} run_all_samples.ps1
            VERBATIM
            )
    endif()
endif()


#
# START BUILDING SAMPLES HERE
#

# simple one file sample targets, no additional files
set (S_TARGETS instance device enumerate enumerate-adv
    depthbuffer initswapchain inittexture uniformbuffer
    descriptor_pipeline_layouts initcommandbuffer
    initrenderpass initframebuffers vertexbuffer
    allocdescriptorsets initshaders initpipeline multithreadcmdbuf
    drawcube drawtexturedcube dynamicuniform texelbuffer
    instance_layer_properties instance_extension_properties
    instance_layer_extension_properties dbgcreatemsgcallback
    enable_validation_with_callback multiple_sets
    copyblitimage template separate_image_sampler
    occlusion_query pipeline_cache pipeline_derivative
    immutable_sampler push_constants drawsubpasses secondarycmd
    spirv_assembly spirv_specialization)
sampleWithSingleFile()

add_subdirectory(utils)

# targets that use spv shaders

# usespirvshader
# set(SAMPLE_SPIRV_SHADERS spirvshader-vert spirvshader-frag)
# sampleExtSPIRVShaders()

# targets that require glsl to spv shader conversion

# useglslshader
# set(SAMPLE_GLSL_FRAG_SHADERS glslshader)
# set(SAMPLE_GLSL_VERT_SHADERS glslshader)
# sampleExtGLSLShaders()

