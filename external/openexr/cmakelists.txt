cmake_minimum_required(VERSION 3.13)

# CMakeLists.txt
message(STATUS "++++++++++++++++++External OpenEXR CMakeList called" )

add_library(ExtOpenEXR INTERFACE)

if (CMP_HOST_WINDOWS)
target_include_directories(ExtOpenEXR INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/openexr/openexr-2.2.0/VS2015/x64/include/OpenEXR
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/openexr/ilmbase-2.2.0/VS2015/x64/include/OpenEXR
)

else()
target_include_directories(ExtOpenEXR INTERFACE ${OpenEXR_INCLUDE_DIRS})
endif()

