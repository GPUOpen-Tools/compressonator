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
// BC6H_utils.h Genenral functions and definitions for use in encoder and decoder
// Revision
// 0.1    First implementation
//////////////////////////////////////////////////////////////////////////////////

#ifndef _BC6H_UTILS_H_
#define _BC6H_UTILS_H_

#include "bc6h_definitions.h"
#include <bitset>
#include <stddef.h>


class BitHeader {
  public:
    BitHeader(BYTE in[], int sizeinbytes) {
        m_bits.reset();
        m_sizeinbytes = sizeinbytes;

        if ((in != NULL) && (sizeinbytes <=16)) {
            // Init bits set with given data
            int bitpos = 0;
            for (int i=0; i<sizeinbytes; i++) {
                int bit = 1;
                for (int j=0; j<8; j++) {
                    m_bits[bitpos] = in[i] & bit?1:0;
                    bit = bit << 1;
                    bitpos++;
                }
            }
        }
    }

    ~BitHeader() {
    }

    void transferbits(BYTE in[],int sizeinbytes) {
        if ((sizeinbytes <= m_sizeinbytes) && (in!=NULL)) {
            // Init bits set with given data
            memset(in,0,sizeinbytes);
            int bitpos = 0;
            for (int i=0; i<sizeinbytes; i++) {
                int bit = 1;
                for (int j=0; j<8; j++) {
                    if (m_bits[bitpos]) in[i]|=bit;
                    bit = bit << 1;
                    bitpos++;
                }
            }
        }
    }

    int getvalue(int start, int bitsize) {
        int value = 0;
        int end = start + bitsize - 1;
        for (; end >= start; end--) {
            value |= m_bits[end]?1:0;
            if (end > start) value <<= 1;
        }

        return value;
    }

    void setvalue(int start, int bitsize, int value, int maskshift = 0) {
        int end = start + bitsize - 1;
        int mask = 0x1 << maskshift;
        for (; start <= end; start++) {
            m_bits[start] = (value&mask)?1:0;
            mask <<= 1;
        }
    }

    std::bitset<128> m_bits;        // 16 bytes max
    int     m_sizeinbytes;
};

extern void ViewData(unsigned char data[]);

extern void    BC6H_WriteBit(BYTE   *base,
                             int  offset,
                             BYTE   bitVal);

extern void BC6H_GetRamp(DWORD endpoint[][MAX_DIMENSION_BIG],
                         double ramp[MAX_DIMENSION_BIG][(1<<BC6H_MAX_INDEX_BITS)],
                         DWORD clusters[2],
                         DWORD componentBits[MAX_DIMENSION_BIG]);

extern const double  BC6H_rampLerpWeights[5][1<<MAX_INDEX_BITS];


#endif

