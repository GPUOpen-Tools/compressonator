cmake_minimum_required(VERSION 3.15)

project(Compressonator)

enable_testing()

set(CMAKE_CXX_STANDARD 17)

include(cmake/HelperFunctions.cmake)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported configuration options" FORCE)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/build-out")

cmp_option(OPTION_CMP_DIRECTX "Use Directx" (CMP_HOST_WINDOWS))
cmp_option(OPTION_CMP_OPENGL "Use OpenGL" (CMP_HOST_WINDOWS))
cmp_option(OPTION_CMP_VULKAN "Use Vulkan" (CMP_HOST_WINDOWS))
cmp_option(OPTION_USE_QT_IMAGELOAD "Use Qt for Image Loading" ON)
cmp_option(OPTION_DISABLE_TESTCODE "Remove test code" OFF)
cmp_option(OPTION_BUILD_SHARED_LIBS "Build SHARED, if disabled STATIC is built" (BUILD_SHARED_LIBS))
cmp_option(OPTION_BUILD_TESTING "Build tests" ON)
cmp_option(OPTION_BUILD_EXAMPLES "Build examples" OFF)
cmp_option(NO_LEGACY_BEHAVIOR "Disables any legacy behavior (before CMake migration)" ON)

if (OPTION_BUILD_TESTING)
    enable_testing()
endif()

if (OPTION_DISABLE_TESTCODE)
    add_compile_definitions(DISABLE_TESTCODE=1)
endif ()

if (OPTION_USE_QT_IMAGELOAD)
    add_compile_definitions(USE_QT_IMAGELOAD=1)
endif()

if (OPTION_BUILD_SHARED_LIBS)
    add_compile_definitions(
        BUILD_AS_PLUGIN_DLL=1
        CMP_BUILD_SHARED_LIBS=1
    )
else()
    add_compile_definitions(
        CMP_BUILD_SHARED_LIBS=0
    )
endif()

if (APPLE OR UNIX)
    add_compile_definitions(_LINUX=1)
endif ()

if (NO_LEGACY_BEHAVIOR)
    add_compile_definitions(NO_LEGACY_BEHAVIOR=1)
endif()

if (CMP_HOST_WINDOWS)
    # add_compile_definitions(USE_NewLoader=1)
    add_compile_definitions(WIN32_LEAN_AND_MEAN=1)
endif ()

# '_DEBUG' is only defined by Visual Studio. Make sure it's definied everywhere
add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)

# Additional options
add_compile_definitions(
    USE_QTWEBKIT                # Enable Qt Webengine interfaces for GUI welcome page
    USE_ETCPACK                 # Use ETCPack for ETC2 else use CModel code!
    USE_MESH_CLI                # CLI Process Mesh (only support glTF and OBJ files)

    ENABLE_MAKE_COMPATIBLE_API  # Byte<->Float to make all source and dest compatible

    # V2.4 / V2.5 features and changes
    USE_OLD_SWIZZLE             # Remove swizzle flag and abide by CMP_Formats
)

# Disable code signing
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")

if (CMP_HOST_WINDOWS)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

if (NOT CMP_HOST_APPLE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

# Configure Externals, they must be added via 'include' before the applications
include(External/cmake/DependencyInfo.cmake)
include(External/CMakeLists.txt)

# add_subdirectory(Compressonator ${CMAKE_BINARY_DIR}/Compressonator)

add_subdirectory(CMP_Core)
add_subdirectory(CMP_Framework)
add_subdirectory(CMP_CompressonatorLib)
add_subdirectory(Applications/_Libs)
add_subdirectory(Applications/_Plugins)
add_subdirectory(Applications)

if (OPTION_BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()