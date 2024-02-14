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
    printf("Starting Framework test program...\n");

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

    printf("All Compressonator Framework tests completed successfully\n");
    return 0;
}