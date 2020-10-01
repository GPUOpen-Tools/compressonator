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
//  Project        ATI Handheld Graphics -- ATI Texture Compression
//
//  Description    This source code is part of the CMP texture compression
//                 library.  It's used to compress textures for OpenGL ES.
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __CMP_H__
#define __CMP_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Color888 {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} Color888_t;

extern unsigned int
atiEncodeRGBBlockATITC(
    Color888_t      (*pPixels)[4][4],
    unsigned int    *pColorLow565or1555Ret, // Return value
    unsigned int    *pColorHigh565Ret       // Return value
);


extern void
atiDecodeRGBBlockATITC(
    Color888_t      (*pPixelsOut)[4][4],
    unsigned int    bitIndices,
    unsigned int    colorLow565or1555,
    unsigned int    colorHigh565
);


extern void
atiEncodeAlphaBlockATITCA4( unsigned char (*pSrcAlpha)[4][4], unsigned int (*pEncodedAlpha)[2] );


extern void
atiDecodeAlphaBlockATITCA4( unsigned char (*pAlphaOut)[4][4], unsigned char *pEncodedData );

#ifdef __cplusplus
}
#endif

#endif  /* __CMP_H__ */
