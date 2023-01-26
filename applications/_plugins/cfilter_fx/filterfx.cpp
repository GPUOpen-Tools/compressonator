//=============================================================================
// Copyright (c) 2021  Advanced Micro Devices, Inc. All rights reserved.
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
#include "filterfx.h"

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
DECLARE_PLUGIN(Plugin_CFilterFx)
SET_PLUGIN_TYPE("FILTERS")
SET_PLUGIN_NAME("EFFECTS")
#else
void* make_Plugin_CFilterFx()
{
    return new Plugin_CFilterFx;
}
#endif

CMIPS       *CFilterMipsFx = NULL;

Plugin_CFilterFx::Plugin_CFilterFx()
{
#ifdef _WIN32
    m_GpuResources = std::make_unique<GpuResources>();
#endif
}

Plugin_CFilterFx::~Plugin_CFilterFx()
{
    CFilterMipsFx = NULL;
}

// Not used return error!
int Plugin_CFilterFx::TC_PluginSetSharedIO(void* SharedCMips)
{
    // check if already initialized
    if (CFilterMipsFx != NULL)
        return CMP_OK;

    if (SharedCMips && CFilterMipsFx == NULL)
    {
        CFilterMipsFx = reinterpret_cast<CMIPS*>(SharedCMips);
        return CMP_OK;
    } else
        CFilterMipsFx = NULL;

    return CMP_ERR_GENERIC;
}

int Plugin_CFilterFx::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{
#ifdef _WIN32
    pPluginVersion->guid = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor    = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor    = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor = TC_PLUGIN_VERSION_MINOR;
    return 0;
}


//--------------------------------------------------------------------------------------------
// DirectX Filter
//--------------------------------------------------------------------------------------------
void Plugin_CFilterFx::Error(TCHAR* pszCaption, TC_ErrorLevel errorLevel, UINT nErrorString)
{
    // Add code to print message to caller
}

int Plugin_CFilterFx::TC_CFilter(CMP_MipSet* srcMipSet, CMP_MipSet* dstMipSet, CMP_CFilterParams *pCFilterParams)
{
    int result = CMP_OK;

#ifdef _WIN32
    assert(m_GpuResources);
    if (!initialized)
    {
        m_GpuResources->GpuCompileShaders(srcMipSet->m_format, pCFilterParams->useSRGB);
        initialized = true;
    }

    ID3D11Device* const        dev           = m_GpuResources->m_D3D11Device;
    ID3D11DeviceContext* const deviceContext = m_GpuResources->m_DeviceContext;

    uint32_t    rgbaBytesPerPixel;
    DXGI_FORMAT d3dTextureFormat;

    switch (srcMipSet->m_format)
    {
    case CMP_FORMAT_ABGR_16F: // fix this to a proper DXGI mapping
    case CMP_FORMAT_RGBA_16F:
        d3dTextureFormat  = DXGI_FORMAT_R16G16B16A16_FLOAT;
        rgbaBytesPerPixel = 8;
        break;
    default:
        d3dTextureFormat  = DXGI_FORMAT_R8G8B8A8_UNORM;
        rgbaBytesPerPixel = 4;
        break;
    }

    D3D11_SUBRESOURCE_DATA   srcTextureSubresData = {srcMipSet->pData, srcMipSet->m_nWidth * rgbaBytesPerPixel};
    CComPtr<ID3D11Texture2D> srcTexture;
    CD3D11_TEXTURE2D_DESC    srcTextureDesc =
        CD3D11_TEXTURE2D_DESC(d3dTextureFormat, srcMipSet->m_nWidth, srcMipSet->m_nHeight, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);
    CHECK_HR(dev->CreateTexture2D(&srcTextureDesc, &srcTextureSubresData, &srcTexture));

    CD3D11_TEXTURE2D_DESC dstTextureDesc =
        CD3D11_TEXTURE2D_DESC(d3dTextureFormat, dstMipSet->m_nWidth, dstMipSet->m_nHeight, 1, 1, D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT);
    CComPtr<ID3D11Texture2D> dstTexture;
    CHECK_HR(dev->CreateTexture2D(&dstTextureDesc, nullptr, &dstTexture));

    CD3D11_SHADER_RESOURCE_VIEW_DESC  srcTextureViewDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(srcTexture, D3D11_SRV_DIMENSION_TEXTURE2D);
    CComPtr<ID3D11ShaderResourceView> srcTextureView;
    CHECK_HR(dev->CreateShaderResourceView(srcTexture, &srcTextureViewDesc, &srcTextureView));

    CD3D11_UNORDERED_ACCESS_VIEW_DESC  dstTextureViewDesc = CD3D11_UNORDERED_ACCESS_VIEW_DESC(dstTexture, D3D11_UAV_DIMENSION_TEXTURE2D);
    CComPtr<ID3D11UnorderedAccessView> dstTextureView;
    CHECK_HR(dev->CreateUnorderedAccessView(dstTexture, &dstTextureViewDesc, &dstTextureView));

    uvec2 srcSize = uvec2(srcMipSet->m_nWidth, srcMipSet->m_nHeight);
    uvec2 dstSize = uvec2(dstMipSet->m_nWidth, dstMipSet->m_nHeight);
    m_GpuResources->CAS(pCFilterParams->fSharpness, dstTextureView, dstSize, srcTextureView, srcSize);

    srcTextureView.Release();
    srcTexture.Release();

    CD3D11_TEXTURE2D_DESC dstStagingTextureDesc =
        CD3D11_TEXTURE2D_DESC(d3dTextureFormat, dstMipSet->m_nWidth, dstMipSet->m_nHeight, 1, 1, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ);
    CComPtr<ID3D11Texture2D> dstStagingTexture;
    CHECK_HR(dev->CreateTexture2D(&dstStagingTextureDesc, nullptr, &dstStagingTexture));
    deviceContext->CopyResource(dstStagingTexture, dstTexture);

    dstTextureView.Release();
    dstTexture.Release();

    // get the processed data to save
    D3D11_MAPPED_SUBRESOURCE mappedDstTexture;
    CHECK_HR(deviceContext->Map(dstStagingTexture, 0, D3D11_MAP_READ, 0, &mappedDstTexture));

    memcpy(dstMipSet->pData, (BYTE*)mappedDstTexture.pData, dstMipSet->dwDataSize);

    deviceContext->Unmap(dstStagingTexture, 0);
    dstStagingTexture.Release();
#endif
    return CMP_OK;
}
