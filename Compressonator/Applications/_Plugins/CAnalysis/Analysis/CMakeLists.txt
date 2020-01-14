cmake_minimum_required(VERSION 3.10)

add_library(Analysis STATIC "")

target_sources(Analysis
               PRIVATE
               ../../Common/ATIFormats.h
               ../../Common/ATIFormats.cpp
               ../../Common/PluginManager.h
               ../../Common/PluginManager.cpp
               ../../Common/SSIM.h
               ../../Common/SSIM.cpp
               ../../Common/TextureIO.h
               ../../Common/TextureIO.cpp
               ../../../CompressonatorGUI/Components/cpImageLoader.h
               ../../../CompressonatorGUI/Components/cpImageLoader.cpp
               ../../../CompressonatorGUI/Common/cvmatandqimage.h
               ../../../CompressonatorGUI/Common/cvmatandqimage.cpp
               ./CAnalysis.cpp
               ./CAnalysis.h
               )
 
find_package(OpenCV) 
if (OpenCV_FOUND)
    target_include_directories(Analysis
                               PRIVATE
                               ${OpenCV_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Package OpenCV are required, but not found. In Unix, run initsetup_unix.sh or sudo apt-get install libopencv-dev to install the libs.")
endif()

# Qt5 include path - users install required
set_property(TARGET Analysis PROPERTY POSITION_INDEPENDENT_CODE TRUE)

find_package(Qt5Gui)
if(Qt5Gui_FOUND)
target_include_directories(Analysis
                           PRIVATE 
                           "${Qt5Gui_INCLUDE_DIRS}")
else()
message(FATAL_ERROR "Package Qt5 (Qt5Gui) are required, but not found. In Unix, run initsetup_unix.sh or sudo apt-get install qtdeclarative5-dev. If is window, please make sure install qt and add the bin path (i.e. C:\Qt\5.7\msvc2015_64\bin) to environment PATH.")
endif()

target_include_directories(Analysis
                           PRIVATE
                           ../../../../CMP_CompressonatorLib
                           ../../../../CMP_Framework/Common/half
                           ../../../../Applications/_Libs/GPU_Decode
                           ../../Common/
                           ../../../CompressonatorGUI/Common/
                           ../../../CompressonatorGUI/Components/
                           ${OpenEXR_INCLUDE_DIRS}
                           )
if(APPLE)
target_include_directories(Analysis
                           PRIVATE
                           /usr/local/include/OpenEXR/)
find_package(Boost COMPONENTS system filesystem) 
if(Boost_FOUND)
    target_include_directories(Analysis PRIVATE ${Boost_INCLUDE_DIRS})
endif()
endif()

if (UNIX)
target_compile_definitions(Analysis PRIVATE _LINUX)
endif()
