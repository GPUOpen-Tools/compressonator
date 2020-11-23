//===============================================================================
// Copyright (c) 2007-2017  Advanced Micro Devices, Inc. All rights reserved.
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
//
//  File Name:   HDR_Encode.cpp
//  Description: Reserved utils function for HDR process
//
//////////////////////////////////////////////////////////////////////////////


#include "hdr_encode.h"
#include <assert.h>
#include <math.h>
#include <float.h>
#include <cstdlib>

namespace HDR_Encode {

#define USE_NEWRAMP

#ifdef USE_RAMPS
#include <mutex>
static int g_init_ramps = 0;
std::mutex mtx;
#endif



//==============================================================================================
// return # of bits needed to store n. handle signed or unsigned cases properly
inline int NBits(int n, bool bIsSigned) {
    int nb;
    if (n == 0) {
        return 0; // no bits needed for 0, signed or not
    } else if (n > 0) {
        for (nb = 0; n; ++nb, n >>= 1);
        return nb + (bIsSigned ? 1 : 0);
    } else {
        assert(bIsSigned);
        for (nb = 0; n < -1; ++nb, n >>= 1);
        return nb + 1;
    }
}

float lerpf(float a, float b, int i, int denom) {
    assert(denom == 3 || denom == 7 || denom == 15);
    assert(i >= 0 && i <= denom);

    int *weights = NULL;

    switch (denom) {
    case 3:
        denom *= 5;
        i *= 5;    // fall through to case 15
    case 7:
        weights = g_aWeights3;
        break;
    case 15:
        weights = g_aWeights4;
        break;
    default:
        assert(0);
    }
    return (a*weights[denom - i] + b*weights[i]) / 64.0f;
}

int QuantizeToInt(short value, int prec, bool signedfloat16, float exposure) {
    (exposure);

    if (prec <= 1) return 0;
    bool negvalue = false;

    // move data to use extra bits for processing
    int ivalue = value;

    if (signedfloat16) {
        if (value < 0) {
            negvalue = true;
            value = -value;
        }
        prec--;
    } else {
        // clamp -ve
        if (value < 0)
            value = 0;
    }

    int iQuantized;
    int bias = (prec > 10 && prec != 16) ? ((1 << (prec - 11)) - 1) : 0;
    bias = (prec == 16) ? 15 : bias;

    iQuantized = ((ivalue << prec) + bias) / (F16HMAX + 1);

    return (negvalue ? -iQuantized : iQuantized);
}

int Unquantize(int comp, unsigned char uBitsPerComp, bool bSigned) {
    int unq = 0, s = 0;
    if (bSigned) {
        if (uBitsPerComp >= 16) {
            unq = comp;
        } else {
            if (comp < 0) {
                s = 1;
                comp = -comp;
            }

            if (comp == 0) unq = 0;
            else if (comp >= ((1 << (uBitsPerComp - 1)) - 1)) unq = 0x7FFF;
            else unq = ((comp << 15) + 0x4000) >> (uBitsPerComp - 1);

            if (s) unq = -unq;
        }
    } else {
        if (uBitsPerComp >= 15) unq = comp;
        else if (comp == 0) unq = 0;
        else if (comp == ((1 << uBitsPerComp) - 1)) unq = 0xFFFF;
        else unq = ((comp << 16) + 0x8000) >> uBitsPerComp;
    }

    return unq;
}

//==============================================================================================
int   PARTITIONS[MAX_SUBSETS][MAX_PARTITIONS][MAX_SUBSET_SIZE] = {
    // Single subset partitions for both BC6H abd BC7
    {
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },

    {
        {
            // 0
            0,0,1,1,
            0,0,1,1,
            0,0,1,1,
            0,0,1,1
        },

        {
            // 1
            0,0,0,1,
            0,0,0,1,
            0,0,0,1,
            0,0,0,1
        },

        {
            // 2
            0,1,1,1,
            0,1,1,1,
            0,1,1,1,
            0,1,1,1
        },

        {
            // 3
            0,0,0,1,
            0,0,1,1,
            0,0,1,1,
            0,1,1,1
        },

        {
            // 4
            0,0,0,0,
            0,0,0,1,
            0,0,0,1,
            0,0,1,1
        },

        {
            // 5
            0,0,1,1,
            0,1,1,1,
            0,1,1,1,
            1,1,1,1
        },

        {
            // 6
            0,0,0,1,
            0,0,1,1,
            0,1,1,1,
            1,1,1,1
        },

        {
            // 7
            0,0,0,0,
            0,0,0,1,
            0,0,1,1,
            0,1,1,1
        },

        {
            // 8
            0,0,0,0,
            0,0,0,0,
            0,0,0,1,
            0,0,1,1
        },

        {
            // 9
            0,0,1,1,
            0,1,1,1,
            1,1,1,1,
            1,1,1,1
        },

        {
            // 10
            0,0,0,0,
            0,0,0,1,
            0,1,1,1,
            1,1,1,1
        },

        {
            // 11
            0,0,0,0,
            0,0,0,0,
            0,0,0,1,
            0,1,1,1
        },

        {
            // 12
            0,0,0,1,
            0,1,1,1,
            1,1,1,1,
            1,1,1,1
        },

        {
            // 13
            0,0,0,0,
            0,0,0,0,
            1,1,1,1,
            1,1,1,1
        },

        {
            // 14
            0,0,0,0,
            1,1,1,1,
            1,1,1,1,
            1,1,1,1
        },

        {
            // 15
            0,0,0,0,
            0,0,0,0,
            0,0,0,0,
            1,1,1,1
        },

        {
            // 16
            0,0,0,0,
            1,0,0,0,
            1,1,1,0,
            1,1,1,1
        },

        {
            // 17
            0,1,1,1,
            0,0,0,1,
            0,0,0,0,
            0,0,0,0
        },

        {
            // 18
            0,0,0,0,
            0,0,0,0,
            1,0,0,0,
            1,1,1,0
        },

        {
            // 19
            0,1,1,1,
            0,0,1,1,
            0,0,0,1,
            0,0,0,0
        },

        {
            // 20
            0,0,1,1,
            0,0,0,1,
            0,0,0,0,
            0,0,0,0
        },

        {
            // 21
            0,0,0,0,
            1,0,0,0,
            1,1,0,0,
            1,1,1,0
        },

        {
            // 22
            0,0,0,0,
            0,0,0,0,
            1,0,0,0,
            1,1,0,0
        },

        {
            // 23
            0,1,1,1,
            0,0,1,1,
            0,0,1,1,
            0,0,0,1
        },

        {
            // 24
            0,0,1,1,
            0,0,0,1,
            0,0,0,1,
            0,0,0,0
        },

        {
            // 25
            0,0,0,0,
            1,0,0,0,
            1,0,0,0,
            1,1,0,0
        },

        {
            // 26
            0,1,1,0,
            0,1,1,0,
            0,1,1,0,
            0,1,1,0
        },

        {
            // 27
            0,0,1,1,
            0,1,1,0,
            0,1,1,0,
            1,1,0,0
        },

        {
            // 28
            0,0,0,1,
            0,1,1,1,
            1,1,1,0,
            1,0,0,0
        },

        {
            // 29
            0,0,0,0,
            1,1,1,1,
            1,1,1,1,
            0,0,0,0
        },

        {
            // 30
            0,1,1,1,
            0,0,0,1,
            1,0,0,0,
            1,1,1,0
        },

        {
            // 31
            0,0,1,1,
            1,0,0,1,
            1,0,0,1,
            1,1,0,0
        },
        // -----------  BC7 only shapes from here on -------------
        {
            // 32
            0,1,0,1,
            0,1,0,1,
            0,1,0,1,
            0,1,0,1
        },

        {
            // 33
            0,0,0,0,
            1,1,1,1,
            0,0,0,0,
            1,1,1,1
        },

        {
            // 34
            0,1,0,1,
            1,0,1,0,
            0,1,0,1,
            1,0,1,0
        },

        {
            // 35
            0,0,1,1,
            0,0,1,1,
            1,1,0,0,
            1,1,0,0
        },

        {
            // 36
            0,0,1,1,
            1,1,0,0,
            0,0,1,1,
            1,1,0,0
        },

        {
            // 37
            0,1,0,1,
            0,1,0,1,
            1,0,1,0,
            1,0,1,0
        },

        {
            // 38
            0,1,1,0,
            1,0,0,1,
            0,1,1,0,
            1,0,0,1
        },

        {
            // 39
            0,1,0,1,
            1,0,1,0,
            1,0,1,0,
            0,1,0,1
        },

        {
            // 40
            0,1,1,1,
            0,0,1,1,
            1,1,0,0,
            1,1,1,0
        },

        {
            // 41
            0,0,0,1,
            0,0,1,1,
            1,1,0,0,
            1,0,0,0
        },

        {
            // 42
            0,0,1,1,
            0,0,1,0,
            0,1,0,0,
            1,1,0,0
        },

        {
            // 43
            0,0,1,1,
            1,0,1,1,
            1,1,0,1,
            1,1,0,0
        },

        {
            // 44
            0,1,1,0,
            1,0,0,1,
            1,0,0,1,
            0,1,1,0
        },

        {
            // 45
            0,0,1,1,
            1,1,0,0,
            1,1,0,0,
            0,0,1,1
        },

        {
            // 46
            0,1,1,0,
            0,1,1,0,
            1,0,0,1,
            1,0,0,1
        },

        {
            // 47
            0,0,0,0,
            0,1,1,0,
            0,1,1,0,
            0,0,0,0
        },

        {
            // 48
            0,1,0,0,
            1,1,1,0,
            0,1,0,0,
            0,0,0,0
        },

        {
            // 49
            0,0,1,0,
            0,1,1,1,
            0,0,1,0,
            0,0,0,0
        },

        {
            // 50
            0,0,0,0,
            0,0,1,0,
            0,1,1,1,
            0,0,1,0
        },

        {
            // 51
            0,0,0,0,
            0,1,0,0,
            1,1,1,0,
            0,1,0,0
        },

        {
            // 52
            0,1,1,0,
            1,1,0,0,
            1,0,0,1,
            0,0,1,1
        },

        {
            // 53
            0,0,1,1,
            0,1,1,0,
            1,1,0,0,
            1,0,0,1
        },

        {
            // 54
            0,1,1,0,
            0,0,1,1,
            1,0,0,1,
            1,1,0,0
        },

        {
            // 55
            0,0,1,1,
            1,0,0,1,
            1,1,0,0,
            0,1,1,0
        },

        {
            // 56
            0,1,1,0,
            1,1,0,0,
            1,1,0,0,
            1,0,0,1
        },

        {
            // 57
            0,1,1,0,
            0,0,1,1,
            0,0,1,1,
            1,0,0,1
        },

        {
            // 58
            0,1,1,1,
            1,1,1,0,
            1,0,0,0,
            0,0,0,1
        },

        {
            // 59
            0,0,0,1,
            1,0,0,0,
            1,1,1,0,
            0,1,1,1
        },

        {
            // 60
            0,0,0,0,
            1,1,1,1,
            0,0,1,1,
            0,0,1,1
        },

        {
            // 61
            0,0,1,1,
            0,0,1,1,
            1,1,1,1,
            0,0,0,0
        },

        {
            // 62
            0,0,1,0,
            0,0,1,0,
            1,1,1,0,
            1,1,1,0
        },

        {
            // 63
            0,1,0,0,
            0,1,0,0,
            0,1,1,1,
            0,1,1,1
        },
    },


    // Table.P3 - only for BC7

    {

        {
            0,0,1,1,
            0,0,1,1,
            0,2,2,1,
            2,2,2,2
        },

        {
            0,0,0,1,
            0,0,1,1,
            2,2,1,1,
            2,2,2,1
        },

        {
            0,0,0,0,
            2,0,0,1,
            2,2,1,1,
            2,2,1,1
        },

        {
            0,2,2,2,
            0,0,2,2,
            0,0,1,1,
            0,1,1,1
        },

        {
            0,0,0,0,
            0,0,0,0,
            1,1,2,2,
            1,1,2,2
        },

        {
            0,0,1,1,
            0,0,1,1,
            0,0,2,2,
            0,0,2,2
        },

        {
            0,0,2,2,
            0,0,2,2,
            1,1,1,1,
            1,1,1,1
        },

        {
            0,0,1,1,
            0,0,1,1,
            2,2,1,1,
            2,2,1,1
        },

        {
            0,0,0,0,
            0,0,0,0,
            1,1,1,1,
            2,2,2,2
        },

        {
            0,0,0,0,
            1,1,1,1,
            1,1,1,1,
            2,2,2,2
        },

        {
            0,0,0,0,
            1,1,1,1,
            2,2,2,2,
            2,2,2,2
        },

        {
            0,0,1,2,
            0,0,1,2,
            0,0,1,2,
            0,0,1,2
        },

        {
            0,1,1,2,
            0,1,1,2,
            0,1,1,2,
            0,1,1,2
        },

        {
            0,1,2,2,
            0,1,2,2,
            0,1,2,2,
            0,1,2,2
        },

        {
            0,0,1,1,
            0,1,1,2,
            1,1,2,2,
            1,2,2,2
        },

        {
            0,0,1,1,
            2,0,0,1,
            2,2,0,0,
            2,2,2,0
        },

        {
            0,0,0,1,
            0,0,1,1,
            0,1,1,2,
            1,1,2,2
        },

        {
            0,1,1,1,
            0,0,1,1,
            2,0,0,1,
            2,2,0,0
        },

        {
            0,0,0,0,
            1,1,2,2,
            1,1,2,2,
            1,1,2,2
        },

        {
            0,0,2,2,
            0,0,2,2,
            0,0,2,2,
            1,1,1,1
        },

        {
            0,1,1,1,
            0,1,1,1,
            0,2,2,2,
            0,2,2,2
        },

        {
            0,0,0,1,
            0,0,0,1,
            2,2,2,1,
            2,2,2,1
        },

        {
            0,0,0,0,
            0,0,1,1,
            0,1,2,2,
            0,1,2,2
        },

        {
            0,0,0,0,
            1,1,0,0,
            2,2,1,0,
            2,2,1,0
        },

        {
            0,1,2,2,
            0,1,2,2,
            0,0,1,1,
            0,0,0,0
        },

        {
            0,0,1,2,
            0,0,1,2,
            1,1,2,2,
            2,2,2,2
        },

        {
            0,1,1,0,
            1,2,2,1,
            1,2,2,1,
            0,1,1,0
        },

        {
            0,0,0,0,
            0,1,1,0,
            1,2,2,1,
            1,2,2,1
        },

        {
            0,0,2,2,
            1,1,0,2,
            1,1,0,2,
            0,0,2,2
        },

        {
            0,1,1,0,
            0,1,1,0,
            2,0,0,2,
            2,2,2,2
        },

        {
            0,0,1,1,
            0,1,2,2,
            0,1,2,2,
            0,0,1,1
        },

        {
            0,0,0,0,
            2,0,0,0,
            2,2,1,1,
            2,2,2,1
        },

        {
            0,0,0,0,
            0,0,0,2,
            1,1,2,2,
            1,2,2,2
        },

        {
            0,2,2,2,
            0,0,2,2,
            0,0,1,2,
            0,0,1,1
        },

        {
            0,0,1,1,
            0,0,1,2,
            0,0,2,2,
            0,2,2,2
        },

        {
            0,1,2,0,
            0,1,2,0,
            0,1,2,0,
            0,1,2,0
        },

        {
            0,0,0,0,
            1,1,1,1,
            2,2,2,2,
            0,0,0,0
        },

        {
            0,1,2,0,
            1,2,0,1,
            2,0,1,2,
            0,1,2,0
        },

        {
            0,1,2,0,
            2,0,1,2,
            1,2,0,1,
            0,1,2,0
        },

        {
            0,0,1,1,
            2,2,0,0,
            1,1,2,2,
            0,0,1,1
        },

        {
            0,0,1,1,
            1,1,2,2,
            2,2,0,0,
            0,0,1,1
        },

        {
            0,1,0,1,
            0,1,0,1,
            2,2,2,2,
            2,2,2,2
        },

        {
            0,0,0,0,
            0,0,0,0,
            2,1,2,1,
            2,1,2,1
        },

        {
            0,0,2,2,
            1,1,2,2,
            0,0,2,2,
            1,1,2,2
        },

        {
            0,0,2,2,
            0,0,1,1,
            0,0,2,2,
            0,0,1,1
        },

        {
            0,2,2,0,
            1,2,2,1,
            0,2,2,0,
            1,2,2,1
        },

        {
            0,1,0,1,
            2,2,2,2,
            2,2,2,2,
            0,1,0,1
        },

        {
            0,0,0,0,
            2,1,2,1,
            2,1,2,1,
            2,1,2,1
        },

        {
            0,1,0,1,
            0,1,0,1,
            0,1,0,1,
            2,2,2,2
        },

        {
            0,2,2,2,
            0,1,1,1,
            0,2,2,2,
            0,1,1,1
        },

        {
            0,0,0,2,
            1,1,1,2,
            0,0,0,2,
            1,1,1,2
        },

        {
            0,0,0,0,
            2,1,1,2,
            2,1,1,2,
            2,1,1,2
        },

        {
            0,2,2,2,
            0,1,1,1,
            0,1,1,1,
            0,2,2,2
        },

        {
            0,0,0,2,
            1,1,1,2,
            1,1,1,2,
            0,0,0,2
        },

        {
            0,1,1,0,
            0,1,1,0,
            0,1,1,0,
            2,2,2,2
        },

        {
            0,0,0,0,
            0,0,0,0,
            2,1,1,2,
            2,1,1,2
        },

        {
            0,1,1,0,
            0,1,1,0,
            2,2,2,2,
            2,2,2,2
        },

        {
            0,0,2,2,
            0,0,1,1,
            0,0,1,1,
            0,0,2,2
        },

        {
            0,0,2,2,
            1,1,2,2,
            1,1,2,2,
            0,0,2,2
        },

        {
            0,0,0,0,
            0,0,0,0,
            0,0,0,0,
            2,1,1,2
        },

        {
            0,0,0,2,
            0,0,0,1,
            0,0,0,2,
            0,0,0,1
        },

        {
            0,2,2,2,
            1,2,2,2,
            0,2,2,2,
            1,2,2,2
        },

        {
            0,1,0,1,
            2,2,2,2,
            2,2,2,2,
            2,2,2,2
        },

        {
            0,1,1,1,
            2,0,1,1,
            2,2,0,1,
            2,2,2,0
        },
    },
};
void    Partition(
    int       shape,
    float    in[][MAX_DIMENSION_BIG],
    float    subsets[MAX_SUBSETS][MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],
    int       count[MAX_SUBSETS],
    int       ShapeTableToUse,
    int       dimension) {
    int   i, j;
    int   *table = NULL;

    // Dont use memset: this is better for now
    for (i = 0; i<MAX_SUBSETS; i++) count[i] = 0;

    switch (ShapeTableToUse) {
    case    0:
    case    1:
        table = &(PARTITIONS[0][0][0]);
        break;
    case    2:
        table = &(PARTITIONS[1][shape][0]);
        break;
    default:
        break;
    }

    // Nothing to do!!: Must indicate an error to user
    if (table == NULL) return;

    for (i = 0; i<MAX_SUBSET_SIZE; i++) {
        int   subset = table[i];
        for (j = 0; j<dimension; j++) {
            subsets[subset][count[subset]][j] = in[i][j];
        }
        if (dimension < MAX_DIMENSION_BIG) {
            subsets[subset][count[subset]][j] = 0.0;
        }
        count[subset]++;
    }
}

//=================================================================================================

void GetEndPoints(float EndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], float outB[MAX_SUBSETS][MAX_SUBSET_SIZE][MAX_DIMENSION_BIG], int max_subsets, int entryCount[MAX_SUBSETS]) {
    // Should have some sort of error notification!
    if (max_subsets > MAX_SUBSETS) return;

    // Save Min and Max OutB points as EndPoints
    for (int subset = 0; subset<max_subsets; subset++) {
        // We now have points on direction vector(s)
        // find the min and max points
        float min = FLT_MAX;
        float max = 0;
        float val;
        int mini = 0;
        int maxi = 0;


        for (int i = 0; i < entryCount[subset]; i++) {
            val = outB[subset][i][0] + outB[subset][i][1] + outB[subset][i][2];
            if (val < min) {
                min = val;
                mini = i;
            }
            if (val > max) {
                max = val;
                maxi = i;
            }
        }

        // Is round best for this !
        for (int c = 0; c < MAX_DIMENSION_BIG; c++) {
            EndPoints[subset][0][c] = outB[subset][mini][c];
        }

        for (int c = 0; c < MAX_DIMENSION_BIG; c++) {
            EndPoints[subset][1][c] = outB[subset][maxi][c];
        }
    }
}

void covariance_d(float data[][MAX_DIMENSION_BIG], int numEntries, float cov[MAX_DIMENSION_BIG][MAX_DIMENSION_BIG], int dimension) {
    int i, j, k;

    for (i = 0; i<dimension; i++)
        for (j = 0; j <= i; j++) {
            cov[i][j] = 0;
            for (k = 0; k<numEntries; k++)
                cov[i][j] += data[k][i] * data[k][j];
        }

    for (i = 0; i<dimension; i++)
        for (j = i + 1; j<dimension; j++)
            cov[i][j] = cov[j][i];
}

void centerInPlace_d(float data[][MAX_DIMENSION_BIG], int numEntries, float mean[MAX_DIMENSION_BIG], int dimension) {
    int i, k;

    for (i = 0; i<dimension; i++) {
        mean[i] = 0;
        for (k = 0; k<numEntries; k++)
            mean[i] += data[k][i];
    }

    if (!numEntries)
        return;

    for (i = 0; i<dimension; i++) {
        mean[i] /= (float)numEntries;
        for (k = 0; k<numEntries; k++)
            data[k][i] -= mean[i];
    }
}

void eigenVector_d(float cov[MAX_DIMENSION_BIG][MAX_DIMENSION_BIG], float vector[MAX_DIMENSION_BIG], int dimension) {
    // calculate an eigenvecto corresponding to a biggest eigenvalue
    // will work for non-zero non-negative matricies only

#define EV_ITERATION_NUMBER 20
#define EV_SLACK            2        /* additive for exp base 2)*/


    int i, j, k, l, m, n, p, q;
    float c[2][MAX_DIMENSION_BIG][MAX_DIMENSION_BIG];
    float maxDiag;

    for (i = 0; i<dimension; i++)
        for (j = 0; j<dimension; j++)
            c[0][i][j] = cov[i][j];

    p = (int)floorf(logf((HDR_FLT_MAX_EXP - EV_SLACK) / ceilf(logf((float)dimension) / logf(2.0f))) / logf(2.0f));

    //assert(p>0);

    p = p >0 ? p : 1;

    q = (EV_ITERATION_NUMBER + p - 1) / p;

    l = 0;

    for (n = 0; n<q; n++) {
        maxDiag = 0;

        for (i = 0; i<dimension; i++)
            maxDiag = c[l][i][i] > maxDiag ? c[l][i][i] : maxDiag;

        if (maxDiag <= 0) {
            return;
        }

        //assert(maxDiag >0);

        for (i = 0; i<dimension; i++)
            for (j = 0; j<dimension; j++)
                c[l][i][j] /= maxDiag;

        for (m = 0; m<p; m++) {
            for (i = 0; i<dimension; i++)
                for (j = 0; j<dimension; j++) {
                    float temp = 0;
                    for (k = 0; k<dimension; k++) {
                        // Notes:
                        // This is the most consuming portion of the code and needs optimizing for perfromance
                        temp += c[l][i][k] * c[l][k][j];
                    }
                    c[1 - l][i][j] = temp;
                }
            l = 1 - l;
        }
    }

    maxDiag = 0;
    k = 0;

    for (i = 0; i<dimension; i++) {
        k = c[l][i][i] > maxDiag ? i : k;
        maxDiag = c[l][i][i] > maxDiag ? c[l][i][i] : maxDiag;
    }
    float t;
    t = 0;
    for (i = 0; i<dimension; i++) {
        t += c[l][k][i] * c[l][k][i];
        vector[i] = c[l][k][i];
    }
    // normalization is really optional
    t = sqrtf(t);
    //assert(t>0);

    if (t <= 0) {
        return;
    }
    for (i = 0; i<dimension; i++)
        vector[i] /= t;
}

void project_d(float data[][MAX_DIMENSION_BIG], int numEntries, float vector[MAX_DIMENSION_BIG], float projection[MAX_ENTRIES], int dimension) {
    // assume that vector is normalized already
    int i, k;

    for (k = 0; k<numEntries; k++) {
        projection[k] = 0;
        for (i = 0; i<dimension; i++) {
            projection[k] += data[k][i] * vector[i];
        }
    }
}

typedef struct {
    float d;
    int i;
} a;

inline int a_compare(const void *arg1, const void *arg2) {
    if (((a*)arg1)->d - ((a*)arg2)->d > 0) return 1;
    if (((a*)arg1)->d - ((a*)arg2)->d < 0) return -1;
    return 0;
};

void sortProjection(float projection[MAX_ENTRIES], int order[MAX_ENTRIES], int numEntries) {
    int i;
    a what[MAX_ENTRIES + MAX_PARTITIONS_TABLE];

    for (i = 0; i < numEntries; i++)
        what[what[i].i = i].d = projection[i];

    qsort((void*)&what, numEntries, sizeof(a), a_compare);

    for (i = 0; i < numEntries; i++)
        order[i] = what[i].i;
};

float totalError_d(float data[MAX_ENTRIES][MAX_DIMENSION_BIG], float data2[MAX_ENTRIES][MAX_DIMENSION_BIG], int numEntries, int dimension) {
    int i, j;
    float t = 0;
    for (i = 0; i<numEntries; i++)
        for (j = 0; j<dimension; j++)
            t += (data[i][j] - data2[i][j])*(data[i][j] - data2[i][j]);

    return t;
};

// input:
//
// v_  points, might be uncentered
// k - number of points in the ramp
// n - number of points in v_
//
// output:
// index, uncentered, in the range 0..k-1
//
void quant_AnD_Shell(float* v_, int k, int n, int *idx) {
#define MAX_BLOCK MAX_ENTRIES
    int i, j;
    float v[MAX_BLOCK];
    float z[MAX_BLOCK];
    a d[MAX_BLOCK];
    float l;
    float mm;
    float r = 0;
    int mi;

    //assert((v_ != NULL) && (n>1) && (k>1));

    float m, M, s, dm = 0.;
    m = M = v_[0];

    for (i = 1; i < n; i++) {
        m = m < v_[i] ? m : v_[i];
        M = M > v_[i] ? M : v_[i];
    }
    if (M == m) {
        for (i = 0; i < n; i++)
            idx[i] = 0;
        return;
    }

    //assert(M - m >0);
    s = (k - 1) / (M - m);
    for (i = 0; i < n; i++) {
        v[i] = v_[i] * s;

        idx[i] = (int)(z[i] = (v[i] + 0.5f /* stabilizer*/ - m *s));  //floorf(v[i] + 0.5f /* stabilizer*/ - m *s));

        d[i].d = v[i] - z[i] - m *s;
        d[i].i = i;
        dm += d[i].d;
        r += d[i].d*d[i].d;
    }
    if (n*r - dm*dm >= (float)(n - 1) / 4 /*slack*/ / 2) {

        dm /= (float)n;

        for (i = 0; i < n; i++)
            d[i].d -= dm;

        qsort((void*)&d, n, sizeof(a), a_compare);

        // got into fundamental simplex
        // move coordinate system origin to its center
        for (i = 0; i < n; i++)
            d[i].d -= (2.0f*(float)i + 1.0f - (float)n) / 2.0f / (float)n;

        mm = l = 0.;
        j = -1;
        for (i = 0; i < n; i++) {
            l += d[i].d;
            if (l < mm) {
                mm = l;
                j = i;
            }
        }

        // position which should be in 0
        j = ++j % n;

        for (i = j; i < n; i++)
            idx[d[i].i]++;
    }
    // get rid of an offset in idx
    mi = idx[0];
    for (i = 1; i < n; i++)
        mi = mi < idx[i] ? mi : idx[i];

    for (i = 0; i < n; i++)
        idx[i] -= mi;
}

float optQuantAnD_d(
    float data[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int numEntries,
    int numClusters,
    int index[MAX_ENTRIES],
    float out[MAX_ENTRIES][MAX_DIMENSION_BIG],
    float direction[MAX_DIMENSION_BIG], float *step,
    int dimension,
    float quality
) {
    int index_[MAX_ENTRIES];

    int maxTry = (int)(MAX_TRY * quality);
    int try_two = 50;

    int i, j, k;
    float t, s;

    float centered[MAX_ENTRIES][MAX_DIMENSION_BIG];

    float mean[MAX_DIMENSION_BIG];

    float cov[MAX_DIMENSION_BIG][MAX_DIMENSION_BIG];

    float projected[MAX_ENTRIES];

    int order_[MAX_ENTRIES];


    for (i = 0; i<numEntries; i++)
        for (j = 0; j<dimension; j++)
            centered[i][j] = data[i][j];

    centerInPlace_d(centered, numEntries, mean, dimension);
    covariance_d(centered, numEntries, cov, dimension);

    // check if they all are the same

    t = 0;
    for (j = 0; j<dimension; j++)
        t += cov[j][j];

    if (numEntries == 0) {
        for (i = 0; i<numEntries; i++) {
            index[i] = 0;
            for (j = 0; j<dimension; j++)
                out[i][j] = mean[j];
        }
        return 0.;
    }

    eigenVector_d(cov, direction, dimension);
    project_d(centered, numEntries, direction, projected, dimension);

    for (i = 0; i<maxTry; i++) {
        int done = 0;

        if (i) {
            do {
                float q;
                q = s = t = 0;

                for (k = 0; k<numEntries; k++) {
                    s += index[k];
                    t += index[k] * index[k];
                }

                for (j = 0; j<dimension; j++) {
                    direction[j] = 0;
                    for (k = 0; k<numEntries; k++)
                        direction[j] += centered[k][j] * index[k];
                    q += direction[j] * direction[j];

                }

                s /= (float)numEntries;
                t = t - s * s * (float)numEntries;
                //assert(t != 0);
                t = (t == 0.0f ? 0.0f : 1.0f / t);
                // We need to requantize

                q = sqrtf(q);
                t *= q;

                if (q != 0)
                    for (j = 0; j<dimension; j++)
                        direction[j] /= q;

                // direction normalized

                project_d(centered, numEntries, direction, projected, dimension);
                sortProjection(projected, order_, numEntries);

                int index__[MAX_ENTRIES];

                // it's projected and centered; cluster centers are (index[i]-s)*t (*dir)
                k = 0;
                for (j = 0; j < numEntries; j++) {
                    while (projected[order_[j]] >(k + 0.5 - s)*t  && k < numClusters - 1)
                        k++;
                    index__[order_[j]] = k;
                }
                done = 1;
                for (j = 0; j < numEntries; j++) {
                    done = (done && (index__[j] == index[j]));
                    index[j] = index__[j];
                }
            } while (!done && try_two--);

            if (i == 1)
                for (j = 0; j < numEntries; j++)
                    index_[j] = index[j];
            else {
                done = 1;
                for (j = 0; j < numEntries; j++) {
                    done = (done && (index_[j] == index[j]));
                    index_[j] = index_[j];
                }
                if (done)
                    break;

            }
        }

        quant_AnD_Shell(projected, numClusters, numEntries, index);
    }
    s = t = 0;

    float q = 0;

    for (k = 0; k<numEntries; k++) {
        s += index[k];
        t += index[k] * index[k];
    }

    for (j = 0; j<dimension; j++) {
        direction[j] = 0;
        for (k = 0; k<numEntries; k++)
            direction[j] += centered[k][j] * index[k];
        q += direction[j] * direction[j];
    }

    s /= (float)numEntries;

    t = t - s * s * (float)numEntries;

    //assert(t != 0);

    t = (t == 0.0 ? 0.0f : 1.0f / t);

    for (i = 0; i<numEntries; i++)
        for (j = 0; j<dimension; j++)
            out[i][j] = mean[j] + direction[j] * t*(index[i] - s);

    // normalize direction for output

    q = sqrtf(q);
    *step = t*q;
    for (j = 0; j<dimension; j++)
        direction[j] /= q;

    return totalError_d(data, out, numEntries, dimension);
}


//=====================================================================================================================
#define LOG_CL_BASE         2
#define BIT_BASE            5
#define LOG_CL_RANGE        5
#define BIT_RANGE           9
#define MAX_CLUSTERS_BIG    16
#define BTT(bits)           (bits-BIT_BASE)
#define CLT(cl)             (cl-LOG_CL_BASE)

const float rampLerpWeights[5][16] = {
    { 0.0 }, // 0 bit index
    { 0.0, 1.0 }, // 1 bit index
    { 0.0, 21.0 / 64.0, 43.0 / 64.0, 1.0 }, // 2 bit index
    { 0.0, 9.0 / 64.0, 18.0 / 64.0, 27.0 / 64.0, 37.0 / 64.0, 46.0 / 64.0, 55.0 / 64.0, 1.0 }, // 3 bit index
    {
        0.0, 4.0 / 64.0, 9.0 / 64.0, 13.0 / 64.0, 17.0 / 64.0, 21.0 / 64.0, 26.0 / 64.0, 30.0 / 64.0,
        34.0 / 64.0, 38.0 / 64.0, 43.0 / 64.0, 47.0 / 64.0, 51.0 / 64.0, 55.0 / 64.0, 60.0 / 64.0, 1.0
    } // 4 bit index
};

float rampf(int clog, int bits, float p1, float p2, int indexPos) {
    (bits);
    // (clog+ LOG_CL_BASE) starts from 2 to 4
    return  (float)p1 + rampLerpWeights[clog + LOG_CL_BASE][indexPos] * (p2 - p1);
}

int all_same_d(float d[][MAX_DIMENSION_BIG], int n, int dimension) {
    int i, j;
    int same = 1;
    for (i = 1; i< n; i++)
        for (j = 0; j< dimension; j++)
            same = same && (d[0][j] == d[i][j]);

    return(same);
}

// return the max index from a set of indexes
int max_index(int a[], int n) {
    int i, m = a[0];
    for (i = 0; i< n; i++)
        m = m > a[i] ? m : a[i];
    return (m);
}

int cluster_mean_d_d(float d[MAX_ENTRIES][MAX_DIMENSION_BIG], float mean[MAX_ENTRIES][MAX_DIMENSION_BIG], int index[], int i_comp[], int i_cnt[], int n, int dimension) {
    // unused index values are underfined
    int i, j, k;
    //assert(n!=0);

    for (i = 0; i< n; i++)
        for (j = 0; j< dimension; j++) {
            // assert(index[i]<MAX_CLUSTERS_BIG);
            mean[index[i]][j] = 0;
            i_cnt[index[i]] = 0;
        }
    k = 0;
    for (i = 0; i< n; i++) {
        for (j = 0; j< dimension; j++)
            mean[index[i]][j] += d[i][j];
        if (i_cnt[index[i]] == 0)
            i_comp[k++] = index[i];
        i_cnt[index[i]]++;
    }

    for (i = 0; i< k; i++)
        for (j = 0; j< dimension; j++)
            mean[i_comp[i]][j] /= (float)i_cnt[i_comp[i]];
    return k;
}

void mean_d_d(float d[][MAX_DIMENSION_BIG], float mean[MAX_DIMENSION_BIG], int n, int dimension) {
    int i, j;
    for (j = 0; j< dimension; j++)
        mean[j] = 0;
    for (i = 0; i< n; i++)
        for (j = 0; j< dimension; j++)
            mean[j] += d[i][j];
    for (j = 0; j< dimension; j++)
        mean[j] /= (float)n;
}

void index_collapse_kernel(int index[], int numEntries) {
    int k;
    int d, D;
    int mi;
    int Mi;
    if (numEntries == 0)
        return;

    mi = Mi = index[0];
    for (k = 1; k<numEntries; k++) {
        mi = mi < index[k] ? mi : index[k];
        Mi = Mi > index[k] ? Mi : index[k];
    }
    D = 1;
    for (d = 2; d <= Mi - mi; d++) {

        for (k = 0; k<numEntries; k++)
            if ((index[k] - mi) % d != 0)
                break;
        if (k >= numEntries)
            D = d;
    }
    for (k = 0; k<numEntries; k++)
        index[k] = (index[k] - mi) / D;
}


//========================================================================================================================
//-------------------------------------------------------------------------------------------------------------------------

int max_i(int a[], int n) {
    int i, m = a[0];
    for (i = 0; i< n; i++)
        m = m > a[i] ? m : a[i];
    return (m);
}

int npv_nd[2][2 * MAX_DIMENSION_BIG] = {
    { 1,2,4,8,16,32,0,0 }, //dimension = 3
    { 1,2,4,0,0,0,0,0 }    //dimension = 4
};

short par_vectors_nd[2][8][128][2][MAX_DIMENSION_BIG] = {
    {
        // Dimension = 3
        {
            { { 0,0,0,0 },{ 0,0,0,0 } },
            { { 0,0,0,0 },{ 0,0,0,0 } }
        },

        // 3*n+1    BCC          3*n+1        Cartesian 3*n            //same parity
        {
            // SAME_PAR
            { { 0,0,0 },{ 0,0,0 } },
            { { 1,1,1 },{ 1,1,1 } }
        },
        // 3*n+2    BCC          3*n+1        BCC          3*n+1
        {
            // BCC
            { { 0,0,0 },{ 0,0,0 } },
            { { 0,0,0 },{ 1,1,1 } },
            { { 1,1,1 },{ 0,0,0 } },
            { { 1,1,1 },{ 1,1,1 } }
        },
        // 3*n+3    FCC                    ???                        // ??????
        // BCC with FCC same or inverted, symmetric
        {
            // BCC_SAME_FCC
            { { 0,0,0 },{ 0,0,0 } },
            { { 1,1,0 },{ 1,1,0 } },
            { { 1,0,1 },{ 1,0,1 } },
            { { 0,1,1 },{ 0,1,1 } },

            { { 0,0,0 },{ 1,1,1 } },
            { { 1,1,1 },{ 0,0,0 } },
            { { 0,1,0 },{ 0,1,0 } },  // ??
            { { 1,1,1 },{ 1,1,1 } },

        },
        // 3*n+4    FCC          3*n+2        FCC          3*n+2
        {

            { { 0,0,0 },{ 0,0,0 } },
            { { 1,1,0 },{ 0,0,0 } },
            { { 1,0,1 },{ 0,0,0 } },
            { { 0,1,1 },{ 0,0,0 } },

            { { 0,0,0 },{ 1,1,0 } },
            { { 1,1,0 },{ 1,1,0 } },
            { { 1,0,1 },{ 1,1,0 } },
            { { 0,1,1 },{ 1,1,0 } },

            { { 0,0,0 },{ 1,0,1 } },
            { { 1,1,0 },{ 1,0,1 } },
            { { 1,0,1 },{ 1,0,1 } },
            { { 0,1,1 },{ 1,0,1 } },

            { { 0,0,0 },{ 0,1,1 } },
            { { 1,1,0 },{ 0,1,1 } },
            { { 1,0,1 },{ 0,1,1 } },
            { { 0,1,1 },{ 0,1,1 } }
        },


        // 3*n+5    Cartesian 3*n+3        FCC          3*n+2            //D^*[6]
        {

            { { 0,0,0 },{ 0,0,0 } },
            { { 1,1,0 },{ 0,0,0 } },
            { { 1,0,1 },{ 0,0,0 } },
            { { 0,1,1 },{ 0,0,0 } },

            { { 0,0,0 },{ 1,1,0 } },
            { { 1,1,0 },{ 1,1,0 } },
            { { 1,0,1 },{ 1,1,0 } },
            { { 0,1,1 },{ 1,1,0 } },

            { { 0,0,0 },{ 1,0,1 } },
            { { 1,1,0 },{ 1,0,1 } },
            { { 1,0,1 },{ 1,0,1 } },
            { { 0,1,1 },{ 1,0,1 } },

            { { 0,0,0 },{ 0,1,1 } },
            { { 1,1,0 },{ 0,1,1 } },
            { { 1,0,1 },{ 0,1,1 } },
            { { 0,1,1 },{ 0,1,1 } },


            { { 1,0,0 },{ 1,1,1 } },
            { { 0,1,0 },{ 1,1,1 } },
            { { 0,0,1 },{ 1,1,1 } },
            { { 1,1,1 },{ 1,1,1 } },

            { { 1,0,0 },{ 0,0,1 } },
            { { 0,1,0 },{ 0,0,1 } },
            { { 0,0,1 },{ 0,0,1 } },
            { { 1,1,1 },{ 0,0,1 } },

            { { 1,0,0 },{ 1,0,0 } },
            { { 0,1,0 },{ 1,0,0 } },
            { { 0,0,1 },{ 1,0,0 } },
            { { 1,1,1 },{ 1,0,0 } },

            { { 1,0,0 },{ 0,1,0 } },
            { { 0,1,0 },{ 0,1,0 } },
            { { 0,0,1 },{ 0,1,0 } },
            { { 1,1,1 },{ 0,1,0 } }
        }
    },// Dimension = 3
    {
        // Dimension = 4
        {
            { { 0,0,0,0 },{ 0,0,0,0 } },
            { { 0,0,0,0 },{ 0,0,0,0 } }
        },

        // 3*n+1    BCC          3*n+1        Cartesian 3*n            //same parity
        {
            // SAME_PAR
            { { 0,0,0,0 },{ 0,0,0,0 } },
            { { 1,1,1,1 },{ 1,1,1,1 } }
        },
        // 3*n+2    BCC          3*n+1        BCC          3*n+1
        {
            // BCC
            { { 0,0,0,0 },{ 0,0,0,0 } },
            { { 0,0,0,0 },{ 1,1,1,1 } },
            { { 1,1,1,1 },{ 0,0,0,0 } },
            { { 1,1,1,1 },{ 1,1,1,1 } }
        },
        // 3 PBIT
        {
            { { 0,0,0,0 },{ 0,0,0,0 } },
            { { 0,0,0,0 },{ 0,1,1,1 } },
            { { 0,1,1,1 },{ 0,0,0,0 } },
            { { 0,1,1,1 },{ 0,1,1,1 } },

            { { 1,0,0,0 },{ 1,0,0,0 } },
            { { 1,0,0,0 },{ 1,1,1,1 } },
            { { 1,1,1,1 },{ 1,0,0,0 } },
            { { 1,1,1,1 },{ 1,1,1,1 } }
        },

        // 4 PBIT
        {
            { { 0,0,0,0 },{ 0,0,0,0 } },
            { { 0,0,0,0 },{ 0,1,1,1 } },
            { { 0,1,1,1 },{ 0,0,0,0 } },
            { { 0,1,1,1 },{ 0,1,1,1 } },

            { { 1,0,0,0 },{ 1,0,0,0 } },
            { { 1,0,0,0 },{ 1,1,1,1 } },
            { { 1,1,1,1 },{ 1,0,0,0 } },
            { { 1,1,1,1 },{ 1,1,1,1 } },

            { { 0,0,0,0 },{ 0,0,0,0 } },
            { { 0,0,0,0 },{ 0,0,1,1 } },
            { { 0,0,1,1 },{ 0,0,0,0 } },
            { { 0,1,0,1 },{ 0,1,0,1 } },

            { { 1,0,0,0 },{ 1,0,0,0 } },
            { { 1,0,0,0 },{ 1,0,1,1 } },
            { { 1,0,1,1 },{ 1,0,0,0 } },
            { { 1,1,0,1 },{ 1,1,0,1 } },

        },

    } // Dimension = 4

};

int get_par_vector(int dim1, int dim2, int dim3, int dim4, int dim5) {
    return par_vectors_nd[dim1][dim2][dim3][dim4][dim5];
}



float quant_single_point_d
(
    float data[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int numEntries, int index[MAX_ENTRIES],
    float out[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int epo_1[2][MAX_DIMENSION_BIG],
    int Mi_,                // last cluster
    int bits[3],            // including parity
    int type,
    int dimension           // This should be either 3 or 4
) {
    if (dimension < 3) return FLT_MAX;

    int i, j;

    float err_0 = FLT_MAX;
    float err_1 = FLT_MAX;

    int idx = 0;
    int idx_1 = 0;

    int epo_0[2][MAX_DIMENSION_BIG];

    int use_par = (type != 0);

    int clog = 0;
    i = Mi_ + 1;
    while (i >>= 1)
        clog++;

    //    assert((1<<clog)== Mi_+1);

    int pn;
    for (pn = 0; pn<npv_nd[dimension - 3][type]; pn++) {
        //1

        int dim1 = dimension - 3;
        int dim2 = type;
        int dim3 = pn;


        int o1[2][MAX_DIMENSION_BIG]; // = { 0,2 };
        int o2[2][MAX_DIMENSION_BIG]; // = { 0,2 };

        for (j = 0; j<dimension; j++) {
            //A
            o2[0][j] = o1[0][j] = 0;
            o2[1][j] = o1[1][j] = 2;

            if (use_par) {
                if (get_par_vector(dim1, dim2, dim3, 0, j))
                    o1[0][j] = 1;
                else
                    o1[1][j] = 1;
                if (get_par_vector(dim1, dim2, dim3, 1, j))
                    o2[0][j] = 1;
                else
                    o2[1][j] = 1;
            }
        } //A

        int t1, t2;

        int dr[MAX_DIMENSION_BIG];
        int dr_0[MAX_DIMENSION_BIG];
        //float tr;

        for (i = 0; i< (1 << clog); i++) {
            //E
            float t = 0;
            int t1o[MAX_DIMENSION_BIG], t2o[MAX_DIMENSION_BIG];

            for (j = 0; j<dimension; j++) {
                // D
                float t_ = FLT_MAX;

                for (t1 = o1[0][j]; t1<o1[1][j]; t1++) {
                    // C
                    for (t2 = o2[0][j]; t2<o2[1][j]; t2++)
                        // This is needed for non-integer mean points of "collapsed" sets
                    {
                        // B

#ifdef USE_RAMPS
                        int tf = (int)floorf(data[0][j]);
                        int tc = (int)ceilf(data[0][j]);
                        // if they are not equal, the same representalbe point is used for
                        // both of them, as all representable points are integers in the rage
                        if (sperr(tf, CLT(clog), BTT(bits[j]), t1, t2, i) > sperr(tc, CLT(clog), BTT(bits[j]), t1, t2, i))
                            dr[j] = tc;
                        else if (sperr(tf, CLT(clog), BTT(bits[j]), t1, t2, i) < sperr(tc, CLT(clog), BTT(bits[j]), t1, t2, i))
                            dr[j] = tf;
                        else
#endif
                            dr[j] = (int)floorf(data[0][j] + 0.5f);

#ifdef USE_RAMPS
                        tr = sperr(dr[j], CLT(clog), BTT(bits[j]), t1, t2, i) + 2.0f * sqrtf(sperr(dr[j], CLT(clog), BTT(bits[j]), t1, t2, i)) * fabsf((float)dr[j] - data[0][j]) +
                             (dr[j] - data[0][j])* (dr[j] - data[0][j]);
                        if (tr < t_) {
                            t_ = tr;
#else
                        t_ = 0;
#endif

                            t1o[j] = t1;
                            t2o[j] = t2;
                            dr_0[j] = dr[j];
#ifdef USE_RAMPS
                            if ((dr_0[j] < 0) || (dr_0[j] > 255)) {
                                dr_0[j] = 0; // Error!
                            }
                        }
#endif
                    } // B
                } //C

                t += t_;
            } // D


            if (t < err_0) {

                idx = i;

                for (j = 0; j<dimension; j++) {
#ifdef USE_RAMPS
                    int p1 = CLT(clog);        // < 3
                    int p2 = BTT(bits[j]);     // < 4
                    int in_data = dr_0[j];          // < SP_ERRIDX_MAX
                    int p4 = t1o[j];           // < 2
                    int p5 = t2o[j];           // < 2
                    int p6 = i;                // < 16

                    // New spidx
                    epo_0[0][j] = spidx(in_data, p1, p2, p4, p5, p6, 0);
                    epo_0[1][j] = spidx(in_data, p1, p2, p4, p5, p6, 1);

                    if (epo_0[1][j] >= SP_ERRIDX_MAX) {
                        epo_0[1][j] = 0; // Error!!
                    }
#else
                    epo_0[0][j] = 0;
                    epo_0[1][j] = 0;
#endif
                }
                err_0 = t;
            }
            if (err_0 == 0)
                break;
        } // E

        if (err_0 < err_1) {
            idx_1 = idx;
            for (j = 0; j<dimension; j++) {
                epo_1[0][j] = epo_0[0][j];
                epo_1[1][j] = epo_0[1][j];
            }
            err_1 = err_0;
        }

        if (err_1 == 0)
            break;
    } //1

    for (i = 0; i< numEntries; i++) {
        index[i] = idx_1;
        for (j = 0; j<dimension; j++) {
            int p1 = CLT(clog);        // < 3
            int p2 = BTT(bits[j]);     // < 4
            int p3 = epo_1[0][j];      // < SP_ERRIDX_MAX
            int p4 = epo_1[1][j];      // < SP_ERRIDX_MAX
            int p5 = idx_1;            // < 16
#ifndef USE_NEWRAMP
            out[i][j] = ramp[p1][p2][p3][p4][p5];
#else
#pragma warning( push )
#pragma warning(disable:4244)
            out[i][j] = (int)rampf(p1, p2, p3, p4, p5);
#pragma warning( pop )
#endif
        }
    }
    return err_1 * numEntries;
}

#define SP_ERRIDX_MAX 256

int expandbits_(int bits, int v) {
    return (v << (8 - bits) | v >> (2 * bits - 8));
}


#ifndef USE_NEWRAMP
float    ep_d[4][SP_ERRIDX_MAX];
float    ramp[3][4][SP_ERRIDX_MAX][SP_ERRIDX_MAX][16];
#else
float ep_df(int bits, int p1) {
    return (float)expandbits_(bits + BIT_BASE, p1);
}

float rampf(int clog, int bits, int p1, int p2, int i) {
    // (clog+ LOG_CL_BASE) starts from 2 to 4
    float ret = floorf((float)ep_df(bits, p1) + rampLerpWeights[clog + LOG_CL_BASE][i] * (float)((ep_df(bits, p2) - ep_df(bits, p1))) + 0.5F);
    if (ret > SP_ERRIDX_MAX) return SP_ERRIDX_MAX - 1;
    return ret;
}
#endif

#ifdef USE_RAMPS

int spidx(int in_data, int in_clog, int in_bits, int in_p2, int in_o1, int in_o2, int in_i) {
    return sp_data[in_data].sp_idx[in_clog][in_bits][in_p2][in_o1][in_o2][in_i];
}

float sperr(int in_data, int clog, int bits, int p2, int o1, int o2) {
    return sp_data[in_data].sp_err[clog][bits][p2][o1][o2];
}
#endif

void init_ramps() {
#ifdef USE_RAMPS
    int clog, bits;
    int in_data; // p1;
    int p2;
    int i;
    int o1, o2;

    if (g_init_ramps > 0) {
        g_init_ramps++;
        return;
    }

    mtx.lock();


    // sp_datap = (SP_DATA **)malloc(SP_ERRIDX_MAX*sizeof(struct SP_DATA));
    // assert(sp_datap);
    // for (int i = 0; i < SP_ERRIDX_MAX; i++)
    // {
    //     sp_datap[i] = (SP_DATA *)malloc(sizeof(struct SP_DATA));
    // }

#ifndef USE_NEWRAMP

    for (bits = BIT_BASE; bits < BIT_RANGE; bits++)
        for (p1 = 0; p1 < (1 << bits); p1++) {
            ep_d[BTT(bits)][p1] = (float)expandbits_(bits, p1);
        }


    for (clog = LOG_CL_BASE; clog < LOG_CL_RANGE; clog++)
        for (bits = BIT_BASE; bits < BIT_RANGE; bits++)
            for (p1 = 0; p1 < (1 << bits); p1++)
                for (p2 = 0; p2 < (1 << bits); p2++) {
                    for (o1 = 0; o1 < (1 << clog); o1++) {
                        ramp[CLT(clog)][BTT(bits)][p1][p2][o1] =
                            floorf((float)ep_d[BTT(bits)][p1] + rampLerpWeights[clog][o1] * (float)((ep_d[BTT(bits)][p2] - ep_d[BTT(bits)][p1])) + 0.5F);
                    }
                }
#endif

    //-----------------------------------------------------------------------------
    // Step 1

    for (clog = LOG_CL_BASE; clog<LOG_CL_RANGE; clog++)
        for (bits = BIT_BASE; bits<BIT_RANGE; bits++)
            for (in_data = 0; in_data<SP_ERRIDX_MAX; in_data++)
                for (o1 = 0; o1<2; o1++)
                    for (o2 = 0; o2<2; o2++)
                        for (i = 0; i<16; i++) {
                            sp_data[in_data].sp_err[CLT(clog)][BTT(bits)][o1][o2][i] = FLT_MAX;
                            sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][0] = -1;
                        }

    // Step 2
    for (clog = LOG_CL_BASE; clog<LOG_CL_RANGE; clog++)
        for (bits = BIT_BASE; bits<BIT_RANGE; bits++)
            for (in_data = 0; in_data<(1 << bits); in_data++)
                for (p2 = 0; p2<(1 << bits); p2++)
                    for (i = 0; i<(1 << clog); i++) {
#ifndef USE_NEWRAMP
                        sp_data[(int)ramp[clog].sp_idx[clog]bits][bits][p1][p2][o1]][p1 & 0x1][p2 & 0x1][o1][0] = p1;
                        sp_data[(int)ramp[clog].sp_idx[clog]bits][bits][p1][p2][o1]][p1 & 0x1][p2 & 0x1][o1][1] = p2;
                        sp_data[(int)ramp[clog].sp_err[clog]bits][bits][p1][p2][o1]][p1 & 0x1][p2 & 0x1][o1] = 0.;
#else
                        int spd_i = (int)rampf(CLT(clog), BTT(bits), in_data, p2, o1);
                        if (spd_i > SP_ERRIDX_MAX) spd_i = SP_ERRIDX_MAX - 1;

                        sp_data[spd_i].sp_idx[CLT(clog)][BTT(bits)][in_data & 0x1][p2 & 0x1][o1][0] = in_data;
                        sp_data[spd_i].sp_idx[CLT(clog)][BTT(bits)][in_data & 0x1][p2 & 0x1][o1][1] = p2;
                        sp_data[spd_i].sp_err[CLT(clog)][BTT(bits)][in_data & 0x1][p2 & 0x1][o1] = 0.;
#endif
                    }

    // Step 3
    for (clog = LOG_CL_BASE; clog<LOG_CL_RANGE; clog++)
        for (bits = BIT_BASE; bits<BIT_RANGE; bits++)
            for (in_data = 0; in_data<SP_ERRIDX_MAX; in_data++)
                for (o1 = 0; o1<2; o1++)
                    for (o2 = 0; o2<2; o2++)
                        for (i = 0; i<(1 << clog); i++)
                            if (sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][0]<0) {
                                int k;
                                for (k = 1; k<SP_ERRIDX_MAX; k++)
                                    if ((in_data - k >= 0 && sp_data[in_data - k].sp_err[CLT(clog)][BTT(bits)][o1][o2][i] == 0) ||
                                            (in_data + k < SP_ERRIDX_MAX && sp_data[in_data + k].sp_err[CLT(clog)][BTT(bits)][o1][o2][i] == 0))
                                        break;
                                {
                                    if ((in_data - k >= 0 && sp_data[in_data - k].sp_err[CLT(clog)][BTT(bits)][o1][o2][i] == 0)) {
                                        sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][0] = sp_data[in_data - k].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][0];
                                        sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][1] = sp_data[in_data - k].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][1];
                                        //printf("sp_data[%2d].sp_idx[%2d][%2d][%2d][%2d][%2d][0] = (%d)\n", in_data, CLT(clog), BTT(bits), o1, o2, i, sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][0]);
                                        //printf("sp_data[%2d].sp_idx[%2d][%2d][%2d][%2d][%2d][1] = (%d)\n", in_data, CLT(clog), BTT(bits), o1, o2, i, sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][1]);
                                    } else if ((in_data + k < SP_ERRIDX_MAX && sp_data[in_data + k].sp_err[CLT(clog)][BTT(bits)][o1][o2][i] == 0)) {
                                        sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][0] = sp_data[in_data + k].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][0];
                                        sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][1] = sp_data[in_data + k].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][1];
                                        //printf("sp_data[%2d].sp_idx[%2d][%2d][%2d][%2d][%2d][0] = %d\n", in_data, CLT(clog), BTT(bits), o1, o2, i, sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][0]);
                                        //printf("sp_data[%2d].sp_idx[%2d][%2d][%2d][%2d][%2d][1] = %d\n", in_data, CLT(clog), BTT(bits), o1, o2, i, sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][1]);
                                    }

                                    sp_data[in_data].sp_err[CLT(clog)][BTT(bits)][o1][o2][i] = (float)k*k;
                                }
                            }

    //for (clog = LOG_CL_BASE; clog<LOG_CL_RANGE; clog++)
    //    for (bits = BIT_BASE; bits<BIT_RANGE; bits++)
    //        for (in_data = 0; in_data<SP_ERRIDX_MAX; in_data++)
    //            for (o1 = 0; o1<2; o1++)
    //                for (o2 = 0; o2<2; o2++)
    //                    for (i = 0; i<16; i++)
    //                         {
    //                             printf("sp_data[%2d].sp_idx[%2d][%2d][%2d][%2d][%2d][0] = %d\n", in_data,CLT(clog), BTT(bits),o1,o2,i, sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][0]);
    //                             printf("sp_data[%2d].sp_idx[%2d][%2d][%2d][%2d][%2d][1] = %d\n", in_data,CLT(clog), BTT(bits),o1,o2,i, sp_data[in_data].sp_idx[CLT(clog)][BTT(bits)][o1][o2][i][1]);
    //                         }

    g_init_ramps++;

    mtx.unlock();

#endif
}

void deinit_ramps() {
#ifdef USE_RAMPS
    if (g_init_ramps > 1)
        g_init_ramps--;
#endif
}

int ep_find_floor(float v, int bits, int use_par, int odd) {
#ifndef USE_NEWRAMP
    float *p = ep_d[BTT(bits)];
#endif
    int i1 = 0;
    int i2 = 1 << (bits - use_par);
    odd = use_par ? odd : 0;
    while (i2 - i1>1) {
        int j = (i1 + i2) / 2;
#ifndef USE_NEWRAMP
        if (v >= p[(j << use_par) + odd])
#else
        if (v >= ep_df(BTT(bits), (j << use_par) + odd))
#endif
            i1 = j;
        else
            i2 = j;
    }
    return (i1 << use_par) + odd;
}



//based on code : ep_shaker_d in BC7 shaker
float ep_shaker_HD(
    float data[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int numEntries,
    int index_[MAX_ENTRIES],
    float out[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int epo_code[2][MAX_DIMENSION_BIG],
    int Mi_,                // last cluster
    int bits[3],            // including parity
    int dimension
) {
    int i, j, k;
    int use_par = 0;
    int clog = 0;

    i = Mi_ + 1;
    while (i >>= 1)
        clog++;

    float mean[MAX_DIMENSION_BIG];
    int index[MAX_ENTRIES];
    int Mi;

    int maxTry = 1;

    for (k = 0; k < numEntries; k++) {
        index[k] = index_[k];
    }

    int done;
    int change;

    int better;

    float   err_o = FLT_MAX;
    float   out_2[MAX_ENTRIES][MAX_DIMENSION_BIG];
    int     idx_2[MAX_ENTRIES];
    int     epo_2[2][MAX_DIMENSION_BIG];

    int max_bits[MAX_DIMENSION_BIG];
    int type = bits[0] % (2 * dimension);

    for (j = 0; j < dimension; j++)
        max_bits[j] = (bits[0] + 2 * dimension - 1) / (2 * dimension);


    // handled below automatically
    int alls = all_same_d(data, numEntries, dimension);

    mean_d_d(data, mean, numEntries, dimension);

    do {
        index_collapse_kernel(index, numEntries);

        Mi = max_index(index, numEntries);  // index can be from requantizer

        int p, q;
        int p0 = -1, q0 = -1;

        float err_2 = FLT_MAX;

        if (Mi == 0) {
            float t;
            int    epo_0[2][MAX_DIMENSION_BIG];
            // either sinle point from the beginning or collapsed index
            if (alls) {
                t = quant_single_point_d(data, numEntries, index, out_2, epo_0, Mi_, bits, type, dimension);
            } else {
                quant_single_point_d(&mean, numEntries, index, out_2, epo_0, Mi_, bits, type, dimension);
                t = totalError_d(data, out_2, numEntries, dimension);
            }

            if (t < err_o) {
                for (k = 0; k<numEntries; k++) {
                    index_[k] = index[k];
                    for (j = 0; j<dimension; j++) {
                        out[k][j] = out_2[k][j];
                        epo_code[0][j] = epo_0[0][j];
                        epo_code[1][j] = epo_0[1][j];
                    }
                };
                err_o = t;
            }
            return err_o;
        }



        for (q = 1; Mi != 0 && q*Mi <= Mi_; q++) { // does not work for single point collapsed index!!!
            for (p = 0; p <= Mi_ - q*Mi; p++) {
                int cidx[MAX_ENTRIES];

                for (k = 0; k<numEntries; k++) {
                    cidx[k] = index[k] * q + p;
                }

                float epa[2][MAX_DIMENSION_BIG];

                //
                // solve RMS problem for center
                //

                float im[2][2] = { { 0,0 },{ 0,0 } };   // matrix /inverse matrix
                float rp[2][MAX_DIMENSION_BIG];            // right part for RMS fit problem

                // get ideal clustr centers
                float cc[MAX_CLUSTERS_BIG][MAX_DIMENSION_BIG];
                int index_cnt[MAX_CLUSTERS_BIG];                        // count of index entries
                int index_comp[MAX_CLUSTERS_BIG];                       // compacted index
                int index_ncl;                                            // number of unique indexes

                index_ncl = cluster_mean_d_d(data, cc, cidx, index_comp, index_cnt, numEntries, dimension); // unrounded

                for (i = 0; i<index_ncl; i++)
                    for (j = 0; j<dimension; j++)
                        cc[index_comp[i]][j] = (float)floorf(cc[index_comp[i]][j] + 0.5f); // more or less ideal location

                for (j = 0; j<dimension; j++) {
                    rp[0][j] = rp[1][j] = 0;
                }

                // weight with cnt if runnning on compacted index
                for (k = 0; k<numEntries; k++) {
                    im[0][0] += (Mi_ - cidx[k])* (Mi_ - cidx[k]);
                    im[0][1] += cidx[k] * (Mi_ - cidx[k]);           // im is symmetric
                    im[1][1] += cidx[k] * cidx[k];

                    for (j = 0; j<dimension; j++) {
                        rp[0][j] += (Mi_ - cidx[k]) * cc[cidx[k]][j];
                        rp[1][j] += cidx[k] * cc[cidx[k]][j];
                    }
                }

                float dd = im[0][0] * im[1][1] - im[0][1] * im[0][1];

                //assert(dd !=0);

                // dd=0 means that cidx[k] and (Mi_-cidx[k]) collinear which implies only one active index;
                // taken care of separately

                im[1][0] = im[0][0];
                im[0][0] = im[1][1] / dd;
                im[1][1] = im[1][0] / dd;
                im[1][0] = im[0][1] = -im[0][1] / dd;

                for (j = 0; j<dimension; j++) {
                    epa[0][j] = (im[0][0] * rp[0][j] + im[0][1] * rp[1][j])*Mi_;
                    epa[1][j] = (im[1][0] * rp[0][j] + im[1][1] * rp[1][j])*Mi_;
                }

                float err_1 = FLT_MAX;
                float out_1[MAX_ENTRIES][MAX_DIMENSION_BIG];
                int idx_1[MAX_ENTRIES];
                int epo_1[2][MAX_DIMENSION_BIG];
                int s1 = 0;
                float epd[2][MAX_DIMENSION_BIG][2];   // first second, coord, begin range end range

                for (j = 0; j<dimension; j++) {
                    for (i = 0; i<2; i++) {
                        // set range
                        epd[i][j][0] = epd[i][j][1] = epa[i][j];
                        epd[i][j][1] += ((1 << bits[j]) - 1 - (int)epd[i][j][1]  < (1 << use_par) ?
                                         (1 << bits[j]) - 1 - (int)epd[i][j][1] : (1 << use_par)) & (~use_par);
                    }
                }

                float ce[MAX_ENTRIES][MAX_CLUSTERS_BIG][MAX_DIMENSION_BIG];
                float err_0 = 0;
                float out_0[MAX_ENTRIES][MAX_DIMENSION_BIG];
                int idx_0[MAX_ENTRIES];

                for (i = 0; i<numEntries; i++) {
                    float d[4];
                    d[0] = data[i][0];
                    d[1] = data[i][1];
                    d[2] = data[i][2];
                    d[3] = data[i][3];
                    for (j = 0; j<(1 << clog); j++)
                        for (k = 0; k < dimension; k++) {
                            ce[i][j][k] = (rampf(CLT(clog), BTT(bits[k]), epd[0][k][0], epd[1][k][0], j) - d[k])*
                                          (rampf(CLT(clog), BTT(bits[k]), epd[0][k][0], epd[1][k][0], j) - d[k]);
                        }
                }

                int s = 0, p1, g;
                int ei0 = 0, ei1 = 0;

                for (p1 = 0; p1<64; p1++) {
                    int j0 = 0;

                    // Gray code increment
                    g = p1 & (-p1);

                    err_0 = 0;

                    for (j = 0; j<dimension; j++) {
                        if (((g >> (2 * j)) & 0x3) != 0) {
                            j0 = j;
                            // new cords
                            ei0 = (((s^g) >> (2 * j)) & 0x1);
                            ei1 = (((s^g) >> (2 * j + 1)) & 0x1);
                        }
                    }
                    s = s ^ g;
                    err_0 = 0;

                    for (i = 0; i<numEntries; i++) {
                        float d[4];
                        d[0] = data[i][0];
                        d[1] = data[i][1];
                        d[2] = data[i][2];
                        d[3] = data[i][3];
                        int    ci = 0;
                        float cmin = FLT_MAX;

                        for (j = 0; j<(1 << clog); j++) {
                            float t_ = 0.;
                            ce[i][j][j0] = (rampf(CLT(clog), BTT(bits[j0]), epd[0][j0][ei0], epd[1][j0][ei1], j) - d[j0])*
                                           (rampf(CLT(clog), BTT(bits[j0]), epd[0][j0][ei0], epd[1][j0][ei1], j) - d[j0]);
                            for (k = 0; k<dimension; k++) {
                                t_ += ce[i][j][k];
                            }

                            if (t_< cmin) {
                                cmin = t_;
                                ci = j;
                            }
                        }

                        idx_0[i] = ci;
                        for (k = 0; k<dimension; k++) {
                            out_0[i][k] = rampf(CLT(clog), BTT(bits[k]), epd[0][k][ei0], epd[1][k][ei1], ci);
                        }
                        err_0 += cmin;
                    }

                    if (err_0 < err_1) {
                        // best in the curent ep cube run
                        for (i = 0; i < numEntries; i++) {
                            idx_1[i] = idx_0[i];
                            for (j = 0; j<dimension; j++)
                                out_1[i][j] = out_0[i][j];
                        }
                        err_1 = err_0;

                        s1 = s; // epo coding
                    }
                }

                // reconstruct epo
                for (j = 0; j<dimension; j++) {
                    {
                        // new cords
                        ei0 = ((s1 >> (2 * j)) & 0x1);
                        ei1 = ((s1 >> (2 * j + 1)) & 0x1);
                        epo_1[0][j] = (int)epd[0][j][ei0];
                        epo_1[1][j] = (int)epd[1][j][ei1];
                    }
                }

                if (err_1 < err_2) {
                    // best in the curent ep cube run
                    for (i = 0; i < numEntries; i++) {
                        idx_2[i] = idx_1[i];
                        for (j = 0; j<dimension; j++)
                            out_2[i][j] = out_1[i][j];
                    }
                    err_2 = err_1;
                    for (j = 0; j<dimension; j++) {
                        epo_2[0][j] = epo_1[0][j];
                        epo_2[1][j] = epo_1[1][j];
                    }
                    p0 = p;
                    q0 = q;
                }
            }
        }

        // change/better
        change = 0;
        for (k = 0; k<numEntries; k++)
            change = change || (index[k] * q0 + p0 != idx_2[k]);

        better = err_2 < err_o;

        if (better) {
            for (k = 0; k<numEntries; k++) {
                index_[k] = index[k] = idx_2[k];
                for (j = 0; j<dimension; j++) {
                    out[k][j] = out_2[k][j];
                    epo_code[0][j] = epo_2[0][j];
                    epo_code[1][j] = epo_2[1][j];
                }
            }
            err_o = err_2;
        }

        done = !(change  &&  better);

        if (maxTry > 0) maxTry--;
        else maxTry = 0;

    } while (!done && maxTry);

    return err_o;
}

float ep_shaker_2_d(
    float data[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int numEntries,
    int index_[MAX_ENTRIES],
    float out[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int epo_code[2][MAX_DIMENSION_BIG],
    int size,
    int Mi_,             // last cluster
    int bits,            // total for all channels
    // defined by total numbe of bits and dimensioin
    int dimension,
    float epo[2][MAX_DIMENSION_BIG]
) {
    if (dimension < 3) return FLT_MAX;

    int i, j, k;
    int max_bits[MAX_DIMENSION_BIG];
    int type = bits % (2 * dimension);
    int use_par = (type != 0);

    for (j = 0; j < dimension; j++)
        max_bits[j] = (bits + 2 * dimension - 1) / (2 * dimension);

    int clog = 0;
    i = Mi_ + 1;
    while (i >>= 1)
        clog++;

    if (CLT(clog) > 3)
        return FLT_MAX;

    float mean[MAX_DIMENSION_BIG];
    int index[MAX_ENTRIES];
    int Mi;

    int maxTry = 8;

    for (k = 0; k < numEntries; k++) {
        index[k] = index_[k];
    }

    int done;
    int change;

    int better;

    float err_o = FLT_MAX;

    int epo_0[2][MAX_DIMENSION_BIG];

    float outg[MAX_ENTRIES][MAX_DIMENSION_BIG];

    // handled below automatically
    int alls = all_same_d(data, numEntries, dimension);
    mean_d_d(data, mean, numEntries, dimension);

    do {
        index_collapse_kernel(index, numEntries);

        Mi = max_i(index, numEntries);  // index can be from requantizer

        int p, q;
        int p0 = -1, q0 = -1;

        float err_0 = FLT_MAX;

        if (Mi == 0) {
            float t;
            // either single point from the beginning or collapsed index
            if (alls) {
                t = quant_single_point_d(data, numEntries, index, outg, epo_0, Mi_, max_bits, type, dimension);
            } else {
                quant_single_point_d(&mean, numEntries, index, outg, epo_0, Mi_, max_bits, type, dimension);
                t = totalError_d(data, outg, numEntries, dimension);
            }

            if (t < err_o) {
                for (k = 0; k<numEntries; k++) {
                    index_[k] = index[k];
                    for (j = 0; j<dimension; j++) {
                        out[k][j] = outg[k][j];
                        epo_code[0][j] = epo_0[0][j];
                        epo_code[1][j] = epo_0[1][j];
                    }
                };
                err_o = t;
            }

            for (j = 0; j<dimension; j++) {
#ifndef USE_NEWRAMP
                epo[0][j] = ramp[CLT(clog)][BTT(max_bits[j])][epo_code[0][j]][epo_code[1][j]][0];
                epo[1][j] = ramp[CLT(clog)][BTT(max_bits[j])][epo_code[0][j]][epo_code[1][j]][(1 << clog) - 1];
#else
                epo[0][j] = rampf(CLT(clog), BTT(max_bits[j]), epo_code[0][j], epo_code[1][j], 0);
                epo[1][j] = rampf(CLT(clog), BTT(max_bits[j]), epo_code[0][j], epo_code[1][j], (1 << clog) - 1);
#endif
            }

            return err_o;
        }

        for (q = 1; Mi != 0 && q*Mi <= Mi_; q++) // does not work for single point collapsed index!!!
            for (p = 0; p <= Mi_ - q*Mi; p++) {
                int cidx[MAX_ENTRIES];

                for (k = 0; k<numEntries; k++)
                    cidx[k] = index[k] * q + p;

                float epa[2][MAX_DIMENSION_BIG];

                //
                // solve RMS problem for center
                //

                float im[2][2] = { { 0,0 },{ 0,0 } };   // matrix /inverse matrix
                float rp[2][MAX_DIMENSION_BIG];            // right part for RMS fit problem

                // get ideal clustr centers
                float cc[MAX_CLUSTERS_BIG][MAX_DIMENSION_BIG];
                int i_cnt[MAX_CLUSTERS_BIG]; // count of index entries
                int i_comp[MAX_CLUSTERS_BIG];   // compacted index
                int ncl;                        // number of unique indexes

                ncl = cluster_mean_d_d(data, cc, cidx, i_comp, i_cnt, numEntries, dimension); // unrounded

                // round
                for (i = 0; i<ncl; i++)
                    for (j = 0; j<dimension; j++)
                        cc[i_comp[i]][j] = (float)floorf(cc[i_comp[i]][j] + 0.5F); // more or less ideal location

                for (j = 0; j<dimension; j++)
                    rp[0][j] = rp[1][j] = 0;

                // weight with cnt if runnning on compacted index
                for (k = 0; k<numEntries; k++) {
                    im[0][0] += (Mi_ - cidx[k])* (Mi_ - cidx[k]);
                    im[0][1] += cidx[k] * (Mi_ - cidx[k]);           // im is symmetric
                    im[1][1] += cidx[k] * cidx[k];

                    for (j = 0; j<dimension; j++) {
                        rp[0][j] += (Mi_ - cidx[k]) * cc[cidx[k]][j];
                        rp[1][j] += cidx[k] * cc[cidx[k]][j];
                    }
                }

                float dd = im[0][0] * im[1][1] - im[0][1] * im[0][1];

                // dd=0 means that cidx[k] and (Mi_-cidx[k]) collinear which implies only one active index;
                // taken care of separately

                im[1][0] = im[0][0];
                im[0][0] = im[1][1] / dd;
                im[1][1] = im[1][0] / dd;
                im[1][0] = im[0][1] = -im[0][1] / dd;

                for (j = 0; j<dimension; j++) {
                    epa[0][j] = (im[0][0] * rp[0][j] + im[0][1] * rp[1][j])*Mi_;
                    epa[1][j] = (im[1][0] * rp[0][j] + im[1][1] * rp[1][j])*Mi_;
                }

                // shake single or                                   - cartesian
                // shake odd/odd and even/even or                    - same parity
                // shake odd/odd odd/even , even/odd and even/even   - bcc

                float err_1 = FLT_MAX;
                int epo_1[2][MAX_DIMENSION_BIG];

                float ed[2][2][MAX_DIMENSION_BIG];
                int epo_2_[2][2][2][MAX_DIMENSION_BIG];

                for (j = 0; j<dimension; j++) {

#ifndef USE_NEWRAMP
                    float(*rb)[SP_ERRIDX_MAX][16] = ramp[CLT(clog)][BTT(max_bits[j])];
#endif

                    int pp[2] = { 0,0 };
                    int rr = (use_par ? 2 : 1);

                    int epi[2][2];  // first/second, coord, begin rage end range


                    for (pp[0] = 0; pp[0]<rr; pp[0]++) {
                        for (pp[1] = 0; pp[1]<rr; pp[1]++) {

                            for (i = 0; i<2; i++) {     // set range
                                epi[i][0] = epi[i][1] = ep_find_floor(epa[i][j], max_bits[j], use_par, pp[i]);

                                epi[i][0] -= ((epi[i][0]  < (size >> 1) - 1 ? epi[i][0] : (size >> 1) - 1))  & (~use_par);

                                epi[i][1] += ((1 << max_bits[j]) - 1 - epi[i][1]  < (size >> 1) ?
                                              (1 << max_bits[j]) - 1 - epi[i][1] : (size >> 1)) & (~use_par);
                            }

                            int p1, p2, step = (1 << use_par);

                            ed[pp[0]][pp[1]][j] = FLT_MAX;

                            for (p1 = epi[0][0]; p1 <= epi[0][1]; p1 += step)
                                for (p2 = epi[1][0]; p2 <= epi[1][1]; p2 += step) {
#ifndef USE_NEWRAMP
                                    float *rbp = rb[p1][p2];
#endif
                                    float t = 0;
                                    int    *ci = cidx;
                                    int    m = numEntries;
                                    int _mc = m;

                                    while (_mc > 0) {
#ifndef USE_NEWRAMP
                                        t += (rbp[ci[_mc - 1]] - data[_mc - 1][j])
                                             *(rbp[ci[_mc - 1]] - data[_mc - 1][j]);
#else
                                        t += (rampf(CLT(clog), BTT(max_bits[j]), p1, p2, ci[_mc - 1]) - data[_mc - 1][j])
                                             *(rampf(CLT(clog), BTT(max_bits[j]), p1, p2, ci[_mc - 1]) - data[_mc - 1][j]);
#endif
                                        _mc--;
                                    }

                                    if (t<ed[pp[0]][pp[1]][j]) {
                                        ed[pp[0]][pp[1]][j] = t;
                                        epo_2_[pp[0]][pp[1]][0][j] = p1;
                                        epo_2_[pp[0]][pp[1]][1][j] = p2;
                                    }
                                }
                        } // pp[1]
                    } // pp[0]
                } // j

                int pn;

                for (pn = 0; pn<npv_nd[dimension - 3][type]; pn++) {
                    int dim1 = dimension - 3;
                    int dim2 = type;
                    int dim3 = pn;
                    int j1;
                    float err_2 = 0;

                    for (j1 = 0; j1<dimension; j1++)
                        err_2 += ed[
                                     get_par_vector(dim1, dim2, dim3, 0, j1)][get_par_vector(dim1, dim2, dim3, 1, j1)][j1];
                    if (err_2 < err_1) {
                        err_1 = err_2;
                        for (j1 = 0; j1<dimension; j1++) {
                            epo_1[0][j1] = epo_2_[get_par_vector(dim1, dim2, dim3, 0, j1)][get_par_vector(dim1, dim2, dim3, 1, j1)][0][j1];
                            epo_1[1][j1] = epo_2_[get_par_vector(dim1, dim2, dim3, 0, j1)][get_par_vector(dim1, dim2, dim3, 1, j1)][1][j1];
                        }
                    }
                }

                if (err_1 <= err_0) { // we'd want to get expanded index;
                    err_0 = err_1;
                    p0 = p;
                    q0 = q;
                    for (j = 0; j<dimension; j++) {
                        epo_0[0][j] = epo_1[0][j];
                        epo_0[1][j] = epo_1[1][j];
                    }
                }
            }
        // requantize
#ifndef USE_NEWRAMP
        float *r[MAX_DIMENSION_BIG];
#endif
        int idg[MAX_ENTRIES];

        float err_r = 0;

        if (CLT(clog) > (LOG_CL_RANGE - LOG_CL_BASE))
            return FLT_MAX;

        for (int jj = 0; jj<dimension; jj++) {
            if (BTT(max_bits[jj]) >(BIT_RANGE - BIT_BASE))
                return FLT_MAX;
            if ((epo_0[0][jj] > 255) || (epo_0[0][jj] < 0))
                return FLT_MAX;
            if ((epo_0[1][jj] > 255) || (epo_0[0][jj] < 0))
                return FLT_MAX;
#ifndef USE_NEWRAMP
            r[jj] = ramp[CLT(clog)][BTT(max_bits[jj])][epo_0[0][jj]][epo_0[1][jj]];
#endif
        }

        for (i = 0; i<numEntries; i++) {
            float  cmin = FLT_MAX;
            int        ci = 0;
            float   d[4];
            d[0] = data[i][0];
            d[1] = data[i][1];
            d[2] = data[i][2];
            d[3] = data[i][3];

            for (j = 0; j < (1 << clog); j++) {
                float t_ = 0.;

                for (k = 0; k<dimension; k++) {
#ifndef USE_NEWRAMP
                    t_ += (r[k][j] - d[k])*
                          (r[k][j] - d[k]);
#else
                    t_ += (rampf(CLT(clog), BTT(max_bits[k]), epo_0[0][k], epo_0[1][k], j) - d[k])*
                          (rampf(CLT(clog), BTT(max_bits[k]), epo_0[0][k], epo_0[1][k], j) - d[k]);
#endif
                }

                if (t_<cmin) {
                    cmin = t_;
                    ci = j;
                }
            }

            idg[i] = ci;
            for (k = 0; k<dimension; k++) {
#ifndef USE_NEWRAMP
                outg[i][k] = r[k][ci];
#else
                outg[i][k] = rampf(CLT(clog), BTT(max_bits[k]), epo_0[0][k], epo_0[1][k], ci);
#endif
            }

            err_r += cmin;
        }

        change = 0;
        for (k = 0; k<numEntries; k++)
            change = change || (index[k] * q0 + p0 != idg[k]);

        better = err_r < err_o;

        if (better) {
            for (k = 0; k<numEntries; k++) {
                index_[k] = index[k] = idg[k];
                for (j = 0; j<dimension; j++) {
                    out[k][j] = outg[k][j];
                    epo_code[0][j] = epo_0[0][j];
                    epo_code[1][j] = epo_0[1][j];
                }
            }
            err_o = err_r;
        }

        done = !(change  &&  better);
        if (maxTry > 0) maxTry--;
        else maxTry = 0;
    } while (!done && maxTry);

    for (j = 0; j<dimension; j++) {
#ifndef USE_NEWRAMP
        epo[0][j] = ramp[CLT(clog)][BTT(max_bits[j])][epo_code[0][j]][epo_code[1][j]][0];
        epo[1][j] = ramp[CLT(clog)][BTT(max_bits[j])][epo_code[0][j]][epo_code[1][j]][(1 << clog) - 1];
#else
        epo[0][j] = rampf(CLT(clog), BTT(max_bits[j]), epo_code[0][j], epo_code[1][j], 0);
        epo[1][j] = rampf(CLT(clog), BTT(max_bits[j]), epo_code[0][j], epo_code[1][j], (1 << clog) - 1);
#endif
    }

    return err_o;
}



}

