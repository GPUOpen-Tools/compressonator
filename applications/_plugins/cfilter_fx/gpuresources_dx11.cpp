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
#include "gpuresources_dx11.h"


GpuResources_dx11::GpuResources_dx11()
{
    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};

    D3D_FEATURE_LEVEL outFeatureLevel;
    CHECK_HR(D3D11CreateDevice(nullptr,                             // pAdapter
                               D3D_DRIVER_TYPE_HARDWARE,            // DriverType
                               NULL,                                // Software
                               D3D11_CREATE_DEVICE_SINGLETHREADED,  // Flags
                               featureLevels,                       // pFeatureLevels
                               _countof(featureLevels),             // FeatureLevels
                               D3D11_SDK_VERSION,                   // SDKVersion
                               &m_D3D11Device,                      // ppDevice
                               &outFeatureLevel,                    // pFeatureLevel
                               &m_DeviceContext));                  // ppImmediateContext
}

