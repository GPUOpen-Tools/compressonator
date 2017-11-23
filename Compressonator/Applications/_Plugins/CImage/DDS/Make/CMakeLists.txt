cmake_minimum_required (VERSION 2.8)

project (DDS)

set (CMP_INCLUDE_DIRS "")

# Qt5 include path - users install required
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
message("Finding Package Qt5 (Qt5Core and Qt5Gui). if not found, for Unix, run initsetup_unix.sh to install. For window, please make sure install qt5.7 and add the bin path (i.e. C:/Qt/5.7/msvc2015_64/bin) to your environment PATH.")

find_package(Qt5Core)
if(Qt5Core_FOUND)
list(APPEND CMP_INCLUDE_DIRS "${Qt5Core_INCLUDE_DIRS}/../")
else()
message(FATAL_ERROR "Package Qt5 (Qt5Core and Qt5Gui) are required, but not found. In Unix, run initsetup_unix.sh. If is window, please make sure install qt and add the include path (i.e. C:\\Qt\\5.7\\msvc2015_64\\) to environment PATH.")
endif()

list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Header/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../")


if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    list(APPEND CMP_INCLUDE_DIRS "/usr/local/include/OpenEXR/")
    list(APPEND CMP_INCLUDE_DIRS "/usr/include/OpenEXR/")
    #build openexr from cmake
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        list(APPEND CMP_INCLUDE_DIRS "/usr/local/ilmbase22build/include/OpenEXR/")
        list(APPEND CMP_INCLUDE_DIRS "/usr/local/openexr22build/include/OpenEXR/")
    endif()
    find_package(Boost COMPONENTS system filesystem) 
    if(Boost_FOUND)
        list(APPEND CMP_INCLUDE_DIR ${Boost_INCLUDE_DIRS}) 
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
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../DDS.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../DDS_DX10.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../DDS_File.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../DDS_Helpers.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../stdafx.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/MIPS.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/TC_PluginInternal.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/UtilFuncs.cpp")

add_library(DDS STATIC ${CMP_SOURCES1})

