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

// Core SDK Example
// Sample application to process a 4x4 image block using Compress and Decompress API's
//
// SDK files required for application to build:
//     CMP_Core.h
//     CMP_Core_xx.lib  For static libs xx is either MD, MT or MDd or MTd,
//                      When using DLL's make sure the  CMP_Core_xx_DLL.dll is in exe path
//

#include <stdio.h>
#include <string>
#include "cmp_core.h"

//------------------------------------------------------------------------------------------------
// Sample 4x4 shapes first sample is a shape0 pattern (1's and 0's) used for BC6 and BC7 encoding.
// These pattern blocks can be used to validate the BCn codecs compression and decompression
// functions.
// By default all BCn codecs are set for high quality encoding. To change this use the
// Set Options. Examples are provided on how to use them for each group of BCn codecs.
//-------------------------------------------------------------------------------------------------
// Test shape0 for  BC1, BC2, BC3, BC7
unsigned char shape0_RGBA[64] = {
// GREEN          GREEN        RED             RED           Pixels
    0,255, 0,255,  0,255, 0,255,  255, 0, 0,255,  255, 0, 0,255,
    0,255, 0,255,  0,255, 0,255,  255, 0, 0,255,  255, 0, 0,255,
    0,255, 0,255,  0,255, 0,255,  255, 0, 0,255,  255, 0, 0,255,
    0,255, 0,255,  0,255, 0,255,  255, 0, 0,255,  255, 0, 0,255
};

// Test shape0 for BC6H RGB Half Type 16bit (No Alpha)
unsigned short shape0_RGBF16[48] = {
    //GREEN          GREEN       RED           RED      Pixels
    0,15128,0,    0,15128,0,    15128,0,0,    15128,0,0,
    0,15128,0,    0,15128,0,    15128,0,0,    15128,0,0,
    0,15128,0,    0,15128,0,    15128,0,0,    15128,0,0,
    0,15128,0,    0,15128,0,    15128,0,0,    15128,0,0
};

// Test shapes for BC4 & BC5 Encoding by channels, data set so that ramps return 0 errors for ecncoder testing
unsigned char shape_1[16]  = { 18, 21, 21,13,18,10,16,5,5,21,16,21,21,7, 7, 2 };
unsigned char shape_2[16]  = { 12, 126, 45,28,45,61,28,12,28,12,45,28,28,77,61,61 };
char          shape_1s[16] = {-19,-19, 20,-13, 20,  9,-13,  3,  3, 20,-13,-19, 20,  9,  9, -1};
char          shape_2s[16] = {-120, -18, 14, -120, 14, 14, -120, 14, 14, 14, -18, -18, 115, 14, 14, -120};

 void ShowResults1(char *testcodec, unsigned char src_1[16], unsigned char decomp_1[]) {
    std::printf("\n[%s]\n", testcodec);
    // show the first row of pixels
    std::printf("original   : (%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d)\n",
                src_1[0], src_1[1], src_1[2],  src_1[3],
                src_1[4], src_1[5], src_1[6],  src_1[7],
                src_1[8], src_1[9], src_1[10], src_1[11],
                src_1[12], src_1[13], src_1[14], src_1[15]);

    std::printf("decompress : (%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d)\n",
                decomp_1[0],  decomp_1[1],  decomp_1[2],  decomp_1[3],
                decomp_1[4],  decomp_1[5],  decomp_1[6],  decomp_1[7],
                decomp_1[8],  decomp_1[9],  decomp_1[10], decomp_1[11],
                decomp_1[12], decomp_1[13], decomp_1[14], decomp_1[15]);

    // Calculate a sum of image diffs:to see how well the codec compressed
    int diffSum = 0;
    for (int i = 0; i < 16; i++) {
        diffSum += abs(src_1[i] - decomp_1[i]);
    }
    std::printf("Data Diff Sum = %d\n", diffSum);
}

void ShowResults1s(char *testcodec, char src_1[], char decomp_1[]) {
    std::printf("\n[%s]\n", testcodec);
    // show the first row of pixels
    std::printf("original   : (%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d)\n",
                src_1[0], src_1[1], src_1[2],  src_1[3],
                src_1[4], src_1[5], src_1[6],  src_1[7],
                src_1[8], src_1[9], src_1[10], src_1[11],
                src_1[12], src_1[13], src_1[14], src_1[15]);

    std::printf("decompress : (%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d)\n",
                decomp_1[0],  decomp_1[1],  decomp_1[2],  decomp_1[3],
                decomp_1[4],  decomp_1[5],  decomp_1[6],  decomp_1[7],
                decomp_1[8],  decomp_1[9],  decomp_1[10], decomp_1[11],
                decomp_1[12], decomp_1[13], decomp_1[14], decomp_1[15]);

    // Calculate a sum of image diffs:to see how well the codec compressed
    int diffSum = 0;
    for (int i = 0; i < 16; i++) {
        diffSum += abs(src_1[i] - decomp_1[i]);
    }
    std::printf("Data Diff Sum = %d\n", diffSum);
}

void ShowResults(char *testcodec, unsigned char src_RGBA[], unsigned char decomp_RGBA[]) {
    std::printf("\n[%s]\n", testcodec);
    // show the first row of pixels
    std::printf("original   : (%3d,%3d,%3d,%3d) (%3d,%3d,%3d,%3d) (%3d,%3d,%3d,%3d) (%3d,%3d,%3d,%3d)\n",
                src_RGBA[0], src_RGBA[1], src_RGBA[2], src_RGBA[3],
                src_RGBA[4], src_RGBA[5], src_RGBA[6], src_RGBA[7],
                src_RGBA[8], src_RGBA[9], src_RGBA[10], src_RGBA[11],
                src_RGBA[12], src_RGBA[13], src_RGBA[14], src_RGBA[15]);

    std::printf("decompress : (%3d,%3d,%3d,%3d) (%3d,%3d,%3d,%3d) (%3d,%3d,%3d,%3d) (%3d,%3d,%3d,%3d)\n",
                decomp_RGBA[0], decomp_RGBA[1], decomp_RGBA[2], decomp_RGBA[3],
                decomp_RGBA[4], decomp_RGBA[5], decomp_RGBA[6], decomp_RGBA[7],
                decomp_RGBA[8], decomp_RGBA[9], decomp_RGBA[10], decomp_RGBA[11],
                decomp_RGBA[12], decomp_RGBA[13], decomp_RGBA[14], decomp_RGBA[15]);

    // Calculate a sum of image diffs:to see how well the codec compressed
    int diffSum = 0;
    for (int i = 0; i < 64; i++) {
        diffSum += abs(src_RGBA[i] - decomp_RGBA[i]);
    }
    std::printf("Image Diff Sum = %d\n", diffSum);
}

void ShowResultsBC6(char* testcodec, unsigned short src_RGBA[], unsigned short decomp_RGBA[])
{
    std::printf("\n[%s]\n", testcodec);
    // show the first row of pixels
    std::printf(
        "original   : (%5d,%5d,%5d), (%5d,%5d,%5d) (%5d,%5d,%5d) (%5d,%5d,%5d)\n",
        src_RGBA[0], src_RGBA[1 ], src_RGBA[2],
        src_RGBA[3], src_RGBA[4 ], src_RGBA[5],
        src_RGBA[6], src_RGBA[7 ], src_RGBA[8],
        src_RGBA[9], src_RGBA[10], src_RGBA[11]);

    std::printf(
        "decompress : (%5d,%5d,%5d), (%5d,%5d,%5d) (%5d,%5d,%5d) (%5d,%5d,%5d)\n",
        decomp_RGBA[0], decomp_RGBA[1 ], decomp_RGBA[2],
        decomp_RGBA[3], decomp_RGBA[4 ], decomp_RGBA[5],
        decomp_RGBA[6], decomp_RGBA[7 ], decomp_RGBA[8],
        decomp_RGBA[9], decomp_RGBA[10], decomp_RGBA[11]);

    // Calculate a sum of image diffs:to see how well the codec compressed
    int diffSum = 0;
    for (int i = 0; i < 48; i++) {
        diffSum += abs(src_RGBA[i] - decomp_RGBA[i]);
    }
    std::printf("Image Diff Sum = %d\n", diffSum);
}

int main(int argc, char* argv[]) {

    unsigned char  imgBuffer[64]     = {0};  // Results buffer for decompressed  shape0_RGBA
    unsigned short imgBufferF16[48]  = {0};  // Results buffer for decompressed  shape0_RGBAF16
    unsigned char  imgBuffer1[16]    = {0};  // Results buffer for decompressed  single channel 1
    unsigned char  imgBuffer2[16]    = {0};  // Results buffer for decompressed  single channel 2
    char           imgBuffer1s[16]   = {0};  // Results buffer for decompressed  single signed channel
    char           imgBuffer2s[16]   = {0};  // Results buffer for decompressed  single signed channel

    unsigned char  cmpBuffer8[8]     = {0};  // Compression buffer for BC1 and BC4 Codecs
    unsigned char  cmpBuffer16[16]   = {0};  // Compression buffer for BC5,BC6 and BC7
             char  cmpBuffer8s[8]    = {0};  // Compression buffer for BC4s Codecs

    //===================================================================
    // Example #1 of how to set compress and decompress for BCn codecs
    //===================================================================

    //==================================================================================
    // BC1 Example: using default options settings
    // This example show 2 ways to call the compress and decompress API
    // one is using C++ convention with n optional parameter, which is null
    // for "C" compilers the third parameters must be set to null
    // Examples to use the third option parameter is shown in Example #2 and Example #3
    // For the remaining codec examples, "C" parameter examples are shown
    // They will work for C++ compilers also
    //==================================================================================

#ifdef __cplusplus
    CompressBlockBC1(shape0_RGBA,16,cmpBuffer8);
    DecompressBlockBC1(cmpBuffer8,imgBuffer);
#else
    CompressBlockBC1(shape0_RGBA,16,cmpBuffer8,NULL);
    DecompressBC1(cmpBuffer8, imgBuffer,NULL);
#endif
    ShowResults("BC1",shape0_RGBA, imgBuffer);

    //============
    // BC2 Example
    //=============
    CompressBlockBC2(shape0_RGBA,16,cmpBuffer16, NULL);
    DecompressBlockBC2(cmpBuffer16, imgBuffer, NULL);
    ShowResults("BC2",shape0_RGBA, imgBuffer);

    //=============
    // BC3 Example
    //=============
    CompressBlockBC3(shape0_RGBA,16,cmpBuffer16, NULL);
    DecompressBlockBC3(cmpBuffer16, imgBuffer, NULL);
    ShowResults("BC3",shape0_RGBA, imgBuffer);

    //==============
    // BC4 Example
    //==============
    CompressBlockBC4(shape_1,4, cmpBuffer8, NULL);
    DecompressBlockBC4(cmpBuffer8, imgBuffer, NULL);
    ShowResults1("BC4", shape_1, imgBuffer);

    //==============================
    // BC4 Signed Channel Example
    //==============================
    CompressBlockBC4S((const char*)shape_1s, 4, cmpBuffer8, NULL);
    DecompressBlockBC4S(cmpBuffer8, imgBuffer1s, NULL);
    ShowResults1s("BC4_S", shape_1s, imgBuffer1s);

    //==============
    // BC5 Example
    //==============
    CompressBlockBC5(shape_1,4, shape_2,4, cmpBuffer16, NULL);
    DecompressBlockBC5(cmpBuffer16, imgBuffer1, imgBuffer2, NULL);
    ShowResults1("BC5 R", shape_1, imgBuffer1);
    ShowResults1("BC5 G", shape_2, imgBuffer2);

    //============================
    // BC5 Signed Channel Example
    //============================
    CompressBlockBC5S(shape_1s, 4, shape_2s, 4, cmpBuffer16, NULL);
    DecompressBlockBC5S(cmpBuffer16, imgBuffer1s, imgBuffer2s, NULL);
    ShowResults1s("BC5_S R", shape_1s, imgBuffer1s);
    ShowResults1s("BC5_S G", shape_2s, imgBuffer2s);

    //=============
    // BC6 Example
    //=============
    CompressBlockBC6(shape0_RGBF16, 12, cmpBuffer16, NULL);
    DecompressBlockBC6(cmpBuffer16,imgBufferF16, NULL);
    ShowResultsBC6("BC6",shape0_RGBF16, imgBufferF16);

    //=============
    // BC7 Example
    //=============
    CompressBlockBC7(shape0_RGBA, 16, cmpBuffer16, NULL);
    DecompressBlockBC7(cmpBuffer16,imgBuffer, NULL);
    ShowResults("BC7",shape0_RGBA, imgBuffer);

    //============================================
    // Example #2 of how to set options for BC1
    //============================================
    {
        // First create an options context
        void *BC15Options;
        CreateOptionsBC1(&BC15Options);

        //check it was successful
        if (BC15Options == NULL) {
            printf("Failed to create BC1 Options context!");
            return (-1);
        }

        // Set Quality
        SetQualityBC1(BC15Options, 1.0f);

        // Setting channel weights {Red,Green,Blue}
        SetChannelWeightsBC1(BC15Options, 0.3086f, 0.6094f, 0.0820f);

        CompressBlockBC1(shape0_RGBA, 16, cmpBuffer8, BC15Options);
        DecompressBlockBC1(cmpBuffer8,imgBuffer,BC15Options);
        ShowResults("BC1 options", shape0_RGBA, imgBuffer);

        DestroyOptionsBC1(BC15Options);
    }

    //============================================
    // Example #3 of how to set options for BC7
    // The process is the same for for BC6
    //=============================================
    {
        // First create an options context
        void *BC7Options;
        CreateOptionsBC7(&BC7Options);

        //check it was successful
        if (BC7Options == NULL) {
            printf("Failed to create BC7 Options context!");
            return (-1);
        }

        // Set quality to low = 0.05, default quality is high = 1.0f
        // valid ranges are 0.0f to 1.0f
        SetQualityBC7(BC7Options,0.05f);

        // BC7 has 8 compressed block modes,
        // by default, all 8 modes are enabled
        // you can select which modes are on or off by using a mode mask
        // each mode is a bit set in the mask and is assigned as shown
        // MSB = Most Significant Bit
        // LSB = Least Significant Bit
        // Encoding modes                  MSB 76543210 LSB
        //                        Bit Mask = 0b11111111 = 0xFF

        // Example use only modes 1 & 6
        SetMaskBC7(BC7Options,0b01000010);

        // Now compress the block
        CompressBlockBC7(shape0_RGBA,16, cmpBuffer16, BC7Options);

        // Decompress the block
        DecompressBlockBC7(cmpBuffer16, imgBuffer);

        // Show results of low quality encoding and reduced modes, expect small errors
        ShowResults("BC7 mode 1&6 only. Loss in quality expected", shape0_RGBA, imgBuffer);

        // cleanup the options context
        DestroyOptionsBC7(BC7Options);
    }

    return 0;
}
