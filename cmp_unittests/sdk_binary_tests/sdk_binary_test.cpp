//=====================================================================
// Copyright 2023-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#include "compressonator.h"

// Initialize all the fields needed for the test mipset
void InitTestMipSet(CMP_MipSet* mipset)
{
    mipset->m_nWidth = mipset->dwWidth = 64;
    mipset->m_nHeight = mipset->dwHeight = 64;

    mipset->m_nDepth = 1;

    mipset->m_format          = CMP_FORMAT_RGBA_8888;
    mipset->m_ChannelFormat   = CF_8bit;
    mipset->m_TextureType     = TT_2D;
    mipset->m_TextureDataType = TDT_ARGB;

    mipset->m_nBlockDepth  = 1;
    mipset->m_nBlockWidth  = 4;
    mipset->m_nBlockHeight = 4;

    mipset->m_nMipLevels    = 1;
    mipset->m_nMaxMipLevels = 10;  // just set to a default of 10, this isn't used

    mipset->m_pMipLevelTable = (CMP_MipLevelTable*)calloc(mipset->m_nMaxMipLevels, sizeof(CMP_MipLevel*));
    for (int i = 0; i < mipset->m_nMaxMipLevels; ++i)
    {
        mipset->m_pMipLevelTable[i] = (CMP_MipLevel*)calloc(1, sizeof(CMP_MipLevel));
    }

    CMP_MipLevel* mipLevel = mipset->m_pMipLevelTable[0];

    mipLevel->m_nWidth       = mipset->m_nWidth;
    mipLevel->m_nHeight      = mipset->m_nHeight;
    mipLevel->m_dwLinearSize = mipLevel->m_nWidth * mipLevel->m_nHeight * 4;
    mipLevel->m_pbData       = (CMP_BYTE*)calloc(1, mipLevel->m_dwLinearSize);

    mipset->dwDataSize = mipLevel->m_dwLinearSize;
    mipset->pData      = mipLevel->m_pbData;
}

int main(int argc, char** argv)
{
    // mipset and mipmap related functions
    {
        CMP_MipSet srcTexture = {};
        srcTexture.m_format   = CMP_FORMAT_ARGB_8888;

        CMP_MipSet destTexture = {};
        destTexture.m_format   = CMP_FORMAT_BC1;

        if (CMP_CreateMipSet(&srcTexture, 64, 64, 1, CF_8bit, TT_2D) != CMP_OK)
        {
            printf("Failed to create mipset.\n");
            return -1;
        }

        if (CMP_MaxFacesOrSlices(&srcTexture, 0) <= 0)
        {
            printf("Failed to get the max faces or slices from mipset.\n");
            return -1;
        }

        if (CMP_CalcMaxMipLevel(128, 128, false) <= 0)
        {
            printf("Failed to calculate maximum mipmap level.\n");
            return -1;
        }

        if (CMP_CalcMinMipSize(128, 128, 2) <= 0)
        {
            printf("Failed to calculate minimum mipmap level.\n");
            return -1;
        }

        if (CMP_getFormat_nChannels(CMP_FORMAT_ARGB_8888) <= 0)
        {
            printf("Failed to get the number of format channels.\n");
            return -1;
        }

        if (CMP_GenerateMIPLevels(&srcTexture, 3) != CMP_OK)
        {
            printf("Failed to generate mipmap levels.\n");
            return -1;
        }

        CMP_CFilterParams filterParams = {};
        filterParams.nMinSize          = 4;
        filterParams.fGammaCorrection  = 1.0f;

        if (CMP_GenerateMIPLevelsEx(&srcTexture, &filterParams) != CMP_OK)
        {
            printf("Failed to generate mipmap levels (using EX function).\n");
            return -1;
        }

        CMP_CompressOptions compressOptions = {};
        compressOptions.DestFormat          = CMP_FORMAT_BC1;
        compressOptions.fquality            = 0.5f;
        compressOptions.SourceFormat        = CMP_FORMAT_RGBA_8888;

        if (CMP_ConvertMipTexture(&srcTexture, &destTexture, &compressOptions, 0) != CMP_OK)
        {
            printf("Failed to compress mipmap texture.\n");
            return -1;
        }

        CMP_AnalysisData analysisData = {};
        if (CMP_MipSetAnlaysis(&srcTexture, &srcTexture, 0, 0, &analysisData) != CMP_OK)
        {
            printf("Failed to analyse mipsets.\n");
            return -1;
        }

        CMP_FreeMipSet(&srcTexture);

        if (CMP_CreateCompressMipSet(&srcTexture, &destTexture) != CMP_OK)
        {
            printf("Failed to create compressed mipset.\n");
            return -1;
        }

        CMP_FreeMipSet(&destTexture);
        CMP_FreeMipSet(&srcTexture);
    }

    // CMP_Texture related functions
    {
        CMP_Texture srcTexture = {};
        srcTexture.dwSize      = sizeof(CMP_Texture);
        srcTexture.dwWidth     = 64;
        srcTexture.dwHeight    = 64;
        srcTexture.dwPitch     = srcTexture.dwWidth * 4;
        srcTexture.format      = CMP_FORMAT_RGBA_8888;
        srcTexture.dwDataSize  = srcTexture.dwWidth * srcTexture.dwHeight * 4;
        srcTexture.pData       = (CMP_BYTE*)calloc(1, srcTexture.dwDataSize);

        if (CMP_CalculateBufferSize(&srcTexture) <= 0)
        {
            printf("Failed to calculate buffer size.\n");
            return -1;
        }

        CMP_Texture destTexture = {};
        destTexture             = srcTexture;
        destTexture.format      = CMP_FORMAT_BC1;
        destTexture.dwDataSize  = CMP_CalculateBufferSize(&destTexture);
        destTexture.pData       = (CMP_BYTE*)calloc(1, destTexture.dwDataSize);

        CMP_CompressOptions compressOptions = {};
        compressOptions.DestFormat          = CMP_FORMAT_BC1;
        compressOptions.fquality            = 0.5f;
        compressOptions.SourceFormat        = CMP_FORMAT_RGBA_8888;

        if (CMP_ConvertTexture(&srcTexture, &destTexture, &compressOptions, 0) != CMP_OK)
        {
            printf("Failed to convert texture.\n");
            return -1;
        }

        free(srcTexture.pData);
        free(destTexture.pData);
    }

    // Block encoder functions (BC6 and BC7)
    {
        BC6HBlockEncoder* bc6Encoder = 0;
        BC7BlockEncoder*  bc7Encoder = 0;

        CMP_BC6H_BLOCK_PARAMETERS bc6Settings = {};

        if (CMP_InitializeBCLibrary() != BC_ERROR_NONE)
        {
            printf("Failed to initialize BC library.\n");
            return -1;
        }

        if (CMP_CreateBC6HEncoder(bc6Settings, &bc6Encoder) != BC_ERROR_NONE)
        {
            printf("Failed to create BC6H block encoder.\n");
            return -1;
        }

        if (CMP_CreateBC7Encoder(0.5, false, false, 0, 0.0, &bc7Encoder) != BC_ERROR_NONE)
        {
            printf("Failed to create BC7 block encoder.\n");
            return -1;
        }

        CMP_FLOAT srcBlockBC6[4 * 4][4] = {};
        double    srcBlockBC7[4 * 4][4] = {};

        CMP_BYTE destBlockBC6[16] = {};
        CMP_BYTE destBlockBC7[16] = {};

        if (CMP_EncodeBC6HBlock(bc6Encoder, srcBlockBC6, destBlockBC6) != BC_ERROR_NONE)
        {
            printf("Failed to encode BC6H block.\n");
            return -1;
        }

        if (CMP_EncodeBC7Block(bc7Encoder, srcBlockBC7, destBlockBC7) != BC_ERROR_NONE)
        {
            printf("Failed to encode BC7 block.\n");
            return -1;
        }

        if (CMP_DecodeBC6HBlock(destBlockBC6, srcBlockBC6) != BC_ERROR_NONE)
        {
            printf("Failed to decode BC6H block.\n");
            return -1;
        }

        if (CMP_DecodeBC7Block(destBlockBC7, srcBlockBC7) != BC_ERROR_NONE)
        {
            printf("Failed to decode BC7 block.\n");
            return -1;
        }

        if (CMP_DestroyBC6HEncoder(bc6Encoder) != BC_ERROR_NONE)
        {
            printf("Failed to destroy BC6H block encoder.\n");
            return -1;
        }

        if (CMP_DestroyBC7Encoder(bc7Encoder) != BC_ERROR_NONE)
        {
            printf("Failed to destroy BC7 block encoder.\n");
            return -1;
        }

        if (CMP_ShutdownBCLibrary() != BC_ERROR_NONE)
        {
            printf("Failed to shutdown BC library.\n");
            return -1;
        }
    }

    // Framework Test code
    {
        /**
         * Initialization
         */

        CMP_MipSet srcTexture  = {};
        CMP_MipSet destTexture = {};

        InitTestMipSet(&srcTexture);
        InitTestMipSet(&destTexture);

        destTexture.m_format = CMP_FORMAT_BC1;

        // manually allocate miplevel data
        CMP_MipLevel* mipLevel = 0;
        CMP_GetMipLevel(&mipLevel, &srcTexture, 0, 0);
        if (mipLevel == 0)
        {
            printf("Failed to get mipmap level 0 from texture.\n");
            return -1;
        }

        KernelOptions kernelOptions = {};

        kernelOptions.width     = 64;
        kernelOptions.height    = 64;
        kernelOptions.fquality  = 0.5f;
        kernelOptions.format    = CMP_FORMAT_BC1;
        kernelOptions.srcformat = CMP_FORMAT_RGBA_8888;

        CMP_InitFramework();

        /**
         * Testing the "Block Encoder" style of functions
        */

        CMP_EncoderSetting encoderSettings = {};

        encoderSettings.format  = CMP_FORMAT_BC1;
        encoderSettings.height  = 4;
        encoderSettings.width   = 4;
        encoderSettings.quality = 0.5f;

        void* encoder = 0;

        if (CMP_CreateBlockEncoder(&encoder, encoderSettings) != CMP_OK)
        {
            printf("Failed to create block encoder.\n");
            return -1;
        }

        unsigned char srcBlock[4 * 4 * 4] = {};
        unsigned char destBlock[8]        = {};

        if (CMP_CompressBlock(&encoder, srcBlock, 4 * 4, destBlock, 0) != CMP_OK)
        {
            printf("Failed to compress test block using CompressBlock.\n");
            return -1;
        }

        if (CMP_CompressBlockXY(&encoder, 0, 0, srcBlock, 4 * 4, destBlock, 0) != CMP_OK)
        {
            printf("Failed to compress test block using CompressBlockXY.\n");
            return -1;
        }

        CMP_DestroyBlockEncoder(&encoder);

        /**
         * Testing the Compute Library interface
        */
        if (CMP_CreateComputeLibrary(&srcTexture, &kernelOptions, 0) != CMP_OK)
        {
            printf("Failed to create the compute library.\n");
            return -1;
        }

        ComputeOptions computeOptions = {};
        if (CMP_SetComputeOptions(&computeOptions) != CMP_OK)
        {
            printf("Failed to set compute options.\n");
            return -1;
        }

        if (CMP_CompressTexture(&kernelOptions, srcTexture, destTexture, 0) != CMP_OK)
        {
            printf("Failed to compress texture.\n");
            return -1;
        }

        KernelPerformanceStats perfStats = {};
        if (CMP_GetPerformanceStats(&perfStats) != CMP_OK)
        {
            printf("Failed to get performance stats.\n");
            return -1;
        }

        KernelDeviceInfo deviceInfo = {};
        if (CMP_GetDeviceInfo(&deviceInfo) != CMP_OK)
        {
            printf("Failed to get device information.\n");
            return -1;
        }

        if (CMP_DestroyComputeLibrary(true) != CMP_OK)
        {
            printf("Failed to destroy the compute library.\n");
            return -1;
        }

        /**
         * Testing the remaining miscellaneous functions
        */

        CMP_FreeMipSet(&destTexture);

        // We can't/don't want to load or save external files, so we just reference the functions via pointers
        CMP_ERROR (*loadPointer)(const char*, CMP_MipSet*) = CMP_LoadTexture;
        CMP_ERROR (*savePointer)(const char*, CMP_MipSet*) = CMP_SaveTexture;

        if (loadPointer == 0 || savePointer == 0)
        {
            printf("Error with assigning function pointers.\n");
            return -1;
        }

        if (CMP_ProcessTexture(&srcTexture, &destTexture, kernelOptions, 0) != CMP_OK)
        {
            printf("Failed to process texture.\n");
            return -1;
        }

        CMP_FreeMipSet(&srcTexture);
        CMP_FreeMipSet(&destTexture);

        CMP_Format2FourCC(CMP_FORMAT_BC1, &destTexture);

        if (CMP_ParseFormat("BC1") != CMP_FORMAT_BC1)
        {
            printf("Failed to parse format string.\n");
            return -1;
        }

        if (CMP_NumberOfProcessors() <= 0)
        {
            printf("Failed to return a valid number of processors.\n");
            return -1;
        }

        if (CMP_IsCompressedFormat(CMP_FORMAT_BC1) != true)
        {
            printf("Compressed format check failed, BC1 return false.\n");
            return -1;
        }

        if (CMP_IsFloatFormat(CMP_FORMAT_BC1))
        {
            printf("Float format check failed, BC1 format returned true.\n");
            return -1;
        }
    }

    printf("Compressonator SDK tests finished successfully.\n");
    return 0;
}