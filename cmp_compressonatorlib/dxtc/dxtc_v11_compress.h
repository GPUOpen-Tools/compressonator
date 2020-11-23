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

#ifndef AMD_DXTC_COMP_H
#define AMD_DXTC_COMP_H

///
//    Public Functions
//

#include <stdint.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

#ifdef __cplusplus
extern "C" {
#endif

void DXTCV11CompressBlockSSE(DWORD *block_32, DWORD *block_dxtc);

#ifdef _WIN32
void __cdecl DXTCV11CompressBlockSSE2(DWORD *block_32, DWORD *block_dxtc);
#endif


void DXTCV11CompressBlockSSEMinimal(DWORD *block_32, DWORD *block_dxtc);
void DXTCV11CompressBlockMinimal(DWORD block_32[16], DWORD block_dxtc[2]);

void DXTCV11CompressAlphaBlock(BYTE block_8[16], DWORD block_dxtc[2]);
void DXTCV11CompressExplicitAlphaBlock(BYTE block_8[16], DWORD block_dxtc[2]);

#ifdef _WIN32
#if defined(_WIN64) || defined(__linux__)
void DXTCV11CompressExplicitAlphaBlockMMX(BYTE block_8[16], DWORD block_dxtc[2]);
#else
void __fastcall DXTCV11CompressExplicitAlphaBlockMMX(BYTE block_8[16], DWORD block_dxtc[2]);
#endif // !_WIN64
#endif //_WIN32

#ifdef __cplusplus
};
#endif


#endif /* AMD_DXTC_COMP_H */
