// Copyright (c) 2023-2024 Advanced Micro Devices, Inc. All rights reserved
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

// Example 5
// Sample showing how to call Compressonator compression functions using external texture data
//
// SDK files required for application:
//     compressonator.h
//     CMP_Framework_xx.lib  For static libs xx is either MD, MT or MDd or MTd,
//                      When using DLL's make sure the  CMP_Framework_xx_DLL.dll is in exe path
//

#include <stdio.h>
#include <stdint.h>
#include <string>

#include "compressonator.h"

int main(int argc, char** argv)
{
    //-------------------------------------
    // Initialize example external texture data
    //-------------------------------------

    const int inputWidth        = 256;
    const int inputHeight       = 256;
    const int inputPixelCount   = inputWidth * inputHeight;
    const int inputChannelCount = 4;

    int8_t* inputData = (int8_t*)calloc(1, inputPixelCount * inputChannelCount);

    for (int i = 0; i < inputPixelCount; ++i)
    {
        inputData[i * inputChannelCount + 0] = 0xFF;
        inputData[i * inputChannelCount + 1] = 0xFF;
        inputData[i * inputChannelCount + 2] = 0x00;
        inputData[i * inputChannelCount + 3] = 0xFF;
    }

    //--------------------------
    // Init framework plugin and IO interfaces
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
    CMP_FORMAT destFormat = CMP_FORMAT_BC7;

    //---------------
    // Create the MipSet to hold the input data
    //---------------
    CMP_MipSet inputTexture = {};
    if (CMP_CreateMipSet(&inputTexture, inputWidth, inputHeight, 1, CF_8bit, TT_2D) != CMP_OK)
    {
        std::printf("Error: Failed to create mipset\n");
        return -1;
    }

    //----------------------------------
    // Copy external data into the created MipSet
    //----------------------------------
    CMP_MipLevel* baseLevel = 0;
    CMP_GetMipLevel(&baseLevel, &inputTexture, 0, 0);

    if (baseLevel == 0)
    {
        printf("Error: MipSet data allocation failed.\n");
        return -1;
    }

    memcpy(baseLevel->m_pbData, inputData, inputPixelCount * inputChannelCount);

    // Setup a results buffer for the processed data
    CMP_MipSet resultTexture = {};
    resultTexture.m_format   = destFormat;

    // Crate a destination buffer based on destination format and MipSet source input
    CMP_CreateCompressMipSet(&resultTexture, &inputTexture);

    //----------------------------------
    // Check we have am image  buffer
    //----------------------------------
    if (resultTexture.pData == NULL)
    {
        std::printf("Error: Destination buffer was not allocated\n");
        return -1;
    }

    //----------------------------------------------------------------
    // Compress the source image
    //----------------------------------------------------------------
    CMP_ERROR status = CMP_OK;

    void* bc7Encoder = 0;

    CMP_EncoderSetting encodeSettings = {};
    encodeSettings.format             = CMP_FORMAT_BC7;
    encodeSettings.quality            = 0.5f;
    encodeSettings.width              = inputTexture.m_nWidth;
    encodeSettings.height             = inputTexture.m_nHeight;

    if (CMP_CreateBlockEncoder(&bc7Encoder, encodeSettings) != 0)
    {
        CMP_FreeMipSet(&inputTexture);
        CMP_FreeMipSet(&resultTexture);
        std::printf("Error: Creating BC7 block encoder");
        return -1;
    }

    //---------------------------------------------------
    // Compress the source image in blocks of 4x4 Texels
    //---------------------------------------------------
    CMP_UINT numBlocksX = inputTexture.m_nWidth / 4;
    CMP_UINT numBlocksY = inputTexture.m_nHeight / 4;
    CMP_UINT srcStride  = inputTexture.m_nWidth * 4;
    CMP_UINT dstStride  = resultTexture.m_nWidth * 8;

    for (CMP_UINT y = 0; y < numBlocksY; ++y)
    {
        for (CMP_UINT x = 0; x < numBlocksX; ++x)
        {
            std::printf("Processing block %3d %3d \r", x, y);
            //------------------------------------------------------------------------------------------------------------
            // Note that src and dst pointers are not changed and remain at a fixed offset for input
            // Compress(x,y,void *src, void *dst) will calculate the correct offset for the src data and destination data
            // buffers based on the compressed block size for the codec, that was set in CMP_CreateBlockEncoder
            // CMP_CompressBlock(void *src, void *dst) can also be used to pass down varying src and dst data pointers
            //-------------------------------------------------------------------------------------------------------------
            if (CMP_CompressBlockXY(&bc7Encoder, x, y, (void*)inputTexture.pData, srcStride, (void*)resultTexture.pData, dstStride) != 0)
            {
                std::printf("\nError processing block (%d,%d)", x, y);
                CMP_FreeMipSet(&inputTexture);
                CMP_FreeMipSet(&resultTexture);
                return -1;
            }
        }
    }

    CMP_DestroyBlockEncoder(&bc7Encoder);

    //--------------------------
    // Save the result to a file
    //--------------------------
    const char* destFileName = "result.dds";
    status                   = CMP_SaveTexture(destFileName, &resultTexture);

    //-----------------
    // Clean up buffers
    //-----------------
    CMP_FreeMipSet(&inputTexture);
    CMP_FreeMipSet(&resultTexture);

    if (status != CMP_OK)
    {
        std::printf("Error %d: Saving processed file %s\n", status, destFileName);
        return -1;
    }

    std::printf("Processing Done\n");

    return 0;
}
