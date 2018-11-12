// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved
// Copyright (c) 2004-2006    ATI Technologies Inc.
//
// Example1: Console application that demonstrates how to use the Compressonator SDK Lib
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
// Example1: Console application that demonstrates how simple SDK API
//
// SDK files required for application:
//     Compressontor.h
//     Compressonator_MD.lib or Compressonator_MDd.lib


#include <stdio.h>
#include <string>

#include "Compressonator.h"

// The Helper code is provided to load Textures from a DDS file (Support Images and Compressed formats supported by DX9)
// Compressed images can also be saved using DDS File (Support Images and Compression format supported by DX9 and partial 
// formats for DX10)

#include "DDS_Helpers.h"

// Example code using high level SDK API's optimized with MultiThreading for processing a single image
// Note: No error checking is done on user arguments, so all parameters must be correct in this example
// (files must exist, values correct format, etc..)

#ifdef _DEBUG
#pragma comment(lib,"Compressonator_MDd.lib")
#else  // Building Release 
#pragma comment(lib,"Compressonator_MD.lib")
#endif

#ifdef _WIN32
#include <windows.h>
#include <time.h>
double timeStampsec()
{
    static LARGE_INTEGER frequency;
    if (frequency.QuadPart == 0)
        QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return now.QuadPart / double(frequency.QuadPart);
}
#endif


bool g_bAbortCompression = false;   // If set true current compression will abort

//---------------------------------------------------------------------------
// Sample loop back code called for each compression block been processed
//---------------------------------------------------------------------------
bool CompressionCallback(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    UNREFERENCED_PARAMETER(pUser1);
    UNREFERENCED_PARAMETER(pUser2);

    std::printf("\rCompression progress = %2.0f", fProgress);

    return g_bAbortCompression;
}

int main(int argc, const char* argv[])
{
    #ifdef _WIN32
    double start_time = timeStampsec();
    #endif
    if (argc < 5)
    {
        std::printf("Example1 SourceFile DestFile Format Quality\n");
        std::printf("This example shows how to compress a single image\n");
        std::printf("to a compression format using a quality setting\n");
        std::printf("usage: Example1.exe ruby.dds ruby_bc7.dds BC7 0.05\n");
        std::printf("this will generate a compressed ruby file in BC7 format\n");
        return 0;
    }

    const char*     pszSourceFile = argv[1];
    const char*     pszDestFile   = argv[2];
    CMP_FORMAT      destFormat    = ParseFormat(argv[3]);
    float           fQuality      = std::stof(argv[4]);

    if (destFormat == CMP_FORMAT_Unknown)
    {
        std::printf("Unsupported destination format\n");
        return 0;
    }

    //==========================
    // Load Source Texture
    //==========================
    CMP_Texture srcTexture;
    if (!LoadDDSFile(pszSourceFile, srcTexture))
    {
        std::printf("Error loading source file!\n");
        return 0;
    }

    //===================================
    // Initialize Compressed Destination 
    //===================================
    CMP_Texture destTexture;
    destTexture.dwSize     = sizeof(destTexture);
    destTexture.dwWidth    = srcTexture.dwWidth;
    destTexture.dwHeight   = srcTexture.dwHeight;
    destTexture.dwPitch    = 0;
    destTexture.format     = destFormat;
    destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
    destTexture.pData = (CMP_BYTE*)malloc(destTexture.dwDataSize);

    //==========================
    // Set Compression Options
    //==========================
    CMP_CompressOptions options = {0};
    options.dwSize       = sizeof(options);
    options.fquality     = fQuality;
    options.dwnumThreads = 8;

    //==========================
    // Compress Texture
    //==========================
    CMP_ERROR   cmp_status;
    cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, &options, &CompressionCallback, NULL, NULL);
    if (cmp_status != CMP_OK)
    {
        free(srcTexture.pData);
        free(destTexture.pData);
        std::printf("Compression returned an error %d\n", cmp_status);
        return cmp_status;
    }

    //==========================
    // Save Compressed Testure
    //==========================
    if (cmp_status == CMP_OK)
        SaveDDSFile(pszDestFile, destTexture);

    free(srcTexture.pData);
    free(destTexture.pData);

#ifdef _WIN32
    std::printf("\nProcessed in %.3f seconds\n", timeStampsec() - start_time);
#endif
   return 0;
}
