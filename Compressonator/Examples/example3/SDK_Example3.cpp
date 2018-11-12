// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved
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
// Example3: Console application that demonstrates how to use the block level SDK API
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

// Example of low level API that give access to compression blocks (4x4) for BC7
 
#define USE_EXAMPLE3

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
        std::printf("Compressonator_Test SourceFile DestFile Quality\n");
        std::printf("This example shows how to compress a single image\n");
        std::printf("to a BC7 compression format using single threaded low level\n");
        std::printf("compression blocks access with a quality setting\n");
        std::printf("usage: Example3.exe ruby.dds ruby_bc7.dds 0.05\n");
        std::printf("this will generate a compressed ruby file in BC7 format\n");
        return 0;
    }

    // Note: No error checking is done on user arguments, so all parameters must be correct in this example
    // (files must exist, values correct format, etc..)
    const char*     pszSourceFile = argv[1];
    const char*     pszDestFile   = argv[2];
    CMP_FORMAT      destFormat    = CMP_FORMAT_BC7;

    // Load the source texture
    CMP_Texture srcTexture;
    if (!LoadDDSFile(pszSourceFile, srcTexture))
    {
        std::printf("Error loading source file!\n");

        return 0;
    }

    // Init dest memory to use for compressed texture
    CMP_Texture destTexture;
    destTexture.dwSize     = sizeof(destTexture);
    destTexture.dwWidth    = srcTexture.dwWidth;
    destTexture.dwHeight   = srcTexture.dwHeight;
    destTexture.dwPitch    = 0;
    destTexture.format     = destFormat;
    destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
    destTexture.pData = (CMP_BYTE*)malloc(destTexture.dwDataSize);

    BC_ERROR   cmp_status;

    // Example 2 : Using Low level block access code valid only BC7 (and BC6H not shown in this example)
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
        CMP_CreateBC7Encoder(
                             0.05,               // Quality set to low
                             0,                  // Do not restrict colors
                             0,                  // Do not restrict alpha
                             0xFF,               // Use all BC7 modes
                             1,                  // Performance set to optimal
                             &BC7Encoder);

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
                // Currently the BC7 encoder is using double data formats
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
                if (cmp_status != BC_ERROR_NONE)
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

    // Save the results
    if (cmp_status == CMP_OK)
        SaveDDSFile(pszDestFile, destTexture);

    // Clean up memory used for textures
    free(srcTexture.pData);
    free(destTexture.pData);

    #ifdef _WIN32
   std::printf("\nProcessed in %.3f seconds\n", timeStampsec() - start_time);
    #endif
    return 0;
}
