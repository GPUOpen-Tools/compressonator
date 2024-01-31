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
#include <stdint.h>

#include "cmp_core.h"

static uint32_t srcBlock[] = {0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF,
                              0xFF00FFFF};

static uint16_t srcBlockShort[] = {0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF,
                                   0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF,
                                   0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF,
                                   0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF};

int main(int argc, char** argv)
{
    void* bc1Options = 0;
    void* bc2Options = 0;
    void* bc3Options = 0;
    void* bc4Options = 0;
    void* bc5Options = 0;
    void* bc6Options = 0;
    void* bc7Options = 0;

    // CreateOptionsBCN block
    {
        if (CreateOptionsBC1(&bc1Options) != 0)
        {
            printf("Failed to create and initialize BC1 Options struct.\n");
            return -1;
        }

        if (CreateOptionsBC2(&bc2Options) != 0)
        {
            printf("Failed to create and initialize BC2 Options struct.\n");
            return -1;
        }

        if (CreateOptionsBC3(&bc3Options) != 0)
        {
            printf("Failed to create and initialize BC3 Options struct.\n");
            return -1;
        }

        if (CreateOptionsBC4(&bc4Options) != 0)
        {
            printf("Failed to create and initialize BC4 Options struct.\n");
            return -1;
        }

        if (CreateOptionsBC5(&bc5Options) != 0)
        {
            printf("Failed to create and initialize BC5 Options struct.\n");
            return -1;
        }

        if (CreateOptionsBC6(&bc6Options) != 0)
        {
            printf("Failed to create and initialize BC6 Options struct.\n");
            return -1;
        }

        if (CreateOptionsBC7(&bc7Options) != 0)
        {
            printf("Failed to create and initialize BC7 Options struct.\n");
            return -1;
        }
    }

    // SetChannelWeightsBCN block
    {
        if (SetChannelWeightsBC1(bc1Options, 0.8f, 1.0f, 0.75f) != 0)
        {
            printf("Failed to set BC1 channel weights.\n");
            return -1;
        }

        if (SetChannelWeightsBC2(bc2Options, 0.8f, 1.0f, 0.75f) != 0)
        {
            printf("Failed to set BC2 channel weights.\n");
            return -1;
        }

        if (SetChannelWeightsBC3(bc3Options, 0.8f, 1.0f, 0.75f) != 0)
        {
            printf("Failed to set BC3 channel weights.\n");
            return -1;
        }
    }

    if (SetDecodeChannelMapping(bc1Options, true) != 0)
    {
        printf("Failed to set the channel mapping.\n");
        return -1;
    }

    // SetQualityBCN block
    {
        if (SetQualityBC1(bc1Options, 0.6f) != 0)
        {
            printf("Failed to set BC1 quality.\n");
            return -1;
        }

        if (SetQualityBC2(bc2Options, 0.6f) != 0)
        {
            printf("Failed to set BC2 quality.\n");
            return -1;
        }

        if (SetQualityBC3(bc3Options, 0.6f) != 0)
        {
            printf("Failed to set BC3 quality.\n");
            return -1;
        }

        if (SetQualityBC4(bc4Options, 0.6f) != 0)
        {
            printf("Failed to set BC4 quality.\n");
            return -1;
        }

        if (SetQualityBC5(bc5Options, 0.6f) != 0)
        {
            printf("Failed to set BC5 quality.\n");
            return -1;
        }

        if (SetQualityBC6(bc6Options, 0.6f) != 0)
        {
            printf("Failed to set BC6 quality.\n");
            return -1;
        }

        if (SetQualityBC7(bc7Options, 0.6f) != 0)
        {
            printf("Failed to set BC7 quality.\n");
            return -1;
        }
    }

    if (SetAlphaThresholdBC1(bc1Options, 10) != 0)
    {
        printf("Failed to set the BC1 alpha threshold.\n");
        return -1;
    }

    if (SetRefineStepsBC1(bc1Options, 5) != 0)
    {
        printf("Failed to set BC1 number of refine steps.\n");
        return -1;
    }

    if (SetMaskBC6(bc6Options, 0xFFFFFFFF) != 0)
    {
        printf("Failed to set BC6 mask value.\n");
        return -1;
    }
    if (SetMaskBC7(bc7Options, 0xFF) != 0)
    {
        printf("Failed to set BC7 mask value.\n");
        return -1;
    }

    if (SetAlphaOptionsBC7(bc7Options, true, false, false) != 0)
    {
        printf("Failed to set BC7 alpha options.\n");
        return -1;
    }

    if (SetErrorThresholdBC7(bc7Options, 0.0f, 1.0f) != 0)
    {
        printf("Failed to set BC7 error threshold.\n");
        return -1;
    }

    // sRGB option block
    {
        if (SetSrgbBC1(bc1Options, false) != 0)
        {
            printf("Failed to set sRGB option for BC1.\n");
            return -1;
        }
        if (SetSrgbBC2(bc2Options, false) != 0)
        {
            printf("Failed to set sRGB option for BC2.\n");
            return -1;
        }
        if (SetSrgbBC3(bc3Options, false) != 0)
        {
            printf("Failed to set sRGB option for BC3.\n");
            return -1;
        }

        if (SetGammaBC1(bc1Options, false) != 0)
        {
            printf("Failed to set old sRGB option for BC1.\n");
            return -1;
        }
        if (SetGammaBC2(bc2Options, false) != 0)
        {
            printf("Failed to set old sRGB option for BC2.\n");
            return -1;
        }
        if (SetGammaBC3(bc3Options, false) != 0)
        {
            printf("Failed to set old sRGB option for BC3.\n");
            return -1;
        }
    }

    if (SetSignedBC6(bc6Options, false))
    {
        printf("Failed to set BC6 signed option.\n");
        return -1;
    }

    if (EnableSSE4() < 0)
    {
        printf("Invalid value returned by EnableSSE4 function.\n");
        return -1;
    }
    if (EnableAVX2() < 0)
    {
        printf("Invalid value returned by EnableAVX2 function.\n");
        return -1;
    }
    if (EnableAVX512() < 0)
    {
        printf("Invalid value returned by EnableAVX512 function.\n");
        return -1;
    }

    if (DisableSIMD() != 0)
    {
        printf("Failed to disable SIMD\n");
        return -1;
    }

    if (GetEnabledSIMDExtension() > 3 || GetEnabledSIMDExtension() < 0)
    {
        printf("Enabled SIMD extension is outside of the expected range.\n");
        return -1;
    }

    // Compress block
    {
        uint8_t resultBlock8[8]   = {};
        uint8_t resultBlock16[16] = {};

        if (CompressBlockBC1((unsigned char*)srcBlock, 16, resultBlock8, bc1Options) != 0)
        {
            printf("Failed to compress BC1 block.\n");
            return -1;
        }
        if (CompressBlockBC2((unsigned char*)srcBlock, 16, resultBlock16, bc2Options) != 0)
        {
            printf("Failed to compress BC2 block.\n");
            return -1;
        }
        if (CompressBlockBC3((unsigned char*)srcBlock, 16, resultBlock16, bc3Options) != 0)
        {
            printf("Failed to compress BC3 block.\n");
            return -1;
        }
        if (CompressBlockBC4((unsigned char*)srcBlock, 16, resultBlock8, bc4Options) != 0)
        {
            printf("Failed to compress BC4 block.\n");
            return -1;
        }
        if (CompressBlockBC4S((char*)srcBlock, 16, resultBlock8, bc4Options) != 0)
        {
            printf("Failed to compress BC4 signed block.\n");
            return -1;
        }
        if (CompressBlockBC5((unsigned char*)srcBlock, 16, ((unsigned char*)srcBlock) + 1, 16, resultBlock16, bc5Options) != 0)
        {
            printf("Failed to compress BC5 block.\n");
            return -1;
        }
        if (CompressBlockBC5S((char*)srcBlock, 16, ((char*)srcBlock) + 1, 16, resultBlock16, bc5Options) != 0)
        {
            printf("Failed to compress BC5 signed block.\n");
            return -1;
        }
        if (CompressBlockBC6((unsigned short*)srcBlockShort, 12, resultBlock16, bc6Options) != 0)
        {
            printf("Failed to compress BC6 block.\n");
            return -1;
        }
        if (CompressBlockBC7((unsigned char*)srcBlock, 16, resultBlock16, bc7Options) != 0)
        {
            printf("Failed to compress BC7 block.\n");
            return -1;
        }
    }

    // Decompress block
    {
        uint8_t  resultBlock[64]      = {};
        uint16_t resultBlockShort[48] = {};

        if (DecompressBlockBC1((unsigned char*)srcBlock, resultBlock, bc1Options) != 0)
        {
            printf("Failed to decompress BC1 block.\n");
            return -1;
        }
        if (DecompressBlockBC2((unsigned char*)srcBlock, resultBlock, bc2Options) != 0)
        {
            printf("Failed to decompress BC2 block.\n");
            return -1;
        }
        if (DecompressBlockBC3((unsigned char*)srcBlock, resultBlock, bc3Options) != 0)
        {
            printf("Failed to decompress BC3 block.\n");
            return -1;
        }
        if (DecompressBlockBC4((unsigned char*)srcBlock, resultBlock, bc4Options) != 0)
        {
            printf("Failed to decompress BC4 block.\n");
            return -1;
        }
        if (DecompressBlockBC4S((unsigned char*)srcBlock, (char*)resultBlock, bc4Options) != 0)
        {
            printf("Failed to decompress BC4 signed block.\n");
            return -1;
        }
        if (DecompressBlockBC5((unsigned char*)srcBlock, resultBlock, resultBlock + 16, bc5Options) != 0)
        {
            printf("Failed to decompress BC5 block.\n");
            return -1;
        }
        if (DecompressBlockBC5S((unsigned char*)srcBlock, (char*)resultBlock, (char*)resultBlock + 16, bc5Options) != 0)
        {
            printf("Failed to decompress BC5 signed block.\n");
            return -1;
        }
        if (DecompressBlockBC6((unsigned char*)srcBlock, resultBlockShort, bc6Options) != 0)
        {
            printf("Failed to decompress BC6 block.\n");
            return -1;
        }
        if (DecompressBlockBC7((unsigned char*)srcBlock, resultBlock, bc7Options) != 0)
        {
            printf("Failed to decompress BC7 block.\n");
            return -1;
        }
    }

    // DestroyOptionsBCN block
    {
        if (DestroyOptionsBC1(bc1Options) != 0)
        {
            printf("Failed to destroy BC1 options struct.\n");
            return -1;
        }

        if (DestroyOptionsBC2(bc2Options) != 0)
        {
            printf("Failed to destroy BC2 options struct.\n");
            return -1;
        }

        if (DestroyOptionsBC3(bc3Options) != 0)
        {
            printf("Failed to destroy BC3 options struct.\n");
            return -1;
        }

        if (DestroyOptionsBC4(bc4Options) != 0)
        {
            printf("Failed to destroy BC4 options struct.\n");
            return -1;
        }

        if (DestroyOptionsBC5(bc5Options) != 0)
        {
            printf("Failed to destroy BC5 options struct.\n");
            return -1;
        }

        if (DestroyOptionsBC6(bc6Options) != 0)
        {
            printf("Failed to destroy BC6 options struct.\n");
            return -1;
        }

        if (DestroyOptionsBC7(bc7Options) != 0)
        {
            printf("Failed to destroy BC7 options struct.\n");
            return -1;
        }
    }

    printf("Compressonator Core tests finished successfully.\n");
    return 0;
}