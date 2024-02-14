
add_executable(cmp_unittests)

target_sources(cmp_unittests
    PRIVATE

    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/utilfuncs.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/utilfuncs.cpp

    test_main.cpp
    
    core_tests.h
    core_tests.cpp

    core_simd_tests.cpp
    framework_tests.cpp
    sdk_tests.cpp
    mipmap_tests.cpp
    codecbuffer_tests.cpp
    codec_tests.cpp
    fileio_test.cpp

    blockconstants.h
    bc6h_tests.cpp
)

target_include_directories(cmp_unittests
    PUBLIC

    ./
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders
    ${PROJECT_SOURCE_DIR}/cmp_core/source
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/catch2
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/buffer/
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/common/
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/block/
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/etc/
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/etc/etcpack/
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
)

target_link_libraries(cmp_unittests 
    CMP_Math
    CMP_Core
    CMP_Framework
    CMP_Compressonator
)

if (OPTION_BUILD_BROTLIG)
    target_sources(cmp_unittests PRIVATE brlg_data.h)

    target_link_libraries(cmp_unittests ExtBrotlig)
endif()

if(CMP_HOST_WINDOWS)
    include(copyfiles.cmake)
endif()

set_target_properties(cmp_unittests PROPERTIES 
    FOLDER ${PROJECT_FOLDER_APPS}
    OUTPUT_NAME "cmp_unittests" 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$(Configuration)"
)
