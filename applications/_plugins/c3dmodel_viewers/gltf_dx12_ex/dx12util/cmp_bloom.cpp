// AMD AMDUtils code
//
// Copyright(c) 2017 Advanced Micro Devices, Inc.All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#include "cmp_dynamicbufferringdx12.h"
#include "cmp_staticbufferpooldx12.h"
#include "cmp_uploadheapdx12.h"
#include "cmp_texturedx12.h"
#include "cmp_postprocps.h"
#include "cmp_texturedx12.h"
#include "cmp_bloom.h"


void CMP_Bloom::OnCreate(
    ID3D12Device* pDevice,
    UINT node,
    ResourceViewHeapsDX12      *pHeaps,
    StaticBufferPoolDX12       *pStaticBufferPool,
    DynamicBufferRingDX12      *pConstantBufferRing
) {
    m_pHeaps = pHeaps;
    m_pConstantBufferRing = pConstantBufferRing;
    m_blurX.OnCreate(pDevice, node, "./plugins/shaders/", "blur.hlsl", DXGI_FORMAT_R11G11B10_FLOAT, pHeaps, pStaticBufferPool, 1);
    m_blurY.OnCreate(pDevice, node, "./plugins/shaders/", "blur.hlsl", DXGI_FORMAT_R11G11B10_FLOAT, pHeaps, pStaticBufferPool, 1);
    m_downscale.OnCreate(pDevice, node, "./plugins/shaders/", "downscale.hlsl", DXGI_FORMAT_R11G11B10_FLOAT, pHeaps, pStaticBufferPool, 1);

    D3D12_BLEND_DESC blendingAdd = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    blendingAdd.IndependentBlendEnable = TRUE;
    blendingAdd.RenderTarget[0] = D3D12_RENDER_TARGET_BLEND_DESC {
        TRUE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ONE, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ONE, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    m_blendAdd.OnCreate(pDevice, node, "./plugins/shaders/", "blend.hlsl", DXGI_FORMAT_R11G11B10_FLOAT, pHeaps, pStaticBufferPool, 1, &blendingAdd);


    D3D12_BLEND_DESC blendingFactor = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    blendingFactor.IndependentBlendEnable = TRUE;
    blendingFactor.RenderTarget[0] = D3D12_RENDER_TARGET_BLEND_DESC {
        TRUE,FALSE,
        D3D12_BLEND_BLEND_FACTOR, D3D12_BLEND_INV_BLEND_FACTOR, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_BLEND_FACTOR, D3D12_BLEND_INV_BLEND_FACTOR, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    m_blendFactor.OnCreate(pDevice, node, "./plugins/shaders/", "blend.hlsl", DXGI_FORMAT_R16G16B16A16_UNORM, pHeaps, pStaticBufferPool, 1, &blendingFactor);

    m_pHeaps->AllocRTVDescriptor(1, &m_inputRTV);
    m_pHeaps->AllocCBV_SRV_UAVDescriptor(1, &m_inputSRV);

    for (int i = 0; i < BLOOM_DEPTH; i++) {
        m_pHeaps->AllocCBV_SRV_UAVDescriptor(1, &m_mipSRV[i]);
        m_pHeaps->AllocRTVDescriptor(1, &m_mipRTV[i]);

        m_pHeaps->AllocCBV_SRV_UAVDescriptor(1, &m_blurTempMipSRV[i]);
        m_pHeaps->AllocRTVDescriptor(1, &m_blurTempMipRTV[i]);
    }
}

void CMP_Bloom::OnCreateWindowSizeDependentResources(ID3D12Device* pDevice, DWORD Width, DWORD Height, UINT node, UINT nodemask)
{
    m_Width = Width;
    m_Height = Height;

    for (int i = 0; i < BLOOM_DEPTH; i++) {
        // downscaling
        m_mip[i].InitRendertarget(pDevice, &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R11G11B10_FLOAT, Width >> (i + 1), Height >> (i + 1), 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET), node, nodemask);
        m_mip[i].Resource()->SetName(L"Bloom::m_mip");
        m_mip[i].CreateSRV(0, &m_mipSRV[i]);
        m_mip[i].CreateRTV(0, &m_mipRTV[i]);

        // blur temporals
        m_blurTempMip[i].InitRendertarget(pDevice, &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R11G11B10_FLOAT, Width >> (i + 1), Height >> (i + 1), 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET), node, nodemask);
        m_blurTempMip[i].Resource()->SetName(L"Bloom::m_temp");
        m_blurTempMip[i].CreateSRV(0, &m_blurTempMipSRV[i]);
        m_blurTempMip[i].CreateRTV(0, &m_blurTempMipRTV[i]);
    }

}

void CMP_Bloom::OnDestroyWindowSizeDependentResources()
{
    for (int i = 0; i < BLOOM_DEPTH; i++) {
        m_mip[i].OnDestroy();
        m_blurTempMip[i].OnDestroy();
    }
}

void CMP_Bloom::OnDestroy()
{
    m_downscale.OnDestroy();
    m_blurY.OnDestroy();
    m_blurX.OnDestroy();
    m_blendAdd.OnDestroy();
    m_blendFactor.OnDestroy();
}

void CMP_Bloom::SetViewPortAndScissor(ID3D12GraphicsCommandList* pCommandList, int i)
{
    D3D12_VIEWPORT viewPort = { 0.0f, 0.0f, static_cast<float>(m_Width >> (i + 1)), static_cast<float>(m_Height >> (i + 1)), 0.0f, 1.0f };
    D3D12_RECT rectScissor = { 0, 0, (LONG)(m_Width >> (i + 1)), (LONG)(m_Height >> (i + 1)) };
    pCommandList->RSSetViewports(1, &viewPort);
    pCommandList->RSSetScissorRects(1, &rectScissor);

}

void CMP_Bloom::Draw(ID3D12GraphicsCommandList* pCommandList, TextureDX12* pInput, float glowFactor)
{
    pInput->CreateRTV(0, &m_inputRTV);
    pInput->CreateSRV(0, &m_inputSRV);

    // downsample
    //
    for(int i=0; i<BLOOM_DEPTH; i++) {
        if (i > 0) {
            pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_mip[i-1].Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
        }

        pCommandList->OMSetRenderTargets(1, &m_mipRTV[i].GetCPU(), true, NULL);
        SetViewPortAndScissor(pCommandList, i);

        cbDownscale *data;
        D3D12_GPU_DESCRIPTOR_HANDLE constantBuffer;
        m_pConstantBufferRing->AllocConstantBuffer(sizeof(cbDownscale), (void **)&data, &constantBuffer);
        data->invWidth  = 1.0f / (float)(m_Width>>i);
        data->invHeight = 1.0f / (float)(m_Height>>i);

        m_downscale.Draw(pCommandList, 1, (i==0)? &m_inputSRV : &m_mipSRV[i-1], constantBuffer);
    }
    pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_mip[BLOOM_DEPTH-1].Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    // [1]0.25, 0.75, 1.5, 2.5, 3.0

    // upscale and blur
    //
    for (int i = BLOOM_DEPTH-1; i >= 0; i--) {
        // gaussian blur
        {
            SetViewPortAndScissor(pCommandList, i);

            // X axis blur
            {
                pCommandList->OMSetRenderTargets(1, &m_blurTempMipRTV[i].GetCPU(), true, NULL);

                cbBlur *data;
                D3D12_GPU_DESCRIPTOR_HANDLE constantBuffer;
                m_pConstantBufferRing->AllocConstantBuffer(sizeof(cbBlur), (void **)&data, &constantBuffer);
                data->dirX = 1.0f / (float)m_Width;
                data->dirY = 0.0f / (float)m_Height;
                m_blurX.Draw(pCommandList, 1, &m_mipSRV[i], constantBuffer);
            }

            pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_blurTempMip[i].Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
            pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_mip[i].Resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

            // Y axis
            {
                pCommandList->OMSetRenderTargets(1, &m_mipRTV[i].GetCPU(), true, NULL);

                cbBlur *data;
                D3D12_GPU_DESCRIPTOR_HANDLE constantBuffer;
                m_pConstantBufferRing->AllocConstantBuffer(sizeof(cbBlur), (void **)&data, &constantBuffer);
                data->dirX = 0.0f / (float)m_Width;
                data->dirY = 1.0f / (float)m_Height;
                m_blurY.Draw(pCommandList, 1, &m_blurTempMipSRV[i], constantBuffer);
            }

            pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_mip[i].Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

            if (i > 0)
                pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_mip[i - 1].Resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
        }

        // blend
        {
            cbBlend *data;
            D3D12_GPU_DESCRIPTOR_HANDLE constantBuffer;
            m_pConstantBufferRing->AllocConstantBuffer(sizeof(cbBlend), (void **)&data, &constantBuffer);

            SetViewPortAndScissor(pCommandList, i-1);
            if (i == 0) {
                data->weight = 1.0;
                float blendFactor[4] = { glowFactor,glowFactor,glowFactor, 1.0 };
                pCommandList->OMSetBlendFactor(blendFactor);
                pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pInput->Resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
                pCommandList->OMSetRenderTargets(1, &m_inputRTV.GetCPU(), true, NULL);
                m_blendFactor.Draw(pCommandList, 1, &m_mipSRV[i], constantBuffer);
            } else {
                data->weight = 1.0;
                pCommandList->OMSetRenderTargets(1, &m_mipRTV[i - 1].GetCPU(), true, NULL);
                m_blendAdd.Draw(pCommandList, 1, &m_mipSRV[i], constantBuffer);
                pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_mip[i - 1].Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
            }
        }

        pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_mip[i].Resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
    }

    for (int i = 0; i < BLOOM_DEPTH; i++) {
        pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_blurTempMip[i].Resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
    }

}