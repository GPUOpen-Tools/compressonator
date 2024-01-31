cmake_minimum_required(VERSION 3.13)

set(FOLDER_NAME ${PROJECT_FOLDER_SDK_EXTERNAL})

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
if(OPTION_BUILD_KTX2)
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
#                  DIRECTX
# ====================================================
if (OPTION_CMP_DIRECTX)
    message(STATUS "++++++++++++++++++ DIRECTX 12")
    add_library(ExtDirectX INTERFACE)

    target_link_libraries(ExtDirectX INTERFACE
        d3d12.lib
        dxgi.lib
        d3dcompiler.lib
    )
endif()

# ====================================================
#                DIRECTX TEX
# ====================================================
if (OPTION_CMP_DIRECTX)
    message(STATUS "++++++++++++++++++ DIRECTX TEX")
    include(${CMAKE_CURRENT_LIST_DIR}/directxtex/CMakeLists.txt)
    # add_library(ExtDirectXTex INTERFACE)
    target_link_libraries(ExtDirectXTex INTERFACE
        DirectXTex
    )
endif()

# ====================================================
#                     OPENCV
# ====================================================
if (OPTION_CMP_OPENCV)
    list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_CURRENT_LIST_DIR}/opencv/)
    message(STATUS "++++++++++++++++++  OPENCV" )
    include(${CMAKE_CURRENT_LIST_DIR}/opencv/CMakeLists.txt)
endif()

# ====================================================
#                   OPENEXR
# ====================================================
if (OPTION_BUILD_EXR)
    message(STATUS "++++++++++++++++++ O P E N E X R" )
    include(${CMAKE_CURRENT_LIST_DIR}/openexr/CMakeLists.txt)
endif()

