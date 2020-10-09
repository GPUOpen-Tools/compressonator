//=============================================================================
// Copyright (c) 2020  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2008-2020, ATI Technologies Inc. All rights reserved.
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
//==============================================================================
#include "boxfilter.h"

#define BUILD_AS_PLUGIN_DLL

// Windows Header Files:
#ifdef _WIN32
#include <windows.h>
#endif

#include <tc_pluginapi.h>
#include <tc_plugininternal.h>
#include <compressonator.h>
#include <texture.h>

#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include "textureio.h"
#endif
#include <algorithm>

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_BoxFilter)
SET_PLUGIN_TYPE("FILTERS")
SET_PLUGIN_NAME("MIPMAP")
#else
void* make_Plugin_BoxFilter() {
    return new Plugin_BoxFilter;
}
#endif

CD3DXModule g_D3DX;
CD3D9Module g_D3D9;
CMIPS       *CFilterMips = NULL;

void CMP_SetMipLevelGammaLinearB(MipLevel* pCurMipLevel, CMP_BYTE* pdata, CMP_FLOAT Gamma, CMP_INT numchannels) {
    for (int y = 0; y < pCurMipLevel->m_nHeight; y++) {
        for (int x = 0; x < pCurMipLevel->m_nWidth; x++) {
            // calc Gamma for the all color channels
            for (int i = 0; i < 3 && i < numchannels; i++) {
                CMP_FLOAT normpixel = 0.0f;
                if (*pdata > 0) {
                    normpixel = *pdata;
                    normpixel = normpixel / 255.0f;
                    normpixel = powf(normpixel, Gamma) * 255.0f;

                    // need to check for signed components
                    if (normpixel > 255)
                        normpixel = 255;
                    else if (normpixel < 0)
                        normpixel = 0;

                    *pdata = (CMP_BYTE) round(normpixel);
                }
                pdata++;
            }
            // if alpha skip it
            if (numchannels > 3)
                *pdata++;
        }
    }
}

template <typename T>
void CMP_SetMipLevelGammaf(MipLevel* pCurMipLevel, T* pdata, CMP_FLOAT Gamma, CMP_INT numchannels) {
    for (int y = 0; y < pCurMipLevel->m_nHeight; y++) {
        for (int x = 0; x < pCurMipLevel->m_nWidth; x++) {
            // calc Gamma for the all color channels
            for (int i = 0; i < 3 && i < numchannels; i++)
                *pdata++ = pow(*pdata, Gamma);
            // if alpha skip it
            if (numchannels > 3)
                *pdata++;
        }
    }
}


void CMP_SetMipSetGamma(MipSet* pMipSet, CMP_FLOAT Gamma) {
    CMIPS     CMips;
    MipLevel* pCurMipLevel;
    CMP_INT   maxFaceOrSlice;
    if (pMipSet->m_TextureType & TT_CubeMap)
        maxFaceOrSlice = 6;
    else
        maxFaceOrSlice = 1;
    for (CMP_INT nCurMipLevel = 0; nCurMipLevel < pMipSet->m_nMipLevels; nCurMipLevel++) {
        for (CMP_INT nFaceOrSlice = 0; nFaceOrSlice < maxFaceOrSlice; nFaceOrSlice++) {
            pCurMipLevel = CMips.GetMipLevel(pMipSet, nCurMipLevel, nFaceOrSlice);
            if (pMipSet->m_ChannelFormat == CF_8bit)
                CMP_SetMipLevelGammaLinearB(pCurMipLevel, pCurMipLevel->m_pbData, Gamma, 4);
            else if (pMipSet->m_ChannelFormat == CF_Float16)
                CMP_SetMipLevelGammaf(pCurMipLevel, pCurMipLevel->m_phfsData, Gamma, 4);
            else if (pMipSet->m_ChannelFormat == CF_Float32)
                CMP_SetMipLevelGammaf(pCurMipLevel, pCurMipLevel->m_pfData, Gamma, 4);
        }
    }
}




template <typename T>
void GenerateMipLevelF(MipLevel* pCurMipLevel, MipLevel* pPrevMipLevelOne, MipLevel* pPrevMipLevelTwo, T* curMipData, T* prevMip1Data, T* prevMip2Data) {
    assert(pCurMipLevel);
    assert(pPrevMipLevelOne);

    if (pCurMipLevel && pPrevMipLevelOne) {
        if (!pPrevMipLevelTwo) {
            bool bDiffHeights = pCurMipLevel->m_nHeight != pPrevMipLevelOne->m_nHeight;
            bool bDiffWidths  = pCurMipLevel->m_nWidth != pPrevMipLevelOne->m_nWidth;
            assert(bDiffHeights || bDiffWidths);
            T* pDst = curMipData;
            for (int y = 0; y < pCurMipLevel->m_nHeight; y++) {
                T* pSrc = prevMip1Data + (2 * y * pPrevMipLevelOne->m_nWidth * 4);
                T* pSrc2;
                if (bDiffHeights) {
                    pSrc2 = pSrc + (pPrevMipLevelOne->m_nWidth * 4);
                } else {
                    //if no change in height, then use same line as source
                    pSrc2 = pSrc;
                }
                for (int x = 0; x < pCurMipLevel->m_nWidth; x++, pSrc += 8, pSrc2 += 8) {
                    T c1[4], c2[4], c3[4], c4[4];
                    memcpy(c1, pSrc, sizeof(c1));
                    memcpy(c3, pSrc2, sizeof(c3));
                    if (bDiffWidths) {
                        memcpy(c2, pSrc + 4, sizeof(c2));
                        memcpy(c4, pSrc2 + 4, sizeof(c4));
                    } else {
                        memcpy(c2, pSrc, sizeof(c2));
                        memcpy(c4, pSrc2, sizeof(c4));
                    }
                    for (int i = 0; i < 4; i++)
                        *pDst++ = (c1[i] + c2[i] + c3[i] + c4[i]) / T(4.f);
                }
            }
        } else {
            //working with volume texture, avg both slices together as well as 4 corners
            bool bDiffHeights = pCurMipLevel->m_nHeight != pPrevMipLevelOne->m_nHeight;
            bool bDiffWidths  = pCurMipLevel->m_nWidth != pPrevMipLevelOne->m_nWidth;
            //don't need to check that either height or width is diff, b/c slices are diff
            T* pDst = curMipData;
            for (int y = 0; y < pCurMipLevel->m_nHeight; y++) {
                T *pSrc, *pSrc2, *pOtherSrc, *pOtherSrc2;
                pSrc      = prevMip1Data + (2 * y * pPrevMipLevelOne->m_nWidth * 4);
                pOtherSrc = prevMip2Data + (2 * y * pPrevMipLevelTwo->m_nWidth * 4);
                if (bDiffHeights) {
                    //point to next line, same column
                    pSrc2      = pSrc + (pPrevMipLevelOne->m_nWidth * 4);
                    pOtherSrc2 = pOtherSrc + (pPrevMipLevelTwo->m_nWidth * 4);
                } else {
                    //if no change in height, then use same line as source
                    pSrc2      = pSrc;
                    pOtherSrc2 = pOtherSrc;
                }
                for (int x = 0; x < pCurMipLevel->m_nWidth; x++, pSrc += 8, pSrc2 += 8, pOtherSrc += 8, pOtherSrc2 += 8) {
                    T c1[4], c2[4], c3[4], c4[4], c5[4], c6[4], c7[4], c8[4];
                    memcpy(c1, pSrc, sizeof(c1));
                    memcpy(c3, pSrc2, sizeof(c3));
                    memcpy(c5, pOtherSrc, sizeof(c5));
                    memcpy(c7, pOtherSrc2, sizeof(c7));
                    if (bDiffWidths) {
                        memcpy(c2, pSrc + 4, sizeof(c2));
                        memcpy(c4, pSrc2 + 4, sizeof(c4));
                        memcpy(c6, pOtherSrc + 4, sizeof(c6));
                        memcpy(c8, pOtherSrc2 + 4, sizeof(c8));
                    } else {
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


Plugin_BoxFilter::Plugin_BoxFilter() {}

Plugin_BoxFilter::~Plugin_BoxFilter() {
    CFilterMips = NULL;
}

// Not used return error!
int Plugin_BoxFilter::TC_PluginSetSharedIO(void* SharedCMips) {
    // check if already initialized
    if (CFilterMips != NULL)
        return CMP_OK;

    if (SharedCMips && CFilterMips == NULL) {
        CFilterMips = reinterpret_cast<CMIPS*>(SharedCMips);

#ifdef _WIN32
        std::string strD3DX = _T("");
        if (LoadD3DX(g_D3DX, strD3DX) == LOAD_FAILED) {
            Error(_T("D3DXFilter Plugin"), EL_Error, CMP_ERR_UNABLE_TO_LOAD_FILE);
            return CMP_ERR_UNABLE_TO_LOAD_FILE;
        }

        // Ensure DX9 is present & can be loaded
        g_D3D9.LoadModule();
        IDirect3D9* pD3D = g_D3D9.Direct3DCreate9(D3D_SDK_VERSION);
        if (pD3D == NULL) {
            Error(_T("D3DXFilter Plugin"), EL_Error, CMP_ERR_UNABLE_TO_LOAD_FILE);
            return CMP_ERR_UNABLE_TO_LOAD_FILE;
        } else
            pD3D->Release();
#endif

        return CMP_OK;
    } else
        CFilterMips = NULL;

    return CMP_ERR_GENERIC;
}

int Plugin_BoxFilter::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) {
    //MessageBox(0,"TC_PluginGetVersion","Plugin_WIC",MB_OK);
#ifdef _WIN32
    pPluginVersion->guid = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor    = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor    = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

//nMinSize : The size in pixels used to determine how many mip levels to generate. Once all dimensions are less than or equal to nMinSize your mipper should generate no more mip levels.
int Plugin_BoxFilter::TC_CFilter(MipSet* pMipSet, CMP_MipSet* pMipSetDst, CMP_CFilterParams* pCFilterParams)
{
    (pMipSetDst);
    assert(pMipSet);
    assert(pMipSet->m_nMipLevels);
    int result = CMP_OK;
    if (!CFilterMips)
        return CMP_ERR_PLUGIN_SHAREDIO_NOT_SET;

    if (pCFilterParams->fGammaCorrection < 0)
        return CMP_ERR_GAMMA_OUTOFRANGE;

    if (pCFilterParams->nFilterType == 0) {
        int nPrevMipLevels = pMipSet->m_nMipLevels;
        int nWidth         = pMipSet->m_nWidth;
        int nHeight        = pMipSet->m_nHeight;

        while (nWidth > pCFilterParams->nMinSize && nHeight > pCFilterParams->nMinSize) {
            nWidth               = (std::max)(nWidth >> 1, 1);
            nHeight              = (std::max)(nHeight >> 1, 1);
            int nCurMipLevel     = pMipSet->m_nMipLevels;
            int maxFacesOrSlices = (std::max)((pMipSet->m_TextureType == TT_VolumeTexture) ? (CMP_MaxFacesOrSlices(pMipSet, nCurMipLevel - 1) >> 1)
                                              : CMP_MaxFacesOrSlices(pMipSet, nCurMipLevel - 1),1);
            for (int nFaceOrSlice = 0; nFaceOrSlice < maxFacesOrSlices; nFaceOrSlice++) {
                MipLevel* pThisMipLevel = CFilterMips->GetMipLevel(pMipSet, nCurMipLevel, nFaceOrSlice);
                if (!pThisMipLevel)
                    continue;
                assert(CFilterMips->GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice)->m_pbData);  //prev miplevel ok

                if (pThisMipLevel->m_pbData) { // Space for mip level already allocated ?
                    if (pThisMipLevel->m_nWidth != nWidth || pThisMipLevel->m_nHeight != nHeight) {
                        // Wrong size - release & reallocate
                        //CFilterMips->FreeMipLevelData(pThisMipLevel);
                        if (CFilterMips->AllocateMipLevelData(pThisMipLevel, nWidth, nHeight, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType) == NULL) {
                            return PE_Unknown;
                        }
                    }
                } else if (CFilterMips->AllocateMipLevelData(pThisMipLevel, nWidth, nHeight, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType) == NULL) {
                    return PE_Unknown;
                }

                assert(pThisMipLevel->m_pbData);
                if (pMipSet->m_TextureType != TT_VolumeTexture) {
                    MipLevel* tempMipOne = CFilterMips->GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice);
                    if (pMipSet->m_ChannelFormat == CF_8bit)
                        GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_pbData, tempMipOne->m_pbData);
                    else if (pMipSet->m_ChannelFormat == CF_Float16)
                        GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_phfsData, tempMipOne->m_phfsData);
                    else if (pMipSet->m_ChannelFormat == CF_Float32)
                        GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_pfData, tempMipOne->m_pfData);
                } else {
                    if (CMP_MaxFacesOrSlices(pMipSet, nCurMipLevel - 1) > 1) {
                        MipLevel* tempMipOne = CFilterMips->GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice * 2);
                        MipLevel* tempMipTwo = CFilterMips->GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice * 2 + 1);
                        //prev miplevel had 2 or more slices, so avg together slices
                        if (pMipSet->m_ChannelFormat == CF_8bit)
                            GenerateMipLevelF(pThisMipLevel, tempMipOne, tempMipTwo, pThisMipLevel->m_pbData, tempMipOne->m_pbData, tempMipTwo->m_pbData);
                        else if (pMipSet->m_ChannelFormat == CF_Float16)
                            GenerateMipLevelF(pThisMipLevel, tempMipOne, tempMipTwo, pThisMipLevel->m_phfsData, tempMipOne->m_phfsData, tempMipTwo->m_phfsData);
                        else if (pMipSet->m_ChannelFormat == CF_Float32)
                            GenerateMipLevelF(pThisMipLevel, tempMipOne, tempMipTwo, pThisMipLevel->m_pfData, tempMipOne->m_pfData, tempMipTwo->m_pfData);
                    } else {
                        MipLevel* tempMipOne = CFilterMips->GetMipLevel(pMipSet, nCurMipLevel - 1, nFaceOrSlice);
                        if (pMipSet->m_ChannelFormat == CF_8bit)
                            GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_pbData, tempMipOne->m_pbData);
                        else if (pMipSet->m_ChannelFormat == CF_Float16)
                            GenerateMipLevelF(pThisMipLevel, tempMipOne, NULL, pThisMipLevel->m_phfsData, tempMipOne->m_phfsData);
                        else if (pMipSet->m_ChannelFormat == CF_Float32)
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
    } else {
#ifdef _WIN32
        result = GenMipLevelsUsingD3DXFilter(pMipSet, pCFilterParams);
#else
        result = CMP_ERR_UNSUPPORTED_DEST_FORMAT;
#endif
    }

    if (pCFilterParams->fGammaCorrection != 1.0f)
        CMP_SetMipSetGamma(pMipSet, pCFilterParams->fGammaCorrection);

    return result;
}

//--------------------------------------------------------------------------------------------
// DirectX MipMap Filter
//--------------------------------------------------------------------------------------------
void Plugin_BoxFilter::Error(TCHAR* pszCaption, TC_ErrorLevel errorLevel, UINT nErrorString) {
    // Add code to print message to caller
}

#ifdef _WIN32

#define HANDLE_ERROR(err)                                                                    \
    {                                                                                        \
        Error(_T("D3DXFilter Plugin"), EL_Error, err); \
        goto Cleanup;                                                                        \
    }

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int Plugin_BoxFilter::GenMipLevelsUsingD3DXFilter(MipSet* pMipSet, CMP_CFilterParams* pD3DXFilterParams) {
    if (pMipSet->m_ChannelFormat != CF_8bit)
        return PE_Unknown;


    DWORD dwFlags = pD3DXFilterParams ? pD3DXFilterParams->dwMipFilterOptions : D3DX_FILTER_TRIANGLE;

    CMP_ERROR    retVal   = CMP_ERR_GENERIC;        // Mapping to DX11 updates todo
    IDirect3D9*        pD3D     = NULL;             // IDXGIFactory2 DXGIAdapter2 IDXGIDevice3
    IDirect3DDevice9*  pDevice  = NULL;             // ID3D11Device2 ID3D11DeviceContext2
    IDirect3DTexture9* pTexture = NULL;             // ID3D11Texture2D
    IDirect3DSurface9* pSurface = NULL;             //
    HWND               hWnd     = NULL;             //

    HMODULE hInstance = GetModuleHandle(NULL);

    TCHAR    szWindowClass[] = _T("D3DXFilter Window");
    WNDCLASS wndClass        = {0, WndProc, 0, 0, hInstance, NULL, NULL, (HBRUSH)COLOR_BACKGROUND, NULL, szWindowClass};
    RegisterClass(&wndClass);

    // Set the window's initial style
    hWnd = CreateWindow(szWindowClass, szWindowClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, NULL, NULL, hInstance, NULL);
    if (hWnd == NULL)
        HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);

    pD3D = g_D3D9.Direct3DCreate9(D3D_SDK_VERSION);
    if (pD3D == NULL)
        HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);

    D3DPRESENT_PARAMETERS presentParams;
    presentParams.BackBufferWidth            = 100;
    presentParams.BackBufferHeight           = 100;
    presentParams.BackBufferCount            = 0;
    presentParams.BackBufferFormat           = D3DFMT_UNKNOWN;
    presentParams.MultiSampleType            = D3DMULTISAMPLE_NONE;
    presentParams.MultiSampleQuality         = 0;
    presentParams.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
    presentParams.hDeviceWindow              = NULL;
    presentParams.Windowed                   = TRUE;
    presentParams.EnableAutoDepthStencil     = FALSE;
    presentParams.AutoDepthStencilFormat     = D3DFMT_UNKNOWN;
    presentParams.Flags                      = 0;
    presentParams.FullScreen_RefreshRateInHz = 0;
    presentParams.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;

    HRESULT hr = 0;
    hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING, &presentParams, &pDevice);
    if (FAILED(hr))
        HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);

    DWORD dwMipLevels = 1;
    int   nWidth      = pMipSet->m_nWidth;
    int   nHeight     = pMipSet->m_nHeight;

    while (nWidth > pD3DXFilterParams->nMinSize || nHeight > pD3DXFilterParams->nMinSize) {
        nWidth  = max(nWidth >> 1, 1);
        nHeight = max(nHeight >> 1, 1);
        dwMipLevels++;
    }

    pDevice->CreateTexture(pMipSet->m_nWidth, pMipSet->m_nHeight, dwMipLevels, 0, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pTexture, NULL);
    if (FAILED(hr))
        HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);

    D3DLOCKED_RECT rect;
    hr = pTexture->LockRect(0, &rect, NULL, 0);
    if (FAILED(hr))
        HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);

    memcpy(rect.pBits, CFilterMips->GetMipLevel(pMipSet, 0)->m_pbData, CFilterMips->GetMipLevel(pMipSet, 0)->m_dwLinearSize);

    hr = pTexture->UnlockRect(0);
    if (FAILED(hr))
        HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);

    hr = g_D3DX.D3DXFilterTexture(pTexture, NULL, D3DX_DEFAULT, dwFlags);
    if (FAILED(hr))
        HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);

    DWORD dwLevels = pTexture->GetLevelCount();
    ASSERT(dwLevels == dwMipLevels);

    DWORD dwLevel;
    for (dwLevel = 1; dwLevel < dwLevels; dwLevel++) {
        hr = pTexture->GetSurfaceLevel(dwLevel, &pSurface);
        if (FAILED(hr))
            HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);

        D3DSURFACE_DESC desc;
        hr = pSurface->GetDesc(&desc);
        if (FAILED(hr))
            HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);

        if (CFilterMips->GetMipLevel(pMipSet, dwLevel)->m_pbData) { // Space for mip level already allocated ?
            if (CFilterMips->GetMipLevel(pMipSet, dwLevel)->m_nWidth != (int)desc.Width ||
                    CFilterMips->GetMipLevel(pMipSet, dwLevel)->m_nHeight != (int)desc.Height) {
                // Wrong size - release & reallocate
                CFilterMips->FreeMipLevelData(CFilterMips->GetMipLevel(pMipSet, dwLevel));
                if (CFilterMips->AllocateMipLevelData(
                            CFilterMips->GetMipLevel(pMipSet, dwLevel), desc.Width, desc.Height, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType) == NULL) {
                    HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);
                }
            }
        } else if (CFilterMips->AllocateMipLevelData(
                       CFilterMips->GetMipLevel(pMipSet, dwLevel), desc.Width, desc.Height, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType) == NULL) {
            HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);
        }

        hr = pSurface->LockRect(&rect, NULL, 0);
        if (FAILED(hr))
            HANDLE_ERROR(CMP_ERR_UNABLE_TO_INIT_D3DX);

        memcpy(CFilterMips->GetMipLevel(pMipSet, dwLevel)->m_pbData, rect.pBits, CFilterMips->GetMipLevel(pMipSet, dwLevel)->m_dwLinearSize);

        pSurface->UnlockRect();
        pSurface->Release();
        pSurface = NULL;
    }

    retVal = CMP_OK;

    // Increment nMipLevel to point to next (empty) miplevel
    while (++dwLevel < dwMipLevels)
        CFilterMips->FreeMipLevelData(CFilterMips->GetMipLevel(pMipSet, dwLevel));

    pMipSet->m_nMipLevels = dwMipLevels;

Cleanup:
    if (pSurface) {
        pSurface->Release();
        pSurface = NULL;
    }
    if (pTexture) {
        pTexture->Release();
        pTexture = NULL;
    }
    if (pDevice) {
        pDevice->Release();
        pDevice = NULL;
    }
    if (pD3D) {
        pD3D->Release();
        pD3D = NULL;
    }
    if (hWnd) {
        DestroyWindow(hWnd);
        hWnd = NULL;
    }


    return retVal;
}
#endif
