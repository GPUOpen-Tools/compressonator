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
/// \file GPU_DirectX.h
//
//=====================================================================


#ifndef H_GPU_DIRECTX
#define H_GPU_DIRECTX

#include <gpu_decodebase.h>
#include <directxtex.h>

#include <dxgiformat.h>
#include <d3d11.h>
#include <directxmath.h>
#include <windows.h>

#include <stdio.h>


namespace GPU_Decode {

#define MAX_ERR_STR 128

class GPU_DirectX : public RenderWindow {
  public:
    GPU_DirectX(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback);
    ~GPU_DirectX();

    virtual CMP_ERROR WINAPI Decompress(
        const CMP_Texture* pSourceTexture,
        CMP_Texture* pDestTexture
    );

  private:
    D3D_DRIVER_TYPE             m_driverType;
    D3D_FEATURE_LEVEL           m_featureLevel;
    ID3D11Device*               m_pd3dDevice;
    ID3D11DeviceContext*        m_pImmediateContext;
    IDXGISwapChain*             m_pSwapChain;
    ID3D11RenderTargetView*     m_pRenderTargetView;
    ID3D11Texture2D*            m_pDepthStencil;
    ID3D11DepthStencilView*     m_pDepthStencilView;
    ID3D11VertexShader*         m_pVertexShader;
    ID3D11PixelShader*          m_pPixelShader;
    ID3D11InputLayout*          m_pVertexLayout;
    ID3D11Buffer*               m_pVertexBuffer;
    ID3D11Buffer*               m_pIndexBuffer;
    ID3D11Buffer*               m_pCBArrayControl;
    ID3D11ShaderResourceView*   m_pSRV;
    ID3D11BlendState*           m_AlphaBlendState;
    ID3D11SamplerState*         m_pSamplerLinear;
    UINT                        m_iCurrentIndex;
    UINT                        m_iMaxIndex;
    UINT                        m_iIndices;
    ID3D11Resource*             m_pResource;

    uint32_t    m_width;
    uint32_t    m_height;

    char m_err_str[MAX_ERR_STR];
    char *hResultErr(HRESULT hr);

    HRESULT InitDevice(const DirectX::TexMetadata& mdata, CMP_FORMAT cmp_format);
    DXGI_FORMAT CMP2DXGIFormat(CMP_FORMAT cmp_format);
    int CaptureAnImage(HWND hWnd, CMP_Texture* pDestTexture);
    void Render();
    void CleanupDevice();
};
}

#endif
