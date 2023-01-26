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

#ifndef _GPURESOURCES_FSR_FILTERFX_H
#define _GPURESOURCES_FSR_FILTERFX_H

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

#include "gpuresources_dx11.h"

#define A_CPU 1
#define USE_CMP_FIDELITY_FX_H
#include "common_def.h"
#include "ffx_fsr1.h"

namespace FSR_EASU
{
#include "./compiled/fsr_easu.h"
}

namespace FSR_BILINEAR
{
#include "./compiled/fsr_bilinear.h"
}

namespace FSR_RCAS
{
#include "./compiled/fsr_rcas.h"
}

class GpuResources_fsr : public GpuResources_dx11
{
public:
    GpuResources_fsr();
    void InitBuffers_fsr();
    void GpuCompileShaders_fsr(CMP_FORMAT format, bool useSRGB);

    void FSR_EASU(ID3D11UnorderedAccessView* dstUav, uvec2 dstSize, ID3D11ShaderResourceView* srcSrv, uvec2 srcSize) const;
    void FSR_BILINEAR(ID3D11UnorderedAccessView* dstUav, uvec2 dstSize, ID3D11ShaderResourceView* srcSrv, uvec2 srcSize) const;
    void FSR_RCAS(bool hdr, float sharpness, ID3D11UnorderedAccessView* dstUav, uvec2 dstSize, ID3D11ShaderResourceView* srcSrv, uvec2 srcSize) const;

private:

    struct ConstantBufferStructureFSR
    {
        varAU4(const0);
        varAU4(const1);
        varAU4(const2);
        varAU4(const3);
        varAU4(sample);
    };

    static void GetShaderCode_FSR_EASU(const BYTE*& outCode_FSR,size_t& outCodeSize_FSR);
    static void GetShaderCode_FSR_BILINEAR(const BYTE*& outCode_FSR,size_t& outCodeSize_FSR);
    static void GetShaderCode_FSR_RCAS(const BYTE*& outCode_FSR,size_t& outCodeSize_FSR);
};


#endif
