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

#ifndef _GT_ENCODE_H_
#define _GT_ENCODE_H_

#include <float.h>
#include "GT_Definitions.h"


class GTBlockEncoder
{
public:

    GTBlockEncoder()
    {
    };


    ~GTBlockEncoder()
    {
#ifdef USE_DBGTRACE
                DbgTrace(("Smallest Error %f\n", (float)m_smallestError));
                DbgTrace(("Largest Error %f\n", (float)m_largestError));
#endif
    };

    // This routine compresses a block and returns the RMS error
    double CompressBlock(CMP_BYTE in[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],  BYTE out[COMPRESSED_BLOCK_SIZE]);

private:

    // Global data setup at initialisation time
    double m_quality;
    double m_performance;
    double m_errorThreshold;
};


#endif