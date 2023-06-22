cmake_minimum_required(VERSION 3.13)

set(FOLDER_NAME "External")

message(STATUS "++++++++++++++++++External CMakeList called" )

if (OPTION_BUILD_APPS_CMP_CLI OR OPTION_BUILD_APPS_CMP_GUI)
    include(${CMAKE_CURRENT_LIST_DIR}/glew/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/glm/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/opengl/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/qt5/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/rapidxml/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/vulkan/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/glfw/CMakeLists.txt)
endif()

if (OPTION_BUILD_BROTLIG)
    include(${CMAKE_CURRENT_LIST_DIR}/brotlig/CMakeLists.txt)
endif()

# enable KTX2 features
if(${CMAKE_VERSION} VERSION_GREATER "3.14.0") 
    include(${CMAKE_CURRENT_LIST_DIR}/ktx/CMakeLists.txt)
endif()

list(APPEND CMAKE_FIND_ROOT_PATH ${DEPENDENCIES_INSTALL_DIR})

#append know paths to cmake search path
list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_CURRENT_LIST_DIR}/glew/glew_1.9.0)

set(PATH_SUFFIXES lib)

# ====================================================
#                    COMMON
# ====================================================
find_package(Threads REQUIRED QUIET)

# ====================================================
#                    CATCH2
# ====================================================
#find_package(Catch2 REQUIRED QUIET
#    PATHS ${CMAKE_FIND_ROOT_PATH}
#)
#
#add_library(ExtCatch2 INTERFACE)
#target_link_libraries(ExtCatch2 INTERFACE Catch2::Catch2)
#

# ====================================================
#                  DIRECTX
# ====================================================
if (OPTION_CMP_DIRECTX )
    message(STATUS "++++++++++++++++++ DIRECTX 12" )
    add_library(ExtDirectX INTERFACE)

    target_link_libraries(ExtDirectX INTERFACE
        d3d12.lib
        dxgi.lib
        d3dcompiler.lib
    )
endif()

# ====================================================
#                DIRECTX   TEX
# ====================================================
if (OPTION_CMP_DIRECTX )
    message(STATUS "++++++++++++++++++ DIRECTX TEX" )
    include(${CMAKE_CURRENT_LIST_DIR}/directxtex/CMakeLists.txt)
    # add_library(ExtDirectXTex INTERFACE)
    target_link_libraries(ExtDirectXTex INTERFACE
         DirectXTex
     )
endif()

# ====================================================
#                   GLSLANG
# ====================================================

#set(GLSLANG_PATHS ${CMAKE_FIND_ROOT_PATH})
#
#if (CMP_HOST_APPLE)
#    set(GLSLANG_CMAKE_DIR ${PROJECT_SOURCE_DIR}/External/vulkan/vulkan-submodule/Apple/macOS/lib/cmake)
#    list(APPEND GLSLANG_PATHS ${PROJECT_SOURCE_DIR}/External/vulkan/vulkan-submodule/Apple/macOS)
#elseif (CMP_HOST_LINUX)
#    set(GLSLANG_CMAKE_DIR ${PROJECT_SOURCE_DIR}/External/vulkan/vulkan-submodule/Linux/x86_64/lib/cmake)
#    list(APPEND GLSLANG_PATHS ${PROJECT_SOURCE_DIR}/External/vulkan/vulkan-submodule/Linux/x86_64/lib/cmake)
#elseif (CMP_HOST_WINDOWS)
#    set(GLSLANG_CMAKE_DIR ${DEPENDENCIES_INSTALL_DIR}/lib/cmake)
#endif()
#
#
#find_package(SPIRV-Tools REQUIRED QUIET
#    PATHS ${GLSLANG_PATHS}
#)
#
#find_package(SPIRV-Tools-opt REQUIRED QUIET
#    PATHS ${GLSLANG_PATHS}
#)
#
#include(${GLSLANG_CMAKE_DIR}/OGLCompilerTargets.cmake)
#include(${GLSLANG_CMAKE_DIR}/HLSLTargets.cmake)
#include(${GLSLANG_CMAKE_DIR}/OSDependentTargets.cmake)
#include(${GLSLANG_CMAKE_DIR}/glslangTargets.cmake)
#include(${GLSLANG_CMAKE_DIR}/SPIRVTargets.cmake)
#
#add_library(ExtGlslang INTERFACE)
#
#target_link_libraries(ExtGlslang 
#    INTERFACE
#        glslang
#        SPIRV
#)

# ====================================================
#                     OPENCV
# ====================================================
# Creates ExtOpenCV
list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_CURRENT_LIST_DIR}/opencv/)
message(STATUS "++++++++++++++++++  OPENCV" )
include(${CMAKE_CURRENT_LIST_DIR}/opencv/CMakeLists.txt)

# ====================================================
#                      ZLIB
# ====================================================
# message(STATUS "++++++++++++++++++ Z L I B" )
# find_library(LIB_ZLIB zlibstatic zlib libz libz.a zlibstatic.lib zlib.lib
# 
#     PATHS ${CMAKE_FIND_ROOT_PATH}
#     PATH_SUFFIXES ${PATH_SUFFIXES}
# )
# 
# if (${LIB_ZLIB} STREQUAL "LIB_ZLIB-NOTFOUND")
#     message(FATAL_ERROR "ZLib not found")
# endif()
# 
# add_library(ZLIB::ZLIB STATIC IMPORTED GLOBAL)
# set_target_properties(ZLIB::ZLIB PROPERTIES
#     INTERFACE_INCLUDE_DIRECTORIES ${DEPENDENCIES_INSTALL_DIR}/include
#     IMPORTED_LOCATION ${LIB_ZLIB}
# )
# 
# add_library(ExtZlib INTERFACE)
# 
# target_link_libraries(ExtZlib INTERFACE
#     ZLIB::ZLIB
# )
# 

# ====================================================
#                   OPENEXR
# ====================================================
message(STATUS "++++++++++++++++++ O P E N E X R" )
include(${CMAKE_CURRENT_LIST_DIR}/openexr/CMakeLists.txt)
# find_package(IlmBase REQUIRED QUIET
#     PATHS ${CMAKE_FIND_ROOT_PATH}
# )
# find_package(OpenEXR REQUIRED QUIET
#     PATHS ${CMAKE_FIND_ROOT_PATH}
# )
# set_target_properties(OpenEXR::IlmImf PROPERTIES IMPORTED_GLOBAL TRUE)
# add_library(ExtOpenEXR INTERFACE)
# 
# target_link_libraries(ExtOpenEXR INTERFACE
#     OpenEXR::IlmImf
# )

