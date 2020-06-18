cmake_minimum_required(VERSION 3.10)

add_library(EXR STATIC "")

target_sources(EXR
               PRIVATE
               ../../Common/TC_PluginAPI.h
               ../../Common/TC_PluginInternal.h
               ../../Common/TC_PluginInternal.cpp
               ../../Common/UtilFuncs.h
               ../../Common/UtilFuncs.cpp
               ../../Common/CMP_FileIO.h
               ../../Common/CMP_FileIO.cpp
               ../../Common/cExr.h
               ../../Common/cExr.cpp
               ../../Common/namespaceAlias.h
               ./EXR.h
               ./EXR.cpp
               )

target_include_directories(EXR
                           PRIVATE
                           ../../../../CMP_CompressonatorLib
                           ../../../../CMP_Framework/Common/half
                           ../../Common/
                           ${OpenEXR_INCLUDE_DIRS}
                           )

if (APPLE)
target_include_directories(EXR
                           PRIVATE
                           /usr/local/include/OpenEXR/)
find_package(Boost COMPONENTS system filesystem) 
if(Boost_FOUND)
    target_include_directories(EXR
                               PRIVATE 
                               ${Boost_INCLUDE_DIRS})
endif()
endif()

if (UNIX)
target_compile_definitions(EXR PRIVATE _LINUX)
endif()
