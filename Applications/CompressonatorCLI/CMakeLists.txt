
add_executable(CompressonatorCLI-bin)

set_target_properties(CompressonatorCLI-bin PROPERTIES
    OUTPUT_NAME "compressonator"
)

if(CMP_HOST_WINDOWS)
    set_target_properties(CompressonatorCLI-bin PROPERTIES 
        WIN32_EXECUTABLE ON
        VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:CompressonatorCLI-bin>
    )
elseif(CMP_HOST_APPLE)
    set_target_properties(CompressonatorCLI-bin PROPERTIES
        LINK_FLAGS "-Wl,-F${CMAKE_OSX_SYSROOT}/System/Library/Frameworks,-v"
    )
endif()

target_compile_definitions(CompressonatorCLI-bin PUBLIC
    -DUSE_WITH_COMMANDLINE_TOOL
#    -DUSE_CMP_SDK
#    -DUSE_AMD_PNG
)

target_sources(CompressonatorCLI-bin PRIVATE

    Source/CompressonatorCLI.cpp
    Source/CompressonatorCLI_Documentation.h
)

target_include_directories(CompressonatorCLI-bin PRIVATE

   Source
)

target_link_libraries(CompressonatorCLI-bin 
    PRIVATE
        CMP_FileIO
        CMP_Core
        CMP_FrameworkLIB
        CMP_MeshCompressor
        CMP_MeshOptimizer
        CompressonatorLIB

        ExtOpenEXR
        GPU_Decode
        Plugin_C3DModel_Loaders
        Plugin_CCMP_Encode
        Plugin_CCMP_SDK
        Plugin_CGPUDecode
        Plugin_CmdLine
        Plugin_CMesh_Mesh_Optimizer
        Plugin_Common_QueryPerformance
        Plugin_GLTF

        # Real plugins
        Plugin_CImage
        Plugin_CAnalysis
)

if (OPTION_USE_QT_IMAGELOAD)
    target_link_libraries(CompressonatorCLI-bin 
        PRIVATE
            ExtQt5
    )
endif()

if (OPTION_BUILD_TESTING)
    add_subdirectory(Tests)
endif()

include(CopyFiles.cmake)

set_target_properties(CompressonatorCLI-bin PROPERTIES FOLDER ${FOLDER_NAME})

if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DESTINATION .
        USE_SOURCE_PERMISSIONS
        PATTERN "cmake_install.cmake" EXCLUDE
        PATTERN "CMakeFiles*" EXCLUDE
        PATTERN "CTestTestfile.cmake" EXCLUDE
        PATTERN "Makefile" EXCLUDE
        PATTERN "Tests*" EXCLUDE
    )
elseif (CMAKE_GENERATOR STREQUAL "Visual Studio 16 2019")
    install(DIRECTORY "$<TARGET_FILE_DIR:CompressonatorCLI-bin>"
        DESTINATION "./CompressonatorCLI"
        USE_SOURCE_PERMISSIONS
    )
elseif (CMAKE_GENERATOR STREQUAL "Xcode")
    install(DIRECTORY "$<TARGET_FILE_DIR:CompressonatorCLI-bin>"
        DESTINATION "./CompressonatorCLI"
        USE_SOURCE_PERMISSIONS
    )
endif()
