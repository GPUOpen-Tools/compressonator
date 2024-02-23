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

#include <string>

#include "single_include/catch2/catch.hpp"

#include "compressonator.h"
#include "common.h"
#include "test_constants.h"

TEST_CASE("Square Texture", "[MIPMAP]")
{
    const std::string inputTexturePath = TEST_DATA_PATH + std::string("/mipmap_128x128.png");

    CMP_ERROR  error;
    CMP_MipSet texture = {};

    error = CMP_LoadTexture(inputTexturePath.c_str(), &texture);
    REQUIRE(error == CMP_OK);

    CHECK(texture.m_nWidth == 128);
    CHECK(texture.m_nHeight == 128);
    CHECK(texture.m_nMaxMipLevels == 8);
    CHECK(texture.m_nMipLevels == 1);

    SECTION("Minimum Size 1")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 1);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 8);
    }

    SECTION("Minimum Size 4")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 4);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 6);
    }

    SECTION("Minimum Size 5")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 5);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 6);
    }

    SECTION("Minimum Size 64")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 64);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 2);
    }

    SECTION("Minimum Size 70")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 70);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 2);
    }

    SECTION("Minimum Size 128")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 128);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 1);
    }
}

TEST_CASE("Tall_Texture", "[MIPMAP]")
{
    const std::string inputTexturePath = TEST_DATA_PATH + std::string("/mipmap_128x512.png");

    CMP_ERROR  error;
    CMP_MipSet texture = {};

    error = CMP_LoadTexture(inputTexturePath.c_str(), &texture);
    REQUIRE(error == CMP_OK);

    CHECK(texture.m_nWidth == 128);
    CHECK(texture.m_nHeight == 512);
    CHECK(texture.m_nMaxMipLevels == 10);
    CHECK(texture.m_nMipLevels == 1);

    SECTION("Minimum Size 1")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 1);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 10);
    }

    SECTION("Minimum Size 128")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 128);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 3);
    }

    SECTION("Minimum Size 256")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 256);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 2);
    }
    SECTION("Minimum Size 300")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 300);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 2);
    }

    SECTION("Minimum Size 512")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 512);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 1);
    }
}

TEST_CASE("Wide_Texture", "[MIPMAP]")
{
    const std::string inputTexturePath = TEST_DATA_PATH + std::string("/mipmap_512x128.png");

    CMP_ERROR  error;
    CMP_MipSet texture = {};

    error = CMP_LoadTexture(inputTexturePath.c_str(), &texture);
    REQUIRE(error == CMP_OK);

    CHECK(texture.m_nWidth == 512);
    CHECK(texture.m_nHeight == 128);
    CHECK(texture.m_nMaxMipLevels == 10);
    CHECK(texture.m_nMipLevels == 1);

    SECTION("Minimum Size 1")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 1);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 10);
    }

    SECTION("Minimum Size 128")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 128);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 3);
    }

    SECTION("Minimum Size 256")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 256);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 2);
    }
    SECTION("Minimum Size 300")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 300);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 2);
    }

    SECTION("Minimum Size 512")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 512);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 1);
    }
}

TEST_CASE("Non_Regular_Texture", "[MIPMAP]")
{
    const std::string inputTexturePath = TEST_DATA_PATH + std::string("/ruby.bmp");

    CMP_ERROR  error;
    CMP_MipSet texture = {};

    error = CMP_LoadTexture(inputTexturePath.c_str(), &texture);
    REQUIRE(error == CMP_OK);

    CHECK(texture.m_nWidth == 576);
    CHECK(texture.m_nHeight == 416);
    CHECK(texture.m_nMaxMipLevels == 10);
    CHECK(texture.m_nMipLevels == 1);

    SECTION("Minimum Size 1")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 1);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 10);
    }

    SECTION("Minimum Size 416")
    {
        error = (CMP_ERROR)CMP_GenerateMIPLevels(&texture, 416);
        REQUIRE(error == CMP_OK);

        CHECK(texture.m_nMipLevels == 2);
    }
}

TEST_CASE("Get_Max_Mipmap_Levels", "[MIPMAP]")
{
    // Method 1

    CHECK(CMP_CalcMaxMipLevel(128, 128, false) == 8);
    CHECK(CMP_CalcMaxMipLevel(130, 130, false) == 8);

    CHECK(CMP_CalcMaxMipLevel(128, 512, false) == 10);
    CHECK(CMP_CalcMaxMipLevel(512, 128, false) == 10);

    // Method 2

    CMP_CMIPS cmips = {};

    CHECK(cmips.GetMaxMipLevels(128, 128, 1) == 8);
    CHECK(cmips.GetMaxMipLevels(130, 130, 1) == 8);

    CHECK(cmips.GetMaxMipLevels(128, 512, 1) == 10);
    CHECK(cmips.GetMaxMipLevels(512, 128, 1) == 10);
}

// NOTE: The CMP_CalcMinMipSize function will always return the width of the mipmap level after (N - 1) steps
// Also, the second argument in the function is the width
TEST_CASE("Get_Min_Mipmap_Size", "[MIPMAP]")
{
    // 128 x 128

    CHECK(CMP_CalcMinMipSize(128, 128, 1) == 128);
    CHECK(CMP_CalcMinMipSize(128, 128, 2) == 64);
    CHECK(CMP_CalcMinMipSize(128, 128, 3) == 32);
    CHECK(CMP_CalcMinMipSize(128, 128, 4) == 16);
    CHECK(CMP_CalcMinMipSize(128, 128, 5) == 8);
    CHECK(CMP_CalcMinMipSize(128, 128, 6) == 4);
    CHECK(CMP_CalcMinMipSize(128, 128, 7) == 2);
    CHECK(CMP_CalcMinMipSize(128, 128, 8) == 1);

    CHECK(CMP_CalcMinMipSize(128, 128, 20) == 1);

    // 512 x 128

    CHECK(CMP_CalcMinMipSize(512, 128, 1) == 128);
    CHECK(CMP_CalcMinMipSize(512, 128, 2) == 64);
    CHECK(CMP_CalcMinMipSize(512, 128, 3) == 32);
    CHECK(CMP_CalcMinMipSize(512, 128, 4) == 16);
    CHECK(CMP_CalcMinMipSize(512, 128, 5) == 8);
    CHECK(CMP_CalcMinMipSize(512, 128, 6) == 4);
    CHECK(CMP_CalcMinMipSize(512, 128, 7) == 2);
    CHECK(CMP_CalcMinMipSize(512, 128, 8) == 1);

    CHECK(CMP_CalcMinMipSize(512, 128, 20) == 1);

    // 128 x 512

    CHECK(CMP_CalcMinMipSize(128, 512, 1) == 512);
    CHECK(CMP_CalcMinMipSize(128, 512, 2) == 256);
    CHECK(CMP_CalcMinMipSize(128, 512, 3) == 128);
    CHECK(CMP_CalcMinMipSize(128, 512, 4) == 64);
    CHECK(CMP_CalcMinMipSize(128, 512, 5) == 32);
    CHECK(CMP_CalcMinMipSize(128, 512, 6) == 16);
    CHECK(CMP_CalcMinMipSize(128, 512, 7) == 8);
    CHECK(CMP_CalcMinMipSize(128, 512, 8) == 4);
    CHECK(CMP_CalcMinMipSize(128, 512, 9) == 2);
    CHECK(CMP_CalcMinMipSize(128, 512, 10) == 1);

    CHECK(CMP_CalcMinMipSize(128, 512, 20) == 1);
}

TEST_CASE("Filter Gamma", "[MIPMAP]")
{
    const std::string inputTexturePath = TEST_DATA_PATH + std::string("/mipmap_128x128.png");

    CMP_ERROR         error;
    CMP_MipSet        texture      = {};
    CMP_MipSet        texture2     = {};
    CMP_CFilterParams CFilterParam = {};

    error = CMP_LoadTexture(inputTexturePath.c_str(), &texture);
    REQUIRE(error == CMP_OK);

    CHECK(texture.m_nWidth == 128);
    CHECK(texture.m_nHeight == 128);
    CHECK(texture.m_nMaxMipLevels == 8);
    CHECK(texture.m_nMipLevels == 1);

    CFilterParam.dwMipFilterOptions = 0;
    CFilterParam.nFilterType        = 0;
    CFilterParam.nMinSize           = 1;
    CFilterParam.fGammaCorrection   = 1;

    error = (CMP_ERROR)CMP_GenerateMIPLevelsEx(&texture, &CFilterParam);
    REQUIRE(error == CMP_OK);

    CMP_MipLevel* imageData = 0;
    CMP_GetMipLevel(&imageData, &texture, 0, 0);
    REQUIRE(imageData != 0);

    CMP_INT colorScore = imageData->m_pcData->rgba[0] + imageData->m_pcData->rgba[1] + imageData->m_pcData->rgba[2];

    SECTION("Filter Gamma 2.0")
    {
        CFilterParam.fGammaCorrection = 2;

        error = (CMP_ERROR)CMP_GenerateMIPLevelsEx(&texture, &CFilterParam);
        REQUIRE(error == CMP_OK);

        CMP_MipLevel* adjustedData = 0;
        CMP_GetMipLevel(&adjustedData, &texture, 0, 0);
        REQUIRE(adjustedData != 0);

        CMP_INT adjustedScore = adjustedData->m_pcData->rgba[0] + adjustedData->m_pcData->rgba[1] + adjustedData->m_pcData->rgba[2];

        // Image should be darker, meaning the gamma-adjusted rgb sum should be less
        CHECK(adjustedScore < colorScore);
    }
}
