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

#include "stdafx.h"
#include <stdarg.h>
#include <stdio.h>
#include "MIPS.h"
#include <assert.h>
#include "TC_PluginInternal.h"

void(*PrintStatusLine)(char *) = NULL;


void PrintInfo(const char* Format, ... )
{
    if (PrintStatusLine)
    {
        // define a pointer to save argument list
        va_list args;
        char buff[1024];
        // process the arguments into our debug buffer
        va_start(args, Format);
        vsprintf(buff, Format, args);
        va_end(args);
        PrintStatusLine(buff);
    }
}

MipLevel* CMIPS::GetMipLevel(const MipSet* pMipSet, int nMipLevel,    int nFaceOrSlice)
{
    if(!pMipSet)
    {
        assert(pMipSet);
        return NULL;
    }

    if (!pMipSet->m_pMipLevelTable)
    {
        return NULL;
    }

    if (nMipLevel > MAX_MIPLEVEL_SUPPORTED)
    {
        return NULL;
    }

    if(nMipLevel > pMipSet->m_nMaxMipLevels)
    {
        assert(nMipLevel <= pMipSet->m_nMaxMipLevels);
        return NULL;
    }
    if(nFaceOrSlice < 0)
    {
        return NULL;    //not an error, indicates requested face doesn't exist
    }
    int nDepth = pMipSet->m_nDepth, index = 0, whichMipLevel = 0;

    switch(pMipSet->m_TextureType)
    {
    case TT_2D:
        if(nFaceOrSlice != 0)
        {
            return NULL;
        }
        return (pMipSet->m_pMipLevelTable)[nMipLevel];
    case TT_CubeMap:
        if(nFaceOrSlice > 6)  //cubemap have at most 6 faces
        {
            assert(nFaceOrSlice > 6);
            return NULL;
        }        
        return (pMipSet->m_pMipLevelTable)[nMipLevel * nDepth + nFaceOrSlice];
    case TT_VolumeTexture:
        while(whichMipLevel <= nMipLevel)
        {
            if(whichMipLevel == nMipLevel)
            {
                return (pMipSet->m_pMipLevelTable)[index + nFaceOrSlice];
            }
            else
            {
                index += nDepth;
                whichMipLevel++;
                nDepth = nDepth>1 ? nDepth>>1 : 1;
            }
        }
        return NULL;
    default:
        assert(0);
        return NULL;
    }
}

int CMIPS::GetMaxMipLevels(int nWidth, int nHeight, int nDepth)
{
    int maxMipLevels = 1;
    assert(nWidth > 0 && nHeight > 0 && nDepth > 0);

    while (nWidth >= 1 || nHeight >= 1 || nDepth > 1)
    {
        maxMipLevels++;

        if (nWidth == 1 || nHeight == 1)
            break;
        //div by 2
        nWidth = nWidth>1 ? nWidth>>1 : 1;
        nHeight = nHeight>1 ? nHeight>>1 : 1;
        nDepth = nDepth>1 ? nDepth>>1 : 1;
 
    }
    return maxMipLevels;
}

bool CMIPS::AllocateMipLevelTable(MipLevelTable** ppMipLevelTable, int nMaxMipLevels, TextureType textureType, int nDepth, int& nLevelsToAllocate
#ifdef USE_MIPSET_FACES
    , int nFaces = 0
#endif
)
{
    //TODO test
    assert(nDepth > 0);
    nLevelsToAllocate = 0;
    //determine # miplevels to allocate based on texture type
    switch(textureType)
    {
    case TT_2D:
        nLevelsToAllocate = nMaxMipLevels;
        if(nDepth != 1)
        {
            return false;
        }
        break;
    case TT_CubeMap:
#ifdef USE_MIPSET_FACES
        if(nDepth > 6)
        {
            return false;
        }
        nLevelsToAllocate = nMaxMipLevels * nFaces;
#else
        if (nDepth > 6)
        {
            return false;
        }
        nLevelsToAllocate = nMaxMipLevels * nDepth;
#endif
        break;
    case TT_VolumeTexture:
        for(int i=0; i < nMaxMipLevels; i++)
        {
            nLevelsToAllocate += nDepth;
            if(nDepth > 1)
            {
                nDepth >>= 1;
            }
        }
        break;
    default:
        return false;
    }
    //allocate the mipLevelTable (buncha pointers to miplevels)
    *ppMipLevelTable = reinterpret_cast<MipLevelTable*>(calloc(nLevelsToAllocate, sizeof(MipLevel*)));
    assert(*ppMipLevelTable);
    return (*ppMipLevelTable != NULL);
}

bool CMIPS::AllocateAllMipLevels(MipLevelTable* pMipLevelTable, TextureType /*textureType*/, int nLevelsToAllocate)
{
    //TODO test
    //allocate each MipLevel that the table points to
    for(int i=0; i<nLevelsToAllocate; i++)
    {
        pMipLevelTable[i] = reinterpret_cast<MipLevel*>(calloc(sizeof(MipLevel), 1));
        //make sure it was allocated ok
        assert(pMipLevelTable[i]);
        if(!pMipLevelTable[i])
        {
            //free previous mipLevels
            for(i -= 1; i>=0; i--)
            {
                if (pMipLevelTable[i])
                {
                    free(pMipLevelTable[i]);
                    pMipLevelTable[i] = NULL;
                }
            }
            return false;
        }
    }
    return true;
}

bool CMIPS::AllocateMipSet(MipSet* pMipSet, ChannelFormat channelFormat, TextureDataType textureDataType, TextureType textureType, int nWidth, int nHeight, int nDepth
#ifdef USE_MIPSET_FACES
    , int nFaces = 0
#endif
)
{
    //TODO test
    assert(pMipSet);
   if (!(nWidth > 0 && nHeight > 0 && nDepth > 0)) return false;

    if(pMipSet->m_pMipLevelTable)
    {
        assert(!pMipSet->m_pMipLevelTable);
        return false;
    }
    //depth only matters for this when its volume texture
    pMipSet->m_nMaxMipLevels = GetMaxMipLevels(nWidth, nHeight, textureType==TT_VolumeTexture ? nDepth : 1);
    if(pMipSet->m_nMipLevels > pMipSet->m_nMaxMipLevels || pMipSet->m_nMipLevels < 0)
        pMipSet->m_nMipLevels = 0;
    pMipSet->m_ChannelFormat = channelFormat;
    pMipSet->m_TextureDataType = textureDataType;
    pMipSet->m_TextureType = textureType;
    //Probably shouldn't wipe this out either pMipSet->m_Flags = MS_Default;
    //On second thought, DONT wipe this out pMipSet->m_CubeFaceMask = 0;
    pMipSet->m_nWidth = nWidth;
    pMipSet->m_nHeight = nHeight;
    pMipSet->m_nDepth = nDepth;
    int numLevelsToAllocate;
    if(!AllocateMipLevelTable(&pMipSet->m_pMipLevelTable, pMipSet->m_nMaxMipLevels, textureType, nDepth, numLevelsToAllocate
#ifdef USE_MIPSET_FACES
        , nFaces
#endif
    ))
    {
        //mipleveltable allocation failed
        return false;
    }
    if(!AllocateAllMipLevels(pMipSet->m_pMipLevelTable, textureType, numLevelsToAllocate))
    {
        //allocation of mip levels failed
        if (pMipSet->m_pMipLevelTable)
        {
            free(pMipSet->m_pMipLevelTable);
            pMipSet->m_pMipLevelTable = NULL;
        }
        return false;
    }
    return true;
}

bool CMIPS::AllocateMipLevelData(MipLevel* pMipLevel, int nWidth, int nHeight, ChannelFormat channelFormat, TextureDataType textureDataType
#ifdef USE_MIPSET_FACES
    , int facedataSize
#endif
)

{
    //TODO test
    assert(pMipLevel);
    assert(nWidth > 0 && nHeight > 0);

#ifdef USE_MIPSET_FACES
    if (facedataSize != 0) {  //facedataSize  = number of bytes each faces read from file
        pMipLevel->m_dwLinearSize = facedataSize;
        pMipLevel->m_nWidth = nWidth;
        pMipLevel->m_nHeight = nHeight;

        pMipLevel->m_pbData = reinterpret_cast<CMP_BYTE*>(malloc(pMipLevel->m_dwLinearSize));

        return (pMipLevel->m_pbData != NULL);
    }
#endif

    CMP_DWORD dwBitsPerPixel;
    switch(channelFormat)
    {
        case CF_8bit:
        case CF_2101010:
        case CF_Float9995E:
            dwBitsPerPixel = 8;
            break;

        case CF_16bit:
        case CF_Float16:
            dwBitsPerPixel = 16;
            break;

        case CF_32bit:
        case CF_Float32:
            dwBitsPerPixel = 32;
            break;

        default:
            assert(0);
            return false;
    }

    switch(textureDataType)
    {
        case TDT_XRGB:
        case TDT_ARGB:
        case TDT_NORMAL_MAP:
            dwBitsPerPixel *= 4;
            break;

        case TDT_R:
            break;

        case TDT_RG:
            dwBitsPerPixel *= 2;
            break;

        default:
            assert(0);
            return false;
    }

    CMP_DWORD dwPitch = PAD_BYTE(nWidth, dwBitsPerPixel);
    pMipLevel->m_dwLinearSize = dwPitch * nHeight;
    pMipLevel->m_nWidth = nWidth;
    pMipLevel->m_nHeight = nHeight;

    pMipLevel->m_pbData = reinterpret_cast<CMP_BYTE*>(malloc(pMipLevel->m_dwLinearSize));

    return (pMipLevel->m_pbData != NULL);
}

bool CMIPS::AllocateCompressedMipLevelData(MipLevel* pMipLevel, int nWidth, int nHeight, CMP_DWORD dwSize)
{
    //TODO test
    assert(pMipLevel);
    assert(nWidth > 0 && nHeight > 0);

    pMipLevel->m_dwLinearSize = dwSize;
    pMipLevel->m_nWidth = nWidth;
    pMipLevel->m_nHeight = nHeight;

    pMipLevel->m_pbData = reinterpret_cast<CMP_BYTE*>(malloc(pMipLevel->m_dwLinearSize));

    return (pMipLevel->m_pbData != NULL);
}

void CMIPS::FreeMipSet(MipSet* pMipSet)
{
    //TODO test
    int nTotalOldMipLevels = 0;
    assert(pMipSet);
    if(pMipSet)
    {
        if(pMipSet->m_pMipLevelTable)
        {
            //determine number of miplevels in the old mipleveltable
            switch(pMipSet->m_TextureType)
            {
            case TT_2D:
                nTotalOldMipLevels = pMipSet->m_nMaxMipLevels;
                break;
            case TT_CubeMap:
                nTotalOldMipLevels = pMipSet->m_nMaxMipLevels * pMipSet->m_nDepth;
                break;
            case TT_VolumeTexture:        
                for(int depth=pMipSet->m_nDepth, mipLevels=0;
                    mipLevels < pMipSet->m_nMaxMipLevels;
                    mipLevels++)
                {
                    nTotalOldMipLevels += depth;
                    if(depth > 1)
                    {
                        depth >>= 1;
                    }
                }
                break;
            default:
                assert(0);
            }
            //free all miplevels and their data except the one use in gui view
            for(int i=0; i<nTotalOldMipLevels-2 ; i++)
            {
                if (pMipSet->m_pMipLevelTable[i]->m_pbData)
                {
                    free(pMipSet->m_pMipLevelTable[i]->m_pbData);
                    pMipSet->m_pMipLevelTable[i]->m_pbData = NULL;
                }

                if (pMipSet->m_pMipLevelTable[i])
                {
                    free(pMipSet->m_pMipLevelTable[i]);
                    pMipSet->m_pMipLevelTable[i] = NULL;
                }
            }

            free(pMipSet->m_pMipLevelTable);
            pMipSet->m_pMipLevelTable = NULL;
            pMipSet->m_nMaxMipLevels  = 0;
            pMipSet->m_nMipLevels     = 0;
        }
    }
}

void CMIPS::PrintError(const char* Format, ... )
{
    char buff[1024];
    // define a pointer to save argument list
    va_list args;
    // process the arguments into our debug buffer
    va_start(args, Format);
    vsprintf(buff,Format,args);
    va_end(args);

    PrintInfo(buff);
}

void CMIPS::Print(const char* Format, ...)
{
    if (!PrintLine) return;

    if (m_infolevel & 0x01)
    {
        char buff[1024];
        // define a pointer to save argument list
        va_list args;
        // process the arguments into our debug buffer
        va_start(args, Format);
        vsprintf(buff, Format, args);
        va_end(args);

        PrintLine(buff);
    }
}

void CMIPS::SetProgress(unsigned int value)
{
    if (SetProgressValue)
    {
        SetProgressValue(value, &m_canceled);
    }
}



