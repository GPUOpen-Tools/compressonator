cmake_minimum_required (VERSION 2.8)

project (IMGAnalysis)

file(GLOB_RECURSE CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../*.cpp")
list(APPEND CMP_SOURCES1 ${CMP_SOURCES1} "${CMAKE_CURRENT_SOURCE_DIR}/../../../../CompressonatorGUI/Common/cvmatandqimage.cpp")
list(APPEND CMP_SOURCES1 ${CMP_SOURCES1} "${CMAKE_CURRENT_SOURCE_DIR}/../../../../CompressonatorGUI/Components/cpImageLoader.cpp")
list(APPEND CMP_SOURCES1 ${CMP_SOURCES1} "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/SSIM.cpp")

file(GLOB_RECURSE CMP_HEADERS1 "${CMAKE_CURRENT_SOURCE_DIR}/../*.h")
list(APPEND CMP_HEADERS1 ${CMP_HEADERS1} "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/PluginInterface.h")
list(APPEND CMP_HEADERS1 ${CMP_HEADERS1} "${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/TC_PluginInternal.h")
list(APPEND CMP_HEADERS1 ${CMP_HEADERS1} "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Header/Compressonator.h")
list(APPEND CMP_HEADERS1 ${CMP_HEADERS1} "${CMAKE_CURRENT_SOURCE_DIR}/../../../../CompressonatorGUI/Common/cvmatandqimage.h")
list(APPEND CMP_HEADERS1 ${CMP_HEADERS1} "${CMAKE_CURRENT_SOURCE_DIR}/../../../../CompressonatorGUI/Components/cpImageLoader.h")

#openexr include path is provided for win
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    list(APPEND CMP_HEADERS1 ${CMP_HEADERS1} "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Common/Lib/Ext/OpenEXR/openexr-2.2.0/VS2015/x64/include/OpenEXR/ImfStandardAttributes.h")
    list(APPEND CMP_HEADERS1 ${CMP_HEADERS1} "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/VS2015/x64/include/OpenEXR/ImathInt64.h")
endif()

set (CMP_INCLUDE_DIRS "")

foreach (_headerFile ${CMP_HEADERS1})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND CMP_INCLUDE_DIRS ${_dir})
endforeach()  

# Qt5 include path - users install required
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
message("Finding Package Qt5 (Qt5Core and Qt5Gui). if not found, for Unix, run initsetup_unix.sh to install. For window, please make sure install qt5.7 and add the bin path (i.e. C:/Qt/5.7/msvc2015_64/bin) to your environment PATH.")
find_package(Qt5Core)
if(Qt5Core_FOUND)
list(APPEND CMP_INCLUDE_DIRS "${Qt5Core_INCLUDE_DIRS}/../")
else()
message(FATAL_ERROR "Package Qt5 (Qt5Core and Qt5Gui) are required, but not found. In Unix, run initsetup_unix.sh. If is window, please make sure install qt and add the include path (i.e. C:\Qt\5.7\msvc2015_64\) to environment PATH.")
endif()



#opencv 
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    message("Finding Package Boost and Opencv. if not found, for Ubuntu, run initsetup_ubuntu.sh to install. for mac, run initsetup_mac.sh to install. ")
    list(APPEND CMP_INCLUDE_DIRS "/usr/local/include/OpenEXR/")
    list(APPEND CMP_INCLUDE_DIRS "/usr/include/OpenEXR/")
    #build Openexr 2.2 from cmake
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        list(APPEND CMP_INCLUDE_DIRS "/usr/local/ilmbase22build/include/OpenEXR/")
        list(APPEND CMP_INCLUDE_DIRS "/usr/local/openexr22build/include/OpenEXR/")
    endif()
    find_package(Boost COMPONENTS system filesystem) 
    if(Boost_FOUND)
        list(APPEND CMP_INCLUDE_DIR ${Boost_INCLUDE_DIRS}) 
    endif()
    find_package(OpenCV) 
    if (OpenCV_FOUND)
        list(APPEND CMP_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS})
    else()
    message(FATAL_ERROR "Package OpenCV are required, but not found. In Unix, run initsetup_unix.sh to install the prerequisite libs.")
    endif()

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    #boost include path is provided
    list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Common/Lib/Ext/Boost/boost_1_59_0/")
    list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Common/Lib/Ext/OpenCV/2.49/include")
endif()

list(REMOVE_DUPLICATES CMP_INCLUDE_DIRS)

include_directories(${CMP_INCLUDE_DIRS})

add_library(IMGAnalysis STATIC ${CMP_SOURCES1})



