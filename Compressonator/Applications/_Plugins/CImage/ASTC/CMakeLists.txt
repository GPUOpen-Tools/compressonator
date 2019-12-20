cmake_minimum_required(VERSION 3.10)

add_library(ASTC STATIC "")

target_sources(ASTC
               PRIVATE
               ../../Common/TC_PluginAPI.h
               ../../Common/TC_PluginInternal.h
               ../../Common/TC_PluginInternal.cpp
               ../../Common/UtilFuncs.h
               ../../Common/UtilFuncs.cpp
               ./cASTC.cpp
               ./cASTC.h
               )

target_include_directories(ASTC
                           PRIVATE
                           ../../../../CMP_CompressonatorLib
                           ../../../../CMP_Framework/Common/half
                           ../../Common/
                           )
if (UNIX)
target_compile_definitions(ASTC PRIVATE _LINUX)
endif()
