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
// BC6H_Encode.cpp : Encoder for BC6H
//
// Revision
// 0.1    First implementation
// 0.2    Removed unused code and disabeled optimization
//
#include <assert.h>
#include "debug.h"
#include "BC6H_Encode.h"
#include "half.h"
#include <float.h>
#include <stdio.h>
#include <math.h>
#include "Common.h"
#include "BC6H_Definitions.h"
#include "BC6H_Partitions.h"
#include "BC6H_Encode.h"
#include "BC6H_Utils.h"
#include "BC6H_3dquant_vpc.h"
#include "BC6H_shake.h"

#pragma warning(disable:4244)
#include "half.h"
#pragma warning(default:4244)

#ifdef BC6H_COMPDEBUGGER
#include "compclient.h"
#endif

extern int  g_block;
extern FILE *g_fp;
int gl_block = 0;

#ifdef BC6H_DEBUG_TO_RESULTS_TXT
extern FILE *g_fp;
extern int  g_mode;
#endif

#ifdef DEBUG_PATTERNS
// random pixel noise range
float BC6HBlockEncoder::DoPixelNoise()
{
    float ret = (rand() % RANDOM_NOISE_LEVEL)/ 100.0;
    return (ret);
}
#endif

/*
Reserved Feature MONOSHAPE_PATTERNS
int BC6HBlockEncoder::FindPattern();
*/

#ifdef BC6H_COMPDEBUGGER
    CompViewerClient    g_CompClient;
    BOOL                g_Remote_Connected;
#endif

int QuantizeToInt(float value, int prec, bool signedfloat16, double exposure)
{
    if (value == 0) value = 0.01f;

    int ivalue = (int)(value * exposure);
    if (prec <= 1) return ivalue;
    
    int iQuantized; 
    bool negvalue = false;
    int bias = (prec > 10) ? ((1<<(prec-1))-1) : 0;    
    
    if (signedfloat16)
    {
        if(ivalue < 0)
        {
            negvalue = true;
            ivalue = -ivalue;
        }
        prec--;
    }
    iQuantized = ((ivalue << prec) + bias) / Float16bitMAX1;
    return (negvalue?-iQuantized:iQuantized);
}

// decompress endpoints
static void decompress_endpts(const int in[2][2][3], int out[2][2][3], const int mode, bool issigned)
{

    if (ModePartition[mode].transformed)
    {
        for (int i=0; i<3; ++i)
        {
            R_0(out) = issigned ? SIGN_EXTEND(R_0(in),ModePartition[mode].IndexPrec) : R_0(in);
            int t;
            t = SIGN_EXTEND(R_1(in), ModePartition[mode].prec[i]);
            t = (t + R_0(in)) & MASK(ModePartition[mode].nbits);
            R_1(out) = issigned ? SIGN_EXTEND(t,ModePartition[mode].nbits) : t;
            
            t = SIGN_EXTEND(R_2(in), ModePartition[mode].prec[i]);
            t = (t + R_0(in)) & MASK(ModePartition[mode].nbits);
            R_2(out) = issigned ? SIGN_EXTEND(t,ModePartition[mode].nbits) : t;
            
            t = SIGN_EXTEND(R_3(in), ModePartition[mode].prec[i]);
            t = (t + R_0(in)) & MASK(ModePartition[mode].nbits);
            R_3(out) = issigned ? SIGN_EXTEND(t,ModePartition[mode].nbits) : t;
        }
    }
    else
    {
        for (int i=0; i<3; ++i)
        {
            R_0(out) = issigned ? SIGN_EXTEND(R_0(in),ModePartition[mode].nbits)   : R_0(in);
            R_1(out) = issigned ? SIGN_EXTEND(R_1(in),ModePartition[mode].prec[i]) : R_1(in);
            R_2(out) = issigned ? SIGN_EXTEND(R_2(in),ModePartition[mode].prec[i]) : R_2(in);
            R_3(out) = issigned ? SIGN_EXTEND(R_3(in),ModePartition[mode].prec[i]) : R_3(in);
        }
    }
}

void SaveDataBlock(AMD_BC6H_Format bc6h_format, BYTE out[BC6H_COMPRESSED_BLOCK_SIZE])
{
    BitHeader header(NULL,BC6H_COMPRESSED_BLOCK_SIZE);

    // Save the RGB end point values
    switch (bc6h_format.m_mode)
    {
        case 1: //0x00
                header.setvalue(0,2,0x00);
                header.setvalue(2 ,1 ,bc6h_format.gy,4);        //        gy[4]
                header.setvalue(3 ,1 ,bc6h_format.by,4);        //        by[4]
                header.setvalue(4 ,1 ,bc6h_format.bz,4);        //        bz[4]
                header.setvalue(5 ,10,bc6h_format.rw);          // 10:    rw[9:0] 
                header.setvalue(15,10,bc6h_format.gw);          // 10:    gw[9:0]
                header.setvalue(25,10,bc6h_format.bw);          // 10:    bw[9:0]
                header.setvalue(35,5 ,bc6h_format.rx);          // 5:     rx[4:0]
                header.setvalue(40,1 ,bc6h_format.gz,4);        //        gz[4]
                header.setvalue(41,4 ,bc6h_format.gy);          // 5:     gy[3:0]
                header.setvalue(45,5 ,bc6h_format.gx);          // 5:     gx[4:0]
                header.setvalue(50,1 ,bc6h_format.bz);          // 5:     bz[0]
                header.setvalue(51,4 ,bc6h_format.gz);          // 5:     gz[3:0]
                header.setvalue(55,5 ,bc6h_format.bx);          // 5:     bx[4:0]
                header.setvalue(60,1 ,bc6h_format.bz,1);        //        bz[1]
                header.setvalue(61,4 ,bc6h_format.by);          // 5:     by[3:0]
                header.setvalue(65,5 ,bc6h_format.ry);          // 5:     ry[4:0]
                header.setvalue(70,1 ,bc6h_format.bz,2);        //        bz[2]
                header.setvalue(71,5 ,bc6h_format.rz);          // 5:     rz[4:0]
                header.setvalue(76,1 ,bc6h_format.bz,3);        //        bz[3]
                break;
        case 2: // 0x01
                header.setvalue(0,2,0x01);
                header.setvalue(2 ,1 ,bc6h_format.gy,5);        //        gy[5]
                header.setvalue(3 ,1 ,bc6h_format.gz,4);        //        gz[4]
                header.setvalue(4 ,1 ,bc6h_format.gz,5);        //        gz[5]
                header.setvalue(5 ,7 ,bc6h_format.rw);          //        rw[6:0] 
                header.setvalue(12,1 ,bc6h_format.bz);          //        bz[0]
                header.setvalue(13,1 ,bc6h_format.bz,1);        //        bz[1]
                header.setvalue(14,1 ,bc6h_format.by,4);        //        by[4]
                header.setvalue(15,7 ,bc6h_format.gw);          //        gw[6:0]
                header.setvalue(22,1 ,bc6h_format.by,5);        //        by[5]
                header.setvalue(23,1 ,bc6h_format.bz,2);        //        bz[2]
                header.setvalue(24,1 ,bc6h_format.gy,4);        //        gy[4]
                header.setvalue(25,7 ,bc6h_format.bw);          // 7:     bw[6:0]
                header.setvalue(32,1 ,bc6h_format.bz,3);        //        bz[3]
                header.setvalue(33,1 ,bc6h_format.bz,5);        //        bz[5]
                header.setvalue(34,1 ,bc6h_format.bz,4);        //        bz[4]
                header.setvalue(35,6 ,bc6h_format.rx);          // 6:     rx[5:0]
                header.setvalue(41,4 ,bc6h_format.gy);          // 6:     gy[3:0]
                header.setvalue(45,6 ,bc6h_format.gx);          // 6:     gx[5:0]
                header.setvalue(51,4 ,bc6h_format.gz);          // 6:     gz[3:0]
                header.setvalue(55,6 ,bc6h_format.bx);          // 6:     bx[5:0]
                header.setvalue(61,4 ,bc6h_format.by);          // 6:     by[3:0]
                header.setvalue(65,6 ,bc6h_format.ry);          // 6:     ry[5:0]
                header.setvalue(71,6 ,bc6h_format.rz);          // 6:     rz[5:0]
                break;
        case 3: // 0x02
                header.setvalue(0,5,0x02);
                header.setvalue(5 ,10,bc6h_format.rw);          // 11:    rw[9:0] 
                header.setvalue(15,10,bc6h_format.gw);          // 11:    gw[9:0]
                header.setvalue(25,10,bc6h_format.bw);          // 11:    bw[9:0]
                header.setvalue(35,5 ,bc6h_format.rx);          // 5:     rx[4:0]
                header.setvalue(40,1 ,bc6h_format.rw,10);       //        rw[10]
                header.setvalue(41,4 ,bc6h_format.gy);          // 4:     gy[3:0]
                header.setvalue(45,4 ,bc6h_format.gx);          // 4:     gx[3:0]
                header.setvalue(49,1 ,bc6h_format.gw,10);       //        gw[10]
                header.setvalue(50,1 ,bc6h_format.bz);          // 4:     bz[0]
                header.setvalue(51,4 ,bc6h_format.gz);          // 4:     gz[3:0]
                header.setvalue(55,4 ,bc6h_format.bx);          // 4:     bx[3:0]
                header.setvalue(59,1 ,bc6h_format.bw,10);       //        bw[10]
                header.setvalue(60,1 ,bc6h_format.bz,1);        //        bz[1]
                header.setvalue(61,4 ,bc6h_format.by);          // 4:     by[3:0]
                header.setvalue(65,5 ,bc6h_format.ry);          // 5:     ry[4:0]
                header.setvalue(70,1 ,bc6h_format.bz,2);        //        bz[2]
                header.setvalue(71,5 ,bc6h_format.rz);          // 5:     rz[4:0]
                header.setvalue(76,1 ,bc6h_format.bz,3);        //        bz[3]
                break;
        case 4: // 0x06
                header.setvalue(0,5,0x06);
                header.setvalue(5 ,10,bc6h_format.rw);          // 11:    rw[9:0] 
                header.setvalue(15,10,bc6h_format.gw);          // 11:    gw[9:0]
                header.setvalue(25,10,bc6h_format.bw);          // 11:    bw[9:0]
                header.setvalue(35,4 ,bc6h_format.rx);          //        rx[3:0]
                header.setvalue(39,1 ,bc6h_format.rw,10);       //        rw[10]
                header.setvalue(40,1 ,bc6h_format.gz,4);        //        gz[4]
                header.setvalue(41,4 ,bc6h_format.gy);          // 5:     gy[3:0]
                header.setvalue(45,5 ,bc6h_format.gx);          //        gx[4:0]
                header.setvalue(50,1 ,bc6h_format.gw,10);       // 5:     gw[10]
                header.setvalue(51,4 ,bc6h_format.gz);          // 5:     gz[3:0]
                header.setvalue(55,4 ,bc6h_format.bx);          // 4:     bx[3:0]
                header.setvalue(59,1 ,bc6h_format.bw,10);       //        bw[10]
                header.setvalue(60,1 ,bc6h_format.bz,1);        //        bz[1]
                header.setvalue(61,4 ,bc6h_format.by);          // 4:     by[3:0]
                header.setvalue(65,4 ,bc6h_format.ry);          // 4:     ry[3:0]
                header.setvalue(69,1 ,bc6h_format.bz);          // 4:     bz[0]
                header.setvalue(70,1 ,bc6h_format.bz,2);        //        bz[2]
                header.setvalue(71,4 ,bc6h_format.rz);          // 4:     rz[3:0]
                header.setvalue(75,1 ,bc6h_format.gy,4);        //        gy[4]
                header.setvalue(76,1 ,bc6h_format.bz,3);        //        bz[3]
                break;
        case 5: // 0x0A
                header.setvalue(0,5,0x0A);
                header.setvalue(5 ,10,bc6h_format.rw);           // 11:   rw[9:0] 
                header.setvalue(15,10,bc6h_format.gw);           // 11:   gw[9:0]
                header.setvalue(25,10,bc6h_format.bw);           // 11:   bw[9:0]
                header.setvalue(35,4 ,bc6h_format.rx);           // 4:    rx[3:0]
                header.setvalue(39,1 ,bc6h_format.rw,10);        //       rw[10]
                header.setvalue(40,1 ,bc6h_format.by,4);         //       by[4]
                header.setvalue(41,4 ,bc6h_format.gy);           // 4:    gy[3:0]
                header.setvalue(45,4 ,bc6h_format.gx);           // 4:    gx[3:0]
                header.setvalue(49,1 ,bc6h_format.gw,10);        //       gw[10]
                header.setvalue(50,1 ,bc6h_format.bz);           // 5:    bz[0]
                header.setvalue(51,4 ,bc6h_format.gz);           // 4:    gz[3:0]
                header.setvalue(55,5 ,bc6h_format.bx);           // 5:    bx[4:0]
                header.setvalue(60,1 ,bc6h_format.bw,10);        //       bw[10]
                header.setvalue(61,4 ,bc6h_format.by);           // 5:    by[3:0]
                header.setvalue(65,4 ,bc6h_format.ry);           // 4:    ry[3:0]
                header.setvalue(69,1 ,bc6h_format.bz,1);         //       bz[1]
                header.setvalue(70,1 ,bc6h_format.bz,2);         //       bz[2]
                header.setvalue(71,4 ,bc6h_format.rz);           // 4:    rz[3:0]
                header.setvalue(75,1 ,bc6h_format.bz,4);         //       bz[4]
                header.setvalue(76,1 ,bc6h_format.bz,3);         //       bz[3]
                break;
        case 6: // 0x0E
                header.setvalue(0,5,0x0E);
                header.setvalue(5 ,9 ,bc6h_format.rw);           // 9:    rw[8:0] 
                header.setvalue(14,1 ,bc6h_format.by,4);         //       by[4]
                header.setvalue(15,9 ,bc6h_format.gw);           // 9:    gw[8:0]
                header.setvalue(24,1 ,bc6h_format.gy,4);         //       gy[4]
                header.setvalue(25,9 ,bc6h_format.bw);           // 9:    bw[8:0]
                header.setvalue(34,1 ,bc6h_format.bz,4);         //       bz[4]
                header.setvalue(35,5 ,bc6h_format.rx);           // 5:    rx[4:0]
                header.setvalue(40,1 ,bc6h_format.gz,4);         //       gz[4]
                header.setvalue(41,4 ,bc6h_format.gy);           // 5:    gy[3:0]
                header.setvalue(45,5 ,bc6h_format.gx);           // 5:    gx[4:0]
                header.setvalue(50,1 ,bc6h_format.bz);           // 5:    bz[0]
                header.setvalue(51,4 ,bc6h_format.gz);           // 5:    gz[3:0]
                header.setvalue(55,5 ,bc6h_format.bx);           // 5:    bx[4:0]
                header.setvalue(60,1 ,bc6h_format.bz,1);         //       bz[1]
                header.setvalue(61,4 ,bc6h_format.by);           // 5:    by[3:0]
                header.setvalue(65,5 ,bc6h_format.ry);           // 5:    ry[4:0]
                header.setvalue(70,1 ,bc6h_format.bz,2);         //       bz[2]
                header.setvalue(71,5 ,bc6h_format.rz);           // 5:    rz[4:0]
                header.setvalue(76,1 ,bc6h_format.bz,3);         //       bz[3]
                break;
        case 7: // 0x12
                header.setvalue(0,5,0x12);
                header.setvalue(5 ,8 ,bc6h_format.rw);           // 8:    rw[7:0] 
                header.setvalue(13,1 ,bc6h_format.gz,4);         //       gz[4]
                header.setvalue(14,1 ,bc6h_format.by,4);         //       by[4]
                header.setvalue(15,8 ,bc6h_format.gw);           // 8:    gw[7:0]
                header.setvalue(23,1 ,bc6h_format.bz,2);         //       bz[2]
                header.setvalue(24,1 ,bc6h_format.gy,4);         //       gy[4]
                header.setvalue(25,8 ,bc6h_format.bw);           // 8:    bw[7:0]
                header.setvalue(33,1 ,bc6h_format.bz,3);         //       bz[3]
                header.setvalue(34,1 ,bc6h_format.bz,4);         //       bz[4]
                header.setvalue(35,6 ,bc6h_format.rx);           // 6:    rx[5:0]
                header.setvalue(41,4 ,bc6h_format.gy);           // 5:    gy[3:0]
                header.setvalue(45,5 ,bc6h_format.gx);           // 5:    gx[4:0]
                header.setvalue(50,1 ,bc6h_format.bz);           // 5:    bz[0]
                header.setvalue(51,4 ,bc6h_format.gz);           // 5:    gz[3:0]
                header.setvalue(55,5 ,bc6h_format.bx);           // 5:    bx[4:0]
                header.setvalue(60,1 ,bc6h_format.bz,1);         //       bz[1]
                header.setvalue(61,4 ,bc6h_format.by);           // 5:    by[3:0]
                header.setvalue(65,6 ,bc6h_format.ry);           // 6:    ry[5:0]
                header.setvalue(71,6 ,bc6h_format.rz);           // 6:    rz[5:0]
                break;
        case 8: // 0x16
                header.setvalue(0,5,0x16);
                header.setvalue(5 ,8 ,bc6h_format.rw);            // 8:   rw[7:0] 
                header.setvalue(13,1 ,bc6h_format.bz);            // 5:   bz[0]
                header.setvalue(14,1 ,bc6h_format.by,4);          //      by[4]
                header.setvalue(15,8 ,bc6h_format.gw);            // 8:   gw[7:0]
                header.setvalue(23,1 ,bc6h_format.gy,5);          //      gy[5]
                header.setvalue(24,1 ,bc6h_format.gy,4);          //      gy[4]
                header.setvalue(25,8 ,bc6h_format.bw);            // 8:   bw[7:0]
                header.setvalue(33,1 ,bc6h_format.gz,5);          //      gz[5]
                header.setvalue(34,1 ,bc6h_format.bz,4);          //      bz[4]
                header.setvalue(35,5 ,bc6h_format.rx);            // 5:   rx[4:0]
                header.setvalue(40,1 ,bc6h_format.gz,4);          //      gz[4]
                header.setvalue(41,4 ,bc6h_format.gy);            // 6:   gy[3:0]
                header.setvalue(45,6 ,bc6h_format.gx);            // 6:   gx[5:0]
                header.setvalue(51,4 ,bc6h_format.gz);            // 6:   gz[3:0]
                header.setvalue(55,5 ,bc6h_format.bx);            // 5:   bx[4:0]
                header.setvalue(60,1 ,bc6h_format.bz,1);          //      bz[1]
                header.setvalue(61,4 ,bc6h_format.by);            // 5:   by[3:0]
                header.setvalue(65,5 ,bc6h_format.ry);            // 5:   ry[4:0]
                header.setvalue(70,1 ,bc6h_format.bz,2);          //      bz[2]
                header.setvalue(71,5 ,bc6h_format.rz);            // 5:   rz[4:0]
                header.setvalue(76,1 ,bc6h_format.bz,3);          //      bz[3]
                break;
        case 9: // 0x1A
                header.setvalue(0,5,0x1A);
                header.setvalue(5 ,8 ,bc6h_format.rw);            // 8:   rw[7:0] 
                header.setvalue(13,1 ,bc6h_format.bz,1);          //      bz[1]
                header.setvalue(14,1 ,bc6h_format.by,4);          //      by[4]
                header.setvalue(15,8 ,bc6h_format.gw);            // 8:   gw[7:0]
                header.setvalue(23,1 ,bc6h_format.by,5);          //      by[5]
                header.setvalue(24,1 ,bc6h_format.gy,4);          //      gy[4]
                header.setvalue(25,8 ,bc6h_format.bw);            // 8:   bw[7:0]
                header.setvalue(33,1 ,bc6h_format.bz,5);          //      bz[5]
                header.setvalue(34,1 ,bc6h_format.bz,4);          //      bz[4]
                header.setvalue(35,5 ,bc6h_format.rx);            // 5:   rx[4:0]
                header.setvalue(40,1 ,bc6h_format.gz,4);          //      gz[4]
                header.setvalue(41,4 ,bc6h_format.gy);            // 5:   gy[3:0]
                header.setvalue(45,5 ,bc6h_format.gx);            // 5:   gx[4:0]
                header.setvalue(50,1 ,bc6h_format.bz);            // 6:   bz[0]
                header.setvalue(51,4 ,bc6h_format.gz);            // 5:   gz[3:0]
                header.setvalue(55,6 ,bc6h_format.bx);            // 6:   bx[5:0]
                header.setvalue(61,4 ,bc6h_format.by);            // 6:   by[3:0]
                header.setvalue(65,5 ,bc6h_format.ry);            // 5:   ry[4:0]
                header.setvalue(70,1 ,bc6h_format.bz,2);          //      bz[2]
                header.setvalue(71,5 ,bc6h_format.rz);            // 5:   rz[4:0]
                header.setvalue(76,1 ,bc6h_format.bz,3);          //      bz[3]
                break;
        case 10: // 0x1E
                header.setvalue(0,5,0x1E);
                header.setvalue(5 ,6 ,bc6h_format.rw);            // 6:   rw[5:0] 
                header.setvalue(11,1 ,bc6h_format.gz,4);          //      gz[4]
                header.setvalue(12,1 ,bc6h_format.bz);            // 6:   bz[0]
                header.setvalue(13,1 ,bc6h_format.bz,1);          //      bz[1]
                header.setvalue(14,1 ,bc6h_format.by,4);          //      by[4]
                header.setvalue(15,6 ,bc6h_format.gw);            // 6:   gw[5:0]
                header.setvalue(21,1 ,bc6h_format.gy,5);          //      gy[5]
                header.setvalue(22,1 ,bc6h_format.by,5);          //      by[5]
                header.setvalue(23,1 ,bc6h_format.bz,2);          //      bz[2]
                header.setvalue(24,1 ,bc6h_format.gy,4);          //      gy[4]
                header.setvalue(25,6 ,bc6h_format.bw);            // 6:   bw[5:0]
                header.setvalue(31,1 ,bc6h_format.gz,5);          //      gz[5]
                header.setvalue(32,1 ,bc6h_format.bz,3);          //      bz[3]
                header.setvalue(33,1 ,bc6h_format.bz,5);          //      bz[5]
                header.setvalue(34,1 ,bc6h_format.bz,4);          //      bz[4]
                header.setvalue(35,6 ,bc6h_format.rx);            // 6:   rx[5:0]
                header.setvalue(41,4 ,bc6h_format.gy);            // 6:   gy[3:0]
                header.setvalue(45,6 ,bc6h_format.gx);            // 6:   gx[5:0]
                header.setvalue(51,4 ,bc6h_format.gz);            // 6:   gz[3:0]
                header.setvalue(55,6 ,bc6h_format.bx);            // 6:   bx[5:0]
                header.setvalue(61,4 ,bc6h_format.by);            // 6:   by[3:0]
                header.setvalue(65,6 ,bc6h_format.ry);            // 6:   ry[5:0]
                header.setvalue(71,6 ,bc6h_format.rz);            // 6:   rz[5:0]
                break;

        // Single regions Modes
        case 11: // 0x03
                header.setvalue(0,5,0x03);
                header.setvalue(5 ,10,bc6h_format.rw);            // 10:   rw[9:0] 
                header.setvalue(15,10,bc6h_format.gw);            // 10:   gw[9:0]
                header.setvalue(25,10,bc6h_format.bw);            // 10:   bw[9:0]
                header.setvalue(35,10,bc6h_format.rx);            // 10:   rx[9:0]
                header.setvalue(45,10,bc6h_format.gx);            // 10:   gx[9:0]
                header.setvalue(55,10,bc6h_format.bx);            // 10:   bx[9:0]
                break;
        case 12: // 0x07
                header.setvalue(0,5,0x07);
                header.setvalue(5 ,10,bc6h_format.rw);            // 11:   rw[9:0] 
                header.setvalue(15,10,bc6h_format.gw);            // 11:   gw[9:0]
                header.setvalue(25,10,bc6h_format.bw);            // 11:   bw[9:0]
                header.setvalue(35,9 ,bc6h_format.rx);            // 9:    rx[8:0]
                header.setvalue(44,1 ,bc6h_format.rw,10);         //       rw[10]
                header.setvalue(45,9 ,bc6h_format.gx);            // 9:    gx[8:0]
                header.setvalue(54,1 ,bc6h_format.gw,10);         //       gw[10]
                header.setvalue(55,9 ,bc6h_format.bx);            // 9:    bx[8:0]
                header.setvalue(64,1 ,bc6h_format.bw,10);         //       bw[10]
                break;
        case 13: // 0x0B
                header.setvalue(0,5,0x0B);
                header.setvalue(5 ,10,bc6h_format.rw);            // 12:   rw[9:0] 
                header.setvalue(15,10,bc6h_format.gw);            // 12:   gw[9:0]
                header.setvalue(25,10,bc6h_format.bw);            // 12:   bw[9:0]
                header.setvalue(35,8 ,bc6h_format.rx);            // 8:    rx[7:0]
                header.setvalue(43,2 ,bc6h_format.rw,10);         //       rw[11:10]
                header.setvalue(45,8 ,bc6h_format.gx);            // 8:    gx[7:0]
                header.setvalue(53,2 ,bc6h_format.gw,10);         //       gw[11:10]
                header.setvalue(55,8 ,bc6h_format.bx);            // 8:    bx[7:0]
                header.setvalue(63,2 ,bc6h_format.bw,10);         //       bw[11:10]
                break;
        case 14: // 0x0F
                header.setvalue(0,5,0x0F);
                header.setvalue(5 ,10,bc6h_format.rw);            // 16:   rw[9:0] 
                header.setvalue(15,10,bc6h_format.gw);            // 16:   gw[9:0]
                header.setvalue(25,10,bc6h_format.bw);            // 16:   bw[9:0]
                header.setvalue(35,4 ,bc6h_format.rx);            //  4:   rx[3:0]
                header.setvalue(39,6 ,bc6h_format.rw,10);         //       rw[15:10]
                header.setvalue(45,4 ,bc6h_format.gx);            //  4:   gx[3:0]
                header.setvalue(49,6 ,bc6h_format.gw,10);         //       gw[15:10]
                header.setvalue(55,4 ,bc6h_format.bx);            //  4:   bx[3:0]
                header.setvalue(59,6 ,bc6h_format.bw,10);         //       bw[15:10]
                break;
        default: // Need to indicate error!
                return;
    }

    // Each format in the mode table can be uniquely identified by the mode bits. 
    // The first ten modes are used for two-region tiles, and the mode bit field 
    // can be either two or five bits long. These blocks also have fields for 
    // the compressed color endpoints (72 or 75 bits), the partition (5 bits), 
    // and the partition indices (46 bits).

    if (bc6h_format.m_mode >= MIN_MODE_FOR_ONE_REGION) 
    {
        int startbit = ONE_REGION_INDEX_OFFSET;
        header.setvalue(startbit, 3,bc6h_format.indices16[0 ]);
        startbit  += 3;
        for (int i= 1; i<16; i++)
        {
            header.setvalue(startbit,4,bc6h_format.indices16[i]);    
            startbit +=4;
        }
    }
    else
    {
        header.setvalue(77,5,bc6h_format.d_shape_index);            // Shape Index
        int startbit = TWO_REGION_INDEX_OFFSET, 
            nbits = 2;
        header.setvalue(startbit,nbits,bc6h_format.indices16[0 ]);    
        for (int i= 1; i<16; i++)
        {
            startbit += nbits; // offset start bit for next index using prior nbits used
            nbits        = g_indexfixups[bc6h_format.d_shape_index] == i?2:3; // get new number of bit to save index with
            header.setvalue(startbit,nbits,bc6h_format.indices16[i]);    
        }
    }

    // save to output buffer our new bit values
    // this can be optimized if header is part of bc6h_format struct
    header.transferbits(out,16);

}

// endpoints fit only if the compression was lossless
static bool endpts_fit(const int orig[2][2][3], const int compressed[2][2][3], const int mode, int max_subsets, bool issigned)
{
    int uncompressed[2][2][3];

    decompress_endpts(compressed, uncompressed, mode, issigned);

    for (int j=0; j<max_subsets; ++j)
    for (int i=0; i<3; ++i)
    {
        if (orig[j][0][i] != uncompressed[j][0][i]) return false;
        if (orig[j][1][i] != uncompressed[j][1][i]) return false;
    }
    return true;
}


void BC6HBlockEncoder::clampF16Max(float    EndPoints[2][2][3])
{
    for(int region=0; region<2; region++)
    for(int ab = 0; ab<2; ab++)
    for (int rgb=0; rgb<3; rgb++)
    {
        if (m_isSigned)
        {
            if (EndPoints[region][ab][rgb] < -F16MAX) EndPoints[region][ab][rgb] = -F16MAX;
            else if (EndPoints[region][ab][rgb] > F16MAX) EndPoints[region][ab][rgb] = F16MAX;
        }
        else
        {
            if (EndPoints[region][ab][rgb] < 0.0) EndPoints[region][ab][rgb] = 0;
            else if (EndPoints[region][ab][rgb] > F16MAX) EndPoints[region][ab][rgb] = F16MAX;
        }
    }
}    

/*=================================================================
    Quantize Endpoints
    for a given mode
==================================================================*/

void BC6HBlockEncoder::QuantizeEndPoint(float    EndPoints[2][2][3], int iEndPoints[2][2][3], int max_subsets, int prec)
{

    for (int subset = 0; subset < max_subsets; ++subset)
    {
        iEndPoints[subset][0][0] = QuantizeToInt(EndPoints[subset][0][0],prec,m_isSigned, m_Exposure);    // A.Red
        iEndPoints[subset][0][1] = QuantizeToInt(EndPoints[subset][0][1],prec,m_isSigned, m_Exposure);    // A.Green
        iEndPoints[subset][0][2] = QuantizeToInt(EndPoints[subset][0][2],prec,m_isSigned, m_Exposure);    // A.Blue
        iEndPoints[subset][1][0] = QuantizeToInt(EndPoints[subset][1][0],prec,m_isSigned, m_Exposure);    // B.Red
        iEndPoints[subset][1][1] = QuantizeToInt(EndPoints[subset][1][1],prec,m_isSigned, m_Exposure);    // B.Green
        iEndPoints[subset][1][2] = QuantizeToInt(EndPoints[subset][1][2],prec,m_isSigned, m_Exposure);    // B.Blue
    }
}

/*=================================================================
    Swap Indices 
    so that indices at fix up points have higher order bit set to 0
==================================================================*/

void BC6HBlockEncoder::SwapIndices(int iEndPoints[2][2][3], int iIndices[3][BC6H_MAX_SUBSET_SIZE], DWORD  entryCount[BC6H_MAX_SUBSETS], int max_subsets, int mode, int shape_pattern)
{
    
    UINT uNumIndices    = 1 << ModePartition[mode].IndexPrec;
    UINT uHighIndexBit    = uNumIndices >> 1;
    
    for(int subset = 0; subset < max_subsets; ++subset)
    {
        // region 0 (subset = 0) The fix-up index for this subset is allways index 0
        // region 1 (subset = 1) The fix-up index for this subset varies based on the shape 
        size_t i = subset?g_Region2FixUp[shape_pattern]:0;

        if(iIndices[subset][i] & uHighIndexBit)
        {
            // high bit is set, swap the aEndPts and indices for this region
            swap(iEndPoints[subset][0][0], iEndPoints[subset][1][0]);
            swap(iEndPoints[subset][0][1], iEndPoints[subset][1][1]);
            swap(iEndPoints[subset][0][2], iEndPoints[subset][1][2]);

            for(size_t j = 0; j < entryCount[subset]; ++j)
            {
                iIndices[subset][j] = uNumIndices - 1 - iIndices[subset][j] ;
            }
        }

    }
}


/*=================================================================
    Compress Endpoints
    according to shape precission
==================================================================*/

void BC6HBlockEncoder::CompressEndPoints(int iEndPoints[2][2][3], int oEndPoints[2][2][3],int max_subsets, int mode)
{

    int Mask;
    if ( ModePartition[mode].transformed)
    {
        for (int i=0; i<3; ++i)
        {
            Mask = MASK(ModePartition[mode].nbits);
            oEndPoints[0][0][i] = iEndPoints[0][0][i] & Mask;    // [0][A]

            Mask = MASK(ModePartition[mode].prec[i]);
            oEndPoints[0][1][i] = (iEndPoints[0][1][i]- iEndPoints[0][0][i]) & Mask; // [0][B] - [0][A]
            
            if (max_subsets > 1)
            {
                oEndPoints[1][0][i] = (iEndPoints[1][0][i] - iEndPoints[0][0][i]) & Mask; // [1][A] - [0][A]
                oEndPoints[1][1][i] = (iEndPoints[1][1][i] - iEndPoints[0][0][i]) & Mask; //  [1][B] - [0][A]
            }
        }
    }
    else
    {
        for (int i=0; i<3; ++i)
        {
            Mask = MASK(ModePartition[mode].nbits);
            oEndPoints[0][0][i] = iEndPoints[0][0][i] & Mask;

            Mask = MASK(ModePartition[mode].prec[i]);
            oEndPoints[0][1][i] = iEndPoints[0][1][i] & Mask;

            if (max_subsets > 1)
            {
                oEndPoints[1][0][i] = iEndPoints[1][0][i] & Mask;
                oEndPoints[1][1][i] = iEndPoints[1][1][i] & Mask;
            }
        }
    }
}


void BC6HBlockEncoder::SaveCompressedBlockData( AMD_BC6H_Format &BC6H_data, 
                                            int oEndPoints[2][2][3],
                                            int iIndices[3][BC6H_MAX_SUBSET_SIZE], 
                                            int max_subsets, 
                                            int mode)
{
        BC6H_data.m_mode    = (unsigned short)mode;
        BC6H_data.index++;

        // Save the data to output
        BC6H_data.rw = oEndPoints[0][0][0]; // rw
        BC6H_data.gw = oEndPoints[0][0][1]; // gw
        BC6H_data.bw = oEndPoints[0][0][2]; // bw
        BC6H_data.rx = oEndPoints[0][1][0]; // rx
        BC6H_data.gx = oEndPoints[0][1][1]; // gx
        BC6H_data.bx = oEndPoints[0][1][2]; // bx

        if (max_subsets > 1)
        {
            // Save the data to output
            BC6H_data.ry = oEndPoints[1][0][0]; // ry
            BC6H_data.gy = oEndPoints[1][0][1]; // gy
            BC6H_data.by = oEndPoints[1][0][2]; // by
            BC6H_data.rz = oEndPoints[1][1][0]; // rz
            BC6H_data.gz = oEndPoints[1][1][1]; // gz
            BC6H_data.bz = oEndPoints[1][1][2]; // bz
        }

        // Map our two subset Indices for the shape to output 4x4 block
        int pos[2] = {0,0};
        int asubset;
        for (int i=0; i<BC6H_MAX_SUBSET_SIZE; i++)
        {
            if (max_subsets > 1)
                asubset                = BC7_PARTITIONS[1][BC6H_data.d_shape_index][i]; // Two region shapes 
            else
                asubset                = BC7_PARTITIONS[0][BC6H_data.d_shape_index][i]; // One region shapes 
            BC6H_data.indices16[i]    = (byte)iIndices[asubset][pos[asubset]];
            pos[asubset]++;
        }

}


#ifdef BC6H_COMPDEBUGGER

void SetLineColor(float colr)
{
    if (g_Remote_Connected)
    {
        float region1[1] = {colr};
        // Clear display to show new pattern encoding
        g_CompClient.SendData(100,sizeof(float),(byte *)region1,false);
    }
}


void RemoteDataReset()
{
    if (g_Remote_Connected)
    {
        float region1[1] = {0};
        // Clear display to show new pattern encoding
        g_CompClient.SendData(0,sizeof(float),(byte *)region1,false);
    }
}

void ShowInputColors(float in[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG])
{
    if (g_Remote_Connected)
    {
        float region1[BC6H_MAX_SUBSET_SIZE][3];
        memset(region1,0,sizeof(region1));

        for (int i = 0; i<16; i++)
        {
            region1[i][0] = in[i][0];
            region1[i][1] = in[i][1];
            region1[i][2] = in[i][2];
        }
        g_CompClient.SendData(5,sizeof(region1),(byte *)region1,false);
    }
}


void ShowEndPointColors()
{
    #ifdef _BC6H_COMPDEBUGGER
    // show the normalized colors on the line
    if (g_Remote_Connected)
    {
    float Region0[BC6H_MAX_SUBSET_SIZE][3];
    float Region1[BC6H_MAX_SUBSET_SIZE][3];

    // Clean initial data
    memset(Region0,0,sizeof(Region0));
    memset(Region1,0,sizeof(Region1));

    for (int i=0; i<BC6H_MAX_SUBSET_SIZE; i++)
    {
        Region1[i][0] = outB[subset][i][0];
        Region1[i][1] = outB[subset][i][1];
        Region1[i][2] = outB[subset][i][2];
    }
    //BYTE B=0;
    //g_CompClient.SendData(0,1,&B,false);
    g_CompClient.SendData(6,sizeof(Region1),(byte *)Region1,false);
    }
    #endif
}

void ShowEndPointLine(AMD_BC6H_Format    &BC6H_data, int max_subsets)
{
    // show the line where most or all colors pass through (_B_)
    if (g_Remote_Connected)
    {
        for (int subset=0; subset<max_subsets; subset++)
            g_CompClient.SendData(4,sizeof(BC6H_data.EndPoints[subset]),(byte *)BC6H_data.EndPoints[subset],false);
    }
}

void ShowBestResults(float in[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG],AMD_BC6H_Format    &BC6H_data)
{
    // show the input colors (_A_)
    if (g_Remote_Connected)
    {
        float region1[BC6H_MAX_SUBSET_SIZE][3];
        memset(region1,0,sizeof(region1));

        // Clear display to show new pattern encoding
        g_CompClient.SendData(0,sizeof(float),(byte *)region1,false);

        for (int i = 0; i<16; i++)
        {
            region1[i][0] = in[i][0];
            region1[i][1] = in[i][1];
            region1[i][2] = in[i][2];
        }
        g_CompClient.SendData(5,sizeof(region1),(byte *)region1,false);

        for (int subset=0; subset < 2; subset++)
        {
            g_CompClient.SendData(4,sizeof(BC6H_data.EndPoints[subset]),(byte *)BC6H_data.EndPoints[subset],false);
            g_CompClient.SendData(6,16*3*sizeof(float),(byte *)BC6H_data.indexedcolors[subset],false);
        }
    }
}
#endif


void    BC6HBlockEncoder::BlockSetup(DWORD blockMode)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    m_parityBits = BC6H_CART;
    m_componentBits[BC6H_COMP_RED]        = ModePartition[blockMode].prec[0];
    m_componentBits[BC6H_COMP_GREEN]    = ModePartition[blockMode].prec[1];
    m_componentBits[BC6H_COMP_BLUE]        = ModePartition[blockMode].prec[2];
    m_componentBits[BC6H_COMP_ALPHA]    = 0;

    m_clusters[0] = 1 << ModePartition[blockMode].IndexPrec;
    m_clusters[1] = 0;
}


void CopyEndPoints(float EndPoints[2][2][3], double outB[2][BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG], int max_subsets, DWORD entryCount[2], float scale)
{
    // Save Min and Max OutB points as EndPoints
    for (int subset=0; subset<max_subsets; subset++)
    {
            // We now have points on direction vector(s) 
            // find the min and max points
            double min = FLT_MAX;
            double max = 0;
            double val;
            int mini = 0;
            int maxi = 0;

    
            for (DWORD i=0; i < entryCount[subset]; i++)
            {
                val = outB[subset][i][0] + outB[subset][i][1] + outB[subset][i][2];
                if (val < min) 
                {
                    min = val;
                    mini = i;
                }
                if (val > max)
                {
                    max  = val;
                    maxi = i;
                }
            }

            EndPoints[subset][0][0] = (float)outB[subset][mini][0]*scale;
            EndPoints[subset][0][1] = (float)outB[subset][mini][1]*scale;
            EndPoints[subset][0][2] = (float)outB[subset][mini][2]*scale;

            EndPoints[subset][1][0] = (float)outB[subset][maxi][0]*scale;
            EndPoints[subset][1][1] = (float)outB[subset][maxi][1]*scale;
            EndPoints[subset][1][2] = (float)outB[subset][maxi][2]*scale;

    }    
}


double BC6HBlockEncoder::optimize_endpts(
                                        AMD_BC6H_Format &BC6H_data,
                                        float    best_EndPoints    [2][2][3], 
                                        int        best_Indices    [2][16],
                                        int        max_subsets, 
                                        int        mode, 
                                        double error)
{
    error = 0; // unreferenced
    // Set the number of bits per endpoint that each color 
    // component has. Note that our quatization has a limit 
    // on the number of bits per channel (Max ? bits)
    int bits[4] = {0,0,0,0};

    if (BC6H_data.region == 2)
    {
        bits[0] = ModePartition[mode].prec[0];
        bits[1] = ModePartition[mode].prec[1];
        bits[2] = ModePartition[mode].prec[2];
        bits[3] = (bits[0]+bits[1]+bits[2]) * 2;
    }
    else
    {
        bits[0] = ModePartition[mode].nbits/3;
        bits[1] = bits[0];
        bits[2] = bits[0];
        bits[3] = (bits[0]+bits[1]+bits[2]) * 2;
    }        

    BlockSetup(mode);

    double  outB[2][BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG];
    double  outB1[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG];
    double  outB2[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG];

    int     epo_code[BC6H_MAX_SUBSETS][2][BC6H_MAX_DIMENSION_BIG];
    double  bestError = 0;
    int     temp_epo_code[2][BC6H_MAX_DIMENSION_BIG];
    double  epo[2][BC6H_MAX_DIMENSION_BIG];
    int        temp_Indices1    [16];
    int        temp_Indices2    [16];
    double    tempPartition[2][16][4];
    int        last_cluster =  (BC6H_data.region == 1)?15:7;
    
    double tempError1;
    double tempError2;
    float  ScaleEndPoints = 100.0f;

    memset(tempPartition,0,sizeof(tempPartition));
    memset(temp_Indices1,0,sizeof(temp_Indices1));
    memset(temp_Indices2,0,sizeof(temp_Indices2));

    for(int subset=0; subset < max_subsets; subset++)
    {

            for(int k=0; k < 16; k++)
            {
                tempPartition[subset][k][0] = BC6H_data.partition[subset][k][0] / ScaleEndPoints;
                tempPartition[subset][k][1] = BC6H_data.partition[subset][k][1] / ScaleEndPoints;
                tempPartition[subset][k][2] = BC6H_data.partition[subset][k][2] / ScaleEndPoints;
                tempPartition[subset][k][3] = BC6H_data.partition[subset][k][3] / ScaleEndPoints;
                temp_Indices1[k]    =  BC6H_data.shape_indices[subset][k];
                temp_Indices2[k]    =  BC6H_data.shape_indices[subset][k];
            }


            tempError1  = BC6H_ep_shaker_d(tempPartition[subset],
                                        BC6H_data.entryCount[subset],
                                        temp_Indices1,
                                        outB1,
                                        temp_epo_code,
                                        last_cluster, 
                                        bits,
                                        BC6H_CART,
                                        3);
             
             tempError2 = BC6H_ep_shaker_2_d(tempPartition[subset],
                                            BC6H_data.entryCount[subset],
                                            temp_Indices2,
                                            outB2,
                                            epo_code[subset],
                                            2,
                                            last_cluster,
                                            bits[3],
                                            3,
                                            epo);
             
             
            if (tempError1 < tempError2)
            {
            
                tempError2 = BC6H_ep_shaker_2_d(tempPartition[subset],
                                           BC6H_data.entryCount[subset],
                                           temp_Indices1,
                                           outB2,
                                           temp_epo_code,
                                           2,
                                           last_cluster,
                                           bits[3],
                                           3,
                                           epo);

                bestError += tempError2;
                // Save the new indices results 
                memcpy(best_Indices[subset],temp_Indices1,sizeof(int)*16);
                memcpy(outB[subset], outB2, sizeof(double) * 16 * 4);
            }
            else
            {
                bestError += tempError1;
                // Save the new results 
                memcpy(best_Indices[subset],temp_Indices2,sizeof(int)*16);
                memcpy(outB[subset], outB1, sizeof(double) * 16 * 4);
            }

    }

    // Save the new end point results 
    CopyEndPoints(best_EndPoints,outB,max_subsets,BC6H_data.entryCount, ScaleEndPoints);

    return (bestError*ScaleEndPoints);
}


double    BC6HBlockEncoder::FindBestPattern(AMD_BC6H_Format &BC6H_data, 
                          bool TwoRegionShapes, 
                          int shape_pattern, 
                          double in[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG])
{
    int        max_subsets            = TwoRegionShapes?2:1;

    double  error = 0;
    double  outB[2][BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG];
    double  direction[BC6H_MAX_DIMENSION_BIG];
    double  step;


    // Index bit size for the patterns been used. 
    // All two zone shapes have 3 bits per color, max index value < 8  
    // All one zone shapes gave 4 bits per color, max index value < 16
    int        Index_BitSize = TwoRegionShapes?8:16;    
    BC6H_data.region    = (unsigned short)max_subsets;
    BC6H_data.index        = 0;

    // for debugging
    double  OrgShape[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG];
    memcpy(OrgShape, in, sizeof(OrgShape));

    BC6H_data.d_shape_index = (unsigned short)shape_pattern;

    // Get the pattern to encode with
    BC6H_Partition( shape_pattern,          // Shape pattern we want to get
                    in,                     // Input data
                    BC6H_data.partition,    // Returns the patterned shape data
                    BC6H_data.entryCount,   // counts the number of pixel used in each subset region num of 0's amd 1's
                    max_subsets,            // Table Shapes to use eithe one regions 1 or two regions 2
                    3);                     // rgb no alpha always = 3


    m_parityBits  = BC6H_CART;    // Should set this someplace else higher up in code.

    // Clear the output buffers: Really don't need to do this!
    memset(outB,0,sizeof(outB));

    //
    // Get Shape bits, Direction, StepSize etc...
    //
    for (int subset = 0; subset < max_subsets; subset++)
    {

            // Single Index Block
            BC6H_optQuantAnD_d(BC6H_data.partition[subset],        // The Shape index to quantize to
                               BC6H_data.entryCount[subset],       // number of 0's and 1's
                               Index_BitSize,                      // Bit size to use for indices
                               BC6H_data.shape_indices[subset],    // Resulting indices for each shape region
                               outB[subset],                       // OutB[] points on a line
                               direction,                          // normalized direction vector
                               &step,                              // incriments 
                               3);                                 // always 3 = RGB
                    
            #ifdef _BC6H_COMPDEBUGGER
                ShowEndPointColors();
            #endif

    } // subsets

    CopyEndPoints(BC6H_data.EndPoints, outB, max_subsets, BC6H_data.entryCount, 1);
    clampF16Max(BC6H_data.EndPoints);

    error = 0;
    double  NewShape[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG];

    // Build the new shape from the partition output data
    int sub0index = 0;
    int sub1index = 0;
    for (int i = 0; i < BC6H_MAX_SUBSET_SIZE; i++)
    {
        // subset 0 or subset 1
        if (BC7_PARTITIONS[1][shape_pattern][i])
        {
            NewShape[i][0] = outB[1][sub1index][0];
            NewShape[i][1] = outB[1][sub1index][1];
            NewShape[i][2] = outB[1][sub1index][2];
            NewShape[i][3] = outB[1][sub1index][3];
            sub1index++;
        }
        else
        {
            NewShape[i][0] = outB[0][sub0index][0];
            NewShape[i][1] = outB[0][sub0index][1];
            NewShape[i][2] = outB[0][sub0index][2];
            NewShape[i][3] = outB[0][sub0index][3];
            sub0index++;
        }

        // Calculate error from original
        error +=    abs(in[i][0] - NewShape[i][0]) +
                    abs(in[i][1] - NewShape[i][1]) +
                    abs(in[i][2] - NewShape[i][2]) +
                    abs(in[i][3] - NewShape[i][3]);
    }


    #ifdef _BC6H_COMPDEBUGGER
        ShowEndPointLine(BC6H_data,max_subsets);
    #endif

    return error;
}

// Mode Pathern seach order to try on endpoints
// The order is to be optimized for best image quality
static int ModeFitOrder[MAX_BC6H_PARTITIONS+1] =  // Pattern order: in order of highest bits to low header bits
                       {
                       0,                // N/A skipped in for loops
                       3,4,5,            // 11 ...
                       1,                // 10 ...
                       6,                // 9  ...
                       7,8,9,            // 8  ...
                       2,                // 7  ...
                       10,               // 6  ...
                       14,               // 16 .
                       13,               // 12 . 
                       12,               // 11 .
                       11                // 10 .
                       }; 

double    BC6HBlockEncoder::EncodePattern(AMD_BC6H_Format &BC6H_data, 
                                        double  error)
{
    int        max_subsets            = BC6H_data.region;

    // now we have input colors (in), output colors (outB) mapped to a line of ends (EndPoints)
    // and a set of colors on the line equally spaced (indexedcolors)
    // Lets assign indices

    // Quantize the EndPoints 
    int iEndPoints[2][2][3];                    // temp endpoints used during calculations
    int iIndices[3][BC6H_MAX_SUBSET_SIZE];        // temp indices used during calculations
    int oEndPoints[2][2][3];                    // endpoints to save for a given mode

    // ModePartition[] starts from 1 to 14
    // If we have a shape pattern set the loop to check modes from 1 to 10 else from 11 to 14
    // of the ModePartition table
    int min_mode = (BC6H_data.region == 2)?1:11; 
     int    max_mode = (BC6H_data.region == 2)?MAX_TWOREGION_PARTITIONS:MAX_BC6H_PARTITIONS;

    bool fits[15];
    memset(fits,0,sizeof(fits));

    //
    // Notes;  Only the endpoints are varying; the indices stay fixed in values!
    // so to optimize which mode we need only check the endpoints error against our original to pick the mode to save
    //
    for (int modes = min_mode; modes <= max_mode; ++modes)
    {
            // Indices data to save for given mode
            memcpy(iIndices,BC6H_data.shape_indices,sizeof(iIndices));
            QuantizeEndPoint(BC6H_data.EndPoints,iEndPoints, max_subsets, ModePartition[ModeFitOrder[modes]].nbits);

#ifdef BC6H_DEBUG_TO_RESULTS_TXT
            //fprintf(g_fp,"Check Fit shape sp %d  Mode 0x%X  bits %d\n",modes,ModePartition[ModeFitOrder[modes]].mode, ModePartition[ModeFitOrder[modes]].nbits);
#endif
            SwapIndices(iEndPoints,iIndices,BC6H_data.entryCount,max_subsets,ModeFitOrder[modes],BC6H_data.d_shape_index);    
            CompressEndPoints(iEndPoints,oEndPoints, max_subsets,ModeFitOrder[modes]);

            /*=================================================================
                Check Fit
            ==================================================================*/

            fits[modes] = endpts_fit(iEndPoints, oEndPoints, ModeFitOrder[modes],max_subsets, m_isSigned);

            if (fits[modes])
            {
#ifdef BC6H_DEBUG_TO_RESULTS_TXT
                        fprintf(g_fp,"FITS --------------------\n");
                        fprintf(g_fp,"Input End Points 0\n");
                        fprintf(g_fp,"A.x = %d\n",iEndPoints[0][0][0]);
                        fprintf(g_fp,"A.y = %d\n",iEndPoints[0][0][1]);
                        fprintf(g_fp,"A.z = %d\n",iEndPoints[0][0][2]);
                        fprintf(g_fp,"B.x = %d\n",iEndPoints[0][1][0]);
                        fprintf(g_fp,"B.y = %d\n",iEndPoints[0][1][1]);
                        fprintf(g_fp,"B.z = %d\n",iEndPoints[0][1][2]);
                        fprintf(g_fp,"Input End Points 1\n");
                        fprintf(g_fp,"A.x = %d\n",iEndPoints[1][0][0]);
                        fprintf(g_fp,"A.y = %d\n",iEndPoints[1][0][1]);
                        fprintf(g_fp,"A.z = %d\n",iEndPoints[1][0][2]);
                        fprintf(g_fp,"B.x = %d\n",iEndPoints[1][1][0]);
                        fprintf(g_fp,"B.y = %d\n",iEndPoints[1][1][1]);
                        fprintf(g_fp,"B.z = %d\n",iEndPoints[1][1][2]);
                        fprintf(g_fp,"Compressed End Points 0\n");
                        fprintf(g_fp,"A.x = %d\n",oEndPoints[0][0][0]);
                        fprintf(g_fp,"A.y = %d\n",oEndPoints[0][0][1]);
                        fprintf(g_fp,"A.z = %d\n",oEndPoints[0][0][2]);
                        fprintf(g_fp,"B.x = %d\n",oEndPoints[0][1][0]);
                        fprintf(g_fp,"B.y = %d\n",oEndPoints[0][1][1]);
                        fprintf(g_fp,"B.z = %d\n",oEndPoints[0][1][2]);
                        fprintf(g_fp,"End Points 1\n");
                        fprintf(g_fp,"A.x = %d\n",oEndPoints[1][0][0]);
                        fprintf(g_fp,"A.y = %d\n",oEndPoints[1][0][1]);
                        fprintf(g_fp,"A.z = %d\n",oEndPoints[1][0][2]);
                        fprintf(g_fp,"B.x = %d\n",oEndPoints[1][1][0]);
                        fprintf(g_fp,"B.y = %d\n",oEndPoints[1][1][1]);
                        fprintf(g_fp,"B.z = %d\n",oEndPoints[1][1][2]);
#endif

                    #ifdef BC6H_COMPDEBUGGER
                            // Non-optimzed points
                            SetLineColor(1.0);
                            ShowEndPointLine(BC6H_data,max_subsets);
                    #endif

                    bool optimize = false; // Disabled 
                    if (optimize)
                    {
                            // This section of code is not work well and is disabled
                            double  opt_toterr;
                            float   best_EndPoints[2][2][3]; 
                            int     best_iEndPoints[2][2][3];
                            int     best_oEndPoints[2][2][3];
                            int     best_iIndices[2][16];
                

                            // Returns optimized endpoints for a given mode and its error value from the input endpoints
                            opt_toterr = optimize_endpts(BC6H_data,                //
                                                        best_EndPoints,            // New endpoint if any optimized
                                                        best_iIndices,             // New optimized indices
                                                        max_subsets,               // 1 or 2 regions shape
                                                        ModeFitOrder[modes],       // Mode we want to fit data into    
                                                        error);                    // Unoptimized error value we started with

                            // Did the optimization take place if so we should have 
                            // a new lower valued error : else we keep the original
                            if (opt_toterr < error)
                            {
                                QuantizeEndPoint(best_EndPoints,best_iEndPoints, max_subsets, ModePartition[ModeFitOrder[modes]].nbits);
                                SwapIndices(best_iEndPoints,best_iIndices,BC6H_data.entryCount,max_subsets,ModeFitOrder[modes],BC6H_data.d_shape_index);
                                CompressEndPoints(best_iEndPoints,best_oEndPoints, max_subsets,ModeFitOrder[modes]);

                                fits[modes] = endpts_fit(best_iEndPoints, best_oEndPoints, ModeFitOrder[modes],max_subsets, m_isSigned);

                                // do these new endpoints fit the mode precision
                                if (fits[modes] && ( opt_toterr <= error) ) 
                                {
                                    #ifdef BC6H_DEBUG_TO_RESULTS_TXT
                                    g_mode = ModePartition[ModeFitOrder[modes]].mode;
                                    #endif
                                    BC6H_data.optimized = true;
                                    // keep the new optimized endpoints
                                    memcpy(BC6H_data.EndPoints,best_EndPoints,sizeof(float)*2*2*3);

                                    SaveCompressedBlockData(BC6H_data,best_oEndPoints,best_iIndices,max_subsets, ModeFitOrder[modes]);

                                    #ifdef BC6H_COMPDEBUGGER
                                        // Optimzed points
                                        SetLineColor(0.5);
                                        ShowEndPointLine(BC6H_data,max_subsets);
                                    #endif

                                    return opt_toterr;
                                }
                            }
                    }// if optimize

                    // Keep the original endpoints
                    #ifdef BC6H_DEBUG_TO_RESULTS_TXT
                    g_mode = ModePartition[ModeFitOrder[modes]].mode;
                    #endif

                    SaveCompressedBlockData(BC6H_data,oEndPoints,iIndices,max_subsets,ModeFitOrder[modes]);

#ifdef BC6H_DEBUG_TO_RESULTS_TXT

#endif

                    return error;
            }

    }

#ifdef BC6H_DEBUG_TO_RESULTS_TXT
        if (g_fp)
        {
            fprintf(g_fp,"=== Error === at block %d\n",g_block);
        }
#endif

    // Should not get here!
    return error;

}


float BC6HBlockEncoder::CompressBlock(float    in[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG], BYTE out[BC6H_COMPRESSED_BLOCK_SIZE])
{
    /* Reserved feature:
    float smono[16];
    float R,G,B;
    float smin = FLT_MAX;
    */


    // Try to remove this section
    double din[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG];
    
    for (int i=0; i<BC6H_MAX_SUBSET_SIZE; i++)
    {
        din[i][0] = (double) in[i][0];
        din[i][1] = (double) in[i][1];
        din[i][2] = (double) in[i][2];
        din[i][3] = (double) in[i][3]; // this is not used....
    }

    #ifdef DEBUG_PATTERNS
    srand (100);
    // Save delta image to file
    fi = fopen("deltaImages.txt","w");
    #endif

    #ifdef BC6H_COMPDEBUGGER
        g_Remote_Connected = g_CompClient.connect();
        RemoteDataReset();
        ShowInputColors(in);
    #endif

    double    bestError        = DBL_MAX;
    int       bestShape        = 0;
    double    Error            = 0;
    int       shape_pattern    = -1;            // init to no shapes found

    AMD_BC6H_Format            BC6H_data;
    AMD_BC6H_Format            best_BC6H_data;

    memset(&best_BC6H_data,0,sizeof(AMD_BC6H_Format));

    if (m_useMonoShapePatterns)
    {
        /*
        Reserved Feature MONOSHAPE_PATTERNS
        */
    }

    if (shape_pattern == -1)
    {
        // Try region 0 
        memset(&BC6H_data,0,sizeof(BC6H_data));
        Error = FindBestPattern(BC6H_data,false,0,din);
    
        // Save our first value
        if (Error < bestError)
        {
                bestError = Error + 1E-24;
                bestShape = -1;
                memcpy(&best_BC6H_data,&BC6H_data,sizeof(BC6H_data));
        }

        // now run through all two regions shapes to find the best pattern
        for (int shape=0; shape<32; shape++)
        {
            memset(&BC6H_data,0,sizeof(BC6H_data));
            Error = FindBestPattern(BC6H_data,true,shape,din);
            if (Error <= bestError)
            {
                bestError = Error + 1E-24;
                bestShape = shape;
                shape_pattern = shape; // for debugging
                memcpy(&best_BC6H_data,&BC6H_data,sizeof(BC6H_data));
            }
        }
    }

    // Optimize the result for encoding
    bestError = EncodePattern(best_BC6H_data,bestError);
    
    // do final encoding and save to output block
    SaveDataBlock(best_BC6H_data,out);

    #ifdef BC6H_COMPDEBUGGER
        // Show the results
        ShowBestResults(in,best_BC6H_data);
        
        if (g_Remote_Connected)
            g_CompClient.disconnect();
    #endif

    #ifdef DEBUG_PATTERNS
    if (fi)
        fclose(fi);
    #endif

    #ifdef BC6H_DEBUG_TO_RESULTS_TXT
        if (g_fp)
        {
            /***********************
            best_BC6H_data.EndPoints[0][0][0],
            best_BC6H_data.EndPoints[0][0][1],
            best_BC6H_data.EndPoints[0][0][2],
            best_BC6H_data.EndPoints[0][1][0],
            best_BC6H_data.EndPoints[0][1][1],
            best_BC6H_data.EndPoints[0][1][2],
            *************************/
            //(%5.0f %5.0f %5.0f) (%5.0f %5.0f %5.0f)"
            fprintf(g_fp,"%3d :: %02x, %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x  Mode %2x Shape %2d %c %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d\n",
                    g_block,
                    0xFF&out[0],
                    0xFF&out[1],
                    0xFF&out[2],
                    0xFF&out[3],
                    0xFF&out[4],
                    0xFF&out[5],
                    0xFF&out[6],
                    0xFF&out[7],
                    0xFF&out[8],
                    0xFF&out[9],
                    0xFF&out[10],
                    0xFF&out[11],
                    0xFF&out[12],
                    0xFF&out[13],
                    0xFF&out[14],
                    0xFF&out[15],
                    g_mode,
                    bestShape,
                    best_BC6H_data.optimized?'*':' ',
                    best_BC6H_data.indices16[0],
                    best_BC6H_data.indices16[1],
                    best_BC6H_data.indices16[2],
                    best_BC6H_data.indices16[3],
                    best_BC6H_data.indices16[4],
                    best_BC6H_data.indices16[5],
                    best_BC6H_data.indices16[6],
                    best_BC6H_data.indices16[7],
                    best_BC6H_data.indices16[8],
                    best_BC6H_data.indices16[9],
                    best_BC6H_data.indices16[10],
                    best_BC6H_data.indices16[11],
                    best_BC6H_data.indices16[12],
                    best_BC6H_data.indices16[13],
                    best_BC6H_data.indices16[14],
                    best_BC6H_data.indices16[15]
        );
        }
    #endif

    g_block++;

    return (float) bestError;
}
