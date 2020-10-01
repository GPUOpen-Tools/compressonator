//=====================================================================
// Copyright 2018 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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

#ifndef _HPC_COMPRESS_H
#define _HPC_COMPRESS_H

template <typename T> void PadBlock(CMP_DWORD j, CMP_BYTE w, CMP_BYTE h, CMP_BYTE c, T block[]) {
    // So who do we perform generic padding ?
    // In powers of two

    CMP_DWORD dwPadHeight = h - j;
    if (dwPadHeight > j) {
        PadBlock(j, w, h >> 1, c, block);
        j = h >> 1;
        dwPadHeight = h - j;
    }
    memcpy(&block[j*w*c], &block[0], dwPadHeight * w * c * sizeof(T));
}

template <typename T> void PadLine(CMP_DWORD i, CMP_BYTE w, CMP_BYTE c, T block[]) {
    // So who do we perform generic padding ?
    // In powers of two

    CMP_DWORD dwPadWidth = w - i;
    if (dwPadWidth > i) {
        PadLine(i, w >> 1, c, block);
        i = w >> 1;
        dwPadWidth = w - i;
    }

    memcpy(&block[i*c], &block[0], dwPadWidth * c * sizeof(T));
}

#endif
