//=====================================================================
// Copyright 2020-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef BLOCKCONSTANTS_H
#define BLOCKCONSTANTS_H
#include <string>
#include <unordered_map>
struct Block
{
    const unsigned char* data;
    const unsigned char* color;
};
struct BlockBC6
{
    const unsigned char* data;
    const float*         color;
};

static const unsigned char BC1_Red_Ignore_Alpha[]{0x0, 0xf8, 0x0, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC1_Blue_Half_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_White_Half_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Black_Half_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Red_Blue_Half_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Red_Green_Half_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Green_Blue_Half_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Red_Full_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Green_Full_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Blue_Full_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_White_Full_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Green_Ignore_Alpha[]{0xe0, 0x7, 0xe0, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC1_Black_Full_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Red_Blue_Full_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Red_Green_Full_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Green_Blue_Full_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Blue_Ignore_Alpha[]{0x1f, 0x0, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC1_White_Ignore_Alpha[]{0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC1_Black_Ignore_Alpha[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC1_Red_Blue_Ignore_Alpha[]{0x1f, 0xf8, 0x1f, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC1_Red_Green_Ignore_Alpha[]{0xe0, 0xff, 0xe0, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC1_Green_Blue_Ignore_Alpha[]{0xff, 0x7, 0xff, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC1_Red_Half_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Green_Half_Alpha[]{0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC2_Red_Ignore_Alpha[]{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0xf8, 0x0, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Blue_Half_Alpha[]{0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x1f, 0x0, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_White_Half_Alpha[]{0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Black_Half_Alpha[]{0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Red_Blue_Half_Alpha[]{0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x1f, 0xf8, 0x1f, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Red_Green_Half_Alpha[]{0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0xe0, 0xff, 0xe0, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Green_Blue_Half_Alpha[]{0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0xff, 0x7, 0xff, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Red_Full_Alpha[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf8, 0x0, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Green_Full_Alpha[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0, 0x7, 0xe0, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Blue_Full_Alpha[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1f, 0x0, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_White_Full_Alpha[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Green_Ignore_Alpha[]{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x7, 0xe0, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Black_Full_Alpha[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Red_Blue_Full_Alpha[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1f, 0xf8, 0x1f, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Red_Green_Full_Alpha[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0, 0xff, 0xe0, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Green_Blue_Full_Alpha[]{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x7, 0xff, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Blue_Ignore_Alpha[]{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x0, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_White_Ignore_Alpha[]{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Black_Ignore_Alpha[]{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Red_Blue_Ignore_Alpha[]{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xf8, 0x1f, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Red_Green_Ignore_Alpha[]{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xe0, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Green_Blue_Ignore_Alpha[]{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7, 0xff, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Red_Half_Alpha[]{0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x0, 0xf8, 0x0, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC2_Green_Half_Alpha[]{0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0xe0, 0x7, 0xe0, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Red_Ignore_Alpha[]{0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf8, 0x0, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Blue_Half_Alpha[]{0x7b, 0x7b, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1f, 0x0, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_White_Half_Alpha[]{0x7b, 0x7b, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Black_Half_Alpha[]{0x7b, 0x7b, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Red_Blue_Half_Alpha[]{0x7b, 0x7b, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1f, 0xf8, 0x1f, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Red_Green_Half_Alpha[]{0x7b, 0x7b, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0, 0xff, 0xe0, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Green_Blue_Half_Alpha[]{0x7b, 0x7b, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x7, 0xff, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Red_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0x0, 0xf8, 0x0, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Green_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xe0, 0x7, 0xe0, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Blue_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0x1f, 0x0, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_White_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Green_Ignore_Alpha[]{0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0, 0x7, 0xe0, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Black_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Red_Blue_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0x1f, 0xf8, 0x1f, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Red_Green_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xe0, 0xff, 0xe0, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Green_Blue_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x7, 0xff, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Blue_Ignore_Alpha[]{0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1f, 0x0, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_White_Ignore_Alpha[]{0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Black_Ignore_Alpha[]{0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Red_Blue_Ignore_Alpha[]{0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1f, 0xf8, 0x1f, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Red_Green_Ignore_Alpha[]{0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0, 0xff, 0xe0, 0xff, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Green_Blue_Ignore_Alpha[]{0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x7, 0xff, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Red_Half_Alpha[]{0x7b, 0x7b, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf8, 0x0, 0xf8, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC3_Green_Half_Alpha[]{0x7b, 0x7b, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0, 0x7, 0xe0, 0x7, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Red_Ignore_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Blue_Half_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_White_Half_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Black_Half_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_Red_Blue_Half_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Red_Green_Half_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Green_Blue_Half_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_Red_Full_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Green_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_Blue_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_White_Full_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Green_Ignore_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_Black_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_Red_Blue_Full_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Red_Green_Full_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Green_Blue_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_Blue_Ignore_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_White_Ignore_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Black_Ignore_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_Red_Blue_Ignore_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Red_Green_Ignore_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Green_Blue_Ignore_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC4_Red_Half_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC4_Green_Half_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_Red_Ignore_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_Blue_Half_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_White_Half_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Black_Half_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_Red_Blue_Half_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_Red_Green_Half_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Green_Blue_Half_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Red_Full_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_Green_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Blue_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_White_Full_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Green_Ignore_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Black_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_Red_Blue_Full_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_Red_Green_Full_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Green_Blue_Full_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Blue_Ignore_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_White_Ignore_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Black_Ignore_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_Red_Blue_Ignore_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_Red_Green_Ignore_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Green_Blue_Ignore_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC5_Red_Half_Alpha[]{0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};
static const unsigned char BC5_Green_Half_Alpha[]{0xff, 0x0, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Red_Ignore_Alpha[]{0xe3, 0x3d, 0x0, 0x0, 0x78, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Blue_Half_Alpha[]{0x3, 0x0, 0x0, 0xde, 0x3, 0x0, 0x80, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_White_Half_Alpha[]{0xe3, 0xbd, 0xf7, 0xde, 0x7b, 0xef, 0xbd, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Black_Half_Alpha[]{0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Red_Blue_Half_Alpha[]{0xe3, 0x3d, 0x0, 0xde, 0x7b, 0xf, 0x80, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Red_Green_Half_Alpha[]{0xe3, 0xbd, 0xf7, 0x0, 0x78, 0xef, 0x3d, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Green_Blue_Half_Alpha[]{0x3, 0x80, 0xf7, 0xde, 0x3, 0xe0, 0xbd, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Red_Full_Alpha[]{0xe3, 0x3d, 0x0, 0x0, 0x78, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Green_Full_Alpha[]{0x3, 0x80, 0xf7, 0x0, 0x0, 0xe0, 0x3d, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Blue_Full_Alpha[]{0x3, 0x0, 0x0, 0xde, 0x3, 0x0, 0x80, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_White_Full_Alpha[]{0xe3, 0xbd, 0xf7, 0xde, 0x7b, 0xef, 0xbd, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Green_Ignore_Alpha[]{0x3, 0x80, 0xf7, 0x0, 0x0, 0xe0, 0x3d, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Black_Full_Alpha[]{0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Red_Blue_Full_Alpha[]{0xe3, 0x3d, 0x0, 0xde, 0x7b, 0xf, 0x80, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Red_Green_Full_Alpha[]{0xe3, 0xbd, 0xf7, 0x0, 0x78, 0xef, 0x3d, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Green_Blue_Full_Alpha[]{0x3, 0x80, 0xf7, 0xde, 0x3, 0xe0, 0xbd, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Blue_Ignore_Alpha[]{0x3, 0x0, 0x0, 0xde, 0x3, 0x0, 0x80, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_White_Ignore_Alpha[]{0xe3, 0xbd, 0xf7, 0xde, 0x7b, 0xef, 0xbd, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Black_Ignore_Alpha[]{0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Red_Blue_Ignore_Alpha[]{0xe3, 0x3d, 0x0, 0xde, 0x7b, 0xf, 0x80, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Red_Green_Ignore_Alpha[]{0xe3, 0xbd, 0xf7, 0x0, 0x78, 0xef, 0x3d, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Green_Blue_Ignore_Alpha[]{0x3, 0x80, 0xf7, 0xde, 0x3, 0xe0, 0xbd, 0xf7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Red_Half_Alpha[]{0xe3, 0x3d, 0x0, 0x0, 0x78, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC6_Green_Half_Alpha[]{0x3, 0x80, 0xf7, 0x0, 0x0, 0xe0, 0x3d, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Red_Ignore_Alpha[]{0x10, 0xff, 0x3, 0x0, 0xc0, 0xff, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Blue_Half_Alpha[]{0x20, 0x0, 0x0, 0x0, 0xf0, 0xff, 0xef, 0xed, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_White_Half_Alpha[]{0x20, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xed, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Black_Half_Alpha[]{0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0xec, 0xed, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Red_Blue_Half_Alpha[]{0x20, 0xff, 0x3f, 0x0, 0xf0, 0xff, 0xef, 0xed, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Red_Green_Half_Alpha[]{0x20, 0xff, 0xff, 0xff, 0xf, 0x0, 0xec, 0xed, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Green_Blue_Half_Alpha[]{0x20, 0x0, 0xc0, 0xff, 0xff, 0xff, 0xef, 0xed, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Red_Full_Alpha[]{0x10, 0xff, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Green_Full_Alpha[]{0x10, 0x0, 0xfc, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Blue_Full_Alpha[]{0x10, 0x0, 0x0, 0xf0, 0x3f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_White_Full_Alpha[]{0x10, 0xff, 0xff, 0xff, 0x3f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Green_Ignore_Alpha[]{0x10, 0x0, 0xfc, 0xf, 0xc0, 0xff, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Black_Full_Alpha[]{0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Red_Blue_Full_Alpha[]{0x10, 0xff, 0x3, 0xf0, 0x3f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Red_Green_Full_Alpha[]{0x10, 0xff, 0xff, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Green_Blue_Full_Alpha[]{0x10, 0x0, 0xfc, 0xff, 0x3f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Blue_Ignore_Alpha[]{0x10, 0x0, 0x0, 0xf0, 0xff, 0xff, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_White_Ignore_Alpha[]{0x10, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Black_Ignore_Alpha[]{0x10, 0x0, 0x0, 0x0, 0xc0, 0xff, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Red_Blue_Ignore_Alpha[]{0x10, 0xff, 0x3, 0xf0, 0xff, 0xff, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Red_Green_Ignore_Alpha[]{0x10, 0xff, 0xff, 0xf, 0xc0, 0xff, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Green_Blue_Ignore_Alpha[]{0x10, 0x0, 0xfc, 0xff, 0xff, 0xff, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Red_Half_Alpha[]{0x20, 0xff, 0x3f, 0x0, 0x0, 0x0, 0xec, 0xed, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static const unsigned char BC7_Green_Half_Alpha[]{0x20, 0x0, 0xc0, 0xff, 0xf, 0x0, 0xec, 0xed, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

Block    BC1_Red_Ignore_Alpha_Block        = {BC1_Red_Ignore_Alpha, nullptr};
Block    BC1_Blue_Half_Alpha_Block         = {BC1_Blue_Half_Alpha, nullptr};
Block    BC1_White_Half_Alpha_Block        = {BC1_White_Half_Alpha, nullptr};
Block    BC1_Black_Half_Alpha_Block        = {BC1_Black_Half_Alpha, nullptr};
Block    BC1_Red_Blue_Half_Alpha_Block     = {BC1_Red_Blue_Half_Alpha, nullptr};
Block    BC1_Red_Green_Half_Alpha_Block    = {BC1_Red_Green_Half_Alpha, nullptr};
Block    BC1_Green_Blue_Half_Alpha_Block   = {BC1_Green_Blue_Half_Alpha, nullptr};
Block    BC1_Red_Full_Alpha_Block          = {BC1_Red_Full_Alpha, nullptr};
Block    BC1_Green_Full_Alpha_Block        = {BC1_Green_Full_Alpha, nullptr};
Block    BC1_Blue_Full_Alpha_Block         = {BC1_Blue_Full_Alpha, nullptr};
Block    BC1_White_Full_Alpha_Block        = {BC1_White_Full_Alpha, nullptr};
Block    BC1_Green_Ignore_Alpha_Block      = {BC1_Green_Ignore_Alpha, nullptr};
Block    BC1_Black_Full_Alpha_Block        = {BC1_Black_Full_Alpha, nullptr};
Block    BC1_Red_Blue_Full_Alpha_Block     = {BC1_Red_Blue_Full_Alpha, nullptr};
Block    BC1_Red_Green_Full_Alpha_Block    = {BC1_Red_Green_Full_Alpha, nullptr};
Block    BC1_Green_Blue_Full_Alpha_Block   = {BC1_Green_Blue_Full_Alpha, nullptr};
Block    BC1_Blue_Ignore_Alpha_Block       = {BC1_Blue_Ignore_Alpha, nullptr};
Block    BC1_White_Ignore_Alpha_Block      = {BC1_White_Ignore_Alpha, nullptr};
Block    BC1_Black_Ignore_Alpha_Block      = {BC1_Black_Ignore_Alpha, nullptr};
Block    BC1_Red_Blue_Ignore_Alpha_Block   = {BC1_Red_Blue_Ignore_Alpha, nullptr};
Block    BC1_Red_Green_Ignore_Alpha_Block  = {BC1_Red_Green_Ignore_Alpha, nullptr};
Block    BC1_Green_Blue_Ignore_Alpha_Block = {BC1_Green_Blue_Ignore_Alpha, nullptr};
Block    BC1_Red_Half_Alpha_Block          = {BC1_Red_Half_Alpha, nullptr};
Block    BC1_Green_Half_Alpha_Block        = {BC1_Green_Half_Alpha, nullptr};
Block    BC2_Red_Ignore_Alpha_Block        = {BC2_Red_Ignore_Alpha, nullptr};
Block    BC2_Blue_Half_Alpha_Block         = {BC2_Blue_Half_Alpha, nullptr};
Block    BC2_White_Half_Alpha_Block        = {BC2_White_Half_Alpha, nullptr};
Block    BC2_Black_Half_Alpha_Block        = {BC2_Black_Half_Alpha, nullptr};
Block    BC2_Red_Blue_Half_Alpha_Block     = {BC2_Red_Blue_Half_Alpha, nullptr};
Block    BC2_Red_Green_Half_Alpha_Block    = {BC2_Red_Green_Half_Alpha, nullptr};
Block    BC2_Green_Blue_Half_Alpha_Block   = {BC2_Green_Blue_Half_Alpha, nullptr};
Block    BC2_Red_Full_Alpha_Block          = {BC2_Red_Full_Alpha, nullptr};
Block    BC2_Green_Full_Alpha_Block        = {BC2_Green_Full_Alpha, nullptr};
Block    BC2_Blue_Full_Alpha_Block         = {BC2_Blue_Full_Alpha, nullptr};
Block    BC2_White_Full_Alpha_Block        = {BC2_White_Full_Alpha, nullptr};
Block    BC2_Green_Ignore_Alpha_Block      = {BC2_Green_Ignore_Alpha, nullptr};
Block    BC2_Black_Full_Alpha_Block        = {BC2_Black_Full_Alpha, nullptr};
Block    BC2_Red_Blue_Full_Alpha_Block     = {BC2_Red_Blue_Full_Alpha, nullptr};
Block    BC2_Red_Green_Full_Alpha_Block    = {BC2_Red_Green_Full_Alpha, nullptr};
Block    BC2_Green_Blue_Full_Alpha_Block   = {BC2_Green_Blue_Full_Alpha, nullptr};
Block    BC2_Blue_Ignore_Alpha_Block       = {BC2_Blue_Ignore_Alpha, nullptr};
Block    BC2_White_Ignore_Alpha_Block      = {BC2_White_Ignore_Alpha, nullptr};
Block    BC2_Black_Ignore_Alpha_Block      = {BC2_Black_Ignore_Alpha, nullptr};
Block    BC2_Red_Blue_Ignore_Alpha_Block   = {BC2_Red_Blue_Ignore_Alpha, nullptr};
Block    BC2_Red_Green_Ignore_Alpha_Block  = {BC2_Red_Green_Ignore_Alpha, nullptr};
Block    BC2_Green_Blue_Ignore_Alpha_Block = {BC2_Green_Blue_Ignore_Alpha, nullptr};
Block    BC2_Red_Half_Alpha_Block          = {BC2_Red_Half_Alpha, nullptr};
Block    BC2_Green_Half_Alpha_Block        = {BC2_Green_Half_Alpha, nullptr};
Block    BC3_Red_Ignore_Alpha_Block        = {BC3_Red_Ignore_Alpha, nullptr};
Block    BC3_Blue_Half_Alpha_Block         = {BC3_Blue_Half_Alpha, nullptr};
Block    BC3_White_Half_Alpha_Block        = {BC3_White_Half_Alpha, nullptr};
Block    BC3_Black_Half_Alpha_Block        = {BC3_Black_Half_Alpha, nullptr};
Block    BC3_Red_Blue_Half_Alpha_Block     = {BC3_Red_Blue_Half_Alpha, nullptr};
Block    BC3_Red_Green_Half_Alpha_Block    = {BC3_Red_Green_Half_Alpha, nullptr};
Block    BC3_Green_Blue_Half_Alpha_Block   = {BC3_Green_Blue_Half_Alpha, nullptr};
Block    BC3_Red_Full_Alpha_Block          = {BC3_Red_Full_Alpha, nullptr};
Block    BC3_Green_Full_Alpha_Block        = {BC3_Green_Full_Alpha, nullptr};
Block    BC3_Blue_Full_Alpha_Block         = {BC3_Blue_Full_Alpha, nullptr};
Block    BC3_White_Full_Alpha_Block        = {BC3_White_Full_Alpha, nullptr};
Block    BC3_Green_Ignore_Alpha_Block      = {BC3_Green_Ignore_Alpha, nullptr};
Block    BC3_Black_Full_Alpha_Block        = {BC3_Black_Full_Alpha, nullptr};
Block    BC3_Red_Blue_Full_Alpha_Block     = {BC3_Red_Blue_Full_Alpha, nullptr};
Block    BC3_Red_Green_Full_Alpha_Block    = {BC3_Red_Green_Full_Alpha, nullptr};
Block    BC3_Green_Blue_Full_Alpha_Block   = {BC3_Green_Blue_Full_Alpha, nullptr};
Block    BC3_Blue_Ignore_Alpha_Block       = {BC3_Blue_Ignore_Alpha, nullptr};
Block    BC3_White_Ignore_Alpha_Block      = {BC3_White_Ignore_Alpha, nullptr};
Block    BC3_Black_Ignore_Alpha_Block      = {BC3_Black_Ignore_Alpha, nullptr};
Block    BC3_Red_Blue_Ignore_Alpha_Block   = {BC3_Red_Blue_Ignore_Alpha, nullptr};
Block    BC3_Red_Green_Ignore_Alpha_Block  = {BC3_Red_Green_Ignore_Alpha, nullptr};
Block    BC3_Green_Blue_Ignore_Alpha_Block = {BC3_Green_Blue_Ignore_Alpha, nullptr};
Block    BC3_Red_Half_Alpha_Block          = {BC3_Red_Half_Alpha, nullptr};
Block    BC3_Green_Half_Alpha_Block        = {BC3_Green_Half_Alpha, nullptr};
Block    BC4_Red_Ignore_Alpha_Block        = {BC4_Red_Ignore_Alpha, nullptr};
Block    BC4_Blue_Half_Alpha_Block         = {BC4_Blue_Half_Alpha, nullptr};
Block    BC4_White_Half_Alpha_Block        = {BC4_White_Half_Alpha, nullptr};
Block    BC4_Black_Half_Alpha_Block        = {BC4_Black_Half_Alpha, nullptr};
Block    BC4_Red_Blue_Half_Alpha_Block     = {BC4_Red_Blue_Half_Alpha, nullptr};
Block    BC4_Red_Green_Half_Alpha_Block    = {BC4_Red_Green_Half_Alpha, nullptr};
Block    BC4_Green_Blue_Half_Alpha_Block   = {BC4_Green_Blue_Half_Alpha, nullptr};
Block    BC4_Red_Full_Alpha_Block          = {BC4_Red_Full_Alpha, nullptr};
Block    BC4_Green_Full_Alpha_Block        = {BC4_Green_Full_Alpha, nullptr};
Block    BC4_Blue_Full_Alpha_Block         = {BC4_Blue_Full_Alpha, nullptr};
Block    BC4_White_Full_Alpha_Block        = {BC4_White_Full_Alpha, nullptr};
Block    BC4_Green_Ignore_Alpha_Block      = {BC4_Green_Ignore_Alpha, nullptr};
Block    BC4_Black_Full_Alpha_Block        = {BC4_Black_Full_Alpha, nullptr};
Block    BC4_Red_Blue_Full_Alpha_Block     = {BC4_Red_Blue_Full_Alpha, nullptr};
Block    BC4_Red_Green_Full_Alpha_Block    = {BC4_Red_Green_Full_Alpha, nullptr};
Block    BC4_Green_Blue_Full_Alpha_Block   = {BC4_Green_Blue_Full_Alpha, nullptr};
Block    BC4_Blue_Ignore_Alpha_Block       = {BC4_Blue_Ignore_Alpha, nullptr};
Block    BC4_White_Ignore_Alpha_Block      = {BC4_White_Ignore_Alpha, nullptr};
Block    BC4_Black_Ignore_Alpha_Block      = {BC4_Black_Ignore_Alpha, nullptr};
Block    BC4_Red_Blue_Ignore_Alpha_Block   = {BC4_Red_Blue_Ignore_Alpha, nullptr};
Block    BC4_Red_Green_Ignore_Alpha_Block  = {BC4_Red_Green_Ignore_Alpha, nullptr};
Block    BC4_Green_Blue_Ignore_Alpha_Block = {BC4_Green_Blue_Ignore_Alpha, nullptr};
Block    BC4_Red_Half_Alpha_Block          = {BC4_Red_Half_Alpha, nullptr};
Block    BC4_Green_Half_Alpha_Block        = {BC4_Green_Half_Alpha, nullptr};
Block    BC5_Red_Ignore_Alpha_Block        = {BC5_Red_Ignore_Alpha, nullptr};
Block    BC5_Blue_Half_Alpha_Block         = {BC5_Blue_Half_Alpha, nullptr};
Block    BC5_White_Half_Alpha_Block        = {BC5_White_Half_Alpha, nullptr};
Block    BC5_Black_Half_Alpha_Block        = {BC5_Black_Half_Alpha, nullptr};
Block    BC5_Red_Blue_Half_Alpha_Block     = {BC5_Red_Blue_Half_Alpha, nullptr};
Block    BC5_Red_Green_Half_Alpha_Block    = {BC5_Red_Green_Half_Alpha, nullptr};
Block    BC5_Green_Blue_Half_Alpha_Block   = {BC5_Green_Blue_Half_Alpha, nullptr};
Block    BC5_Red_Full_Alpha_Block          = {BC5_Red_Full_Alpha, nullptr};
Block    BC5_Green_Full_Alpha_Block        = {BC5_Green_Full_Alpha, nullptr};
Block    BC5_Blue_Full_Alpha_Block         = {BC5_Blue_Full_Alpha, nullptr};
Block    BC5_White_Full_Alpha_Block        = {BC5_White_Full_Alpha, nullptr};
Block    BC5_Green_Ignore_Alpha_Block      = {BC5_Green_Ignore_Alpha, nullptr};
Block    BC5_Black_Full_Alpha_Block        = {BC5_Black_Full_Alpha, nullptr};
Block    BC5_Red_Blue_Full_Alpha_Block     = {BC5_Red_Blue_Full_Alpha, nullptr};
Block    BC5_Red_Green_Full_Alpha_Block    = {BC5_Red_Green_Full_Alpha, nullptr};
Block    BC5_Green_Blue_Full_Alpha_Block   = {BC5_Green_Blue_Full_Alpha, nullptr};
Block    BC5_Blue_Ignore_Alpha_Block       = {BC5_Blue_Ignore_Alpha, nullptr};
Block    BC5_White_Ignore_Alpha_Block      = {BC5_White_Ignore_Alpha, nullptr};
Block    BC5_Black_Ignore_Alpha_Block      = {BC5_Black_Ignore_Alpha, nullptr};
Block    BC5_Red_Blue_Ignore_Alpha_Block   = {BC5_Red_Blue_Ignore_Alpha, nullptr};
Block    BC5_Red_Green_Ignore_Alpha_Block  = {BC5_Red_Green_Ignore_Alpha, nullptr};
Block    BC5_Green_Blue_Ignore_Alpha_Block = {BC5_Green_Blue_Ignore_Alpha, nullptr};
Block    BC5_Red_Half_Alpha_Block          = {BC5_Red_Half_Alpha, nullptr};
Block    BC5_Green_Half_Alpha_Block        = {BC5_Green_Half_Alpha, nullptr};
BlockBC6 BC6_Red_Ignore_Alpha_Block        = {BC6_Red_Ignore_Alpha, nullptr};
BlockBC6 BC6_Blue_Half_Alpha_Block         = {BC6_Blue_Half_Alpha, nullptr};
BlockBC6 BC6_White_Half_Alpha_Block        = {BC6_White_Half_Alpha, nullptr};
BlockBC6 BC6_Black_Half_Alpha_Block        = {BC6_Black_Half_Alpha, nullptr};
BlockBC6 BC6_Red_Blue_Half_Alpha_Block     = {BC6_Red_Blue_Half_Alpha, nullptr};
BlockBC6 BC6_Red_Green_Half_Alpha_Block    = {BC6_Red_Green_Half_Alpha, nullptr};
BlockBC6 BC6_Green_Blue_Half_Alpha_Block   = {BC6_Green_Blue_Half_Alpha, nullptr};
BlockBC6 BC6_Red_Full_Alpha_Block          = {BC6_Red_Full_Alpha, nullptr};
BlockBC6 BC6_Green_Full_Alpha_Block        = {BC6_Green_Full_Alpha, nullptr};
BlockBC6 BC6_Blue_Full_Alpha_Block         = {BC6_Blue_Full_Alpha, nullptr};
BlockBC6 BC6_White_Full_Alpha_Block        = {BC6_White_Full_Alpha, nullptr};
BlockBC6 BC6_Green_Ignore_Alpha_Block      = {BC6_Green_Ignore_Alpha, nullptr};
BlockBC6 BC6_Black_Full_Alpha_Block        = {BC6_Black_Full_Alpha, nullptr};
BlockBC6 BC6_Red_Blue_Full_Alpha_Block     = {BC6_Red_Blue_Full_Alpha, nullptr};
BlockBC6 BC6_Red_Green_Full_Alpha_Block    = {BC6_Red_Green_Full_Alpha, nullptr};
BlockBC6 BC6_Green_Blue_Full_Alpha_Block   = {BC6_Green_Blue_Full_Alpha, nullptr};
BlockBC6 BC6_Blue_Ignore_Alpha_Block       = {BC6_Blue_Ignore_Alpha, nullptr};
BlockBC6 BC6_White_Ignore_Alpha_Block      = {BC6_White_Ignore_Alpha, nullptr};
BlockBC6 BC6_Black_Ignore_Alpha_Block      = {BC6_Black_Ignore_Alpha, nullptr};
BlockBC6 BC6_Red_Blue_Ignore_Alpha_Block   = {BC6_Red_Blue_Ignore_Alpha, nullptr};
BlockBC6 BC6_Red_Green_Ignore_Alpha_Block  = {BC6_Red_Green_Ignore_Alpha, nullptr};
BlockBC6 BC6_Green_Blue_Ignore_Alpha_Block = {BC6_Green_Blue_Ignore_Alpha, nullptr};
BlockBC6 BC6_Red_Half_Alpha_Block          = {BC6_Red_Half_Alpha, nullptr};
BlockBC6 BC6_Green_Half_Alpha_Block        = {BC6_Green_Half_Alpha, nullptr};
Block    BC7_Red_Ignore_Alpha_Block        = {BC7_Red_Ignore_Alpha, nullptr};
Block    BC7_Blue_Half_Alpha_Block         = {BC7_Blue_Half_Alpha, nullptr};
Block    BC7_White_Half_Alpha_Block        = {BC7_White_Half_Alpha, nullptr};
Block    BC7_Black_Half_Alpha_Block        = {BC7_Black_Half_Alpha, nullptr};
Block    BC7_Red_Blue_Half_Alpha_Block     = {BC7_Red_Blue_Half_Alpha, nullptr};
Block    BC7_Red_Green_Half_Alpha_Block    = {BC7_Red_Green_Half_Alpha, nullptr};
Block    BC7_Green_Blue_Half_Alpha_Block   = {BC7_Green_Blue_Half_Alpha, nullptr};
Block    BC7_Red_Full_Alpha_Block          = {BC7_Red_Full_Alpha, nullptr};
Block    BC7_Green_Full_Alpha_Block        = {BC7_Green_Full_Alpha, nullptr};
Block    BC7_Blue_Full_Alpha_Block         = {BC7_Blue_Full_Alpha, nullptr};
Block    BC7_White_Full_Alpha_Block        = {BC7_White_Full_Alpha, nullptr};
Block    BC7_Green_Ignore_Alpha_Block      = {BC7_Green_Ignore_Alpha, nullptr};
Block    BC7_Black_Full_Alpha_Block        = {BC7_Black_Full_Alpha, nullptr};
Block    BC7_Red_Blue_Full_Alpha_Block     = {BC7_Red_Blue_Full_Alpha, nullptr};
Block    BC7_Red_Green_Full_Alpha_Block    = {BC7_Red_Green_Full_Alpha, nullptr};
Block    BC7_Green_Blue_Full_Alpha_Block   = {BC7_Green_Blue_Full_Alpha, nullptr};
Block    BC7_Blue_Ignore_Alpha_Block       = {BC7_Blue_Ignore_Alpha, nullptr};
Block    BC7_White_Ignore_Alpha_Block      = {BC7_White_Ignore_Alpha, nullptr};
Block    BC7_Black_Ignore_Alpha_Block      = {BC7_Black_Ignore_Alpha, nullptr};
Block    BC7_Red_Blue_Ignore_Alpha_Block   = {BC7_Red_Blue_Ignore_Alpha, nullptr};
Block    BC7_Red_Green_Ignore_Alpha_Block  = {BC7_Red_Green_Ignore_Alpha, nullptr};
Block    BC7_Green_Blue_Ignore_Alpha_Block = {BC7_Green_Blue_Ignore_Alpha, nullptr};
Block    BC7_Red_Half_Alpha_Block          = {BC7_Red_Half_Alpha, nullptr};
Block    BC7_Green_Half_Alpha_Block        = {BC7_Green_Half_Alpha, nullptr};

static std::unordered_map<std::string, Block> blocks{{"BC1_Red_Ignore_Alpha", BC1_Red_Ignore_Alpha_Block},
                                                     {"BC1_Blue_Half_Alpha", BC1_Blue_Half_Alpha_Block},
                                                     {"BC1_White_Half_Alpha", BC1_White_Half_Alpha_Block},
                                                     {"BC1_Black_Half_Alpha", BC1_Black_Half_Alpha_Block},
                                                     {"BC1_Red_Blue_Half_Alpha", BC1_Red_Blue_Half_Alpha_Block},
                                                     {"BC1_Red_Green_Half_Alpha", BC1_Red_Green_Half_Alpha_Block},
                                                     {"BC1_Green_Blue_Half_Alpha", BC1_Green_Blue_Half_Alpha_Block},
                                                     {"BC1_Red_Full_Alpha", BC1_Red_Full_Alpha_Block},
                                                     {"BC1_Green_Full_Alpha", BC1_Green_Full_Alpha_Block},
                                                     {"BC1_Blue_Full_Alpha", BC1_Blue_Full_Alpha_Block},
                                                     {"BC1_White_Full_Alpha", BC1_White_Full_Alpha_Block},
                                                     {"BC1_Green_Ignore_Alpha", BC1_Green_Ignore_Alpha_Block},
                                                     {"BC1_Black_Full_Alpha", BC1_Black_Full_Alpha_Block},
                                                     {"BC1_Red_Blue_Full_Alpha", BC1_Red_Blue_Full_Alpha_Block},
                                                     {"BC1_Red_Green_Full_Alpha", BC1_Red_Green_Full_Alpha_Block},
                                                     {"BC1_Green_Blue_Full_Alpha", BC1_Green_Blue_Full_Alpha_Block},
                                                     {"BC1_Blue_Ignore_Alpha", BC1_Blue_Ignore_Alpha_Block},
                                                     {"BC1_White_Ignore_Alpha", BC1_White_Ignore_Alpha_Block},
                                                     {"BC1_Black_Ignore_Alpha", BC1_Black_Ignore_Alpha_Block},
                                                     {"BC1_Red_Blue_Ignore_Alpha", BC1_Red_Blue_Ignore_Alpha_Block},
                                                     {"BC1_Red_Green_Ignore_Alpha", BC1_Red_Green_Ignore_Alpha_Block},
                                                     {"BC1_Green_Blue_Ignore_Alpha", BC1_Green_Blue_Ignore_Alpha_Block},
                                                     {"BC1_Red_Half_Alpha", BC1_Red_Half_Alpha_Block},
                                                     {"BC1_Green_Half_Alpha", BC1_Green_Half_Alpha_Block},
                                                     {"BC2_Red_Ignore_Alpha", BC2_Red_Ignore_Alpha_Block},
                                                     {"BC2_Blue_Half_Alpha", BC2_Blue_Half_Alpha_Block},
                                                     {"BC2_White_Half_Alpha", BC2_White_Half_Alpha_Block},
                                                     {"BC2_Black_Half_Alpha", BC2_Black_Half_Alpha_Block},
                                                     {"BC2_Red_Blue_Half_Alpha", BC2_Red_Blue_Half_Alpha_Block},
                                                     {"BC2_Red_Green_Half_Alpha", BC2_Red_Green_Half_Alpha_Block},
                                                     {"BC2_Green_Blue_Half_Alpha", BC2_Green_Blue_Half_Alpha_Block},
                                                     {"BC2_Red_Full_Alpha", BC2_Red_Full_Alpha_Block},
                                                     {"BC2_Green_Full_Alpha", BC2_Green_Full_Alpha_Block},
                                                     {"BC2_Blue_Full_Alpha", BC2_Blue_Full_Alpha_Block},
                                                     {"BC2_White_Full_Alpha", BC2_White_Full_Alpha_Block},
                                                     {"BC2_Green_Ignore_Alpha", BC2_Green_Ignore_Alpha_Block},
                                                     {"BC2_Black_Full_Alpha", BC2_Black_Full_Alpha_Block},
                                                     {"BC2_Red_Blue_Full_Alpha", BC2_Red_Blue_Full_Alpha_Block},
                                                     {"BC2_Red_Green_Full_Alpha", BC2_Red_Green_Full_Alpha_Block},
                                                     {"BC2_Green_Blue_Full_Alpha", BC2_Green_Blue_Full_Alpha_Block},
                                                     {"BC2_Blue_Ignore_Alpha", BC2_Blue_Ignore_Alpha_Block},
                                                     {"BC2_White_Ignore_Alpha", BC2_White_Ignore_Alpha_Block},
                                                     {"BC2_Black_Ignore_Alpha", BC2_Black_Ignore_Alpha_Block},
                                                     {"BC2_Red_Blue_Ignore_Alpha", BC2_Red_Blue_Ignore_Alpha_Block},
                                                     {"BC2_Red_Green_Ignore_Alpha", BC2_Red_Green_Ignore_Alpha_Block},
                                                     {"BC2_Green_Blue_Ignore_Alpha", BC2_Green_Blue_Ignore_Alpha_Block},
                                                     {"BC2_Red_Half_Alpha", BC2_Red_Half_Alpha_Block},
                                                     {"BC2_Green_Half_Alpha", BC2_Green_Half_Alpha_Block},
                                                     {"BC3_Red_Ignore_Alpha", BC3_Red_Ignore_Alpha_Block},
                                                     {"BC3_Blue_Half_Alpha", BC3_Blue_Half_Alpha_Block},
                                                     {"BC3_White_Half_Alpha", BC3_White_Half_Alpha_Block},
                                                     {"BC3_Black_Half_Alpha", BC3_Black_Half_Alpha_Block},
                                                     {"BC3_Red_Blue_Half_Alpha", BC3_Red_Blue_Half_Alpha_Block},
                                                     {"BC3_Red_Green_Half_Alpha", BC3_Red_Green_Half_Alpha_Block},
                                                     {"BC3_Green_Blue_Half_Alpha", BC3_Green_Blue_Half_Alpha_Block},
                                                     {"BC3_Red_Full_Alpha", BC3_Red_Full_Alpha_Block},
                                                     {"BC3_Green_Full_Alpha", BC3_Green_Full_Alpha_Block},
                                                     {"BC3_Blue_Full_Alpha", BC3_Blue_Full_Alpha_Block},
                                                     {"BC3_White_Full_Alpha", BC3_White_Full_Alpha_Block},
                                                     {"BC3_Green_Ignore_Alpha", BC3_Green_Ignore_Alpha_Block},
                                                     {"BC3_Black_Full_Alpha", BC3_Black_Full_Alpha_Block},
                                                     {"BC3_Red_Blue_Full_Alpha", BC3_Red_Blue_Full_Alpha_Block},
                                                     {"BC3_Red_Green_Full_Alpha", BC3_Red_Green_Full_Alpha_Block},
                                                     {"BC3_Green_Blue_Full_Alpha", BC3_Green_Blue_Full_Alpha_Block},
                                                     {"BC3_Blue_Ignore_Alpha", BC3_Blue_Ignore_Alpha_Block},
                                                     {"BC3_White_Ignore_Alpha", BC3_White_Ignore_Alpha_Block},
                                                     {"BC3_Black_Ignore_Alpha", BC3_Black_Ignore_Alpha_Block},
                                                     {"BC3_Red_Blue_Ignore_Alpha", BC3_Red_Blue_Ignore_Alpha_Block},
                                                     {"BC3_Red_Green_Ignore_Alpha", BC3_Red_Green_Ignore_Alpha_Block},
                                                     {"BC3_Green_Blue_Ignore_Alpha", BC3_Green_Blue_Ignore_Alpha_Block},
                                                     {"BC3_Red_Half_Alpha", BC3_Red_Half_Alpha_Block},
                                                     {"BC3_Green_Half_Alpha", BC3_Green_Half_Alpha_Block},
                                                     {"BC4_Red_Ignore_Alpha", BC4_Red_Ignore_Alpha_Block},
                                                     {"BC4_Blue_Half_Alpha", BC4_Blue_Half_Alpha_Block},
                                                     {"BC4_White_Half_Alpha", BC4_White_Half_Alpha_Block},
                                                     {"BC4_Black_Half_Alpha", BC4_Black_Half_Alpha_Block},
                                                     {"BC4_Red_Blue_Half_Alpha", BC4_Red_Blue_Half_Alpha_Block},
                                                     {"BC4_Red_Green_Half_Alpha", BC4_Red_Green_Half_Alpha_Block},
                                                     {"BC4_Green_Blue_Half_Alpha", BC4_Green_Blue_Half_Alpha_Block},
                                                     {"BC4_Red_Full_Alpha", BC4_Red_Full_Alpha_Block},
                                                     {"BC4_Green_Full_Alpha", BC4_Green_Full_Alpha_Block},
                                                     {"BC4_Blue_Full_Alpha", BC4_Blue_Full_Alpha_Block},
                                                     {"BC4_White_Full_Alpha", BC4_White_Full_Alpha_Block},
                                                     {"BC4_Green_Ignore_Alpha", BC4_Green_Ignore_Alpha_Block},
                                                     {"BC4_Black_Full_Alpha", BC4_Black_Full_Alpha_Block},
                                                     {"BC4_Red_Blue_Full_Alpha", BC4_Red_Blue_Full_Alpha_Block},
                                                     {"BC4_Red_Green_Full_Alpha", BC4_Red_Green_Full_Alpha_Block},
                                                     {"BC4_Green_Blue_Full_Alpha", BC4_Green_Blue_Full_Alpha_Block},
                                                     {"BC4_Blue_Ignore_Alpha", BC4_Blue_Ignore_Alpha_Block},
                                                     {"BC4_White_Ignore_Alpha", BC4_White_Ignore_Alpha_Block},
                                                     {"BC4_Black_Ignore_Alpha", BC4_Black_Ignore_Alpha_Block},
                                                     {"BC4_Red_Blue_Ignore_Alpha", BC4_Red_Blue_Ignore_Alpha_Block},
                                                     {"BC4_Red_Green_Ignore_Alpha", BC4_Red_Green_Ignore_Alpha_Block},
                                                     {"BC4_Green_Blue_Ignore_Alpha", BC4_Green_Blue_Ignore_Alpha_Block},
                                                     {"BC4_Red_Half_Alpha", BC4_Red_Half_Alpha_Block},
                                                     {"BC4_Green_Half_Alpha", BC4_Green_Half_Alpha_Block},
                                                     {"BC5_Red_Ignore_Alpha", BC5_Red_Ignore_Alpha_Block},
                                                     {"BC5_Blue_Half_Alpha", BC5_Blue_Half_Alpha_Block},
                                                     {"BC5_White_Half_Alpha", BC5_White_Half_Alpha_Block},
                                                     {"BC5_Black_Half_Alpha", BC5_Black_Half_Alpha_Block},
                                                     {"BC5_Red_Blue_Half_Alpha", BC5_Red_Blue_Half_Alpha_Block},
                                                     {"BC5_Red_Green_Half_Alpha", BC5_Red_Green_Half_Alpha_Block},
                                                     {"BC5_Green_Blue_Half_Alpha", BC5_Green_Blue_Half_Alpha_Block},
                                                     {"BC5_Red_Full_Alpha", BC5_Red_Full_Alpha_Block},
                                                     {"BC5_Green_Full_Alpha", BC5_Green_Full_Alpha_Block},
                                                     {"BC5_Blue_Full_Alpha", BC5_Blue_Full_Alpha_Block},
                                                     {"BC5_White_Full_Alpha", BC5_White_Full_Alpha_Block},
                                                     {"BC5_Green_Ignore_Alpha", BC5_Green_Ignore_Alpha_Block},
                                                     {"BC5_Black_Full_Alpha", BC5_Black_Full_Alpha_Block},
                                                     {"BC5_Red_Blue_Full_Alpha", BC5_Red_Blue_Full_Alpha_Block},
                                                     {"BC5_Red_Green_Full_Alpha", BC5_Red_Green_Full_Alpha_Block},
                                                     {"BC5_Green_Blue_Full_Alpha", BC5_Green_Blue_Full_Alpha_Block},
                                                     {"BC5_Blue_Ignore_Alpha", BC5_Blue_Ignore_Alpha_Block},
                                                     {"BC5_White_Ignore_Alpha", BC5_White_Ignore_Alpha_Block},
                                                     {"BC5_Black_Ignore_Alpha", BC5_Black_Ignore_Alpha_Block},
                                                     {"BC5_Red_Blue_Ignore_Alpha", BC5_Red_Blue_Ignore_Alpha_Block},
                                                     {"BC5_Red_Green_Ignore_Alpha", BC5_Red_Green_Ignore_Alpha_Block},
                                                     {"BC5_Green_Blue_Ignore_Alpha", BC5_Green_Blue_Ignore_Alpha_Block},
                                                     {"BC5_Red_Half_Alpha", BC5_Red_Half_Alpha_Block},
                                                     {"BC5_Green_Half_Alpha", BC5_Green_Half_Alpha_Block},
                                                     {"BC7_Red_Ignore_Alpha", BC7_Red_Ignore_Alpha_Block},
                                                     {"BC7_Blue_Half_Alpha", BC7_Blue_Half_Alpha_Block},
                                                     {"BC7_White_Half_Alpha", BC7_White_Half_Alpha_Block},
                                                     {"BC7_Black_Half_Alpha", BC7_Black_Half_Alpha_Block},
                                                     {"BC7_Red_Blue_Half_Alpha", BC7_Red_Blue_Half_Alpha_Block},
                                                     {"BC7_Red_Green_Half_Alpha", BC7_Red_Green_Half_Alpha_Block},
                                                     {"BC7_Green_Blue_Half_Alpha", BC7_Green_Blue_Half_Alpha_Block},
                                                     {"BC7_Red_Full_Alpha", BC7_Red_Full_Alpha_Block},
                                                     {"BC7_Green_Full_Alpha", BC7_Green_Full_Alpha_Block},
                                                     {"BC7_Blue_Full_Alpha", BC7_Blue_Full_Alpha_Block},
                                                     {"BC7_White_Full_Alpha", BC7_White_Full_Alpha_Block},
                                                     {"BC7_Green_Ignore_Alpha", BC7_Green_Ignore_Alpha_Block},
                                                     {"BC7_Black_Full_Alpha", BC7_Black_Full_Alpha_Block},
                                                     {"BC7_Red_Blue_Full_Alpha", BC7_Red_Blue_Full_Alpha_Block},
                                                     {"BC7_Red_Green_Full_Alpha", BC7_Red_Green_Full_Alpha_Block},
                                                     {"BC7_Green_Blue_Full_Alpha", BC7_Green_Blue_Full_Alpha_Block},
                                                     {"BC7_Blue_Ignore_Alpha", BC7_Blue_Ignore_Alpha_Block},
                                                     {"BC7_White_Ignore_Alpha", BC7_White_Ignore_Alpha_Block},
                                                     {"BC7_Black_Ignore_Alpha", BC7_Black_Ignore_Alpha_Block},
                                                     {"BC7_Red_Blue_Ignore_Alpha", BC7_Red_Blue_Ignore_Alpha_Block},
                                                     {"BC7_Red_Green_Ignore_Alpha", BC7_Red_Green_Ignore_Alpha_Block},
                                                     {"BC7_Green_Blue_Ignore_Alpha", BC7_Green_Blue_Ignore_Alpha_Block},
                                                     {"BC7_Red_Half_Alpha", BC7_Red_Half_Alpha_Block},
                                                     {"BC7_Green_Half_Alpha", BC7_Green_Half_Alpha_Block}};

static std::unordered_map<std::string, BlockBC6> blocksBC6{{"BC6_Red_Ignore_Alpha", BC6_Red_Ignore_Alpha_Block},
                                                           {"BC6_Blue_Half_Alpha", BC6_Blue_Half_Alpha_Block},
                                                           {"BC6_White_Half_Alpha", BC6_White_Half_Alpha_Block},
                                                           {"BC6_Black_Half_Alpha", BC6_Black_Half_Alpha_Block},
                                                           {"BC6_Red_Blue_Half_Alpha", BC6_Red_Blue_Half_Alpha_Block},
                                                           {"BC6_Red_Green_Half_Alpha", BC6_Red_Green_Half_Alpha_Block},
                                                           {"BC6_Green_Blue_Half_Alpha", BC6_Green_Blue_Half_Alpha_Block},
                                                           {"BC6_Red_Full_Alpha", BC6_Red_Full_Alpha_Block},
                                                           {"BC6_Green_Full_Alpha", BC6_Green_Full_Alpha_Block},
                                                           {"BC6_Blue_Full_Alpha", BC6_Blue_Full_Alpha_Block},
                                                           {"BC6_White_Full_Alpha", BC6_White_Full_Alpha_Block},
                                                           {"BC6_Green_Ignore_Alpha", BC6_Green_Ignore_Alpha_Block},
                                                           {"BC6_Black_Full_Alpha", BC6_Black_Full_Alpha_Block},
                                                           {"BC6_Red_Blue_Full_Alpha", BC6_Red_Blue_Full_Alpha_Block},
                                                           {"BC6_Red_Green_Full_Alpha", BC6_Red_Green_Full_Alpha_Block},
                                                           {"BC6_Green_Blue_Full_Alpha", BC6_Green_Blue_Full_Alpha_Block},
                                                           {"BC6_Blue_Ignore_Alpha", BC6_Blue_Ignore_Alpha_Block},
                                                           {"BC6_White_Ignore_Alpha", BC6_White_Ignore_Alpha_Block},
                                                           {"BC6_Black_Ignore_Alpha", BC6_Black_Ignore_Alpha_Block},
                                                           {"BC6_Red_Blue_Ignore_Alpha", BC6_Red_Blue_Ignore_Alpha_Block},
                                                           {"BC6_Red_Green_Ignore_Alpha", BC6_Red_Green_Ignore_Alpha_Block},
                                                           {"BC6_Green_Blue_Ignore_Alpha", BC6_Green_Blue_Ignore_Alpha_Block},
                                                           {"BC6_Red_Half_Alpha", BC6_Red_Half_Alpha_Block},
                                                           {"BC6_Green_Half_Alpha", BC6_Green_Half_Alpha_Block}};

#endif