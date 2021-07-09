//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   ATIXCodec.h
//  Description: performs the DXT-style block compression
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _ATIXCODEC_H_
#define _ATIXCODEC_H_

CODECFLOAT CompRGBBlock(CODECFLOAT* block_32,
                        CMP_DWORD compressedBlock[2],
                        CMP_WORD dwBlockSize,
                        CMP_BYTE nRedBits,
                        CMP_BYTE nGreenBits,
                        CMP_BYTE nBlueBits,
                        CMP_BYTE nEndpoints[3][2],
                        CMP_BYTE* pcIndices,
                        CMP_BYTE dwNumPoints,
                        bool _bUseSSE2,
                        bool b3DRefinement,
                        CMP_BYTE nRefinementSteps,
                        CODECFLOAT* _pfChannelWeights = NULL,
                        bool _bUseAlpha = false,
                        CODECFLOAT _fAlphaThreshold = 0.5);

CODECFLOAT CompRGBBlock(CMP_DWORD* block_32,
                        CMP_DWORD compressedBlock[2],
                        CMP_WORD dwBlockSize,
                        CMP_BYTE nRedBits,
                        CMP_BYTE nGreenBits,
                        CMP_BYTE nBlueBits,
                        CMP_BYTE nEndpoints[3][2],
                        CMP_BYTE* pcIndices,
                        CMP_BYTE dwNumPoints,
                        bool _bUseSSE2,
                        bool b3DRefinement,
                        CMP_BYTE nRefinementSteps,
                        CODECFLOAT* _pfChannelWeights = NULL,
                        bool _bUseAlpha = false,
                        CMP_BYTE _nAlphaThreshold = 128);

/*--------------------------------------------------------------------------------------------
// input [0,1]
void CompBlock1X(CODECFLOAT* _Blk,              [IN] scalar data block (alphas or normals) in float format
                 CMP_DWORD blockCompressed[2], [OUT] compressed data in DXT5 alpha foramt
                 int _NbrClrs,                    [IN] actual number of elements in the block
                 int _intPrec,                    [IN} integer precision; it applies both to the input data and
                                                       to the ramp points
                 int _fracPrec,                   [IN] fractional precision of the ramp points
                 bool _bFixedRamp,                [IN] non-fixed ramp means we have input and generate
                                                       output as float. fixed ramp means that they are fractional numbers.
                 bool _bUseSSE2                    [IN] forces to switch to the SSE2 implementation
                )

---------------------------------------------------------------------------------------------*/

CODECFLOAT CompBlock1X(CODECFLOAT* _Blk,
                       CMP_WORD dwBlockSize,
                       CMP_BYTE nEndpoints[2],
                       CMP_BYTE* pcIndices,
                       CMP_BYTE dwNumPoints,
                       bool bFixedRampPoints,
                       bool _bUseSSE2 = true,
                       int _intPrec = 8,
                       int _fracPrec = 0,
                       bool _bFixedRamp = true);

CODECFLOAT CompBlock1XS(CODECFLOAT* _Blk,
                       CMP_WORD    dwBlockSize,
                       CMP_BYTE    nEndpoints[2],
                       CMP_BYTE*   pcIndices,
                       CMP_BYTE    dwNumPoints,
                       bool        bFixedRampPoints,
                       bool        _bUseSSE2   = true,
                       int         _intPrec    = 8,
                       int         _fracPrec   = 0,
                       bool        _bFixedRamp = true);


/*--------------------------------------------------------------------------------------------
// input [0,255]
void CompBlock1X(CMP_BYTE* _Blk,                [IN] scalar data block (alphas or normals) in 8 bits format
                 CMP_DWORD blockCompressed[2],    [OUT] compressed data in DXT5 alpha foramt
                 int _NbrClrs,                        [IN] actual number of elements in the block
                 int _intPrec,                        [IN] integer precision; it applies both to the input data and
                                                         to the ramp points
                 int _fracPrec,                        [IN] fractional precision of the ramp points
                 bool _bFixedRamp,                    [IN] always true at this point
                 bool _bUseSSE2                        [IN] forces to switch to the SSE2 implementation
                )
---------------------------------------------------------------------------------------------*/

CODECFLOAT CompBlock1X(CMP_BYTE* _Blk,
                       CMP_WORD dwBlockSize,
                       CMP_BYTE nEndpoints[2],
                       CMP_BYTE* pcIndices,
                       CMP_BYTE dwNumPoints,
                       bool bFixedRampPoints,
                       bool _bUseSSE2 = true,
                       int _intPrec = 8,
                       int _fracPrec = 0,
                       bool _bFixedRamp = true);

CODECFLOAT CompBlock1XS(CMP_SBYTE* _Blk,
                       CMP_WORD  dwBlockSize,
                       CMP_BYTE  nEndpoints[2],
                       CMP_BYTE* pcIndices,
                       CMP_BYTE  dwNumPoints,
                       bool      bFixedRampPoints,
                       bool      _bUseSSE2   = true,
                       int       _intPrec    = 8,
                       int       _fracPrec   = 0,
                       bool      _bFixedRamp = true);

#endif