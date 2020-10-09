cmake_minimum_required(VERSION 3.13)

message(STATUS "(1)----------------------  OpenCV CmakeList for ${CMAKE_CURRENT_LIST_DIR}" )

add_library(ExtOpenCV INTERFACE)

set(ExtOpenCV_BIN_PATH ${PROJECT_SOURCE_DIR}/../common/lib/ext/opencv/2.49/x64/VS2015/bin)
#set_property(GLOBAL PROPERTY ExtOpenCV_BIN_PATH "${ExtOpenCV_BIN_PATH}")
#list(APPEND CMAKE_FIND_ROOT_PATH ${ExtOpenCV_BIN_PATH})

target_include_directories(ExtOpenCV INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/opencv/2.49/include
)

# if(POLICY CMP0091)
#   cmake_policy(SET CMP0091 NEW)
# endif()
# 
# project(CompressonatorOpenCV)
# 
# set (OPENCV_CONFIG_FILE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/opencv2/build)
# set (PROJECT_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/opencv2/build)
# set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/opencv2/)
# 
# # =========== OPENCV v2.9.0 options
# if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
#   set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
# endif()
# 
# #Create build rules for OpenCV Documentation
# option( BUILD_DOCS OFF)
# 
# #Build all examples
# option( BUILD_EXAMPLES OFF)
# 
# #Enables 'make package_source' command
# option( BUILD_PACKAGE OFF)
# 
# #Build performance tests
# option( BUILD_PERF_TESTS OFF)
# 
# #Build shared libraries (.dll/.so) instead of static ones (.lib/.a)
# option( BUILD_SHARED_LIBS OFF)
# 
# # #Download and build TBB from source
# # option( BUILD_TBB OFF)
# 
# #Build accuracy & regression tests
# option( BUILD_TESTS OFF)
# 
# # #Include debug info into debug libs (not MSCV only)
# # option( BUILD_WITH_DEBUG_INFO OFF)
# 
# # #Enables use of staticaly linked CRT for staticaly linked OpenCV
# # option( BUILD_WITH_STATIC_CRT OFF)
# 
#  # ================ Media I/O =====================
# 
# #Build libjpeg from source
# option( BUILD_JPEG OFF)
# 
# #Build openexr from source
# option( BUILD_OPENEXR OFF)
# 
# #Build libpng from source
# option( BUILD_PNG OFF)
# 
# #Build libtiff from source
# option( BUILD_TIFF OFF)
# 
# #Include JPEG2K support
# option( WITH_JASPER OFF)
# 
# #Include JPEG support
# option( WITH_JPEG OFF)
# 
# 
# option( BUILD_opencv_calib3d OFF)
# option( BUILD_opencv_contrib OFF)
# option( BUILD_opencv_features2d OFF)
# option( BUILD_opencv_flann OFF)
# option( BUILD_opencv_gpu OFF)
# option( BUILD_opencv_legacy OFF)
# option( BUILD_opencv_ml OFF)
# option( BUILD_opencv_nonfree OFF)
# option( BUILD_opencv_objdetect OFF)
# option( BUILD_opencv_ocl OFF)
# option( BUILD_opencv_photo OFF)
# option( BUILD_opencv_stitching OFF)
# option( BUILD_opencv_superres OFF)
# option( BUILD_opencv_ts OFF)
# option( BUILD_opencv_video OFF)
# option( BUILD_opencv_videostab OFF)
# 
# option( BUILD_LIST "core, highgui, imgproc")
# 
# #Use precompiled headers
# option( ENABLE_PRECOMPILED_HEADERS OFF)
# 
# #Build with Win32 UI Backend support
# option( WITH_WIN32UI OFF)
# 
# 
# 
# # ================ Video I/O =====================
# #Include Video for Windows support
# option(WITH_VFW  OFF)
# 
# #Build HighGUI with DirectShow support
# option( WITH_DSHOW OFF)
# 
# #Include FFMPEG support
# option( WITH_FFMPEG OFF)
# 
# # ================ third party =====================
# #Include OpenCL Runtime support
# option( WITH_OPENCL OFF)
# 
# 
# # ================ Current Source and build folders, other then that of root CMakeList =====================
# unset(OpenCV_DIR CACHE)
# 
# 
# # is the directory where the compiled or generated files from the current CMakeLists.txt will go to
# set (CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/opencv2/build)
# 
# # this is the directory where the currently processed CMakeLists.txt is located in
# set (CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/opencv2 )
# 
# #Installation Directory
# set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/opencv2/build/install)
# 
# #Output directory for applications
# set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/opencv2/build/bin)
# 
# set( CMAKE_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/opencv2)
# set( CMAKE_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/opencv2/build)
# 
# # ================ OpenCV CMakeList.txt location =====================
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/opencv2)
# 
# # CMakeLists.txt
# message(STATUS "(1:1)----------------------  OpenCV CmakeList for ${CMAKE_CURRENT_LIST_DIR}" )
# 
# # if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise
# # this is the top level directory of your build tree
# message( STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR} )
# 
# # if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this
# # is the directory where the compiled or generated files from the current CMakeLists.txt will go to
# message( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )
# 
# # this is the directory, from which cmake was started, i.e. the top level source directory
# message( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )
# 
# # this is the directory where the currently processed CMakeLists.txt is located in
# message( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )
# 
# # contains the full path to the top level directory of your build tree
# message( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )
# 
# # contains the full path to the root of your project source directory,
# # i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command
# message( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )
# 
# # set this variable to specify a common place where CMake should put all executable files
# # (instead of CMAKE_CURRENT_BINARY_DIR)
# message( STATUS "EXECUTABLE_OUTPUT_PATH: " ${EXECUTABLE_OUTPUT_PATH} )
# 
# # set this variable to specify a common place where CMake should put all libraries
# # (instead of CMAKE_CURRENT_BINARY_DIR)
# message( STATUS "LIBRARY_OUTPUT_PATH:     " ${LIBRARY_OUTPUT_PATH} )
# 
# # tell CMake to search first in directories listed in CMAKE_MODULE_PATH
# # when you use FIND_PACKAGE() or INCLUDE()
# message( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} )
# 
# # this is the complete path of the cmake which runs currently (e.g. /usr/local/bin/cmake)
# message( STATUS "CMAKE_COMMAND: " ${CMAKE_COMMAND} )
# 
# # this is the CMake installation directory
# message( STATUS "CMAKE_ROOT: " ${CMAKE_ROOT} )
# 
# # this is the filename including the complete path of the file where this variable is used.
# message( STATUS "CMAKE_CURRENT_LIST_FILE: " ${CMAKE_CURRENT_LIST_FILE} )
# 
# # this is linenumber where the variable is used
# message( STATUS "CMAKE_CURRENT_LIST_LINE: " ${CMAKE_CURRENT_LIST_LINE} )
# 
# # this is used when searching for include files e.g. using the FIND_PATH() command.
# message( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )
# 
# # this is used when searching for libraries e.g. using the FIND_LIBRARY() command.
# message( STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH} )
# 
# # the complete system name, e.g. "Linux-2.4.22", "FreeBSD-5.4-RELEASE" or "Windows 5.1"
# message( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )
# 
# # the short system name, e.g. "Linux", "FreeBSD" or "Windows"
# message( STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME} )
# 
# # only the version part of CMAKE_SYSTEM
# message( STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION} )
# 
# # the processor name (e.g. "Intel(R) Pentium(R) M processor 2.00GHz")
# message( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )
# 
# # is TRUE on all UNIX-like OS's, including Apple OS X and CygWin
# message( STATUS "UNIX: " ${UNIX} )
# 
# # is TRUE on Windows, including CygWin
# message( STATUS "WIN32: " ${WIN32} )
# 
# include(${CMAKE_CURRENT_LIST_DIR}/opencv-include.cmake)

