#ifndef H_ATI_ETCPACK_H
#define H_ATI_ETCPACK_H

//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
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

#include "etcpack_lib.h"

extern int formatSigned;
extern int format;

enum { ETC1_RGB_NO_MIPMAPS, ETC2PACKAGE_RGB_NO_MIPMAPS, ETC2PACKAGE_RGBA_NO_MIPMAPS_OLD, ETC2PACKAGE_RGBA_NO_MIPMAPS, ETC2PACKAGE_RGBA1_NO_MIPMAPS, ETC2PACKAGE_R_NO_MIPMAPS, ETC2PACKAGE_RG_NO_MIPMAPS, ETC2PACKAGE_R_SIGNED_NO_MIPMAPS, ETC2PACKAGE_RG_SIGNED_NO_MIPMAPS, ETC2PACKAGE_sRGB_NO_MIPMAPS, ETC2PACKAGE_sRGBA_NO_MIPMAPS, ETC2PACKAGE_sRGBA1_NO_MIPMAPS };


void atiEncodeRGBBlockETC(
    unsigned char *pPixels,     //for Color888_t(* pPixels)[ 4 ] [ 4 ]
    unsigned int *pCompressed1,    // Return value
    unsigned int *pCompressed2     // Return value
);

void atiDecodeRGBBlockETC(
    unsigned char *pPixels,        //for Color888_t(* pPixels)[ 4 ] [ 4 ]
    unsigned int compressed1,
    unsigned int compressed2
);


void decompressBlockETC21BitAlpha(unsigned int block_part1, unsigned int block_part2, uint8 *img, uint8* alphaimg, int width, int height, int startx, int starty);
void decompressBlockAlpha(uint8* data, uint8* img, int width, int height, int ix, int iy);

void compressBlockAlphaFast(uint8 * data, int ix, int iy, int width, int height, uint8* returnData);
void compressBlockAlphaSlow(uint8* data, int ix, int iy, int width, int height, uint8* returnData);
void compressBlockETC2Fast(uint8 *img, uint8* alphaimg, uint8 *imgdec, int width, int height, int startx, int starty, unsigned int &compressed1, unsigned int &compressed2);
void compressBlockETC2Exhaustive(uint8 *img, uint8 *imgdec, int width, int height, int startx, int starty, unsigned int &compressed1, unsigned int &compressed2);
#endif
