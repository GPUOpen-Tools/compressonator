//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2020 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef _CMP_MIPS_H
#define _CMP_MIPS_H

#include "Common.h"
#include "Compressonator.h"
#include "stdlib.h"

#ifndef CMP_MAX
#define CMP_MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#define CMP_PAD_BYTE(width, bpp) (((bpp) * (width) + 7) / 8)

#define CMP_MIPS_FOURCC_DX10                    CMP_MAKEFOURCC('D', 'X', '1', '0')

template <class T> const T& max(const T& a, const T& b) {
    return (a < b) ? b : a;
}

extern CMP_INT CMP_MaxFacesOrSlices(const CMP_MipSet* pMipSet, CMP_INT nMipLevel);

#endif
