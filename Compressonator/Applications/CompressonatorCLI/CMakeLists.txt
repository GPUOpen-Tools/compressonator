cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

add_executable(CompressonatorCLI-bin "")

target_sources(CompressonatorCLI-bin
               PRIVATE
               Source/CompressonatorCLI.cpp
               Source/CompressonatorCLI_Documentation.h
               ../_Plugins/Common/ATIFormats.h
               ../_Plugins/Common/ATIFormats.cpp
               ../_Plugins/Common/cmdline.h
               ../_Plugins/Common/cmdline.cpp
               ../_Plugins/Common/CMP_FileIO.h
               ../_Plugins/Common/CMP_FileIO.cpp
               ../_Plugins/Common/ModelData.h
               ../_Plugins/Common/ModelData.cpp
               ../_Plugins/Common/PluginInterface.h
               ../_Plugins/Common/PluginManager.h
               ../_Plugins/Common/PluginManager.cpp
               ../_Plugins/Common/query_timer.h
               ../_Plugins/Common/query_timer.cpp
               ../_Plugins/Common/TextureIO.h
               ../_Plugins/Common/TextureIO.cpp
               ../_Plugins/Common/gltf/tiny_gltf2.h
               ../_Plugins/Common/gltf/tiny_gltf2_utils.h
               ../_Plugins/Common/gltf/tiny_gltf2_utils.cpp
               )

target_include_directories(CompressonatorCLI-bin
                           PRIVATE
                           Source
                           ../_Plugins/Common
                           ../_Plugins/Common/json
                           ../_Plugins/Common/gltf
                           ../_Plugins/CMesh/mesh_optimizer
                           ../_Libs/GPU_Decode
                           ../_Libs/CMP_MeshOptimizer
                           ../_Libs/CMP_Math
                           ../../CMP_CompressonatorLib
                           ../../CMP_Framework/
                           ../../CMP_Framework/Common/half
                           ../../CMP_Core/shaders
                           ../../CMP_Core/source
                           ${draco_INCLUDE_DIRS}
                           ${OpenEXR_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS})

if(APPLE)
target_include_directories(CompressonatorCLI-bin
                           PRIVATE
                           /usr/local/include/OpenEXR/)
endif()

# Qt5 include path - users install required
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (UNIX)
target_compile_definitions(CompressonatorCLI-bin PRIVATE _LINUX)
endif()

set (CMP_LIBS "")
if (UNIX)
    list(APPEND CMP_LIBS Compressonator
                         CMP_Framework
                         CMP_MeshCompressor
                         ASTC
                         EXR
                         KTX
                         TGA
                         Analysis)
    if(NOT APPLE)
        list(APPEND CMP_LIBS Threads::Threads
                             ${OpenEXR_LIBRARIES})
    else()
        list(APPEND CMP_LIBS "/usr/lib/libz.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libHalf.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libImath.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libIlmThread.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libIex.dylib")
        list(APPEND CMP_LIBS "/usr/local/lib/libIlmImf.dylib")
    endif()
endif()

list(APPEND CMP_LIBS ${Boost_LIBRARIES}
                     ${OpenCV_LIBRARIES}
                     ${draco_LIBRARIES})

target_link_libraries(CompressonatorCLI-bin
                      ${CMP_LIBS}
                      Qt5::Gui
                      )

install(TARGETS CompressonatorCLI-bin)
