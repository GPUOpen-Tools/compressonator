//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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

// DDS.h : Defines the entry point for the DLL application.


#ifndef _DDS_HELPERS_H
#define _DDS_HELPERS_H


#include "tc_pluginapi.h"
#include "texture.h"
#include "dds.h"

typedef struct _ARGB8888Struct {
    int        nFlags;
    void*    pMemory;
    int        nRMask;
    int        nRShift;
    int        nGMask;
    int        nGShift;
    int        nBMask;
    int        nBShift;
} ARGB8888Struct;

typedef enum _ExtraFlags {
    EF_None = 0,
    EF_UseBitMasks = 0x1,
} ExtraFlags;

typedef TC_PluginError (PreLoopFunction)(FILE*& pFile, DDSD2*& pDDSD, MipSet*& pMipSet, void*& extra);
typedef TC_PluginError (LoopFunction)(FILE*& pFile, DDSD2*& pDDSD, MipSet*& pMipSet, void*& extra, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
typedef TC_PluginError (PostLoopFunction)(FILE*& pFile, DDSD2*& pDDSD, MipSet*& pMipSet, void*& extra);
TC_PluginError GenericLoadFunction(FILE*& pFile, DDSD2*& pDDSD, MipSet*& pMipSet, void*& extra, ChannelFormat channelFormat, TextureDataType textureDataType, PreLoopFunction fnPreLoop, LoopFunction fnLoop, PostLoopFunction fnPostLoop);

bool IsD3D10Format(const MipSet* pMipSet);
void DetermineTextureType(const DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError GenericLoadFunction(FILE*& pFile, DDSD2*& pDDSD, MipSet*& pMipSet, void*& extra, ChannelFormat channelFormat,
                                   TextureDataType textureDataType, PreLoopFunction fnPreLoop, LoopFunction fnLoop, PostLoopFunction fnPostLoop);
TC_PluginError PreLoopDefault(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError LoopDefault(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*& extra, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopDefault(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError PreLoopFourCC(FILE*& pFile, DDSD2*& pDDSD, MipSet*& pMipSet, void*& extra);
TC_PluginError LoopFourCC(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*& /*extra*/, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopFourCC(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError PreLoopRGB565(FILE*&, DDSD2*& pDDSD, MipSet*& pMipSet, void*& extra);
TC_PluginError LoopRGB565(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*& extra, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopRGB565(FILE*&, DDSD2*&, MipSet*&, void*& extra);
TC_PluginError PreLoopRGB888(FILE*&, DDSD2*& pDDSD, MipSet*& pMipSet, void*& extra);
TC_PluginError LoopRGB888(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*& extra, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopRGB888(FILE*&, DDSD2*&, MipSet*&, void*& extra);
TC_PluginError PreLoopRGB8888(FILE*&, DDSD2*&, MipSet*& pMipSet, void*&);
TC_PluginError LoopRGB8888(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*& extra, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError LoopRGB8888_S(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*& extra, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopRGB8888(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError PreLoopABGR32F(FILE*&, DDSD2*&, MipSet*& pMipSet, void*&);
TC_PluginError LoopABGR32F(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopABGR32F(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError LoopGR32F(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError LoopR32F(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError LoopR16F(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PreLoopABGR16F(FILE*&, DDSD2*&, MipSet*& pMipSet, void*&);
TC_PluginError LoopABGR16F(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopABGR16F(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError PreLoopG8(FILE*&, DDSD2*&, MipSet*& pMipSet, void*&);
TC_PluginError LoopG8(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopG8(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError PreLoopAG8(FILE*&, DDSD2*&, MipSet*& pMipSet, void*&);
TC_PluginError LoopAG8(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopAG8(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError PreLoopG16(FILE*&, DDSD2*&, MipSet*& pMipSet, void*&);
TC_PluginError LoopG16(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopG16(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError PreLoopA8(FILE*&, DDSD2*&, MipSet*& pMipSet, void*&);
TC_PluginError LoopA8(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&,int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopA8(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError PreLoopABGR16(FILE*&, DDSD2*&, MipSet*& pMipSet, void*&);
TC_PluginError LoopABGR16(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&,int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopABGR16(FILE*&, DDSD2*&, MipSet*&, void*&);

TC_PluginError PreLoopG16R16(FILE*&, DDSD2*&, MipSet*& pMipSet, void*&);
TC_PluginError LoopG16R16(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopG16R16(FILE*&, DDSD2*&, MipSet*&, void*&);

TC_PluginError PreLoopABGR32(FILE*&, DDSD2*&, MipSet*& pMipSet, void*&);
TC_PluginError LoopABGR32(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&,int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopABGR32(FILE*&, DDSD2*&, MipSet*&, void*&);

TC_PluginError LoopR32G32(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&,int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError LoopR10G10B10A2(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&,int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError LoopR9G9B9E5(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);

TC_PluginError LoopR16G16(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&,int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError LoopR32(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError LoopR8G8(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&,int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);

TC_PluginError PreLoopR16(FILE*&, DDSD2*&, MipSet*&, void*&);
TC_PluginError LoopR16(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);
TC_PluginError PostLoopR16(FILE*&, DDSD2*&, MipSet*&, void*&);

TC_PluginError LoopR8(FILE*& pFile, DDSD2*&, MipSet*& pMipSet, void*&, int nMipLevel, int nFaceOrSlice, CMP_DWORD dwWidth, CMP_DWORD dwHeight);

bool SetupDDSD(DDSD2& ddsd2, const MipSet* pMipSet, bool bCompressed);
bool SetupDDSD_DX10(DDSD2& ddsd2, const MipSet* pMipSet, bool bCompressed);

#endif
