cmake_minimum_required(VERSION 3.10)


# Reserved for Future releases
file(GLOB_RECURSE CMP_CORE_SRC
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc1_encode_kernel.h
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc1_encode_kernel.cpp
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc2_encode_kernel.h
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc2_encode_kernel.cpp
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc3_encode_kernel.h
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc3_encode_kernel.cpp
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc4_encode_kernel.h
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc4_encode_kernel.cpp
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc5_encode_kernel.h
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc5_encode_kernel.cpp
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc6_encode_kernel.h
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc6_encode_kernel.cpp
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc7_encode_kernel.h
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bc7_encode_kernel.cpp
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bcn_common_kernel.h
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/bcn_common_api.h
      ${PROJECT_SOURCE_DIR}/cmp_core/source/cmp_core.h
      ${PROJECT_SOURCE_DIR}/cmp_core/source/cmp_math_func.h
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/
      ${PROJECT_SOURCE_DIR}/cmp_core/shaders/common_def.h
      ${PROJECT_SOURCE_DIR}/cmp_core/source/cmp_math_vec4.h
)

file(GLOB_RECURSE CMP_SRCS
    # Lossy Compression
    "apc/*.h"
    "apc/*.cpp"
    "atc/*.h"
    "atc/*.cpp"
    "ati/*.h"
    "ati/*.cpp"
    "ati/*.c"
    "basis/*.h"
    "basis/*.cpp"
    "bc6h/*.h"
    "bc6h/*.cpp"
    "bc7/*.h"
    "bc7/*.cpp"
    "block/*.h"
    "block/*.cpp"
    "buffer/*.h"
    "buffer/*.cpp"
    "dxt/*.h"
    "dxt/*.cpp"
    "dxtc/*.h"
    "dxtc/*.cpp"
    "dxtc/*.c"
    "etc/*.h"
    "etc/*.cpp"
    "etc/etcpack/*.h"
    "etc/etcpack/*.cpp"
    "etc/etcpack/*.cxx"
    "gt/*.h"
    "gt/*.cpp"

    # Common 
    "common/*.h"
    "common/*.cpp"
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/*.h
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/*.cpp
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half/*.h
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half/*.cpp
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/atiformats.cpp
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/atiformats.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/format_conversion.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/format_conversion.cpp
)

if (OPTION_BUILD_ASTC)
    file(GLOB_RECURSE CMP_ASTC_SRCS
        "astc/*.h"
        "astc/*.cpp"
        "astc/arm/*.h"
        "astc/arm/*.cpp"
        )
    list(APPEND CMP_SRCS ${CMP_ASTC_SRCS})
endif()

if (OPTION_BUILD_BROTLIG)
    file(GLOB_RECURSE CMP_BRLG_SRCS
        "brotlig/*.h"
        "brotlig/*.cpp"
    )
    list(APPEND CMP_SRCS ${CMP_BRLG_SRCS})
endif()

add_library(CMP_Compressonator 
            STATIC  
            version.h
            common.h
            compress.cpp
            compressonator.h
            compressonator.cpp
            ${CMP_SRCS} 
            ${CMP_CORE_SRC}  
            )

source_group("cmp_core" FILES ${CMP_CORE_SRC})

target_include_directories(CMP_Compressonator
                           PRIVATE
                           ./
                           # Lossy Compression
                           apc
                           atc
                           ati
                           basis
                           bc6h
                           bc7
                           block
                           buffer
                           dxt
                           dxtc
                           etc
                           etc/etcpack/
                           gt

                           # Common
                           common
                           ${PROJECT_SOURCE_DIR}/cmp_core/shaders
                           ${PROJECT_SOURCE_DIR}/cmp_core/source
                           ${PROJECT_SOURCE_DIR}/cmp_framework/common
                           ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
                           ${PROJECT_SOURCE_DIR}/applications/_plugins/common
                           )

set_target_properties(CMP_Compressonator PROPERTIES FOLDER "Libs")

if (OPTION_BUILD_ASTC)
    target_include_directories(CMP_Compressonator
        PRIVATE
        astc
        astc/arm/
    )
endif()

if (OPTION_BUILD_BROTLIG)
    target_include_directories(CMP_Compressonator PRIVATE brotlig)

    # should there be an error if OPTION_CMP_DIRECTX isn't ON?
    target_link_libraries(CMP_Compressonator PRIVATE
        ExtBrotlig
        dxcompiler.lib
        dxgi.lib
        d3d12.lib
        dxguid.lib
    )
endif()