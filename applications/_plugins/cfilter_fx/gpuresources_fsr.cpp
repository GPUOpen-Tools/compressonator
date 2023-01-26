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

#include "gpuresources_fsr.h"

GpuResources_fsr::GpuResources_fsr()
{
}

void GpuResources_fsr::InitBuffers_fsr()
{
    //====================
    // FSR Constant buffer
    //====================

    CD3D11_BUFFER_DESC constantBufferDesc_FSR = CD3D11_BUFFER_DESC(sizeof(ConstantBufferStructureFSR), D3D11_BIND_CONSTANT_BUFFER);
    CHECK_HR(m_D3D11Device->CreateBuffer(&constantBufferDesc_FSR, nullptr, &m_ConstantBufferFSR));
}


void GpuResources_fsr::GpuCompileShaders_fsr(CMP_FORMAT format, bool useSRGB)
{

    //=====================================
    // FSR EASU
    //=====================================
    const BYTE* shaderCode_FSR;
    size_t      shaderCodeSize_FSR;

    GetShaderCode_FSR_EASU(shaderCode_FSR,shaderCodeSize_FSR);
    CHECK_HR(m_D3D11Device->CreateComputeShader(shaderCode_FSR, shaderCodeSize_FSR, nullptr, &m_ComputeShader_FSR_EASU));

    //=====================================
    // FSR BILINEAR
    //=====================================
    const BYTE* shaderCode_BILINEAR;
    size_t      shaderCodeSize_BILINEAR;

    GetShaderCode_FSR_BILINEAR(shaderCode_BILINEAR,shaderCodeSize_BILINEAR);
    CHECK_HR(m_D3D11Device->CreateComputeShader(shaderCode_BILINEAR, shaderCodeSize_BILINEAR, nullptr, &m_ComputeShader_FSR_BILINEAR));

    //=====================================
    // FSR RCAS
    //=====================================
    const BYTE* shaderCode_RCAS;
    size_t      shaderCodeSize_RCAS;

    GetShaderCode_FSR_RCAS(shaderCode_RCAS,shaderCodeSize_RCAS);
    CHECK_HR(m_D3D11Device->CreateComputeShader(shaderCode_RCAS, shaderCodeSize_RCAS, nullptr, &m_ComputeShader_FSR_RCAS));
}

void GpuResources_fsr::GetShaderCode_FSR_EASU(const BYTE*& outCode_FSR,
                                     size_t&      outCodeSize_FSR)
{
     outCode_FSR         = FSR_EASU::g_mainCS;
     outCodeSize_FSR     = _countof(FSR_EASU::g_mainCS);
}

void GpuResources_fsr::GetShaderCode_FSR_BILINEAR(const BYTE*& outCode_FSR,
                                     size_t&      outCodeSize_FSR)
{
     outCode_FSR         = FSR_BILINEAR::g_mainCS;
     outCodeSize_FSR     = _countof(FSR_BILINEAR::g_mainCS);
}

void GpuResources_fsr::GetShaderCode_FSR_RCAS(const BYTE*& outCode_FSR,
                                     size_t&      outCodeSize_FSR)
{
     outCode_FSR         = FSR_RCAS::g_mainCS;
     outCodeSize_FSR     = _countof(FSR_RCAS::g_mainCS);
}

void GpuResources_fsr::FSR_EASU(ID3D11UnorderedAccessView* dstUav, uvec2 dstSize, ID3D11ShaderResourceView* srcSrv, uvec2 srcSize) const
{
    ConstantBufferStructureFSR constBufStruct;

    FsrEasuCon(constBufStruct.const0,
               constBufStruct.const1,
               constBufStruct.const2,
               constBufStruct.const3,
               // This the rendered part of the input image.
               (float)srcSize.x, (float)srcSize.y, 
               // This is the size of the input image.
               (float)srcSize.x, (float)srcSize.y, 
               // This is the size of the output image.
               (float)dstSize.x, (float)dstSize.y);

    m_DeviceContext->UpdateSubresource(m_ConstantBufferFSR, 0, NULL, &constBufStruct, sizeof(constBufStruct), 0);

    ID3D11ComputeShader* const computeShader = m_ComputeShader_FSR_EASU;
    m_DeviceContext->CSSetShader(computeShader, nullptr, 0);

    ID3D11Buffer* cbPtr = m_ConstantBufferFSR;
    m_DeviceContext->CSSetConstantBuffers(0, 1, &cbPtr);
    m_DeviceContext->CSSetShaderResources(0, 1, &srcSrv);
    m_DeviceContext->CSSetUnorderedAccessViews(0, 1, &dstUav, nullptr);
    m_DeviceContext->Dispatch(CeilDiv(dstSize.x, 16u), CeilDiv(dstSize.y, 16u), 1);
}

void GpuResources_fsr::FSR_BILINEAR(ID3D11UnorderedAccessView* dstUav, uvec2 dstSize, ID3D11ShaderResourceView* srcSrv, uvec2 srcSize) const
{
    ConstantBufferStructureFSR constBufStruct;

    FsrEasuCon(constBufStruct.const0,
               constBufStruct.const1,
               constBufStruct.const2,
               constBufStruct.const3,
               // This the rendered part of the input image.
               (float)srcSize.x, (float)srcSize.y, 
               // This is the size of the input image.
               (float)srcSize.x, (float)srcSize.y, 
               // This is the size of the output image.
               (float)dstSize.x, (float)dstSize.y);

    m_DeviceContext->UpdateSubresource(m_ConstantBufferFSR, 0, NULL, &constBufStruct, sizeof(constBufStruct), 0);

    ID3D11ComputeShader* const computeShader = m_ComputeShader_FSR_BILINEAR;
    m_DeviceContext->CSSetShader(computeShader, nullptr, 0);

    ID3D11Buffer* cbPtr = m_ConstantBufferFSR;
    m_DeviceContext->CSSetConstantBuffers(0, 1, &cbPtr);
    m_DeviceContext->CSSetShaderResources(0, 1, &srcSrv);
    m_DeviceContext->CSSetUnorderedAccessViews(0, 1, &dstUav, nullptr);
    m_DeviceContext->Dispatch(CeilDiv(dstSize.x, 16u), CeilDiv(dstSize.y, 16u), 1);
}

void GpuResources_fsr::FSR_RCAS(bool hdr, float sharpness, ID3D11UnorderedAccessView* dstUav, uvec2 dstSize, ID3D11ShaderResourceView* srcSrv, uvec2 srcSize) const
{
    ConstantBufferStructureFSR constBufStruct = {0};

    FsrRcasCon(constBufStruct.const0,sharpness);
    if (hdr)
        constBufStruct.sample[0] = 1;

    m_DeviceContext->UpdateSubresource(m_ConstantBufferFSR, 0, NULL, &constBufStruct, sizeof(constBufStruct), 0);

    ID3D11ComputeShader* const computeShader = m_ComputeShader_FSR_RCAS;
    m_DeviceContext->CSSetShader(computeShader, nullptr, 0);

    ID3D11Buffer* cbPtr = m_ConstantBufferFSR;
    m_DeviceContext->CSSetConstantBuffers(0, 1, &cbPtr);
    m_DeviceContext->CSSetShaderResources(0, 1, &srcSrv);
    m_DeviceContext->CSSetUnorderedAccessViews(0, 1, &dstUav, nullptr);
    m_DeviceContext->Dispatch(CeilDiv(dstSize.x, 16u), CeilDiv(dstSize.y, 16u), 1);
}
