cmake_minimum_required(VERSION 3.10)

add_library(CMP_Framework STATIC "")
file(GLOB_RECURSE half
     "Common/half/*.h"
     "Common/half/*.cpp"
     )
file(GLOB_RECURSE DDS
     "../Applications/_Plugins/CImage/DDS/*h"
     "../Applications/_Plugins/CImage/DDS/*.cpp"
     )
file(GLOB_RECURSE HPC
     "../Applications/_Plugins/CCMP_Encode/HPC/*h"
     "../Applications/_Plugins/CCMP_Encode/HPC/*.cpp"
     )

target_sources(CMP_Framework
               PRIVATE
               ../CMP_Core/shaders/BC1_Encode_kernel.h
               ../CMP_Core/shaders/BC1_Encode_kernel.cpp
               ../CMP_Core/shaders/BC2_Encode_kernel.h
               ../CMP_Core/shaders/BC2_Encode_kernel.cpp
               ../CMP_Core/shaders/BC3_Encode_kernel.h
               ../CMP_Core/shaders/BC3_Encode_kernel.cpp
               ../CMP_Core/shaders/BC4_Encode_kernel.h
               ../CMP_Core/shaders/BC4_Encode_kernel.cpp
               ../CMP_Core/shaders/BC5_Encode_kernel.h
               ../CMP_Core/shaders/BC5_Encode_kernel.cpp
               ../CMP_Core/shaders/BC6_Encode_kernel.h
               ../CMP_Core/shaders/BC6_Encode_kernel.cpp
               ../CMP_Core/shaders/BC7_Encode_Kernel.h
               ../CMP_Core/shaders/BC7_Encode_Kernel.cpp
               ../CMP_Core/shaders/BCn_Common_Kernel.h
               ../CMP_Core/source/CMP_Core.h
               ../Applications/_Libs/CMP_Math/cmp_math_common.h
               ../Applications/_Libs/CMP_Math/cmp_math_common.cpp
               ../Applications/_Libs/CMP_Math/cmp_math_cpuid.h
               ../Applications/_Libs/CMP_Math/cmp_math_cpuid.cpp
               ../Applications/_Libs/CMP_Math/kernel_math.cpp
               ../Applications/_Plugins/Common/ATIFormats.cpp
               ../Applications/_Plugins/Common/ATIFormats.h
               ../Applications/_Plugins/Common/PluginBase.h
               ../Applications/_Plugins/Common/PluginInterface.h
               ../Applications/_Plugins/Common/PluginManager.h
               ../Applications/_Plugins/Common/PluginManager.cpp
               ../Applications/_Plugins/Common/query_timer.h
               ../Applications/_Plugins/Common/query_timer.cpp
               ../Applications/_Plugins/Common/stb_image.h
               ../Applications/_Plugins/Common/TC_PluginAPI.h
               ../Applications/_Plugins/Common/TC_PluginInternal.h
               ../Applications/_Plugins/Common/TC_PluginInternal.cpp
               ../Applications/_Plugins/Common/UtilFuncs.h
               ../Applications/_Plugins/Common/UtilFuncs.cpp
               ../Applications/_Plugins/CCMP_SDK/BC1/BC1.h
               ../Applications/_Plugins/CCMP_SDK/BC1/BC1.cpp
               ../Applications/_Plugins/CCMP_SDK/BC2/BC2.h
               ../Applications/_Plugins/CCMP_SDK/BC2/BC2.cpp
               ../Applications/_Plugins/CCMP_SDK/BC3/BC3.h
               ../Applications/_Plugins/CCMP_SDK/BC3/BC3.cpp
               ../Applications/_Plugins/CCMP_SDK/BC4/BC4.h
               ../Applications/_Plugins/CCMP_SDK/BC4/BC4.cpp
               ../Applications/_Plugins/CCMP_SDK/BC5/BC5.h
               ../Applications/_Plugins/CCMP_SDK/BC5/BC5.cpp
               ../Applications/_Plugins/CCMP_SDK/BC6/BC6H.h
               ../Applications/_Plugins/CCMP_SDK/BC6/BC6H.cpp
               ../Applications/_Plugins/CCMP_SDK/BC7/BC7.h
               ../Applications/_Plugins/CCMP_SDK/BC7/BC7.cpp
               Common/CMP_BoxFilter.cpp
               Common/CMP_BoxFilter.h
               Common/CMP_MIPS.cpp
               Common/CMP_MIPS.h
               Compute_Base.cpp
               Compute_Base.h
               ${half}
               ${DDS}
               ${HPC}
               )
target_include_directories(CMP_Framework
                           PRIVATE
                           ../CMP_Core/source
                           ../CMP_Core/shaders
                           ../CMP_CompressonatorLib
                           Common/half/
                           ../Applications/_Plugins/Common/
                           ../Applications/_Libs/CMP_Math/
                           ../Applications/_Libs/GPU_Decode
                           ./
                          )
if (UNIX)
target_compile_definitions(CMP_Framework PRIVATE _LINUX)
endif()