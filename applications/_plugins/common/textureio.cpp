//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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
/// \file TextureIO.cpp
/// \version 2.20
//
//=====================================================================

#include "textureio.h"

#include "compressonator.h"
#include "common.h"
#include "cmp_fileio.h"
#include "pluginmanager.h"
#include "plugininterface.h"

#include <gpu_decode.h>

// #if defined(WIN32) && !defined(NO_LEGACY_BEHAVIOR)
// #define OPTION_CMP_QT
// #endif

#if (OPTION_CMP_QT == 1)
#include <mipstoqimage.h>
#include <QtCore/QCoreApplication>
#include <QtGui/qimage.h>
#include <QtGui/qrgb.h>

#ifdef _DEBUG
#pragma comment(lib, "Qt5Cored.lib")
#pragma comment(lib, "Qt5Guid.lib")
#else
#pragma comment(lib, "Qt5Core.lib")
#pragma comment(lib, "Qt5Gui.lib")
#endif

#endif // CMP_USE_QT

#include <algorithm>
#include <iostream>
#include <string>
#include <locale>
#include <cstdio>

using namespace std;


// Global plugin manager instance
extern PluginManager g_pluginManager;
extern bool g_bAbortCompression;

inline CMP_FLOAT clamp(CMP_FLOAT a, CMP_FLOAT l, CMP_FLOAT h) {
    return (a < l) ? l : ((a > h) ? h : a);
}


void astc_find_closest_blockdim_2d(float target_bitrate, int *x, int *y, int consider_illegal) {
    int blockdims[6] = { 4, 5, 6, 8, 10, 12 };

    float best_error = 1000;
    float aspect_of_best = 1;
    int i, j;

    // Y dimension
    for (i = 0; i < 6; i++) {
        // X dimension
        for (j = i; j < 6; j++) {
            //              NxN       MxN         8x5               10x5              10x6
            int is_legal = (j == i) || (j == i + 1) || (j == 3 && j == 1) || (j == 4 && j == 1) || (j == 4 && j == 2);

            if (consider_illegal || is_legal) {
                float bitrate = 128.0f / (blockdims[i] * blockdims[j]);
                float bitrate_error = fabs(bitrate - target_bitrate);
                float aspect = (float)blockdims[j] / blockdims[i];
                if (bitrate_error < best_error || (bitrate_error == best_error && aspect < aspect_of_best)) {
                    *x = blockdims[j];
                    *y = blockdims[i];
                    best_error = bitrate_error;
                    aspect_of_best = aspect;
                }
            }
        }
    }
}

void astc_find_closest_blockxy_2d(int *x, int *y, int consider_illegal) {
    (void)consider_illegal;

    int blockdims[6] = { 4, 5, 6, 8, 10, 12 };

    bool exists_x = std::find(std::begin(blockdims), std::end(blockdims), (*x)) != std::end(blockdims);
    bool exists_y = std::find(std::begin(blockdims), std::end(blockdims), (*y)) != std::end(blockdims);

    if (exists_x && exists_y) {
        if ((*x) < (*y)) {
            int temp = *x;
            *x = *y;
            *y = temp;
        }
        float bitrateF = float(128.0f / ((*x)*(*y)));
        astc_find_closest_blockdim_2d(bitrateF, x, y, 0);
    } else {
        float bitrateF = float(128.0f / ((*x)*(*y)));
        astc_find_closest_blockdim_2d(bitrateF, x, y, 0);
    }

}

bool DeCompressionCallback(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
    UNREFERENCED_PARAMETER(pUser1);
    UNREFERENCED_PARAMETER(pUser2);

    static float Progress = 0;
    if (fProgress > Progress)
        Progress = fProgress;
    PrintInfo("\rDeCompression progress = %3.0f", Progress);
    return g_bAbortCompression;
}

bool IsDestinationUnCompressed(const char *fname) {
    bool isuncompressed = true;
    std::string file_extension = CMP_GetJustFileExt(fname);

    // tolower
    for(char& c : file_extension)
        c =tolower(c);

    if (file_extension.compare(".dds") == 0) {
        isuncompressed = false;
    }
    else if (file_extension.compare(".astc") == 0) {
        isuncompressed = false;
    }
    else if (file_extension.compare(".ktx") == 0) {
        isuncompressed = false;
    } 
    else if (file_extension.compare(".ktx2") == 0)
    {
        isuncompressed = false;
    }
    else if (file_extension.compare(".raw") == 0)
    {
        isuncompressed = false;
    } else if (file_extension.compare(".basis") == 0) {
        isuncompressed = false;
    }
#ifdef USE_CRN
    else if (file_extension.compare(".crn") == 0) {
        isuncompressed = false;
    }
#endif

    return isuncompressed;
}

CMP_FORMAT FormatByFileExtension(const char *fname, MipSet *pMipSet) {
    std::string file_extension = CMP_GetJustFileExt(fname);

    // To upper
    std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(),
                   [](unsigned char c) -> unsigned char { return tolower(c); });

    pMipSet->m_TextureDataType = TDT_ARGB;

    if (file_extension.compare(".exr") == 0) {
        pMipSet->m_ChannelFormat = CF_Float16;
        return CMP_FORMAT_ARGB_16F;
    }

    pMipSet->m_ChannelFormat = CF_8bit;
    return CMP_FORMAT_ARGB_8888;
}

CMP_FORMAT GetFormat(CMP_DWORD dwFourCC) {
    switch(dwFourCC) {
    case CMP_FOURCC_ATI1N:              return CMP_FORMAT_ATI1N;
    case CMP_FOURCC_ATI2N:              return CMP_FORMAT_ATI2N;
    case CMP_FOURCC_ATI2N_XY:           return CMP_FORMAT_ATI2N_XY;
    case CMP_FOURCC_ATI2N_DXT5:         return CMP_FORMAT_ATI2N_DXT5;
    case CMP_FOURCC_DXT1:               return CMP_FORMAT_DXT1;
    case CMP_FOURCC_DXT3:               return CMP_FORMAT_DXT3;
    case CMP_FOURCC_DXT5:               return CMP_FORMAT_DXT5;
    case CMP_FOURCC_DXT5_xGBR:          return CMP_FORMAT_DXT5_xGBR;
    case CMP_FOURCC_DXT5_RxBG:          return CMP_FORMAT_DXT5_RxBG;
    case CMP_FOURCC_DXT5_RBxG:          return CMP_FORMAT_DXT5_RBxG;
    case CMP_FOURCC_DXT5_xRBG:          return CMP_FORMAT_DXT5_xRBG;
    case CMP_FOURCC_DXT5_RGxB:          return CMP_FORMAT_DXT5_RGxB;
    case CMP_FOURCC_DXT5_xGxR:          return CMP_FORMAT_DXT5_xGxR;

    // Deprecated but still supported for decompression
    // Some definition are not valid FOURCC values nut are used as Custom formats
    // so that DDS files can be used for storage
    case CMP_FOURCC_DXT5_GXRB:          return CMP_FORMAT_DXT5_xRBG;
    case CMP_FOURCC_DXT5_GRXB:          return CMP_FORMAT_DXT5_RxBG;
    case CMP_FOURCC_DXT5_RXGB:          return CMP_FORMAT_DXT5_xGBR;
    case CMP_FOURCC_DXT5_BRGX:          return CMP_FORMAT_DXT5_RGxB;

    case CMP_FOURCC_ATC_RGB:            return CMP_FORMAT_ATC_RGB;
    case CMP_FOURCC_ATC_RGBA_EXPLICIT:  return CMP_FORMAT_ATC_RGBA_Explicit;
    case CMP_FOURCC_ATC_RGBA_INTERP:    return CMP_FORMAT_ATC_RGBA_Interpolated;
    case CMP_FOURCC_ETC_RGB:            return CMP_FORMAT_ETC_RGB;
    case CMP_FOURCC_ETC2_RGB:           return CMP_FORMAT_ETC2_RGB;
    case CMP_FOURCC_ETC2_SRGB:          return CMP_FORMAT_ETC2_SRGB;
    case CMP_FOURCC_ETC2_RGBA:          return CMP_FORMAT_ETC2_RGBA;
    case CMP_FOURCC_ETC2_RGBA1:         return CMP_FORMAT_ETC2_RGBA1;
    case CMP_FOURCC_ETC2_SRGBA:         return CMP_FORMAT_ETC2_SRGBA;
    case CMP_FOURCC_ETC2_SRGBA1:        return CMP_FORMAT_ETC2_SRGBA1;
    case CMP_FOURCC_BC4S:               return CMP_FORMAT_BC4_S;
    case CMP_FOURCC_BC4:
    case CMP_FOURCC_BC4U:               return CMP_FORMAT_ATI1N;  
    case CMP_FOURCC_BC5:                return CMP_FORMAT_BC5;
    case CMP_FOURCC_BC5S:               return CMP_FORMAT_BC5_S;
    case CMP_FOURCC_BC6H:               return CMP_FORMAT_BC6H;
    case CMP_FOURCC_BC7:                return CMP_FORMAT_BC7;
    case CMP_FOURCC_ASTC:               return CMP_FORMAT_ASTC;
#ifdef USE_APC
    case CMP_FOURCC_APC:                return CMP_FORMAT_APC;
#endif
#ifdef USE_GTC
    case CMP_FOURCC_GTC:                return CMP_FORMAT_GTC;
#endif
#ifdef USE_BASIS
    case CMP_FOURCC_BASIS:              return CMP_FORMAT_BASIS;
#endif
    default:
        return CMP_FORMAT_Unknown;
    }
}



CMP_FORMAT GetFormat(MipSet* pMipSet) {
    assert(pMipSet);
    if(pMipSet == NULL)
        return CMP_FORMAT_Unknown;

    switch(pMipSet->m_ChannelFormat) {
    case CF_8bit:
        switch(pMipSet->m_TextureDataType) {
        case TDT_R:
            return CMP_FORMAT_R_8;
        case TDT_RG:
            return CMP_FORMAT_RG_8;
        default:
            return CMP_FORMAT_ARGB_8888;
        }
    case CF_Float16:
        switch(pMipSet->m_TextureDataType) {
        case TDT_R:
            return CMP_FORMAT_R_16F;
        case TDT_RG:
            return CMP_FORMAT_RG_16F;
        default:
            return CMP_FORMAT_ARGB_16F;
        }
    case CF_Float32:
        switch(pMipSet->m_TextureDataType) {
        case TDT_R:
            return CMP_FORMAT_R_32F;
        case TDT_RG:
            return CMP_FORMAT_RG_32F;
        default:
            return CMP_FORMAT_ARGB_32F;
        }
    case CF_Float9995E:
        return CMP_FORMAT_RGBE_32F;

    case CF_Compressed:
        return GetFormat(pMipSet->m_dwFourCC2 ? pMipSet->m_dwFourCC2 : pMipSet->m_dwFourCC);
    case CF_16bit:
        switch(pMipSet->m_TextureDataType) {
        case TDT_R:
            return CMP_FORMAT_R_16;
        case TDT_RG:
            return CMP_FORMAT_RG_16;
        default:
            return CMP_FORMAT_ARGB_16;
        }
    case CF_2101010:
        return CMP_FORMAT_ARGB_2101010;

#ifdef ARGB_32_SUPPORT
    case CF_32bit:
        switch(pMipSet->m_TextureDataType) {
        case TDT_R:
            return CMP_FORMAT_R_32;
        case TDT_RG:
            return CMP_FORMAT_RG_32;
        default:
            return CMP_FORMAT_ARGB_32;
        }
#endif // ARGB_32_SUPPORT

    default:
        return CMP_FORMAT_Unknown;
    }
}


bool FloatFormat(CMP_FORMAT InFormat) {
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


bool CompressedFileFormat(std::string file) {
    std::string file_extension = CMP_GetJustFileExt(file);
    // To upper
    std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(),
                   [](unsigned char c) -> unsigned char { return toupper(c); });
    file_extension.erase(std::remove(file_extension.begin(), file_extension.end(), '.'), file_extension.end());

    if (file_extension == "BMP")
        return false;
    else
        return true;
}


//
// Used exclusively by the GUI app: when using CPU/GPU based decode
// ToDo : Remove this code and try to use ProcessCMDLine
MipSet* DecompressMIPSet(MipSet *MipSetIn, CMP_GPUDecode decodeWith, Config *configSetting, CMP_Feedback_Proc pFeedbackProc) {
    // validate MipSet is Compressed
    if (!MipSetIn->m_compressed) return NULL;
    if ((MipSetIn->m_TextureType == TT_CubeMap) && !(configSetting->useCPU)) {
        configSetting->errMessage = "GPU based cubemap decode is currently not supported in this version.Please view decode images using CPU (under Settings->Application Options)";
        PrintInfo("GPU based cubemap decode is currently not supported in this version. Please view decode images using CPU (under Settings->Application Options)\n");
        return NULL;
    }
    if (MipSetIn->m_format == CMP_FORMAT_ASTC && !(configSetting->useCPU) && decodeWith == CMP_GPUDecode::GPUDecode_DIRECTX) {
        configSetting->errMessage = "ASTC format does not supported by DirectX API. Please view ASTC compressed images using other options (CPU) (under Settings->Application Options).";
        PrintInfo("Decompress Error: ASTC format does not supported by DirectX API. Please view ASTC compressed images using CPU (under Settings->Application Options).\n");
        return NULL;
    } else if (MipSetIn->m_format == CMP_FORMAT_ASTC && !(configSetting->useCPU) && decodeWith == CMP_GPUDecode::GPUDecode_OPENGL) {
        configSetting->errMessage = "Decode ASTC with OpenGL is not supported. Please view ASTC compressed images using other options (CPU) (under Settings->Application Options).";
        PrintInfo("Decompress Error: Decode ASTC with OpenGL is not supported. Please view ASTC compressed images using CPU (under Settings->Application Options).\n");
        return NULL;
    }

    // Compress Options
    bool silent = true;
    CMP_CompressOptions    CompressOptions;
    memset(&CompressOptions, 0, sizeof(CMP_CompressOptions));
    CompressOptions.dwnumThreads = 0;
    CMIPS m_CMIPS;

    MipSet    *MipSetOut = new MipSet();
    memset(MipSetOut, 0, sizeof(MipSet));

    MipSetOut->m_TextureDataType = TDT_ARGB;
    MipSetOut->m_swizzle = false;
    MipSetOut->m_CubeFaceMask = MipSetIn->m_CubeFaceMask;
    MipSetOut->m_Flags = MipSetIn->m_Flags;
    MipSetOut->m_nDepth = MipSetIn->m_nDepth;
    MipSetOut->m_nMaxMipLevels = MipSetIn->m_nMaxMipLevels;
    MipSetOut->m_nHeight = MipSetIn->m_nHeight;
    MipSetOut->m_nWidth = MipSetIn->m_nWidth;

    // BMP is saved as CMP_FORMAT_ARGB_8888
    // EXR is saved as CMP_FORMAT_ARGB_16F
    switch (MipSetIn->m_format) {
    case CMP_FORMAT_BC4_S:
    case CMP_FORMAT_BC5_S:
        MipSetOut->m_format = CMP_FORMAT_RGBA_8888_S;
        break;
    case CMP_FORMAT_BC6H:
    case CMP_FORMAT_BC6H_SF:
        MipSetOut->m_format = CMP_FORMAT_ARGB_16F;
        MipSetOut->m_ChannelFormat = CF_Float16;
        break;
    default:
        MipSetOut->m_format = CMP_FORMAT_ARGB_8888;
        break;
    }

    // Allocate output MipSet
    if (!m_CMIPS.AllocateMipSet(MipSetOut,
                                MipSetOut->m_ChannelFormat,
                                MipSetOut->m_TextureDataType,
                                MipSetIn->m_TextureType,
                                MipSetIn->m_nWidth,
                                MipSetIn->m_nHeight,
                                MipSetIn->m_nDepth)) {          // depthsupport, what should nDepth be set as here?
        configSetting->errMessage = "Memory Error(2): allocating MIPSet Output buffer.";
        PrintInfo("Memory Error(2): allocating MIPSet Output buffer\n");
        m_CMIPS.FreeMipSet(MipSetOut);
        delete MipSetOut;
        MipSetOut = NULL;
        return NULL;
    }

    MipLevel* pCmpMipLevel = m_CMIPS.GetMipLevel(MipSetIn, 0);
    int nMaxFaceOrSlice    = CMP_MaxFacesOrSlices(MipSetIn, 0);
    int nMaxMipLevels      = MipSetIn->m_nMipLevels;
    int nWidth = pCmpMipLevel->m_nWidth;
    int nHeight = pCmpMipLevel->m_nHeight;

#ifdef USE_BASIS
    if (MipSetIn->m_format == CMP_FORMAT_BASIS) {
        // These are handled by basis structure: for compressonator use only top most mip levels
        nMaxFaceOrSlice = 1;
        nMaxMipLevels   = 1;
    }
#endif

    CMP_BYTE* pMipData = m_CMIPS.GetMipLevel(MipSetIn, 0, 0)->m_pbData;

    for (int nFaceOrSlice = 0; nFaceOrSlice<nMaxFaceOrSlice; nFaceOrSlice++) {
        int nMipWidth = nWidth;
        int nMipHeight = nHeight;

        for (int nMipLevel = 0; nMipLevel< nMaxMipLevels; nMipLevel++) {
            MipLevel* pInMipLevel = m_CMIPS.GetMipLevel(MipSetIn, nMipLevel, nFaceOrSlice);
            if (!pInMipLevel) {
                configSetting->errMessage = "Memory Error(2): allocating MIPSet Output Cmp level buffer";
                PrintInfo("Memory Error(2): allocating MIPSet Output Cmp level buffer\n");
                m_CMIPS.FreeMipSet(MipSetOut);
                delete MipSetOut;
                MipSetOut = NULL;
                return NULL;
            }

            // Valid Mip Level ?
            if (pInMipLevel->m_pbData)
                pMipData = pInMipLevel->m_pbData;

            if (!m_CMIPS.AllocateMipLevelData(m_CMIPS.GetMipLevel(MipSetOut, nMipLevel, nFaceOrSlice), nMipWidth,
                                              nMipHeight, MipSetOut->m_ChannelFormat, MipSetOut->m_TextureDataType)) {
                configSetting->errMessage = "Memory Error(2): allocating MIPSet Output level buffer.";
                PrintInfo("Memory Error(2): allocating MIPSet Output level buffer\n");
                m_CMIPS.FreeMipSet(MipSetOut);
                delete MipSetOut;
                MipSetOut = NULL;
                return NULL;
            }

            //----------------------------
            // Compressed source
            //-----------------------------
            CMP_Texture srcTexture;
            srcTexture.dwSize = sizeof(srcTexture);
            srcTexture.dwWidth = nMipWidth;
            srcTexture.dwHeight = nMipHeight;
            srcTexture.dwPitch = 0;
            srcTexture.nBlockWidth = MipSetIn->m_nBlockWidth;
            srcTexture.nBlockHeight = MipSetIn->m_nBlockHeight;
            srcTexture.nBlockDepth = MipSetIn->m_nBlockDepth;
            srcTexture.format = MipSetIn->m_format;
            srcTexture.transcodeFormat =  MipSetIn->m_transcodeFormat;
            srcTexture.dwDataSize = CMP_CalculateBufferSize(&srcTexture);
            srcTexture.pData = pMipData;
            srcTexture.pMipSet         = MipSetIn;

            //-----------------------------
            // Uncompressed Destination
            //-----------------------------
            CMP_Texture destTexture;
            destTexture.dwSize = sizeof(destTexture);
            destTexture.dwWidth = nMipWidth;
            destTexture.dwHeight = nMipHeight;
            destTexture.dwPitch = 0;
            destTexture.nBlockWidth = MipSetOut->m_nBlockWidth;
            destTexture.nBlockHeight = MipSetOut->m_nBlockHeight;
            destTexture.nBlockDepth = MipSetOut->m_nBlockDepth;
            destTexture.format = MipSetOut->m_format;
            destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
            destTexture.pData = m_CMIPS.GetMipLevel(MipSetOut, nMipLevel, nFaceOrSlice)->m_pbData;
            destTexture.pMipSet      = MipSetOut;

            if (!silent) {
                if ((nMipLevel > 1) || (nFaceOrSlice > 1))
                    PrintInfo("\rProcessing destination MipLevel %2d FaceOrSlice %2d", nMipLevel + 1, nFaceOrSlice);
                else
                    PrintInfo("\rProcessing destination    ");
            }

            try {
#ifdef _WIN32
                if ((IsBadReadPtr(srcTexture.pData, srcTexture.dwDataSize))) {
                    configSetting->errMessage = "Memory Error(2): Source image cannot be accessed.";
                    PrintInfo("Memory Error(2): Source image\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }

                if (/*(srcTexture.dwDataSize > destTexture.dwDataSize) ||*/ (IsBadWritePtr(destTexture.pData, destTexture.dwDataSize))) {
                    configSetting->errMessage = "Memory Error(2): Destination image must be compatible with source.";
                    PrintInfo("Memory Error(2): Destination image must be compatible with source\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }

#else
                FILE *nullfd = std::fopen("/dev/random", "w");
                if (std::fwrite(srcTexture.pData, srcTexture.dwDataSize, 1, nullfd) < 0) {
                    configSetting->errMessage = "Memory Error(2): Source image cannot be accessed.";
                    PrintInfo("Memory Error(2): Source image\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }
                std::fclose(nullfd);
                nullfd = std::fopen("/dev/random", "w");
                if (std::fwrite(destTexture.pData, destTexture.dwDataSize, 1, nullfd) < 0) {
                    configSetting->errMessage = "Memory Error(2): Destination image must be compatible with source.";
                    PrintInfo("Memory Error(2): Destination image must be compatible with source\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }
                std::fclose(nullfd);
#endif

                // Return values of the CMP_ calls should be checked for failures
                CMP_ERROR res;
                if (configSetting->useCPU) {
                    res = CMP_ConvertTexture(&srcTexture, &destTexture, &CompressOptions, pFeedbackProc);
                    if (res != CMP_OK) {
                        configSetting->errMessage = "Compress Failed with Error " + res;
                        PrintInfo("Compress Failed with Error %d\n", res);
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    }
                } else {
#ifdef _WIN32
                    CMP_ERROR res;
                    CMP_FORMAT hold_destformat = destTexture.format;
                    res = CMP_DecompressTexture(&srcTexture, &destTexture, decodeWith);

                    // Did decompress adjust any formats, if so make sure  MipSetOut->m_format is updated
                    // This typically can happen when GPU views are used and the captured framebuffer differs 
                    // from that which was set as a destination texture format. Note all buffer sizes and channel formats
                    // should match when this happens. Typical case can be moving from SNORM to UNORM bytes
                    if (hold_destformat != destTexture.format)
                        MipSetOut->m_format = destTexture.format;


                    if (res == CMP_ERR_UNSUPPORTED_GPU_ASTC_DECODE) {
                        configSetting->errMessage = "Error: ASTC compressed texture is not supported by the GPU device.\n";
                        PrintInfo("Error: ASTC compressed texture is not supported by the GPU device.\n");
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    } else if (res == CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB) {
                        configSetting->errMessage = "Error: Failed to load decompress lib for this image or view.\n";
                        PrintInfo("Error: Failed to decompress with the API selected.\n");
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    } else if (res != CMP_OK) {
                        configSetting->errMessage = "Decompress Failed. Texture format not supported. Please view the compressed images using other options (CPU) (under Settings->Application Options)";
                        PrintInfo("Decompress Failed with Error %d\n", res);
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    }
#else
                    PrintInfo("GPU Decompress is not supported in linux.\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
#endif
                }


            } catch (std::exception& e) {
                PrintInfo(e.what());
                m_CMIPS.FreeMipSet(MipSetOut);
                delete MipSetOut;
                MipSetOut = NULL;
                return NULL;
            }

            pMipData += srcTexture.dwDataSize;

            nMipWidth = (nMipWidth>1) ? (nMipWidth >> 1) : 1;
            nMipHeight = (nMipHeight>1) ? (nMipHeight >> 1) : 1;
        }
    }

    MipSetOut->m_nMipLevels = MipSetIn->m_nMipLevels;

    return MipSetOut;
}

void swap_Bytes(CMP_BYTE *src, int width, int height, int offset) {
    int  i, j;
    CMP_BYTE b;

    for (i = 0; i<height; i++) {
        for (j = 0; j<width; j++) {
            b = *src;         // hold 1st byte
            *src = *(src + 2);     // move 1st to offsetrd
            *(src + 2) = b;            // save offset to 1st
            src = src + offset;   // move to next set of bytes
        }
    }

}

void SwizzleMipMap(MipSet *pMipSet) {
    CMP_DWORD dwWidth;
    CMP_DWORD dwHeight;
    CMP_BYTE     *pData;

    for (int nMipLevel = 0; nMipLevel<pMipSet->m_nMipLevels; nMipLevel++) {
        for (int nFaceOrSlice = 0; nFaceOrSlice< CMP_MaxFacesOrSlices(pMipSet, nMipLevel); nFaceOrSlice++) {
            //=====================
            // Uncompressed source
            //======================
            MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(pMipSet, nMipLevel, nFaceOrSlice);
            dwWidth = pInMipLevel->m_nWidth;
            dwHeight = pInMipLevel->m_nHeight;
            pData = pInMipLevel->m_pbData;

            // Swizzle to RGBA format when compressing from uncompressed DDS file! This is a Patch for now.
            // may want to try this patch on other file types BMP & PNG to move swizzle out to main code.
            switch (pMipSet->m_TextureDataType) {
            case TDT_ARGB:
                swap_Bytes(pInMipLevel->m_pbData, dwWidth, dwHeight, 4);
                break;
            case TDT_XRGB:
                swap_Bytes(pInMipLevel->m_pbData, dwWidth, dwHeight, 3);
                break;
            default:
                break;
            }
        }
    }

}

// Determine if RGB channel to BGA can be done or skipped
// for special cases of compressed formats.

bool KeepSwizzle(CMP_FORMAT destformat) {
    // determin of the swizzle flag needs to be turned on!
    switch (destformat) {
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_BC4_S:
    case CMP_FORMAT_ATI1N:  // same as BC4
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_BC5_S:
    case CMP_FORMAT_ATI2N:         // same as BC5  channels swizzled to : Green & Red
    case CMP_FORMAT_ATI2N_XY:      // BC5  Red & Green Channel
    case CMP_FORMAT_ATI2N_DXT5:    //
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:        // same as BC1
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_DXT3:        // same as BC2
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_DXT5:        // same as BC3
    case CMP_FORMAT_ATC_RGB:
    case CMP_FORMAT_ATC_RGBA_Explicit:
    case CMP_FORMAT_ATC_RGBA_Interpolated:
        return true;
        break;
    default:
        break;
    }

    return false;
}

int AMDLoadMIPSTextureImage(const char *SourceFile, MipSet *MipSetIn, bool use_OCV, void *pluginManager) {
    if (pluginManager == NULL)
        return -1;

    PluginInterface_Image *plugin_Image;

    PluginManager *plugin_Manager = (PluginManager *)pluginManager;
    if (use_OCV) {
        plugin_Image = reinterpret_cast<PluginInterface_Image *>(plugin_Manager->GetPlugin("IMAGE", "OCV"));
    } else {
        std::string file_extension = CMP_GetFileExtension(SourceFile, false, true);
        plugin_Image = reinterpret_cast<PluginInterface_Image*>(plugin_Manager->GetPlugin("IMAGE", (char*)file_extension.c_str()));
    }

    // do the load
    if (plugin_Image) {
        plugin_Image->TC_PluginSetSharedIO(&g_CMIPS);

        if (plugin_Image->TC_PluginFileLoadTexture(SourceFile, MipSetIn) != 0) {
            // Process Error
            delete plugin_Image;
            plugin_Image = NULL;
            return -1;
        }

        delete plugin_Image;
        plugin_Image = NULL;
    } else {

#if (OPTION_CMP_QT == 1)
        // Failed to load using a AMD Plugin
        // Try Qt based
        int result = -1;
        QString filename(SourceFile);
        QImage *qimage = new QImage(filename);
        if (qimage) {
            result = QImage2MIPS(qimage, g_CMIPS, MipSetIn);
            delete qimage;
            qimage = NULL;
        }

        return result;
#else
        return -1;
#endif
    }

    return 0;
}

int AMDSaveMIPSTextureImage(const char *DestFile, MipSet *MipSetIn, bool use_OCV, CMP_CompressOptions option) {
    bool filesaved = false;
    CMIPS m_CMIPS;

    std::string file_extension = CMP_GetJustFileExt(DestFile);
    file_extension.erase(std::remove(file_extension.begin(), file_extension.end(), '.'), file_extension.end());
    for(char& c : file_extension)
        c = toupper(c);

    PluginInterface_Image *plugin_Image;

    if (use_OCV) {
        plugin_Image = reinterpret_cast<PluginInterface_Image *>(g_pluginManager.GetPlugin("IMAGE", "OCV"));
    } else {
        plugin_Image = reinterpret_cast<PluginInterface_Image *>(g_pluginManager.GetPlugin("IMAGE", (char *)file_extension.c_str()));
    }

    if (plugin_Image) {
        plugin_Image->TC_PluginSetSharedIO(&m_CMIPS);

        bool holdswizzle = MipSetIn->m_swizzle;

        if (file_extension.compare("TGA") == 0) {
            // Special case Patch for TGA plugin
            // to be fixed in V2.5 release
            MipSetIn->m_swizzle = false;
            switch (MipSetIn->m_isDeCompressed) {
            case CMP_FORMAT_ASTC:
            case CMP_FORMAT_BC7:
            case CMP_FORMAT_BC6H:
            case CMP_FORMAT_BC6H_SF:
            case CMP_FORMAT_ETC_RGB:
            case CMP_FORMAT_ETC2_RGB:
                MipSetIn->m_swizzle = true;
                break;
            }

            if ((MipSetIn->m_ChannelFormat == CF_Float32) || (MipSetIn->m_ChannelFormat == CF_Float16)) {
                PrintInfo("\nError: TGA plugin does not support floating point data saving. Please use other file extension (i.e. dds).\n");
            }
        }

        if (plugin_Image->TC_PluginFileSaveTexture(DestFile, MipSetIn) == 0) {
            filesaved = true;
        }

        MipSetIn->m_swizzle = holdswizzle;

        delete plugin_Image;
        plugin_Image = NULL;
    }


#if (OPTION_CMP_QT == 1)
    if (!filesaved)
    {
        // Try Qt based filesave!
        QImage *qimage = MIPS2QImage(&m_CMIPS, MipSetIn, 0, 0, option, nullptr);

        if (qimage) {
            if (!qimage->save(DestFile)) {
                delete qimage;
                qimage = NULL;
                return(-1);
            }
            delete qimage;
            qimage = NULL;
            filesaved = true;
        } else
            return -1;
    }
#endif

    if (!filesaved) {
        return -1;
    }

    return 0;
}


bool FormatSupportsQualitySetting(CMP_FORMAT format) {
    return CMP_IsCompressedFormat(format);
}

bool FormatSupportsDXTCBase(CMP_FORMAT format) {
    switch (format) {
    case  CMP_FORMAT_ATI1N                :
    case  CMP_FORMAT_ATI2N                :
    case  CMP_FORMAT_ATI2N_XY             :
    case  CMP_FORMAT_ATI2N_DXT5           :
    case  CMP_FORMAT_BC1                  :
    case  CMP_FORMAT_BC2                  :
    case  CMP_FORMAT_BC3                  :
    case  CMP_FORMAT_BC4                  :
    case  CMP_FORMAT_BC5                  :
    case  CMP_FORMAT_DXT1                 :
    case  CMP_FORMAT_DXT3                 :
    case  CMP_FORMAT_DXT5                 :
    case  CMP_FORMAT_DXT5_xGBR            :
    case  CMP_FORMAT_DXT5_RxBG            :
    case  CMP_FORMAT_DXT5_RBxG            :
    case  CMP_FORMAT_DXT5_xRBG            :
    case  CMP_FORMAT_DXT5_RGxB            :
    case  CMP_FORMAT_DXT5_xGxR            :
        return (true);
        break;
    default:
        break;
    }
    return false;
}




