//=====================================================================
// Copyright (c) 2020    Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================

#include <stdio.h>
#include <stdlib.h>
#include "compressonator.h"
#include "common.h"
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "cgpuhw.h"
#include <stb_image.h>
CMIPS *GPU_HWMips = nullptr;

#define BUILD_AS_PLUGIN_DLL

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_CGpuHW)
SET_PLUGIN_TYPE("PIPELINE")
SET_PLUGIN_NAME("GPU_HW")
#else
void* make_Plugin_Compute_GpuHW() {
    return new Plugin_CGpuHW;
}
#endif

#pragma comment(lib,"advapi32.lib")        // for RegCloseKey and other Reg calls ...


Plugin_CGpuHW::Plugin_CGpuHW() {
    m_pComputeBase = NULL;
}

Plugin_CGpuHW::~Plugin_CGpuHW() {
    if (m_pComputeBase)
        delete m_pComputeBase;
}

int Plugin_CGpuHW::TC_PluginSetSharedIO(void* Shared) {
    if (!Shared) return 1;
    GPU_HWMips = reinterpret_cast<CMIPS *> (Shared);
    GPU_HWMips->m_infolevel = 0x01; // Turn on print Info
    return 0;
}

int Plugin_CGpuHW::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) {
    pPluginVersion->guid                    = g_GUID_GPU;
    pPluginVersion->dwAPIVersionMajor       = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor       = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_CGpuHW::TC_Init(void* kernel_options) {
    m_pComputeBase = (ComputeBase*)new CGpuHW(kernel_options);
    if (m_pComputeBase == NULL)
        return -1;
    return 0;
}

#ifdef ENABLE_MAKE_COMPATIBLE_API
bool Plugin_CGpuHW::IsFloatFormat(CMP_FORMAT InFormat) {
    switch (InFormat) {
    case CMP_FORMAT_ARGB_16F:
    case CMP_FORMAT_ABGR_16F:
    case CMP_FORMAT_RGBA_16F:
    case CMP_FORMAT_BGRA_16F:
    case CMP_FORMAT_RG_16F:
    case CMP_FORMAT_R_16F:
    case CMP_FORMAT_ARGB_32F:
    case CMP_FORMAT_ABGR_32F:
    case CMP_FORMAT_RGBA_32F:
    case CMP_FORMAT_BGRA_32F:
    case CMP_FORMAT_RGB_32F:
    case CMP_FORMAT_BGR_32F:
    case CMP_FORMAT_RG_32F:
    case CMP_FORMAT_R_32F:
    case CMP_FORMAT_BC6H:
    case CMP_FORMAT_BC6H_SF:
    case CMP_FORMAT_RGBE_32F: {
        return true;
    }
    break;
    default:
        break;
    }

    return false;
}

inline float clamp(float a, float l, float h) {
    return (a < l) ? l : ((a > h) ? h : a);
}

inline float knee(double x, double f) {
    return float(log(x * f + 1.f) / f);
}

float Plugin_CGpuHW::findKneeValueHPC(float x, float y) {
    float f0 = 0;
    float f1 = 1.f;

    while (knee(x, f1) > y) {
        f0 = f1;
        f1 = f1 * 2.f;
    }

    for (int i = 0; i < 30; ++i) {
        const float f2 = (f0 + f1) / 2.f;
        const float y2 = knee(x, f2);

        if (y2 < y) {
            f1 = f2;
        } else {
            f0 = f2;
        }
    }

    return (f0 + f1) / 2.f;
}

CMP_ERROR Plugin_CGpuHW::CF_16BitTo8Bit(CMP_WORD* sBlock, CMP_BYTE* cBlock, CMP_DWORD dwBlockSize) {
    assert(sBlock);
    assert(cBlock);
    assert(dwBlockSize);
    if (sBlock && cBlock && dwBlockSize) {
        for (CMP_DWORD i = 0; i < dwBlockSize; i++) {
            cBlock[i] =  (CMP_BYTE)(sBlock[i] / 257);
        }
    }

    return CMP_OK;
}

CMP_ERROR Plugin_CGpuHW::Byte2HalfShort(CMP_HALFSHORT* hfsBlock, CMP_BYTE* cBlock, CMP_DWORD dwBlockSize) {
    assert(hfsBlock);
    assert(cBlock);
    assert(dwBlockSize);
    if (hfsBlock && cBlock && dwBlockSize) {
        for (CMP_DWORD i = 0; i < dwBlockSize; i++) {
            hfsBlock[i] = CMP_HALF(float(cBlock[i] / 255.0f)).bits();
        }
    }

    return CMP_OK;
}

CMP_ERROR Plugin_CGpuHW::Float2Byte(CMP_BYTE cBlock[], CMP_FLOAT* fBlock, MipSet& srcTexture, const CMP_CompressOptions* pOptions) {
    assert(cBlock);
    assert(fBlock);
    assert(&srcTexture);

    if (cBlock && fBlock) {
        CMP_HALF* hfData = (CMP_HALF*)fBlock;
        float r = 0, g = 0, b = 0, a = 0;

        float kl = powf(2.f, pOptions->fInputKneeLow);
        float f = findKneeValueHPC(powf(2.f, pOptions->fInputKneeHigh) - kl, powf(2.f, 3.5f) - kl);
        float luminance3f = powf(2, -3.5);         // always assume max intensity is 1 and 3.5f darker for scale later
        float invGamma = 1 / pOptions->fInputGamma; //for gamma correction
        float scale = (float)255.0 * powf(luminance3f, invGamma);
        int i = 0;

        for (unsigned int y = 0; y < srcTexture.dwHeight; y++) {
            for (unsigned int x = 0; x < srcTexture.dwWidth; x++) {
                if (srcTexture.m_ChannelFormat == CF_Float16) {
                    r = (float)(*hfData);
                    hfData++;
                    g = (float)(*hfData);
                    hfData++;
                    b = (float)(*hfData);
                    hfData++;
                    a = (float)(*hfData);
                    hfData++;
                } else if (srcTexture.m_ChannelFormat == CF_Float32) {
                    r = (float)(*fBlock);
                    fBlock++;
                    g = (float)(*fBlock);
                    fBlock++;
                    b = (float)(*fBlock);
                    fBlock++;
                    a = (float)(*fBlock);
                    fBlock++;
                }

                CMP_BYTE r_b, g_b, b_b, a_b;


                //  1) Compensate for fogging by subtracting defog
                //     from the raw pixel values.
                // We assume a defog of 0
                if (pOptions->fInputDefog > 0.0) {
                    r = r - pOptions->fInputDefog;
                    g = g - pOptions->fInputDefog;
                    b = b - pOptions->fInputDefog;
                    a = a - pOptions->fInputDefog;
                }

                //  2) Multiply the defogged pixel values by
                //     2^(exposure + 2.47393).
                const float exposeScale = powf(2, pOptions->fInputExposure + 2.47393f);
                r = r * exposeScale;
                g = g * exposeScale;
                b = b * exposeScale;
                a = a * exposeScale;

                //  3) Values that are now 1.0 are called "middle gray".
                //     If defog and exposure are both set to 0.0, then
                //     middle gray corresponds to a raw pixel value of 0.18.
                //     In step 6, middle gray values will be mapped to an
                //     intensity 3.5 f-stops below the display's maximum
                //     intensity.

                //  4) Apply a knee function.  The knee function has two
                //     parameters, kneeLow and kneeHigh.  Pixel values
                //     below 2^kneeLow are not changed by the knee
                //     function.  Pixel values above kneeLow are lowered
                //     according to a logarithmic curve, such that the
                //     value 2^kneeHigh is mapped to 2^3.5.  (In step 6,
                //     this value will be mapped to the the display's
                //     maximum intensity.)
                if (r > kl) {
                    r = kl + knee(r - kl, f);
                }
                if (g > kl) {
                    g = kl + knee(g - kl, f);
                }
                if (b > kl) {
                    b = kl + knee(b - kl, f);
                }
                if (a > kl) {
                    a = kl + knee(a - kl, f);
                }

                //  5) Gamma-correct the pixel values, according to the
                //     screen's gamma.  (We assume that the gamma curve
                //     is a simple power function.)
                r = powf(r, invGamma);
                g = powf(g, invGamma);
                b = powf(b, invGamma);
                a = powf(a, pOptions->fInputGamma);

                //  6) Scale the values such that middle gray pixels are
                //     mapped to a frame buffer value that is 3.5 f-stops
                //     below the display's maximum intensity.
                r *= scale;
                g *= scale;
                b *= scale;
                a *= scale;

                r_b = (CMP_BYTE)clamp(r, 0.f, 255.f);
                g_b = (CMP_BYTE)clamp(g, 0.f, 255.f);
                b_b = (CMP_BYTE)clamp(b, 0.f, 255.f);
                a_b = (CMP_BYTE)clamp(a, 0.f, 255.f);
                cBlock[i] = r_b;
                i++;
                cBlock[i] = g_b;
                i++;
                cBlock[i] = b_b;
                i++;
                cBlock[i] = a_b;
                i++;
            }

        }

    }

    return CMP_OK;
}
#endif

CMP_ERROR Plugin_CGpuHW::TC_Compress(void* Options, MipSet& SrcTexture, MipSet& destTexture, CMP_Feedback_Proc pFeedback) {
    CMP_ERROR result = CMP_OK;

#ifdef ENABLE_MAKE_COMPATIBLE_API

    bool srcFloat;
    bool destFloat;

    srcFloat  = (SrcTexture.m_ChannelFormat == CF_Float16  || SrcTexture.m_ChannelFormat == CF_Float32) ? true : false;

    if (destTexture.m_format != CMP_FORMAT_Unknown)
        destFloat= IsFloatFormat(destTexture.m_format);
    else
        destFloat = (destTexture.m_ChannelFormat == CF_Float16 || destTexture.m_ChannelFormat == CF_Float32) ? true : false;

    bool newBuffer = false;

    // store original source data while ACF data is been processed
    CMP_BYTE    *hold_pData = NULL;
    CMP_DWORD   hold_dwDataSize = 0;
    CMP_FORMAT  hold_format = CMP_FORMAT_Unknown;

    if (srcFloat && !destFloat) {
        hold_pData      = SrcTexture.pData;
        hold_format     = SrcTexture.m_format;
        hold_dwDataSize = SrcTexture.dwDataSize;

        CMP_DWORD size = SrcTexture.dwWidth * SrcTexture.dwHeight;
        CMP_FLOAT*pfData = new CMP_FLOAT[SrcTexture.dwDataSize];
        memcpy(pfData, SrcTexture.pData, SrcTexture.dwDataSize);

        CMP_BYTE *byteData = new CMP_BYTE[size * 4];

        CMP_CompressOptions fDataOptions;
        fDataOptions.fInputDefog    = AMD_CODEC_DEFOG_DEFAULT;
        fDataOptions.fInputExposure = AMD_CODEC_EXPOSURE_DEFAULT;
        fDataOptions.fInputKneeLow  = AMD_CODEC_KNEELOW_DEFAULT;
        fDataOptions.fInputKneeHigh = AMD_CODEC_KNEEHIGH_DEFAULT;
        fDataOptions.fInputGamma    = AMD_CODEC_GAMMA_DEFAULT;
        Float2Byte(byteData, pfData, SrcTexture, &fDataOptions);

        delete[] pfData;
        SrcTexture.pData        = byteData;
        SrcTexture.m_format     = CMP_FORMAT_ARGB_8888;
        SrcTexture.dwDataSize   = size * 4;
        newBuffer = true;
    }

    else if (!srcFloat && destFloat) {
        // Process the current mip level data
        hold_pData      = SrcTexture.pData;
        hold_format     = SrcTexture.m_format;
        hold_dwDataSize = SrcTexture.dwDataSize;

        CMP_DWORD size = SrcTexture.dwWidth * SrcTexture.dwHeight;
        CMP_BYTE *pbData = SrcTexture.pData;
        CMP_HALFSHORT *hfloatData = new CMP_HALFSHORT[size * 4];
        Byte2HalfShort(hfloatData, pbData, size * 4);

        SrcTexture.pData        = (CMP_BYTE*)hfloatData;
        SrcTexture.m_format     = CMP_FORMAT_ARGB_16F;
        SrcTexture.dwDataSize   = size * 4 * 2;
        newBuffer = true;
    } else { // both src & dest are of type int
        // check if src format is 8 bit and dest is 8 bit if not convert src to match dest
        if ((SrcTexture.m_ChannelFormat == CF_16bit) && (destTexture.m_ChannelFormat == CF_Compressed)) {
            hold_pData      = SrcTexture.pData;
            hold_format     = SrcTexture.m_format;
            hold_dwDataSize = SrcTexture.dwDataSize;

            CMP_DWORD size = hold_dwDataSize/2;
            CMP_WORD  *pbData = (CMP_WORD  *)SrcTexture.pData;
            CMP_BYTE *sData = new CMP_BYTE[size];
            CF_16BitTo8Bit(pbData,sData, size);

            SrcTexture.pData        = (CMP_BYTE*)sData;
            SrcTexture.m_format     = CMP_FORMAT_ARGB_8888;
            SrcTexture.dwDataSize   = size;
            newBuffer = true;
        }

    }
#endif

    if (m_pComputeBase)
        result = m_pComputeBase->Compress((KernelOptions *)Options, SrcTexture,destTexture,pFeedback);

#ifdef ENABLE_MAKE_COMPATIBLE_API
    if (newBuffer) {
        // remove the new data
        free(SrcTexture.pData);
        // restore original data
        SrcTexture.pData        = hold_pData;
        SrcTexture.m_format     = hold_format;
        SrcTexture.dwDataSize   = hold_dwDataSize;
    }
#endif

    return result;
}

CMP_ERROR Plugin_CGpuHW::TC_GetPerformanceStats(void* pPerfStats) {
    CMP_ERROR result = CMP_ERR_NOPERFSTATS;
    if (m_pComputeBase) {
        KernelPerformanceStats *PerfStats =  reinterpret_cast<KernelPerformanceStats *>(pPerfStats);
        PerfStats->m_num_blocks  = m_pComputeBase->GetBlockSize();
        PerfStats->m_computeShaderElapsedMS = m_pComputeBase->GetProcessElapsedTimeMS();
        PerfStats->m_CmpMTxPerSec  = m_pComputeBase->GetMTxPerSec();
        result = CMP_OK;
    }
    return result;
}

CMP_ERROR Plugin_CGpuHW::TC_GetDeviceInfo(void* pDeviceInfo) {
    CMP_ERROR result = CMP_ERR_NOPERFSTATS;
    if (m_pComputeBase) {
        KernelDeviceInfo *DeviceInfo =  reinterpret_cast<KernelDeviceInfo *>(pDeviceInfo);
        snprintf(DeviceInfo->m_deviceName,sizeof(DeviceInfo->m_deviceName),"%s",m_pComputeBase->GetDeviceName());
        snprintf(DeviceInfo->m_version,sizeof(DeviceInfo->m_version),"%s",m_pComputeBase->GetVersion());
        DeviceInfo->m_maxUCores      = m_pComputeBase->GetMaxUCores();
        result = CMP_OK;
    }
    return result;
}

void Plugin_CGpuHW::TC_SetComputeOptions(void* options) {
    if (m_pComputeBase)
        m_pComputeBase->SetComputeOptions((ComputeOptions *)options);
}

char* Plugin_CGpuHW::TC_ComputeSourceFile() {
    return NULL;
}

int Plugin_CGpuHW::TC_Close() {
    if (m_pComputeBase) {
        delete m_pComputeBase;
        m_pComputeBase = NULL;
    }
    return 0;
}


