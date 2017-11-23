cmake_minimum_required (VERSION 2.8)

project (EXR)

set (CMP_INCLUDE_DIRS "")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Header/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Header/Codec/")


if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    find_package(Boost COMPONENTS system filesystem) 
    if(Boost_FOUND)
        list(APPEND CMP_INCLUDE_DIR ${Boost_INCLUDE_DIRS}) 
    endif()
    
    list(APPEND CMP_INCLUDE_DIRS "/usr/local/include/OpenEXR/")
    list(APPEND CMP_INCLUDE_DIRS "/usr/include/OpenEXR/")
    #build openexr from cmake
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        list(APPEND CMP_INCLUDE_DIRS "/usr/local/ilmbase22build/include/OpenEXR/")
        list(APPEND CMP_INCLUDE_DIRS "/usr/local/openexr22build/include/OpenEXR/")
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # boost and openexr include paths are provided with the source
    list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Common/Lib/Ext/Boost/boost_1_59_0/")
    list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Common/Lib/Ext/OpenEXR/openexr-2.2.0/VS2015/x64/include/OpenEXR/")
    list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/VS2015/x64/include/OpenEXR/")
endif()

list(REMOVE_DUPLICATES CMP_INCLUDE_DIRS)
include_directories(${CMP_INCLUDE_DIRS})

add_definitions(-DAPPLICATION_PLUGIN)

set (CMP_SOURCES1 "")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/cExr.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/MIPS.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/TC_PluginInternal.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/UtilFuncs.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../EXR.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../stdafx.cpp")

add_library(EXR STATIC ${CMP_SOURCES1})

