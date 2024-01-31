//===============================================================================
// Copyright (c) 2022-2024  Advanced Micro Devices, Inc. All rights reserved.
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
//
//  File Name:   BRLG_Encode.h
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _BRLG_ENCODE_H_
#define _BRLG_ENCODE_H_

#include <float.h>
#include "brlg_definitions.h"

class BRLGBlockEncoder
{
public:
    BRLGBlockEncoder(double quality)
    {
        m_quality = (float)quality;
    };

    ~BRLGBlockEncoder(){};

    // This routine compresses a block and returns any errors
    double CompressBlock(CMP_BYTE* in, CMP_BYTE out[COMPRESSED_BLOCK_SIZE]);

private:
    // Global data setup at initialisation time
    float m_quality;
};

#endif
