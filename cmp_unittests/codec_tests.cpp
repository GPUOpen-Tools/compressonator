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

#include "codecbuffer.h"
#include "codec.h"
#include "codec_etc.h"
#include "codec_etc_rgb.h"
#include "codec_etc_rgba_interpolated.h"
#include "codec_etc_rgba_explicit.h"
#include "codec_etc2.h"
#include "codec_etc2_rgb.h"
#include "codec_etc2_rgba.h"
#include "codec_etc2_rgba1.h"

// The test data is filled completely with yellow pixels. This was done so that there would be a difference between the red and blue channels which is
// important to test that no unexpected swizzling is happening

static CMP_BYTE testBlockDataRGB[4 * 4 * 3] = {0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF,
                                               0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
                                               0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00};

static CMP_BYTE testBlockDataRGBA[4 * 4 * 4] = {0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,
                                                0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,
                                                0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,
                                                0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF};

static CCodecBuffer* GenerateTestBuffer(CMP_FORMAT bufferFormat)
{
    CCodecBuffer* buffer = 0;

    CodecBufferType bufferType = GetCodecBufferType(bufferFormat);

    CMP_BYTE* bufferData     = testBlockDataRGB;
    CMP_DWORD bufferDataSize = sizeof(testBlockDataRGB);

    if (bufferFormat == CMP_FORMAT_RGBA_8888)
    {
        bufferData     = testBlockDataRGBA;
        bufferDataSize = sizeof(testBlockDataRGBA);
    }

    buffer = CreateCodecBuffer(bufferType, 4, 4, 1, 4, 4, 0, bufferData, bufferDataSize);

    return buffer;
}

// NOTE: Currently these only test a basic block encoding and decoding (single block full of yellow)
//       Ideally, we would test a wider variety of possible functionality and types of blocks for each codec.

TEST_CASE("ETC_RGB Codec Encoding", "[CODEC][ETC]")
{
    CCodec_ETC_RGB* codec = new CCodec_ETC_RGB();

    CCodecBuffer* srcBuffer  = GenerateTestBuffer(CMP_FORMAT_RGB_888);
    CCodecBuffer* destBuffer = codec->CreateBuffer(4, 4, 1, 4, 4);

    CHECK(codec->Compress(*srcBuffer, *destBuffer) == CE_OK);

    CMP_BYTE* destBlock = destBuffer->GetData();

    CHECK(destBlock[0] == 0xF8);
    CHECK(destBlock[1] == 0xF8);
    CHECK(destBlock[2] == 0x00);
    CHECK(destBlock[3] == 0x02);
    CHECK(destBlock[4] == 0x00);
    CHECK(destBlock[5] == 0x00);
    CHECK(destBlock[6] == 0x00);
    CHECK(destBlock[7] == 0x00);

    delete srcBuffer;
    delete destBuffer;
    delete codec;
}
TEST_CASE("ETC_RGB Codec Decoding", "[CODEC][ETC]")
{
    CCodec_ETC_RGB* codec = new CCodec_ETC_RGB();

    CodecBufferType destBufferType = GetCodecBufferType(CMP_FORMAT_RGB_888);

    CMP_BYTE srcBlock[8] = {0xF8, 0xF8, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00};

    CCodecBuffer* srcBuffer  = codec->CreateBuffer(4, 4, 1, 4, 4, 0, srcBlock, sizeof(srcBlock));
    CCodecBuffer* destBuffer = CreateCodecBuffer(destBufferType, 4, 4, 1, 4, 4);

    CHECK(codec->Decompress(*srcBuffer, *destBuffer) == CE_OK);

    CMP_BYTE* destBlock = destBuffer->GetData();

    CHECK(destBlock[0] == 0xFF);
    CHECK(destBlock[1] == 0xFF);
    CHECK(destBlock[2] == 0x02);

    CHECK(destBlock[3] == 0xFF);
    CHECK(destBlock[4] == 0xFF);
    CHECK(destBlock[5] == 0x02);

    CHECK(destBlock[6] == 0xFF);
    CHECK(destBlock[7] == 0xFF);
    CHECK(destBlock[8] == 0x02);

    CHECK(destBlock[9] == 0xFF);
    CHECK(destBlock[10] == 0xFF);
    CHECK(destBlock[11] == 0x02);

    CHECK(destBlock[12] == 0xFF);
    CHECK(destBlock[13] == 0xFF);
    CHECK(destBlock[14] == 0x02);

    CHECK(destBlock[15] == 0xFF);
    CHECK(destBlock[16] == 0xFF);
    CHECK(destBlock[17] == 0x02);

    CHECK(destBlock[18] == 0xFF);
    CHECK(destBlock[19] == 0xFF);
    CHECK(destBlock[20] == 0x02);

    CHECK(destBlock[21] == 0xFF);
    CHECK(destBlock[22] == 0xFF);
    CHECK(destBlock[23] == 0x02);

    CHECK(destBlock[24] == 0xFF);
    CHECK(destBlock[25] == 0xFF);
    CHECK(destBlock[26] == 0x02);

    CHECK(destBlock[27] == 0xFF);
    CHECK(destBlock[28] == 0xFF);
    CHECK(destBlock[29] == 0x02);

    CHECK(destBlock[30] == 0xFF);
    CHECK(destBlock[31] == 0xFF);
    CHECK(destBlock[32] == 0x02);

    CHECK(destBlock[33] == 0xFF);
    CHECK(destBlock[34] == 0xFF);
    CHECK(destBlock[35] == 0x02);

    CHECK(destBlock[36] == 0xFF);
    CHECK(destBlock[37] == 0xFF);
    CHECK(destBlock[38] == 0x02);

    CHECK(destBlock[39] == 0xFF);
    CHECK(destBlock[40] == 0xFF);
    CHECK(destBlock[41] == 0x02);

    CHECK(destBlock[42] == 0xFF);
    CHECK(destBlock[43] == 0xFF);
    CHECK(destBlock[44] == 0x02);

    CHECK(destBlock[45] == 0xFF);
    CHECK(destBlock[46] == 0xFF);
    CHECK(destBlock[47] == 0x02);

    delete srcBuffer;
    delete destBuffer;
    delete codec;
}

// TODO: If ever this feature is fully implemented, these tests should be completed
#ifdef SUPPORT_ETC_ALPHA
TEST_CASE("ETC_RGBA_Interpolated Codec Encoding", "[CODEC][ETC]")
{
    CCodec_ETC_RGBA_Interpolated* codec = new CCodec_ETC_RGBA_Interpolated();

    REQUIRE(false);

    delete codec;
}
TEST_CASE("ETC_RGBA_Interpolated Codec Decoding", "[CODEC][ETC]")
{
    CCodec_ETC_RGBA_Interpolated* codec = new CCodec_ETC_RGBA_Interpolated();

    REQUIRE(false);

    delete codec;
}
#endif

// TODO: If ever this feature is fully implemented, these tests should be completed
#ifdef SUPPORT_ETC_ALPHA
TEST_CASE("ETC_RGBA_Explicit Codec Encoding", "[CODEC][ETC]")
{
    CCodec_ETC_RGBA_Explicit* codec = new CCodec_ETC_RGBA_Explicit();

    REQUIRE(false);

    delete codec;
}
TEST_CASE("ETC_RGBA_Explicit Codec Decoding", "[CODEC][ETC]")
{
    CCodec_ETC_RGBA_Explicit* codec = new CCodec_ETC_RGBA_Explicit();

    REQUIRE(false);

    delete codec;
}
#endif

TEST_CASE("ETC2_RGB Codec Encoding", "[CODEC][ETC2]")
{
    CCodec_ETC2_RGB* codec = new CCodec_ETC2_RGB(CT_ETC2_RGB);

    CCodecBuffer* srcBuffer  = GenerateTestBuffer(CMP_FORMAT_RGB_888);
    CCodecBuffer* destBuffer = codec->CreateBuffer(4, 4, 1, 4, 4);

    CHECK(codec->Compress(*srcBuffer, *destBuffer) == CE_OK);

    CMP_BYTE* destBlock = destBuffer->GetData();

    // We assume that the result of encoding the solid yellow input block will be an ETC2 block in planar mode, where each colour is yellow
    CHECK(destBlock[0] == 0x7F);
    CHECK(destBlock[1] == 0x7E);
    CHECK(destBlock[2] == 0x04);
    CHECK(destBlock[3] == 0x7F);
    CHECK(destBlock[4] == 0xFE);
    CHECK(destBlock[5] == 0x07);
    CHECK(destBlock[6] == 0xFF);
    CHECK(destBlock[7] == 0xC0);

    delete srcBuffer;
    delete destBuffer;
    delete codec;
}
TEST_CASE("ETC2_RGB Codec Decoding", "[CODEC][ETC2]")
{
    CCodec_ETC2_RGB* codec = new CCodec_ETC2_RGB(CT_ETC2_RGB);

    CMP_BYTE srcBlock[8] = {0x7F, 0x7E, 0x04, 0x7F, 0xFE, 0x07, 0xFF, 0xC0};

    CodecBufferType destBufferType = GetCodecBufferType(CMP_FORMAT_RGB_888);

    CCodecBuffer* srcBuffer  = codec->CreateBuffer(4, 4, 1, 4, 4, 0, srcBlock, sizeof(srcBlock));
    CCodecBuffer* destBuffer = CreateCodecBuffer(destBufferType, 4, 4, 1, 4, 4);

    CHECK(codec->Decompress(*srcBuffer, *destBuffer) == CE_OK);

    CMP_BYTE* destBlock = destBuffer->GetData();

    CHECK(destBlock[0] == 0xFF);
    CHECK(destBlock[1] == 0xFF);
    CHECK(destBlock[2] == 0x00);

    CHECK(destBlock[3] == 0xFF);
    CHECK(destBlock[4] == 0xFF);
    CHECK(destBlock[5] == 0x00);

    CHECK(destBlock[6] == 0xFF);
    CHECK(destBlock[7] == 0xFF);
    CHECK(destBlock[8] == 0x00);

    CHECK(destBlock[9] == 0xFF);
    CHECK(destBlock[10] == 0xFF);
    CHECK(destBlock[11] == 0x00);

    CHECK(destBlock[12] == 0xFF);
    CHECK(destBlock[13] == 0xFF);
    CHECK(destBlock[14] == 0x00);

    CHECK(destBlock[15] == 0xFF);
    CHECK(destBlock[16] == 0xFF);
    CHECK(destBlock[17] == 0x00);

    CHECK(destBlock[18] == 0xFF);
    CHECK(destBlock[19] == 0xFF);
    CHECK(destBlock[20] == 0x00);

    CHECK(destBlock[21] == 0xFF);
    CHECK(destBlock[22] == 0xFF);
    CHECK(destBlock[23] == 0x00);

    CHECK(destBlock[24] == 0xFF);
    CHECK(destBlock[25] == 0xFF);
    CHECK(destBlock[26] == 0x00);

    CHECK(destBlock[27] == 0xFF);
    CHECK(destBlock[28] == 0xFF);
    CHECK(destBlock[29] == 0x00);

    CHECK(destBlock[30] == 0xFF);
    CHECK(destBlock[31] == 0xFF);
    CHECK(destBlock[32] == 0x00);

    CHECK(destBlock[33] == 0xFF);
    CHECK(destBlock[34] == 0xFF);
    CHECK(destBlock[35] == 0x00);

    CHECK(destBlock[36] == 0xFF);
    CHECK(destBlock[37] == 0xFF);
    CHECK(destBlock[38] == 0x00);

    CHECK(destBlock[39] == 0xFF);
    CHECK(destBlock[40] == 0xFF);
    CHECK(destBlock[41] == 0x00);

    CHECK(destBlock[42] == 0xFF);
    CHECK(destBlock[43] == 0xFF);
    CHECK(destBlock[44] == 0x00);

    CHECK(destBlock[45] == 0xFF);
    CHECK(destBlock[46] == 0xFF);
    CHECK(destBlock[47] == 0x00);

    delete srcBuffer;
    delete destBuffer;
    delete codec;
}

TEST_CASE("ETC2_RGBA1 Codec Encoding", "[CODEC][ETC2]")
{
    CCodec_ETC2_RGBA1* codec = new CCodec_ETC2_RGBA1(CT_ETC2_RGBA1);

    CCodecBuffer* srcBuffer  = GenerateTestBuffer(CMP_FORMAT_RGBA_8888);
    CCodecBuffer* destBuffer = codec->CreateBuffer(4, 4, 1, 4, 4);

    CHECK(codec->Compress(*srcBuffer, *destBuffer) == CE_OK);

    CMP_BYTE* destBlock = destBuffer->GetData();

    // We expect the block to be in the simple "differential" mode with alpha disabled, so the only set bits will be the R0 and G0 colour channels
    CHECK(destBlock[0] == 0xF8);
    CHECK(destBlock[1] == 0xF8);
    CHECK(destBlock[2] == 0x00);
    CHECK(destBlock[3] == 0x00);
    CHECK(destBlock[4] == 0x00);
    CHECK(destBlock[5] == 0x00);
    CHECK(destBlock[6] == 0x00);
    CHECK(destBlock[7] == 0x00);

    delete srcBuffer;
    delete destBuffer;
    delete codec;
}
TEST_CASE("ETC2_RGBA1 Codec Decoding", "[CODEC][ETC2]")
{
    CCodec_ETC2_RGBA1* codec = new CCodec_ETC2_RGBA1(CT_ETC2_RGBA1);

    CMP_BYTE srcBlock[8] = {0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    CodecBufferType destBufferType = GetCodecBufferType(CMP_FORMAT_RGBA_8888);

    CCodecBuffer* srcBuffer  = codec->CreateBuffer(4, 4, 1, 4, 4, 0, srcBlock, sizeof(srcBlock));
    CCodecBuffer* destBuffer = CreateCodecBuffer(destBufferType, 4, 4, 1, 4, 4);

    CHECK(codec->Decompress(*srcBuffer, *destBuffer) == CE_OK);

    CMP_BYTE* destBlock = destBuffer->GetData();

    CHECK(destBlock[0] == 0xFF);
    CHECK(destBlock[1] == 0xFF);
    CHECK(destBlock[2] == 0x00);
    CHECK(destBlock[3] == 0xFF);

    CHECK(destBlock[4] == 0xFF);
    CHECK(destBlock[5] == 0xFF);
    CHECK(destBlock[6] == 0x00);
    CHECK(destBlock[7] == 0xFF);

    CHECK(destBlock[8] == 0xFF);
    CHECK(destBlock[9] == 0xFF);
    CHECK(destBlock[10] == 0x00);
    CHECK(destBlock[11] == 0xFF);

    CHECK(destBlock[12] == 0xFF);
    CHECK(destBlock[13] == 0xFF);
    CHECK(destBlock[14] == 0x00);
    CHECK(destBlock[15] == 0xFF);

    CHECK(destBlock[16] == 0xFF);
    CHECK(destBlock[17] == 0xFF);
    CHECK(destBlock[18] == 0x00);
    CHECK(destBlock[19] == 0xFF);

    CHECK(destBlock[20] == 0xFF);
    CHECK(destBlock[21] == 0xFF);
    CHECK(destBlock[22] == 0x00);
    CHECK(destBlock[23] == 0xFF);

    CHECK(destBlock[24] == 0xFF);
    CHECK(destBlock[25] == 0xFF);
    CHECK(destBlock[26] == 0x00);
    CHECK(destBlock[27] == 0xFF);

    CHECK(destBlock[28] == 0xFF);
    CHECK(destBlock[29] == 0xFF);
    CHECK(destBlock[30] == 0x00);
    CHECK(destBlock[31] == 0xFF);

    CHECK(destBlock[32] == 0xFF);
    CHECK(destBlock[33] == 0xFF);
    CHECK(destBlock[34] == 0x00);
    CHECK(destBlock[35] == 0xFF);

    CHECK(destBlock[36] == 0xFF);
    CHECK(destBlock[37] == 0xFF);
    CHECK(destBlock[38] == 0x00);
    CHECK(destBlock[39] == 0xFF);

    CHECK(destBlock[40] == 0xFF);
    CHECK(destBlock[41] == 0xFF);
    CHECK(destBlock[42] == 0x00);
    CHECK(destBlock[43] == 0xFF);

    CHECK(destBlock[44] == 0xFF);
    CHECK(destBlock[45] == 0xFF);
    CHECK(destBlock[46] == 0x00);
    CHECK(destBlock[47] == 0xFF);

    CHECK(destBlock[48] == 0xFF);
    CHECK(destBlock[49] == 0xFF);
    CHECK(destBlock[50] == 0x00);
    CHECK(destBlock[51] == 0xFF);

    CHECK(destBlock[52] == 0xFF);
    CHECK(destBlock[53] == 0xFF);
    CHECK(destBlock[54] == 0x00);
    CHECK(destBlock[55] == 0xFF);

    CHECK(destBlock[56] == 0xFF);
    CHECK(destBlock[57] == 0xFF);
    CHECK(destBlock[58] == 0x00);
    CHECK(destBlock[59] == 0xFF);

    CHECK(destBlock[60] == 0xFF);
    CHECK(destBlock[61] == 0xFF);
    CHECK(destBlock[62] == 0x00);
    CHECK(destBlock[63] == 0xFF);

    delete srcBuffer;
    delete destBuffer;
    delete codec;
}

TEST_CASE("ETC2_RGBA Codec Encoding", "[CODEC][ETC2]")
{
    CCodec_ETC2_RGBA* codec = new CCodec_ETC2_RGBA(CT_ETC2_RGBA);

    CCodecBuffer* srcBuffer  = GenerateTestBuffer(CMP_FORMAT_RGBA_8888);
    CCodecBuffer* destBuffer = codec->CreateBuffer(4, 4, 1, 4, 4);

    CHECK(codec->Compress(*srcBuffer, *destBuffer) == CE_OK);

    CMP_BYTE* destBlock = destBuffer->GetData();

    // This codec uses a regular ETC2 block to store colours in conjunction with a EAC block to store alpha.

    // EAC block. Since we have full alpha, it is a very simple block with mostly zeroes
    CHECK(destBlock[0] == 0xFF);
    CHECK(destBlock[1] == 0x00);
    CHECK(destBlock[2] == 0x00);
    CHECK(destBlock[3] == 0x00);
    CHECK(destBlock[4] == 0x00);
    CHECK(destBlock[5] == 0x00);
    CHECK(destBlock[6] == 0x00);
    CHECK(destBlock[7] == 0x00);

    // We expect this regular ETC2 block to be equal to the ETC2_RGB codec values
    CHECK(destBlock[8] == 0x7F);
    CHECK(destBlock[9] == 0x7E);
    CHECK(destBlock[10] == 0x04);
    CHECK(destBlock[11] == 0x7F);
    CHECK(destBlock[12] == 0xFE);
    CHECK(destBlock[13] == 0x07);
    CHECK(destBlock[14] == 0xFF);
    CHECK(destBlock[15] == 0xC0);

    delete srcBuffer;
    delete destBuffer;
    delete codec;
}
TEST_CASE("ETC2_RGBA Codec Decoding", "[CODEC][ETC2]")
{
    CCodec_ETC2_RGBA* codec = new CCodec_ETC2_RGBA(CT_ETC2_RGBA);

    CMP_BYTE srcData[] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7E, 0x04, 0x7F, 0xFE, 0x07, 0xFF, 0xC0};

    CodecBufferType destBufferType = GetCodecBufferType(CMP_FORMAT_RGBA_8888);

    CCodecBuffer* srcBuffer  = codec->CreateBuffer(4, 4, 1, 4, 4, 0, srcData, sizeof(srcData));
    CCodecBuffer* destBuffer = CreateCodecBuffer(destBufferType, 4, 4, 1, 4, 4);

    CHECK(codec->Decompress(*srcBuffer, *destBuffer) == CE_OK);

    CMP_BYTE* destBlock = destBuffer->GetData();

    CHECK(destBlock[0] == 0xFF);
    CHECK(destBlock[1] == 0xFF);
    CHECK(destBlock[2] == 0x00);
    CHECK(destBlock[3] == 0xFF);

    CHECK(destBlock[4] == 0xFF);
    CHECK(destBlock[5] == 0xFF);
    CHECK(destBlock[6] == 0x00);
    CHECK(destBlock[7] == 0xFF);

    CHECK(destBlock[8] == 0xFF);
    CHECK(destBlock[9] == 0xFF);
    CHECK(destBlock[10] == 0x00);
    CHECK(destBlock[11] == 0xFF);

    CHECK(destBlock[12] == 0xFF);
    CHECK(destBlock[13] == 0xFF);
    CHECK(destBlock[14] == 0x00);
    CHECK(destBlock[15] == 0xFF);

    CHECK(destBlock[16] == 0xFF);
    CHECK(destBlock[17] == 0xFF);
    CHECK(destBlock[18] == 0x00);
    CHECK(destBlock[19] == 0xFF);

    CHECK(destBlock[20] == 0xFF);
    CHECK(destBlock[21] == 0xFF);
    CHECK(destBlock[22] == 0x00);
    CHECK(destBlock[23] == 0xFF);

    CHECK(destBlock[24] == 0xFF);
    CHECK(destBlock[25] == 0xFF);
    CHECK(destBlock[26] == 0x00);
    CHECK(destBlock[27] == 0xFF);

    CHECK(destBlock[28] == 0xFF);
    CHECK(destBlock[29] == 0xFF);
    CHECK(destBlock[30] == 0x00);
    CHECK(destBlock[31] == 0xFF);

    CHECK(destBlock[32] == 0xFF);
    CHECK(destBlock[33] == 0xFF);
    CHECK(destBlock[34] == 0x00);
    CHECK(destBlock[35] == 0xFF);

    CHECK(destBlock[36] == 0xFF);
    CHECK(destBlock[37] == 0xFF);
    CHECK(destBlock[38] == 0x00);
    CHECK(destBlock[39] == 0xFF);

    CHECK(destBlock[40] == 0xFF);
    CHECK(destBlock[41] == 0xFF);
    CHECK(destBlock[42] == 0x00);
    CHECK(destBlock[43] == 0xFF);

    CHECK(destBlock[44] == 0xFF);
    CHECK(destBlock[45] == 0xFF);
    CHECK(destBlock[46] == 0x00);
    CHECK(destBlock[47] == 0xFF);

    CHECK(destBlock[48] == 0xFF);
    CHECK(destBlock[49] == 0xFF);
    CHECK(destBlock[50] == 0x00);
    CHECK(destBlock[51] == 0xFF);

    CHECK(destBlock[52] == 0xFF);
    CHECK(destBlock[53] == 0xFF);
    CHECK(destBlock[54] == 0x00);
    CHECK(destBlock[55] == 0xFF);

    CHECK(destBlock[56] == 0xFF);
    CHECK(destBlock[57] == 0xFF);
    CHECK(destBlock[58] == 0x00);
    CHECK(destBlock[59] == 0xFF);

    CHECK(destBlock[60] == 0xFF);
    CHECK(destBlock[61] == 0xFF);
    CHECK(destBlock[62] == 0x00);
    CHECK(destBlock[63] == 0xFF);

    delete srcBuffer;
    delete destBuffer;
    delete codec;
}