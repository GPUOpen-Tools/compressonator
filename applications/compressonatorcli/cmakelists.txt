message(STATUS "++++++++++++++++++ CLI link_directories ${CMAKE_CURRENT_LIST_DIR}")

# this must be before add_executable
link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/opencv/2.49/x64/VS2015/lib
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2017/x64
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/zlib/zlib-1.2.10/VS2015/x64/lib
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/openexr/ilmbase-2.2.0/VS2015/x64/lib
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/openexr/openexr-2.2.0/VS2015/x64/lib
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/glew/1.9.0/lib/x64 )

add_executable(CompressonatorCLI-bin)

add_dependencies(CompressonatorCLI-bin extern_ktx)

set_target_properties(CompressonatorCLI-bin PROPERTIES
    AUTORCC ON
    OUTPUT_NAME "compressonatorcli"
)

if(CMP_HOST_WINDOWS)
    set_target_properties(CompressonatorCLI-bin PROPERTIES 
        WIN32_EXECUTABLE OFF             #Build an executable with a WinMain entry point on windows.
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

file(GLOB_RECURSE RESOURCES

    resources/*
)

target_sources(CompressonatorCLI-bin PRIVATE

    ${RESOURCES}
    Source/CompressonatorCLI.cpp
    Source/CompressonatorCLI_Documentation.h
)

target_include_directories(CompressonatorCLI-bin PRIVATE
    ${PROJECT_SOURCE_DIR}/CMP_CompressonatorLib
    ./Source
)

target_link_libraries(CompressonatorCLI-bin 
    PRIVATE
        # Static Libs
        ExtKTX
        ExtGLEW
#        ExtOpenEXR 
        ExtDirectXTex 
        CMP_FileIO
        CMP_Core
        CMP_FrameworkLIB
        CMP_MeshCompressor
        CMP_MeshOptimizer
        CompressonatorLIB
        GPU_Decode
        # Static "plugin" Libs
        Plugin_C3DModel_Loaders
        Plugin_CCMP_Encode
        Plugin_CCMP_SDK
        Plugin_CGPUDecode
        Plugin_CmdLine
        Plugin_CMesh_Mesh_Optimizer
        Plugin_Common_QueryPerformance
        Plugin_GLTF
        # Dynamic plugins
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
