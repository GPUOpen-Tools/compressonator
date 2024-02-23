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

#include "single_include/catch2/catch.hpp"

#include "codecbuffer_rgba8888.h"
#include "codecbuffer_rgba8888s.h"
#include "codecbuffer_rgb888.h"
#include "codecbuffer_rgb888s.h"

typedef unsigned int uint;

// Testing Data

static const uint k_blockWidth  = 4;
static const uint k_blockHeight = 4;

static uint g_dataLength = 0;

static CMP_BYTE*  g_byteData   = 0;
static CMP_SBYTE* g_sbyteData  = 0;
static CMP_WORD*  g_wordData   = 0;
static CMP_DWORD* g_dwordData  = 0;
static CMP_HALF*  g_halfData   = 0;
static float*     g_floatData  = 0;
static double*    g_doubleData = 0;

static void SetFloatData(bool isSigned = false)
{
    float maxValue = isSigned ? 127.0f : 255.0f;

    for (uint i = 0; i < g_dataLength; ++i)
    {
        float value = i / maxValue;

        g_halfData[i]   = (CMP_HALF)value;
        g_floatData[i]  = value;
        g_doubleData[i] = (double)value;
    }
}

static void GenerateTestData(unsigned int numValues = 128, bool isSigned = false)
{
    if (g_byteData)
        free(g_byteData);
    if (g_sbyteData)
        free(g_sbyteData);
    if (g_wordData)
        free(g_wordData);
    if (g_dwordData)
        free(g_dwordData);
    if (g_halfData)
        free(g_halfData);
    if (g_floatData)
        free(g_floatData);
    if (g_doubleData)
        free(g_doubleData);

    g_byteData   = (CMP_BYTE*)calloc(numValues, sizeof(CMP_BYTE));
    g_sbyteData  = (CMP_SBYTE*)calloc(numValues, sizeof(CMP_SBYTE));
    g_wordData   = (CMP_WORD*)calloc(numValues, sizeof(CMP_WORD));
    g_dwordData  = (CMP_DWORD*)calloc(numValues, sizeof(CMP_DWORD));
    g_halfData   = (CMP_HALF*)calloc(numValues, sizeof(CMP_HALF));
    g_floatData  = (float*)calloc(numValues, sizeof(float));
    g_doubleData = (double*)calloc(numValues, sizeof(double));

    g_dataLength = numValues;

    for (unsigned int i = 0; i < numValues; ++i)
    {
        g_byteData[i]  = i;
        g_sbyteData[i] = i;
        g_wordData[i]  = i;
        g_dwordData[i] = i;
    }

    SetFloatData(isSigned);
}

template <typename T>
static T* GetSrcDataOfType()
{
    REQUIRE(false);
}

template <>
static CMP_BYTE* GetSrcDataOfType<CMP_BYTE>()
{
    return (CMP_BYTE*)g_byteData;
}
template <>
static CMP_SBYTE* GetSrcDataOfType<CMP_SBYTE>()
{
    return (CMP_SBYTE*)g_sbyteData;
}
template <>
static CMP_WORD* GetSrcDataOfType<CMP_WORD>()
{
    return (CMP_WORD*)g_wordData;
}
template <>
static CMP_DWORD* GetSrcDataOfType<CMP_DWORD>()
{
    return (CMP_DWORD*)g_dwordData;
}
template <>
static CMP_HALF* GetSrcDataOfType<CMP_HALF>()
{
    return (CMP_HALF*)g_halfData;
}
template <>
static float* GetSrcDataOfType<float>()
{
    return (float*)g_floatData;
}
template <>
static double* GetSrcDataOfType<double>()
{
    return (double*)g_doubleData;
}

enum TestChannel
{
    INVALID_CHANNEL = -1,
    RED_CHANNEL     = 0,
    GREEN_CHANNEL,
    BLUE_CHANNEL,
    ALPHA_CHANNEL
};

template <typename T>
static void CheckEqual(T value1, T value2)
{
    CHECK(value1 == value2);
}

template <>
static void CheckEqual<CMP_HALF>(CMP_HALF value1, CMP_HALF value2)
{
    static const float epsilon = 0.001f;

    float f1 = (float)value1;
    float f2 = (float)value2;

    CHECK(f1 <= f2 + epsilon);
    CHECK(f1 >= f2 - epsilon);
}

template <>
static void CheckEqual<float>(float value1, float value2)
{
    static const float epsilon = 0.001f;
    CHECK(value1 <= value2 + epsilon);
    CHECK(value1 >= value2 - epsilon);
}

template <>
static void CheckEqual<double>(double value1, double value2)
{
    static const double epsilon = 0.001;
    CHECK(value1 <= value2 + epsilon);
    CHECK(value1 >= value2 - epsilon);
}

template <typename T>
static void VerifyRGBA(CCodecBuffer* buffer, uint x, uint y, uint w, uint h, TestChannel inputChannel, bool convertedType = false)
{
    int totalSize = w * h * 4;
    int offset    = y * w * 4 + x * 4;

    T* readBlock = (T*)calloc(totalSize - offset, sizeof(T));

    bool noAlpha = buffer->GetChannelCount() < 4;

    bool error = buffer->ReadBlockRGBA(x, y, w, h, readBlock);
    CHECK(error);

    // we can only check the raw pointer for types that are native to the codec, otherwise the conversion will result in expected values
    if (!convertedType)
    {
        // Needed to make this work for buffers with less than 4 channels. Note that the blocks read from the ReadBlockRGBA function will always have
        // 4 channels regardless of the internal data representation of the codecbuffer
        uint channelCount = buffer->GetChannelCount();

        CMP_BYTE* bufferData = buffer->GetData();

        for (uint row = y; row < h; ++row)
        {
            for (uint col = x; col < w; ++col)
            {
                for (uint channel = 0; channel < channelCount; ++channel)
                {
                    CHECK(readBlock[row * w * 4 + col * 4 + channel] == (T)bufferData[row * w * channelCount + col * channelCount + channel]);
                }
            }
        }
    }

    T* srcData = GetSrcDataOfType<T>();

    static const uint redChannelIndex   = RGBA8888_CHANNEL_R;
    static const uint greenChannelIndex = RGBA8888_CHANNEL_G;
    static const uint blueChannelIndex  = RGBA8888_CHANNEL_B;
    static const uint alphaChannelIndex = RGBA8888_CHANNEL_A;

    // TODO: In the future we might want to have our codec buffers to display more consistent behaviour, making parts of this unnecessary

    T zeroValue = 0;

    // For converted types the conversion will mess up the zero values when calling ReadBlockRGBA after using one of the channel writing functions
    if (convertedType)
    {
        if (inputChannel == RED_CHANNEL || inputChannel == GREEN_CHANNEL)
            zeroValue = readBlock[blueChannelIndex];
        else if (inputChannel == BLUE_CHANNEL || inputChannel == ALPHA_CHANNEL)
            zeroValue = readBlock[redChannelIndex];
    }

    for (uint row = y; row < h; ++row)
    {
        for (uint col = x; col < w; ++col)
        {
            for (uint channel = 0; channel < 4; ++channel)
            {
                T* readValue = readBlock + (row - y) * w * 4 + (col - x) * 4 + channel;

                if (inputChannel == INVALID_CHANNEL)
                {
                    T expectedValue = channel == 3 && noAlpha ? readBlock[alphaChannelIndex] : srcData[row * w * 4 + col * 4 + channel];
                    CheckEqual(*readValue, expectedValue);
                }
                else
                {
                    T expectedValue = srcData[row * w + col];

                    if (channel == 3 && noAlpha)
                        expectedValue = readBlock[alphaChannelIndex];
                    else if (inputChannel == RED_CHANNEL && channel != redChannelIndex)
                        expectedValue = zeroValue;
                    else if (inputChannel == GREEN_CHANNEL && channel != greenChannelIndex)
                        expectedValue = zeroValue;
                    else if (inputChannel == BLUE_CHANNEL && channel != blueChannelIndex)
                        expectedValue = zeroValue;
                    else if (inputChannel == ALPHA_CHANNEL && channel != alphaChannelIndex)
                        expectedValue = zeroValue;

                    CheckEqual(*readValue, expectedValue);
                }
            }
        }
    }
}

template <typename T>
static void VerifyChannel(CCodecBuffer* buffer, uint x, uint y, uint w, uint h, TestChannel channel, bool allEqual = false)
{
    T* readData = (T*)calloc(w * h, sizeof(T));

    bool error = false;

    if (channel == RED_CHANNEL)
        error = buffer->ReadBlockR(x, y, w, h, readData);
    else if (channel == GREEN_CHANNEL)
        error = buffer->ReadBlockG(x, y, w, h, readData);
    else if (channel == BLUE_CHANNEL)
        error = buffer->ReadBlockB(x, y, w, h, readData);
    else if (channel == ALPHA_CHANNEL)
        error = buffer->ReadBlockA(x, y, w, h, readData);

    CHECK(error);

    T* srcData = GetSrcDataOfType<T>();

    for (uint i = 0; i < w * h; ++i)
    {
        if (allEqual)
            CheckEqual(readData[i], readData[0]);
        else
            CheckEqual(readData[i], srcData[y * w + x + i]);
    }

    free(readData);
}

template <typename T>
static inline void VerifyRedChannel(CCodecBuffer* buffer, uint x, uint y, uint w, uint h, bool allEqual = false)
{
    VerifyChannel<T>(buffer, x, y, w, h, RED_CHANNEL, allEqual);
}
template <typename T>
static inline void VerifyGreenChannel(CCodecBuffer* buffer, uint x, uint y, uint w, uint h, bool allEqual = false)
{
    VerifyChannel<T>(buffer, x, y, w, h, GREEN_CHANNEL, allEqual);
}
template <typename T>
static inline void VerifyBlueChannel(CCodecBuffer* buffer, uint x, uint y, uint w, uint h, bool allEqual = false)
{
    VerifyChannel<T>(buffer, x, y, w, h, BLUE_CHANNEL, allEqual);
}
template <typename T>
static inline void VerifyAlphaChannel(CCodecBuffer* buffer, uint x, uint y, uint w, uint h, bool allEqual = false)
{
    VerifyChannel<T>(buffer, x, y, w, h, ALPHA_CHANNEL, allEqual);
}

template <typename T>
static void RunChannelTest(CCodecBuffer* buffer, TestChannel testChannel, bool convertedType = false)
{
    T* srcData = GetSrcDataOfType<T>();

    bool noAlpha = testChannel == ALPHA_CHANNEL && buffer->GetChannelCount() < 4;

    bool error = false;

    if (testChannel == RED_CHANNEL)
        error = buffer->WriteBlockR(0, 0, 4, 4, srcData);
    else if (testChannel == GREEN_CHANNEL)
        error = buffer->WriteBlockG(0, 0, 4, 4, srcData);
    else if (testChannel == BLUE_CHANNEL)
        error = buffer->WriteBlockB(0, 0, 4, 4, srcData);
    else if (testChannel == ALPHA_CHANNEL)
        error = buffer->WriteBlockA(0, 0, 4, 4, srcData);

    if (noAlpha)
        CHECK_FALSE(error);
    else
        CHECK(error);

    // check data from raw pointer to make sure it is correct
    // we can only check the raw pointer for types that are native to the codec, otherwise the conversion will result in expected values
    if (!convertedType)
    {
        uint channelCount = buffer->GetChannelCount();
        uint channelIndex = 0;

        if (testChannel == RED_CHANNEL)
            channelIndex = RGBA8888_CHANNEL_R;
        else if (testChannel == GREEN_CHANNEL)
            channelIndex = RGBA8888_CHANNEL_G;
        else if (testChannel == BLUE_CHANNEL)
            channelIndex = RGBA8888_CHANNEL_B;
        else if (testChannel == ALPHA_CHANNEL)
            channelIndex = RGBA8888_CHANNEL_A;

        CMP_BYTE* bufferData = buffer->GetData();
        for (int i = 0; i < 16 * channelCount; ++i)
        {
            if (!noAlpha && i % channelCount == channelIndex)
                CHECK(bufferData[i] == (CMP_BYTE)srcData[i / channelCount]);
            else
                CHECK(bufferData[i] == 0);
        }
    }

    // verify each channel's read functions

    VerifyRedChannel<T>(buffer, 0, 0, 4, 4, testChannel != RED_CHANNEL);
    VerifyGreenChannel<T>(buffer, 0, 0, 4, 4, testChannel != GREEN_CHANNEL);
    VerifyBlueChannel<T>(buffer, 0, 0, 4, 4, testChannel != BLUE_CHANNEL);
    VerifyAlphaChannel<T>(buffer, 0, 0, 4, 4, noAlpha || testChannel != ALPHA_CHANNEL);

    VerifyRGBA<T>(buffer, 0, 0, 4, 4, testChannel, convertedType);
}

template <typename T>
static void RunRGBATest(CCodecBuffer* buffer, bool convertedType = false)
{
    static const unsigned int width     = 4;
    static const unsigned int height    = 4;
    static const unsigned int numPixels = width * height;

    uint channelCount = buffer->GetChannelCount();

    T* srcData = GetSrcDataOfType<T>();

    // Contrary to the name of the function, this writes in BGRA order, which is a constant feature of our codec buffers

    CHECK(buffer->WriteBlockRGBA(0, 0, width, height, srcData));

    // check data from raw pointer to make sure it is correct
    // we can only check the raw pointer for types that are native to the codec, otherwise the conversion will result in expected values
    if (!convertedType)
    {
        CMP_BYTE* bufferData = buffer->GetData();
        for (int i = 0; i < numPixels * 4; ++i)
        {
            if (channelCount < 4)
                break;

            CHECK((T)bufferData[i] == srcData[i]);
        }
    }

    static const uint redChannelIndex   = RGBA8888_CHANNEL_R;
    static const uint greenChannelIndex = RGBA8888_CHANNEL_G;
    static const uint blueChannelIndex  = RGBA8888_CHANNEL_B;
    static const uint alphaChannelIndex = RGBA8888_CHANNEL_A;

    T readData[numPixels] = {};

    // blue channel

    CHECK(buffer->ReadBlockB(0, 0, width, height, readData));

    for (int i = 0; i < numPixels; ++i)
    {
        CheckEqual(readData[i], srcData[i * 4 + blueChannelIndex]);
    }

    // green channel

    CHECK(buffer->ReadBlockG(0, 0, width, height, readData));

    for (int i = 0; i < numPixels; ++i)
    {
        CheckEqual(readData[i], srcData[i * 4 + greenChannelIndex]);
    }

    // red channel

    CHECK(buffer->ReadBlockR(0, 0, width, height, readData));

    for (int i = 0; i < numPixels; ++i)
    {
        CheckEqual(readData[i], srcData[i * 4 + redChannelIndex]);
    }

    // alpha channel

    CHECK(buffer->ReadBlockA(0, 0, width, height, readData));

    if (channelCount < 4)
    {
        for (int i = 0; i < numPixels; ++i)
        {
            CheckEqual(readData[i], readData[0]);
        }
    }
    else
    {
        for (int i = 0; i < numPixels; ++i)
        {
            CheckEqual(readData[i], srcData[i * 4 + alphaChannelIndex]);
        }
    }

    VerifyRGBA<T>(buffer, 0, 0, width, height, INVALID_CHANNEL, convertedType);
}

template <typename T>
static void VerifyOffsetBlockChannel(CCodecBuffer* buffer, uint blockOffsetX, uint blockOffsetY, T* testData, TestChannel channelIndex)
{
    T blockChannel[k_blockWidth * k_blockHeight] = {};

    uint channelOffset = 0;

    if (channelIndex == RED_CHANNEL)
    {
        CHECK(buffer->ReadBlockR(blockOffsetX * k_blockWidth, blockOffsetY * k_blockHeight, k_blockWidth, k_blockHeight, blockChannel));
        channelOffset = 2;
    }
    else if (channelIndex == GREEN_CHANNEL)
    {
        CHECK(buffer->ReadBlockG(blockOffsetX * k_blockWidth, blockOffsetY * k_blockHeight, k_blockWidth, k_blockHeight, blockChannel));
        channelOffset = 1;
    }
    else if (channelIndex == BLUE_CHANNEL)
    {
        CHECK(buffer->ReadBlockB(blockOffsetX * k_blockWidth, blockOffsetY * k_blockHeight, k_blockWidth, k_blockHeight, blockChannel));
        channelOffset = 0;
    }
    else if (channelIndex == ALPHA_CHANNEL)
    {
        CHECK(buffer->ReadBlockA(blockOffsetX * k_blockWidth, blockOffsetY * k_blockHeight, k_blockWidth, k_blockHeight, blockChannel));
        channelOffset = 3;
    }
    else
    {
        assert(!"Invalid channel passed to VerifyOffsetBlockChannel");
        return;
    }

    CMP_DWORD bufferWidth = buffer->GetWidth();
    uint      blockOffset = blockOffsetY * k_blockHeight * bufferWidth * 4 + blockOffsetX * k_blockWidth * 4;

    for (uint row = 0; row < k_blockWidth; ++row)
    {
        for (uint col = 0; col < k_blockHeight; ++col)
        {
            CheckEqual(blockChannel[row * k_blockWidth + col], testData[blockOffset + row * bufferWidth * 4 + col * 4 + channelOffset]);
        }
    }
}

template <typename T>
static void VerifyOffsetBlock(CCodecBuffer* buffer, uint blockOffsetX, uint blockOffsetY, T* testData)
{
    // verify RGBA block

    T block[k_blockWidth * k_blockHeight * 4] = {};

    CMP_DWORD bufferWidth = buffer->GetWidth();

    CHECK(buffer->ReadBlockRGBA(blockOffsetX * k_blockWidth, blockOffsetY * k_blockHeight, k_blockWidth, k_blockHeight, block));

    uint channelCount = buffer->GetChannelCount();

    uint blockOffset = blockOffsetY * k_blockHeight * bufferWidth * 4 + blockOffsetX * k_blockWidth * 4;

    for (uint row = 0; row < k_blockWidth; ++row)
    {
        for (uint col = 0; col < k_blockHeight; ++col)
        {
            for (uint channel = 0; channel < channelCount; ++channel)
            {
                CheckEqual(block[row * k_blockWidth * 4 + col * 4 + channel], testData[blockOffset + row * bufferWidth * 4 + col * 4 + channel]);
            }
        }
    }

    // verify individual channels

    VerifyOffsetBlockChannel(buffer, blockOffsetX, blockOffsetY, testData, RED_CHANNEL);
    VerifyOffsetBlockChannel(buffer, blockOffsetX, blockOffsetY, testData, GREEN_CHANNEL);
    VerifyOffsetBlockChannel(buffer, blockOffsetX, blockOffsetY, testData, BLUE_CHANNEL);

    if (channelCount == 4)
        VerifyOffsetBlockChannel(buffer, blockOffsetX, blockOffsetY, testData, ALPHA_CHANNEL);
}

// TESTS

// a wrapper around the CHECK macro for return values on functions
// this is to make it easier to step through the code for debugging
#define CHECK_RETURN_WRAPPER(src) \
    {                             \
        auto errorCode = (src);   \
        CHECK(errorCode);         \
    }

TEST_CASE("CodecBuffer_RGBA8888", "[CODECBUFFER]")
{
    GenerateTestData();

    const unsigned int width  = 4;
    const unsigned int height = 4;

    CCodecBuffer_RGBA8888 buffer = CCodecBuffer_RGBA8888(k_blockWidth, k_blockHeight, 1, width, height);

    CMP_BYTE* testData = g_byteData;

    SECTION("Check Properties")
    {
        CHECK(buffer.GetBufferType() == CBT_RGBA8888);
        CHECK(buffer.GetChannelDepth() == 8);
        CHECK(buffer.GetChannelCount() == 4);
        CHECK_FALSE(buffer.IsFloat());
        CHECK(buffer.GetWidth() == width);
        CHECK(buffer.GetHeight() == height);
        CHECK(buffer.GetPitch() == width * 4);
        CHECK(buffer.GetFormat() == CMP_FORMAT_RGBA_8888);
        CHECK(buffer.GetTranscodeFormat() == CMP_FORMAT_Unknown);
        CHECK(buffer.GetBlockWidth() == k_blockWidth);
        CHECK(buffer.GetBlockHeight() == k_blockHeight);
        CHECK(buffer.GetBlockDepth() == 1);
        CHECK(buffer.GetData() != 0);
        CHECK(buffer.GetDataSize() == width * height * 4);
    }

    SECTION("WriteBlockR")
    {
        RunChannelTest<CMP_BYTE>(&buffer, RED_CHANNEL);
        RunChannelTest<CMP_SBYTE>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_WORD>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, RED_CHANNEL, true);
        RunChannelTest<float>(&buffer, RED_CHANNEL, true);
        RunChannelTest<double>(&buffer, RED_CHANNEL, true);
    }

    SECTION("WriteBlockG")
    {
        RunChannelTest<CMP_BYTE>(&buffer, GREEN_CHANNEL);
        RunChannelTest<CMP_SBYTE>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_WORD>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<float>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<double>(&buffer, GREEN_CHANNEL, true);
    }

    SECTION("WriteBlockB")
    {
        RunChannelTest<CMP_BYTE>(&buffer, BLUE_CHANNEL);
        RunChannelTest<CMP_SBYTE>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_WORD>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<float>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<double>(&buffer, BLUE_CHANNEL, true);
    }

    SECTION("WriteBlockA")
    {
        RunChannelTest<CMP_BYTE>(&buffer, ALPHA_CHANNEL);
        RunChannelTest<CMP_SBYTE>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<CMP_WORD>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<float>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<double>(&buffer, ALPHA_CHANNEL, true);
    }

    SECTION("WriteBlockRGBA")
    {
        RunRGBATest<CMP_BYTE>(&buffer);
        RunRGBATest<CMP_SBYTE>(&buffer, true);
        RunRGBATest<CMP_WORD>(&buffer, true);
        RunRGBATest<CMP_DWORD>(&buffer, true);
        RunRGBATest<CMP_HALF>(&buffer, true);
        RunRGBATest<float>(&buffer, true);
        RunRGBATest<double>(&buffer, true);
    }

    SECTION("Multiple Block Input")
    {
        const uint inputWidth  = 16;
        const uint inputHeight = 16;

        GenerateTestData(1024);
        testData = g_byteData;

        CCodecBuffer_RGBA8888 tempBuffer(4, 4, 1, inputWidth, inputHeight);

        CHECK_RETURN_WRAPPER(tempBuffer.WriteBlockRGBA(0, 0, inputWidth, inputHeight, testData));

        // check that the internal data matches the input data

        CMP_BYTE* bufferData = tempBuffer.GetData();

        for (uint i = 0; i < inputWidth * inputHeight * 4; ++i)
        {
            CheckEqual(bufferData[i], testData[i]);
        }

        // check some of the blocks
        VerifyOffsetBlock(&tempBuffer, 0, 0, testData);
        VerifyOffsetBlock(&tempBuffer, 1, 0, testData);
        VerifyOffsetBlock(&tempBuffer, 3, 2, testData);
    }

    SECTION("Test Padding")
    {
        // This test is to make sure that the padding capabilities of the codec buffer work properly
        // When the requested width to write is more than the width of the source
        //  we pad by repeating the sequence of values for the rest of the line
        // When the requested height is more than the height of the source we similarly
        //  repeat the previous rows of data for the rest of the block

        CHECK_RETURN_WRAPPER(buffer.WriteBlockRGBA(0, 0, width, height, testData));

        CMP_BYTE block[8 * 8] = {};

        CHECK_RETURN_WRAPPER(buffer.ReadBlockR(0, 0, 8, 8, block));

        // check the first 4 rows of real data with line padding

        for (uint row = 0; row < 4; ++row)
        {
            for (uint col = 0; col < 8; ++col)
            {
                CMP_BYTE testValue = testData[row * width * 4 + (col % 4) * 4 + 2];
                CheckEqual(block[row * 8 + col], testValue);
            }
        }

        // check that the last 4 rows are just repeats of the first 4 from block padding

        uint midPoint = 8 * 4;
        for (uint i = 0; i < midPoint; ++i)
        {
            CheckEqual(block[i], block[i + midPoint]);
        }
    }

    SECTION("Offset Writing")
    {
        // write block with x offset of 2 and y offset of 1, so each channel should look like:
        //       0 0 0 0
        //       0 0 0 1
        //       0 0 2 3
        //       0 0 4 5

        static const CMP_BYTE expectedValues[] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 3, 0, 0, 4, 5};

        int xOffset = 2;
        int yOffset = 1;

        CHECK(buffer.WriteBlockR(xOffset, yOffset, 4, 4, testData));
        CHECK(buffer.WriteBlockG(xOffset, yOffset, 4, 4, testData));
        CHECK(buffer.WriteBlockB(xOffset, yOffset, 4, 4, testData));
        CHECK(buffer.WriteBlockA(xOffset, yOffset, 4, 4, testData));

        CMP_BYTE blockR[width * height] = {};
        CMP_BYTE blockG[width * height] = {};
        CMP_BYTE blockB[width * height] = {};
        CMP_BYTE blockA[width * height] = {};

        CHECK(buffer.ReadBlockR(0, 0, 4, 4, blockR));
        CHECK(buffer.ReadBlockG(0, 0, 4, 4, blockG));
        CHECK(buffer.ReadBlockB(0, 0, 4, 4, blockB));
        CHECK(buffer.ReadBlockA(0, 0, 4, 4, blockA));

        for (uint i = 0; i < width * height; ++i)
        {
            CHECK(blockR[i] == expectedValues[i]);
            CHECK(blockG[i] == expectedValues[i]);
            CHECK(blockB[i] == expectedValues[i]);
            CHECK(blockA[i] == expectedValues[i]);
        }

        // checking WriteBlockRGBA

        static const CMP_BYTE expectedRGBAValues[] = {0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,
                                                      0,  0,  0,  1,  2, 3, 4, 5, 6, 7, 0, 0, 0,  0,  0,  0,  0,  0,  8,  9, 10, 11,
                                                      12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 16, 17, 18, 19, 20, 21, 22, 23};

        CHECK(buffer.WriteBlockRGBA(xOffset, yOffset, 4, 4, testData));

        CMP_BYTE block[width * height * 4] = {};

        CHECK(buffer.ReadBlockRGBA(0, 0, 4, 4, block));

        for (uint i = 0; i < width * height * 4; ++i)
        {
            CHECK(block[i] == expectedRGBAValues[i]);
        }
    }

    SECTION("Offset Reading")
    {
        CHECK_RETURN_WRAPPER(buffer.WriteBlockRGBA(0, 0, 4, 4, testData));

        static const unsigned int xOffset    = 1;
        static const unsigned int yOffset    = 2;
        static const unsigned int readWidth  = 3;
        static const unsigned int readHeight = 2;

        static const CMP_BYTE expectedRGBAValues[] = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};

        CMP_BYTE block[width * height * 4] = {};
        CHECK_RETURN_WRAPPER(buffer.ReadBlockRGBA(xOffset, yOffset, readWidth, readHeight, block));

        for (uint i = 0; i < sizeof(expectedRGBAValues) / sizeof(expectedRGBAValues[0]); ++i)
        {
            CHECK(block[i] == expectedRGBAValues[i]);
        }

        // recall that for regular read and write functions, the codec buffers use BGRA order

        static const CMP_BYTE expectedRValues[] = {38, 42, 46, 54, 58, 62};
        static const CMP_BYTE expectedGValues[] = {37, 41, 45, 53, 57, 61};
        static const CMP_BYTE expectedBValues[] = {36, 40, 44, 52, 56, 60};
        static const CMP_BYTE expectedAValues[] = {39, 43, 47, 55, 59, 63};

        CMP_BYTE blockR[width * height] = {};
        CMP_BYTE blockG[width * height] = {};
        CMP_BYTE blockB[width * height] = {};
        CMP_BYTE blockA[width * height] = {};

        CHECK(buffer.ReadBlockR(xOffset, yOffset, readWidth, readHeight, blockR));
        CHECK(buffer.ReadBlockG(xOffset, yOffset, readWidth, readHeight, blockG));
        CHECK(buffer.ReadBlockB(xOffset, yOffset, readWidth, readHeight, blockB));
        CHECK(buffer.ReadBlockA(xOffset, yOffset, readWidth, readHeight, blockA));

        for (uint i = 0; i < sizeof(expectedRValues) / sizeof(expectedRValues[0]); ++i)
        {
            CHECK(blockR[i] == expectedRValues[i]);
            CHECK(blockG[i] == expectedGValues[i]);
            CHECK(blockB[i] == expectedBValues[i]);
            CHECK(blockA[i] == expectedAValues[i]);
        }
    }

    SECTION("Pitch Change")
    {
        uint newPitch = 2 * width * sizeof(CMP_BYTE) * 4;

        CCodecBuffer_RGBA8888 altBuffer(k_blockWidth, k_blockHeight, 1, width, height, newPitch);

        bool error = altBuffer.WriteBlockRGBA(0, 0, 4, 4, testData);
        CHECK(error);

        CMP_BYTE* data = altBuffer.GetData();
        REQUIRE(altBuffer.GetDataSize() == newPitch * height);

        for (uint row = 0; row < height; ++row)
        {
            for (uint col = 0; col < width * 2; ++col)
            {
                for (uint channel = 0; channel < 4; ++channel)
                {
                    if (col >= 4)
                        CHECK((int)data[row * newPitch + col * 4 + channel] == 0);
                    else
                        CHECK((int)data[row * newPitch + col * 4 + channel] == (int)testData[row * width * 4 + col * 4 + channel]);
                }
            }
        }
    }

    SECTION("Copy Function")
    {
        // The copy function of codec buffers is only concerned with copying the data contained within the buffer parameter
        // It fails if the two buffers don't have the same width and height

        bool error = buffer.WriteBlockRGBA(0, 0, 4, 4, testData);
        CHECK(error);

        CCodecBuffer_RGBA8888 copiedBuffer(k_blockWidth, k_blockHeight, 1, width, height);
        copiedBuffer.Copy(buffer);

        CMP_BYTE* srcData  = buffer.GetData();
        CMP_BYTE* destData = copiedBuffer.GetData();

        CHECK(memcmp(srcData, destData, width * height * 4) == 0);

        // test with incompatible buffer

        CCodecBuffer_RGBA8888 badBuffer(1, 1, 1, 1, 1);
        CHECK_NOTHROW(badBuffer.Copy(buffer));

        CMP_BYTE* badBufferData = badBuffer.GetData();

        CHECK(memcmp(srcData, badBufferData, 1 * 1 * 4) != 0);
    }

    SECTION("Reading Before Writing")
    {
        CMP_BYTE result[16 * 4] = {};
        buffer.ReadBlockRGBA(0, 0, 4, 4, result);

        for (int i = 0; i < sizeof(result); ++i)
        {
            CHECK(result[i] == 0);
        }
    }

    SECTION("Setter and Getter Functions")
    {
        buffer.SetPitch(100);
        CHECK(buffer.GetPitch() == 100);

        buffer.SetFormat(CMP_FORMAT_DXT1);
        CHECK(buffer.GetFormat() == CMP_FORMAT_DXT1);

        buffer.SetTranscodeFormat(CMP_FORMAT_ARGB_8888);
        CHECK(buffer.GetTranscodeFormat() == CMP_FORMAT_ARGB_8888);

        buffer.SetDataSize(1024);
        CHECK(buffer.GetDataSize() == 1024);

        buffer.SetBlockWidth(16);
        CHECK(buffer.GetBlockWidth() == 16);

        buffer.SetBlockHeight(15);
        CHECK(buffer.GetBlockHeight() == 15);

        buffer.SetBlockDepth(2);
        CHECK(buffer.GetBlockDepth() == 2);
    }
}

TEST_CASE("CodecBuffer_RGBA8888S", "[CODECBUFFER]")
{
    GenerateTestData(128, true);

    const unsigned int width  = 4;
    const unsigned int height = 4;

    CCodecBuffer_RGBA8888S buffer = CCodecBuffer_RGBA8888S(k_blockWidth, k_blockHeight, 1, width, height);

    CMP_SBYTE* testData = g_sbyteData;

    SECTION("Check Properties")
    {
        CHECK(buffer.GetBufferType() == CBT_RGBA8888S);
        CHECK(buffer.GetChannelDepth() == 8);
        CHECK(buffer.GetChannelCount() == 4);
        CHECK_FALSE(buffer.IsFloat());
        CHECK(buffer.GetWidth() == width);
        CHECK(buffer.GetHeight() == height);
        CHECK(buffer.GetPitch() == width * 4);
        CHECK(buffer.GetFormat() == CMP_FORMAT_RGBA_8888_S);
        CHECK(buffer.GetTranscodeFormat() == CMP_FORMAT_Unknown);
        CHECK(buffer.GetBlockWidth() == k_blockWidth);
        CHECK(buffer.GetBlockHeight() == k_blockHeight);
        CHECK(buffer.GetBlockDepth() == 1);
        CHECK(buffer.GetData() != 0);
        CHECK(buffer.GetDataSize() == width * height * 4);
    }

    SECTION("WriteBlockR")
    {
        RunChannelTest<CMP_BYTE>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_SBYTE>(&buffer, RED_CHANNEL, false);
        RunChannelTest<CMP_WORD>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, RED_CHANNEL, true);
        RunChannelTest<float>(&buffer, RED_CHANNEL, true);
        RunChannelTest<double>(&buffer, RED_CHANNEL, true);
    }

    SECTION("WriteBlockG")
    {
        RunChannelTest<CMP_BYTE>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_SBYTE>(&buffer, GREEN_CHANNEL, false);
        RunChannelTest<CMP_WORD>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<float>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<double>(&buffer, GREEN_CHANNEL, true);
    }

    SECTION("WriteBlockB")
    {
        RunChannelTest<CMP_BYTE>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_SBYTE>(&buffer, BLUE_CHANNEL, false);
        RunChannelTest<CMP_WORD>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<float>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<double>(&buffer, BLUE_CHANNEL, true);
    }

    SECTION("WriteBlockA")
    {
        RunChannelTest<CMP_BYTE>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<CMP_SBYTE>(&buffer, ALPHA_CHANNEL, false);
        RunChannelTest<CMP_WORD>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<float>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<double>(&buffer, ALPHA_CHANNEL, true);
    }

    SECTION("WriteBlockRGBA")
    {
        RunRGBATest<CMP_BYTE>(&buffer, true);
        RunRGBATest<CMP_SBYTE>(&buffer, false);
        RunRGBATest<CMP_WORD>(&buffer, true);
        RunRGBATest<CMP_DWORD>(&buffer, true);
        RunRGBATest<CMP_HALF>(&buffer, true);
        RunRGBATest<float>(&buffer, true);
        RunRGBATest<double>(&buffer, true);
    }

    SECTION("Multiple Block Input")
    {
        const uint inputWidth  = 16;
        const uint inputHeight = 16;

        GenerateTestData(1024);
        testData = g_sbyteData;

        CCodecBuffer_RGBA8888S tempBuffer(4, 4, 1, inputWidth, inputHeight);

        CHECK_RETURN_WRAPPER(tempBuffer.WriteBlockRGBA(0, 0, inputWidth, inputHeight, testData));

        // check that the internal data matches the input data

        CMP_SBYTE* bufferData = (CMP_SBYTE*)tempBuffer.GetData();

        for (uint i = 0; i < inputWidth * inputHeight * 4; ++i)
        {
            CheckEqual(bufferData[i], testData[i]);
        }

        // check some of the blocks
        VerifyOffsetBlock(&tempBuffer, 0, 0, testData);
        VerifyOffsetBlock(&tempBuffer, 1, 0, testData);
        VerifyOffsetBlock(&tempBuffer, 3, 2, testData);
    }

    SECTION("Test Padding")
    {
        // This test is to make sure that the padding capabilities of the codec buffer work properly
        // When the requested width to write is more than the width of the source
        //  we pad by repeating the sequence of values for the rest of the line
        // When the requested height is more than the height of the source we similarly
        //  repeat the previous rows of data for the rest of the block

        CHECK_RETURN_WRAPPER(buffer.WriteBlockRGBA(0, 0, width, height, testData));

        CMP_SBYTE block[8 * 8] = {};

        CHECK_RETURN_WRAPPER(buffer.ReadBlockR(0, 0, 8, 8, block));

        // check the first 4 rows of real data with line padding

        for (uint row = 0; row < 4; ++row)
        {
            for (uint col = 0; col < 8; ++col)
            {
                CMP_SBYTE testValue = testData[row * width * 4 + (col % 4) * 4 + 2];
                CheckEqual(block[row * 8 + col], testValue);
            }
        }

        // check that the last 4 rows are just repeats of the first 4 from block padding

        uint midPoint = 8 * 4;
        for (uint i = 0; i < midPoint; ++i)
        {
            CheckEqual(block[i], block[i + midPoint]);
        }
    }

    SECTION("Offset Writing")
    {
        // write block with x offset of 2 and y offset of 1, so each channel should look like:
        //       0 0 0 0
        //       0 0 0 1
        //       0 0 2 3
        //       0 0 4 5

        static const CMP_SBYTE expectedValues[] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 3, 0, 0, 4, 5};

        int xOffset = 2;
        int yOffset = 1;

        CHECK(buffer.WriteBlockR(xOffset, yOffset, 4, 4, testData));
        CHECK(buffer.WriteBlockG(xOffset, yOffset, 4, 4, testData));
        CHECK(buffer.WriteBlockB(xOffset, yOffset, 4, 4, testData));
        CHECK(buffer.WriteBlockA(xOffset, yOffset, 4, 4, testData));

        CMP_SBYTE blockR[width * height] = {};
        CMP_SBYTE blockG[width * height] = {};
        CMP_SBYTE blockB[width * height] = {};
        CMP_SBYTE blockA[width * height] = {};

        CHECK(buffer.ReadBlockR(0, 0, 4, 4, blockR));
        CHECK(buffer.ReadBlockG(0, 0, 4, 4, blockG));
        CHECK(buffer.ReadBlockB(0, 0, 4, 4, blockB));
        CHECK(buffer.ReadBlockA(0, 0, 4, 4, blockA));

        for (uint i = 0; i < width * height; ++i)
        {
            CHECK(blockR[i] == expectedValues[i]);
            CHECK(blockG[i] == expectedValues[i]);
            CHECK(blockB[i] == expectedValues[i]);
            CHECK(blockA[i] == expectedValues[i]);
        }

        // checking WriteBlockRGBA

        static const CMP_SBYTE expectedRGBAValues[] = {0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,
                                                       0,  0,  0,  1,  2, 3, 4, 5, 6, 7, 0, 0, 0,  0,  0,  0,  0,  0,  8,  9, 10, 11,
                                                       12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 16, 17, 18, 19, 20, 21, 22, 23};

        CHECK(buffer.WriteBlockRGBA(xOffset, yOffset, 4, 4, testData));

        CMP_SBYTE block[width * height * 4] = {};

        CHECK(buffer.ReadBlockRGBA(0, 0, 4, 4, block));

        for (uint i = 0; i < width * height * 4; ++i)
        {
            CHECK(block[i] == expectedRGBAValues[i]);
        }
    }

    SECTION("Offset Reading")
    {
        CHECK_RETURN_WRAPPER(buffer.WriteBlockRGBA(0, 0, 4, 4, testData));

        static const unsigned int xOffset    = 1;
        static const unsigned int yOffset    = 2;
        static const unsigned int readWidth  = 3;
        static const unsigned int readHeight = 2;

        static const CMP_SBYTE expectedRGBAValues[] = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};

        CMP_SBYTE block[width * height * 4] = {};
        CHECK_RETURN_WRAPPER(buffer.ReadBlockRGBA(xOffset, yOffset, readWidth, readHeight, block));

        for (uint i = 0; i < sizeof(expectedRGBAValues) / sizeof(expectedRGBAValues[0]); ++i)
        {
            CHECK(block[i] == expectedRGBAValues[i]);
        }

        // recall that for regular read and write functions, the codec buffers use BGRA order

        static const CMP_SBYTE expectedRValues[] = {38, 42, 46, 54, 58, 62};
        static const CMP_SBYTE expectedGValues[] = {37, 41, 45, 53, 57, 61};
        static const CMP_SBYTE expectedBValues[] = {36, 40, 44, 52, 56, 60};
        static const CMP_SBYTE expectedAValues[] = {39, 43, 47, 55, 59, 63};

        CMP_SBYTE blockR[width * height] = {};
        CMP_SBYTE blockG[width * height] = {};
        CMP_SBYTE blockB[width * height] = {};
        CMP_SBYTE blockA[width * height] = {};

        CHECK(buffer.ReadBlockR(xOffset, yOffset, readWidth, readHeight, blockR));
        CHECK(buffer.ReadBlockG(xOffset, yOffset, readWidth, readHeight, blockG));
        CHECK(buffer.ReadBlockB(xOffset, yOffset, readWidth, readHeight, blockB));
        CHECK(buffer.ReadBlockA(xOffset, yOffset, readWidth, readHeight, blockA));

        for (uint i = 0; i < sizeof(expectedRValues) / sizeof(expectedRValues[0]); ++i)
        {
            CHECK(blockR[i] == expectedRValues[i]);
            CHECK(blockG[i] == expectedGValues[i]);
            CHECK(blockB[i] == expectedBValues[i]);
            CHECK(blockA[i] == expectedAValues[i]);
        }
    }

    SECTION("Pitch Change")
    {
        uint newPitch = 2 * width * sizeof(CMP_SBYTE) * 4;

        CCodecBuffer_RGBA8888S altBuffer(k_blockWidth, k_blockHeight, 1, width, height, newPitch);

        bool error = altBuffer.WriteBlockRGBA(0, 0, 4, 4, testData);
        CHECK(error);

        CMP_BYTE* data = altBuffer.GetData();
        REQUIRE(altBuffer.GetDataSize() == newPitch * height);

        for (uint row = 0; row < height; ++row)
        {
            for (uint col = 0; col < width * 2; ++col)
            {
                for (uint channel = 0; channel < 4; ++channel)
                {
                    if (col >= 4)
                        CHECK((int)data[row * newPitch + col * 4 + channel] == 0);
                    else
                        CHECK((int)data[row * newPitch + col * 4 + channel] == (int)g_byteData[row * width * 4 + col * 4 + channel]);
                }
            }
        }
    }

    SECTION("Copy Function")
    {
        // The copy function of codec buffers is only concerned with copying the data contained within the buffer parameter
        // It fails if the two buffers don't have the same width and height

        bool error = buffer.WriteBlockRGBA(0, 0, 4, 4, testData);
        CHECK(error);

        CCodecBuffer_RGBA8888S copiedBuffer(k_blockWidth, k_blockHeight, 1, width, height);
        copiedBuffer.Copy(buffer);

        CMP_BYTE* srcData  = buffer.GetData();
        CMP_BYTE* destData = copiedBuffer.GetData();

        CHECK(memcmp(srcData, destData, width * height * 4) == 0);

        // test with incompatible buffer

        CCodecBuffer_RGBA8888S badBuffer(1, 1, 1, 1, 1);
        CHECK_NOTHROW(badBuffer.Copy(buffer));

        CMP_BYTE* badBufferData = badBuffer.GetData();

        CHECK(memcmp(srcData, badBufferData, 1 * 1 * 4) != 0);
    }

    SECTION("Reading Before Writing")
    {
        CMP_SBYTE result[16 * 4] = {};
        buffer.ReadBlockRGBA(0, 0, 4, 4, result);

        for (int i = 0; i < sizeof(result); ++i)
        {
            CHECK(result[i] == 0);
        }
    }

    SECTION("Setter and Getter Functions")
    {
        buffer.SetPitch(100);
        CHECK(buffer.GetPitch() == 100);

        buffer.SetFormat(CMP_FORMAT_DXT1);
        CHECK(buffer.GetFormat() == CMP_FORMAT_DXT1);

        buffer.SetTranscodeFormat(CMP_FORMAT_ARGB_8888);
        CHECK(buffer.GetTranscodeFormat() == CMP_FORMAT_ARGB_8888);

        buffer.SetDataSize(1024);
        CHECK(buffer.GetDataSize() == 1024);

        buffer.SetBlockWidth(16);
        CHECK(buffer.GetBlockWidth() == 16);

        buffer.SetBlockHeight(15);
        CHECK(buffer.GetBlockHeight() == 15);

        buffer.SetBlockDepth(2);
        CHECK(buffer.GetBlockDepth() == 2);
    }
}

TEST_CASE("CodecBuffer_RGB888", "[CODECBUFFER]")
{
    GenerateTestData(128);

    const unsigned int width  = 4;
    const unsigned int height = 4;

    CCodecBuffer_RGB888 buffer = CCodecBuffer_RGB888(k_blockWidth, k_blockHeight, 1, width, height);

    CMP_BYTE* testData = g_byteData;

    SECTION("Check Properties")
    {
        CHECK(buffer.GetBufferType() == CBT_RGB888);
        CHECK(buffer.GetChannelDepth() == 8);
        CHECK(buffer.GetChannelCount() == 3);
        CHECK_FALSE(buffer.IsFloat());
        CHECK(buffer.GetWidth() == width);
        CHECK(buffer.GetHeight() == height);
        CHECK(buffer.GetPitch() == width * 3);
        CHECK(buffer.GetFormat() == CMP_FORMAT_RGB_888);
        CHECK(buffer.GetTranscodeFormat() == CMP_FORMAT_Unknown);
        CHECK(buffer.GetBlockWidth() == k_blockWidth);
        CHECK(buffer.GetBlockHeight() == k_blockHeight);
        CHECK(buffer.GetBlockDepth() == 1);
        CHECK(buffer.GetData() != 0);
        CHECK(buffer.GetDataSize() == width * height * 3);
    }

    SECTION("WriteBlockR")
    {
        RunChannelTest<CMP_BYTE>(&buffer, RED_CHANNEL, false);
        RunChannelTest<CMP_SBYTE>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_WORD>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, RED_CHANNEL, true);
        RunChannelTest<float>(&buffer, RED_CHANNEL, true);
        RunChannelTest<double>(&buffer, RED_CHANNEL, true);
    }

    SECTION("WriteBlockG")
    {
        RunChannelTest<CMP_BYTE>(&buffer, GREEN_CHANNEL, false);
        RunChannelTest<CMP_SBYTE>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_WORD>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<float>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<double>(&buffer, GREEN_CHANNEL, true);
    }

    SECTION("WriteBlockB")
    {
        RunChannelTest<CMP_BYTE>(&buffer, BLUE_CHANNEL, false);
        RunChannelTest<CMP_SBYTE>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_WORD>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<float>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<double>(&buffer, BLUE_CHANNEL, true);
    }

    SECTION("WriteBlockA")
    {
        RunChannelTest<CMP_BYTE>(&buffer, ALPHA_CHANNEL, false);
        //RunChannelTest<CMP_SBYTE>(&buffer, ALPHA_CHANNEL, true);
        //RunChannelTest<CMP_WORD>(&buffer, ALPHA_CHANNEL, true);
        //RunChannelTest<CMP_DWORD>(&buffer, ALPHA_CHANNEL, true);
        //RunChannelTest<CMP_HALF>(&buffer, ALPHA_CHANNEL, true);
        //RunChannelTest<float>(&buffer, ALPHA_CHANNEL, true);
        //RunChannelTest<double>(&buffer, ALPHA_CHANNEL, true);
    }

    SECTION("WriteBlockRGBA")
    {
        RunRGBATest<CMP_BYTE>(&buffer, false);
        RunRGBATest<CMP_SBYTE>(&buffer, true);
        RunRGBATest<CMP_WORD>(&buffer, true);
        RunRGBATest<CMP_DWORD>(&buffer, true);
        RunRGBATest<CMP_HALF>(&buffer, true);
        RunRGBATest<float>(&buffer, true);
        RunRGBATest<double>(&buffer, true);
    }

    SECTION("Multiple Block Input")
    {
        const uint inputWidth  = 16;
        const uint inputHeight = 16;

        GenerateTestData(1024);
        testData = g_byteData;

        CCodecBuffer_RGB888 tempBuffer(4, 4, 1, inputWidth, inputHeight);

        CHECK_RETURN_WRAPPER(tempBuffer.WriteBlockRGBA(0, 0, inputWidth, inputHeight, testData));

        // check that the internal data matches the input data

        CMP_BYTE* bufferData = tempBuffer.GetData();

        for (uint row = 0; row < inputHeight; ++row)
        {
            for (uint col = 0; col < inputWidth; ++col)
            {
                for (uint channel = 0; channel < 3; ++channel)
                {
                    CheckEqual(bufferData[row * inputWidth * 3 + col * 3 + channel], testData[row * inputWidth * 4 + col * 4 + channel]);
                }
            }
        }

        // check some of the blocks
        VerifyOffsetBlock(&tempBuffer, 0, 0, testData);
        VerifyOffsetBlock(&tempBuffer, 1, 0, testData);
        VerifyOffsetBlock(&tempBuffer, 3, 2, testData);
    }

    SECTION("Test Padding")
    {
        // This test is to make sure that the padding capabilities of the codec buffer work properly
        // When the requested width to write is more than the width of the source
        //  we pad by repeating the sequence of values for the rest of the line
        // When the requested height is more than the height of the source we similarly
        //  repeat the previous rows of data for the rest of the block

        CHECK_RETURN_WRAPPER(buffer.WriteBlockRGBA(0, 0, width, height, testData));

        CMP_BYTE block[8 * 8] = {};

        CHECK_RETURN_WRAPPER(buffer.ReadBlockR(0, 0, 8, 8, block));

        // check the first 4 rows of real data with line padding

        for (uint row = 0; row < 4; ++row)
        {
            for (uint col = 0; col < 8; ++col)
            {
                CMP_BYTE testValue = testData[row * width * 4 + (col % 4) * 4 + 2];
                CheckEqual(block[row * 8 + col], testValue);
            }
        }

        // check that the last 4 rows are just repeats of the first 4 from block padding

        uint midPoint = 8 * 4;
        for (uint i = 0; i < midPoint; ++i)
        {
            CheckEqual(block[i], block[i + midPoint]);
        }
    }

    SECTION("Offset Writing")
    {
        // write block with x offset of 2 and y offset of 1, so each channel should look like:
        //       0 0 0 0
        //       0 0 0 1
        //       0 0 2 3
        //       0 0 4 5

        static const CMP_BYTE expectedValues[] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 3, 0, 0, 4, 5};

        int xOffset = 2;
        int yOffset = 1;

        CHECK(buffer.WriteBlockR(xOffset, yOffset, 4, 4, testData));
        CHECK(buffer.WriteBlockG(xOffset, yOffset, 4, 4, testData));
        CHECK(buffer.WriteBlockB(xOffset, yOffset, 4, 4, testData));

        CMP_BYTE blockR[width * height] = {};
        CMP_BYTE blockG[width * height] = {};
        CMP_BYTE blockB[width * height] = {};

        CHECK(buffer.ReadBlockR(0, 0, 4, 4, blockR));
        CHECK(buffer.ReadBlockG(0, 0, 4, 4, blockG));
        CHECK(buffer.ReadBlockB(0, 0, 4, 4, blockB));

        for (uint i = 0; i < width * height; ++i)
        {
            CHECK(blockR[i] == expectedValues[i]);
            CHECK(blockG[i] == expectedValues[i]);
            CHECK(blockB[i] == expectedValues[i]);
        }

        // checking WriteBlockRGBA

        static const CMP_BYTE expectedRGBAValues[] = {0,  0,    0,  0xFF, 0, 0,    0, 0xFF, 0, 0,    0, 0xFF, 0,  0,    0,  0xFF, 0,  0,    0,  0xFF, 0,  0,
                                                      0,  0xFF, 0,  1,    2, 0xFF, 4, 5,    6, 0xFF, 0, 0,    0,  0xFF, 0,  0,    0,  0xFF, 8,  9,    10, 0xFF,
                                                      12, 13,   14, 0xFF, 0, 0,    0, 0xFF, 0, 0,    0, 0xFF, 16, 17,   18, 0xFF, 20, 21,   22, 0xFF};

        CHECK(buffer.WriteBlockRGBA(xOffset, yOffset, 4, 4, testData));

        CMP_BYTE block[width * height * 4] = {};

        CHECK(buffer.ReadBlockRGBA(0, 0, 4, 4, block));

        for (uint i = 0; i < width * height * 4; ++i)
        {
            CHECK(block[i] == expectedRGBAValues[i]);
        }
    }

    SECTION("Offset Reading")
    {
        CHECK_RETURN_WRAPPER(buffer.WriteBlockRGBA(0, 0, 4, 4, testData));

        static const unsigned int xOffset    = 1;
        static const unsigned int yOffset    = 2;
        static const unsigned int readWidth  = 3;
        static const unsigned int readHeight = 2;

        static const CMP_BYTE expectedRGBAValues[] = {36, 37, 38, 0xFF, 40, 41, 42, 0xFF, 44, 45, 46, 0xFF,
                                                      52, 53, 54, 0xFF, 56, 57, 58, 0xFF, 60, 61, 62, 0xFF};

        CMP_BYTE block[width * height * 4] = {};
        CHECK_RETURN_WRAPPER(buffer.ReadBlockRGBA(xOffset, yOffset, readWidth, readHeight, block));

        for (uint i = 0; i < sizeof(expectedRGBAValues) / sizeof(expectedRGBAValues[0]); ++i)
        {
            CHECK(block[i] == expectedRGBAValues[i]);
        }

        // recall that for regular read and write functions, the codec buffers use BGRA order

        static const CMP_BYTE expectedRValues[] = {38, 42, 46, 54, 58, 62};
        static const CMP_BYTE expectedGValues[] = {37, 41, 45, 53, 57, 61};
        static const CMP_BYTE expectedBValues[] = {36, 40, 44, 52, 56, 60};

        CMP_BYTE blockR[width * height] = {};
        CMP_BYTE blockG[width * height] = {};
        CMP_BYTE blockB[width * height] = {};

        CHECK(buffer.ReadBlockR(xOffset, yOffset, readWidth, readHeight, blockR));
        CHECK(buffer.ReadBlockG(xOffset, yOffset, readWidth, readHeight, blockG));
        CHECK(buffer.ReadBlockB(xOffset, yOffset, readWidth, readHeight, blockB));

        for (uint i = 0; i < sizeof(expectedRValues) / sizeof(expectedRValues[0]); ++i)
        {
            CHECK(blockR[i] == expectedRValues[i]);
            CHECK(blockG[i] == expectedGValues[i]);
            CHECK(blockB[i] == expectedBValues[i]);
        }
    }

    SECTION("Pitch Change")
    {
        uint channelCount = buffer.GetChannelCount();
        uint newPitch     = 2 * width * sizeof(CMP_BYTE) * channelCount;

        CCodecBuffer_RGB888 altBuffer(k_blockWidth, k_blockHeight, 1, width, height, newPitch);

        bool error = altBuffer.WriteBlockRGBA(0, 0, 4, 4, testData);
        CHECK(error);

        CMP_BYTE* data = altBuffer.GetData();
        REQUIRE(altBuffer.GetDataSize() == newPitch * height);

        for (uint row = 0; row < height; ++row)
        {
            for (uint col = 0; col < width * 2; ++col)
            {
                for (uint channel = 0; channel < channelCount; ++channel)
                {
                    if (col >= width)
                        CHECK((int)data[row * newPitch + col * channelCount + channel] == 0);
                    else
                        CHECK((int)data[row * newPitch + col * channelCount + channel] == (int)g_byteData[row * width * 4 + col * 4 + channel]);
                }
            }
        }
    }

    SECTION("Copy Function")
    {
        // The copy function of codec buffers is only concerned with copying the data contained within the buffer parameter
        // It fails if the two buffers don't have the same width and height

        bool error = buffer.WriteBlockRGBA(0, 0, 4, 4, testData);
        CHECK(error);

        CCodecBuffer_RGB888 copiedBuffer(k_blockWidth, k_blockHeight, 1, width, height);
        copiedBuffer.Copy(buffer);

        CMP_BYTE* srcData  = buffer.GetData();
        CMP_BYTE* destData = copiedBuffer.GetData();

        CHECK(memcmp(srcData, destData, width * height * buffer.GetChannelCount()) == 0);

        // test with incompatible buffer

        CCodecBuffer_RGB888 badBuffer(1, 1, 1, 1, 1);
        CHECK_NOTHROW(badBuffer.Copy(buffer));

        CMP_BYTE* badBufferData = badBuffer.GetData();

        CHECK(memcmp(srcData, badBufferData, 1 * 1 * 4) != 0);
    }

    SECTION("Reading Before Writing")
    {
        CMP_BYTE result[4 * 4 * 4] = {};

        buffer.ReadBlockRGBA(0, 0, 4, 4, result);

        for (int i = 0; i < sizeof(result) / sizeof(result[0]); ++i)
        {
            if (i % 4 == 3)
                CHECK(result[i] == (CMP_BYTE)0xFF);
            else
                CHECK(result[i] == 0);
        }
    }

    SECTION("Setter and Getter Functions")
    {
        buffer.SetPitch(100);
        CHECK(buffer.GetPitch() == 100);

        buffer.SetFormat(CMP_FORMAT_DXT1);
        CHECK(buffer.GetFormat() == CMP_FORMAT_DXT1);

        buffer.SetTranscodeFormat(CMP_FORMAT_ARGB_8888);
        CHECK(buffer.GetTranscodeFormat() == CMP_FORMAT_ARGB_8888);

        buffer.SetDataSize(1024);
        CHECK(buffer.GetDataSize() == 1024);

        buffer.SetBlockWidth(16);
        CHECK(buffer.GetBlockWidth() == 16);

        buffer.SetBlockHeight(15);
        CHECK(buffer.GetBlockHeight() == 15);

        buffer.SetBlockDepth(2);
        CHECK(buffer.GetBlockDepth() == 2);
    }
}

TEST_CASE("CodecBuffer_RGB888S", "[CODECBUFFER]")
{
    GenerateTestData(128, true);

    const unsigned int width  = 4;
    const unsigned int height = 4;

    CCodecBuffer_RGB888S buffer = CCodecBuffer_RGB888S(k_blockWidth, k_blockHeight, 1, width, height);

    CMP_SBYTE* testData = g_sbyteData;

    SECTION("Check Properties")
    {
        CHECK(buffer.GetBufferType() == CBT_RGB888S);
        CHECK(buffer.GetChannelDepth() == 8);
        CHECK(buffer.GetChannelCount() == 3);
        CHECK_FALSE(buffer.IsFloat());
        CHECK(buffer.GetWidth() == width);
        CHECK(buffer.GetHeight() == height);
        CHECK(buffer.GetPitch() == width * 3);
        CHECK(buffer.GetFormat() == CMP_FORMAT_RGB_888_S);
        CHECK(buffer.GetTranscodeFormat() == CMP_FORMAT_Unknown);
        CHECK(buffer.GetBlockWidth() == k_blockWidth);
        CHECK(buffer.GetBlockHeight() == k_blockHeight);
        CHECK(buffer.GetBlockDepth() == 1);
        CHECK(buffer.GetData() != 0);
        CHECK(buffer.GetDataSize() == width * height * 3);
    }

    SECTION("WriteBlockR")
    {
        RunChannelTest<CMP_BYTE>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_SBYTE>(&buffer, RED_CHANNEL, false);
        RunChannelTest<CMP_WORD>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, RED_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, RED_CHANNEL, true);
        RunChannelTest<float>(&buffer, RED_CHANNEL, true);
        RunChannelTest<double>(&buffer, RED_CHANNEL, true);
    }

    SECTION("WriteBlockG")
    {
        RunChannelTest<CMP_BYTE>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_SBYTE>(&buffer, GREEN_CHANNEL, false);
        RunChannelTest<CMP_WORD>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<float>(&buffer, GREEN_CHANNEL, true);
        RunChannelTest<double>(&buffer, GREEN_CHANNEL, true);
    }

    SECTION("WriteBlockB")
    {
        RunChannelTest<CMP_BYTE>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_SBYTE>(&buffer, BLUE_CHANNEL, false);
        RunChannelTest<CMP_WORD>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_DWORD>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<CMP_HALF>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<float>(&buffer, BLUE_CHANNEL, true);
        RunChannelTest<double>(&buffer, BLUE_CHANNEL, true);
    }

    SECTION("WriteBlockA")
    {
        //RunChannelTest<CMP_BYTE>(&buffer, ALPHA_CHANNEL, true);
        RunChannelTest<CMP_SBYTE>(&buffer, ALPHA_CHANNEL, false);
        //RunChannelTest<CMP_WORD>(&buffer, ALPHA_CHANNEL, true);
        //RunChannelTest<CMP_DWORD>(&buffer, ALPHA_CHANNEL, true);
        //RunChannelTest<CMP_HALF>(&buffer, ALPHA_CHANNEL, true);
        //RunChannelTest<float>(&buffer, ALPHA_CHANNEL, true);
        //RunChannelTest<double>(&buffer, ALPHA_CHANNEL, true);
    }

    SECTION("WriteBlockRGBA")
    {
        RunRGBATest<CMP_BYTE>(&buffer, true);
        RunRGBATest<CMP_SBYTE>(&buffer, false);
        RunRGBATest<CMP_WORD>(&buffer, true);
        RunRGBATest<CMP_DWORD>(&buffer, true);
        RunRGBATest<CMP_HALF>(&buffer, true);
        RunRGBATest<float>(&buffer, true);
        RunRGBATest<double>(&buffer, true);
    }

    SECTION("Multiple Block Input")
    {
        const uint inputWidth  = 16;
        const uint inputHeight = 16;

        GenerateTestData(1024);
        testData = g_sbyteData;

        CCodecBuffer_RGB888S tempBuffer(4, 4, 1, inputWidth, inputHeight);

        CHECK_RETURN_WRAPPER(tempBuffer.WriteBlockRGBA(0, 0, inputWidth, inputHeight, testData));

        // check that the internal data matches the input data

        CMP_SBYTE* bufferData = (CMP_SBYTE*)tempBuffer.GetData();

        for (uint row = 0; row < inputHeight; ++row)
        {
            for (uint col = 0; col < inputWidth; ++col)
            {
                for (uint channel = 0; channel < 3; ++channel)
                {
                    CheckEqual(bufferData[row * inputWidth * 3 + col * 3 + channel], testData[row * inputWidth * 4 + col * 4 + channel]);
                }
            }
        }

        // check some of the blocks
        VerifyOffsetBlock(&tempBuffer, 0, 0, testData);
        VerifyOffsetBlock(&tempBuffer, 1, 0, testData);
        VerifyOffsetBlock(&tempBuffer, 3, 2, testData);
    }

    SECTION("Test Padding")
    {
        // This test is to make sure that the padding capabilities of the codec buffer work properly
        // When the requested width to write is more than the width of the source
        //  we pad by repeating the sequence of values for the rest of the line
        // When the requested height is more than the height of the source we similarly
        //  repeat the previous rows of data for the rest of the block

        CHECK_RETURN_WRAPPER(buffer.WriteBlockRGBA(0, 0, width, height, testData));

        CMP_SBYTE block[8 * 8] = {};

        CHECK_RETURN_WRAPPER(buffer.ReadBlockR(0, 0, 8, 8, block));

        // check the first 4 rows of real data with line padding

        for (uint row = 0; row < 4; ++row)
        {
            for (uint col = 0; col < 8; ++col)
            {
                CMP_SBYTE testValue = testData[row * width * 4 + (col % 4) * 4 + 2];
                CheckEqual(block[row * 8 + col], testValue);
            }
        }

        // check that the last 4 rows are just repeats of the first 4 from block padding

        uint midPoint = 8 * 4;
        for (uint i = 0; i < midPoint; ++i)
        {
            CheckEqual(block[i], block[i + midPoint]);
        }
    }

    SECTION("Offset Writing")
    {
        // write block with x offset of 2 and y offset of 1, so each channel should look like:
        //       0 0 0 0
        //       0 0 0 1
        //       0 0 2 3
        //       0 0 4 5

        static const CMP_SBYTE expectedValues[] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 3, 0, 0, 4, 5};

        int xOffset = 2;
        int yOffset = 1;

        CHECK(buffer.WriteBlockR(xOffset, yOffset, 4, 4, testData));
        CHECK(buffer.WriteBlockG(xOffset, yOffset, 4, 4, testData));
        CHECK(buffer.WriteBlockB(xOffset, yOffset, 4, 4, testData));

        CMP_SBYTE blockR[width * height] = {};
        CMP_SBYTE blockG[width * height] = {};
        CMP_SBYTE blockB[width * height] = {};

        CHECK(buffer.ReadBlockR(0, 0, 4, 4, blockR));
        CHECK(buffer.ReadBlockG(0, 0, 4, 4, blockG));
        CHECK(buffer.ReadBlockB(0, 0, 4, 4, blockB));

        for (uint i = 0; i < width * height; ++i)
        {
            CHECK(blockR[i] == expectedValues[i]);
            CHECK(blockG[i] == expectedValues[i]);
            CHECK(blockB[i] == expectedValues[i]);
        }

        // checking WriteBlockRGBA

        static const CMP_SBYTE expectedRGBAValues[] = {0,  0,    0,  0xFF, 0, 0,    0, 0xFF, 0, 0,    0, 0xFF, 0,  0,    0,  0xFF, 0,  0,    0,  0xFF, 0,  0,
                                                       0,  0xFF, 0,  1,    2, 0xFF, 4, 5,    6, 0xFF, 0, 0,    0,  0xFF, 0,  0,    0,  0xFF, 8,  9,    10, 0xFF,
                                                       12, 13,   14, 0xFF, 0, 0,    0, 0xFF, 0, 0,    0, 0xFF, 16, 17,   18, 0xFF, 20, 21,   22, 0xFF};

        CHECK(buffer.WriteBlockRGBA(xOffset, yOffset, 4, 4, testData));

        CMP_SBYTE block[width * height * 4] = {};

        CHECK(buffer.ReadBlockRGBA(0, 0, 4, 4, block));

        for (uint i = 0; i < width * height * 4; ++i)
        {
            CHECK(block[i] == expectedRGBAValues[i]);
        }
    }

    SECTION("Offset Reading")
    {
        CHECK_RETURN_WRAPPER(buffer.WriteBlockRGBA(0, 0, 4, 4, testData));

        static const unsigned int xOffset    = 1;
        static const unsigned int yOffset    = 2;
        static const unsigned int readWidth  = 3;
        static const unsigned int readHeight = 2;

        static const CMP_SBYTE expectedRGBAValues[] = {36, 37, 38, 0xFF, 40, 41, 42, 0xFF, 44, 45, 46, 0xFF,
                                                       52, 53, 54, 0xFF, 56, 57, 58, 0xFF, 60, 61, 62, 0xFF};

        CMP_SBYTE block[width * height * 4] = {};
        CHECK_RETURN_WRAPPER(buffer.ReadBlockRGBA(xOffset, yOffset, readWidth, readHeight, block));

        for (uint i = 0; i < sizeof(expectedRGBAValues) / sizeof(expectedRGBAValues[0]); ++i)
        {
            CHECK(block[i] == expectedRGBAValues[i]);
        }

        // recall that for regular read and write functions, the codec buffers use BGRA order

        static const CMP_SBYTE expectedRValues[] = {38, 42, 46, 54, 58, 62};
        static const CMP_SBYTE expectedGValues[] = {37, 41, 45, 53, 57, 61};
        static const CMP_SBYTE expectedBValues[] = {36, 40, 44, 52, 56, 60};

        CMP_SBYTE blockR[width * height] = {};
        CMP_SBYTE blockG[width * height] = {};
        CMP_SBYTE blockB[width * height] = {};

        CHECK(buffer.ReadBlockR(xOffset, yOffset, readWidth, readHeight, blockR));
        CHECK(buffer.ReadBlockG(xOffset, yOffset, readWidth, readHeight, blockG));
        CHECK(buffer.ReadBlockB(xOffset, yOffset, readWidth, readHeight, blockB));

        for (uint i = 0; i < sizeof(expectedRValues) / sizeof(expectedRValues[0]); ++i)
        {
            CHECK(blockR[i] == expectedRValues[i]);
            CHECK(blockG[i] == expectedGValues[i]);
            CHECK(blockB[i] == expectedBValues[i]);
        }
    }

    SECTION("Pitch Change")
    {
        uint channelCount = buffer.GetChannelCount();
        uint newPitch     = 2 * width * sizeof(CMP_SBYTE) * channelCount;

        CCodecBuffer_RGB888S altBuffer(k_blockWidth, k_blockHeight, 1, width, height, newPitch);

        bool error = altBuffer.WriteBlockRGBA(0, 0, 4, 4, testData);
        CHECK(error);

        CMP_BYTE* data = altBuffer.GetData();
        REQUIRE(altBuffer.GetDataSize() == newPitch * height);

        for (uint row = 0; row < height; ++row)
        {
            for (uint col = 0; col < width * 2; ++col)
            {
                for (uint channel = 0; channel < channelCount; ++channel)
                {
                    if (col >= width)
                        CHECK((int)data[row * newPitch + col * channelCount + channel] == 0);
                    else
                        CHECK((int)data[row * newPitch + col * channelCount + channel] == (int)g_byteData[row * width * 4 + col * 4 + channel]);
                }
            }
        }
    }

    SECTION("Copy Function")
    {
        // The copy function of codec buffers is only concerned with copying the data contained within the buffer parameter
        // It fails if the two buffers don't have the same width and height

        bool error = buffer.WriteBlockRGBA(0, 0, 4, 4, testData);
        CHECK(error);

        CCodecBuffer_RGB888S copiedBuffer(k_blockWidth, k_blockHeight, 1, width, height);
        copiedBuffer.Copy(buffer);

        CMP_BYTE* srcData  = buffer.GetData();
        CMP_BYTE* destData = copiedBuffer.GetData();

        CHECK(memcmp(srcData, destData, width * height * buffer.GetChannelCount()) == 0);

        // test with incompatible buffer

        CCodecBuffer_RGB888S badBuffer(1, 1, 1, 1, 1);
        CHECK_NOTHROW(badBuffer.Copy(buffer));

        CMP_BYTE* badBufferData = badBuffer.GetData();

        CHECK(memcmp(srcData, badBufferData, 1 * 1 * 4) != 0);
    }

    SECTION("Reading Before Writing")
    {
        CMP_SBYTE result[4 * 4 * 4] = {};

        buffer.ReadBlockRGBA(0, 0, 4, 4, result);

        for (int i = 0; i < sizeof(result) / sizeof(result[0]); ++i)
        {
            if (i % 4 == 3)
                CHECK(result[i] == (CMP_SBYTE)0xFF);
            else
                CHECK(result[i] == 0);
        }
    }

    SECTION("Setter and Getter Functions")
    {
        buffer.SetPitch(100);
        CHECK(buffer.GetPitch() == 100);

        buffer.SetFormat(CMP_FORMAT_DXT1);
        CHECK(buffer.GetFormat() == CMP_FORMAT_DXT1);

        buffer.SetTranscodeFormat(CMP_FORMAT_ARGB_8888);
        CHECK(buffer.GetTranscodeFormat() == CMP_FORMAT_ARGB_8888);

        buffer.SetDataSize(1024);
        CHECK(buffer.GetDataSize() == 1024);

        buffer.SetBlockWidth(16);
        CHECK(buffer.GetBlockWidth() == 16);

        buffer.SetBlockHeight(15);
        CHECK(buffer.GetBlockHeight() == 15);

        buffer.SetBlockDepth(2);
        CHECK(buffer.GetBlockDepth() == 2);
    }
}