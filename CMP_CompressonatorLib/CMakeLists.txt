
add_library(CompressonatorLIB)

file(GLOB_RECURSE CMP_ASM_SRCS
    *.asm
)

set(CMP_ASM_COMPILE_OPTIONS "/arch:SSE" "/arch:SSE2")

foreach(asm_src ${CMP_ASM_SRCS})
    set_source_files_properties(${asm_src} PROPERTIES 
        LANGUAGE ASM_MASM
        COMPILE_DEFINITIONS X64=1 USE_SSE2=1 USE_SSE=1
    )
endforeach()

file(GLOB_RECURSE CMP_SRCS
    *.c
    *.cpp
    *.cxx
    *.h
)

target_sources(CompressonatorLIB PRIVATE

    # ${CMP_ASM_SRCS}
    ${CMP_SRCS}
)

if (CMP_HOST_WINDOWS)
    target_compile_definitions(CompressonatorLIB PRIVATE -D_WIN64)
endif()

target_include_directories(CompressonatorLIB PUBLIC

    ASTC
    ASTC/ARM/
    ATC
    ATI
    BASIS
    BC6H
    BC7
    Block
    Buffer
    DXT
    DXTC
    ETC
    ETC/etcpack/
    GT
    Common
    ./
)

target_link_libraries(CompressonatorLIB PRIVATE

    PRIVATE
        CMP_Math
        CMP_Common
        CMP_FrameworkLIB
        CMP_Core

        Plugin_PluginManager
        Plugin_Common_KernelDef
        Plugin_Common_STBImage

        ExtOpenEXR
        Plugin_Half
        
    INTERFACE
        Plugin_Half
)
