//=====================================================================
// Copyright 2022-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#include "single_include/catch2/catch.hpp"

#include "compressonator.h"

static inline bool CheckFloatsEqual(float value1, float value2, float error = 0.02f)
{
    return std::abs(value1 - value2) <= error;
}

static bool CheckFloatBuffersEqual(float* buffer1, float* buffer2, uint32_t numElements, float error = 0.02f)
{
    for (uint32_t i = 0; i < numElements; ++i)
    {
        if (!CheckFloatsEqual(buffer1[i], buffer2[i], error))
            return false;
    }

    return true;
}

static void FillBuffer(float* buffer, uint32_t numElements, float value)
{
    for (uint32_t i = 0; i < numElements; ++i)
    {
        buffer[i] = value;
    }
}

static void FillRGBABuffer(float* buffer, uint32_t size, float r, float g, float b, float a)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        buffer[i * 4 + 0] = r;
        buffer[i * 4 + 1] = g;
        buffer[i * 4 + 2] = b;
        buffer[i * 4 + 3] = a;
    }
}

static CMP_Texture CreateRGBA32FTexture(uint32_t width, uint32_t height, bool allocateData = false)
{
    CMP_Texture texture = {};

    texture.dwSize     = sizeof(CMP_Texture);
    texture.dwWidth    = width;
    texture.dwHeight   = height;
    texture.dwPitch    = width * sizeof(float) * 4;
    texture.format     = CMP_FORMAT_RGBA_32F;
    texture.dwDataSize = width * height * 4 * sizeof(float);

    if (allocateData)
        texture.pData = (CMP_BYTE*)calloc(1, texture.dwDataSize);

    return texture;
}

static CMP_Texture CreateBC6HTexture(uint32_t width, uint32_t height, bool isSigned = false, bool allocateData = false)
{
    CMP_Texture texture = {};
    texture.dwSize      = sizeof(CMP_Texture);
    texture.dwWidth     = width;
    texture.dwHeight    = height;
    texture.dwPitch     = width;

    if (isSigned)
        texture.format = CMP_FORMAT_BC6H_SF;
    else
        texture.format = CMP_FORMAT_BC6H;

    texture.dwDataSize = CMP_CalculateBufferSize(&texture);

    if (allocateData)
        texture.pData = (CMP_BYTE*)calloc(1, texture.dwDataSize);

    return texture;
}

static CMP_CompressOptions DefaultTestCompressOptions()
{
    CMP_CompressOptions options = {};
    options.dwSize              = sizeof(options);
    options.fquality            = 0.05f;
    options.dwnumThreads        = 1;

    return options;
}

TEST_CASE("BC6H Codec Tests", "[BC6H]")
{
    const uint32_t width       = 256;
    const uint32_t height      = 256;
    const uint32_t numChannels = 4;

    CMP_CompressOptions options = DefaultTestCompressOptions();

    CMP_Texture srcTexture          = CreateRGBA32FTexture(width, height, true);
    CMP_Texture compressedTexture   = CreateBC6HTexture(width, height, false, true);
    CMP_Texture decompressedTexture = CreateRGBA32FTexture(width, height, true);

    REQUIRE(compressedTexture.format == CMP_FORMAT_BC6H);

    SECTION("Unsigned Texture Data")
    {
        FillRGBABuffer((float*)srcTexture.pData, width * height, 15.0f, 15.0f, 15.0f, 1.0f);

        CMP_ERROR result = CMP_ConvertTexture(&srcTexture, &compressedTexture, &options, 0);
        REQUIRE(result == CMP_OK);

        result = CMP_ConvertTexture(&compressedTexture, &decompressedTexture, &options, 0);
        REQUIRE(result == CMP_OK);

        REQUIRE(CheckFloatBuffersEqual((float*)srcTexture.pData, (float*)decompressedTexture.pData, width * height * numChannels));
    }

    SECTION("Signed Texture Data")
    {
        FillRGBABuffer((float*)srcTexture.pData, width * height, -10.0f, -10.0f, -10.0f, 1.0f);

        CMP_ERROR result = CMP_ConvertTexture(&srcTexture, &compressedTexture, &options, 0);
        REQUIRE(result == CMP_OK);

        result = CMP_ConvertTexture(&compressedTexture, &decompressedTexture, &options, 0);
        REQUIRE(result == CMP_OK);

        bool buffersEqual = true;
        for (uint32_t i = 0; i < width * height * numChannels && buffersEqual; ++i)
        {
            float srcValue          = *((float*)srcTexture.pData + i);
            float decompressedValue = *((float*)decompressedTexture.pData + i);

            if (srcValue < 0.0f && decompressedValue == 0.0f)
                continue;

            if (CheckFloatsEqual(srcValue, decompressedValue))
                continue;

            buffersEqual = false;
        }

        REQUIRE(buffersEqual);
    }
}

TEST_CASE("BC6H_SF Codec Tests", "[BC6H][BC6H_SF]")
{
    const uint32_t width       = 256;
    const uint32_t height      = 256;
    const uint32_t numChannels = 4;

    CMP_CompressOptions options = DefaultTestCompressOptions();

    CMP_Texture srcTexture          = CreateRGBA32FTexture(width, height, true);
    CMP_Texture compressedTexture   = CreateBC6HTexture(width, height, true, true);
    CMP_Texture decompressedTexture = CreateRGBA32FTexture(width, height, true);

    REQUIRE(compressedTexture.format == CMP_FORMAT_BC6H_SF);

    SECTION("Signed Texture Data")
    {
        FillRGBABuffer((float*)srcTexture.pData, width * height, -10.0f, -10.0f, -10.0f, 1.0f);

        CMP_ERROR result = CMP_ConvertTexture(&srcTexture, &compressedTexture, &options, 0);
        REQUIRE(result == CMP_OK);

        result = CMP_ConvertTexture(&compressedTexture, &decompressedTexture, &options, 0);
        REQUIRE(result == CMP_OK);

        REQUIRE(CheckFloatBuffersEqual((float*)srcTexture.pData, (float*)decompressedTexture.pData, width * height * numChannels, 0.05f));
    }

    SECTION("Non-Signed Texture Data")
    {
        FillRGBABuffer((float*)srcTexture.pData, width * height, 15.0f, 15.0f, 15.0f, 1.0f);

        CMP_ERROR result = CMP_ConvertTexture(&srcTexture, &compressedTexture, &options, 0);
        REQUIRE(result == CMP_OK);

        result = CMP_ConvertTexture(&compressedTexture, &decompressedTexture, &options, 0);
        REQUIRE(result == CMP_OK);

        REQUIRE(CheckFloatBuffersEqual((float*)srcTexture.pData, (float*)decompressedTexture.pData, width * height * numChannels));
    }

    SECTION("Signed Normalized Texture Data")
    {
        // filling in the src texture data

        uint32_t rowOffset = 0;
        FillRGBABuffer((float*)srcTexture.pData + rowOffset * width * numChannels, width * height - rowOffset * width, -1.0f, -1.0f, -1.0f, 1.0f);

        rowOffset = height / 3;
        FillRGBABuffer((float*)srcTexture.pData + rowOffset * width * numChannels, width * height - rowOffset * width, 0.0f, 0.0f, 0.0f, 1.0f);

        rowOffset = 2 * height / 3;
        FillRGBABuffer((float*)srcTexture.pData + rowOffset * width * numChannels, width * height - rowOffset * width, 1.0f, 1.0f, 1.0f, 1.0f);

        // compress texture

        CMP_ERROR result = CMP_ConvertTexture(&srcTexture, &compressedTexture, &options, 0);
        REQUIRE(result == CMP_OK);

        // decompress texture

        result = CMP_ConvertTexture(&compressedTexture, &decompressedTexture, &options, 0);
        REQUIRE(result == CMP_OK);

        // compare results

        REQUIRE(CheckFloatBuffersEqual((float*)srcTexture.pData, (float*)decompressedTexture.pData, width * height * numChannels));
    }

    free(srcTexture.pData);
    free(compressedTexture.pData);
    free(decompressedTexture.pData);
}