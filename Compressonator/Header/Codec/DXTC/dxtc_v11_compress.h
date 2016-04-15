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


#ifdef __cplusplus
extern "C" {
#endif

void __cdecl DXTCV11CompressBlockSSE(DWORD *block_32, DWORD *block_dxtc);
void __cdecl DXTCV11CompressBlockSSE2(DWORD *block_32, DWORD *block_dxtc);

void __cdecl DXTCV11CompressAlphaBlock(BYTE block_8[16], DWORD block_dxtc[2]);
void __cdecl DXTCV11CompressExplicitAlphaBlock(BYTE block_8[16], DWORD block_dxtc[2]);


#ifdef _WIN64
void __cdecl DXTCV11CompressExplicitAlphaBlockMMX(BYTE block_8[16], DWORD block_dxtc[2]);
#else
void __fastcall DXTCV11CompressExplicitAlphaBlockMMX(BYTE block_8[16], DWORD block_dxtc[2]);
#endif // !_WIN64

#ifdef __cplusplus
};
#endif


#endif /* AMD_DXTC_COMP_H */
