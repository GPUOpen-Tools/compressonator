cmake_minimum_required(VERSION 3.10)

add_library(CMP_Compressonator STATIC "")

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
     "./*.h"
     "./*.cpp"
     "apc/*.h"
     "apc/*.cpp"
     "astc/*.h"
     "astc/*.cpp"
     "astc/arm/*.h"
     "astc/arm/*.cpp"
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
     "common/*.h"
     "common/*.cpp"
     ${PROJECT_SOURCE_DIR}/cmp_framework/common/*.h
     ${PROJECT_SOURCE_DIR}/cmp_framework/common/*.cpp
     ${PROJECT_SOURCE_DIR}/cmp_framework/common/half/*.h
     ${PROJECT_SOURCE_DIR}/cmp_framework/common/half/*.cpp
     ${PROJECT_SOURCE_DIR}/applications/_plugins/common/atiformats.cpp
     ${PROJECT_SOURCE_DIR}/applications/_plugins/common/atiformats.h 
     )

target_sources(CMP_Compressonator
               PRIVATE
               ${CMP_SRCS}
               ${CMP_CORE_SRC} 
               )

target_include_directories(CMP_Compressonator
                           PRIVATE
                           ./
                           apc
                           astc
                           astc/arm/
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
                           common
                           ${PROJECT_SOURCE_DIR}/cmp_core/shaders
                           ${PROJECT_SOURCE_DIR}/cmp_core/source
                           ${PROJECT_SOURCE_DIR}/cmp_framework/common
                           ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
                           ${PROJECT_SOURCE_DIR}/applications/_plugins/common
                           )

set_target_properties(CMP_Compressonator PROPERTIES FOLDER "Libs")
