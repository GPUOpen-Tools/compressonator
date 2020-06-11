# CMakeLists.txt

cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

project(CompressonatorOpenCV)

add_subdirectory(opencv-submodule ${CMAKE_CURRENT_BINARY_DIR}/submodule)
