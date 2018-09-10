cmake_minimum_required (VERSION 2.8)

project (CompressonatorCLI)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
add_definitions(-DDISABLE_TESTCODE)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
add_definitions(-D_LINUX)
endif()

#uncomment below to rebuild all the dependency libraries from cmake
#if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#add_custom_target(
#    dependencyLibs
#    COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/buildCLI_unix.sh
#)
#endif()

set (CMP_INCLUDE_DIRS "")

# Qt5 include path - users install required
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message("Finding Package Qt5 (Qt5Core and Qt5Gui). if not found, for Ubuntu, run initsetup_ubuntu.sh to install. For mac,  run initsetup_mac.sh to install. For window, please make sure install qt5.7 and add the bin path (i.e. C:/Qt/5.7/msvc2015_64/bin) to your environment PATH.")  

find_package(Qt5Core)
if(Qt5Core_FOUND)
list(APPEND CMP_INCLUDE_DIRS "${Qt5Core_INCLUDE_DIRS}/../")
list(APPEND CMP_INCLUDE_DIRS "${Qt5Core_INCLUDE_DIRS}")
else()
message(FATAL_ERROR "Package Qt5 (Qt5Core) are required, but not found. In Unix, run initsetup_unix.sh. If is window, please make sure install qt and add the bin path (i.e. C:\Qt\5.7\msvc2015_64\bin) to environment PATH.")
endif()

find_package(Qt5Gui)
if(Qt5Gui_FOUND)
list(APPEND CMP_INCLUDE_DIRS "${Qt5Gui_INCLUDE_DIRS}")
else()
message(FATAL_ERROR "Package Qt5 (Qt5Gui) are required, but not found. In Unix, run initsetup_unix.sh. If is window, please make sure install qt and add the bin path (i.e. C:\Qt\5.7\msvc2015_64\bin) to environment PATH.")
endif()

# boost and openexr include paths
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin") 
    message("Finding Package Boost and OpenCV. if not found, for Ubuntu, run initsetup_ubuntu.sh to install. For mac,  run initsetup_mac.sh to install.")

    find_package(Boost COMPONENTS system filesystem) 
    if(Boost_FOUND)
        list(APPEND CMP_INCLUDE_DIR ${Boost_INCLUDE_DIRS}) 
    endif()
    
    find_package(OpenCV) 
    if (OpenCV_FOUND)
        list(APPEND CMP_INCLUDE_DIR ${OpenCV_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "Package OpenCV are required, but not found. In Unix, run initsetup_unix.sh to install the prerequisite libs.")
    endif()
    
    list(APPEND CMP_INCLUDE_DIRS "/usr/local/include/OpenEXR/")
    #build openexr from cmake
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        list(APPEND CMP_INCLUDE_DIRS "/usr/local/ilmbase22build/include/OpenEXR/")
        list(APPEND CMP_INCLUDE_DIRS "/usr/local/openexr22build/include/OpenEXR/")
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/Boost/boost_1_59_0/")
    list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/OpenEXR/openexr-2.2.0/VS2015/x64/include/OpenEXR/")
    list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/VS2015/x64/include/OpenEXR/")
	
endif()

list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../Header/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../Header/Codec/ASTC/ARM/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../Header/Codec/ASTC/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/CMP_MeshCompressor/Draco/src/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/CMP_MeshOptimizer/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/gltf/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/json/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CMesh/mesh_optimizer/")
list(APPEND CMP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CMesh/Tootle/")

list(REMOVE_DUPLICATES CMP_INCLUDE_DIRS)
include_directories(${CMP_INCLUDE_DIRS})

set (CMP_SOURCES1 "")
set (CMP_MESHOPT_SRC "")
file(GLOB_RECURSE CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/*.cc")
file(GLOB_RECURSE CMP_MESHOPT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../../Source/CMP_MeshOptimizer/*cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/ATIFormats.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/cmdline.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/MIPS.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/PluginManager.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/TextureIO.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/gltf/GltfCommon.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/gltf/tiny_gltf2_utils.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/gltf/glTFHelpers.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/json/json.h")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/UtilFuncs.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/Common/ModelData.cpp")
list (APPEND CMP_SOURCES1 "${CMAKE_CURRENT_SOURCE_DIR}/../Source/CompressonatorCLI.cpp")


add_executable(CompressonatorCLI ${CMP_SOURCES1} ${CMP_MESHOPT_SRC})

# (ubuntu only)uncomment below to rebuild all the dependency libraries from cmake 
#if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#add_dependencies(CompressonatorCLI dependencyLibs)
#endif()


set (CMP_LIBS "")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    
    find_package(OpenCV) 
    if (OpenCV_FOUND)
        list(APPEND CMP_LIBS ${OpenCV_LIBRARIES})
    else()
        message(FATAL_ERROR "Package OpenCV are required, but not found. In Unix, run initsetup_unix.sh to install the prerequisite libs.")
    endif()
    find_package(Boost COMPONENTS system filesystem) 
    if(Boost_FOUND)
        list(APPEND CMP_LIBS ${Boost_LIBRARIES})
    endif()

	if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libz.a")

        #list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libboost_filesystem.a")
        #list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libboost_system.a") 

        list(APPEND CMP_LIBS "/usr/local/ilmbase22build/lib/libHalf.so")
        list(APPEND CMP_LIBS "/usr/local/ilmbase22build/lib/libImath-2_2.so")
        list(APPEND CMP_LIBS "/usr/local/ilmbase22build/lib/libIlmThread-2_2.so")
        list(APPEND CMP_LIBS "/usr/local/ilmbase22build/lib/libIex-2_2.so")
        list(APPEND CMP_LIBS "/usr/local/openexr22build/lib/libIlmImf-2_2.so")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        list(APPEND CMP_LIBS "/usr/lib/libz.dylib")
        
        #list(APPEND CMP_LIBS "/usr/lib/libboost_filesystem.a")
        #list(APPEND CMP_LIBS "/usr/lib/libboost_system.a") 

        list(APPEND CMP_LIBS "/usr/local/lib/libHalf.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libImath.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libIlmThread.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libIex.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libIlmImf.dylib")
    endif()
    
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../Make/libCompressonator.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CFilter/Make/libBoxFilter.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CImage/ASTC/Make/libASTC.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CImage/DDS/Make/libDDS.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CImage/EXR/Make/libEXR.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CImage/TGA/Make/libTGA.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CImage/KTX/Make/libKTX.a")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CAnalysis/Analysis/Make/libIMGAnalysis.a")
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libz.a")
        list(APPEND CMP_LIBS "/usr/lib/x86_64-linux-gnu/libpthread.so")
    endif()
    
    find_package(GLEW)
    if (GLEW_FOUND)
        list(APPEND CMP_LIBS ${GLEW_LIBRARIES})
    else()
        message(FATAL_ERROR "Package glew are required, but not found. In Unix, run initsetup_unix.sh to install the prerequisite libs.")
    endif()

    target_link_libraries(CompressonatorCLI ${CMP_LIBS} Qt5::Core Qt5::Gui) 

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/glew/1.9.0/lib/x64/glew32.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/zlib/zlib-1.2.10/VS2015/x64/lib/zlibstatic.lib")

    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/Boost/boost_1_59_0/lib/VC14/x86_64/libboost_filesystem-vc140-mt-1_59.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/Boost/boost_1_59_0/lib/VC14/x86_64/libboost_system-vc140-mt-1_59.lib") 

    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/VS2015/x64/lib/Half.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/VS2015/x64/lib/Imath.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/VS2015/x64/lib/IlmThread.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/VS2015/x64/lib/Iex.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/OpenEXR/openexr-2.2.0/VS2015/x64/lib/IlmImf.lib")

    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/OpenCV/2.49/x64/VS2015/lib/opencv_core249.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/OpenCV/2.49/x64/VS2015/lib/opencv_highgui249.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/Lib/Ext/OpenCV/2.49/x64/VS2015/lib/opencv_imgproc249.lib")

    message("Build configuration is ${CMAKE_BUILD_TYPE}.")

    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../Make/${CMAKE_BUILD_TYPE}/Compressonator.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CFilter/Make/${CMAKE_BUILD_TYPE}/BoxFilter.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CImage/ASTC/Make/${CMAKE_BUILD_TYPE}/ASTC.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CImage/DDS/Make/${CMAKE_BUILD_TYPE}/DDS.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CImage/EXR/Make/${CMAKE_BUILD_TYPE}/EXR.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CImage/TGA/Make/${CMAKE_BUILD_TYPE}/TGA.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CImage/KTX/Make/${CMAKE_BUILD_TYPE}/KTX.lib")
    list(APPEND CMP_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../_Plugins/CAnalysis/Analysis/Make/${CMAKE_BUILD_TYPE}/IMGAnalysis.lib")

    message("Finding Package Qt5 (Qt5Widget). For window, please make sure install qt5.7 and add the bin path (i.e. C:/Qt/5.7/msvc2015_64/bin) to your environment PATH.")

    find_package(Qt5Widgets)

    target_link_libraries(CompressonatorCLI ${CMP_LIBS} Qt5::Core Qt5::Gui Qt5::Widgets) 
endif()
