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

// There are two examples of how to compress a source image 
// 1st is using high level SDK API's optimized with MultiThreading
// 2nd is an example of low level API that give access to compression blocks (4x4) for BC6H and BC7
 
// Comment out USE_EXAMPLE1 if you want to try BC7 with low level access
#define USE_EXAMPLE1 


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

bool g_bAbortCompression = false;   // If set true current compression will abort

//---------------------------------------------------------------------------
// Sample loop back code called for each compression block been processed
//---------------------------------------------------------------------------
bool CompressionCallback(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    UNREFERENCED_PARAMETER(pUser1);
    UNREFERENCED_PARAMETER(pUser2);
    printf("\rCompression progress = %2.0f", fProgress);
    return g_bAbortCompression;
}

int main(int argc, const char* argv[])
{
    if (argc < 5)
    {
        printf("Compressonator_Test SourceFile DestFile Format Quality\n");
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
        printf("Unsupported destination format\n");
        return 0;
    }

    // Load the source texture
    CMP_Texture srcTexture;
    if (!LoadDDSFile(pszSourceFile, srcTexture))
    {
        printf("Error loading source file!\n");
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

    CMP_CompressOptions options;
    memset(&options, 0, sizeof(options));
    options.dwSize = sizeof(options);

    // Option 1 of setting compression options - Prefered Method
    snprintf(options.CmdSet[0].strCommand, sizeof(AMD_CMD_SET::strCommand), "Quality");
    snprintf(options.CmdSet[0].strParameter, sizeof(AMD_CMD_SET::strParameter), "%s", argv[4]);  // Use user specified Quality (lower values increases performance)
    snprintf(options.CmdSet[1].strCommand, sizeof(AMD_CMD_SET::strCommand), "ModeMask");
    snprintf(options.CmdSet[1].strParameter, sizeof(AMD_CMD_SET::strParameter), "207");          // 0xCF
    snprintf(options.CmdSet[2].strCommand, sizeof(AMD_CMD_SET::strCommand), "NumThreads");     // Use Multi Threading for fast performance
    snprintf(options.CmdSet[2].strParameter, sizeof(AMD_CMD_SET::strParameter), "8");
    options.NumCmds = 3;

    // Option 2 of setting compression options - For backward compatibility  (will be removed in future releases)
    // options.fquality             = fQuality;
    // options.dwmodeMask           = 0xFF;
    // options.dwnumThreads         = 8;

    // Example 1 : Using SDK API
#ifdef USE_EXAMPLE1
    CMP_ERROR  cmp_status;
    cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, &options, &CompressionCallback, NULL, NULL);
    if (cmp_status != CMP_OK)
    {
        if (srcTexture.pData)  free(srcTexture.pData);
        if (destTexture.pData) free(destTexture.pData);
        printf("Compression returned an error %d\n", cmp_status);
        return 0;
    }

#else
    BC_ERROR  cmp_status;
    // Example 2 : Using Low level Block Access code valid only for BC6H and BC7
    if (destTexture.format == CMP_FORMAT_BC7)
    {

        // Step 1: Initialize the Codec: Need to call it only once, repeated calls will return BC_ERROR_LIBRARY_ALREADY_INITIALIZED
        if (CMP_InitializeBCLibrary() != BC_ERROR_NONE)
        {
            printf("BC Codec already initialized!\n");
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
                    printf("Compression error at block X = %d Block Y = %d \n", i,j);
                    i = dwBlocksX;
                    j = dwBlocksY;
                }
                dstIndex += 16;

                // Show Progress
                float fProgress = 100.f * (j * dwBlocksX) / dwBlocksXY;
                printf("\rCompression progress = %2.0f", fProgress);
            }
        }

        // Step 5 Free up the BC7 Encoder
        CMP_DestroyBC7Encoder(BC7Encoder);

        // Step 6 Close the BC Codec
        CMP_ShutdownBCLibrary();
    }
#endif

    if (cmp_status == CMP_OK)
        SaveDDSFile(pszDestFile, destTexture);

   if (srcTexture.pData)  free(srcTexture.pData);
   if (destTexture.pData) free(destTexture.pData);

   return 0;
}
