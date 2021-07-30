//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2021 (c), Advanced Micro Devices, Inc. All rights reserved.
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
/// \file TextureIO.h
/// \version 2.20
//
//=====================================================================

#ifndef TEXTUREIO_H_
#define TEXTUREIO_H_

#include "compressonator.h"
#include "common.h"
#include "texture.h"

#include <cmath>

#pragma warning( push )
#pragma warning(disable:4100)
#pragma warning(disable:4800)
#pragma warning( pop )

typedef struct {
    bool swizzle = false;
    bool useCPU = true;
    std::string errMessage = "";
} Config;

typedef struct {
    CMP_DWORD        dwSize;
    CMP_DWORD        dwFourCC;
    float            fWeightingRed;
    float            fWeightingGreen;
    float            fWeightingBlue;
    bool             bUseAdaptiveWeighting;
    bool             bDXT1UseAlpha;
    unsigned char    nDXT1AlphaThreshold;
    CMP_Speed     nCompressionSpeed;
#ifdef AMD_COMPRESS_INTERNAL_BUILD
    unsigned char    nRefinementMethod;
    unsigned char    nRefinementSteps;
    unsigned char    nAPCQuality;
    bool             bUseSSE2;
    bool             bDeriveZ;
    bool             bUseOldCompressor;
    bool             bForceFloatPath;
#endif // AMD_COMPRESS_INTERNAL_BUILD
    //  BC7 Options
    CMP_DWORD         dwnumThreads;
    double            fquality;

    bool              brestrictColour;
    bool              brestrictAlpha;
    CMP_DWORD         dwmodeMask;
}    ATICompressor_CompressParams;

typedef struct _R9G9B9E5 {
    union {
        struct {
            uint32_t rm : 9; // r-mantissa
            uint32_t gm : 9; // g-mantissa
            uint32_t bm : 9; // b-mantissa
            uint32_t e : 5; // shared exponent
        };
        uint32_t v;
    };

    operator uint32_t () const {
        return v;
    }

    _R9G9B9E5& operator= (const _R9G9B9E5& floatrgb9e5) {
        v = floatrgb9e5.v;
        return *this;
    }
    _R9G9B9E5& operator= (uint32_t Packed) {
        v = Packed;
        return *this;
    }
} R9G9B9E5;

CMP_FORMAT      GetFormat(MipSet* pMipSet);


extern  CMIPS *g_CMIPS;

bool            IsFileExt(const char *fname, const char *fext);
bool            IsDestinationUnCompressed(const char *fname);
CMP_FORMAT      FormatByFileExtension(const char *fname, MipSet *pMipSet);

int             AMDLoadMIPSTextureImage(const char *SourceFile, MipSet *CMips, bool use_OCV, void *pluginManager);
int             AMDSaveMIPSTextureImage(const char *DestFile, MipSet *CMips, bool use_OCV, CMP_CompressOptions option);

MipSet* DecompressMIPSet(MipSet *MipSetIn, CMP_GPUDecode decodeWith, Config *configSetting, CMP_Feedback_Proc pFeedbackProc);

bool            CompressedFileFormat(std::string file);
bool            FloatFormat(CMP_FORMAT format);

void            astc_find_closest_blockdim_2d(float target_bitrate, int *x, int *y, int consider_illegal);
void            astc_find_closest_blockxy_2d(int *x, int *y, int consider_illegal);

bool            FormatSupportsQualitySetting(CMP_FORMAT format);
bool            FormatSupportsDXTCBase(CMP_FORMAT format);

extern void     SwizzleMipMap(MipSet *pMipSet);
extern bool     KeepSwizzle(CMP_FORMAT destformat);

CMP_FLOAT       F16toF32(CMP_HALFSHORT f);
CMP_HALFSHORT   F32toF16(CMP_FLOAT   f);


std::string GetFileExtension(std::string file, CMP_BOOL incDot, CMP_BOOL upperCase);
#endif
