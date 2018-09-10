cmake_minimum_required (VERSION 2.8)

project (Compressonator)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_definitions(-DDISABLE_TESTCODE)

endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
add_definitions(-D_LINUX)
endif()

file(GLOB_RECURSE CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../Source/*.cpp")
file(GLOB_RECURSE CMP_SOURCES2 "${CMAKE_CURRENT_SOURCE_DIR}/../Source/*.c")
file(GLOB_RECURSE CMP_SOURCES3 "${CMAKE_CURRENT_SOURCE_DIR}/../Source/*.cxx")

file(GLOB_RECURSE CMP_HEADERS1 "${CMAKE_CURRENT_SOURCE_DIR}/../Header/*.h")
file(GLOB_RECURSE CMP_HEADERS2 "${CMAKE_CURRENT_SOURCE_DIR}/../Source/*.h")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    list(APPEND CMP_HEADERS1 ${CMP_HEADERS1} "${CMAKE_CURRENT_SOURCE_DIR}/../../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/Half/half.h")
    list(APPEND CMP_SOURCES1 ${CMP_SOURCES1} "${CMAKE_CURRENT_SOURCE_DIR}/../../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/Half/half.cpp")
endif()


list(APPEND CMP_SOURCES1 ${CMP_SOURCES1} "${CMP_SOURCES2}")
list(APPEND CMP_SOURCES1 ${CMP_SOURCES1} "${CMP_SOURCES3}")

set (CMP_INCLUDE_DIRS "")

foreach (_headerFile ${CMP_HEADERS1})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND CMP_INCLUDE_DIRS ${_dir})
endforeach()

foreach (_headerFile ${CMP_HEADERS2})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND CMP_INCLUDE_DIRS ${_dir})
endforeach()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    list(APPEND CMP_INCLUDE_DIRS "/usr/local/include/OpenEXR/")
    list(APPEND CMP_INCLUDE_DIRS "/usr/include/OpenEXR/")
    #build Openexr 2.2 from cmake
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        list(APPEND CMP_INCLUDE_DIRS "/usr/local/ilmbase22build/include/OpenEXR/")
        list(APPEND CMP_INCLUDE_DIRS "/usr/local/openexr22build/include/OpenEXR/")
    endif()
endif()

list(REMOVE_DUPLICATES CMP_INCLUDE_DIRS)

include_directories(${CMP_INCLUDE_DIRS})

add_library(Compressonator STATIC ${CMP_SOURCES1})
