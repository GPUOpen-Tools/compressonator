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
#include "cmp_blurps.h"

#include <directxmath.h>

using namespace DirectX;


float GaussianWeight(float x) {
    return expf(-.5f* x*x) / sqrtf(2.0f*XM_PI);
}

float GaussianArea(float x0, float x1, int samples) {
    float n = 0;

    for (int i = 0; i < samples; i++) {
        float t = (float)i / (float)(samples);
        float x = ((1.0f - t) * x0) + (t * x1);
        n += GaussianWeight(x);
    }

    return n * (x1 - x0) / (float)samples;
}

void GenerateGaussianWeights(int count, float *out) {
    // a 3 sigma width covers 99.7 of the kernel
    float delta = 3.0f / (float)count;

    out[0] = GaussianArea(0.0f, delta / 2.0f, 500) * 2.0f;

    for (int i = 1; i < count; i++) {
        float x = delta*i - (delta / 2.0f);
        out[i] = GaussianArea(x, x + delta, 1000);
    }
}

void CMP_BlurPS::OnCreate(
    ID3D12Device* pDevice,
    UINT node,
    ResourceViewHeapsDX12      *pHeaps,
    StaticBufferPoolDX12       *pStaticBufferPool,
    DynamicBufferRingDX12      *pConstantBufferRing
) {
    m_pHeaps = pHeaps;
    m_pConstantBufferRing = pConstantBufferRing;
    m_blurX.OnCreate(pDevice, node, "./plugins/shaders/", "blur.hlsl", DXGI_FORMAT_R8G8B8A8_UNORM, pHeaps, pStaticBufferPool, 1);
    m_blurY.OnCreate(pDevice, node, "./plugins/shaders/", "blur.hlsl", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, pHeaps, pStaticBufferPool, 1);

    m_pHeaps->AllocCBV_SRV_UAVDescriptor(1, &m_tempSRV);
    m_pHeaps->AllocRTVDescriptor(1, &m_tempRTV);

    /*
        float out[16];
        for (int k = 3; k <= 16; k++)
        {
            GenerateGaussianWeights(k, out);

            char str[1024];
            int ii = 0;
            ii += sprintf_s(&str[ii], 1024 - ii, "int s_lenght = %i; float s_coeffs[] = {", k);
            for (int i = 0; i < k; i++)
            {
                ii += sprintf_s(&str[ii], 1024 - ii, "%f, ", out[i]);
            }

            float r = out[0];
            for (int i = 1; i < k; i++)
                r += 2 * out[i];

            assert(r <= 1.0f && r>0.97f);


            ii += sprintf_s(&str[ii], 1024 - ii, "}; // norm = %f\n", r);

            OutputDebugStringA(str);
        }
    */
}

void CMP_BlurPS::OnCreateWindowSizeDependentResources(ID3D12Device* pDevice, DWORD Width, DWORD Height, UINT node, UINT nodemask)
{
    m_Width = Width;
    m_Height = Height;

    m_temp.InitRendertarget(pDevice, &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, Width, Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET), node, nodemask);
    m_temp.Resource()->SetName(L"Blur::m_temp");

    m_temp.CreateSRV(0, &m_tempSRV);

    m_temp.CreateRTV(0, &m_tempRTV);

}

void CMP_BlurPS::OnDestroyWindowSizeDependentResources()
{
    m_temp.OnDestroy();
}

void CMP_BlurPS::OnDestroy()
{
    m_blurY.OnDestroy();
    m_blurX.OnDestroy();
}


void CMP_BlurPS::Draw(ID3D12GraphicsCommandList* pCommandList, CBV_SRV_UAV* pSrcResource, D3D12_CPU_DESCRIPTOR_HANDLE* pDestination)
{
    D3D12_VIEWPORT viewPort = { 0.0f, 0.0f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.0f, 1.0f };
    D3D12_RECT rectScissor = { 0, 0, (LONG)m_Width, (LONG)m_Height };

    pCommandList->RSSetViewports(1, &viewPort);
    pCommandList->RSSetScissorRects(1, &rectScissor);


    // X axis
    {
        pCommandList->OMSetRenderTargets(1, &m_tempRTV.GetCPU(), true, NULL);

        cbBlur *dataX;
        D3D12_GPU_DESCRIPTOR_HANDLE constantBufferX;
        m_pConstantBufferRing->AllocConstantBuffer(sizeof(cbBlur), (void **)&dataX, &constantBufferX);
        dataX->dirX = 1.0f / (float)m_Width;
        dataX->dirY = 0.0f / (float)m_Height;
        m_blurX.Draw(pCommandList, 1, pSrcResource, constantBufferX);
    }

    pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_temp.Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    // Y axis
    {
        pCommandList->OMSetRenderTargets(1, pDestination, true, NULL);

        cbBlur *dataY;
        D3D12_GPU_DESCRIPTOR_HANDLE constantBufferY;
        m_pConstantBufferRing->AllocConstantBuffer(sizeof(cbBlur), (void **)&dataY, &constantBufferY);
        dataY->dirX = 0.0f / (float)m_Width;
        dataY->dirY = 1.0f / (float)m_Height;
        m_blurY.Draw(pCommandList, 1, &m_tempSRV, constantBufferY);
    }

    pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_temp.Resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

}