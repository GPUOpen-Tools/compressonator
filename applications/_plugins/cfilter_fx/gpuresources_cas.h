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

#ifndef _GPURESOURCES_CAS_FILTERFX_H
#define _GPURESOURCES_CAS_FILTERFX_H

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
#include "ffx_cas.h"

namespace NoScaling0_Linear0
{
#include "./compiled/CompiledShader_NoScaling0_Linear0.h"
}
namespace NoScaling1_Linear0
{
#include "./compiled/CompiledShader_NoScaling1_Linear0.h"
}
namespace NoScaling0_Linear1
{
#include "./compiled/CompiledShader_NoScaling0_Linear1.h"
}
namespace NoScaling1_Linear1
{
#include "./compiled/CompiledShader_NoScaling1_Linear1.h"
}

#ifdef USE_FSR
#include "gpuresources_fsr.h"
#endif

class GpuResources_cas: public GpuResources_fsr
{
public:
    GpuResources_cas();
    void InitBuffers_cas();
    void GpuCompileShaders_cas(CMP_FORMAT format, bool useSRGB);

    void CAS(float sharpness, ID3D11UnorderedAccessView* dstUav, uvec2 dstSize, ID3D11ShaderResourceView* srcSrv, uvec2 srcSize);

private:
    struct ConstantBufferStructureCAS
    {
        varAU4(const0);
        varAU4(const1);
    };

    static void GetShaderCode_CAS(  const BYTE*& outCode_NoScaling0,
                                    size_t&      outCodeSize_NoScaling0,
                                    const BYTE*& outCode_NoScaling1,
                                    size_t&      outCodeSize_NoScaling1,
                                    CMP_FORMAT   format,
                                    bool         useSRGB);

};


#endif
