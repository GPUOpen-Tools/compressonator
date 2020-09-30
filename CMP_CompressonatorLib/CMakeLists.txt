
# add_library(CompressonatorLIB)
# 
# file(GLOB_RECURSE CMP_ASM_SRCS
#     *.asm
# )
# 
# set(CMP_ASM_COMPILE_OPTIONS "/arch:SSE" "/arch:SSE2")
# 
# foreach(asm_src ${CMP_ASM_SRCS})
#     set_source_files_properties(${asm_src} PROPERTIES 
#         LANGUAGE ASM_MASM
#         COMPILE_DEFINITIONS X64=1 USE_SSE2=1 USE_SSE=1
#     )
# endforeach()
# 
# file(GLOB_RECURSE CMP_SRCS
#     *.c
#     *.cpp
#     *.cxx
#     *.h
# )
# 
# target_sources(CompressonatorLIB PRIVATE
# 
#     # ${CMP_ASM_SRCS}
#     ${CMP_SRCS}
# )
# 
# if (CMP_HOST_WINDOWS)
#     target_compile_definitions(CompressonatorLIB PRIVATE -D_WIN64)
# endif()
# 
# target_include_directories(CompressonatorLIB PUBLIC
#     ASTC
#     ASTC/ARM/
#     ATC
#     ATI
#     BASIS
#     BC6H
#     BC7
#     Block
#     Buffer
#     DXT
#     DXTC
#     ETC
#     ETC/etcpack/
#     GT
#     Common
#     ./
# )
# 
# target_link_libraries(CompressonatorLIB PRIVATE
# 
#     PRIVATE
#         CMP_Math
#         CMP_Common
#         CMP_FrameworkLIB
#         CMP_Core
# 
#         Plugin_PluginManager
#         Plugin_Common_KernelDef
#         Plugin_Common_STBImage
# 
#         ExtOpenEXR
#         Plugin_Half
#         
#     INTERFACE
#         Plugin_Half
# )

cmake_minimum_required(VERSION 3.13)

add_library(CompressonatorLIB STATIC "")

file(GLOB_RECURSE CMP_SRCS
     "./*.h"
     "./*.cpp"
     "ASTC/*.h"
     "ASTC/*.cpp"
     "ASTC/ARM/*.h"
     "ASTC/ARM/*.cpp"
     "ATC/*.h"
     "ATC/*.cpp"
     "ATI/*.h"
     "ATI/*.cpp"
     "ATI/*.c"
     "BASIS/*.h"
     "BASIS/*.cpp"
     "BC6H/*.h"
     "BC6H/*.cpp"
     "BC7/*.h"
     "BC7/*.cpp"
     "Block/*.h"
     "Block/*.cpp"
     "Buffer/*.h"
     "Buffer/*.cpp"
     "DXT/*.h"
     "DXT/*.cpp"
     "DXTC/*.h"
     "DXTC/*.cpp"
     "DXTC/*.c"
     "ETC/*.h"
     "ETC/*.cpp"
     "ETC/etcpack/*.h"
     "ETC/etcpack/*.cpp"
     "ETC/etcpack/*.cxx"
     "GT/*.h"
     "GT/*.cpp"
     "APC/*.h"
     "APC/*.cpp"
     "Common/*.h"
     "Common/*.cpp"
     "../CMP_Framework/Common/*.h"
     "../CMP_Framework/Common/*.cpp"
     "../CMP_Framework/Common/half/*.h"
     "../CMP_Framework/Common/half/*.cpp"
     "../Applications/_Plugins/Common/ATIFormats.cpp"
     "../Applications/_Plugins/Common/ATIFormats.h"
     )

target_sources(CompressonatorLIB
               PRIVATE
               ${CMP_SRCS}
               )

target_include_directories(CompressonatorLIB
                           PRIVATE
                           ./
                           ASTC
                           ASTC/ARM/
                           ATC
                           ATI
                           BASIS
                           BC6H
                           BC7
                           Block
                           Buffer
                           DXT
                           DXTC
                           ETC
                           ETC/etcpack/
                           APC
                           GT
                           Common
                           ../CMP_Framework/Common
                           ../CMP_Framework/Common/half
                           ../Applications/_Plugins/Common)

