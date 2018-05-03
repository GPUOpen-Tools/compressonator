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

#ifndef _MIPS_H
#define _MIPS_H

#include "stdlib.h"
#include "TC_PluginAPI.h"

#define MAX_MIPLEVEL_SUPPORTED 20


#define MIPSET_PIN_DATA_ID     "69AEBDB3-5F67-436D-82C2-724FDC4972DA"

// #define USE_MIPSET_FACES

extern void(*PrintStatusLine)(char *);
extern void PrintInfo(const char* Format, ...);
class CMIPS
{
    public: 
        CMIPS()  { };
        ~CMIPS() { };

    MipLevel* GetMipLevel(const MipSet* pMipSet, int nMipLevel, int nFaceOrSlice=0);
    int  GetMaxMipLevels(int nWidth, int nHeight, int nDepth);
    bool AllocateMipLevelTable(MipLevelTable** ppMipLevelTable, int nMaxMipLevels, TextureType textureType, int nDepth, int& nLevelsToAllocate
        #ifdef USE_MIPSET_FACES
        ,int nFaces=0
        #endif
        );
    bool AllocateAllMipLevels(MipLevelTable* pMipLevelTable, TextureType /*textureType*/, int nLevelsToAllocate);
    bool AllocateMipSet(MipSet* pMipSet, ChannelFormat channelFormat, TextureDataType textureDataType, TextureType textureType, int nWidth, int nHeight, int nDepth
#ifdef USE_MIPSET_FACES
        , int nFaces = 0
#endif
    );
    bool AllocateMipLevelData(MipLevel* pMipLevel, int nWidth, int nHeight, ChannelFormat channelFormat, TextureDataType textureDataType
#ifdef USE_MIPSET_FACES
        , int facedataSize =0
#endif
    );
    bool AllocateCompressedMipLevelData(MipLevel* pMipLevel, int nWidth, int nHeight, CMP_DWORD dwSize);

    void FreeMipSet(MipSet* pMipSet);

    void PrintError(const char* Format, ... );

    // User Configurable Print lines 
    int m_infolevel = 1;
    void(*PrintLine)(char *) = nullptr;
    void Print(const char* Format, ...);

    bool m_canceled = false;
    void(*SetProgressValue)(unsigned int, bool *canceled) = nullptr;
    void SetProgress(unsigned int value);
};

#endif
