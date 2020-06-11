# Compressonator GUI

# Enable automoc and autorcc
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# get_property(Qt5Core_VERSION_MAJOR GLOBAL PROPERTY Qt5Core_VERSION_MAJOR)
# get_property(Qt5Core_VERSION_MINOR GLOBAL PROPERTY Qt5Core_VERSION_MINOR)

add_subdirectory(Common)
add_subdirectory(Components)
add_subdirectory(GeneratedFiles)
add_subdirectory(QPropertyPages)

# Compressonator GUI Application
add_executable(CompressonatorGUI-bin)

set_target_properties(CompressonatorGUI-bin PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOMOC_COMPILER_PREDEFINES ON
)

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

    Images/*
)

file(GLOB_RECURSE RESOURCES

    Resources/*
)

target_sources(CompressonatorGUI-bin PRIVATE

    ${SRCS}
    ${RESOURCES}
    CompressonatorGUI.qrc
)

target_include_directories(CompressonatorGUI-bin PRIVATE

    ./Resources
    ./Source
)

target_link_libraries(CompressonatorGUI-bin 

    PRIVATE    
        CMP_GUI
        CMP_GUI_BIN_Common
        CMP_GUI_BIN_Components
        CMP_GUI_BIN_GeneratedFiles
        CMP_GUI_BIN_QPropertyPages
        CMP_Mesh
        CompressonatorLIB
        GPU_Decode
        Plugin_CImage
        Plugin_Common_UtilFuncs
        Plugin_GLTF
        Plugin_PluginManager
        Plugin_TCPluginAPI

        # Real plugins
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
