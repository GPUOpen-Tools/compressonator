# Compressonator GUI
# this must be before add_executable
link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/opencv/2.49/x64/VS2015/lib
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/zlib/zlib-1.2.10/VS2015/x64/lib
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/openexr/ilmbase-2.2.0/VS2015/x64/lib
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/openexr/openexr-2.2.0/VS2015/x64/lib
    )

# Enable automoc and autorcc
set(CMAKE_INCLUDE_CURRENT_DIR ON) 

add_subdirectory(Common)
add_subdirectory(Components)
add_subdirectory(GeneratedFiles)
add_subdirectory(QPropertyPages)

# Compressonator GUI Application
add_executable(CompressonatorGUI-bin)

add_dependencies(CompressonatorGUI-bin extern_ktx)

set_target_properties(CompressonatorGUI-bin PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOMOC_COMPILER_PREDEFINES ON
    OUTPUT_NAME "compressonator"
)

# Find the QtWidgets library
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

if(CMP_HOST_WINDOWS)
    set_target_properties(CompressonatorGUI-bin PROPERTIES 
        WIN32_EXECUTABLE ON
        AUTOMOC_MOC_OPTIONS -D_WIN32
        VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:CompressonatorGUI-bin>
    )
elseif(CMP_HOST_APPLE)
    set_target_properties(CompressonatorGUI-bin PROPERTIES 
        MACOSX_BUNDLE ON
        LINK_FLAGS "-Wl,-F${CMAKE_OSX_SYSROOT}/System/Library/Frameworks,-v"
    )
endif()

file(GLOB_RECURSE SRCS

    Source/*.cpp
    Source/*.h
)

file(GLOB_RECURSE IMAGES

    images/*
)

file(GLOB_RECURSE RESOURCES

    resources/*
)

target_sources(CompressonatorGUI-bin PRIVATE

    ${SRCS}
    ${RESOURCES}
    CompressonatorGUI.qrc
)

target_include_directories(CompressonatorGUI-bin PRIVATE
     ${PROJECT_SOURCE_DIR}/CMP_CompressonatorLib
    ./Resources
    ./Source
)

target_link_libraries(CompressonatorGUI-bin 

    PRIVATE
        # Static libs
        ExtKTX
        ExtGLEW
        CMP_GUI
        CMP_GUI_BIN_Common
        CMP_GUI_BIN_Components
        CMP_GUI_BIN_GeneratedFiles
        CMP_GUI_BIN_QPropertyPages
        CMP_Mesh
        CompressonatorLIB
        GPU_Decode

        # Static plugin libs
        Plugin_CImage
        Plugin_Common_UtilFuncs
        Plugin_GLTF
        Plugin_PluginManager
        Plugin_TCPluginAPI

        # Dynamic plugins
        CMP_MeshCompressor
        Plugin_C3DModel_Loaders
        Plugin_C3DModel_Viewers
        Plugin_CGPUDecode
        Plugin_CMesh
) 

include(CopyFiles.cmake)

set_target_properties(CompressonatorGUI-bin PROPERTIES FOLDER ${FOLDER_NAME})

if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DESTINATION .
        USE_SOURCE_PERMISSIONS
        PATTERN "cmake_install.cmake" EXCLUDE
        PATTERN "CMakeFiles*" EXCLUDE
        PATTERN "Common*" EXCLUDE
        PATTERN "Components*" EXCLUDE
        PATTERN "CompressonatorGUI-bin_autogen*" EXCLUDE
        PATTERN "CTestTestfile.cmake" EXCLUDE
        PATTERN "GeneratedFiles*" EXCLUDE
        PATTERN "Makefile" EXCLUDE
        PATTERN "QPropertyPages*" EXCLUDE
    )
elseif (CMAKE_GENERATOR STREQUAL "Visual Studio 16 2019")
    install(DIRECTORY "$<TARGET_FILE_DIR:CompressonatorGUI-bin>"
        DESTINATION "./CompressonatorGUI"
        USE_SOURCE_PERMISSIONS
    )
elseif (CMAKE_GENERATOR STREQUAL "Xcode")
    install(DIRECTORY "$<TARGET_BUNDLE_DIR:CompressonatorGUI-bin>"
        DESTINATION "./CompressonatorGUI"
        USE_SOURCE_PERMISSIONS
    )
endif()



#============== Helper to show all currently accessable variables in cmake
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames) 
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
