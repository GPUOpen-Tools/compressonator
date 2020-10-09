cmake_minimum_required(VERSION 3.13)

#if(POLICY CMP0091)
#  cmake_policy(SET CMP0091 NEW)
#endif()
#
#project(CompressonatorZlib)

add_library(ExtZLib INTERFACE)
target_link_libraries(ExtZlib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/zlib/zlib-1.2.10/VS2015/x64/lib/zlibstatic.lib)

# add_subdirectory(zlib-submodule ${CMAKE_CURRENT_BINARY_DIR}/submodule)

