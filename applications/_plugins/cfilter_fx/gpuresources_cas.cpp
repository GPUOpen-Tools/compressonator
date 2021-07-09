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

#include "gpuresources_cas.h"

GpuResources_cas::GpuResources_cas() { }

void GpuResources_cas::InitBuffers_cas()
{
    //====================
    // CAS Constant buffer
    //====================
    CD3D11_BUFFER_DESC constantBufferDesc_CAS = CD3D11_BUFFER_DESC(sizeof(ConstantBufferStructureCAS), D3D11_BIND_CONSTANT_BUFFER);
    CHECK_HR(m_D3D11Device->CreateBuffer(&constantBufferDesc_CAS, nullptr, &m_ConstantBufferCAS));
}

void GpuResources_cas::GpuCompileShaders_cas(CMP_FORMAT format, bool useSRGB)
{
    //=====================================
    // CAS
    //=====================================
    const BYTE* shaderCode_NoScaling0;
    const BYTE* shaderCode_NoScaling1;
    size_t      shaderCodeSize_NoScaling0;
    size_t      shaderCodeSize_NoScaling1;
    GetShaderCode_CAS(shaderCode_NoScaling0, shaderCodeSize_NoScaling0, shaderCode_NoScaling1, shaderCodeSize_NoScaling1, format, useSRGB);
    CHECK_HR(m_D3D11Device->CreateComputeShader(shaderCode_NoScaling0, shaderCodeSize_NoScaling0, nullptr, &m_CasComputeShader));
    CHECK_HR(m_D3D11Device->CreateComputeShader(shaderCode_NoScaling1, shaderCodeSize_NoScaling1, nullptr, &m_CasComputeShader_NoScaling));
}

void GpuResources_cas::GetShaderCode_CAS(const BYTE*& outCode_NoScaling0,
                                         size_t&      outCodeSize_NoScaling0,
                                         const BYTE*& outCode_NoScaling1,
                                         size_t&      outCodeSize_NoScaling1,
                                         CMP_FORMAT   format,
                                         bool         useSRGB)
{
    if (!useSRGB || CMP_IsFloatFormat(format))
    {
        outCode_NoScaling0     = NoScaling0_Linear1::g_mainCS;
        outCodeSize_NoScaling0 = _countof(NoScaling0_Linear1::g_mainCS);
        outCode_NoScaling1     = NoScaling1_Linear1::g_mainCS;
        outCodeSize_NoScaling1 = _countof(NoScaling1_Linear1::g_mainCS);
    }
    else
    {
        outCode_NoScaling0     = NoScaling0_Linear0::g_mainCS;
        outCodeSize_NoScaling0 = (uint32_t)_countof(NoScaling0_Linear0::g_mainCS);
        outCode_NoScaling1     = NoScaling1_Linear0::g_mainCS;
        outCodeSize_NoScaling1 = (uint32_t)_countof(NoScaling1_Linear0::g_mainCS);
    }
}

void GpuResources_cas::CAS(float sharpness, ID3D11UnorderedAccessView* dstUav, uvec2 dstSize, ID3D11ShaderResourceView* srcSrv, uvec2 srcSize)
{
    ConstantBufferStructureCAS constBufStruct;

    CasSetup(constBufStruct.const0, 
             constBufStruct.const1, 
             sharpness, 
            (float)srcSize.x, (float)srcSize.y, 
            (float)dstSize.x, (float)dstSize.y);

    m_DeviceContext->UpdateSubresource(m_ConstantBufferCAS, 0, NULL, &constBufStruct, sizeof(constBufStruct), 0);

    const bool                 noScaling     = dstSize.x == srcSize.x && dstSize.y == srcSize.y;
    ID3D11ComputeShader* const computeShader = noScaling ? m_CasComputeShader_NoScaling : m_CasComputeShader;
    m_DeviceContext->CSSetShader(computeShader, nullptr, 0);

    ID3D11Buffer* cbPtr = m_ConstantBufferCAS;
    m_DeviceContext->CSSetConstantBuffers(0, 1, &cbPtr);
    m_DeviceContext->CSSetShaderResources(0, 1, &srcSrv);
    m_DeviceContext->CSSetUnorderedAccessViews(0, 1, &dstUav, nullptr);
    m_DeviceContext->Dispatch(CeilDiv(dstSize.x, 16u), CeilDiv(dstSize.y, 16u), 1);
}



