# CMakeLists.txt
message(STATUS "++++++++++++++++++External OpenEXR CMakeList called" )
cmake_minimum_required(VERSION 3.13)

add_library(ExtOpenEXR INTERFACE)

# target_link_libraries(ExtOpenEXR INTERFACE 
#     ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/openexr/openexr-2.2.0/VS2015/x64/lib
#     ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/openexr/ilmbase-2.2.0/VS2015/x64/lib
# )

target_include_directories(ExtOpenEXR INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/openexr/openexr-2.2.0/VS2015/x64/include/OpenEXR
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/openexr/ilmbase-2.2.0/VS2015/x64/include/OpenEXR
)

# if(POLICY CMP0091)
#   cmake_policy(SET CMP0091 NEW)
# endif()
# 
# project(CompressonatorOpenEXR)
# 
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../Common/Lib/Ext/openexr/  ${CMAKE_CURRENT_BINARY_DIR}/../../../Common/Lib/Ext/openexr/)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../Common/Lib/Ext/openexr/IlmBase  ${CMAKE_CURRENT_BINARY_DIR}/../../../Common/Lib/Ext/openexr/IlmBase)
