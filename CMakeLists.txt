cmake_minimum_required(VERSION 3.15)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# if(POLICY CMP0091)
#   cmake_policy(SET CMP0091 NEW)
# endif()

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
cmp_option(OPTION_BUILD_EXAMPLES "Build examples" ON)
cmp_option(NO_LEGACY_BEHAVIOR "Disables any legacy behavior (before CMake migration)" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


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
   set(CMAKE_EXE_LINKER_FLAGS "/INCREMENTAL:NO")
endif ()

# '_DEBUG' is only defined by Visual Studio. Make sure it's definied everywhere
add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)

# Additional options
add_compile_definitions(
    USE_QTWEBKIT                # Enable Qt Webengine interfaces for GUI welcome page
    USE_ETCPACK                 # Use ETCPack for ETC2 else use CModel code!
    USE_MESH_CLI                # CLI Process Mesh (only support glTF and OBJ files)
    ENABLE_MAKE_COMPATIBLE_API  # Byte<->Float to make all source and dest compatible
    USE_OLD_SWIZZLE             # Remove swizzle flag and abide by CMP_Formats
    USE_MESH_DRACO_EXTENSION    # Mesh Compression with Draco support in glTF and OBJ files only
    USE_QTWEBKIT                # Enable Qt Webengine interfaces for GUI welcome page

    # USE_CONVECTION_KERNELS                      # Test External Lib : ConvectionKernels for quality& performance, to enable use q=0.1 in HPC
    # USE_GPU_PIPELINE_VULKAN                     # Enable to use CMP_Core codecs on a Vulkan Pipeline
    # USE_CPU_PERFORMANCE_COUNTERS                # Enable to use performance monitoring with CPU high precision counters in place of GPU query performance counters
    # TEST_CMP_CORE_DECODER                       # Test CMP_Core Decoders using Compressonator SDK Decoders, 
    # BC7_DEBUG_TO_RESULTS_TXT                    # Send debug info to a results text file
    # DXT5_COMPDEBUGGER                           # Remote connect data to Comp Debugger views
    # BC6H_COMPDEBUGGER                           # Remote connect data to Comp Debugger views
    # BC7_COMPDEBUGGER                            # Remote connect data to Comp Debugger views
    # BC6H_NO_OPTIMIZE_ENDPOINTS                  # Turn off BC6H optimization of endpoints - useful for debugging quantization and mode checking
    # BC6H_DEBUG_TO_RESULTS_TXT                   # Generates a Results.txt file on exe working directory; MultiThreading is turned off for debuging to text file
    # BC6H_DECODE_DEBUG                           # Enables debug info on decoder
    # GT_COMPDEBUGGER                             # Remote connect data to Comp Debugger views
    # USE_MULTIPLE_MESH_DECODE                    # Enable multiple meshes and multiple primitives draco decompression
    # USE_MESH_DRACO_SETTING                      # Expose draco settings for draco mesh compression, 
                                                  # if disabled default setting will be used for mesh compression
    #USE_GLTF2_MIPSET                             # Enable Image Transcode & Compression support for GLTF files using TextureIO
    #USE_FILEIO                                   # Used for debugging code
    
    #New features under devlopement
    #USE_GTC                                      # LDR Gradient Texture Compressor patent pending
    #USE_APC                                      # Enable APC encoding reserved for future releases
    #USE_BASIS                                    # Future release:: Universal format for transcoding codecs
    #USE_CMP_TRANSCODE                            # Future release:: Transcode BASIS/GTC to other compressed formats
    
    # Possible future releases features
    #ARGB_32_SUPPORT                              # Enables 32bit Float channel formats
    #SUPPORT_ETC_ALPHA                            # for ATC_RGB output enable A
    #SHOW_PROCESS_MEMORY                          # display available CPU process memory
    #USE_BCN_IMAGE_DEBUG                          # Enables Combobox in Image View for low level BCn based block compression in debug mode
    #USE_CRN                                      # Enabled .crn file output using CRUNCH encoder
    #USE_3DCONVERT                                # Enable 3D model conversion (glTF<->obj) icon
    #ENABLE_USER_ETC2S_FORMATS                    # Enable users to set these formats in CLI and GUI applications
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
include(External/cmake/CMakeLists.txt)
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
