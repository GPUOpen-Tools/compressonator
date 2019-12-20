cmake_minimum_required(VERSION 3.10)

add_library(TGA STATIC "")

target_sources(TGA
               PRIVATE
               ../../Common/TC_PluginAPI.h
               ../../Common/TC_PluginInternal.h
               ../../Common/TC_PluginInternal.cpp
               ../../Common/UtilFuncs.h
               ../../Common/UtilFuncs.cpp
               ./TGA.cpp
               ./TGA.h
               )

target_include_directories(TGA
                           PRIVATE
                           ../../../../CMP_CompressonatorLib
                           ../../../../CMP_Framework/Common/half
                           ../../Common/
                           )
if (UNIX)
target_compile_definitions(TGA PRIVATE _LINUX)
endif()
