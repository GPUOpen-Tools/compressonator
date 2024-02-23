//==============================================================================
// Copyright (c) 2021-2024    Advanced Micro Devices, Inc. All rights reserved.
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
//===============================================================================

#include "common_def.h"
#include "bcn_common_api.h"
#include "bcn_common_kernel.h"

#ifdef USE_CMP
#include "bc1_cmp.h"
#endif

//=================================
// External Codecs For Base Testing
//=================================
#ifdef USE_BETSY
#include "./external/bc1_betsy.h"
#endif

#ifdef USE_ICBC
#include "./external/bc1_icbc.h"
#endif

#ifdef USE_RGBCX
// CMP setting ranges vs levels map as
//      quality   = Level
//         0.0    = 0
//         0.1    = 1
//         0.2    = 2
//         ...
//         0.19   = 19
//         > 0.19 = 19
//
#include "./external/bc1_cmprgbcx.h"
#endif

#ifdef USE_INT
#include "./external/bc1_int.h"
#endif

#ifdef USE_STB
#include "./external/bc1_stb.h"
#endif

#ifdef USE_SQUISH
#include "./external/bc1_squish.h"
#endif

#ifdef USE_HUMUS
#include "./external/bc1_humus.h"
#endif

// Compress a BC1 block
static CGU_Vec2ui CompressBlockBC1_UNORM2(CGU_Vec4f rgbblockf[BLOCK_SIZE_4X4], CMP_BC15Options BC15options)
{
#if (defined(USE_CMP) || defined(USE_BETSY) || defined(USE_ICBC) || defined(USE_INT) || defined(USE_RGBCX) || defined(USE_SQUISH) || defined(USE_HUMUS))
#ifdef USE_CMP
    return CompressBlockBC1_NORMALIZED(rgbblockf, BC15options);
#endif
#ifdef USE_BETSY
    CGU_Vec3f rgbblock3f[BLOCK_SIZE_4X4];
    for (int i = 0; i <= BLOCK_SIZE_4X4; i++)
        rgbblock3f[i] = rgbblockf[i].rgb;

    return CompressBlockBC1_BETSY(rgbblock3f, BC15options.m_fquality, BC15options.m_bIsSRGB);
#endif

#ifdef USE_ICBC
    return CompressBlockBC1_ICBC(rgbblockf, BC15options.m_fquality, BC15options.m_bIsSRGB);
#endif

#ifdef USE_INT
    return CompressBlockBC1_INT(rgbblockf, BC15options.m_fquality, BC15options.m_bIsSRGB);
#endif

#ifdef USE_RGBCX
    return CompressBlockBC1_RGBCX(rgbblockf, BC15options.m_fquality, BC15options.m_bIsSRGB);
#endif

#ifdef USE_STB
    return CompressBlockBC1_STB(rgbblockf, BC15options.m_fquality, BC15options.m_bIsSRGB);
#endif

#ifdef USE_SQUISH
    return CompressBlockBC1_SQUISH(rgbblockf, BC15options.m_fquality, BC15options.m_bIsSRGB);
#endif

#ifdef USE_HUMUS
    CGU_Vec3f rgbblock3f[BLOCK_SIZE_4X4];
    for (int i = 0; i <= BLOCK_SIZE_4X4; i++)
        rgbblock3f[i] = rgbblockf[i].rgb;
    return CompressBlockBC1_HUMUS(rgbblock3f, BC15options.m_fquality, BC15options.m_bIsSRGB);
#endif
#else
    CMP_UNUSED(BC15options);
    CMP_UNUSED(rgbblockf);
    CGU_Vec2ui cmpBlock = {0, 0};
    return cmpBlock;
#endif
}
