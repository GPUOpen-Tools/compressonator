
# Include local GUI required libs
add_subdirectory(common)
add_subdirectory(components)
add_subdirectory(qpropertypages)

if(CMP_HOST_WINDOWS)
    # this must be before add_executable
    link_directories(
         ${PROJECT_EXTERNAL_LIBDIR}/opencv/2.49/x64/VS2015/lib
         ${PROJECT_EXTERNAL_LIBDIR}/zlib/zlib-1.2.10/VS2015/x64/lib
         ${PROJECT_EXTERNAL_LIBDIR}/openexr/ilmbase-2.2.0/VS2015/x64/lib
         ${PROJECT_EXTERNAL_LIBDIR}/openexr/openexr-2.2.0/VS2015/x64/lib
         ${PROJECT_EXTERNAL_LIBDIR}/glew/1.9.0/lib/x64 
         ${OpenCV_LIB_DIRS}
        )

    # this must be before add_executable
    if (OPTION_CMP_DIRECTX)
    link_directories(
         ${PROJECT_EXTERNAL_LIBDIR}/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64
        )
    endif()

endif()

file(GLOB_RECURSE RESOURCES
    resources/*
)

file(GLOB_RECURSE APP_SOURCES
               source/cpmaincomponents.cpp
               source/cpmaincomponents.h
               source/main.cpp
               source/qtignorecompilerwarnings.h
               compressonatorgui.qrc

               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/mipstoqimage.cpp
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/mipstoqimage.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/atiformats.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/atiformats.cpp
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/cmdline.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/cmdline.cpp
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/cmp_fileio.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/cmp_fileio.cpp
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/modeldata.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/modeldata.cpp
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/plugininterface.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/pluginmanager.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/pluginmanager.cpp
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/query_timer.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/query_timer.cpp
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/textureio.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/textureio.cpp
)

if(CMP_HOST_WINDOWS)
add_executable(CompressonatorGUI-bin
               WIN32 
               ${RESOURCES}
               ${APP_SOURCES}
               )
else()
add_executable(CompressonatorGUI-bin
               ${RESOURCES}
               ${APP_SOURCES}
               )
endif()

set (CMP_LIBS "")
list(APPEND CMP_LIBS    CMP_Common
                        CMP_Compressonator
                        CMP_Framework
                        CMP_GpuDecode
                        # CMP_MeshCompressor
                        CMP_MeshOptimizer

                        CMP_GUI_Gltf
                        CMP_GUI_Components
                        CMP_GUI_Common
                        CMP_GUI_QPropertyPages

                        Image_Analysis
                        Image_ASTC
                        Image_EXR
                        Image_KTX
                        Image_TGA

                        # min links
                        ${OpenEXR_LIBRARIES}
                        ${OpenCV_LIBRARIES}
                        ${Boost_LIBRARIES}
                        ${draco_LIBRARIES}
                        Threads::Threads
                        )

if(CMP_HOST_WINDOWS)
    list(APPEND CMP_LIBS    ExtQt5 
                            ExtQt5Widgets
                            Qt5::Xml
                            Image_KTX2
                            )
else()
    list(APPEND CMP_LIBS    Qt5::Widgets
                            Qt5::Qml 
                            Qt5::OpenGL 
                            Qt5::WebEngineWidgets 
                            Qt5::Xml
                            )

# allow const char* -> char*
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(CompressonatorGUI-bin 
    PRIVATE
    "-Wno-c++11-compat-deprecated-writable-strings")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(CompressonatorGUI-bin 
    PRIVATE
    "-Wno-write-strings")
endif()


endif()

target_link_libraries(CompressonatorGUI-bin  ${CMP_LIBS} )

target_include_directories(CompressonatorGUI-bin PRIVATE
                          components
                          qpropertypages
                          common
                          ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
                          ${PROJECT_SOURCE_DIR}/cmp_framework/
                          ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
                          ${PROJECT_SOURCE_DIR}/applications/_plugins/common
                          ${PROJECT_SOURCE_DIR}/applications/_plugins/canalysis/analysis
                          ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf/
                          ${PROJECT_SOURCE_DIR}/applications/_plugins/common/json/
                          ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
                          ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_mesh
                          ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
                          # ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshcompressor/draco/src
                          ${PROJECT_SOURCE_DIR}/applications/_plugins/cmesh/mesh_optimizer
                          ${PROJECT_SOURCE_DIR}/cmp_core/shaders
                          ${PROJECT_SOURCE_DIR}/cmp_core/source
                          ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math
                          ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
                          ${PROJECT_EXTERNAL_LIBDIR}/rapidxml
                          ${OpenCV_INCLUDE_DIRS}
                          ${draco_INCLUDE_DIRS} )

if(CMP_HOST_WINDOWS)
    include(copyfiles.cmake)
    set_target_properties(CompressonatorGUI-bin PROPERTIES 
        FOLDER "Applications"
        # LINK_FLAGS "/SUBSYSTEM:CONSOLE"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$(Configuration)"
        AUTOMOC ON
        AUTORCC ON
        AUTOMOC_COMPILER_PREDEFINES ON
        OUTPUT_NAME "compressonator"
        )
else()
    set_target_properties(CompressonatorGUI-bin PROPERTIES 
        FOLDER "Applications"
        AUTOMOC ON
        AUTORCC ON
        AUTOMOC_COMPILER_PREDEFINES ON
        OUTPUT_NAME "compressonator-bin"
        )
endif()

