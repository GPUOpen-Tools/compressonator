# CMakeLists.txt

cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

project(CompressonatorZlib)

add_subdirectory(zlib-submodule ${CMAKE_CURRENT_BINARY_DIR}/submodule)

