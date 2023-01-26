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
#include "cmp_plugininterface.h"
#include "atiformats.h"

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

void find_closest_blockdim_2d(float target_bitrate, int *x, int *y, int consider_illegal) {
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

void find_closest_blockxy_2d(int *x, int *y, int consider_illegal) {
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
        find_closest_blockdim_2d(bitrateF, x, y, 0);
    } else {
        float bitrateF = float(128.0f / ((*x)*(*y)));
        find_closest_blockdim_2d(bitrateF, x, y, 0);
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
#ifdef USE_LOSSLESS_COMPRESSION
    else if (file_extension.compare(".brlg") == 0) {
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
        return CMP_FORMAT_RGBA_16F;
    }

    pMipSet->m_ChannelFormat = CF_8bit;
    return CMP_FORMAT_ARGB_8888;
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
#if (OPTION_BUILD_ASTC == 1)
    if (MipSetIn->m_format == CMP_FORMAT_ASTC && !(configSetting->useCPU) && decodeWith == CMP_GPUDecode::GPUDecode_DIRECTX) {
        configSetting->errMessage = "ASTC format does not supported by DirectX API. Please view ASTC compressed images using other options (CPU) (under Settings->Application Options).";
        PrintInfo("Decompress Error: ASTC format does not supported by DirectX API. Please view ASTC compressed images using CPU (under Settings->Application Options).\n");
        return NULL;
    } else if (MipSetIn->m_format == CMP_FORMAT_ASTC && !(configSetting->useCPU) && decodeWith == CMP_GPUDecode::GPUDecode_OPENGL) {
        configSetting->errMessage = "Decode ASTC with OpenGL is not supported. Please view ASTC compressed images using other options (CPU) (under Settings->Application Options).";
        PrintInfo("Decompress Error: Decode ASTC with OpenGL is not supported. Please view ASTC compressed images using CPU (under Settings->Application Options).\n");
        return NULL;
    }
#endif
    if (MipSetIn->m_format == CMP_FORMAT_BROTLIG || MipSetIn->m_format == CMP_FORMAT_BINARY)
    {
        configSetting->errMessage = "BRLG and BINARY formats cannot be decompressed and viewed.";
        PrintInfo("Decompress Error: BRLG and BINARY formats cannot be decompressed.\n");
        return NULL;
    }
    // Compress Options
    bool silent = true;
    CMP_CompressOptions CompressOptions;
    memset(&CompressOptions, 0, sizeof(CMP_CompressOptions));
    CompressOptions.dwnumThreads = 0;
    CMIPS m_CMIPS;

    MipSet* MipSetOut = new MipSet();
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
        MipSetOut->m_format = CMP_FORMAT_RGBA_16F;
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

void SwizzleMipSet(MipSet* pMipSet)
{
    if (pMipSet->m_ChannelFormat == CF_Compressed)
        return;

    for (int nMipLevel = 0; nMipLevel<pMipSet->m_nMipLevels; nMipLevel++)
    {
        for (int nFaceOrSlice = 0; nFaceOrSlice< CMP_MaxFacesOrSlices(pMipSet, nMipLevel); nFaceOrSlice++)
        {
            //=====================
            // Uncompressed source
            //======================
            MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(pMipSet, nMipLevel, nFaceOrSlice);

            CMP_DWORD width = pInMipLevel->m_nWidth;
            CMP_DWORD height = pInMipLevel->m_nHeight;
            CMP_BYTE* data = pInMipLevel->m_pbData;

            CMP_DWORD numChannels = 4;

            if (pMipSet->m_TextureDataType == TDT_XRGB || pMipSet->m_TextureDataType == TDT_RGB)
                numChannels = 3;
            
            CMP_DWORD bytesPerChannel = GetChannelFormatBitSize(pMipSet->m_format) / 8;

            CMP_BYTE* redChannel = data;
            CMP_BYTE* blueChannel = data + 2*bytesPerChannel;

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    for (uint32_t i = 0; i < bytesPerChannel; ++i)
                    {
                        CMP_BYTE temp = redChannel[i];
                        redChannel[i] = blueChannel[i];
                        blueChannel[i] = temp;
                    }

                    redChannel += bytesPerChannel*numChannels;
                    blueChannel += bytesPerChannel*numChannels;
                }
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

#if (OPTION_CMP_QT == 1)
QImage* CMP_CreateQImage(const char* SourceFile)
{
    QImage* image = new QImage(SourceFile);
    if (image)
    {
        QImage::Format format = image->format();
        // Check supported QImage to MipSet conversion
        if (!((format == QImage::Format_ARGB32) || 
              (format == QImage::Format_ARGB32_Premultiplied) || 
              (format == QImage::Format_RGB32) ||
              (format == QImage::Format_Mono) || 
              (format == QImage::Format_Grayscale8) || 
              (format == QImage::Format_Indexed8)))
        {
            int    width = image->width(), height = image->height();
            QImage newimage = image->convertToFormat(QImage::Format_ARGB32);
            delete image;
            image = new QImage(width, height, QImage::Format_ARGB32);
            if (image)
                *image = newimage.copy();
            else
                return NULL;
        }
    }
    else
        return NULL;
    return image;
}
#endif

int AMDLoadMIPSTextureImage(const char *SourceFile, MipSet *MipSetIn, bool use_OCV, void *pluginManager) {
    if (pluginManager == NULL)
        return -1;

    PluginInterface_Image *plugin_Image;

    PluginManager *plugin_Manager = (PluginManager *)pluginManager;
    if (use_OCV)
    {
        plugin_Image = reinterpret_cast<PluginInterface_Image *>(plugin_Manager->GetPlugin("IMAGE", "OCV"));
    }
#ifdef USE_LOSSLESS_COMPRESSION_BINARY
    else if (MipSetIn->m_format == CMP_FORMAT_BINARY)
    {
        plugin_Image = reinterpret_cast<PluginInterface_Image*>(plugin_Manager->GetPlugin("IMAGE", "BINARY"));
    }
#endif
    else
    {
        std::string file_extension = CMP_GetFileExtension(SourceFile, false, true);
        plugin_Image = reinterpret_cast<PluginInterface_Image*>(plugin_Manager->GetPlugin("IMAGE", (char*)file_extension.c_str()));
    }

    // do the load
    if (plugin_Image) {
        plugin_Image->TC_PluginSetSharedIO(g_CMIPS);

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
        QImage* qimage = CMP_CreateQImage(SourceFile);
        if (qimage != NULL)
        {
            result = QImage2MIPS(qimage, g_CMIPS, MipSetIn);
            delete qimage;
        }
        return result;
#else 
        //DS - fallback to stb, push for a minimal commandline size and load time...
        extern CMP_ERROR stb_load(const char* SourceFile, MipSet* MipSetIn);
        return stb_load(SourceFile, MipSetIn);
#endif
    }

    return 0;
}

int AMDSaveMIPSTextureImage(const char *DestFile, MipSet *MipSetIn, bool use_OCV, CMP_CompressOptions option) {
    bool filesaved = false;
    CMIPS m_CMIPS;
    m_CMIPS.PrintLine = PrintStatusLine;

    std::string file_extension = CMP_GetJustFileExt(DestFile);
    file_extension.erase(std::remove(file_extension.begin(), file_extension.end(), '.'), file_extension.end());
    for(char& c : file_extension)
        c = toupper(c);

    PluginInterface_Image *plugin_Image;

    if (use_OCV)
    {
        plugin_Image = reinterpret_cast<PluginInterface_Image *>(g_pluginManager.GetPlugin("IMAGE", "OCV"));
    }
#ifdef USE_LOSSLESS_COMPRESSION_BINARY
    else if (MipSetIn->m_format == CMP_FORMAT_BINARY)
    {
        plugin_Image = reinterpret_cast<PluginInterface_Image*>(g_pluginManager.GetPlugin("IMAGE", "BINARY"));
    }
#endif
    else
    {
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
#if (OPTION_BUILD_ASTC == 1)
            case CMP_FORMAT_ASTC:
#endif
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




