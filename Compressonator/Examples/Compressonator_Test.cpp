// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved
// Copyright (c) 2004-2006    ATI Technologies Inc.
//
// CMPTest.exe   : Console application that demonstrates how to use the SDK Libs
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

#include <stdio.h>
#include <assert.h>
#include <string>
#include <math.h>
#include <float.h>

#include "Compressonator.h"

// The Helper code is provided to load Textures from a DDS file (Support Images and Compressed formats supported by DX9)
// Compressed images can also be saved using DDS File (Support Images and Compression format supported by DX9 and partial 
// formats for DX10)

#include "Compressonator_Test_Helpers.h"

// There are several examples of how to compress source images
// EXAMPLE1 is using high level SDK API's optimized with MultiThreading for processing a single image
// EXAMPLE2 is using high level SDK API's optimized with MultiThreading for processing multiple images
// EXAMPLE3 is an example of low level API that give access to compression blocks (4x4) for BC6H and BC7
 
#define USE_EXAMPLE1
//#define USE_EXAMPLE2
//#define USE_EXAMPLE3

#ifdef USE_EXAMPLE2
#if __cplusplus < 199711L
    #error This library needs at least a C++11 compliant compiler
#endif
#include <thread>
#define TEST_REPEATED_THREADS       // Enable this if you have a 3rd Image and want to test multiple thread targets from a single image
#define MXT 5                       // Max number of samples to generate using TEST_REPEATED_THREADS: Note Max is limited by available system mem
#endif

#ifdef _DEBUG

// Example linking for using Compressonator_Mxd DLL's
// Note: Copy the Compressonator DLL's to your target dir before running the app
#ifdef  BUILD_MTd_DLL
#pragma comment(lib,"Compressonator_MTd_DLL.lib")
#endif

#ifdef  BUILD_MDd_DLL
#pragma comment(lib,"Compressonator_MDd_DLL.lib")
#endif

// Example static linking for Compressonator_Mx libs
#ifdef  BUILD_MTd
#pragma comment(lib,"Compressonator_MTd.lib")
#endif

#ifdef  BUILD_MDd
#pragma comment(lib,"Compressonator_MDd.lib")
#endif

#else  // Building Release 

// Example linking for using Compressonator_Mx DLL's
// Note: Copy the Compressonator DLL's to your target dir before running the app
#ifdef  BUILD_MT_DLL
#pragma comment(lib,"Compressonator_MT_DLL.lib")
#endif

#ifdef  BUILD_MD_DLL
#pragma comment(lib,"Compressonator_MD_DLL.lib")
#endif

// Example static linking for Compressonator_Mx libs
#ifdef  BUILD_MT
#pragma comment(lib,"Compressonator_MT.lib")
#endif

#ifdef  BUILD_MD
#pragma comment(lib,"Compressonator_MD.lib")
#endif

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
        std::printf("Compressonator_Test SourceFile DestFile Format Quality\n");

        return 0;
    }

    // Note: No error checking is done on user arguments, so all parameters must be correct in this example
    // (files must exist, values correct format, etc..)
    const char*     pszSourceFile = argv[1];
    const char*     pszDestFile   = argv[2];
    CMP_FORMAT destFormat    = ParseFormat(argv[3]);
    float      fQuality      = std::stof(argv[4]);

    if (destFormat == CMP_FORMAT_Unknown)
    {

        std::printf("Unsupported destination format\n");

        return 0;
    }

    // Load the source texture
    CMP_Texture srcTexture;
    if (!LoadDDSFile(pszSourceFile, srcTexture))
    {
        std::printf("Error loading source file!\n");

        return 0;
    }

    // Init dest texture
    CMP_Texture destTexture;
    destTexture.dwSize     = sizeof(destTexture);
    destTexture.dwWidth    = srcTexture.dwWidth;
    destTexture.dwHeight   = srcTexture.dwHeight;
    destTexture.dwPitch    = 0;
    destTexture.format     = destFormat;
    destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
    destTexture.pData = (CMP_BYTE*)malloc(destTexture.dwDataSize);

    CMP_CompressOptions options = {0};
    //memset(&options, 0, sizeof(options));
    options.dwSize = sizeof(options);

    // Option 1 of setting compression options - Prefered Method
    snprintf(options.CmdSet[0].strCommand, sizeof(AMD_CMD_SET::strCommand), "Quality");
    snprintf(options.CmdSet[0].strParameter, sizeof(AMD_CMD_SET::strParameter), "%s", argv[4]);  // Use user specified Quality (lower values increases performance)
    snprintf(options.CmdSet[1].strCommand, sizeof(AMD_CMD_SET::strCommand), "ModeMask");
    snprintf(options.CmdSet[1].strParameter, sizeof(AMD_CMD_SET::strParameter), "207");          // 0xCF
    snprintf(options.CmdSet[2].strCommand, sizeof(AMD_CMD_SET::strCommand), "NumThreads");     // Use Multi Threading for fast performance
    snprintf(options.CmdSet[2].strParameter, sizeof(AMD_CMD_SET::strParameter), "8");
    options.NumCmds = 3;

#ifdef USE_EXAMPLE1
    CMP_ERROR   cmp_status;
    cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, &options, &CompressionCallback, NULL, NULL);
    if (cmp_status != CMP_OK)
    {
        if (srcTexture.pData)  free(srcTexture.pData);
        if (destTexture.pData) free(destTexture.pData);
        std::printf("Compression returned an error %d\n", cmp_status);
        return 0;
    }

    if (cmp_status == CMP_OK)
        SaveDDSFile(pszDestFile, destTexture);

    if (srcTexture.pData)  free(srcTexture.pData);
    if (destTexture.pData) free(destTexture.pData);
#endif

#ifdef USE_EXAMPLE2
    if (argc < 8)
    {
        std::printf("Example2 requires 8 settings:\nSourceFile1 DestFile1 Format1 Quality1 SourceFile2 DestFile2 Format2 Quality2\n");
        return 0;
    }

    // Note: No error checking is done on user arguments, so all parameters must be correct in this example
    const char*     pszSourceFile2 = argv[5];
    const char*     pszDestFile2 = argv[6];
    CMP_FORMAT destFormat2 = ParseFormat(argv[7]);
    float      fQuality2 = std::stof(argv[8]);

    CMP_ERROR   cmp_status1;
    CMP_ERROR   cmp_status2;

    // Load the 2nd source texture
    CMP_Texture srcTexture2;
    if (!LoadDDSFile(pszSourceFile2, srcTexture2))
    {
        std::printf("Error loading source file!\n");
        return 0;
    }

    // Init dest texture
    CMP_Texture destTexture2;
    destTexture2.dwSize = sizeof(destTexture2);
    destTexture2.dwWidth = srcTexture2.dwWidth;
    destTexture2.dwHeight = srcTexture2.dwHeight;
    destTexture2.dwPitch = 0;
    destTexture2.format = destFormat2;
    destTexture2.dwDataSize = CMP_CalculateBufferSize(&destTexture2);
    destTexture2.pData = (CMP_BYTE*)malloc(destTexture2.dwDataSize);


    CMP_CompressOptions options2;
    memset(&options2, 0, sizeof(options2));
    options2.dwSize = sizeof(options2);

    // Option 1 of setting compression options - Prefered Method
    sprintf(options2.CmdSet[0].strCommand, "Quality");
    sprintf(options2.CmdSet[0].strParameter, "%s", argv[8]);  // Use user specified Quality (lower values increases performance)
    sprintf(options2.CmdSet[1].strCommand, "ModeMask");
    sprintf(options2.CmdSet[1].strParameter, "207");          // 0xCF
    sprintf(options2.CmdSet[2].strCommand, "NumThreads");     // Use Multi Threading for fast performance
    sprintf(options2.CmdSet[2].strParameter, "8");
    options2.NumCmds = 3;


#ifdef TEST_REPEATED_THREADS

    if (argc < 11)
    {
        std::printf("This test requires 11 settings:\nSourceFile1 DestFile1 Format1 Quality1 SourceFile2 DestFile2 Format2 Quality2 SourceFile3 Format3 Quality3\n");
        return 0;
    }

    const char*     pszSourceFile3 = argv[9];
    CMP_FORMAT destFormat3 = ParseFormat(argv[10]);
    float      fQuality3 = std::stof(argv[11]);

    CMP_Texture             srcTexture3;
    CMP_Texture             destTexture3[MXT] = {};
    CMP_ERROR               cmp_status3;
    CMP_CompressOptions     options3;

    // Load the 3rd source texture
    if (!LoadDDSFile(pszSourceFile3, srcTexture3))
    {
        std::printf("Load source file failed %d\n", cmp_status3);
        return 0;
    }

    for (int i = 0; i < MXT; i++)
    {
        // Init dest texture
        destTexture3[i].dwSize = sizeof(destTexture3[i]);
        destTexture3[i].dwWidth = srcTexture3.dwWidth;
        destTexture3[i].dwHeight = srcTexture3.dwHeight;
        destTexture3[i].dwPitch = 0;
        destTexture3[i].format = destFormat3;
        destTexture3[i].dwDataSize = CMP_CalculateBufferSize(&destTexture3[i]);
        destTexture3[i].pData = (CMP_BYTE*)malloc(destTexture3[i].dwDataSize);
    }

    memset(&options3, 0, sizeof(options3));
    options3.dwSize = sizeof(options3);

    // Option 1 of setting compression options - Prefered Method
    sprintf(options3.CmdSet[0].strCommand, "Quality");
    sprintf(options3.CmdSet[0].strParameter, "%s", argv[11]);  // Use user specified Quality (lower values increases performance)
    sprintf(options3.CmdSet[1].strCommand, "ModeMask");
    sprintf(options3.CmdSet[1].strParameter, "207");          // 0xCF
    sprintf(options3.CmdSet[2].strCommand, "NumThreads");     // Use Multi Threading for fast performance
    sprintf(options3.CmdSet[2].strParameter, "8");
    options3.NumCmds = 3;
#endif

    try
    {
        //--------------------------------------------------------------------------------------------------
        // When using the current implementation of BC7 or BC6 Codecs as Multithreaded 
        // you should initial first the libs : Since dynamic look up tables are used for optimal performance
        // this only needs to be called once.
        // Do a simmilar operation if format is BC6H
        //---------------------------------------------------------------------------------------------------
        // if (destTexture.format == CMP_FORMAT_BC7)
        // {
        //     // Initialize the Codec: Need to call it only once, repeated calls will return BC_ERROR_LIBRARY_ALREADY_INITIALIZED
        //     if (CMP_InitializeBCLibrary() != BC_ERROR_NONE)
        //     {
        //         std::printf("BC Codec already initialized!\n");
        //     }
        // }


        // Use CMP_ConvertTexture as Lambda function
        std::thread t1([&]() { cmp_status1 = CMP_ConvertTexture(&srcTexture,  &destTexture,  &options,  &CompressionCallback, NULL, NULL); });
        std::thread t2([&]() { cmp_status2 = CMP_ConvertTexture(&srcTexture2, &destTexture2, &options2, &CompressionCallback, NULL, NULL); });

#ifdef TEST_REPEATED_THREADS
        // Issue note: cmp_status3 is not used as an array!. ie cmp_status3[MXT] in lambda calls - so status of results is Un-deterministic!
        std::thread t3[MXT];
        for (int i =0; i<MXT; i++)
            t3[i] = std::thread([&]() { cmp_status3 = CMP_ConvertTexture(&srcTexture3, &destTexture3[i], &options3, &CompressionCallback, NULL, NULL); });
#endif

        t1.join();
        t2.join();

#ifdef TEST_REPEATED_THREADS
        for (int i = 0; i<MXT; i++)
            t3[i].join();
#endif

        // Use this to check serialization
        // cmp_status1 = CMP_ConvertTexture(&srcTexture,  &destTexture,  &options,  &CompressionCallback, NULL, NULL);
        // cmp_status2 = CMP_ConvertTexture(&srcTexture2, &destTexture2, &options2, &CompressionCallback, NULL, NULL);

        //------------------------
        // Free up the BC7 Encoder
        //------------------------
        //if (destTexture.format == CMP_FORMAT_BC7)
        //{
        //    CMP_ShutdownBCLibrary();
        //}

    } 
    catch (const std::exception& ex)
    {
        std::printf("Error: %s\n",ex.what());
    }

    if (cmp_status1 == CMP_OK)
        SaveDDSFile(pszDestFile, destTexture);

    if (cmp_status2 == CMP_OK)
        SaveDDSFile(pszDestFile2, destTexture2);

#ifdef TEST_REPEATED_THREADS
    std::string str;
    if (cmp_status3 == CMP_OK)
    {
        for (int i = 0; i < MXT; i++)
        {
                str.clear();
                str.append("results");
                str.append(std::to_string(i + 10).c_str());
                str.append(".dds");
                SaveDDSFile(str.c_str(), destTexture3[i]);
       }
    }
#endif

    std::printf("\n");

    if (srcTexture.pData)  free(srcTexture.pData);
    if (destTexture.pData) free(destTexture.pData);

    if (srcTexture2.pData)  free(srcTexture2.pData);
    if (destTexture2.pData) free(destTexture2.pData);


#ifdef TEST_REPEATED_THREADS
    if (srcTexture3.pData)  free(srcTexture3.pData);
    for (int i = 0; i < MXT; i++)
    {
        if (destTexture3[i].pData) free(destTexture3[i].pData);
    }
#endif


#endif

#ifdef USE_EXAMPLE3
    CMP_ERROR   cmp_status;

    // Example 2 : Using Low level Block Access code valid only for BC6H and BC7
    if (destTexture.format == CMP_FORMAT_BC7)
    {

        // Step 1: Initialize the Codec: Need to call it only once, repeated calls will return BC_ERROR_LIBRARY_ALREADY_INITIALIZED
        if (CMP_InitializeBCLibrary() != BC_ERROR_NONE)
        {
            std::printf("BC Codec already initialized!\n");
        }

        // Step 2: Create a BC7 Encoder
        BC7BlockEncoder *BC7Encoder;

        // Note we are setting quality low for faster encoding in this sample
        CMP_CreateBC7Encoder(0.05, 0, 0, 0xFF, 1, &BC7Encoder);

        // Pointer to source data
        CMP_BYTE *pdata = (CMP_BYTE *)srcTexture.pData;

        const CMP_DWORD dwBlocksX = ((srcTexture.dwWidth  + 3) >> 2);
        const CMP_DWORD dwBlocksY = ((srcTexture.dwHeight + 3) >> 2);
        const CMP_DWORD dwBlocksXY = dwBlocksX*dwBlocksY;
        
        CMP_DWORD dstIndex  = 0;    // Destination block index
        CMP_DWORD srcIndex  = 0;    // Source block index

        // Step 4: Process the blocks
        for (CMP_DWORD j = 0; j < dwBlocksY; j++)
        {

            for (CMP_DWORD i = 0; i < dwBlocksX; i++)
            {
                // Get a input block of data to encode
                // Currently the BC7 encoder is using Double (This will change to Byte in future releases)
                double blockToEncode[16][4];
                for (int row = 0; row < 4; row++)
                {
                    for (int col = 0; col < 4; col++)
                    {
                        blockToEncode[row*4 + col][BC_COMP_RED]   = (double)*(pdata+srcIndex);
                        blockToEncode[row*4 + col][BC_COMP_GREEN] = (double)*(pdata+srcIndex + 1);
                        blockToEncode[row*4 + col][BC_COMP_BLUE]  = (double)*(pdata+srcIndex + 2);
                        blockToEncode[row*4 + col][BC_COMP_ALPHA] = (double)*(pdata+srcIndex + 3);
                        srcIndex += 4;
                    }
                }

                // Call the block encoder : output is 128 bit compressed data
                cmp_status = CMP_EncodeBC7Block(BC7Encoder, blockToEncode, (destTexture.pData + dstIndex));
                if (cmp_status != CMP_OK)
                {
                    std::printf("Compression error at block X = %d Block Y = %d \n", i,j);
                    i = dwBlocksX;
                    j = dwBlocksY;
                }
                dstIndex += 16;

                // Show Progress
                float fProgress = 100.f * (j * dwBlocksX) / dwBlocksXY;

                std::printf("\rCompression progress = %2.0f", fProgress);

            }
        }

        // Step 5 Free up the BC7 Encoder
        CMP_DestroyBC7Encoder(BC7Encoder);

        // Step 6 Close the BC Codec
        CMP_ShutdownBCLibrary();
    }

    if (cmp_status == CMP_OK)
        SaveDDSFile(pszDestFile, destTexture);

    if (srcTexture.pData)  free(srcTexture.pData);
    if (destTexture.pData) free(destTexture.pData);
#endif
   #ifdef _WIN32
   std::printf("\nProcessed in %.3f seconds\n", timeStampsec() - start_time);
   #endif
   return 0;
}
