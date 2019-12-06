//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//===============================================================================
//
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


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#pragma warning(disable:4189)
#pragma warning(disable:4244)
#pragma warning(disable:4514)
#endif //_MSC_VER

#if 1
    // Set SPEED to FAST or MEDIUM
    #define MEDIUM      2
    #define FAST        3
    #define SPEED       FAST

    #if SPEED != FAST && SPEED != MEDIUM
        #error SPEED must be set to FAST or MEDIUM!
    #endif
#endif

#include "etcpack_lib.h"

//-------------------------------------------------------------------------
// WRAPPER ADDED AROUND FUNCTION THAT ENCODES A SINGLE 4X4 BLOCK INTO ETC
// Note:  readCompressParams() must be called before each new image.
//-------------------------------------------------------------------------
void atiEncodeRGBBlockETC(
    unsigned char *pPixels,        //Color888_t(* pPixels)[ 4 ] [ 4 ],
    unsigned int *pCompressed1,    // Return value
    unsigned int *pCompressed2    // Return value
    )
{
    unsigned int compressed1, compressed2;    // Used to interface C's (unsigned int*) to C++'s &(unsigned int)
#if SPEED == FAST
    unsigned char tmp[4*4*3];                // Required just by "fast" ETC function

    compressBlockDiffFlipFastPerceptual( (uint8 *)pPixels, (uint8 *)tmp, 4, 4, 0, 0, compressed1, compressed2 );
#elif SPEED == MEDIUM
    compressBlockDiffFlipMediumPerceptual( (uint8 *)pPixels, 4, 4, 0, 0, compressed1, compressed2 );
#else
    #error SPEED must be set to FAST or MEDIUM!
#endif

    *pCompressed1 = compressed1;
    *pCompressed2 = compressed2;
}

//-------------------------------------------------------------------------
// WRAPPER ADDED AROUND FUNCTION THAT DECODES A SINGLE 4X4 BLOCK INTO ETC
// Note:  readCompressParams() must be called before each new image.
//-------------------------------------------------------------------------
void atiDecodeRGBBlockETC(
    unsigned char *pPixels,        //Color888_t(* pPixels)[ 4 ] [ 4 ],
    unsigned int compressed1,
    unsigned int compressed2
    )
{
    decompressBlockDiffFlip( compressed1, compressed2, (uint8*)pPixels, 4, 4, 0, 0);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER
