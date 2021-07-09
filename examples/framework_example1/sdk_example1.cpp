// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved
// Copyright (c) 2004-2006    ATI Technologies Inc.
//
// Example1: Console application that demonstrates how to use the Compressonator Framework Lib
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
// Example1: Console application that demonstrates Framework SDK API
//
// SDK files required for application:
//     compressonator.h
//     CMP_Framework_xx.lib  For static libs xx is either MD, MT or MDd or MTd,
//                      When using DLL builds make sure the  CMP_Framework_xx_DLL.dll is in exe path


#include <stdio.h>
#include <string>

#include "compressonator.h"

// Example code using high level SDK API's optimized with MultiThreading for processing a single image
// Note:Limited error checking is done on user arguments, so all parameters must be correct in this example
// (files must exist, correct format and quality values, etc..)

#ifdef _WIN32
#include <windows.h>
#include <time.h>
double timeStampsec() {
    static LARGE_INTEGER frequency;
    if (frequency.QuadPart == 0)
        QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return now.QuadPart / double(frequency.QuadPart);
}
#endif


CMP_BOOL g_bAbortCompression = false;   // If set true current compression will abort

//---------------------------------------------------------------------------
// Sample loop back code called for each compression block been processed
//---------------------------------------------------------------------------
CMP_BOOL CompressionCallback(CMP_FLOAT fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
    UNREFERENCED_PARAMETER(pUser1);
    UNREFERENCED_PARAMETER(pUser2);

    std::printf("\rCompression progress = %2.0f", fProgress);

    return g_bAbortCompression;
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    double start_time = timeStampsec();
#endif
    if (argc < 5) {
        std::printf("Example1 SourceFile DestFile Format Quality\n");
        std::printf("This example shows how to compress a single image by generating mipmap levels\n");
        std::printf("then compressing it using the compression format and quality settings\n");
        std::printf("Quality is in the range of 0.0 to 1.0\n");
        std::printf("When using DLL builds make sure the  CMP_Framework_xx_DLL.dll is in exe path\n");
        std::printf("Usage: Example1.exe ruby.dds ruby_bc7.dds BC7 0.05\n");
        std::printf("this will generate a mip mapped compressed ruby file in BC7 format\n");
        return 0;
    }

    const char*     pszSourceFile = argv[1];
    const char*     pszDestFile   = argv[2];
    CMP_FORMAT      destFormat    = CMP_ParseFormat(argv[3]);
    CMP_ERROR       cmp_status;
    CMP_FLOAT       fQuality;

    try {
        fQuality = std::stof(argv[4]);
        if (fQuality < 0.0f) {
            fQuality = 0.0f;
            std::printf("Warning: Quality setting is out of range using 0.0\n");
        }
        if (fQuality > 1.0f) {
            fQuality = 1.0f;
            std::printf("Warning: Quality setting is out of range using 1.0\n");
        }
    } catch (...) {
        std::printf("Error: Unable to process quality setting\n");
        return -1;
    }

    if (destFormat == CMP_FORMAT_Unknown) {
        std::printf("Error: Unsupported destination format\n");
        return 0;
    }

    //---------------
    // Load the image
    //---------------
    CMP_MipSet MipSetIn;
    memset(&MipSetIn, 0, sizeof(CMP_MipSet));
    cmp_status = CMP_LoadTexture(pszSourceFile, &MipSetIn);
    if (cmp_status != CMP_OK) {
        std::printf("Error %d: Loading source file!\n",cmp_status);
        return -1;
    }

    //----------------------------------------------------------------------
    // generate mipmap level for the source image, if not already generated
    //----------------------------------------------------------------------

    if (MipSetIn.m_nMipLevels <= 1) {
        CMP_INT requestLevel = 10; // Request 10 miplevels for the source image

        //------------------------------------------------------------------------
        // Checks what the minimum image size will be for the requested mip levels
        // if the request is too large, a adjusted minimum size will be returns
        //------------------------------------------------------------------------
        CMP_INT nMinSize = CMP_CalcMinMipSize(MipSetIn.m_nHeight, MipSetIn.m_nWidth, 10);

        //--------------------------------------------------------------
        // now that the minimum size is known, generate the miplevels
        // users can set any requested minumum size to use. The correct
        // miplevels will be set acordingly.
        //--------------------------------------------------------------
        CMP_GenerateMIPLevels(&MipSetIn, nMinSize);
    }

    //==========================
    // Set Compression Options
    //==========================
    KernelOptions   kernel_options;
    memset(&kernel_options, 0, sizeof(KernelOptions));

    kernel_options.format   = destFormat;   // Set the format to process
    kernel_options.fquality = fQuality;     // Set the quality of the result
    kernel_options.threads  = 0;            // Auto setting

    //--------------------------------------------------------------
    // Setup a results buffer for the processed file,
    // the content will be set after the source texture is processed
    // in the call to CMP_ConvertMipTexture()
    //--------------------------------------------------------------
    CMP_MipSet MipSetCmp;
    memset(&MipSetCmp, 0, sizeof(CMP_MipSet));

    //===============================================
    // Compress the texture using Compressonator Lib
    //===============================================
    cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, CompressionCallback);
    if (cmp_status != CMP_OK) {
        CMP_FreeMipSet(&MipSetIn);
        std::printf("Compression returned an error %d\n", cmp_status);
        return cmp_status;
    }

    //----------------------------------------------------------------
    // Save the result into a DDS file
    //----------------------------------------------------------------
    cmp_status = CMP_SaveTexture(pszDestFile, &MipSetCmp);
    CMP_FreeMipSet(&MipSetIn);
    CMP_FreeMipSet(&MipSetCmp);

    if (cmp_status != CMP_OK) {
        std::printf("Error %d: Saving processed file %s!\n",cmp_status,pszDestFile);
        return -1;
    }

#ifdef _WIN32
    std::printf("\nProcessed in %.3f seconds\n", timeStampsec() - start_time);
#else
    std::printf("Process Done \n");
#endif
    return 0;
}
