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
//
// BC6H_Decode.cpp : Decoder for BC6H
//
// Revision
// 0.1    First implementation
//

#include <assert.h>
#include "debug.h"
#include "common.h"
#include "hdr_encode.h"
#include "bc6h_definitions.h"
#include "bc6h_decode.h"
#include "bc6h_utils.h"
#include <bitset>
#include <stddef.h>

#ifdef TEST_CMP_CORE_DECODER
#include "cmp_core.h"
#endif

#ifdef BC6H_DECODE_DEBUG
int  g_dblock = 0;
#endif

/*using namespace std;*/
using namespace HDR_Encode;

float dec_red_out[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG] = {0};

//---------------------------------------------------------------------------------------------------------------------------------------
// Need improve or use alternate implementation: These segments are based on NV code and need to be redone
//---------------------------------------------------------------------------------------------------------------------------------------

#define    SIGN_EXTEND(w,tbits)    ((((signed(w))&(1<<((tbits)-1)))?((~0)<<(tbits)):0)|(signed(w)))
#define    MASK(n)    ((1<<(n))-1)

// NV code : used with modifications
void extract_compressed_endpoints2(AMD_BC6H_Format& bc6h_format) {
    int i;
    int t;

    if (bc6h_format.issigned) {
        if (bc6h_format.istransformed) {
            for (i=0; i<NCHANNELS; i++) {
                bc6h_format.E[0].A[i] = SIGN_EXTEND(bc6h_format.EC[0].A[i],bc6h_format.wBits);

                t = SIGN_EXTEND(bc6h_format.EC[0].B[i], bc6h_format.tBits[i]); // C_RED
                t = (t + bc6h_format.EC[0].A[i]) & MASK(bc6h_format.wBits);
                bc6h_format.E[0].B[i] = SIGN_EXTEND(t,bc6h_format.wBits);

                t = SIGN_EXTEND(bc6h_format.EC[1].A[i], bc6h_format.tBits[i]); //C_GREEN
                t = (t + bc6h_format.EC[0].A[i]) & MASK(bc6h_format.wBits);
                bc6h_format.E[1].A[i] = SIGN_EXTEND(t,bc6h_format.wBits);

                t = SIGN_EXTEND(bc6h_format.EC[1].B[i], bc6h_format.tBits[i]); //C_BLUE
                t = (t + bc6h_format.EC[0].A[i]) & MASK(bc6h_format.wBits);
                bc6h_format.E[1].B[i] = SIGN_EXTEND(t,bc6h_format.wBits);
            }
        } else {
            for (i=0; i<NCHANNELS; i++) {
                bc6h_format.E[0].A[i] = SIGN_EXTEND(bc6h_format.EC[0].A[i],bc6h_format.wBits);
                bc6h_format.E[0].B[i] = SIGN_EXTEND(bc6h_format.EC[0].B[i],bc6h_format.tBits[i]); //C_RED
                bc6h_format.E[1].A[i] = SIGN_EXTEND(bc6h_format.EC[1].A[i],bc6h_format.tBits[i]); //C_GREEN
                bc6h_format.E[1].B[i] = SIGN_EXTEND(bc6h_format.EC[1].B[i],bc6h_format.tBits[i]); //C_BLUE
            }
        }

    } else {
        if (bc6h_format.istransformed) {
            for (i=0; i<NCHANNELS; i++) {
                bc6h_format.E[0].A[i] = bc6h_format.EC[0].A[i];
                t = SIGN_EXTEND(bc6h_format.EC[0].B[i], bc6h_format.tBits[i]); // C_RED
                bc6h_format.E[0].B[i] = (t + bc6h_format.EC[0].A[i]) & MASK(bc6h_format.wBits);

                t = SIGN_EXTEND(bc6h_format.EC[1].A[i], bc6h_format.tBits[i]); // C_GREEN
                bc6h_format.E[1].A[i] = (t + bc6h_format.EC[0].A[i]) & MASK(bc6h_format.wBits);

                t = SIGN_EXTEND(bc6h_format.EC[1].B[i], bc6h_format.tBits[i]); //C_BLUE
                bc6h_format.E[1].B[i] = (t + bc6h_format.EC[0].A[i]) & MASK(bc6h_format.wBits);
            }
        } else {
            for (i=0; i<NCHANNELS; i++) {
                bc6h_format.E[0].A[i] = bc6h_format.EC[0].A[i];
                bc6h_format.E[0].B[i] = bc6h_format.EC[0].B[i];
                bc6h_format.E[1].A[i] = bc6h_format.EC[1].A[i];
                bc6h_format.E[1].B[i] = bc6h_format.EC[1].B[i];
            }
        }
    }

}

void extract_compressed_endpoints(AMD_BC6H_Format& bc6h_format) {
    int i;
    int t;

    if (bc6h_format.issigned) {
        if (bc6h_format.istransformed) {
            for (i=0; i<NCHANNELS; i++) {
                bc6h_format.E[0].A[i] = SIGN_EXTEND(bc6h_format.EC[0].A[i],bc6h_format.wBits);

                t = SIGN_EXTEND(bc6h_format.EC[0].B[i], bc6h_format.tBits[i]); //C_RED
                t = (t + bc6h_format.EC[0].A[i]) & MASK(bc6h_format.wBits);
                bc6h_format.E[0].B[i] = SIGN_EXTEND(t,bc6h_format.wBits);
            }
        } else {
            for (i=0; i<NCHANNELS; i++) {
                bc6h_format.E[0].A[i] = SIGN_EXTEND(bc6h_format.EC[0].A[i],bc6h_format.wBits);
                bc6h_format.E[0].B[i] = SIGN_EXTEND(bc6h_format.EC[0].B[i],bc6h_format.tBits[i]); //C_RED
            }
        }

    } else {
        if (bc6h_format.istransformed) {
            for (i=0; i<NCHANNELS; i++) {
                bc6h_format.E[0].A[i] = bc6h_format.EC[0].A[i];
                t = SIGN_EXTEND(bc6h_format.EC[0].B[i], bc6h_format.tBits[i]); //C_RED
                bc6h_format.E[0].B[i] = (t + bc6h_format.EC[0].A[i]) & MASK(bc6h_format.wBits);
            }
        } else {
            for (i=0; i<NCHANNELS; i++) {
                bc6h_format.E[0].A[i] = bc6h_format.EC[0].A[i];
                bc6h_format.E[0].B[i] = bc6h_format.EC[0].B[i];
            }
        }
    }

}

// NV code: Used with modifcations
int unquantize(AMD_BC6H_Format& bc6h_format, int q, int prec) {
    int unq = 0, s;

    switch (bc6h_format.format) {
    // modify this case to move the multiplication by 31 after interpolation.
    // Need to use finish_unquantize.

    // since we have 16 bits available, let's unquantize this to 16 bits unsigned
    // thus the scale factor is [0-7c00)/[0-10000) = 31/64
    case UNSIGNED_F16:
        if (prec >= 15)
            unq = q;
        else if (q == 0)
            unq = 0;
        else if (q == ((1<<prec)-1))
            unq = U16MAX;
        else
            unq = (q * (U16MAX+1) + (U16MAX+1)/2) >> prec;
        break;

    // here, let's stick with S16 (no apparent quality benefit from going to S17)
    // range is (-7c00..7c00)/(-8000..8000) = 31/32
    case SIGNED_F16:
        // don't remove this test even though it appears equivalent to the code below
        // as it isn't -- the code below can overflow for prec = 16
        if (prec >= 16)
            unq = q;
        else {
            if (q < 0) {
                s = 1;
                q = -q;
            } else s = 0;

            if (q == 0)
                unq = 0;
            else if (q >= ((1<<(prec-1))-1))
                unq = s ? -S16MAX : S16MAX;
            else {
                unq = (q * (S16MAX+1) + (S16MAX+1)/2) >> (prec-1);
                if (s)
                    unq = -unq;
            }
        }
        break;
    }
    return unq;
}

int lerp(int a, int b, int i, int denom) {
    assert (denom == 3 || denom == 7 || denom == 15);
    assert (i >= 0 && i <= denom);

    int shift = 6, *weights = NULL;

    switch(denom) {
    case 3:
        denom *= 5;
        i *= 5;    // fall through to case 15
    case 15:
        weights = g_aWeights4;
        break;
    case 7:
        weights = g_aWeights3;
        break;
    default:
        assert(0);
    }

#pragma warning(disable:4244)
    // no need to round these as this is an exact division
    return (int)(a*weights[denom-i] +b*weights[i]) / float(1 << shift);
}

int finish_unquantize(AMD_BC6H_Format bc6h_format, int q) {
    if (bc6h_format.format == UNSIGNED_F16)
        return (q * 31) >> 6;                                        // scale the magnitude by 31/64
    else if (bc6h_format.format == SIGNED_F16)
        return (q < 0) ? -(((-q) * 31) >> 5) : (q * 31) >> 5;        // scale the magnitude by 31/32
    else
        return q;
}

void generate_palette_quantized(int max, AMD_BC6H_Format& bc6h_format, int region) {
    // scale endpoints
    int a, b, c;            // really need a IntVec3...

    a = unquantize(bc6h_format, bc6h_format.E[region].A[0], bc6h_format.wBits);
    b = unquantize(bc6h_format, bc6h_format.E[region].B[0], bc6h_format.wBits);

    // interpolate : This part of code is used for debuging data
    for (int i = 0; i < max; i++) {
        c = finish_unquantize(bc6h_format, lerp(a, b, i, max-1));
        bc6h_format.Palete[region][i].x = c;
    }

    a = unquantize(bc6h_format, bc6h_format.E[region].A[1], bc6h_format.wBits);
    b = unquantize(bc6h_format, bc6h_format.E[region].B[1], bc6h_format.wBits);

    // interpolate
    for (int i = 0; i < max; i++)
        bc6h_format.Palete[region][i].y = finish_unquantize(bc6h_format, lerp(a, b, i, max-1));

    a = unquantize(bc6h_format,bc6h_format.E[region].A[2], bc6h_format.wBits);
    b = unquantize(bc6h_format,bc6h_format.E[region].B[2], bc6h_format.wBits);

    // interpolate
    for (int i = 0; i < max; i++)
        bc6h_format.Palete[region][i].z = finish_unquantize(bc6h_format, lerp(a, b, i, max-1));
}

AMD_BC6H_Format extract_format(BYTE in[COMPRESSED_BLOCK_SIZE]) {
    AMD_BC6H_Format bc6h_format;
    unsigned short decvalue;
    BYTE iData[COMPRESSED_BLOCK_SIZE];
    memcpy(iData,in,COMPRESSED_BLOCK_SIZE);

    memset(&bc6h_format,0,sizeof(AMD_BC6H_Format));

    // 2 bit mode has Mode bit:2 = 0 and mode bits:1 = 0 or 1
    // 5 bit mode has Mode bit:2 = 1
    if ((in[0]&0x02) > 0) {
        decvalue = (in[0]&0x1F);    // first five bits
    } else {
        decvalue = (in[0]&0x01);    // first two bits
    }

    BitHeader header(in,16);

    switch (decvalue) {
    case 0x00:
        bc6h_format.m_mode          = 1; // 10:5:5:5
        bc6h_format.wBits           = 10;
        bc6h_format.tBits[C_RED]    = 5;
        bc6h_format.tBits[C_GREEN]  = 5;
        bc6h_format.tBits[C_BLUE]   = 5;
        bc6h_format.rw = header.getvalue(5,10);             // 10:   rw[9:0]
        bc6h_format.rx = header.getvalue(35,5);             // 5:    rx[4:0]
        bc6h_format.ry = header.getvalue(65,5);             // 5:    ry[4:0]
        bc6h_format.rz = header.getvalue(71,5);             // 5:    rz[4:0]
        bc6h_format.gw = header.getvalue(15,10);            // 10:   gw[9:0]
        bc6h_format.gx = header.getvalue(45,5);             // 5:    gx[4:0]
        bc6h_format.gy = header.getvalue(41,4) |            // 5:    gy[3:0]
                         (header.getvalue(2,1) << 4);        //       gy[4]
        bc6h_format.gz = header.getvalue(51,4) |            // 5:    gz[3:0]
                         (header.getvalue(40,1) << 4);       //       gz[4]
        bc6h_format.bw = header.getvalue(25,10);            // 10:   bw[9:0]
        bc6h_format.bx = header.getvalue(55,5);             // 5:    bx[4:0]
        bc6h_format.by = header.getvalue(61,4) |            // 5:    by[3:0]
                         (header.getvalue(3,1) << 4);        //       by[4]
        bc6h_format.bz = header.getvalue(50,1) |            // 5:    bz[0]
                         (header.getvalue(60,1) << 1) |      //       bz[1]
                         (header.getvalue(70,1) << 2) |      //       bz[2]
                         (header.getvalue(76,1) << 3) |      //       bz[3]
                         (header.getvalue(4,1) << 4);        //       bz[4]
        break;
    case 0x01:
        bc6h_format.m_mode          = 2;    // 7:6:6:6
        bc6h_format.wBits           = 7;
        bc6h_format.tBits[C_RED]    = 6;
        bc6h_format.tBits[C_GREEN]  = 6;
        bc6h_format.tBits[C_BLUE]   = 6;
        bc6h_format.rw = header.getvalue(5,7);               // 7:    rw[6:0]
        bc6h_format.rx = header.getvalue(35,6);              // 6:    rx[5:0]
        bc6h_format.ry = header.getvalue(65,6);              // 6:    ry[5:0]
        bc6h_format.rz = header.getvalue(71,6);              // 6:    rz[5:0]
        bc6h_format.gw = header.getvalue(15,7);              // 7:    gw[6:0]
        bc6h_format.gx = header.getvalue(45,6);              // 6:    gx[5:0]
        bc6h_format.gy = header.getvalue(41,4)    |          // 6:    gy[3:0]
                         (header.getvalue(24,1) << 4) |       //       gy[4]
                         (header.getvalue(2,1)   << 5);       //       gy[5]
        bc6h_format.gz = header.getvalue(51,4)    |          // 6:    gz[3:0]
                         (header.getvalue(3,1) << 4) |        //       gz[4]
                         (header.getvalue(4,1) << 5);         //       gz[5]
        bc6h_format.bw = header.getvalue(25,7);              // 7:    bw[6:0]
        bc6h_format.bx = header.getvalue(55,6);              // 6:    bx[5:0]
        bc6h_format.by = header.getvalue(61,4)    |          // 6:    by[3:0]
                         (header.getvalue(14,1) << 4) |       //       by[4]
                         (header.getvalue(22,1) << 5);        //       by[5]
        bc6h_format.bz = header.getvalue(12,1)    |          // 6:    bz[0]
                         (header.getvalue(13,1) << 1) |       //       bz[1]
                         (header.getvalue(23,1) << 2) |       //       bz[2]
                         (header.getvalue(32,1) << 3) |       //       bz[3]
                         (header.getvalue(34,1) << 4) |       //       bz[4]
                         (header.getvalue(33,1) << 5);        //       bz[5]
        break;
    case 0x02:
        bc6h_format.m_mode          = 3;  // 11:5:4:4
        bc6h_format.wBits           = 11;
        bc6h_format.tBits[C_RED]    = 5;
        bc6h_format.tBits[C_GREEN]  = 4;
        bc6h_format.tBits[C_BLUE]   = 4;
        bc6h_format.rw = header.getvalue(5,10)  |            //11:    rw[9:0]
                         (header.getvalue(40,1) << 10);       //       rw[10]
        bc6h_format.rx = header.getvalue(35,5);              // 5:    rx[4:0]
        bc6h_format.ry = header.getvalue(65,5);              // 5:    ry[4:0]
        bc6h_format.rz = header.getvalue(71,5);              // 5:    rz[4:0]
        bc6h_format.gw = header.getvalue(15,10) |            //11:    gw[9:0]
                         (header.getvalue(49,1) << 10);       //       gw[10]
        bc6h_format.gx = header.getvalue(45,4);              //4:     gx[3:0]
        bc6h_format.gy = header.getvalue(41,4);              //4:     gy[3:0]
        bc6h_format.gz = header.getvalue(51,4);              //4:     gz[3:0]
        bc6h_format.bw = header.getvalue(25,10) |            //11:    bw[9:0]
                         (header.getvalue(59,1) << 10);       //       bw[10]
        bc6h_format.bx = header.getvalue(55,4);              //4:     bx[3:0]
        bc6h_format.by = header.getvalue(61,4);              //4:     by[3:0]
        bc6h_format.bz = header.getvalue(50,1) |             //4:     bz[0]
                         (header.getvalue(60,1) << 1) |       //       bz[1]
                         (header.getvalue(70,1) << 2) |       //       bz[2]
                         (header.getvalue(76,1) << 3);        //       bz[3]
        break;
    case 0x06:
        bc6h_format.m_mode          = 4;  // 11:4:5:4
        bc6h_format.wBits           = 11;
        bc6h_format.tBits[C_RED]    = 4;
        bc6h_format.tBits[C_GREEN]  = 5;
        bc6h_format.tBits[C_BLUE]   = 4;
        bc6h_format.rw = header.getvalue(5,10)  |             //11:   rw[9:0]
                         (header.getvalue(39,1) << 10);        //      rw[10]
        bc6h_format.rx = header.getvalue(35,4);               //4:    rx[3:0]
        bc6h_format.ry = header.getvalue(65,4);               //4:    ry[3:0]
        bc6h_format.rz = header.getvalue(71,4);               //4:    rz[3:0]
        bc6h_format.gw = header.getvalue(15,10) |             //11:   gw[9:0]
                         (header.getvalue(50,1) << 10);        //      gw[10]
        bc6h_format.gx = header.getvalue(45,5);               //5:    gx[4:0]
        bc6h_format.gy = header.getvalue(41,4) |              //5:    gy[3:0]
                         (header.getvalue(75,1) << 4);         //      gy[4]
        bc6h_format.gz = header.getvalue(51,4) |              //5:    gz[3:0]
                         (header.getvalue(40,1) << 4);         //      gz[4]
        bc6h_format.bw = header.getvalue(25,10) |             //11:   bw[9:0]
                         (header.getvalue(59,1) << 10);        //      bw[10]
        bc6h_format.bx = header.getvalue(55,4);               //4:    bx[3:0]
        bc6h_format.by = header.getvalue(61,4);               //4:    by[3:0]
        bc6h_format.bz = header.getvalue(69,1) |              //4:    bz[0]
                         (header.getvalue(60,1) << 1) |        //      bz[1]
                         (header.getvalue(70,1) << 2) |        //      bz[2]
                         (header.getvalue(76,1) << 3);         //      bz[3]
        break;
    case 0x0A:
        bc6h_format.m_mode          = 5; // 11:4:4:5
        bc6h_format.wBits           = 11;
        bc6h_format.tBits[C_RED]    = 4;
        bc6h_format.tBits[C_GREEN]  = 4;
        bc6h_format.tBits[C_BLUE]   = 5;
        bc6h_format.rw = header.getvalue(5,10)  |             //11:   rw[9:0]
                         (header.getvalue(39,1) << 10);        //      rw[10]
        bc6h_format.rx = header.getvalue(35,4);               //4:    rx[3:0]
        bc6h_format.ry = header.getvalue(65,4);               //4:    ry[3:0]
        bc6h_format.rz = header.getvalue(71,4);               //4:    rz[3:0]
        bc6h_format.gw = header.getvalue(15,10) |             //11:   gw[9:0]
                         (header.getvalue(49,1) << 10);        //      gw[10]
        bc6h_format.gx = header.getvalue(45,4);               //4:    gx[3:0]
        bc6h_format.gy = header.getvalue(41,4);               //4:    gy[3:0]
        bc6h_format.gz = header.getvalue(51,4);               //4:    gz[3:0]
        bc6h_format.bw = header.getvalue(25,10) |             //11:   bw[9:0]
                         (header.getvalue(60,1) << 10);        //      bw[10]
        bc6h_format.bx = header.getvalue(55,5);               //5:    bx[4:0]
        bc6h_format.by = header.getvalue(61,4);               //5:    by[3:0]
        (header.getvalue(40,1) << 4);         //      by[4]
        bc6h_format.bz = header.getvalue(50,1) |              //5:    bz[0]
                         (header.getvalue(69,1) << 1) |        //      bz[1]
                         (header.getvalue(70,1) << 2) |        //      bz[2]
                         (header.getvalue(76,1) << 3) |        //      bz[3]
                         (header.getvalue(75,1) << 4);         //      bz[4]
        break;
    case 0x0E:
        bc6h_format.m_mode          = 6;  // 9:5:5:5
        bc6h_format.wBits           = 9;
        bc6h_format.tBits[C_RED]    = 5;
        bc6h_format.tBits[C_GREEN]  = 5;
        bc6h_format.tBits[C_BLUE]   = 5;
        bc6h_format.rw = header.getvalue(5,9);                 //9:   rw[8:0]
        bc6h_format.gw = header.getvalue(15,9);                //9:   gw[8:0]
        bc6h_format.bw = header.getvalue(25,9);                //9:   bw[8:0]
        bc6h_format.rx = header.getvalue(35,5);                //5:   rx[4:0]
        bc6h_format.gx = header.getvalue(45,5);                //5:   gx[4:0]
        bc6h_format.bx = header.getvalue(55,5);                //5:   bx[4:0]
        bc6h_format.ry = header.getvalue(65,5);                //5:   ry[4:0]
        bc6h_format.gy = header.getvalue(41,4) |               //5:   gy[3:0]
                         (header.getvalue(24,1) << 4);          //     gy[4]
        bc6h_format.by = header.getvalue(61,4) |               //5:   by[3:0]
                         (header.getvalue(14,1) << 4);          //     by[4]
        bc6h_format.rz = header.getvalue(71,5);                //5:   rz[4:0]
        bc6h_format.gz = header.getvalue(51,4) |               //5:   gz[3:0]
                         (header.getvalue(40,1) << 4);          //     gz[4]
        bc6h_format.bz = header.getvalue(50,1) |               //5:   bz[0]
                         (header.getvalue(60,1) << 1) |         //     bz[1]
                         (header.getvalue(70,1) << 2) |         //     bz[2]
                         (header.getvalue(76,1) << 3) |         //     bz[3]
                         (header.getvalue(34,1) << 4);          //     bz[4]
        break;
    case 0x12:
        bc6h_format.m_mode          = 7;  // 8:6:5:5
        bc6h_format.wBits           = 8;
        bc6h_format.tBits[C_RED]    = 6;
        bc6h_format.tBits[C_GREEN]  = 5;
        bc6h_format.tBits[C_BLUE]   = 5;
        bc6h_format.rw = header.getvalue(5,8);                 //8:    rw[7:0]
        bc6h_format.gw = header.getvalue(15,8);                //8:    gw[7:0]
        bc6h_format.bw = header.getvalue(25,8);                //8:    bw[7:0]
        bc6h_format.rx = header.getvalue(35,6);                //6:    rx[5:0]
        bc6h_format.gx = header.getvalue(45,5);                //5:    gx[4:0]
        bc6h_format.bx = header.getvalue(55,5);                //5:    bx[4:0]
        bc6h_format.ry = header.getvalue(65,6);                //6:    ry[5:0]
        bc6h_format.gy = header.getvalue(41,4) |               //5:    gy[3:0]
                         (header.getvalue(24,1) << 4);          //      gy[4]
        bc6h_format.by = header.getvalue(61,4) |               //5:    by[3:0]
                         (header.getvalue(14,1) << 4);          //      by[4]
        bc6h_format.rz = header.getvalue(71,6);                //6:    rz[5:0]
        bc6h_format.gz = header.getvalue(51,4) |               //5:    gz[3:0]
                         (header.getvalue(13,1) << 4);          //      gz[4]
        bc6h_format.bz = header.getvalue(50,1) |               //5:    bz[0]
                         (header.getvalue(60,1) << 1) |         //      bz[1]
                         (header.getvalue(23,1) << 2) |         //      bz[2]
                         (header.getvalue(33,1) << 3) |         //      bz[3]
                         (header.getvalue(34,1) << 4);          //      bz[4]
        break;
    case 0x16:
        bc6h_format.m_mode          = 8;  // 8:5:6:5
        bc6h_format.wBits           = 8;
        bc6h_format.tBits[C_RED]    = 5;
        bc6h_format.tBits[C_GREEN]  = 6;
        bc6h_format.tBits[C_BLUE]   = 5;
        bc6h_format.rw = header.getvalue(5,8);                 //8:    rw[7:0]
        bc6h_format.gw = header.getvalue(15,8);                //8:    gw[7:0]
        bc6h_format.bw = header.getvalue(25,8);                //8:    bw[7:0]
        bc6h_format.rx = header.getvalue(35,5);                //5:    rx[4:0]
        bc6h_format.gx = header.getvalue(45,6);                //6:    gx[5:0]
        bc6h_format.bx = header.getvalue(55,5);                //5:    bx[4:0]
        bc6h_format.ry = header.getvalue(65,5);                //5:    ry[4:0]
        bc6h_format.gy = header.getvalue(41,4) |               //6:    gy[3:0]
                         (header.getvalue(24,1) << 4) |         //      gy[4]
                         (header.getvalue(23,1) << 5);          //      gy[5]
        bc6h_format.by = header.getvalue(61,4) |               //5:    by[3:0]
                         (header.getvalue(14,1) << 4);          //      by[4]
        bc6h_format.rz = header.getvalue(71,5);                //5:    rz[4:0]
        bc6h_format.gz = header.getvalue(51,4) |               //6:    gz[3:0]
                         (header.getvalue(40,1) << 4) |         //      gz[4]
                         (header.getvalue(33,1) << 5);          //      gz[5]
        bc6h_format.bz = header.getvalue(13,1) |               //5:    bz[0]
                         (header.getvalue(60,1) << 1) |         //      bz[1]
                         (header.getvalue(70,1) << 2) |         //      bz[2]
                         (header.getvalue(76,1) << 3) |         //      bz[3]
                         (header.getvalue(34,1) << 4);          //      bz[4]
        break;
    case 0x1A:
        bc6h_format.m_mode          = 9;  // 8:5:5:6
        bc6h_format.wBits           = 8;
        bc6h_format.tBits[C_RED]    = 5;
        bc6h_format.tBits[C_GREEN]  = 5;
        bc6h_format.tBits[C_BLUE]   = 6;
        bc6h_format.rw = header.getvalue(5,8);                 //8:    rw[7:0]
        bc6h_format.gw = header.getvalue(15,8);                //8:    gw[7:0]
        bc6h_format.bw = header.getvalue(25,8);                //8:    bw[7:0]
        bc6h_format.rx = header.getvalue(35,5);                //5:    rx[4:0]
        bc6h_format.gx = header.getvalue(45,5);                //5:    gx[4:0]
        bc6h_format.bx = header.getvalue(55,6);                //6:    bx[5:0]
        bc6h_format.ry = header.getvalue(65,5);                //5:    ry[4:0]
        bc6h_format.gy = header.getvalue(41,4) |               //5:    gy[3:0]
                         (header.getvalue(24,1) << 4);          //      gy[4]
        bc6h_format.by = header.getvalue(61,4)    |            //6:    by[3:0]
                         (header.getvalue(14,1) << 4) |         //      by[4]
                         (header.getvalue(23,1) << 5);          //      by[5]
        bc6h_format.rz = header.getvalue(71,5);                //5:    rz[4:0]
        bc6h_format.gz = header.getvalue(51,4) |               //5:    gz[3:0]
                         (header.getvalue(40,1) << 4);          //      gz[4]
        bc6h_format.bz = header.getvalue(50,1) |               //6:    bz[0]
                         (header.getvalue(13,1) << 1) |         //      bz[1]
                         (header.getvalue(70,1) << 2) |         //      bz[2]
                         (header.getvalue(76,1) << 3) |         //      bz[3]
                         (header.getvalue(34,1) << 4) |         //      bz[4]
                         (header.getvalue(33,1) << 5);          //      bz[5]
        break;
    case 0x1E:
        bc6h_format.m_mode          = 10;  // 6:6:6:6
        bc6h_format.istransformed   = FALSE;
        bc6h_format.wBits           = 6;
        bc6h_format.tBits[C_RED]    = 6;
        bc6h_format.tBits[C_GREEN]  = 6;
        bc6h_format.tBits[C_BLUE]   = 6;
        bc6h_format.rw = header.getvalue(5,6);                 //6:    rw[5:0]
        bc6h_format.gw = header.getvalue(15,6);                //6:    gw[5:0]
        bc6h_format.bw = header.getvalue(25,6);                //6:    bw[5:0]
        bc6h_format.rx = header.getvalue(35,6);                //6:    rx[5:0]
        bc6h_format.gx = header.getvalue(45,6);                //6:    gx[5:0]
        bc6h_format.bx = header.getvalue(55,6);                //6:    bx[5:0]
        bc6h_format.ry = header.getvalue(65,6);                //6:    ry[5:0]
        bc6h_format.gy = header.getvalue(41,4) |               //6:    gy[3:0]
                         (header.getvalue(24,1) << 4) |         //      gy[4]
                         (header.getvalue(21,1) << 5);          //      gy[5]
        bc6h_format.by = header.getvalue(61,4)    |            //6:    by[3:0]
                         (header.getvalue(14,1) << 4) |         //      by[4]
                         (header.getvalue(22,1) << 5);          //      by[5]
        bc6h_format.rz = header.getvalue(71,6);                //6:    rz[5:0]
        bc6h_format.gz = header.getvalue(51,4) |               //6:    gz[3:0]
                         (header.getvalue(11,1) << 4) |         //      gz[4]
                         (header.getvalue(31,1) << 5);          //      gz[5]
        bc6h_format.bz = header.getvalue(12,1) |               //6:    bz[0]
                         (header.getvalue(13,1) << 1) |         //      bz[1]
                         (header.getvalue(23,1) << 2) |         //      bz[2]
                         (header.getvalue(32,1) << 3) |         //      bz[3]
                         (header.getvalue(34,1) << 4) |         //      bz[4]
                         (header.getvalue(33,1) << 5);          //      bz[5]
        break;

    // Single region modes
    case 0x03:
        bc6h_format.m_mode            = 11;  // 10:10
        bc6h_format.wBits             = 10;
        bc6h_format.tBits[C_RED]      = 10;
        bc6h_format.tBits[C_GREEN]    = 10;
        bc6h_format.tBits[C_BLUE]     = 10;
        bc6h_format.rw = header.getvalue(5,10);             // 10: rw[9:0]
        bc6h_format.gw = header.getvalue(15,10);            // 10: gw[9:0]
        bc6h_format.bw = header.getvalue(25,10);            // 10: bw[9:0]
        bc6h_format.rx = header.getvalue(35,10);            // 10: rx[9:0]
        bc6h_format.gx = header.getvalue(45,10);            // 10: gx[9:0]
        bc6h_format.bx = header.getvalue(55,10);            // 10: bx[9:0]
        break;
    case 0x07:
        bc6h_format.m_mode              = 12;  // 11:9
        bc6h_format.wBits               = 11;
        bc6h_format.tBits[C_RED]        = 9;
        bc6h_format.tBits[C_GREEN]      = 9;
        bc6h_format.tBits[C_BLUE]       = 9;
        bc6h_format.rw = header.getvalue(5,10) |               // 10:   rw[9:0]
                         (header.getvalue(44,1) << 10);         //       rw[10]
        bc6h_format.gw = header.getvalue(15,10) |              // 10:   gw[9:0]
                         (header.getvalue(54,1) << 10);         //       gw[10]
        bc6h_format.bw = header.getvalue(25,10) |              // 10:   bw[9:0]
                         (header.getvalue(64,1) << 10);         //       bw[10]
        bc6h_format.rx = header.getvalue(35,9);                // 9:    rx[8:0]
        bc6h_format.gx = header.getvalue(45,9);                // 9:    gx[8:0]
        bc6h_format.bx = header.getvalue(55,9);                // 9:    bx[8:0]
        break;
    case 0x0B:
        bc6h_format.m_mode              = 13;  // 12:8
        bc6h_format.wBits               = 12;
        bc6h_format.tBits[C_RED]        = 8;
        bc6h_format.tBits[C_GREEN]      = 8;
        bc6h_format.tBits[C_BLUE]       = 8;
        bc6h_format.rw = header.getvalue(5, 10) |               // 12:   rw[9:0]
                         (header.getvalue(43, 1) << 11) |       //       rw[11]
                         (header.getvalue(44, 1) << 10);        //       rw[10]
        bc6h_format.gw = header.getvalue(15, 10) |              // 12:   gw[9:0]
                         (header.getvalue(53, 1) << 11) |       //       gw[11]
                         (header.getvalue(54, 1) << 10);        //       gw[10]
        bc6h_format.bw = header.getvalue(25,10) |               // 12:   bw[9:0]
                         (header.getvalue(63, 1) << 11) |       //       bw[11]
                         (header.getvalue(64,1) << 10);         //       bw[10]
        bc6h_format.rx = header.getvalue(35,8);                 //  8:   rx[7:0]
        bc6h_format.gx = header.getvalue(45,8);                 //  8:   gx[7:0]
        bc6h_format.bx = header.getvalue(55,8);                 //  8:   bx[7:0]
        break;
    case 0x0F:
        bc6h_format.m_mode          = 14;  // 16:4
        bc6h_format.wBits           = 16;
        bc6h_format.tBits[C_RED]    = 4;
        bc6h_format.tBits[C_GREEN]  = 4;
        bc6h_format.tBits[C_BLUE]   = 4;
        bc6h_format.rw = header.getvalue(5,10) |                // 16:   rw[9:0]
                         (header.getvalue(39, 1) << 15) |       //       rw[15]
                         (header.getvalue(40, 1) << 14) |       //       rw[14]
                         (header.getvalue(41, 1) << 13) |       //       rw[13]
                         (header.getvalue(42, 1) << 12) |       //       rw[12]
                         (header.getvalue(43, 1) << 11) |       //       rw[11]
                         (header.getvalue(44, 1) << 10);        //       rw[10]
        bc6h_format.gw = header.getvalue(15,10) |               // 16:   gw[9:0]
                         (header.getvalue(49, 1) << 15) |       //       gw[15]
                         (header.getvalue(50, 1) << 14) |       //       gw[14]
                         (header.getvalue(51, 1) << 13) |       //       gw[13]
                         (header.getvalue(52, 1) << 12) |       //       gw[12]
                         (header.getvalue(53, 1) << 11) |       //       gw[11]
                         (header.getvalue(54, 1) << 10);        //       gw[10]
        bc6h_format.bw = header.getvalue(25,10) |               // 16:   bw[9:0]
                         (header.getvalue(59, 1) << 15) |       //       bw[15]
                         (header.getvalue(60, 1) << 14) |       //       bw[14]
                         (header.getvalue(61, 1) << 13) |       //       bw[13]
                         (header.getvalue(62, 1) << 12) |       //       bw[12]
                         (header.getvalue(63, 1) << 11) |       //       bw[11]
                         (header.getvalue(64, 1) << 10);        //       bw[10]
        bc6h_format.rx = header.getvalue(35,4);                 // 4:    rx[3:0]
        bc6h_format.gx = header.getvalue(45,4);                 // 4:    gx[3:0]
        bc6h_format.bx = header.getvalue(55,4);                 // 4:    bx[3:0]
        break;
    default:
        bc6h_format.m_mode = 0;
        return bc6h_format;
    }

    // Each format in the mode table can be uniquely identified by the mode bits.
    // The first ten modes are used for two-region tiles, and the mode bit field
    // can be either two or five bits long. These blocks also have fields for
    // the compressed color endpoints (72 or 75 bits), the partition (5 bits),
    // and the partition indices (46 bits).

    if (bc6h_format.m_mode <= 10) {
        bc6h_format.region = BC6_TWO;
        // Get the shape index bits 77 to 81
        bc6h_format.d_shape_index = (unsigned short) header.getvalue(77,5);
        bc6h_format.istransformed = (bc6h_format.m_mode < 10) ? TRUE : FALSE;
    } else {
        bc6h_format.region           = BC6_ONE;
        bc6h_format.d_shape_index    = 0;
        bc6h_format.istransformed    = (bc6h_format.m_mode > 11) ? TRUE : FALSE;
    }

    // Save the points in a form easy to compute with
    bc6h_format.EC[0].A[0] = bc6h_format.rw;
    bc6h_format.EC[0].B[0] = bc6h_format.rx;
    bc6h_format.EC[1].A[0] = bc6h_format.ry;
    bc6h_format.EC[1].B[0] = bc6h_format.rz;
    bc6h_format.EC[0].A[1] = bc6h_format.gw;
    bc6h_format.EC[0].B[1] = bc6h_format.gx;
    bc6h_format.EC[1].A[1] = bc6h_format.gy;
    bc6h_format.EC[1].B[1] = bc6h_format.gz;
    bc6h_format.EC[0].A[2] = bc6h_format.bw;
    bc6h_format.EC[0].B[2] = bc6h_format.bx;
    bc6h_format.EC[1].A[2] = bc6h_format.by;
    bc6h_format.EC[1].B[2] = bc6h_format.bz;

    if (bc6h_format.region    == BC6_ONE) {
        int startbits = ONE_REGION_INDEX_OFFSET;
        bc6h_format.indices16[0] = (std::uint8_t) header.getvalue(startbits,3);
        startbits+=3;
        for (int i=1; i<16; i++) {
            bc6h_format.indices16[i] = (std::uint8_t) header.getvalue(startbits,4);
            startbits+=4;
        }
    } else {
        int startbit = TWO_REGION_INDEX_OFFSET,
            nbits = 2;
        bc6h_format.indices16[0 ] = (std::uint8_t) header.getvalue(startbit,2);
        for (int i= 1; i<16; i++) {
            startbit += nbits; // offset start bit for next index using prior nbits used
            nbits    = g_indexfixups[bc6h_format.d_shape_index] == i?2:3; // get new number of bit to save index with
            bc6h_format.indices16[i] = (std::uint8_t) header.getvalue(startbit,nbits);
        }

    }

    return bc6h_format;
}

//---------------------------------------------------------------------------------------------------------------------------------------

void BC6HBlockDecoder::DecompressBlock( float out[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],BYTE in[COMPRESSED_BLOCK_SIZE]) {

    AMD_BC6H_Format bc6h_format = extract_format(in);
    if (!bc6signed)
        bc6h_format.format = UNSIGNED_F16;
    else
        bc6h_format.format = SIGNED_F16;

    if(bc6h_format.region == BC6_ONE) {
        extract_compressed_endpoints(bc6h_format);
        generate_palette_quantized(16,bc6h_format,0);
    } else { //mode.type == BC6_TWO
        extract_compressed_endpoints2(bc6h_format);
        for (int r=0; r<2; r++) {
            generate_palette_quantized(8,bc6h_format,r);
        }
    }


    BC6H_Vec3 data;
    int indexPos=0;
    CMP_HALF rgb[3];

    // Note first 32 BC6H_PARTIONS is shared with BC6H
    // Partitioning is always arranged such that index 0 is always in subset 0 of BC6H_PARTIONS array
    // Partition order goes from top-left to bottom-right, moving left to right and then top to bottom.
    for (int block_row = 0; block_row < 4; block_row++)
        for (int block_col = 0; block_col < 4; block_col++) {
            // Need to check region logic
            // gets the region (0 or 1) in the partition set
            //int region = bc6h_format.region == BC6_ONE?0:REGION(block_col,block_row,bc6h_format.d_shape_index);
            // for a one region partitions : its always return 0 so there is room for performance improvement
            // by seperating the condition into another looped call.
            //int region = bc6h_format.region == BC6_ONE?0:BC6H_PARTITIONS[1][bc6h_format.d_shape_index][indexPos];
            int region = bc6h_format.region == BC6_ONE?0:PARTITIONS[1][bc6h_format.d_shape_index][indexPos];

            // Index is validated as ok
            int paleteIndex  = bc6h_format.indices[block_row][block_col];

            // this result is validated ok for region = BC6_ONE , BC6_TWO To be determined
            data = bc6h_format.Palete[region][paleteIndex];

            // Int to Half
            rgb[0].setBits((unsigned short) data.x);
            rgb[1].setBits((unsigned short) data.y);
            rgb[2].setBits((unsigned short) data.z);

            out[indexPos][0]  = (float) rgb[0];    // r;
            out[indexPos][1]  = (float) rgb[1]; // g;
            out[indexPos][2]  = (float) rgb[2];    // b;
            out[indexPos][3]  = 1.0f;

            indexPos++;
        }

#ifdef BC6H_DECODE_DEBUG
    g_dblock++;
#endif

}

