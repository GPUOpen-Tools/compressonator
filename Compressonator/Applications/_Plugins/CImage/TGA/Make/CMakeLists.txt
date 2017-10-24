cmake_minimum_required (VERSION 2.8)

project (TGA)

set (CMP_INCLUDE_DIRS "")

list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Header/")

list(REMOVE_DUPLICATES CMP_INCLUDE_DIRS)
include_directories(${CMP_INCLUDE_DIRS})

add_definitions(-DAPPLICATION_PLUGIN)

set (CMP_SOURCES1 "")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/MIPS.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/TC_PluginInternal.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/UtilFuncs.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../TGA.cpp")

add_library(TGA STATIC ${CMP_SOURCES1})

target_link_libraries(TGA libz.a)

