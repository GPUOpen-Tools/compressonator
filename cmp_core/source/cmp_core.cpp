#include "bc1_encode_kernel.h"
#include "cpu_extensions.h"

enum SIMD_ENABLED_EXTENSIONS
{
    SIMD_ENABLED_INVALID = -1,
    SIMD_ENABLED_NONE = 0,
    SIMD_ENABLED_SSE4 = 1,
    SIMD_ENABLED_AVX2 = 2,
    SIMD_ENABLED_AVX512 = 3
};

static int g_simdExtensionSet = SIMD_ENABLED_INVALID;

int CMP_CDECL EnableSSE4()
{
    int error = BC1EnableSSE4();

    g_simdExtensionSet = error == 0 ? SIMD_ENABLED_SSE4 : g_simdExtensionSet;

    return error;
}

int CMP_CDECL EnableAVX2()
{
    int error = BC1EnableAVX2();

    g_simdExtensionSet = error == 0 ? SIMD_ENABLED_AVX2 : g_simdExtensionSet;

    return error;
}

int CMP_CDECL EnableAVX512()
{
    int error = BC1EnableAVX512();

    g_simdExtensionSet = error == 0 ? SIMD_ENABLED_AVX512 : g_simdExtensionSet;

    return error;
}

int CMP_CDECL DisableSIMD()
{
    BC1DisableSIMD();

    g_simdExtensionSet = SIMD_ENABLED_NONE;

    return CGU_CORE_OK;
}

int CMP_CDECL GetEnabledSIMDExtension()
{
    if (g_simdExtensionSet != SIMD_ENABLED_INVALID)
        return g_simdExtensionSet;

    CPUExtensions extensions =  GetCPUExtensions();

    if (IsAvailableAVX512(extensions))
        return SIMD_ENABLED_AVX512;
    else if (IsAvailableAVX2(extensions))
        return SIMD_ENABLED_AVX2;
    else if (IsAvailableSSE4(extensions))
        return SIMD_ENABLED_SSE4;

    return SIMD_ENABLED_NONE;
}