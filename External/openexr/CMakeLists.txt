# CMakeLists.txt

cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

project(CompressonatorOpenEXR)

add_subdirectory(openexr-submodule ${CMAKE_BINARY_DIR}/submodule)
