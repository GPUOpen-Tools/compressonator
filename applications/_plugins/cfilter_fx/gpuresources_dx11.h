//=====================================================================
// Copyright 2021 (c), Advanced Micro Devices, Inc. All rights reserved.
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
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

#ifndef _GPURESOURCES_DX11_FILTERFX_H
#define _GPURESOURCES_DX11_FILTERFX_H

#include "plugininterface.h"

#include <Windows.h>
#include <atlbase.h>  // CComPtr
#include "d3d11.h"

#include <string>
#include <algorithm>
#include <exception>
#include <stdexcept>

#include <cstdio>
#include <cassert>

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define LINE_STRING STRINGIZE(__LINE__)

#define CHECK_HR(expr)                                                                   \
    do                                                                                   \
    {                                                                                    \
        if (FAILED(expr))                                                                \
            throw std::runtime_error(__FILE__ "(" LINE_STRING "): FAILED( " #expr " )"); \
    } while (false)

struct uvec2
{
    uint32_t x, y;
    uvec2()
    {
    }
    uvec2(uint32_t x_, uint32_t y_)
        : x(x_)
        , y(y_)
    {
    }
};

template <typename T>
inline T CeilDiv(T x, T y)
{
    return (x + y - 1) / y;
}

class GpuResources_dx11 
{
public:
    GpuResources_dx11();
    CComPtr<ID3D11Device>        m_D3D11Device;
    CComPtr<ID3D11DeviceContext> m_DeviceContext;

    CComPtr<ID3D11Buffer>        m_ConstantBufferCAS;
    CComPtr<ID3D11ComputeShader> m_CasComputeShader;
    CComPtr<ID3D11ComputeShader> m_CasComputeShader_NoScaling;

    CComPtr<ID3D11Buffer>        m_ConstantBufferFSR;
    CComPtr<ID3D11ComputeShader> m_ComputeShader_FSR_EASU;
    CComPtr<ID3D11ComputeShader> m_ComputeShader_FSR_BILINEAR;
    CComPtr<ID3D11ComputeShader> m_ComputeShader_FSR_RCAS;
};

#endif
