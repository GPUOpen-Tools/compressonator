cmake_minimum_required(VERSION 3.10)

add_library(KTX STATIC "")

file(GLOB_RECURSE KTXLib
     "./Lib/*.h"
     "./Lib/*.cpp"
     "./Lib/*.c"
     )
target_sources(KTX
               PRIVATE
               ${KTXLib}
               ./KTX.cpp
               ./cKTX.h
               ./softfloat.cpp
               ./softfloat.h
               )

target_include_directories(KTX
                           PRIVATE
                           ../../../../CMP_CompressonatorLib
                           ../../../../Applications/_Libs/GPU_Decode
                           ../../../../CMP_Framework/Common/half
                           ../../Common/
                           ./Lib/
                           ./
                           )
if (UNIX)
target_compile_definitions(KTX PRIVATE _LINUX)
find_package(OpenGL) 
if (OpenGL_FOUND)
    if(APPLE)
        target_include_directories(KTX
                                   PRIVATE
                                   /usr/local/include)
    endif()
endif()
endif()
