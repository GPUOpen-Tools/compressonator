//===============================================================================
// Copyright (c) 2014-2016  Advanced Micro Devices, Inc. All rights reserved.
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
//////////////////////////////////////////////////////////////////////////////////

#ifndef _BC6H_DECODE_H_
#define _BC6H_DECODE_H_

#include <float.h>
#include "bc6h_library.h"
#include "bc6h_definitions.h"

class BC6HBlockDecoder {
  public:
    BC6HBlockDecoder() {};
    ~BC6HBlockDecoder() {};
    void DecompressBlock(float out[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],BYTE in[COMPRESSED_BLOCK_SIZE]);

    bool bc6signed = false; // this is suppiled by user for compression for SIGNED_F16 or UNSIGNED_F16 or obtained during decompression

};


#endif
