//=====================================================================
// Copyright (c) 2023-2024   Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//=====================================================================

#include "bc1_encode_kernel.h"
#include "cpu_extensions.h"

enum SIMD_ENABLED_EXTENSIONS
{
    SIMD_ENABLED_INVALID = -1,
    SIMD_ENABLED_NONE    = 0,
    SIMD_ENABLED_SSE4    = 1,
    SIMD_ENABLED_AVX2    = 2,
    SIMD_ENABLED_AVX512  = 3
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

    CPUExtensions extensions = GetCPUExtensions();

    if (IsAvailableAVX512(extensions))
        return SIMD_ENABLED_AVX512;
    else if (IsAvailableAVX2(extensions))
        return SIMD_ENABLED_AVX2;
    else if (IsAvailableSSE4(extensions))
        return SIMD_ENABLED_SSE4;

    return SIMD_ENABLED_NONE;
}