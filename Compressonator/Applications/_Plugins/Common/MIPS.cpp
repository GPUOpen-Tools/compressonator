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
#include <stdio.h>
#include "MIPS.h"

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
        vsprintf_s(buff, Format, args);
        va_end(args);
        PrintStatusLine(buff);
    }
}


MipLevel* CMIPS::GetMipLevel(const MipSet* pMipSet, int nMipLevel)
{
    int nFaceOrSlice = 0;
    return GetMipLevel(pMipSet,nMipLevel,nFaceOrSlice);
}

MipLevel* CMIPS::GetMipLevel(const MipSet* pMipSet, int nMipLevel,    int nFaceOrSlice)
{
    if(!pMipSet)
    {
        ASSERT(pMipSet);
        return NULL;
    }
    if(nMipLevel >= pMipSet->m_nMaxMipLevels)
    {
        ASSERT(nMipLevel < pMipSet->m_nMaxMipLevels);
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
        if(nFaceOrSlice >= nDepth)
        {
            ASSERT(nFaceOrSlice < nDepth);
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
        ASSERT(0);
        return NULL;
    }
}

int CMIPS::GetMaxMipLevels(int nWidth, int nHeight, int nDepth)
{
    int maxMipLevels = 1;
    ASSERT(nWidth > 0 && nHeight > 0 && nDepth > 0);

    while (nWidth > 1 || nHeight > 1 || nDepth > 1)
    {
        maxMipLevels++;
        //div by 2
        nWidth = nWidth>1 ? nWidth>>1 : 1;
        nHeight = nHeight>1 ? nHeight>>1 : 1;
        nDepth = nDepth>1 ? nDepth>>1 : 1;
    }
    return maxMipLevels;
}

bool CMIPS::AllocateMipLevelTable(MipLevelTable** ppMipLevelTable, int nMaxMipLevels, TextureType textureType, int nDepth, int& nLevelsToAllocate)
{
    //TODO test
    ASSERT(nDepth > 0);
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
        if(nDepth > 6)
        {
            return false;
        }
        nLevelsToAllocate = nMaxMipLevels * nDepth;
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
    ASSERT(*ppMipLevelTable);
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
        ASSERT(pMipLevelTable[i]);
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

bool CMIPS::AllocateMipSet(MipSet* pMipSet, ChannelFormat channelFormat, TextureDataType textureDataType, TextureType textureType, int nWidth, int nHeight, int nDepth)
{
    //TODO test
    ASSERT(pMipSet);
    ASSERT(nWidth > 0 && nHeight > 0 && nDepth > 0);
    if(pMipSet->m_pMipLevelTable)
    {
        ASSERT(!pMipSet->m_pMipLevelTable);
        return false;
    }
    //depth only matters for this when its volume texture
    pMipSet->m_nMaxMipLevels = GetMaxMipLevels(nWidth, nHeight, textureType==TT_VolumeTexture ? nDepth : 1);
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
    if(!AllocateMipLevelTable(&pMipSet->m_pMipLevelTable, pMipSet->m_nMaxMipLevels, textureType, nDepth, numLevelsToAllocate))
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

bool CMIPS::AllocateMipLevelData(MipLevel* pMipLevel, int nWidth, int nHeight, ChannelFormat channelFormat, TextureDataType textureDataType)
{
    //TODO test
    ASSERT(pMipLevel);
    ASSERT(nWidth > 0 && nHeight > 0);

    DWORD dwBitsPerPixel;
    switch(channelFormat)
    {
        case CF_8bit:
        case CF_2101010:
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
            ASSERT(0);
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
            ASSERT(0);
            return false;
    }

    DWORD dwPitch = PAD_BYTE(nWidth, dwBitsPerPixel);
    pMipLevel->m_dwLinearSize = dwPitch * nHeight;
    pMipLevel->m_nWidth = nWidth;
    pMipLevel->m_nHeight = nHeight;

    pMipLevel->m_pbData = reinterpret_cast<BYTE*>(malloc(pMipLevel->m_dwLinearSize));

    return (pMipLevel->m_pbData != NULL);
}

bool CMIPS::AllocateCompressedMipLevelData(MipLevel* pMipLevel, int nWidth, int nHeight, DWORD dwSize)
{
    //TODO test
    ASSERT(pMipLevel);
    ASSERT(nWidth > 0 && nHeight > 0);

    pMipLevel->m_dwLinearSize = dwSize;
    pMipLevel->m_nWidth = nWidth;
    pMipLevel->m_nHeight = nHeight;

    pMipLevel->m_pbData = reinterpret_cast<BYTE*>(malloc(pMipLevel->m_dwLinearSize));

    return (pMipLevel->m_pbData != NULL);
}

void CMIPS::FreeMipSet(MipSet* pMipSet)
{
    //TODO test
    int nTotalOldMipLevels = 0;
    ASSERT(pMipSet);
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
                ASSERT(0);
            }
            //free all miplevels and their data
            for(int i=0; i<nTotalOldMipLevels; i++)
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
    vsprintf_s(buff,Format,args);
    va_end(args);

    PrintInfo(buff);
}


