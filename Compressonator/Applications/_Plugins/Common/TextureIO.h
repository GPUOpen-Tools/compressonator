//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#define USE_QT_IMAGELOAD

#include "Compressonator.h"
#include "stdafx.h"
#include "Texture.h"
#include "cmdline.h"
#include "MIPS.h"
#include "ImfStandardAttributes.h"
#include "ImathBox.h"
#include "ImfArray.h"
#include "ImfRgba.h"

#ifdef USE_QT_IMAGELOAD
#include <QtGui/qrgb.h>
#endif

typedef struct {
    bool swizzle = false;
    bool useCPU = true;
    string errMessage = "";
} Config;

typedef struct
{
    CMP_DWORD            dwSize;
    CMP_DWORD            dwFourCC;
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


CMP_FORMAT      GetFormat(MipSet* pMipSet);
void            Format2FourCC(CMP_FORMAT format, MipSet *pMipSet);

extern  CMIPS *g_CMIPS;

bool            IsFileExt(const char *fname, const char *fext);
bool            IsDestinationUnCompressed(const char *fname);
CMP_FORMAT      FormatByFileExtension(const char *fname, MipSet *pMipSet);

int             AMDLoadMIPSTextureImage(const char *SourceFile, MipSet *CMips, bool use_OCV, void *pluginManager);
int             AMDSaveMIPSTextureImage(const char *DestFile, MipSet *CMips, bool use_OCV);

MipSet* DecompressMIPSet(MipSet *MipSetIn, CMP_GPUDecode decodeWith, Config *configSetting, CMP_Feedback_Proc pFeedbackProc);

#ifdef USE_QT_IMAGELOAD
QRgb            floatToQrgba(float r, float g, float b, float a);
#endif

bool            CompressedFormat(CMP_FORMAT format);
bool            CompressedFileFormat(string file);
bool            FloatFormat(CMP_FORMAT format);

void            astc_find_closest_blockdim_2d(float target_bitrate, int *x, int *y, int consider_illegal);
void            astc_find_closest_blockxy_2d(int *x, int *y, int consider_illegal);

bool            FormatSupportsQualitySetting(CMP_FORMAT format);
bool            FormatSupportsDXTCBase(CMP_FORMAT format);

extern void     SwizzleMipMap(MipSet *pMipSet);
extern bool     KeepSwizzle(CMP_FORMAT destformat);

CMP_FLOAT      F16toF32(CMP_HALF f);
CMP_HALF    F32toF16(CMP_FLOAT   f);
#ifndef _WIN32
int MaxFacesOrSlices(const MipSet* pMipSet, int nMipLevel);
#endif
#endif
