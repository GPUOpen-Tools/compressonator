cmake_minimum_required(VERSION 3.10)

add_library(CMP_Core STATIC "")

target_sources(CMP_Core
               PRIVATE
                   shaders/BC1_Encode_kernel.h
                   shaders/BC1_Encode_kernel.cpp
                   shaders/BC2_Encode_kernel.h
                   shaders/BC2_Encode_kernel.cpp
                   shaders/BC3_Encode_kernel.h
                   shaders/BC3_Encode_kernel.cpp
                   shaders/BC4_Encode_kernel.h
                   shaders/BC4_Encode_kernel.cpp
                   shaders/BC5_Encode_kernel.h
                   shaders/BC5_Encode_kernel.cpp
                   shaders/BC6_Encode_kernel.h
                   shaders/BC6_Encode_kernel.cpp
                   shaders/BC7_Encode_Kernel.h
                   shaders/BC7_Encode_Kernel.cpp
                   shaders/BCn_Common_Kernel.h
                   shaders/Common_Def.h
                   )

target_include_directories(CMP_Core
                           PRIVATE
                           shaders
                           source)
add_subdirectory(test)

if (UNIX)
target_compile_definitions(CMP_Core PRIVATE _LINUX ASPM_GPU)
endif()
