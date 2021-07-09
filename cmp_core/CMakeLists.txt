cmake_minimum_required(VERSION 3.10)

add_library(CMP_Core STATIC)

target_sources(CMP_Core
               PRIVATE
                shaders/bc1_encode_kernel.h
                shaders/bc1_common_kernel.h
                shaders/bc1_encode_kernel.cpp
                shaders/bc2_encode_kernel.h
                shaders/bc2_encode_kernel.cpp
                shaders/bc3_encode_kernel.h
                shaders/bc3_encode_kernel.cpp
                shaders/bc4_encode_kernel.h
                shaders/bc4_encode_kernel.cpp
                shaders/bc5_encode_kernel.h
                shaders/bc5_encode_kernel.cpp
                shaders/bc6_encode_kernel.h
                shaders/bc6_encode_kernel.cpp
                shaders/bc7_encode_kernel.h
                shaders/bc7_common_encoder.h
                shaders/bc7_encode_kernel.cpp
                shaders/bcn_common_kernel.h
                shaders/bcn_common_api.h
                shaders/common_def.h
                source/cmp_core.h
                source/cmp_math_vec4.h
                source/cmp_math_func.h
                )

if (OPTION_CMP_GTC)
target_sources(CMP_Core
               PRIVATE
                shaders/gtc_encode_kernel.h
                shaders/gtc_encode_kernel.cpp
	)
endif()

if (OPTION_CMP_APC)
target_sources(CMP_Core
               PRIVATE
                shaders/apc_encode_kernel.h
                shaders/apc_encode_kernel.cpp
	)
endif()


target_include_directories(CMP_Core
                           PRIVATE
                           shaders
                           source)

if (UNIX)
target_compile_definitions(CMP_Core PRIVATE _LINUX ASPM_GPU)
endif()

set_target_properties(CMP_Core PROPERTIES FOLDER "Libs")
