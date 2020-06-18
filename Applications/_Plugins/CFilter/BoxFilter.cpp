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

#include "BoxFilter.h"

// Windows Header Files:
#ifdef _WIN32
#include <windows.h>
#endif

#include <TC_PluginAPI.h>
#include <TC_PluginInternal.h>
#include <Compressonator.h>
#include <Texture.h>

#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include "TextureIO.h"
#endif
#include <algorithm>
CMIPS *CMips;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_BoxFilter)
SET_PLUGIN_TYPE("FILTERS")
SET_PLUGIN_NAME("BOXFILTER")
#else
void *make_Plugin_BoxFilter() { return new Plugin_BoxFilter; } 
#endif

Plugin_BoxFilter::Plugin_BoxFilter()
{ 
}

Plugin_BoxFilter::~Plugin_BoxFilter()
{ 
}

int Plugin_BoxFilter::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        CMips = static_cast<CMIPS *>(Shared);
        return 0;
    }
    return 1;
}


int Plugin_BoxFilter::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{ 
    //MessageBox(0,"TC_PluginGetVersion","Plugin_WIC",MB_OK);  
#ifdef _WIN32
    pPluginVersion->guid                    = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor       = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor       = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

//nMinSize : The size in pixels used to determine how many mip levels to generate. Once all dimensions are less than or equal to nMinSize your mipper should generate no more mip levels.
int Plugin_BoxFilter::TC_GenerateMIPLevels(MipSet *pMipSet, int nMinSize)
{
    assert(pMipSet);
    assert(pMipSet->m_nMipLevels);

    
    int nPrevMipLevels = pMipSet->m_nMipLevels;
    int nWidth = pMipSet->m_nWidth;
    int nHeight = pMipSet->m_nHeight;

    while(nWidth > nMinSize && nHeight > nMinSize)
    {
        nWidth = (std::max)(nWidth >> 1, 1);
        nHeight = (std::max)(nHeight >> 1, 1);
        int nCurMipLevel = pMipSet->m_nMipLevels;
        int maxFacesOrSlices = (std::max)((pMipSet->m_TextureType == TT_VolumeTexture) ? (CMP_MaxFacesOrSlices(pMipSet, nCurMipLevel-1)>>1) : CMP_MaxFacesOrSlices(pMipSet, nCurMipLevel-1), 1);
        for(int nFaceOrSlice=0; nFaceOrSlice<maxFacesOrSlices; nFaceOrSlice++)
        {
            MipLevel* pThisMipLevel = CMips->GetMipLevel(pMipSet, nCurMipLevel, nFaceOrSlice);
            if (!pThisMipLevel) continue;
            assert(CMips->GetMipLevel(pMipSet, nCurMipLevel-1, nFaceOrSlice)->m_pbData);    //prev miplevel ok

            if(pThisMipLevel->m_pbData) // Space for mip level already allocated ?
            {
                if(pThisMipLevel->m_nWidth != nWidth || pThisMipLevel->m_nHeight != nHeight)
                {
                    // Wrong size - release & reallocate
                    //CMips->FreeMipLevelData(pThisMipLevel);
                    if(CMips->AllocateMipLevelData(pThisMipLevel, nWidth, nHeight, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType) == NULL)
                    {
                        return PE_Unknown;
                    }
                }
            }
            else if(CMips->AllocateMipLevelData(pThisMipLevel, nWidth, nHeight, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType) == NULL)
            {
                return PE_Unknown;
            }

            assert(pThisMipLevel->m_pbData);
            if(pMipSet->m_TextureType != TT_VolumeTexture)
            {
                MipLevel *tempMipOne = CMips->GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice);
                if(pMipSet->m_ChannelFormat == CF_8bit)
                    GenerateMipLevel(pThisMipLevel, tempMipOne);
                else if (pMipSet->m_ChannelFormat == CF_Float16)
                    GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_phfsData, tempMipOne->m_phfsData);
                else if(pMipSet->m_ChannelFormat == CF_Float32)
                    GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_pfData, tempMipOne->m_pfData);
            }
            else
            {
                if(CMP_MaxFacesOrSlices(pMipSet, nCurMipLevel-1) > 1)
                {
                    MipLevel *tempMipOne = CMips->GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice * 2);
                    MipLevel *tempMipTwo = CMips->GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice * 2 + 1);
                    //prev miplevel had 2 or more slices, so avg together slices
                    if(pMipSet->m_ChannelFormat == CF_8bit)
                        GenerateMipLevel(pThisMipLevel, tempMipOne, tempMipTwo);
                    else if (pMipSet->m_ChannelFormat == CF_Float16)
                        GenerateMipLevelF(pThisMipLevel, tempMipOne, tempMipTwo, pThisMipLevel->m_phfsData, tempMipOne->m_phfsData, tempMipTwo->m_phfsData);
                    else if(pMipSet->m_ChannelFormat == CF_Float32)
                        GenerateMipLevelF(pThisMipLevel, tempMipOne, tempMipTwo, pThisMipLevel->m_pfData, tempMipOne->m_pfData, tempMipTwo->m_pfData);
                }
                else
                {
                    MipLevel *tempMipOne = CMips->GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice);
                    if(pMipSet->m_ChannelFormat == CF_8bit)
                        GenerateMipLevel(pThisMipLevel, tempMipOne);
                    else if (pMipSet->m_ChannelFormat == CF_Float16)
                        GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_phfsData, tempMipOne->m_phfsData);
                    else if(pMipSet->m_ChannelFormat == CF_Float32)
                        GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_pfData, tempMipOne->m_pfData);
                }
            }
        }

        if (pMipSet->m_nMipLevels < MAX_MIPLEVEL_SUPPORTED)
            ++pMipSet->m_nMipLevels;
        else
            break;
        if (nWidth == 1 || nHeight == 1)
            break;
    }


    // Increment for the one that already existed
    // pMipSet->m_nMipLevels++;

    //free up unused miplevels. shouldn't be any b/c we should have made all possible
    //for(int i=pMipSet->m_nMipLevels; i<nPrevMipLevels; i++)
    //{
    //    for(int j=0; j<MaxFacesOrSlices(pMipSet, i); j++)
    //    {
    //        CMips->FreeMipLevelData(CMips->GetMipLevel(pMipSet, i, j));
    //    }
    //}

    return PE_OK;
}

void GenerateMipLevel(MipLevel* pCurMipLevel, MipLevel* pPrevMipLevelOne, MipLevel* pPrevMipLevelTwo)
{
    assert(pCurMipLevel);
    assert(pPrevMipLevelOne);

    if(pCurMipLevel && pPrevMipLevelOne)
    {
        if(!pPrevMipLevelTwo)
        {
            bool bDiffHeights = pCurMipLevel->m_nHeight != pPrevMipLevelOne->m_nHeight;
            bool bDiffWidths = pCurMipLevel->m_nWidth != pPrevMipLevelOne->m_nWidth;
            CMP_COLOR *pDst = pCurMipLevel->m_pcData;
            for(int y=0; y<pCurMipLevel->m_nHeight; y++)
            {
                CMP_COLOR *pSrc = pPrevMipLevelOne->m_pcData + (2 * y * pPrevMipLevelOne->m_nWidth);
                CMP_COLOR *pSrc2;
                if(bDiffHeights)
                {
                    pSrc2 = pSrc+pPrevMipLevelOne->m_nWidth;
                }
                else
                {
                    //if no change in height, then use same line as source
                    pSrc2 = pSrc;
                }
                for(int x=0; x<pCurMipLevel->m_nWidth; x++, pSrc+=2, pSrc2+=2)
                {
                    CMP_COLOR c1, c2, c3, c4;
                    c1 = *pSrc;
                    c3 = *pSrc2;
                    if(bDiffWidths)
                    {
                        c2 = pSrc[1];
                        c4 = pSrc2[1];
                    }
                    else
                    {
                        c2 = *pSrc;
                        c4 = *pSrc2;
                    }
                    for(int i=0; i<4 ;i++)
                        c1.rgba[i] = static_cast<CMP_BYTE>((c1.rgba[i] + c2.rgba[i] + c3.rgba[i] + c4.rgba[i]+2)/4);
                    *pDst++= c1;
                }
            }
        }
        else
        {//working with volume texture, avg both slices together as well as 4 corners
            bool bDiffHeights = pCurMipLevel->m_nHeight != pPrevMipLevelOne->m_nHeight;
            bool bDiffWidths = pCurMipLevel->m_nWidth != pPrevMipLevelOne->m_nWidth;
            //don't need to check that either height or width is diff, b/c slices are diff
            CMP_COLOR *pDst = pCurMipLevel->m_pcData;
            for(int y=0; y<pCurMipLevel->m_nHeight; y++)
            {
                CMP_COLOR *pSrc, *pSrc2, *pOtherSrc, *pOtherSrc2;
                pSrc = pPrevMipLevelOne->m_pcData + (2 * y * pPrevMipLevelOne->m_nWidth);
                pOtherSrc = pPrevMipLevelTwo->m_pcData + (2 * y * pPrevMipLevelTwo->m_nWidth);
                if(bDiffHeights)
                {
                    //point to next line, same column
                    pSrc2 = pSrc + pPrevMipLevelOne->m_nWidth;
                    pOtherSrc2 = pOtherSrc + pPrevMipLevelTwo->m_nWidth;
                }
                else
                {
                    //if no change in height, then use same line as source
                    pSrc2 = pSrc;
                    pOtherSrc2 = pOtherSrc;
                }
                for(int x=0; x<pCurMipLevel->m_nWidth; x++, pSrc+=2, pSrc2+=2, pOtherSrc+=2, pOtherSrc2+=2)
                {
                    CMP_COLOR c1, c2, c3, c4, c5, c6, c7, c8;
                    c1 = *pSrc;
                    c3 = *pSrc2;
                    c5 = *pOtherSrc;
                    c7 = *pOtherSrc2;
                    if(bDiffWidths)
                    {
                        c2 = pSrc[1];
                        c4 = pSrc2[1];
                        c6 = pOtherSrc[1];
                        c8 = pOtherSrc2[1];
                    }
                    else
                    {
                        c2 = *pSrc;
                        c4 = *pSrc2;
                        c6 = *pOtherSrc;
                        c8 = *pOtherSrc2;
                    }
                    for(int i=0; i<4; i++)
                    {
                        c1.rgba[i] = static_cast<CMP_BYTE>(((int)c1.rgba[i] + c2.rgba[i] + c3.rgba[i] + c4.rgba[i] + c5.rgba[i] + c6.rgba[i] + c7.rgba[i] + c8.rgba[i]+4)/8);
                    }
                    *pDst++= c1;
                }
            }
        }
    }
}

template <typename T>
void GenerateMipLevelF(MipLevel* pCurMipLevel, MipLevel* pPrevMipLevelOne, MipLevel* pPrevMipLevelTwo, T* curMipData, T* prevMip1Data, T* prevMip2Data)
{
    assert(pCurMipLevel);
    assert(pPrevMipLevelOne);

    if (pCurMipLevel && pPrevMipLevelOne)
    {
        if (!pPrevMipLevelTwo)
        {
            bool bDiffHeights = pCurMipLevel->m_nHeight != pPrevMipLevelOne->m_nHeight;
            bool bDiffWidths = pCurMipLevel->m_nWidth != pPrevMipLevelOne->m_nWidth;
            assert(bDiffHeights || bDiffWidths);
            T *pDst = curMipData;
            for (int y = 0; y<pCurMipLevel->m_nHeight; y++)
            {
                T *pSrc = prevMip1Data + (2 * y * pPrevMipLevelOne->m_nWidth * 4);
                T *pSrc2;
                if (bDiffHeights)
                {
                    pSrc2 = pSrc + (pPrevMipLevelOne->m_nWidth * 4);
                }
                else
                {
                    //if no change in height, then use same line as source
                    pSrc2 = pSrc;
                }
                for (int x = 0; x<pCurMipLevel->m_nWidth; x++, pSrc += 8, pSrc2 += 8)
                {
                    T c1[4], c2[4], c3[4], c4[4];
                    memcpy(c1, pSrc, sizeof(c1));
                    memcpy(c3, pSrc2, sizeof(c3));
                    if (bDiffWidths)
                    {
                        memcpy(c2, pSrc + 4, sizeof(c2));
                        memcpy(c4, pSrc2 + 4, sizeof(c4));
                    }
                    else
                    {
                        memcpy(c2, pSrc, sizeof(c2));
                        memcpy(c4, pSrc2, sizeof(c4));
                    }
                    for (int i = 0; i<4; i++)
                        *pDst++ = (c1[i] + c2[i] + c3[i] + c4[i]) / T(4.f);
                }
            }
        }
        else
        {//working with volume texture, avg both slices together as well as 4 corners
            bool bDiffHeights = pCurMipLevel->m_nHeight != pPrevMipLevelOne->m_nHeight;
            bool bDiffWidths = pCurMipLevel->m_nWidth != pPrevMipLevelOne->m_nWidth;
            //don't need to check that either height or width is diff, b/c slices are diff
            T *pDst = curMipData;
            for (int y = 0; y<pCurMipLevel->m_nHeight; y++)
            {
                T *pSrc, *pSrc2, *pOtherSrc, *pOtherSrc2;
                pSrc = prevMip1Data + (2 * y * pPrevMipLevelOne->m_nWidth * 4);
                pOtherSrc = prevMip2Data + (2 * y * pPrevMipLevelTwo->m_nWidth * 4);
                if (bDiffHeights)
                {
                    //point to next line, same column
                    pSrc2 = pSrc + (pPrevMipLevelOne->m_nWidth * 4);
                    pOtherSrc2 = pOtherSrc + (pPrevMipLevelTwo->m_nWidth * 4);
                }
                else
                {
                    //if no change in height, then use same line as source
                    pSrc2 = pSrc;
                    pOtherSrc2 = pOtherSrc;
                }
                for (int x = 0; x<pCurMipLevel->m_nWidth; x++, pSrc += 8, pSrc2 += 8, pOtherSrc += 8, pOtherSrc2 += 8)
                {
                    T c1[4], c2[4], c3[4], c4[4], c5[4], c6[4], c7[4], c8[4];
                    memcpy(c1, pSrc, sizeof(c1));
                    memcpy(c3, pSrc2, sizeof(c3));
                    memcpy(c5, pOtherSrc, sizeof(c5));
                    memcpy(c7, pOtherSrc2, sizeof(c7));
                    if (bDiffWidths)
                    {
                        memcpy(c2, pSrc + 4, sizeof(c2));
                        memcpy(c4, pSrc2 + 4, sizeof(c4));
                        memcpy(c6, pOtherSrc + 4, sizeof(c6));
                        memcpy(c8, pOtherSrc2 + 4, sizeof(c8));
                    }
                    else
                    {
                        memcpy(c2, pSrc, sizeof(c2));
                        memcpy(c4, pSrc2, sizeof(c4));
                        memcpy(c6, pOtherSrc, sizeof(c6));
                        memcpy(c8, pOtherSrc2, sizeof(c8));
                    }
                    for (int i = 0; i<4; i++)
                        *pDst++ = (c1[i] + c2[i] + c3[i] + c4[i] + c5[i] + c6[i] + c7[i] + c8[i]) / T(8.f);
                }
            }
        }
    }
}

