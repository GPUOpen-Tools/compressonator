//===============================================================================
// Copyright (c) 2014-2018  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   GT_Encode.h
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _GTCH_ENCODE_H_
#define _GTCH_ENCODE_H_

#include "GT_Definitions.h"

class GTCHBlockEncoder
{
public:

    GTCHBlockEncoder(
        double quality,
        double performance,
        double errorThreshold)
    {
        m_quality = quality;
        m_performance = performance;
        m_errorThreshold = errorThreshold;
        m_isSigned = false;
    };

    ~GTCHBlockEncoder()    {    };

    // This routine compresses a block and returns the RMS error
    double CompressBlock(
        float     in[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],
        CMP_BYTE  out[COMPRESSED_BLOCK_SIZE]
    );

private:
    // Global data setup at initialisation time
    double m_quality;
    double m_performance;
    double m_errorThreshold;
    bool   m_isSigned;
};

#endif
