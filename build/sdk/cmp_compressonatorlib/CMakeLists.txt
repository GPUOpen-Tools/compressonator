file(GLOB_RECURSE CMP_SRCS
    # Lossy Compression
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/apc/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/apc/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/atc/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/atc/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/ati/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/ati/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/ati/*.c
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/basis/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/basis/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/bc6h/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/bc6h/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/bc7/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/bc7/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/block/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/block/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/buffer/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/buffer/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/dxt/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/dxt/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/dxtc/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/dxtc/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/dxtc/*.c
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/etc/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/etc/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/etc/etcpack/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/etc/etcpack/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/etc/etcpack/*.cxx
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/gt/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/gt/*.cpp

    # Common 
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/common/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/common/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_framework/common/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_framework/common/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/cmp_framework/common/half/*.h
    ${COMPRESSONATOR_ROOT_PATH}/cmp_framework/common/half/*.cpp
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/common/atiformats.cpp
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/common/atiformats.h
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/common/format_conversion.h
    ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/common/format_conversion.cpp
)

if (OPTION_BUILD_ASTC)
    file(GLOB_RECURSE CMP_ASTC_SRCS
        ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/astc/*.h
        ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/astc/*.cpp
        ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/astc/arm/*.h
        ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/astc/arm/*.cpp
        )
    list(APPEND CMP_SRCS ${CMP_ASTC_SRCS})
endif()

if (OPTION_BUILD_BROTLIG)
    file(GLOB_RECURSE CMP_BRLG_SRCS
        ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/brotlig/*.h
        ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/brotlig/*.cpp
    )
    list(APPEND CMP_SRCS ${CMP_BRLG_SRCS})
endif()

add_library(CMP_Compressonator 
            SHARED  
            ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/version.h
            ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/common.h
            ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/compress.cpp
            ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/compressonator.h
            ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/compressonator.cpp
            ${CMP_SRCS}
            )

target_include_directories(CMP_Compressonator
                           PRIVATE
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/
                           # Lossy Compression
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/apc
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/atc
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/ati
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/basis
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/bc6h
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/bc7
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/block
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/buffer
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/dxt
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/dxtc
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/etc
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/etc/etcpack/
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/gt

                           # Common
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/common
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_framework/common
                           ${COMPRESSONATOR_ROOT_PATH}/cmp_framework/common/half
                           ${COMPRESSONATOR_ROOT_PATH}/applications/_plugins/common
                           )

set_target_properties(CMP_Compressonator PROPERTIES 
    FOLDER ${PROJECT_FOLDER_SDK_LIBS}
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if (OPTION_BUILD_ASTC)
    target_include_directories(CMP_Compressonator
        PRIVATE
        ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/astc
        ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/astc/arm/
    )
endif()

if (OPTION_BUILD_BROTLIG)
    target_include_directories(CMP_Compressonator PRIVATE ${COMPRESSONATOR_ROOT_PATH}/cmp_compressonatorlib/brotlig)

    # should there be an error if OPTION_CMP_DIRECTX isn't ON?
    target_link_libraries(CMP_Compressonator PRIVATE
        ExtBrotlig
        dxcompiler.lib
        dxgi.lib
        d3d12.lib
        dxguid.lib
    )
endif()

target_link_libraries(CMP_Compressonator PUBLIC CMP_Core)