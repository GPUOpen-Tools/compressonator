//===============================================================================
// Copyright (c) 2007-2018  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//===============================================================================
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
//  File Name:   Codec_ETC.h
//  Description: interface for the CCodec_ETC class
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _Codec_ETC_H_INCLUDED_
#define _Codec_ETC_H_INCLUDED_

#include "common.h"
#include "codec_block_4x4.h"

#ifdef USE_ETCPACK
#include "etcpack.h"
#include "etcpack_lib.h"
#else
typedef unsigned char uint8;
void cmp_decompressBlockETC2c(unsigned int block_part1, unsigned int block_part2, uint8 *img);
void cmp_compressBlockETC2FastPerceptual(uint8 *img, uint8 *imgdec, unsigned int &compressed1, unsigned int &compressed2);
#endif

#define SWIZZLE_DWORD(i) ((((i >> 24) & BYTE_MASK)) | (((i >> 16) & BYTE_MASK) << 8) | (((i >> 8) & BYTE_MASK) << 16) | ((i & BYTE_MASK) << 24))

#define ATC_OFFSET_ALPHA 0
#define ATC_OFFSET_RGB 2

class CCodec_ETC : public CCodec_Block_4x4 {
  public:
    CCodec_ETC(CodecType codecType);
    virtual ~CCodec_ETC();

  protected:
    CodecError CompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);
    CodecError CompressRGBABlock_ExplicitAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]);
    CodecError CompressRGBABlock_InterpolatedAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]);
    CodecError CompressExplicitAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    CodecError CompressInterpolatedAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);

    void DecompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);
    void DecompressRGBABlock_ExplicitAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]);
    void DecompressRGBABlock_InterpolatedAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]);
    void DecompressExplicitAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    void DecompressInterpolatedAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);

    void EncodeAlphaBlock(CMP_DWORD compressedBlock[2], CMP_BYTE nEndpoints[2], CMP_BYTE nIndices[BLOCK_SIZE_4X4]);
    void GetCompressedAlphaRamp(CMP_BYTE alpha[8], CMP_DWORD compressedBlock[2]);
};


#endif // !defined(_Codec_ETC_H_INCLUDED_)
