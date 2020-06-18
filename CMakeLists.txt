cmake_minimum_required(VERSION 3.10)

project(Compressonator)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Gui Widgets OpenGL Qml WebEngineWidgets Xml REQUIRED)

# CMake policy CMP0072
set(OpenGL_GL_PREFERENCE "GLVND")

if (OpenCV_FOUND)
else()
    message(FATAL_ERROR "Package OpenCV are required, but not found. "
            "In Unix, run initsetup_unix.sh or sudo apt-get install "
            "libopencv-dev to install the libs.")
endif()

if (Qt5Gui_FOUND)
else()
  message(FATAL_ERROR "Package Qt5 (Qt5Gui) are required, but not found. "
          "In Unix, run initsetup_unix.sh or sudo apt-get install "
          "qtdeclarative5-dev. If is window, please make sure install qt and "
          "add the bin path (i.e. C:\Qt\5.7\msvc2015_64\bin) to environment PATH.")
endif()

if (UNIX)
  find_package(PkgConfig REQUIRED)

  # OpenEXRConfig.cmake is broken on Linux, use PkgConfig
  # find_package(OpenEXR REQUIRED)
  pkg_check_modules(OpenEXR REQUIRED OpenEXR)

  # Draco cmake find scripts are also broken (looks for src/ path in install directory)
  # find_package(Draco REQUIRED)
  pkg_check_modules(draco draco)

  if(draco_FOUND)
  else()
    # TODO: To do this properly we require a generated config header from CMake
    # Which can be done with configure_file.
    # See: https://cmake.org/cmake/help/latest/command/configure_file.html
    add_definitions(-DCMAKE_DRACO_NOT_FOUND)
  endif()
endif()

#gltf code needs _LINUX define
if (UNIX)
  add_definitions(-D_LINUX)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(CMP_Framework)
add_subdirectory(CMP_CompressonatorLib)
add_subdirectory(Applications/_Plugins/CImage/ASTC)
add_subdirectory(Applications/_Plugins/CImage/EXR)
add_subdirectory(Applications/_Plugins/CImage/KTX)
add_subdirectory(Applications/_Plugins/CImage/TGA)
add_subdirectory(Applications/_Plugins/CAnalysis/Analysis)
add_subdirectory(Applications/_Plugins/Common)
add_subdirectory(Applications/_Libs/CMP_MeshCompressor)
add_subdirectory(Applications/_Libs/CMP_MeshOptimizer)
add_subdirectory(Applications/_Libs/GPU_Decode)
add_subdirectory(Applications/CompressonatorCLI)
add_subdirectory(Applications/CompressonatorGUI)

add_dependencies(CompressonatorCLI-bin Compressonator CMP_Framework ASTC EXR KTX TGA Analysis CMP_MeshCompressor)
