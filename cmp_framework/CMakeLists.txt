cmake_minimum_required(VERSION 3.10)

add_library(CMP_Framework STATIC "")


if(CMP_HOST_WINDOWS)
    target_compile_definitions(CMP_Framework PUBLIC
        -DCMP_USE_XMMINTRIN
        )
endif()

file(GLOB_RECURSE HALF_SRC
        ${PROJECT_SOURCE_DIR}/cmp_framework/common/half/*.h
        ${PROJECT_SOURCE_DIR}/cmp_framework/common/half/*.cpp
        )
file(GLOB_RECURSE DDS_SRC
        ${PROJECT_SOURCE_DIR}/applications/_plugins/cimage/dds/*.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/cimage/dds/*.cpp
        )
file(GLOB_RECURSE HPC_SRC
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_encode/hpc/*.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_encode/hpc/*.cpp
        )
file(GLOB_RECURSE CORE_SHADERS_SRC
        ${PROJECT_SOURCE_DIR}/cmp_core/shaders/*.h
        ${PROJECT_SOURCE_DIR}/cmp_core/shaders/*.cpp
        )
file(GLOB_RECURSE CORE_SRC
        ${PROJECT_SOURCE_DIR}/cmp_core/source/*.h
        ${PROJECT_SOURCE_DIR}/cmp_core/source/*.cpp
        )
file(GLOB_RECURSE CMP_FRAMEWORK_SRC
        ${PROJECT_SOURCE_DIR}/cmp_framework/*.h
        ${PROJECT_SOURCE_DIR}/cmp_framework/*.cpp
        )
file(GLOB_RECURSE CMP_FRAMEWORK_COMMON_SRC
        ${PROJECT_SOURCE_DIR}/cmp_framework/common/*.h
        ${PROJECT_SOURCE_DIR}/cmp_framework/common/*.cpp
        )

target_sources(CMP_Framework
        PRIVATE
        ${HALF_SRC}
        ${DDS_SRC}
        ${HPC_SRC}
        ${CORE_SHADERS_SRC}
        ${CORE_SRC}
        ${CMP_FRAMEWORK_SRC}
        ${CMP_FRAMEWORK_COMMON_SRC}
        
        # CMP_Math
        ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math/cmp_math_common.h
        ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math/cmp_math_common.cpp
        ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math/cmp_math_cpuid.h
        ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math/cmp_math_cpuid.cpp
        
        # CMP_COMMON
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/atiformats.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/atiformats.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/pluginbase.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/plugininterface.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/pluginmanager.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/pluginmanager.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/query_timer.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/query_timer.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/stb_image.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf/stb_image_write.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/tc_pluginapi.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/tc_plugininternal.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/tc_plugininternal.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/utilfuncs.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/utilfuncs.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/cmp_fileio.h 
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/cmp_fileio.cpp

        # CMP_SDK
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bcn.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bcn.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc1/bc1.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc1/bc1.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc2/bc2.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc2/bc2.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc3/bc3.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc3/bc3.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc4/bc4.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc4/bc4.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc5/bc5.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc5/bc5.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc6/bc6h.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc6/bc6h.cpp
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc7/bc7.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/bc7/bc7.cpp
        )


if (OPTION_CMP_GTC)
target_sources(CMP_Framework
        PRIVATE
        # Extra 
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/gtc/gtc.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/gtc/gtc.cpp
        )
endif()

if (OPTION_CMP_APC)
target_sources(CMP_Framework
        PRIVATE
        # Extra 
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/apc/apc.h
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk/apc/apc.cpp
        )
endif()

target_include_directories(CMP_Framework
        PRIVATE
        ./
        ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
        ${PROJECT_SOURCE_DIR}/cmp_core/source
        ${PROJECT_SOURCE_DIR}/cmp_core/shaders
        ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
        ${PROJECT_SOURCE_DIR}/applications/_plugins/ccmp_sdk
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common
        ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf
        ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math
        ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
        )

if (UNIX)
    target_compile_definitions(CMP_Framework PRIVATE _LINUX)
endif()

set_target_properties(CMP_Framework PROPERTIES FOLDER "Libs")
