# CMakeLists.txt

# Compressonator Dependencies
cmake_minimum_required(VERSION 3.15)

project(CompressonatorDependencies)

option(BUILD_SHARED_LIBS "Build SHARED, if disabled STATIC is built" OFF)

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif ()

if (NOT DEFINED COMPRESSONATOR_PROJECT_DIR)
    get_filename_component(COMPRESSONATOR_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../ ABSOLUTE)
endif ()

if (NOT DEFINED NUM_PARALLEL_JOBS)
    set(NUM_PARALLEL_JOBS 8)
endif ()

if (NOT (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "Debug"))
    message(FATAL_ERROR "CMAKE_BUILD_TYPE Bad variable: ${CMAKE_BUILD_TYPE}")
endif ()

include(DependencyInfo.cmake)

message(STATUS "BUILD_SHARED_LIBS -- ${BUILD_SHARED_LIBS}")
message(STATUS "CMAKE_BUILD_TYPE -- ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_STANDARD -- ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_COMMAND -- ${CMAKE_COMMAND}")
message(STATUS "COMPRESSONATOR_PROJECT_DIR -- ${COMPRESSONATOR_PROJECT_DIR}")
message(STATUS "DEPENDENCIES_INSTALL_DIR -- ${DEPENDENCIES_INSTALL_DIR}")
message(STATUS "NUM_PARALLEL_JOBS -- ${NUM_PARALLEL_JOBS}")

# Dependencies
add_subdirectory(${COMPRESSONATOR_PROJECT_DIR}/External/vulkan/vulkan-submodule ${CMAKE_BINARY_DIR}/vulkan)
add_subdirectory(${COMPRESSONATOR_PROJECT_DIR}/External/qt5/qt5-submodule ${CMAKE_BINARY_DIR}/qt5)
add_subdirectory(CustomTargets ${CMAKE_BINARY_DIR}/CustomTargets)

# Write a generic c++ source file to compile
file(
    WRITE ${CMAKE_CURRENT_BINARY_DIR}/external.cpp
    "// Dummy source file
#include <stdio.h>

int main()
{
    printf(\"Compressonator Dependencies\\n\");

    return 0;
}
"
)

# Target with all dependencies to ensure everything is built
add_executable(Dependencies)
target_sources(Dependencies PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/external.cpp)
if (WIN32)
    add_dependencies(Dependencies 
        catch2
        glew
        glslang
        opencv
        openexr
        zlib
    )
else()
    add_dependencies(Dependencies 
        catch2
        glew
        opencv
        openexr
        zlib
    )
endif()
