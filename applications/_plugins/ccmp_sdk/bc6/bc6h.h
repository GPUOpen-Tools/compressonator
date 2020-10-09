//=====================================================================
// Copyright (c) 2018    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file BC6H.h
//
//=====================================================================

#ifndef _PLUGIN_COMPUTE_BC6H_H
#define _PLUGIN_COMPUTE_BC6H_H

#include "common_def.h"
#include "plugininterface.h"
#include "bc6_encode_kernel.h"
#include "hpc_compress.h"           // padline defs
#include "cmp_math_common.h"

#pragma warning(push)
#pragma warning(disable : 4244)
#include "half.h"
#pragma warning(pop)
typedef half           CMP_HALF;   ///< A 16-bit floating point number class
#ifdef _WIN32
// {9DFF2240-614C-436E-A2E9-FC0993FD4483}
static const GUID g_GUID = { 0x9dff2240, 0x614c, 0x436e, { 0xa2, 0xe9, 0xfc, 0x9, 0x93, 0xfd, 0x44, 0x83 } };
#else
static const GUID g_GUID = 0;
#endif

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

class Plugin_BC6H : public PluginInterface_Encoder {
  public:
    Plugin_BC6H();
    virtual ~Plugin_BC6H();
    int   TC_PluginSetSharedIO(void* Shared);
    int   TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    void* TC_Create();
    void  TC_Destroy(void* codec);
    int   TC_Init(void   *kernel_options);
    char *TC_ComputeSourceFile(unsigned int  Compute_type);
    void TC_Start();
    void TC_End();
  private:
    KernelOptions *m_KernelOptions;
    void InitCodecDefaults();
    bool InitCodecDone;
    CMIPS *CMips = nullptr;
};

class BC6H_EncodeClass : public CMP_Encoder {
    int CompressBlock(unsigned int xBlock, unsigned int yBlock, void *in, void *out);
    int CompressBlock(void *in, void *out, void *blockoptions);
    int DecompressBlock(unsigned int xBlock, unsigned int yBlock, void *in, void *out);
    int DecompressBlock(void *in, void *out);
    int CompressTexture(void *in, void *out,void *processOptions);
    int DecompressTexture(void *in, void *out,void *processOptions);
};

extern void *make_Plugin_BC6H();

namespace BC6H_FILE {
BC6H_Encode      g_BC6HEncode;
}

//============================= BC6H Codec Host Code ====================================
void init_ramps(BC6H_Encode * BC6HEncode);
void init_members(BC6H_Encode * BC6HEncode);
void init_quant(BC6H_Encode * BC6HEncode);

#include <stdio.h>      /* printf */
#include <math.h>       /* floor */
#include <algorithm>    // std::max

#ifdef REPLACE_CPU_CODE
// Threshold quality below which we will always run fast quality and shaking
// Self note: User should be able to set this?
extern double g_qFAST_THRESHOLD;
extern double g_HIGHQULITY_THRESHOLD;
extern BC6H_Encode g_BC6HEncode;

class BC6HBlockEncoder {
  public:

    BC6HBlockEncoder(DWORD validModeMask,
                     BOOL  imageNeedsAlpha,
                     double quality,
                     BOOL colourRestrict,
                     BOOL alphaRestrict,
                     double performance = 1.0
                    ) {
        //#     Should be done by passing down from constuctor!
        BC6HEncode = &g_BC6HEncode;

        // Bug check : ModeMask must be > 0
        if (validModeMask <= 0)
            BC6HEncode->m_validModeMask = 0xCF;
        else
            BC6HEncode->m_validModeMask = validModeMask;

        BC6HEncode->m_quality = min(1.0, max(quality, 0.0));
        BC6HEncode->m_performance = min(1.0, max(performance, 0.0));
        BC6HEncode->m_imageNeedsAlpha = imageNeedsAlpha;
        BC6HEncodeLocal.m_smallestError = DBL_MAX;
        BC6HEncodeLocal.m_largestError = 0.0;
        BC6HEncode->m_colourRestrict = colourRestrict;
        BC6HEncode->m_alphaRestrict = alphaRestrict;

        BC6HEncode->m_quantizerRangeThreshold = 255 * BC6HEncode->m_performance;

        if (BC6HEncode->m_quality < g_qFAST_THRESHOLD) { // Make sure this is below 0.5 since we are x2 below.
            BC6HEncode->m_shakerRangeThreshold = 0.;

            // Scale m_quality to be a linar range 0 to 1 in this section
            // to maximize quality with fast performance...
            BC6HEncode->m_errorThreshold = 256. * (1.0 - ((BC6HEncode->m_quality*2.0) / g_qFAST_THRESHOLD));
            // Limit the size of the partition search space based on Quality
            BC6HEncode->m_partitionSearchSize = max((1.0 / 16.0), ((BC6HEncode->m_quality*2.0) / g_qFAST_THRESHOLD));
        } else {
            // m_qaulity = set the quality user want to see on encoding
            // higher values will produce better encoding results.
            // m_performance  - sets a perfoamce level for a specified quality level


            if (BC6HEncode->m_quality < g_HIGHQULITY_THRESHOLD) {
                BC6HEncode->m_shakerRangeThreshold = 255 * (BC6HEncode->m_quality / 10);                    // gain  performance within FAST_THRESHOLD and HIGHQULITY_THRESHOLD range
                BC6HEncode->m_errorThreshold = 256. * (1.0 - (BC6HEncode->m_quality / g_qFAST_THRESHOLD));
                // Limit the size of the partition search space based on Quality
                BC6HEncode->m_partitionSearchSize = max((1.0 / 16.0), (BC6HEncode->m_quality / g_qFAST_THRESHOLD));
            } else {
                BC6HEncode->m_shakerRangeThreshold = 255 * BC6HEncode->m_quality;     // lowers performance with incresing values
                BC6HEncode->m_errorThreshold = 0;                         // Dont exit early
                BC6HEncode->m_partitionSearchSize = 1.0;                 // use all partitions for best quality
            }
        }
    };


    ~BC6HBlockEncoder() {
    };

    // This routine compresses a block and returns the RMS error
    double CompressBlock(double in[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG], BYTE   out[COMPRESSED_BLOCK_SIZE]);

  private:
    BC6H_Encode          *BC6HEncode;
    BC6H_Encode_local    BC6HEncodeLocal;
};



extern void GetRamp(DWORD endpoint[][MAX_DIMENSION_BIG],
                    CGU_FLOAT ramp[MAX_DIMENSION_BIG][MAX_ENTRIES_QUANT_TRACE],
                    DWORD clusters[2],
                    DWORD componentBits[MAX_DIMENSION_BIG]);

extern void init_ramps(BC6H_Encode * BC6HEncode);
extern void init_members(BC6H_Encode * BC6HEncode);
extern void init_quant(BC6H_Encode * BC6HEncode);
extern __constant char BC6H_PARTITIONS[MAX_SUBSETS][MAX_PARTITIONS][MAX_SUBSET_SIZE];
#endif

// Used by BC6H_Decode
const float  rampLerpWeights[5][MAX_ENTRIES_QUANT_TRACE] = {
#if USE_FINAL_BC6H_WEIGHTS
    { 0.0 }, // 0 bit index
    { 0.0, 1.0 }, // 1 bit index
    { 0.0, 21.0 / 64.0, 43.0 / 64.0, 1.0 }, // 2 bit index
    { 0.0, 9.0 / 64.0, 18.0 / 64.0, 27.0 / 64.0, 37.0 / 64.0, 46.0 / 64.0, 55.0 / 64.0, 1.0 }, // 3 bit index
    {
        0.0, 4.0 / 64.0, 9.0 / 64.0, 13.0 / 64.0, 17.0 / 64.0, 21.0 / 64.0, 26.0 / 64.0, 30.0 / 64.0,
        34.0 / 64.0, 38.0 / 64.0, 43.0 / 64.0, 47.0 / 64.0, 51.0 / 64.0, 55.0 / 64.0, 60.0 / 64.0, 1.0
    } // 4 bit index
#else
    // Pure linear weights
    { 0.0f}, // 0 bit index
    { 0.0f, 1.0f }, // 1 bit index
    { 0.0f, (1.0f / 3.0f), (2.0f / 3.0f), 1.0f },  // 2 bit index
    { 0.0f, (1.0f / 7.0f), (2.0f / 7.0f), (3.0f / 7.0f), (4.0f / 7.0f), (5.0f / 7.0f), (6.0f / 7.0f), 1.0f },  // 3 bit index
    {
        0.0f, (1.0f / 15.0f), (2.0f / 15.0f), (3.0f / 15.0f), (4.0f / 15.0f), (5.0f / 15.0f), (6.0f / 15.0f), (7.0f / 15.0f),
        ( 8.0f / 15.0f), (9.0f / 15.0f), (10.0f / 15.0f), (11.0f / 15.0f), (12.0f / 15.0f), (13.0f / 15.0f), (14.0f / 15.0f), 1.0f
    } // 4 bit index
#endif
};

#endif
