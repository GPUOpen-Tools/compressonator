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

#include <string>

#include "single_include/catch2/catch.hpp"

#include "test_constants.h"
#include "compressonator.h"
#include "common_def.h"
#include "cmp_core.h"
#include "cpu_timing.h"

TEST_CASE("Enabling_SIMD", "[SIMD]")
{
    CHECK(GetEnabledSIMDExtension() > 0);
    CHECK(GetEnabledSIMDExtension() <= 3);

    DisableSIMD();
    CHECK(GetEnabledSIMDExtension() == 0);

    if (EnableSSE4() == CGU_CORE_OK)
        CHECK(GetEnabledSIMDExtension() == 1);

    if (EnableAVX2() == CGU_CORE_OK)
        CHECK(GetEnabledSIMDExtension() == 2);

    if (EnableAVX512() == CGU_CORE_OK)
        CHECK(GetEnabledSIMDExtension() == 3);
}

static void LoopedBC1Compression(CMP_BYTE* srcData, unsigned int numBlocks)
{
    static const unsigned int NUM_LOOPS = 15;

    CMP_BYTE compressedData[16];

    for (unsigned int loopIndex = 0; loopIndex < NUM_LOOPS; ++loopIndex)
    {
        for (unsigned int i = 0; i < numBlocks; ++i)
        {
            CompressBlockBC1(srcData + i * 16, 16, compressedData, 0);
        }
    }
}

TEST_CASE("BC1_Timing", "[SIMD]")
{
    printf("BC1 SIMD Timing Test\n");
    printf("------------------------------------\n");

    cpu_timer timer;

    std::string inputImagePath = TEST_DATA_PATH + std::string("/ruby.bmp");

    CMP_MipSet texture = {};
    CMP_LoadTexture(inputImagePath.c_str(), &texture);

    REQUIRE(texture.m_nWidth > 0);
    REQUIRE(texture.m_nHeight > 0);

    CMP_MipLevel* imageData = 0;
    CMP_GetMipLevel(&imageData, &texture, 0, 0);

    REQUIRE(imageData != 0);

    unsigned int numBlocks = texture.m_nWidth / 4 * texture.m_nHeight / 4;

    unsigned char compressedData[16];

    DisableSIMD();

    printf("Compressing texture without SIMD...\n");

    timer.Start(0);

    LoopedBC1Compression(imageData->m_pbData, numBlocks);

    timer.Stop(0);

    printf("Time to compress without SIMD: %f ms\n\n", timer.GetTimeMS(0));

    // SSE encoding

    if (EnableSSE4() == CGU_CORE_OK)
    {
        printf("Compressing texture with SSE...\n");

        timer.Start(0);

        LoopedBC1Compression(imageData->m_pbData, numBlocks);

        timer.Stop(0);

        printf("Time to compress using SSE4: %f ms\n\n", timer.GetTimeMS(0));
    }
    else
        printf("Skipping SSE test because it is not supported on the current CPU.\n");

    // AVX2 encoding

    if (EnableAVX2() == CGU_CORE_OK)
    {
        printf("Compressing texture with AVX...\n");

        timer.Start(0);

        LoopedBC1Compression(imageData->m_pbData, numBlocks);

        timer.Stop(0);

        printf("Time to compress using AVX2: %f ms\n\n", timer.GetTimeMS(0));
    }
    else
        printf("Skipping AVX test because it is not supported on the current CPU.\n");

    // AVX-512 encoding

    if (EnableAVX512() == CGU_CORE_OK)
    {
        printf("Compressing texture with AVX-512...\n");

        timer.Start(0);

        LoopedBC1Compression(imageData->m_pbData, numBlocks);

        timer.Stop(0);

        printf("Time to compress using AVX-512: %f ms\n\n", timer.GetTimeMS(0));
    }
    else
        printf("Skipping AVX-512 test because it is not supported on the current CPU.\n");

    printf("\n");
}

TEST_CASE("BC1_Compression", "[SIMD]")
{
    printf("BC1 SIMD Compression Test\n");
    printf("------------------------------------\n");

    std::string inputImagePath = TEST_DATA_PATH + std::string("/ruby.bmp");

    CMP_MipSet texture = {};
    CMP_LoadTexture(inputImagePath.c_str(), &texture);

    REQUIRE(texture.m_nWidth > 0);
    REQUIRE(texture.m_nHeight > 0);

    CMP_MipLevel* imageData = 0;
    CMP_GetMipLevel(&imageData, &texture, 0, 0);

    REQUIRE(imageData != 0);

    unsigned int numBlocks = texture.m_nWidth / 4 * texture.m_nHeight / 4;

    unsigned char* referenceData  = (unsigned char*)calloc(numBlocks, 8);
    unsigned char* compressedData = (unsigned char*)calloc(numBlocks, 8);

    // Original Reference Encoding

    printf("Compressing texture without SIMD...\n");

    DisableSIMD();

    for (unsigned int i = 0; i < numBlocks; ++i)
    {
        int result = CompressBlockBC1(imageData->m_pbData + i * 16, 16, referenceData + i * 8, 0);
        REQUIRE(result == CGU_CORE_OK);
    }

    printf("Compression complete.\n\n");

    // SSE encoding

    if (EnableSSE4() == CGU_CORE_OK)
    {
        printf("Compressing texture with SSE...\n");

        for (unsigned int i = 0; i < numBlocks; ++i)
        {
            int result = CompressBlockBC1(imageData->m_pbData + i * 16, 16, compressedData + i * 8, 0);
            REQUIRE(result == CGU_CORE_OK);
        }

        CHECK(memcmp(referenceData, compressedData, numBlocks * 8) == 0);

        printf("Compression complete.\n\n");
    }
    else
        printf("Skipping SSE test because it is not supported on the current CPU.\n");

    // AVX2 encoding

    if (EnableAVX2() == CGU_CORE_OK)
    {
        printf("Compressing texture with AVX...\n");

        for (unsigned int i = 0; i < numBlocks; ++i)
        {
            int result = CompressBlockBC1(imageData->m_pbData + i * 16, 16, compressedData + i * 8, 0);
            REQUIRE(result == CGU_CORE_OK);
        }

        CHECK(memcmp(referenceData, compressedData, numBlocks * 8) == 0);

        printf("Compression complete.\n\n");
    }
    else
        printf("Skipping AVX test because it is not supported on the current CPU.\n");

    // AVX-512 encoding

    if (EnableAVX512() == CGU_CORE_OK)
    {
        printf("Compressing texture with AVX-512...\n");

        for (unsigned int i = 0; i < numBlocks; ++i)
        {
            int result = CompressBlockBC1(imageData->m_pbData + i * 16, 16, compressedData + i * 8, 0);
            REQUIRE(result == CGU_CORE_OK);
        }

        CHECK(memcmp(referenceData, compressedData, numBlocks * 8) == 0);

        printf("Compression complete.\n\n");
    }
    else
        printf("Skipping AVX-512 test because it is not supported on the current CPU.\n");

    printf("\n");
}
