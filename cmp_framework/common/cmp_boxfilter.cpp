//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2020 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#include <stdio.h>
#include "cmp_mips.h"
#include "cmp_boxfilter.h"

template <typename T>
void CMP_GenerateMipLevelF(CMP_MipLevel* pCurMipLevel,
                           CMP_MipLevel* pPrevMipLevelOne,
                           CMP_MipLevel* pPrevMipLevelTwo,
                           T*            curMipData,
                           T*            prevMip1Data,
                           T*            prevMip2Data)
{
    assert(pCurMipLevel);
    assert(pPrevMipLevelOne);

    if (pCurMipLevel && pPrevMipLevelOne)
    {
        if (!pPrevMipLevelTwo)
        {
            bool bDiffHeights = pCurMipLevel->m_nHeight != pPrevMipLevelOne->m_nHeight;
            bool bDiffWidths  = pCurMipLevel->m_nWidth != pPrevMipLevelOne->m_nWidth;
            assert(bDiffHeights || bDiffWidths);
            T* pDst = curMipData;
            for (int y = 0; y < pCurMipLevel->m_nHeight; y++)
            {
                T* pSrc = prevMip1Data + (2 * y * pPrevMipLevelOne->m_nWidth * 4);
                T* pSrc2;
                if (bDiffHeights)
                {
                    pSrc2 = pSrc + (pPrevMipLevelOne->m_nWidth * 4);
                }
                else
                {
                    //if no change in height, then use same line as source
                    pSrc2 = pSrc;
                }
                for (int x = 0; x < pCurMipLevel->m_nWidth; x++, pSrc += 8, pSrc2 += 8)
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
                    for (int i = 0; i < 4; i++)
                        *pDst++ = (c1[i] + c2[i] + c3[i] + c4[i]) / T(4.f);
                }
            }
        }
        else
        {
            //working with volume texture, avg both slices together as well as 4 corners
            bool bDiffHeights = pCurMipLevel->m_nHeight != pPrevMipLevelOne->m_nHeight;
            bool bDiffWidths  = pCurMipLevel->m_nWidth != pPrevMipLevelOne->m_nWidth;
            //don't need to check that either height or width is diff, b/c slices are diff
            T* pDst = curMipData;
            for (int y = 0; y < pCurMipLevel->m_nHeight; y++)
            {
                T *pSrc, *pSrc2, *pOtherSrc, *pOtherSrc2;
                pSrc      = prevMip1Data + (2 * y * pPrevMipLevelOne->m_nWidth * 4);
                pOtherSrc = prevMip2Data + (2 * y * pPrevMipLevelTwo->m_nWidth * 4);
                if (bDiffHeights)
                {
                    //point to next line, same column
                    pSrc2      = pSrc + (pPrevMipLevelOne->m_nWidth * 4);
                    pOtherSrc2 = pOtherSrc + (pPrevMipLevelTwo->m_nWidth * 4);
                }
                else
                {
                    //if no change in height, then use same line as source
                    pSrc2      = pSrc;
                    pOtherSrc2 = pOtherSrc;
                }
                for (int x = 0; x < pCurMipLevel->m_nWidth; x++, pSrc += 8, pSrc2 += 8, pOtherSrc += 8, pOtherSrc2 += 8)
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
                    for (int i = 0; i < 4; i++)
                        *pDst++ = (c1[i] + c2[i] + c3[i] + c4[i] + c5[i] + c6[i] + c7[i] + c8[i]) / T(8.f);
                }
            }
        }
    }
}

//nMinSize : The size in pixels used to determine how many mip levels to generate. Once all dimensions are less than or equal to nMinSize your mipper should generate no more mip levels.
CMP_INT CMP_API CMP_GenerateMIPLevelsEx(CMP_MipSet* pMipSet, CMP_CFilterParams* CFilterParam)
{
    CMP_CMIPS CMips;
    assert(pMipSet);
    assert(pMipSet->m_nMipLevels);

    CMP_INT        nWidth          = pMipSet->m_nWidth;
    CMP_INT        nHeight         = pMipSet->m_nHeight;
    CMP_HALFSHORT* null_half       = 0;
    CMP_FLOAT*     null_float      = 0;
    CMP_MipLevel*  null_tempMipTwo = nullptr;

    while (nWidth >= CFilterParam->nMinSize && nHeight >= CFilterParam->nMinSize)
    {
        nWidth                   = CMP_MAX(nWidth >> 1, 1);
        nHeight                  = CMP_MAX(nHeight >> 1, 1);
        CMP_INT nCurMipLevel     = pMipSet->m_nMipLevels;
        CMP_INT maxFacesOrSlices = CMP_MAX((pMipSet->m_TextureType == TT_VolumeTexture) ? (CMP_MaxFacesOrSlices(pMipSet, nCurMipLevel - 1) >> 1)
                                                                                        : CMP_MaxFacesOrSlices(pMipSet, nCurMipLevel - 1),
                                           1);
        for (CMP_INT nFaceOrSlice = 0; nFaceOrSlice < maxFacesOrSlices; nFaceOrSlice++)
        {
            CMP_MipLevel* pThisMipLevel = CMips.GetMipLevel(pMipSet, nCurMipLevel, nFaceOrSlice);
            if (!pThisMipLevel)
                continue;
            assert(CMips.GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice)->m_pbData);  //prev miplevel ok

            if (pThisMipLevel->m_pbData)
            {  // Space for mip level already allocated ?
                if (pThisMipLevel->m_nWidth != nWidth || pThisMipLevel->m_nHeight != nHeight)
                {
                    // Wrong size - release & reallocate
                    //CMips->FreeMipLevelData(pThisMipLevel);
                    if (CMips.AllocateMipLevelData(pThisMipLevel, nWidth, nHeight, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType) == NULL)
                    {
                        return CMP_ERR_GENERIC;
                    }
                }
            }
            else if (CMips.AllocateMipLevelData(pThisMipLevel, nWidth, nHeight, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType) == NULL)
            {
                return CMP_ERR_GENERIC;
            }

            assert(pThisMipLevel->m_pbData);
            if (pMipSet->m_TextureType != TT_VolumeTexture)
            {
                CMP_MipLevel* tempMipOne = CMips.GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice);
                if (pMipSet->m_ChannelFormat == CF_8bit)
                {
                    if (pMipSet->m_format == CMP_FORMAT_RGBA_8888_S)
                        CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_psbData, tempMipOne->m_psbData);
                    else
                        CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_pbData, tempMipOne->m_pbData);
                }
                else if (pMipSet->m_ChannelFormat == CF_Float16)
                    CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, null_tempMipTwo, pThisMipLevel->m_phfsData, tempMipOne->m_phfsData, null_half);
                else if (pMipSet->m_ChannelFormat == CF_Float32)
                    CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, null_tempMipTwo, pThisMipLevel->m_pfData, tempMipOne->m_pfData, null_float);
            }
            else
            {
                if (CMP_MaxFacesOrSlices(pMipSet, nCurMipLevel - 1) > 1)
                {
                    CMP_MipLevel* tempMipOne = CMips.GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice * 2);
                    CMP_MipLevel* tempMipTwo = CMips.GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice * 2 + 1);
                    //prev miplevel had 2 or more slices, so avg together slices
                    if (pMipSet->m_ChannelFormat == CF_8bit)
                    {
                        if (pMipSet->m_format == CMP_FORMAT_RGBA_8888_S)
                            CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, tempMipTwo, pThisMipLevel->m_psbData, tempMipOne->m_psbData, tempMipTwo->m_psbData);
                        else
                            CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, tempMipTwo, pThisMipLevel->m_pbData, tempMipOne->m_pbData, tempMipTwo->m_pbData);
                    }
                    else if (pMipSet->m_ChannelFormat == CF_Float16)
                        CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, tempMipTwo, pThisMipLevel->m_phfsData, tempMipOne->m_phfsData, tempMipTwo->m_phfsData);
                    else if (pMipSet->m_ChannelFormat == CF_Float32)
                        CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, tempMipTwo, pThisMipLevel->m_pfData, tempMipOne->m_pfData, tempMipTwo->m_pfData);
                }
                else
                {
                    CMP_MipLevel* tempMipOne = CMips.GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice);
                    if (pMipSet->m_ChannelFormat == CF_8bit)
                    {
                        if (pMipSet->m_format == CMP_FORMAT_RGBA_8888_S)
                            CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_psbData, tempMipOne->m_psbData);
                        else
                            CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_pbData, tempMipOne->m_pbData);
                    }
                    else if (pMipSet->m_ChannelFormat == CF_Float16)
                        CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, null_tempMipTwo, pThisMipLevel->m_phfsData, tempMipOne->m_phfsData, null_half);
                    else if (pMipSet->m_ChannelFormat == CF_Float32)
                        CMP_GenerateMipLevelF(pThisMipLevel, tempMipOne, null_tempMipTwo, pThisMipLevel->m_pfData, tempMipOne->m_pfData, null_float);
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

    return CMP_OK;
}

CMP_INT CMP_API CMP_GenerateMIPLevels(CMP_MipSet* pMipSet, CMP_INT nMinSize)
{
    CMP_CFilterParams CFilterParam;
    CFilterParam.dwMipFilterOptions = 0;
    CFilterParam.nFilterType        = 0;
    CFilterParam.nMinSize           = nMinSize;
    CFilterParam.fGammaCorrection   = 1.0;
    return CMP_GenerateMIPLevelsEx(pMipSet, &CFilterParam);
}
