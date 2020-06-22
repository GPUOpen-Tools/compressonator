//=====================================================================
// Copyright (c) 2016    Advanced Micro Devices, Inc. All rights reserved.
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

// #define DECOMPRESS_SAVE_TO_BMP

#include "GPU_DirectX.h"
#include "directx_shaders\vs.h"
#include "directx_shaders\ps1D.h"
#include "directx_shaders\\ps1Darray.h"
#include "directx_shaders\\ps2D.h"
#include "directx_shaders\ps2Darray.h"
#include "directx_shaders\ps3D.h"
#include "directx_shaders\psCube.h"
#include "D3D11.h"

using namespace DirectX;
using namespace GPU_Decode;

#pragma comment(lib, "opengl32.lib")        // Open GL used for Window Context!
#pragma comment(lib, "D3D11.lib")

#pragma pack(push,1)
struct SimpleVertex
{
    XMFLOAT4 Pos;
    XMFLOAT4 Tex;
};

struct CBArrayControl
{
    float Index;
    float pad[3];
};
#pragma pack(pop)



//--------------------------------------------------------------------------------------


char *GPU_DirectX::hResultErr(HRESULT hr)
{
   FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
       NULL, hr,
       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
       m_err_str,
       MAX_ERR_STR,
       NULL );
   return(m_err_str);
}

//---------------------------------------------------------------------------------
GPU_DirectX::GPU_DirectX(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback):RenderWindow("DirectX")
{
    m_driverType = D3D_DRIVER_TYPE_NULL;
    m_featureLevel = D3D_FEATURE_LEVEL_11_0;
    m_pd3dDevice = nullptr;
    m_pImmediateContext = nullptr;
    m_pSwapChain = nullptr;
    m_pRenderTargetView = nullptr;
    m_pDepthStencil = nullptr;
    m_pDepthStencilView = nullptr;
    m_pVertexShader = nullptr;
    m_pPixelShader = nullptr;
    m_pVertexLayout = nullptr;
    m_pVertexBuffer = nullptr;
    m_pIndexBuffer = nullptr;
    m_pCBArrayControl = nullptr;
    m_pSRV = nullptr;
    m_AlphaBlendState = nullptr;
    m_pSamplerLinear = nullptr;
    m_pResource = nullptr;
    m_iCurrentIndex = 0;
    m_iMaxIndex = 1;
    m_iIndices = 0;

    //set default width and height if is 0
    if (Width <= 0)
        Width = 640;
    if (Height <= 0)
        Height = 480;

    // Allign data 
    m_width  = ((Width  + 3) / 4) * 4;
    m_height = ((Height + 3) / 4) * 4;

    HRESULT hr = InitWindow(Width, Height, callback);
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed InitWindow: %s\n",hResultErr(hr));
        assert(0);
    }

    EnableWindowContext(m_hWnd, &m_hDC, &m_hRC);
}


GPU_DirectX::~GPU_DirectX()
{
}




//--------------------------------------------------------------------------------------


HRESULT GPU_DirectX::InitDevice(const TexMetadata& mdata, CMP_FORMAT cmp_format)
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(m_hWnd, &rc);
    UINT width  = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    if(cmp_format == CMP_FORMAT_ARGB_16F)
        sd.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    else
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        m_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
    {
       fprintf(stderr, "[DirectX] Failed D3D11CreateDeviceAndSwapChain: %s\n",hResultErr(hr));
       return hr;
    }

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed GetBuffer: %s\n",hResultErr(hr));
        return hr;
    }

    hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed CreateRenderTargetView: %s\n",hResultErr(hr));
        return hr;
    }

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width  = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = m_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &m_pDepthStencil);
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed CreateTexture2D: %s\n",hResultErr(hr));
        return hr;
    }

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed CreateDepthStencilView: %s\n",hResultErr(hr));
        return hr;
    }

    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pImmediateContext->RSSetViewports(1, &vp);

    // Create the vertex shader
    hr = m_pd3dDevice->CreateVertexShader(g_VS, sizeof(g_VS), nullptr, &m_pVertexShader);
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed CreateVertexShader: %s\n",hResultErr(hr));
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = m_pd3dDevice->CreateInputLayout(layout, numElements, g_VS, sizeof(g_VS), &m_pVertexLayout);
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed CreateInputLayout: %s\n",hResultErr(hr));
        return hr;
    }

    // Set the input layout
    m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

    // Select the pixel shader
    bool isCubeMap = false;
    bool is1D = false;
    const BYTE* pshader = nullptr;
    size_t pshader_size = 0;


    //------------------------------------------------
    // Enable the code as needed for each new case
    //------------------------------------------------
    switch (mdata.dimension)
    {
    case TEX_DIMENSION_TEXTURE1D:
        if (mdata.arraySize > 1)
        {
            pshader = g_PS_1DArray;
            pshader_size = sizeof(g_PS_1DArray);
        }
        else
        {
            pshader = g_PS_1D;
            pshader_size = sizeof(g_PS_1D);
        }
        is1D = true;
        break;

    case TEX_DIMENSION_TEXTURE2D:
        if (mdata.miscFlags & TEX_MISC_TEXTURECUBE)
        {
            pshader = g_PS_Cube;
            pshader_size = sizeof(g_PS_Cube);
            isCubeMap = true;
        }
        else 
        if (mdata.arraySize > 1)
        {
            pshader = g_PS_2DArray;
            pshader_size = sizeof(g_PS_2DArray);
        }
        else
        {
         pshader = g_PS_2D;
         pshader_size = sizeof(g_PS_2D);
        }
        break;

    case TEX_DIMENSION_TEXTURE3D:
        pshader = g_PS_3D;
        pshader_size = sizeof(g_PS_3D);
        break;

    default:
        return E_FAIL;
    }
    if (!(pshader && pshader_size > 0))
        fprintf(stderr, "[DirectX12] No shader available");
    assert(pshader && pshader_size > 0);

    // Create the pixel shader
    hr = m_pd3dDevice->CreatePixelShader(pshader, pshader_size, nullptr, &m_pPixelShader);
    if (FAILED(hr))
        return hr;

    // Create vertex buffer
    UINT nverts;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));

    static const SimpleVertex verticesCube[] =
    {
        // Render cubemaps as horizontal cross

        // XPOS
        { XMFLOAT4(.5f, .25f, 0.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f) },
        { XMFLOAT4(1.f, .25f, 0.f, 1.f), XMFLOAT4(1.f, 0.f, 0.f, 0.f) },
        { XMFLOAT4(.5f, -.25f, 0.f, 1.f), XMFLOAT4(0.f, 1.f, 0.f, 0.f) },
        { XMFLOAT4(1.f, -.25f, 0.f, 1.f), XMFLOAT4(1.f, 1.f, 0.f, 0.f) },

        // XNEG
        { XMFLOAT4(-.5f, .25f, 0.f, 1.f), XMFLOAT4(0.f, 0.f, 1.f, 0.f) },
        { XMFLOAT4(0.f, .25f, 0.f, 1.f), XMFLOAT4(1.f, 0.f, 1.f, 0.f) },
        { XMFLOAT4(-.5f, -.25f, 0.f, 1.f), XMFLOAT4(0.f, 1.f, 1.f, 0.f) },
        { XMFLOAT4(0.f, -.25f, 0.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 0.f) },

        // YPOS
        { XMFLOAT4(-.5f, .75f, 0.f, 1.f), XMFLOAT4(0.f, 0.f, 2.f, 0.f) },
        { XMFLOAT4(0.f, .75f, 0.f, 1.f), XMFLOAT4(1.f, 0.f, 2.f, 0.f) },
        { XMFLOAT4(-.5f, .25f, 0.f, 1.f), XMFLOAT4(0.f, 1.f, 2.f, 0.f) },
        { XMFLOAT4(0.f, .25f, 0.f, 1.f), XMFLOAT4(1.f, 1.f, 2.f, 0.f) },

        // YNEG
        { XMFLOAT4(-.5f, -.25f, 0.f, 1.f), XMFLOAT4(0.f, 0.f, 3.f, 0.f) },
        { XMFLOAT4(0.f, -.25f, 0.f, 1.f), XMFLOAT4(1.f, 0.f, 3.f, 0.f) },
        { XMFLOAT4(-.5f, -.75f, 0.f, 1.f), XMFLOAT4(0.f, 1.f, 3.f, 0.f) },
        { XMFLOAT4(0.f, -.75f, 0.f, 1.f), XMFLOAT4(1.f, 1.f, 3.f, 0.f) },

        // ZPOS
        { XMFLOAT4(0.f, .25f, 0.f, 1.f), XMFLOAT4(0.f, 0.f, 4.f, 0.f) },
        { XMFLOAT4(.5f, .25f, 0.f, 1.f), XMFLOAT4(1.f, 0.f, 4.f, 0.f) },
        { XMFLOAT4(0.f, -.25f, 0.f, 1.f), XMFLOAT4(0.f, 1.f, 4.f, 0.f) },
        { XMFLOAT4(.5f, -.25f, 0.f, 1.f), XMFLOAT4(1.f, 1.f, 4.f, 0.f) },

        // ZNEG
        { XMFLOAT4(-1.f, .25f, 0.f, 1.f), XMFLOAT4(0.f, 0.f, 5.f, 0.f) },
        { XMFLOAT4(-.5f, .25f, 0.f, 1.f), XMFLOAT4(1.f, 0.f, 5.f, 0.f) },
        { XMFLOAT4(-1.f, -.25f, 0.f, 1.f), XMFLOAT4(0.f, 1.f, 5.f, 0.f) },
        { XMFLOAT4(-.5f, -.25f, 0.f, 1.f), XMFLOAT4(1.f, 1.f, 5.f, 0.f) },
    };

    static const SimpleVertex vertices[] =
    {
        { XMFLOAT4(-1.f, 1.f, 0.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f) },
        { XMFLOAT4(1.f, 1.f, 0.f, 1.f), XMFLOAT4(1.f, 0.f, 0.f, 0.f) },
        { XMFLOAT4(-1.f, -1.f, 0.f, 1.f), XMFLOAT4(0.f, 1.f, 0.f, 0.f) },
        { XMFLOAT4(1.f, -1.f, 0.f, 1.f), XMFLOAT4(1.f, 1.f, 0.f, 0.f) },
    };

    static const SimpleVertex vertices1D[] =
    {
        { XMFLOAT4(-1.f, .05f, 0.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f) },
        { XMFLOAT4(1.f, .05f, 0.f, 1.f), XMFLOAT4(1.f, 0.f, 0.f, 0.f) },
        { XMFLOAT4(-1.f, -.05f, 0.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f) },
        { XMFLOAT4(1.f, -.05f, 0.f, 1.f), XMFLOAT4(1.f, 0.f, 0.f, 0.f) },
    };

    if (isCubeMap)
    {
        nverts = _countof(verticesCube);
        InitData.pSysMem = verticesCube;
    }
    else if (is1D)
    {
        nverts = _countof(vertices1D);
        InitData.pSysMem = vertices1D;
    }
    else
    {
        nverts = _countof(vertices);
        InitData.pSysMem = vertices;
    }

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * nverts;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed CreateBuffer 1 : %s\n",hResultErr(hr));
        return hr;
    }

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // Create index buffer
    static const WORD indicesCube[] =
    {
        0, 1, 2,
        2, 1, 3,
        4, 5, 6,
        6, 5, 7,
        8, 9, 10,
        10, 9, 11,
        12, 13, 14,
        14, 13, 15,
        16, 17, 18,
        18, 17, 19,
        20, 21, 22,
        22, 21, 23
    };

    static const WORD indices[] =
    {
        0, 1, 2,
        2, 1, 3
    };

    if (isCubeMap)
    {
        m_iIndices = _countof(indicesCube);
        InitData.pSysMem = indicesCube;
    }
    else
    {
        m_iIndices = _countof(indices);
        InitData.pSysMem = indices;
    }

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = m_iIndices * sizeof(WORD);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed CreateBuffer 2 : %s\n",hResultErr(hr));
        return hr;
    }

    // Set index buffer
    m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffers
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBArrayControl);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &m_pCBArrayControl);
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed CreateBuffer 3 : %s\n",hResultErr(hr));
        return hr;
    }

    // Create the state objects
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
    if (FAILED(hr))
        return hr;

    D3D11_BLEND_DESC dsc =
    {
        false,
        false,
        {
            false,
            D3D11_BLEND_SRC_ALPHA,
            D3D11_BLEND_INV_SRC_ALPHA,
            D3D11_BLEND_OP_ADD,
            D3D11_BLEND_ZERO,
            D3D11_BLEND_ZERO,
            D3D11_BLEND_OP_ADD,
            D3D11_COLOR_WRITE_ENABLE_ALL
        }
    };
    hr = m_pd3dDevice->CreateBlendState(&dsc, &m_AlphaBlendState);
    if (FAILED(hr))
    {
        fprintf(stderr, "[DirectX] Failed CreateBlendState: %s\n",hResultErr(hr));
        return hr;
    }

    return S_OK;
}



//--------------------------------------------------------------------------------------
void GPU_DirectX::Render()
{
    float ClearColor[4] = { 0.f, 0.f, 0.f, 1.0f }; //red,green,blue,alpha
    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    float bf[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_pImmediateContext->OMSetBlendState(m_AlphaBlendState, bf, 0xffffffff);

    CBArrayControl cb;
    cb.Index = (float)m_iCurrentIndex;
    m_pImmediateContext->UpdateSubresource(m_pCBArrayControl, 0, nullptr, &cb, 0, 0);

    m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
    m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pCBArrayControl);
    m_pImmediateContext->PSSetShaderResources(0, 1, &m_pSRV);
    m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
    m_pImmediateContext->DrawIndexed(m_iIndices, 0, 0);

    m_pSwapChain->Present(0, 0);

}


//--------------------------------------------------------------------------------------
void GPU_DirectX::CleanupDevice()
{
    if (m_pImmediateContext) m_pImmediateContext->ClearState();

    if (m_pSamplerLinear) m_pSamplerLinear->Release();
    if (m_AlphaBlendState) m_AlphaBlendState->Release();
    if (m_pSRV) m_pSRV->Release();
    if (m_pVertexBuffer) m_pVertexBuffer->Release();
    if (m_pIndexBuffer) m_pIndexBuffer->Release();
    if (m_pCBArrayControl) m_pCBArrayControl->Release();
    if (m_pVertexLayout) m_pVertexLayout->Release();
    if (m_pVertexShader) m_pVertexShader->Release();
    if (m_pPixelShader) m_pPixelShader->Release();
    if (m_pDepthStencil) m_pDepthStencil->Release();
    if (m_pDepthStencilView) m_pDepthStencilView->Release();
    if (m_pRenderTargetView) m_pRenderTargetView->Release();
    if (m_pSwapChain) m_pSwapChain->Release();
    if (m_pImmediateContext) m_pImmediateContext->Release();
    if (m_pd3dDevice) m_pd3dDevice->Release();
    if (m_pResource) m_pResource->Release();
}


DXGI_FORMAT GPU_DirectX::CMP2DXGIFormat(CMP_FORMAT cmp_format)
{
    DXGI_FORMAT dxgi_format;

    switch (cmp_format)
    {
    // Compression formats ----------
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:
                            dxgi_format = DXGI_FORMAT_BC1_UNORM;
                            break;
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_DXT3:
                            dxgi_format = DXGI_FORMAT_BC2_UNORM;
                            break;
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_DXT5:
                            dxgi_format = DXGI_FORMAT_BC3_UNORM;
                            break;
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_ATI1N:
                            dxgi_format = DXGI_FORMAT_BC4_UNORM;
                            break;
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_ATI2N:
    case CMP_FORMAT_ATI2N_XY:
    case CMP_FORMAT_ATI2N_DXT5:
                            dxgi_format = DXGI_FORMAT_BC5_UNORM;
                            break;
    case CMP_FORMAT_BC6H:
                            dxgi_format = DXGI_FORMAT_BC6H_UF16;
                            break;
    case CMP_FORMAT_BC6H_SF:
                            dxgi_format = DXGI_FORMAT_BC6H_SF16;
                            break;
    case CMP_FORMAT_BC7:
                            dxgi_format = DXGI_FORMAT_BC7_UNORM;
                            break;

    // Unknown compression mapping to Direct X
    case CMP_FORMAT_ASTC:
    case CMP_FORMAT_ATC_RGB:
    case CMP_FORMAT_ATC_RGBA_Explicit:
    case CMP_FORMAT_ATC_RGBA_Interpolated:
    case CMP_FORMAT_DXT5_xGBR:
    case CMP_FORMAT_DXT5_RxBG:
    case CMP_FORMAT_DXT5_RBxG:
    case CMP_FORMAT_DXT5_xRBG:
    case CMP_FORMAT_DXT5_RGxB:
    case CMP_FORMAT_DXT5_xGxR:
    case CMP_FORMAT_ETC_RGB:
    case CMP_FORMAT_ETC2_RGB:
    case CMP_FORMAT_ETC2_RGBA:
    case CMP_FORMAT_ETC2_RGBA1:
#ifdef USE_GTC
    case CMP_FORMAT_GTC:
#endif
#ifdef USE_BASIS
    case CMP_FORMAT_BASIS:
#endif
        // -----------------------------------
    case CMP_FORMAT_Unknown:
    default:
        dxgi_format = DXGI_FORMAT_UNKNOWN;
        break;
    }

    return dxgi_format;
}

//--------------------------------------------------------------------------------------
#pragma warning( suppress : 6262 )

CMP_ERROR WINAPI GPU_DirectX::Decompress(
    const CMP_Texture* pSourceTexture,
    CMP_Texture* pDestTexture
    )
{
    HRESULT hr;

    TexMetadata mdata;

    memset(&mdata, 0, sizeof(TexMetadata));
    mdata.height= m_height; // pSourceTexture->dwHeight;
    mdata.width = m_width; // pSourceTexture->dwWidth;
    mdata.depth = 1;
    mdata.arraySize = 1;
    mdata.mipLevels = 1;
    mdata.dimension = TEX_DIMENSION_TEXTURE2D;
    mdata.format = CMP2DXGIFormat(pSourceTexture->format);

/***  Use this for debugging DDS file loads
    LPWSTR lpImageSrc = L"";
    ScratchImage image;
    hr = LoadFromDDSFile(lpImageSrc, DDS_FLAGS_NONE, &mdata, image);
    if (FAILED(hr))
    {
        wchar_t buff[2048] = { 0 };
        swprintf_s(buff, "Failed to load texture file\n\nFilename = %ls\nHRESULT %08X", lpImageSrc, hr);
        return 0;
    }
***/

    if (FAILED(InitDevice(mdata, pDestTexture->format)))
    {
        CleanupDevice();
        return CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB;
    }

    if (mdata.dimension == TEX_DIMENSION_TEXTURE3D)
    {
            wchar_t buff[2048] = { 0 };
            swprintf_s(buff, L"Arrays of volume textures are not supported\n\nArray size %Iu", mdata.arraySize);
            return CMP_ERR_GENERIC;
    }
    else
    {
        m_iMaxIndex = static_cast<UINT>(mdata.arraySize);
    }

    switch (mdata.format)
    {
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            if (m_featureLevel < D3D_FEATURE_LEVEL_11_0)
            {
                wchar_t buff[2048] = { 0 };
                swprintf_s(buff, L"BC6H/BC7 requires DirectX 11 hardware\n\nDXGI Format %d\nFeature Level %d", 
                                 pSourceTexture->format, m_featureLevel);
                return CMP_ERR_GENERIC;
            }
            break;

        default:
        {
            UINT flags = 0;
            hr = m_pd3dDevice->CheckFormatSupport(mdata.format, &flags);
            if (FAILED(hr) || !(flags & (D3D11_FORMAT_SUPPORT_TEXTURE1D | D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_TEXTURE3D)))
            {
                wchar_t buff[2048] = { 0 };
                swprintf_s(buff, L"Format not supported by DirectX hardware\n\nDXGI Format %d\nFeature Level %d\nHRESULT = %08X", mdata.format, m_featureLevel, hr);
                return CMP_ERR_GENERIC;
            }
        }
        break;
    }

    // Special case to make sure Texture cubes remain arrays
    mdata.miscFlags &= ~TEX_MISC_TEXTURECUBE;

    Image srcImage;

    // Set size to 4 pixel boundaries
    // Should check format is compressed for none compressed cases
    // where 4 pixel bound is not required.
    srcImage.width              = m_width; // pSourceTexture->dwWidth;//((pSourceTexture->dwWidth  + 3) / 4) * 4;
    srcImage.height             = m_height;// pSourceTexture->dwHeight;//((pSourceTexture->dwHeight + 3) / 4) * 4;
    srcImage.format             = mdata.format;
    srcImage.pixels             = pSourceTexture->pData;

    ComputePitch(mdata.format, srcImage.width, srcImage.height, srcImage.rowPitch, srcImage.slicePitch, CP_FLAGS_NONE);

    hr = CreateShaderResourceView(m_pd3dDevice, &srcImage, 1, mdata, &m_pSRV);
    if (FAILED(hr))
    {
         return CMP_ERR_GENERIC;
    }

#ifdef SHOW_WINDOW
    // Activate the window: Use for debugging!
    ShowWindow(m_hWnd, SW_SHOW);
#endif
    //  Wait in Main message loop, until render is complete!!
    //  then exit

    MSG msg = { 0 };

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();
            break;
        }
    }

    m_pRenderTargetView->GetResource(&m_pResource);
    ScratchImage sratchimage;
    CaptureTexture(m_pd3dDevice, m_pImmediateContext,m_pResource, sratchimage);

    size_t pxsize    = sratchimage.GetPixelsSize();

    // Check the size matches our output
    if (pxsize >= pDestTexture->dwDataSize)
    {
        //pDestTexture->dwWidth  = m_width;
        //pDestTexture->dwHeight = m_height;
        uint8_t *pxdata = sratchimage.GetPixels();
        memcpy(pDestTexture->pData, pxdata, pDestTexture->dwDataSize);
    }

    CleanupDevice();

    return CMP_OK; // msg.wParam;
}



