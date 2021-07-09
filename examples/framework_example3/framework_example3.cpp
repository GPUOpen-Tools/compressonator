// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved
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

// Example3
// Sample application to process a image file using Block Level Compression with a BC7 encoder
//
// SDK files required for application:
//     compressonator.h
//     CMP_Framework_xx.lib  For static libs xx is either MD, MT or MDd or MTd,
//                      When using DLL's make sure the  CMP_Framework_xx_DLL.dll is in exe path
//

#include <stdio.h>
#include <string>
#include "compressonator.h"

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

bool g_bAbortCompression = false;   // If set true current compression will abort

//---------------------------------------------------------------------------
// Sample loop back code called for each compression block been processed
//---------------------------------------------------------------------------
bool CompressionCallback(CMP_FLOAT fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
    (pUser1);
    (pUser2);

    std::printf("\rCompression progress = %3.0f  ", fProgress);

    return g_bAbortCompression;
}

int main(int argc, char* argv[]) {

    if (argc < 4) {
        std::printf("Example3.exe SourceFile DestFile Quality\n");
        std::printf("This example shows how to compress a single image\n");
        std::printf("using block level encoding with BC7\n");
        std::printf("Quality is in the range of 0.0 to 1.0\n");
        std::printf("When using DLL builds make sure the  CMP_Framework_xx_DLL.dll is in exe path\n");
        std::printf("usage: Example3.exe sample.dds result_bc7.dds 1.0\n");
        std::printf("this will generate a high quality compressed ruby file in BC7 format\n");
        return 0;
    }

    // please note the params are not checked for errors
    const char*     pszSourceFile = argv[1];
    const char*     pszDestFile = argv[2];
    CMP_FLOAT       fQuality;

    try {
        fQuality = std::stof(argv[3]);
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

    //--------------------------
    // Init frameworks
    // plugin and IO interfaces
    //--------------------------
    CMP_InitFramework();

    //=====================================================================================================
    // You can optionally use a 4th argv to set a destFormat other then the example BC7
    // just add additional encoder dll's in the example binary path.
    // example: to use BC1 add CMP_BC1_MD.dll.
    // You can edit CopyFiles.bat to add additional libs to the correct build folders of this application.
    //
    // CMP_FORMAT      destFormat    = CMP_ParseFormat(argv[4]);
    //======================================================================================================
    CMP_FORMAT      destFormat = CMP_FORMAT_BC7;

    //---------------
    // Load the image
    //---------------
    CMP_MipSet MipSetIn;
    memset(&MipSetIn, 0, sizeof(CMP_MipSet));
    if (CMP_LoadTexture(pszSourceFile, &MipSetIn) != CMP_OK) {
        std::printf("Error: Loading source file %s\n", pszSourceFile);
        return -1;
    }

    //-----------------------------------------------------
    // Check texture for width and height as multiple of 4
    //-----------------------------------------------------
    if ((MipSetIn.m_nWidth % 4) > 0 || (MipSetIn.m_nHeight % 4) > 0) {
        std::printf("Error: Texture width and height must be multiple of 4\n");
        return -1;
    }

    //----------------------------------
    // Check we have a image  buffer
    //----------------------------------
    if (MipSetIn.pData == NULL) {
        std::printf("Error: Texture buffer was not allocated\n");
        return -1;
    }


    // Setup a results buffer for the processed file,
    CMP_MipSet MipSetCmp;
    memset(&MipSetCmp, 0, sizeof(CMP_MipSet));
    MipSetCmp.m_format = CMP_FORMAT_BC7;   // Set a destination format

    // Crate a destination buffer based on destination format and MipSet source input
    CMP_CreateCompressMipSet(&MipSetCmp, &MipSetIn);

    //----------------------------------
    // Check we have am image  buffer
    //----------------------------------
    if (MipSetCmp.pData == NULL) {
        std::printf("Error: Destination buffer was not allocated\n");
        return -1;
    }

    //----------------------------------------------------------------
    // Compress the source image
    //----------------------------------------------------------------
    CMP_ERROR cmp_status;

#ifdef _WIN32
    double process_start_time = timeStampsec();
#endif

    void *BC7block_encoder;
    BC7block_encoder = NULL;
    CMP_EncoderSetting encodeSettings;
    encodeSettings.format   = CMP_FORMAT_BC7;
    encodeSettings.quality  = fQuality;
    encodeSettings.width    = MipSetIn.m_nWidth;
    encodeSettings.height   = MipSetIn.m_nHeight;

    if (CMP_CreateBlockEncoder(&BC7block_encoder, encodeSettings) != 0) {
        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);
        std::printf("Error: Creating BC7 block encoder");
        return -1;
    }

    //---------------------------------------------------
    // Compress the source image in blocks of 4x4 Texels
    //---------------------------------------------------
    CMP_UINT blockWidth = MipSetIn.m_nWidth / 4;
    CMP_UINT blockHeight = MipSetIn.m_nHeight / 4;
    CMP_UINT srcStride   = MipSetIn.m_nWidth * 4;
    CMP_UINT dstStride   = MipSetIn.m_nWidth * 8;

    for (CMP_UINT y = 0; y < blockHeight; y++)
        for (CMP_UINT x = 0; x < blockWidth; x++) {
            std::printf("Processing block %3d %3d \r", x, y);
            //------------------------------------------------------------------------------------------------------------
            // Note that src and dst pointers are not changed and remain at a fixed offset for input
            // Compress(x,y,void *src, void *dst) will calculate the correct offset for the src data and destination data
            // buffers based on the compressed block size for the codec, that was set in CMP_CreateBlockEncoder
            // CMP_CompressBlock(void *src, void *dst) can also be used to pass down varying src and dst data pointers
            //-------------------------------------------------------------------------------------------------------------
            if (CMP_CompressBlockXY(&BC7block_encoder, x, y, (void *)MipSetIn.pData, srcStride, (void *)MipSetCmp.pData, dstStride) != 0) {
                std::printf("\nError processing block (%d,%d)", x, y);
                CMP_FreeMipSet(&MipSetIn);
                CMP_FreeMipSet(&MipSetCmp);
                return -1;
            }
        }

    CMP_DestroyBlockEncoder(&BC7block_encoder);

#ifdef _WIN32
    double process_end_time = timeStampsec();
#endif

    //--------------------------
    // Save the result to a file
    //--------------------------
    cmp_status = CMP_SaveTexture(pszDestFile, &MipSetCmp);

    //-----------------
    // Clean up buffers
    //-----------------
    CMP_FreeMipSet(&MipSetIn);
    CMP_FreeMipSet(&MipSetCmp);

    if (cmp_status != CMP_OK) {
        std::printf("Error %d: Saving processed file %s!\n", cmp_status, pszDestFile);
        return -1;
    }

#ifdef _WIN32
    std::printf("\nProcessed in %.3f seconds\n", process_end_time - process_start_time);
#else
    std::printf("Process Done \n");
#endif
    return 0;
}


