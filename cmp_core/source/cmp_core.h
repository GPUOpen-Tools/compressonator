//=====================================================================
// Copyright (c) 2020   Advanced Micro Devices, Inc. All rights reserved.
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
/// \file CMP_Core.h  CPU User Interface
//
//=====================================================================

#ifndef CMP_CORE_H
#define CMP_CORE_H

#include <stdint.h>
#ifdef _WIN32
#define CMP_CDECL __cdecl
#else
#define CMP_CDECL
#endif

//====================================================================================
// API Definitions for Core API
//------------------------------------------------------------------------------------
// All API return 0 on success else error codes > 0
// See Common_Def.h CGU_CORE_ values for the error codes
//=====================================================================================

//======================================================================================================
// Block level setting option: Create and Destroy Reference Pointers
//======================================================================================================
// Context create and destroy to use for BCn codec settings, where n is the set [1,2,3,4,5,6,7]
// All codecs will use default max quality settings, users can create multiple contexts to
// set quality levels, masks , channel mapping, etc...

int32_t CMP_CDECL CreateOptionsBC1(void **optionsBC1);
int32_t CMP_CDECL CreateOptionsBC2(void **optionsBC2);
int32_t CMP_CDECL CreateOptionsBC3(void **optionsBC3);
int32_t CMP_CDECL CreateOptionsBC4(void **optionsBC4);
int32_t CMP_CDECL CreateOptionsBC5(void **optionsBC5);
int32_t CMP_CDECL CreateOptionsBC6(void **optionsBC6);
int32_t CMP_CDECL CreateOptionsBC7(void **optionsBC7);

int32_t CMP_CDECL DestroyOptionsBC1(void *optionsBC1);
int32_t CMP_CDECL DestroyOptionsBC2(void *optionsBC2);
int32_t CMP_CDECL DestroyOptionsBC3(void *optionsBC3);
int32_t CMP_CDECL DestroyOptionsBC4(void *optionsBC4);
int32_t CMP_CDECL DestroyOptionsBC5(void *optionsBC5);
int32_t CMP_CDECL DestroyOptionsBC6(void *optionsBC6);
int32_t CMP_CDECL DestroyOptionsBC7(void *optionsBC7);


//======================================================================================================
// Block level settings using the options Reference Pointers
//======================================================================================================

// Setting channel Weights : Applies to BC1, BC2 and BC3 valid ranges are [0..1.0f] Default is {1.0f, 1.0f , 1.0f}
// Use channel weightings. With swizzled formats the weighting applies to the data within the specified channel not the channel itself.
int32_t CMP_CDECL SetChannelWeightsBC1(void *options, float WeightRed, float WeightGreen, float WeightBlue);
int32_t CMP_CDECL SetChannelWeightsBC2(void *options, float WeightRed, float WeightGreen, float WeightBlue);
int32_t CMP_CDECL SetChannelWeightsBC3(void *options, float WeightRed, float WeightGreen, float WeightBlue);


//  True sets mapping CMP_Core BC1, BC2 & BC3 to decode Red,Green,Blue and Alpha as
//       RGBA to channels [0,1,2,3] else BGRA maps to [0,1,2,3]
//  Default is set to true.
int32_t CMP_CDECL SetDecodeChannelMapping(void* options, bool mapRGBA);

int32_t CMP_CDECL SetQualityBC1(void *options, float fquality);
int32_t CMP_CDECL SetQualityBC2(void *options, float fquality);
int32_t CMP_CDECL SetQualityBC3(void *options, float fquality);
int32_t CMP_CDECL SetQualityBC4(void *options, float fquality);
int32_t CMP_CDECL SetQualityBC5(void *options, float fquality);
int32_t CMP_CDECL SetQualityBC6(void *options, float fquality);
int32_t CMP_CDECL SetQualityBC7(void *options, float fquality);


int32_t CMP_CDECL SetAlphaThresholdBC1(void* options, uint8_t alphaThreshold);

int32_t CMP_CDECL SetMaskBC6(void* options, uint32_t mask);
int32_t CMP_CDECL SetMaskBC7(void* options, uint8_t mask);

int32_t CMP_CDECL SetAlphaOptionsBC7(void *options, bool imageNeedsAlpha, bool colourRestrict, bool alphaRestrict);
int32_t CMP_CDECL SetErrorThresholdBC7(void *options, float minThreshold, float maxThreshold);

// Set if the content is in sRGB color space (true) or linear (false).
// The default is false.
int32_t CMP_CDECL SetGammaBC1(void *options, bool sRGB);
int32_t CMP_CDECL SetGammaBC2(void *options, bool sRGB);
int32_t CMP_CDECL SetGammaBC3(void *options, bool sRGB);
int32_t CMP_CDECL SetGammaBC7(void *options, bool sRGB);

// Set if the content is signed (true) or unsigned (false).
// The default is false.
// For BC4 and BC5 this determines if the encoded or decoded byte is treated as SNORM or UNORM.
// For BC6, the encoded or decoded data is always FP16, but affects the clamping of the values UF16 vs SF16.
int32_t CMP_CDECL SetSignedBC4(void *options, bool snorm);
int32_t CMP_CDECL SetSignedBC5(void *options, bool snorm);
int32_t CMP_CDECL SetSignedBC6(void *options, bool sf16);

//======================================================================================================
// (4x4) Block level 4 channel source CompressBlock and DecompressBlock API for BCn Codecs
//======================================================================================================
// The options parameter for these API can be set to null in the calls if defaults settings is sufficient
// Example: CompressBlockBC1(srcBlock,16,cmpBlock,NULL);   For "C" call
//          CompressBlockBC1(srcBlock,16,cmpBlock);        For "C++" calls
//
// To use this parameter first create the options context using the CreateOptions call
// then use the Set Options to set various codec settings and pass them to the appropriate
// Compress or Decompress API.
// The source (srcBlock) channel format is expected to be RGBA:8888 by default for LDR Codecs
// for BC6H the format is RGBA Half float (16 bits per channel)
//------------------------------------------------------------------------------------------------------
#ifdef __cplusplus
#define CMP_DEFAULTNULL  =nullptr
#else
#define CMP_DEFAULTNULL
#endif

//=========================================================================================================
// 4 channel Sources, default format RGBA:8888 is processed as a 4x4 block starting at srcBlock location
// where each row of the block is calculated from srcStride
//=========================================================================================================
int32_t CMP_CDECL CompressBlockBC1(const uint8_t* srcBlock, uint32_t srcStrideInBytes, uint8_t cmpBlock[8 ], const void* options CMP_DEFAULTNULL);
int32_t CMP_CDECL CompressBlockBC2(const uint8_t* srcBlock, uint32_t srcStrideInBytes, uint8_t cmpBlock[16], const void* options CMP_DEFAULTNULL);
int32_t CMP_CDECL CompressBlockBC3(const uint8_t* srcBlock, uint32_t srcStrideInBytes, uint8_t cmpBlock[16], const void* options CMP_DEFAULTNULL);
int32_t CMP_CDECL CompressBlockBC7(const uint8_t* srcBlock, uint32_t srcStrideInBytes, uint8_t cmpBlock[16], const void* options CMP_DEFAULTNULL);

int32_t CMP_CDECL DecompressBlockBC1(const uint8_t cmpBlock[8 ], uint8_t srcBlock[64], const void* options CMP_DEFAULTNULL);
int32_t CMP_CDECL DecompressBlockBC2(const uint8_t cmpBlock[16], uint8_t srcBlock[64], const void* options CMP_DEFAULTNULL);
int32_t CMP_CDECL DecompressBlockBC3(const uint8_t cmpBlock[16], uint8_t srcBlock[64], const void* options CMP_DEFAULTNULL);
int32_t CMP_CDECL DecompressBlockBC7(const uint8_t cmpBlock[16], uint8_t srcBlock[64], const void* options CMP_DEFAULTNULL);

//================================================
// 1 channel Source 4x4 8 bits per block
//================================================
int32_t CMP_CDECL CompressBlockBC4(const uint8_t* srcBlock, uint32_t srcStrideInBytes, uint8_t cmpBlock[8], const void* options CMP_DEFAULTNULL);
int32_t CMP_CDECL DecompressBlockBC4(const uint8_t cmpBlock[8], uint8_t srcBlock[16], const void* options CMP_DEFAULTNULL);

int32_t CMP_CDECL CompressBlockBC4S(const int8_t* srcBlock, uint32_t srcStrideInBytes, int8_t cmpBlock[8], const void* options CMP_DEFAULTNULL);
int32_t CMP_CDECL DecompressBlockBC4S(const int8_t cmpBlock[8], int8_t srcBlock[16], const void* options CMP_DEFAULTNULL);


//================================================
// 2 channel Source 2x(4x4 8 bits)
//================================================
int32_t CMP_CDECL CompressBlockBC5(const uint8_t* srcBlock1,
                                   uint32_t       srcStrideInBytes1,
                                   const uint8_t* srcBlock2,
                                   uint32_t       srcStrideInBytes2,
                                   uint8_t        cmpBlock[16],
                                   const void* options CMP_DEFAULTNULL);
int32_t CMP_CDECL DecompressBlockBC5(const uint8_t cmpBlock[16], uint8_t srcBlock1[16], uint8_t srcBlock2[16], const void* options CMP_DEFAULTNULL);


int32_t CMP_CDECL CompressBlockBC5S(const int8_t*  srcBlock1,
                                    uint32_t       srcStrideInBytes1,
                                    const int8_t*  srcBlock2,
                                    uint32_t       srcStrideInBytes2,
                                    int8_t         cmpBlock[16],
                                    const void*    options CMP_DEFAULTNULL);

int32_t CMP_CDECL DecompressBlockBC5S(const int8_t cmpBlock[16], int8_t srcBlock1[16], int8_t srcBlock2[16],
                                      const void* options CMP_DEFAULTNULL);


//========================================================================================
// For 3 channel Source  RGB_16, Note srcStride is in unsigned short steps (2 bytes each)
//========================================================================================
int32_t CMP_CDECL CompressBlockBC6(const uint16_t* srcBlock, uint32_t srcStrideInShorts, uint8_t cmpBlock[16], const void* options CMP_DEFAULTNULL);
int32_t CMP_CDECL DecompressBlockBC6(const uint8_t cmpBlock[16], uint16_t srcBlock[48], const void* options CMP_DEFAULTNULL);

#endif  // CMP_CORE
