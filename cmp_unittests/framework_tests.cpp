//=====================================================================
// Copyright (c) 2023-2024, Advanced Micro Devices, Inc. All rights reserved.
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

#include <string>

#include "compressonator.h"

#include "test_constants.h"

TEST_CASE("Load_Texture_16_Bit", "[FRAMEWORK]")
{
    CMP_MipSet texture = {};

    std::string texturePath = TEST_DATA_PATH + std::string("/16bit_image.png");

    CMP_ERROR error = CMP_LoadTexture(texturePath.c_str(), &texture);

    REQUIRE(error == CMP_OK);

    REQUIRE(texture.m_nWidth == 64);
    REQUIRE(texture.m_nHeight == 64);
    REQUIRE(texture.m_nDepth == 1);
    REQUIRE(texture.m_nMipLevels == 1);
    REQUIRE(texture.m_format == CMP_FORMAT_RGBA_8888);
    REQUIRE(texture.dwDataSize > 0);
    REQUIRE(texture.m_TextureType == TT_2D);
    REQUIRE(texture.m_TextureDataType == TDT_ARGB);

    // NOTE: We automatically convert loaded 16-bit textures to 8-bit. This is done within the stb image loading code
    REQUIRE(texture.m_ChannelFormat == CF_8bit);

    CMP_FreeMipSet(&texture);
}