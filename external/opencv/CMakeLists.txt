cmake_minimum_required(VERSION 3.13)

message(STATUS "(1)----------------------  OpenCV CmakeList for ${CMAKE_CURRENT_LIST_DIR}" )

add_library(ExtOpenCV INTERFACE)

set(ExtOpenCV_BIN_PATH ${PROJECT_SOURCE_DIR}/../common/lib/ext/opencv/2.49/x64/VS2015/bin)

#set_property(GLOBAL PROPERTY ExtOpenCV_BIN_PATH "${ExtOpenCV_BIN_PATH}")
#list(APPEND CMAKE_FIND_ROOT_PATH ${ExtOpenCV_BIN_PATH})

if (CMP_HOST_WINDOWS)
    if (NOT DEFINED ENV{OPENCV_DIR})
        message(FATAL_ERROR "A 'OPENCV_DIR' environment variable is required to indicate where OpenCV is installed.")
    endif()

    set(OpenCV_INCLUDE_DIRS
        $ENV{OPENCV_DIR}/build/include
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/opencv/2.49/include
    )

    set(OpenCV_LIB_DIRS
        $ENV{OPENCV_DIR}/build/x64/vc15/lib
    )
endif()

target_include_directories(ExtOpenCV INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/opencv/2.49/include
)

