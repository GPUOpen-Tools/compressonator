cmake_minimum_required(VERSION 3.13)

# CMakeLists.txt
message(STATUS "++++++++++++++++++ " )
message(STATUS "++++++++++++++++++ External-KTX" )
message(STATUS "++++++++++++++++++ " )

# if(POLICY CMP0091)
#   cmake_policy(SET CMP0091 NEW)
# endif()

if (CMP_HOST_WINDOWS)
    # build project files
# if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
#   if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 19)
#     set(BOOST_TOOLSET msvc-14.1)

    ExternalProject_Add(extern_ktx
        PREFIX ${PROJECT_SOURCE_DIR}/../common/lib/ext/ktx
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/../common/lib/ext/ktx
        BINARY_DIR ${PROJECT_SOURCE_DIR}/../common/lib/ext/ktx/build
        DOWNLOAD_DIR .
        CMAKE_ARGS 
            -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_SOURCE_DIR}/../common/lib/ext/ktx
            -G ${CMAKE_GENERATOR}
    )
endif()

#project(CompressonatorGLEW)
add_library(ExtKTX INTERFACE)

set(ExtKTX_BIN_PATH ${PROJECT_SOURCE_DIR}/../common/lib/ext/glew/1.9.0/bin/x64)

target_link_libraries(ExtKTX INTERFACE
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/ktx/build/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/ktx.lib
)

target_include_directories(ExtKTX INTERFACE
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/ktx/other_include
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/ktx/include
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/ktx/lib

)
