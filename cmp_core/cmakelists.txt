
add_library(CMP_Core)

target_sources(CMP_Core PRIVATE
    shaders/BC1_Encode_kernel.h
    shaders/BC1_Encode_kernel.cpp
    shaders/BC2_Encode_kernel.h
    shaders/BC2_Encode_kernel.cpp
    shaders/BC3_Encode_kernel.h
    shaders/BC3_Encode_kernel.cpp
    shaders/BC4_Encode_kernel.h
    shaders/BC4_Encode_kernel.cpp
    shaders/BC5_Encode_kernel.h
    shaders/BC5_Encode_kernel.cpp
    shaders/BC6_Encode_kernel.h
    shaders/BC6_Encode_kernel.cpp
    shaders/BC7_Encode_Kernel.h
    shaders/BC7_Encode_Kernel.cpp
    shaders/BCn_Common_Kernel.h
    shaders/Common_Def.h

    source/CMP_Core.h
    source/cmp_math_vec4.h
)

target_include_directories(CMP_Core PUBLIC
    shaders
    source
)

if (UNIX OR APPLE)
    target_compile_definitions(CMP_Core PRIVATE ASPM_GPU)
endif()

if (OPTION_BUILD_TESTING)
    add_subdirectory(test)
endif()