// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved
// Copyright (c) 2004-2006    ATI Technologies Inc.
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
// Example2: Console application that demonstrates how to use compression API in a
//           multithread environment
//
// SDK files required for application:
//     Compressontor.h
//     Compressonator_MD.lib or Compressonator_MDd.lib

#include <stdio.h>
#include <assert.h>
#include <string>
#include <math.h>
#include <float.h>

#include "Compressonator.h"

// The Helper code is provided to load Textures from a DDS file (Support Images and Compressed formats supported by DX9)
// Compressed images can also be saved using DDS File (Support Images and Compression format supported by DX9 and partial 
// formats for DX10)

#include "DDS_Helpers.h"

// EXAMPLE2 is using high level SDK API's optimized with MultiThreading for processing multiple images
 
#define USE_EXAMPLE2

#if __cplusplus < 199711L
    #error This library needs at least a C++11 compliant compiler
#endif
#include <thread>
#define MXT 2       // Max number of compressed sample formats to generate, this is limited by available system mem

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
    if (argc < 4)
    {
        std::printf("Example2.exe SourceFile Format1 Format2 Quality\n");
        std::printf("This example shows how to compress a single image into two\n");
        std::printf("compression formats using multi threading\n");
        std::printf("usage: Example2.exe ruby.dds BC1 BC7 0.05\n");
        std::printf("this will generate a result_0.dds for BC1\n");
        std::printf("and                  result_1.dds for BC7\n");
        return 0;
    }

    // Note: No error checking is done on user arguments, so all parameters must be correct in this example
    // (files must exist, values correct format, etc..)
    const char*     pszSourceFile       = argv[1];
    CMP_FORMAT      destFormat[MXT]     = { ParseFormat(argv[2]),ParseFormat(argv[3]) };
    float           fQuality            = std::stof(argv[4]);

    if ((destFormat[0] == CMP_FORMAT_Unknown)||(destFormat[1] == CMP_FORMAT_Unknown))
    {
        std::printf("Unsupported destination format\n");
        return 0;
    }

    //==========================
    // Load Source Texture #1
    //==========================
    CMP_Texture srcTexture;
    if (!LoadDDSFile(pszSourceFile, srcTexture))
    {
        std::printf("Error loading source file!\n");
        return 0;
    }

    //===================================
    // Initialize Compressed Destinations
    //===================================
    CMP_Texture destTexture[MXT];

    for (int i = 0; i<MXT; i++)
    {
        destTexture[0].dwSize     = sizeof(destTexture);
        destTexture[0].dwWidth    = srcTexture.dwWidth;
        destTexture[0].dwHeight   = srcTexture.dwHeight;
        destTexture[0].dwPitch    = 0;
        destTexture[0].format     = destFormat[i];
        destTexture[0].dwDataSize = CMP_CalculateBufferSize(&destTexture[i]);
        destTexture[0].pData = (CMP_BYTE*)malloc(destTexture[0].dwDataSize);
    }

    //=======================================
    // Set Compression Options for Textures
    //=======================================
    CMP_CompressOptions options = {0};
    options.dwSize       = sizeof(options);
    options.fquality     = fQuality;            // Quality
    options.dwnumThreads = 8;                   // Number of threads to use per texture

    //=====================================================
    // Compress the Texture to multiple compressed formats
    //=====================================================
    CMP_ERROR cmp_status;
    try
    {
        //--------------------------------------------------------------------------------
        // Issue note: cmp_status3 is not used as an array!. 
        // ie cmp_status3[MXT] in lambda calls - so status of results is Un-deterministic!
        //--------------------------------------------------------------------------------
        std::thread t3[MXT];
        for (int i =0; i<MXT; i++)
            t3[i] = std::thread([&]() { cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture[i], &options, &CompressionCallback, NULL, NULL); });

        // Finish the encoders
        for (int i = 0; i<MXT; i++)
            t3[i].join();
    } 
    catch (const std::exception& ex)
    {
        std::printf("Error: %s\n",ex.what());
    }

    //======================================
    // Save Compressed Textures To DDS Files
    //======================================
    std::string str;
    if (cmp_status == CMP_OK)
    {
        for (int i = 0; i < MXT; i++)
        {
                str.clear();
                str.append("result_");
                str.append(std::to_string(i).c_str());
                str.append(".dds");
                SaveDDSFile(str.c_str(), destTexture[i]);
       }
    }

    if (srcTexture.pData)  free(srcTexture.pData);
    for (int i = 0; i < MXT; i++)
    {
        if (destTexture[i].pData) free(destTexture[i].pData);
    }

   std::printf("\n");
   #ifdef _WIN32
   std::printf("\nProcessed in %.3f seconds\n", timeStampsec() - start_time);
   #endif

   // Clean up memory used for textures
   free(srcTexture.pData);
   for (int i = 0; i < MXT; i++)
        free(destTexture[i].pData);

   return 0;
}
