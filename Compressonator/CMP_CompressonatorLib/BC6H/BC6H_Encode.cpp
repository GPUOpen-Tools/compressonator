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
#include <float.h>
#include <stdio.h>
#include <math.h>
#include "Common.h"
#include "HDR_Encode.h"
#include "BC6H_Definitions.h"
#include "BC6H_Encode.h"
#include "BC6H_utils.h"

using namespace HDR_Encode;


#define USE_SHAKERHD  // reserved for future use!

BYTE Cmp_Red_Block[16] = { 0xc2,0x7b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0x03,0x00,0x00,0x00,0x00,0x00 };

extern int  g_block;
extern FILE *g_fp;
int gl_block = 0;

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

void SaveDataBlock(AMD_BC6H_Format bc6h_format, BYTE out[BC6H_COMPRESSED_BLOCK_SIZE])
    {
        BitHeader header(NULL, BC6H_COMPRESSED_BLOCK_SIZE);

        // Save the RGB end point values
        switch (bc6h_format.m_mode)
        {
        case 1: //0x00
            header.setvalue(0, 2, 0x00);
            header.setvalue(2, 1, bc6h_format.gy, 4);        //        gy[4]
            header.setvalue(3, 1, bc6h_format.by, 4);        //        by[4]
            header.setvalue(4, 1, bc6h_format.bz, 4);        //        bz[4]
            header.setvalue(5, 10, bc6h_format.rw);          // 10:    rw[9:0]
            header.setvalue(15, 10, bc6h_format.gw);          // 10:    gw[9:0]
            header.setvalue(25, 10, bc6h_format.bw);          // 10:    bw[9:0]
            header.setvalue(35, 5, bc6h_format.rx);          // 5:     rx[4:0]
            header.setvalue(40, 1, bc6h_format.gz, 4);        //        gz[4]
            header.setvalue(41, 4, bc6h_format.gy);          // 5:     gy[3:0]
            header.setvalue(45, 5, bc6h_format.gx);          // 5:     gx[4:0]
            header.setvalue(50, 1, bc6h_format.bz);          // 5:     bz[0]
            header.setvalue(51, 4, bc6h_format.gz);          // 5:     gz[3:0]
            header.setvalue(55, 5, bc6h_format.bx);          // 5:     bx[4:0]
            header.setvalue(60, 1, bc6h_format.bz, 1);        //        bz[1]
            header.setvalue(61, 4, bc6h_format.by);          // 5:     by[3:0]
            header.setvalue(65, 5, bc6h_format.ry);          // 5:     ry[4:0]
            header.setvalue(70, 1, bc6h_format.bz, 2);        //        bz[2]
            header.setvalue(71, 5, bc6h_format.rz);          // 5:     rz[4:0]
            header.setvalue(76, 1, bc6h_format.bz, 3);        //        bz[3]
            break;
        case 2: // 0x01
            header.setvalue(0, 2, 0x01);
            header.setvalue(2, 1, bc6h_format.gy, 5);        //        gy[5]
            header.setvalue(3, 1, bc6h_format.gz, 4);        //        gz[4]
            header.setvalue(4, 1, bc6h_format.gz, 5);        //        gz[5]
            header.setvalue(5, 7, bc6h_format.rw);          //        rw[6:0]
            header.setvalue(12, 1, bc6h_format.bz);          //        bz[0]
            header.setvalue(13, 1, bc6h_format.bz, 1);        //        bz[1]
            header.setvalue(14, 1, bc6h_format.by, 4);        //        by[4]
            header.setvalue(15, 7, bc6h_format.gw);          //        gw[6:0]
            header.setvalue(22, 1, bc6h_format.by, 5);        //        by[5]
            header.setvalue(23, 1, bc6h_format.bz, 2);        //        bz[2]
            header.setvalue(24, 1, bc6h_format.gy, 4);        //        gy[4]
            header.setvalue(25, 7, bc6h_format.bw);          // 7:     bw[6:0]
            header.setvalue(32, 1, bc6h_format.bz, 3);        //        bz[3]
            header.setvalue(33, 1, bc6h_format.bz, 5);        //        bz[5]
            header.setvalue(34, 1, bc6h_format.bz, 4);        //        bz[4]
            header.setvalue(35, 6, bc6h_format.rx);          // 6:     rx[5:0]
            header.setvalue(41, 4, bc6h_format.gy);          // 6:     gy[3:0]
            header.setvalue(45, 6, bc6h_format.gx);          // 6:     gx[5:0]
            header.setvalue(51, 4, bc6h_format.gz);          // 6:     gz[3:0]
            header.setvalue(55, 6, bc6h_format.bx);          // 6:     bx[5:0]
            header.setvalue(61, 4, bc6h_format.by);          // 6:     by[3:0]
            header.setvalue(65, 6, bc6h_format.ry);          // 6:     ry[5:0]
            header.setvalue(71, 6, bc6h_format.rz);          // 6:     rz[5:0]
            break;
        case 3: // 0x02
            header.setvalue(0, 5, 0x02);
            header.setvalue(5, 10, bc6h_format.rw);          // 11:    rw[9:0]
            header.setvalue(15, 10, bc6h_format.gw);          // 11:    gw[9:0]
            header.setvalue(25, 10, bc6h_format.bw);          // 11:    bw[9:0]
            header.setvalue(35, 5, bc6h_format.rx);          // 5:     rx[4:0]
            header.setvalue(40, 1, bc6h_format.rw, 10);       //        rw[10]
            header.setvalue(41, 4, bc6h_format.gy);          // 4:     gy[3:0]
            header.setvalue(45, 4, bc6h_format.gx);          // 4:     gx[3:0]
            header.setvalue(49, 1, bc6h_format.gw, 10);       //        gw[10]
            header.setvalue(50, 1, bc6h_format.bz);          // 4:     bz[0]
            header.setvalue(51, 4, bc6h_format.gz);          // 4:     gz[3:0]
            header.setvalue(55, 4, bc6h_format.bx);          // 4:     bx[3:0]
            header.setvalue(59, 1, bc6h_format.bw, 10);       //        bw[10]
            header.setvalue(60, 1, bc6h_format.bz, 1);        //        bz[1]
            header.setvalue(61, 4, bc6h_format.by);          // 4:     by[3:0]
            header.setvalue(65, 5, bc6h_format.ry);          // 5:     ry[4:0]
            header.setvalue(70, 1, bc6h_format.bz, 2);        //        bz[2]
            header.setvalue(71, 5, bc6h_format.rz);          // 5:     rz[4:0]
            header.setvalue(76, 1, bc6h_format.bz, 3);        //        bz[3]
            break;
        case 4: // 0x06
            header.setvalue(0, 5, 0x06);
            header.setvalue(5, 10, bc6h_format.rw);          // 11:    rw[9:0]
            header.setvalue(15, 10, bc6h_format.gw);          // 11:    gw[9:0]
            header.setvalue(25, 10, bc6h_format.bw);          // 11:    bw[9:0]
            header.setvalue(35, 4, bc6h_format.rx);          //        rx[3:0]
            header.setvalue(39, 1, bc6h_format.rw, 10);       //        rw[10]
            header.setvalue(40, 1, bc6h_format.gz, 4);        //        gz[4]
            header.setvalue(41, 4, bc6h_format.gy);          // 5:     gy[3:0]
            header.setvalue(45, 5, bc6h_format.gx);          //        gx[4:0]
            header.setvalue(50, 1, bc6h_format.gw, 10);       // 5:     gw[10]
            header.setvalue(51, 4, bc6h_format.gz);          // 5:     gz[3:0]
            header.setvalue(55, 4, bc6h_format.bx);          // 4:     bx[3:0]
            header.setvalue(59, 1, bc6h_format.bw, 10);       //        bw[10]
            header.setvalue(60, 1, bc6h_format.bz, 1);        //        bz[1]
            header.setvalue(61, 4, bc6h_format.by);          // 4:     by[3:0]
            header.setvalue(65, 4, bc6h_format.ry);          // 4:     ry[3:0]
            header.setvalue(69, 1, bc6h_format.bz);          // 4:     bz[0]
            header.setvalue(70, 1, bc6h_format.bz, 2);        //        bz[2]
            header.setvalue(71, 4, bc6h_format.rz);          // 4:     rz[3:0]
            header.setvalue(75, 1, bc6h_format.gy, 4);        //        gy[4]
            header.setvalue(76, 1, bc6h_format.bz, 3);        //        bz[3]
            break;
        case 5: // 0x0A
            header.setvalue(0, 5, 0x0A);
            header.setvalue(5, 10, bc6h_format.rw);           // 11:   rw[9:0]
            header.setvalue(15, 10, bc6h_format.gw);           // 11:   gw[9:0]
            header.setvalue(25, 10, bc6h_format.bw);           // 11:   bw[9:0]
            header.setvalue(35, 4, bc6h_format.rx);           // 4:    rx[3:0]
            header.setvalue(39, 1, bc6h_format.rw, 10);        //       rw[10]
            header.setvalue(40, 1, bc6h_format.by, 4);         //       by[4]
            header.setvalue(41, 4, bc6h_format.gy);           // 4:    gy[3:0]
            header.setvalue(45, 4, bc6h_format.gx);           // 4:    gx[3:0]
            header.setvalue(49, 1, bc6h_format.gw, 10);        //       gw[10]
            header.setvalue(50, 1, bc6h_format.bz);           // 5:    bz[0]
            header.setvalue(51, 4, bc6h_format.gz);           // 4:    gz[3:0]
            header.setvalue(55, 5, bc6h_format.bx);           // 5:    bx[4:0]
            header.setvalue(60, 1, bc6h_format.bw, 10);        //       bw[10]
            header.setvalue(61, 4, bc6h_format.by);           // 5:    by[3:0]
            header.setvalue(65, 4, bc6h_format.ry);           // 4:    ry[3:0]
            header.setvalue(69, 1, bc6h_format.bz, 1);         //       bz[1]
            header.setvalue(70, 1, bc6h_format.bz, 2);         //       bz[2]
            header.setvalue(71, 4, bc6h_format.rz);           // 4:    rz[3:0]
            header.setvalue(75, 1, bc6h_format.bz, 4);         //       bz[4]
            header.setvalue(76, 1, bc6h_format.bz, 3);         //       bz[3]
            break;
        case 6: // 0x0E
            header.setvalue(0, 5, 0x0E);
            header.setvalue(5, 9, bc6h_format.rw);           // 9:    rw[8:0]
            header.setvalue(14, 1, bc6h_format.by, 4);         //       by[4]
            header.setvalue(15, 9, bc6h_format.gw);           // 9:    gw[8:0]
            header.setvalue(24, 1, bc6h_format.gy, 4);         //       gy[4]
            header.setvalue(25, 9, bc6h_format.bw);           // 9:    bw[8:0]
            header.setvalue(34, 1, bc6h_format.bz, 4);         //       bz[4]
            header.setvalue(35, 5, bc6h_format.rx);           // 5:    rx[4:0]
            header.setvalue(40, 1, bc6h_format.gz, 4);         //       gz[4]
            header.setvalue(41, 4, bc6h_format.gy);           // 5:    gy[3:0]
            header.setvalue(45, 5, bc6h_format.gx);           // 5:    gx[4:0]
            header.setvalue(50, 1, bc6h_format.bz);           // 5:    bz[0]
            header.setvalue(51, 4, bc6h_format.gz);           // 5:    gz[3:0]
            header.setvalue(55, 5, bc6h_format.bx);           // 5:    bx[4:0]
            header.setvalue(60, 1, bc6h_format.bz, 1);         //       bz[1]
            header.setvalue(61, 4, bc6h_format.by);           // 5:    by[3:0]
            header.setvalue(65, 5, bc6h_format.ry);           // 5:    ry[4:0]
            header.setvalue(70, 1, bc6h_format.bz, 2);         //       bz[2]
            header.setvalue(71, 5, bc6h_format.rz);           // 5:    rz[4:0]
            header.setvalue(76, 1, bc6h_format.bz, 3);         //       bz[3]
            break;
        case 7: // 0x12
            header.setvalue(0, 5, 0x12);
            header.setvalue(5, 8, bc6h_format.rw);           // 8:    rw[7:0]
            header.setvalue(13, 1, bc6h_format.gz, 4);         //       gz[4]
            header.setvalue(14, 1, bc6h_format.by, 4);         //       by[4]
            header.setvalue(15, 8, bc6h_format.gw);           // 8:    gw[7:0]
            header.setvalue(23, 1, bc6h_format.bz, 2);         //       bz[2]
            header.setvalue(24, 1, bc6h_format.gy, 4);         //       gy[4]
            header.setvalue(25, 8, bc6h_format.bw);           // 8:    bw[7:0]
            header.setvalue(33, 1, bc6h_format.bz, 3);         //       bz[3]
            header.setvalue(34, 1, bc6h_format.bz, 4);         //       bz[4]
            header.setvalue(35, 6, bc6h_format.rx);           // 6:    rx[5:0]
            header.setvalue(41, 4, bc6h_format.gy);           // 5:    gy[3:0]
            header.setvalue(45, 5, bc6h_format.gx);           // 5:    gx[4:0]
            header.setvalue(50, 1, bc6h_format.bz);           // 5:    bz[0]
            header.setvalue(51, 4, bc6h_format.gz);           // 5:    gz[3:0]
            header.setvalue(55, 5, bc6h_format.bx);           // 5:    bx[4:0]
            header.setvalue(60, 1, bc6h_format.bz, 1);         //       bz[1]
            header.setvalue(61, 4, bc6h_format.by);           // 5:    by[3:0]
            header.setvalue(65, 6, bc6h_format.ry);           // 6:    ry[5:0]
            header.setvalue(71, 6, bc6h_format.rz);           // 6:    rz[5:0]
            break;
        case 8: // 0x16
            header.setvalue(0, 5, 0x16);
            header.setvalue(5, 8, bc6h_format.rw);            // 8:   rw[7:0]
            header.setvalue(13, 1, bc6h_format.bz);            // 5:   bz[0]
            header.setvalue(14, 1, bc6h_format.by, 4);          //      by[4]
            header.setvalue(15, 8, bc6h_format.gw);            // 8:   gw[7:0]
            header.setvalue(23, 1, bc6h_format.gy, 5);          //      gy[5]
            header.setvalue(24, 1, bc6h_format.gy, 4);          //      gy[4]
            header.setvalue(25, 8, bc6h_format.bw);            // 8:   bw[7:0]
            header.setvalue(33, 1, bc6h_format.gz, 5);          //      gz[5]
            header.setvalue(34, 1, bc6h_format.bz, 4);          //      bz[4]
            header.setvalue(35, 5, bc6h_format.rx);            // 5:   rx[4:0]
            header.setvalue(40, 1, bc6h_format.gz, 4);          //      gz[4]
            header.setvalue(41, 4, bc6h_format.gy);            // 6:   gy[3:0]
            header.setvalue(45, 6, bc6h_format.gx);            // 6:   gx[5:0]
            header.setvalue(51, 4, bc6h_format.gz);            // 6:   gz[3:0]
            header.setvalue(55, 5, bc6h_format.bx);            // 5:   bx[4:0]
            header.setvalue(60, 1, bc6h_format.bz, 1);          //      bz[1]
            header.setvalue(61, 4, bc6h_format.by);            // 5:   by[3:0]
            header.setvalue(65, 5, bc6h_format.ry);            // 5:   ry[4:0]
            header.setvalue(70, 1, bc6h_format.bz, 2);          //      bz[2]
            header.setvalue(71, 5, bc6h_format.rz);            // 5:   rz[4:0]
            header.setvalue(76, 1, bc6h_format.bz, 3);          //      bz[3]
            break;
        case 9: // 0x1A
            header.setvalue(0, 5, 0x1A);
            header.setvalue(5, 8, bc6h_format.rw);            // 8:   rw[7:0]
            header.setvalue(13, 1, bc6h_format.bz, 1);          //      bz[1]
            header.setvalue(14, 1, bc6h_format.by, 4);          //      by[4]
            header.setvalue(15, 8, bc6h_format.gw);            // 8:   gw[7:0]
            header.setvalue(23, 1, bc6h_format.by, 5);          //      by[5]
            header.setvalue(24, 1, bc6h_format.gy, 4);          //      gy[4]
            header.setvalue(25, 8, bc6h_format.bw);            // 8:   bw[7:0]
            header.setvalue(33, 1, bc6h_format.bz, 5);          //      bz[5]
            header.setvalue(34, 1, bc6h_format.bz, 4);          //      bz[4]
            header.setvalue(35, 5, bc6h_format.rx);            // 5:   rx[4:0]
            header.setvalue(40, 1, bc6h_format.gz, 4);          //      gz[4]
            header.setvalue(41, 4, bc6h_format.gy);            // 5:   gy[3:0]
            header.setvalue(45, 5, bc6h_format.gx);            // 5:   gx[4:0]
            header.setvalue(50, 1, bc6h_format.bz);            // 6:   bz[0]
            header.setvalue(51, 4, bc6h_format.gz);            // 5:   gz[3:0]
            header.setvalue(55, 6, bc6h_format.bx);            // 6:   bx[5:0]
            header.setvalue(61, 4, bc6h_format.by);            // 6:   by[3:0]
            header.setvalue(65, 5, bc6h_format.ry);            // 5:   ry[4:0]
            header.setvalue(70, 1, bc6h_format.bz, 2);          //      bz[2]
            header.setvalue(71, 5, bc6h_format.rz);            // 5:   rz[4:0]
            header.setvalue(76, 1, bc6h_format.bz, 3);          //      bz[3]
            break;
        case 10: // 0x1E
            header.setvalue(0, 5, 0x1E);
            header.setvalue(5, 6, bc6h_format.rw);            // 6:   rw[5:0]
            header.setvalue(11, 1, bc6h_format.gz, 4);          //      gz[4]
            header.setvalue(12, 1, bc6h_format.bz);            // 6:   bz[0]
            header.setvalue(13, 1, bc6h_format.bz, 1);          //      bz[1]
            header.setvalue(14, 1, bc6h_format.by, 4);          //      by[4]
            header.setvalue(15, 6, bc6h_format.gw);            // 6:   gw[5:0]
            header.setvalue(21, 1, bc6h_format.gy, 5);          //      gy[5]
            header.setvalue(22, 1, bc6h_format.by, 5);          //      by[5]
            header.setvalue(23, 1, bc6h_format.bz, 2);          //      bz[2]
            header.setvalue(24, 1, bc6h_format.gy, 4);          //      gy[4]
            header.setvalue(25, 6, bc6h_format.bw);            // 6:   bw[5:0]
            header.setvalue(31, 1, bc6h_format.gz, 5);          //      gz[5]
            header.setvalue(32, 1, bc6h_format.bz, 3);          //      bz[3]
            header.setvalue(33, 1, bc6h_format.bz, 5);          //      bz[5]
            header.setvalue(34, 1, bc6h_format.bz, 4);          //      bz[4]
            header.setvalue(35, 6, bc6h_format.rx);            // 6:   rx[5:0]
            header.setvalue(41, 4, bc6h_format.gy);            // 6:   gy[3:0]
            header.setvalue(45, 6, bc6h_format.gx);            // 6:   gx[5:0]
            header.setvalue(51, 4, bc6h_format.gz);            // 6:   gz[3:0]
            header.setvalue(55, 6, bc6h_format.bx);            // 6:   bx[5:0]
            header.setvalue(61, 4, bc6h_format.by);            // 6:   by[3:0]
            header.setvalue(65, 6, bc6h_format.ry);            // 6:   ry[5:0]
            header.setvalue(71, 6, bc6h_format.rz);            // 6:   rz[5:0]
            break;

            // Single regions Modes
        case 11: // 0x03
            header.setvalue(0, 5, 0x03);
            header.setvalue(5, 10, bc6h_format.rw);            // 10:   rw[9:0]
            header.setvalue(15, 10, bc6h_format.gw);            // 10:   gw[9:0]
            header.setvalue(25, 10, bc6h_format.bw);            // 10:   bw[9:0]
            header.setvalue(35, 10, bc6h_format.rx);            // 10:   rx[9:0]
            header.setvalue(45, 10, bc6h_format.gx);            // 10:   gx[9:0]
            header.setvalue(55, 10, bc6h_format.bx);            // 10:   bx[9:0]
            break;
        case 12: // 0x07
            header.setvalue(0, 5, 0x07);
            header.setvalue(5, 10, bc6h_format.rw);            // 11:   rw[9:0]
            header.setvalue(15, 10, bc6h_format.gw);            // 11:   gw[9:0]
            header.setvalue(25, 10, bc6h_format.bw);            // 11:   bw[9:0]
            header.setvalue(35, 9, bc6h_format.rx);            // 9:    rx[8:0]
            header.setvalue(44, 1, bc6h_format.rw, 10);         //       rw[10]
            header.setvalue(45, 9, bc6h_format.gx);            // 9:    gx[8:0]
            header.setvalue(54, 1, bc6h_format.gw, 10);         //       gw[10]
            header.setvalue(55, 9, bc6h_format.bx);            // 9:    bx[8:0]
            header.setvalue(64, 1, bc6h_format.bw, 10);         //       bw[10]
            break;
        case 13: // 0x0B
            header.setvalue(0, 5, 0x0B);
            header.setvalue(5, 10, bc6h_format.rw);            // 12:   rw[9:0]
            header.setvalue(15, 10, bc6h_format.gw);            // 12:   gw[9:0]
            header.setvalue(25, 10, bc6h_format.bw);            // 12:   bw[9:0]
            header.setvalue(35, 8, bc6h_format.rx);            // 8:    rx[7:0]
            header.setvalue(43, 1, bc6h_format.rw, 11);         //       rw[11]
            header.setvalue(44, 1, bc6h_format.rw, 10);         //       rw[10]
            header.setvalue(45, 8, bc6h_format.gx);            // 8:    gx[7:0]
            header.setvalue(53, 1, bc6h_format.gw, 11);         //       gw[11]
            header.setvalue(54, 1, bc6h_format.gw, 10);         //       gw[10]
            header.setvalue(55, 8, bc6h_format.bx);            // 8:    bx[7:0]
            header.setvalue(63, 1, bc6h_format.bw, 11);         //       bw[11]
            header.setvalue(64, 1, bc6h_format.bw, 10);         //       bw[10]
            break;
        case 14: // 0x0F
            header.setvalue(0, 5, 0x0F);
            header.setvalue(5, 10, bc6h_format.rw);            // 16:   rw[9:0]
            header.setvalue(15, 10, bc6h_format.gw);            // 16:   gw[9:0]
            header.setvalue(25, 10, bc6h_format.bw);            // 16:   bw[9:0]
            header.setvalue(35, 4, bc6h_format.rx);            //  4:   rx[3:0]
            header.setvalue(39, 6, bc6h_format.rw, 10);         //       rw[15:10]
            header.setvalue(45, 4, bc6h_format.gx);            //  4:   gx[3:0]
            header.setvalue(49, 6, bc6h_format.gw, 10);         //       gw[15:10]
            header.setvalue(55, 4, bc6h_format.bx);            //  4:   bx[3:0]
            header.setvalue(59, 6, bc6h_format.bw, 10);         //       bw[15:10]
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
            header.setvalue(startbit, 3, bc6h_format.indices16[0]);
            startbit += 3;
            for (int i = 1; i<16; i++)
            {
                header.setvalue(startbit, 4, bc6h_format.indices16[i]);
                startbit += 4;
            }
        }
        else
        {
            header.setvalue(77, 5, bc6h_format.d_shape_index);            // Shape Index
            int startbit = TWO_REGION_INDEX_OFFSET,
                nbits = 2;
            header.setvalue(startbit, nbits, bc6h_format.indices16[0]);
            for (int i = 1; i<16; i++)
            {
                startbit += nbits; // offset start bit for next index using prior nbits used
                nbits = g_indexfixups[bc6h_format.d_shape_index] == i ? 2 : 3; // get new number of bit to save index with
                header.setvalue(startbit, nbits, bc6h_format.indices16[i]);
            }
        }

        // save to output buffer our new bit values
        // this can be optimized if header is part of bc6h_format struct
        header.transferbits(out, 16);

    }


// decompress endpoints
static void decompress_endpts(const int in[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int out[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], const int mode, bool issigned)
{

    if (ModePartition[mode].transformed)
    {
        for (int i=0; i<3; ++i)
        {
            R_0(out) = issigned ? SIGN_EXTEND(R_0(in),ModePartition[mode].nbits) : R_0(in);
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

// endpoints fit only if the compression was lossless
static bool endpts_fit(const int orig[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], const int compressed[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], const int mode, int max_subsets, bool issigned)
{
    int uncompressed[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG];

    decompress_endpts(compressed, uncompressed, mode, issigned);

    for (int j=0; j<max_subsets; ++j)
    for (int i=0; i<3; ++i)
    {
        if (orig[j][0][i] != uncompressed[j][0][i]) return false;
        if (orig[j][1][i] != uncompressed[j][1][i]) return false;
    }

    return true;
}

// Dont know exact limits : for now just say is -2.0 to +2.0
void BC6HBlockEncoder::clampF16Max(float EndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG])
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
                // Zero region
                // if ((EndPoints[region][ab][rgb] > -0.01) && ((EndPoints[region][ab][rgb] < 0.01))) EndPoints[region][ab][rgb] = 0.0;
            }
}

/*=================================================================
    Quantize Endpoints
    for a given mode
==================================================================*/

void BC6HBlockEncoder::QuantizeEndPointToF16Prec(float EndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int iEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int max_subsets, int prec)
{

    for (int subset = 0; subset < max_subsets; ++subset)
    {
        iEndPoints[subset][0][0] = QuantizeToInt((short)EndPoints[subset][0][0],prec,m_isSigned, m_Exposure);    // A.Red
        iEndPoints[subset][0][1] = QuantizeToInt((short)EndPoints[subset][0][1],prec,m_isSigned, m_Exposure);    // A.Green
        iEndPoints[subset][0][2] = QuantizeToInt((short)EndPoints[subset][0][2],prec,m_isSigned, m_Exposure);    // A.Blue
        iEndPoints[subset][1][0] = QuantizeToInt((short)EndPoints[subset][1][0],prec,m_isSigned, m_Exposure);    // B.Red
        iEndPoints[subset][1][1] = QuantizeToInt((short)EndPoints[subset][1][1],prec,m_isSigned, m_Exposure);    // B.Green
        iEndPoints[subset][1][2] = QuantizeToInt((short)EndPoints[subset][1][2],prec,m_isSigned, m_Exposure);    // B.Blue
    }
}

/*=================================================================
    Swap Indices
    so that indices at fix up points have higher order bit set to 0
==================================================================*/

void BC6HBlockEncoder::SwapIndices(int iEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int iIndices[3][BC6H_MAX_SUBSET_SIZE], int  entryCount[BC6H_MAX_SUBSETS], int max_subsets, int mode, int shape_pattern)
{

    unsigned int uNumIndices    = 1 << ModePartition[mode].IndexPrec;
    unsigned int uHighIndexBit    = uNumIndices >> 1;

    for(int subset = 0; subset < max_subsets; ++subset)
    {
        // region 0 (subset = 0) The fix-up index for this subset is allways index 0
        // region 1 (subset = 1) The fix-up index for this subset varies based on the shape
        size_t i = subset?g_Region2FixUp[shape_pattern]:0;

        if(iIndices[subset][i] & uHighIndexBit)
        {
            // high bit is set, swap the aEndPts and indices for this region
            std::swap(iEndPoints[subset][0][0], iEndPoints[subset][1][0]);
            std::swap(iEndPoints[subset][0][1], iEndPoints[subset][1][1]);
            std::swap(iEndPoints[subset][0][2], iEndPoints[subset][1][2]);

            for(size_t j = 0; j < (size_t)entryCount[subset]; ++j)
            {
                iIndices[subset][j] = uNumIndices - 1 - iIndices[subset][j] ;
            }
        }

    }
}


/*=================================================================
    Tranforms according to shape precission
==================================================================*/
// helper function to check transform overflow
bool isOverflow(int endpoint, int nbit, bool bIsSigned)
{
    if (bIsSigned)
    {
        int nbRequired; //bits required for the encode
        int nb;
        if (endpoint == 0)
        {
            return false; // no overflow
        }
        else if (endpoint > 0)
        {
            for (nb = 0; endpoint; ++nb, endpoint >>= 1);
            nbRequired = nb + (bIsSigned ? 1 : 0);
            if (nbRequired > nbit) //overflow
                return true;
        }
        else //negative endpoints
        {
            if (!bIsSigned) return true;

            for (nb = 0; endpoint < -1; ++nb, endpoint >>= 1);
            nbRequired = nb + 1;
            if (nbRequired > nbit) //overflow
                return true;
        }

        return false;
    }
    else
    {
        int maxRange = (int)pow(2, nbit - 1) - 1;
        int minRange = (int)-(pow(2, nbit - 1));

        //no overflow
        if ((endpoint >= minRange) && (endpoint <= maxRange))
            return false;
        else //overflow
            return true;
    }
}

// Bug in this code : Need to add signed bit to values
bool BC6HBlockEncoder::TransformEndPoints(AMD_BC6H_Format &BC6H_data, int iEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int oEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG],int max_subsets, int mode)
{
    int Mask;
    if ( ModePartition[mode].transformed)
    {
        BC6H_data.istransformed = true;
        for (int i=0; i<3; ++i)
        {
            Mask = MASK(ModePartition[mode].nbits);
            oEndPoints[0][0][i] = iEndPoints[0][0][i] & Mask;    // [0][A]

            Mask = MASK(ModePartition[mode].prec[i]);
            oEndPoints[0][1][i] = iEndPoints[0][1][i]- iEndPoints[0][0][i]; // [0][B] - [0][A]

            if (isOverflow(oEndPoints[0][1][i], ModePartition[mode].prec[i], BC6H_data.issigned))
                return false;

            oEndPoints[0][1][i] = (oEndPoints[0][1][i] & Mask);

            //redo the check for sign overflow for one region case
            if (max_subsets <= 1)
            {
                if (isOverflow(oEndPoints[0][1][i], ModePartition[mode].prec[i], BC6H_data.issigned))
                    return false;
            }

            if (max_subsets > 1)
            {
                oEndPoints[1][0][i] = iEndPoints[1][0][i] - iEndPoints[0][0][i];  // [1][A] - [0][A]
                if (isOverflow(oEndPoints[1][0][i], ModePartition[mode].prec[i], BC6H_data.issigned))
                    return false;

                oEndPoints[1][0][i] = (oEndPoints[1][0][i] & Mask);

                oEndPoints[1][1][i] = iEndPoints[1][1][i] - iEndPoints[0][0][i];  // [1][B] - [0][A]
                if (isOverflow(oEndPoints[1][1][i], ModePartition[mode].prec[i], BC6H_data.issigned))
                    return false;

                oEndPoints[1][1][i] = (oEndPoints[1][1][i] & Mask);
            }
        }
    }
    else
    {
        BC6H_data.istransformed = false;
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

    return true;
}


void BC6HBlockEncoder::SaveCompressedBlockData( AMD_BC6H_Format &BC6H_data,
                                            int oEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG],
                                            int iIndices[2][BC6H_MAX_SUBSET_SIZE],
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
                asubset                = PARTITIONS[1][BC6H_data.d_shape_index][i]; // Two region shapes
            else
                asubset                = PARTITIONS[0][BC6H_data.d_shape_index][i]; // One region shapes
            BC6H_data.indices16[i]    = (std::uint8_t)iIndices[asubset][pos[asubset]];
            pos[asubset]++;
        }

}


void palitizeEndPointsF(AMD_BC6H_Format &BC6H_data, float fEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG])
{
    // scale endpoints
    float  Ar,Ag,Ab, Br,Bg,Bb;


    // Compose index colors from end points
    if (BC6H_data.region == 1)
    {
        Ar = fEndPoints[0][0][0];
        Ag = fEndPoints[0][0][1];
        Ab = fEndPoints[0][0][2];
        Br = fEndPoints[0][1][0];
        Bg = fEndPoints[0][1][1];
        Bb = fEndPoints[0][1][2];

        for (int i = 0; i < 16; i++)
        {

            // Red
            BC6H_data.Paletef[0][i].x = lerpf(Ar, Br, i, 15);
            // Green
            BC6H_data.Paletef[0][i].y = lerpf(Ag, Bg, i, 15);
            // Blue
            BC6H_data.Paletef[0][i].z = lerpf(Ab, Bb, i, 15);
        }

    }
    else //mode.type == BC6_TWO
    {
        for (int region = 0; region<2; region++)
        {
            Ar = fEndPoints[region][0][0];
            Ag = fEndPoints[region][0][1];
            Ab = fEndPoints[region][0][2];
            Br = fEndPoints[region][1][0];
            Bg = fEndPoints[region][1][1];
            Bb = fEndPoints[region][1][2];
            for (int i = 0; i < 8; i++)
            {
                // Red
                BC6H_data.Paletef[region][i].x = lerpf(Ar, Br, i, 7);
                // Greed
                BC6H_data.Paletef[region][i].y = lerpf(Ag, Bg, i, 7);
                // Blue
                BC6H_data.Paletef[region][i].z = lerpf(Ab, Bb, i, 7);
            }

        }
    }
}

float CalcOneRegionEndPtsError(AMD_BC6H_Format &BC6H_data, float fEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int shape_indices[MAX_SUBSETS][MAX_SUBSET_SIZE])
{
    float error = 0;

    for (int i = 0; i < BC6H_MAX_SUBSET_SIZE; i++)
    {
        for (int m = 0; m < MAX_END_POINTS; m++)
        {
            for (int n = 0; n < NCHANNELS; n++)
            {
                float calencpts = fEndPoints[0][m][n] + (abs(fEndPoints[0][m][n] - fEndPoints[0][m][n]) * (shape_indices[0][i] / 15));
                error += abs(BC6H_data.din[i][n] - calencpts);
            }
        }
    }

    return error;
}

float CalcShapeError(AMD_BC6H_Format &BC6H_data, float fEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], bool SkipPallet)
{
    int maxPallet;
    int subset = 0;
    float totalError = 0.0f;
    int region = (BC6H_data.region - 1);

    if (region == 0)
        maxPallet = 16;
    else
        maxPallet = 8;

    if (!SkipPallet)
        palitizeEndPointsF(BC6H_data, fEndPoints);

    for (int i =0; i < MAX_SUBSET_SIZE; i++)
    {
        float error = 0.0f;
        float bestError = 0.0f;

        if (region == 0)
        {
            subset = 0;
        }
        else
        {
            //subset 0 or subset 1
            subset = PARTITIONS[region][BC6H_data.d_shape_index][i];
        }

        // initialize bestError to the difference for first data
        bestError = abs(BC6H_data.din[i][0] - BC6H_data.Paletef[subset][0].x) +
            abs(BC6H_data.din[i][1] - BC6H_data.Paletef[subset][0].y) +
            abs(BC6H_data.din[i][2] - BC6H_data.Paletef[subset][0].z);

        // loop through the rest of the data until find the best error
        for (int j = 1; j < maxPallet && bestError > 0; j++)
        {
            error = abs(BC6H_data.din[i][0] - BC6H_data.Paletef[subset][j].x) +
                abs(BC6H_data.din[i][1] - BC6H_data.Paletef[subset][j].y) +
                abs(BC6H_data.din[i][2] - BC6H_data.Paletef[subset][j].z);

            if (error <= bestError)
                bestError = error;
            else
                break;
        }
        totalError += bestError;
    }

    return totalError;
}

void ReIndexShapef(AMD_BC6H_Format &BC6H_data, int shape_indices[BC6H_MAX_SUBSETS][MAX_SUBSET_SIZE])
{
    float error = 0;
    float bestError;
    int bestIndex = 0;
    int sub0index = 0;
    int sub1index = 0;
    int MaxPallet;
    int region = (BC6H_data.region - 1);

    if (region == 0)
        MaxPallet = 16;
    else
        MaxPallet = 8;

    for (int i = 0; i < BC6H_MAX_SUBSET_SIZE; i++)
    {
        // subset 0 or subset 1
        if (PARTITIONS[region][BC6H_data.d_shape_index][i])
        {
            bestError = FLT_MAX;
            bestIndex = 0;

            // For two shape regions max Pallet is 8
            for (int j = 0; j < MaxPallet; j++)
            {
                // Calculate error from original
                error = abs(BC6H_data.din[i][0] - BC6H_data.Paletef[1][j].x) +
                        abs(BC6H_data.din[i][1] - BC6H_data.Paletef[1][j].y) +
                        abs(BC6H_data.din[i][2] - BC6H_data.Paletef[1][j].z);
                if (error < bestError)
                {
                    bestError = error;
                    bestIndex = j;
                }
            }

            shape_indices[1][sub1index] = bestIndex;
            sub1index++;
        }
        else
        {
            // This is shared for one or two shape regions max Pallet either 16 or 8
            bestError = FLT_MAX;
            bestIndex = 0;

            for (int j = 0; j < MaxPallet; j++)
            {
                // Calculate error from original
                error = abs(BC6H_data.din[i][0] - BC6H_data.Paletef[0][j].x) +
                        abs(BC6H_data.din[i][1] - BC6H_data.Paletef[0][j].y) +
                        abs(BC6H_data.din[i][2] - BC6H_data.Paletef[0][j].z);
                if (error < bestError)
                {
                    bestError = error;
                    bestIndex = j;
                }
            }

            shape_indices[0][sub0index] = bestIndex;
            sub0index++;
        }
    }

}

float    BC6HBlockEncoder::FindBestPattern(AMD_BC6H_Format &BC6H_data,
                          bool TwoRegionShapes,
                          int shape_pattern)
{
    // Index bit size for the patterns been used.
    // All two zone shapes have 3 bits per color, max index value < 8
    // All one zone shapes gave 4 bits per color, max index value < 16
    int        Index_BitSize = TwoRegionShapes ? 8 : 16;
    int     max_subsets = TwoRegionShapes ? 2 : 1;
    float  direction[NCHANNELS];
    float  step;

    BC6H_data.region    = (unsigned short)max_subsets;
    BC6H_data.index        = 0;
    BC6H_data.d_shape_index = (unsigned short)shape_pattern;
    memset(BC6H_data.partition, 0, sizeof(BC6H_data.partition));
    memset(BC6H_data.shape_indices, 0, sizeof(BC6H_data.shape_indices));

    // Get the pattern to encode with
    Partition( shape_pattern,          // Shape pattern we want to get
               BC6H_data.din,          // Input data
               BC6H_data.partition,    // Returns the patterned shape data
               BC6H_data.entryCount,   // counts the number of pixel used in each subset region num of 0's amd 1's
               max_subsets,            // Table Shapes to use eithe one regions 1 or two regions 2
               3);                     // rgb no alpha always = 3


    float  error[MAX_SUBSETS] = { 0.0,FLT_MAX,FLT_MAX };
    int    BestOutB = 0;
    float  BestError;        //the lowest error from vector direction quantization
    float  BestError_endpts; //the lowest error from endpoints extracted from the vector direction quantization

    float  outB[2][2][MAX_SUBSET_SIZE][MAX_DIMENSION_BIG];
    int     shape_indicesB[2][MAX_SUBSETS][MAX_SUBSET_SIZE];

    for (int subset = 0; subset < max_subsets; subset++)
    {
        error[0] += optQuantAnD_d(
            BC6H_data.partition[subset],        // input data
            BC6H_data.entryCount[subset],       // number of input points above (not clear about 1, better to avoid)
            Index_BitSize,                      // number of clusters on the ramp, 8  or 16
            shape_indicesB[0][subset],          // output index, if not all points of the ramp used, 0 may not be assigned
            outB[0][subset],                    // resulting quantization
            direction,                          // direction vector of the ramp (check normalization)
            &step,                              // step size (check normalization)
            3,                                  // number of channels (always 3 = RGB for BC6H)
            m_quality                           // Quality set number of retry to get good end points
                                                // Max retries = MAX_TRY = 4000 when Quality is 1.0
                                                // Min = 0 and default with quality 0.05 is 200 times
            );
   }

    BestError = error[0];
    BestOutB  = 0;

    // The following code is almost complete - runs very slow and not sure if % of improvement is justified..
#ifdef USE_SHAKERHD
    // Valid only for 2 region shapes
    if ((max_subsets > 1) && (m_quality > 0.80))
    {
        int     tempIndices[MAX_SUBSET_SIZE];
        // int     temp_epo_code[2][2][MAX_DIMENSION_BIG];
        int     bits[3] = { 8,8,8 };     // Channel index bit size

        // float   epo[2][MAX_DIMENSION_BIG];
        int     epo_code[MAX_SUBSETS][2][MAX_DIMENSION_BIG];
        // int     shakeSize = 8;

        error[1] = 0.0;
        for (int subset = 0; subset < max_subsets; subset++)
        {
            for (int k = 0; k < BC6H_data.entryCount[subset]; k++)
            {
                tempIndices[k] = shape_indicesB[0][subset][k];
            }

             error[1] += ep_shaker_HD(
                 BC6H_data.partition[subset],
                 BC6H_data.entryCount[subset],
                 tempIndices,                    // output index, if not all points of the ramp used, 0 may not be assigned
                 outB[1][subset],                // resulting quantization
                 epo_code[subset],
                 BC6H_data.entryCount[subset] - 1,
                 bits,
                 3
             );

            // error[1] += ep_shaker_2_d(
            //      BC6H_data.partition[subset],
            //      BC6H_data.entryCount[subset],
            //      tempIndices,                    // output index, if not all points of the ramp used, 0 may not be assigned
            //      outB[1][subset],                // resulting quantization
            //      epo_code[subset],
            //      shakeSize,
            //      BC6H_data.entryCount[subset] - 1,
            //      bits[0],
            //      3,
            //      epo
            //      );


            for (int k = 0; k < BC6H_data.entryCount[subset]; k++)
            {
                shape_indicesB[1][subset][k] = tempIndices[k];
            }

        } // subsets

        if (BestError > error[1])
        {
            BestError = error[1];
            BestOutB = 1;
            for (int subset = 0; subset < max_subsets; subset++)
            {
                for (int k = 0; k < MAX_DIMENSION_BIG; k++)
                {
                    BC6H_data.fEndPoints[subset][0][k] = (float)epo_code[subset][0][k];
                    BC6H_data.fEndPoints[subset][1][k] = (float)epo_code[subset][1][k];
                }
            }
        }

    }
#endif

    // Save the best for BC6H data processing later
    if (BestOutB == 0)
        GetEndPoints(BC6H_data.fEndPoints, outB[BestOutB], max_subsets, BC6H_data.entryCount);

    memcpy(BC6H_data.shape_indices, shape_indicesB[BestOutB], sizeof(BC6H_data.shape_indices));
    clampF16Max(BC6H_data.fEndPoints);

    BestError_endpts = CalcShapeError(BC6H_data, BC6H_data.fEndPoints, false);
    return BestError_endpts;
}

int finish_unquantizeF16(int q, bool isSigned)
{
    // Is it F16 Signed else F16 Unsigned
    if (isSigned)
        return (q < 0) ? -(((-q) * 31) >> 5) : (q * 31) >> 5;       // scale the magnitude by 31/32
    else
        return (q * 31) >> 6;                                       // scale the magnitude by 31/64

    // Note for Undefined we should return q as is

}

void decompress_endpoints1(AMD_BC6H_Format& bc6h_format, int oEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], float outf[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int mode)
{
    int i;
    int t;
    int out[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG];

    if (bc6h_format.issigned)
    {
        if (bc6h_format.istransformed)
        {
            for (i = 0; i<NCHANNELS; i++)
            {
                out[0][0][i] = SIGN_EXTEND(oEndPoints[0][0][i], ModePartition[mode].nbits);

                t = SIGN_EXTEND(oEndPoints[0][1][i], ModePartition[mode].prec[i]); //C_RED
                t = (t + oEndPoints[0][0][i]) & MASK(ModePartition[mode].nbits);
                out[0][1][i] = SIGN_EXTEND(t, ModePartition[mode].nbits);

                // Unquantize all points to nbits
                out[0][0][i] = Unquantize(out[0][0][i], (unsigned char)ModePartition[mode].nbits, true);
                out[0][1][i] = Unquantize(out[0][1][i], (unsigned char)ModePartition[mode].nbits, true);

                // F16 format
                outf[0][0][i] = (float)finish_unquantizeF16(out[0][0][i], true);
                outf[0][1][i] = (float)finish_unquantizeF16(out[0][1][i], true);
            }
        }
        else
        {
            for (i = 0; i<NCHANNELS; i++)
            {
                out[0][0][i] = SIGN_EXTEND(oEndPoints[0][0][i], ModePartition[mode].nbits);
                out[0][1][i] = SIGN_EXTEND(oEndPoints[0][1][i], ModePartition[mode].prec[i]);

                // Unquantize all points to nbits
                out[0][0][i] = Unquantize(out[0][0][i], (unsigned char)ModePartition[mode].nbits, true);
                out[0][1][i] = Unquantize(out[0][1][i], (unsigned char)ModePartition[mode].nbits, true);

                // F16 format
                outf[0][0][i] = (float)finish_unquantizeF16(out[0][0][i], true);
                outf[0][1][i] = (float)finish_unquantizeF16(out[0][1][i], true);
            }
        }

    }
    else
    {
        if (bc6h_format.istransformed)
        {
            for (i = 0; i<NCHANNELS; i++)
            {
                out[0][0][i] = oEndPoints[0][0][i];
                t = SIGN_EXTEND(oEndPoints[0][1][i], ModePartition[mode].prec[i]);
                out[0][1][i] = (t + oEndPoints[0][0][i]) & MASK(ModePartition[mode].nbits);

                // Unquantize all points to nbits
                out[0][0][i] = Unquantize(out[0][0][i], (unsigned char)ModePartition[mode].nbits, false);
                out[0][1][i] = Unquantize(out[0][1][i], (unsigned char)ModePartition[mode].nbits, false);

                // F16 format
                outf[0][0][i] = (float)finish_unquantizeF16(out[0][0][i], false);
                outf[0][1][i] = (float)finish_unquantizeF16(out[0][1][i], false);
            }
        }
        else
        {
            for (i = 0; i<NCHANNELS; i++)
            {
                out[0][0][i] = oEndPoints[0][0][i];
                out[0][1][i] = oEndPoints[0][1][i];

                // Unquantize all points to nbits
                out[0][0][i] = Unquantize(out[0][0][i], (unsigned char)ModePartition[mode].nbits, false);
                out[0][1][i] = Unquantize(out[0][1][i], (unsigned char)ModePartition[mode].nbits, false);

                // F16 format
                outf[0][0][i] = (float)finish_unquantizeF16(out[0][0][i], false);
                outf[0][1][i] = (float)finish_unquantizeF16(out[0][1][i], false);
            }
        }
    }
}

void decompress_endpoints2(AMD_BC6H_Format& bc6h_format, int oEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], float outf[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int mode)
{
    int i;
    int t;
    int out[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG];

    if (bc6h_format.issigned)
    {
        if (bc6h_format.istransformed)
        {
            for (i = 0; i<NCHANNELS; i++)
            {
                // get the quantized values
                out[0][0][i] = SIGN_EXTEND(oEndPoints[0][0][i], ModePartition[mode].nbits);

                t = SIGN_EXTEND(oEndPoints[0][1][i], ModePartition[mode].prec[i]);
                t = (t + oEndPoints[0][0][i]) & MASK(ModePartition[mode].nbits);
                out[0][1][i] = SIGN_EXTEND(t, ModePartition[mode].nbits);

                t = SIGN_EXTEND(oEndPoints[1][0][i], ModePartition[mode].prec[i]);
                t = (t + oEndPoints[0][0][i]) & MASK(ModePartition[mode].nbits);
                out[1][0][i] = SIGN_EXTEND(t, ModePartition[mode].nbits);

                t = SIGN_EXTEND(oEndPoints[1][1][i], ModePartition[mode].prec[i]);
                t = (t + oEndPoints[0][0][i]) & MASK(ModePartition[mode].nbits);
                out[1][1][i] = SIGN_EXTEND(t, ModePartition[mode].nbits);

                // Unquantize all points to nbits
                out[0][0][i] = Unquantize(out[0][0][i], (unsigned char)ModePartition[mode].nbits, true);
                out[0][1][i] = Unquantize(out[0][1][i], (unsigned char)ModePartition[mode].nbits, true);
                out[1][0][i] = Unquantize(out[1][0][i], (unsigned char)ModePartition[mode].nbits, true);
                out[1][1][i] = Unquantize(out[1][1][i], (unsigned char)ModePartition[mode].nbits, true);

                // F16 format
                outf[0][0][i] = (float)finish_unquantizeF16(out[0][0][i], true);
                outf[0][1][i] = (float)finish_unquantizeF16(out[0][1][i], true);
                outf[1][0][i] = (float)finish_unquantizeF16(out[1][0][i], true);
                outf[1][1][i] = (float)finish_unquantizeF16(out[1][1][i], true);

            }
        }
        else
        {
            for (i = 0; i<NCHANNELS; i++)
            {
                out[0][0][i] = SIGN_EXTEND(oEndPoints[0][0][i], ModePartition[mode].nbits);
                out[0][1][i] = SIGN_EXTEND(oEndPoints[0][1][i], ModePartition[mode].prec[i]);
                out[1][0][i] = SIGN_EXTEND(oEndPoints[1][0][i], ModePartition[mode].prec[i]);
                out[1][1][i] = SIGN_EXTEND(oEndPoints[1][1][i], ModePartition[mode].prec[i]);

                // Unquantize all points to nbits
                out[0][0][i] = Unquantize(out[0][0][i], (unsigned char)ModePartition[mode].nbits, true);
                out[0][1][i] = Unquantize(out[0][1][i], (unsigned char)ModePartition[mode].nbits, true);
                out[1][0][i] = Unquantize(out[1][0][i], (unsigned char)ModePartition[mode].nbits, true);
                out[1][1][i] = Unquantize(out[1][1][i], (unsigned char)ModePartition[mode].nbits, true);

                // nbits to F16 format
                outf[0][0][i] = (float)finish_unquantizeF16(out[0][0][i], true);
                outf[0][1][i] = (float)finish_unquantizeF16(out[0][1][i], true);
                outf[1][0][i] = (float)finish_unquantizeF16(out[1][0][i], true);
                outf[1][1][i] = (float)finish_unquantizeF16(out[1][1][i], true);
            }
        }

    }
    else
    {
        if (bc6h_format.istransformed)
        {
            for (i = 0; i<NCHANNELS; i++)
            {
                out[0][0][i] = oEndPoints[0][0][i];
                t = SIGN_EXTEND(oEndPoints[0][1][i], ModePartition[mode].prec[i]);
                out[0][1][i] = (t + oEndPoints[0][0][i]) & MASK(ModePartition[mode].nbits);

                t = SIGN_EXTEND(oEndPoints[1][0][i], ModePartition[mode].prec[i]);
                out[1][0][i] = (t + oEndPoints[0][0][i]) & MASK(ModePartition[mode].nbits);

                t = SIGN_EXTEND(oEndPoints[1][1][i], ModePartition[mode].prec[i]);
                out[1][1][i] = (t + oEndPoints[0][0][i]) & MASK(ModePartition[mode].nbits);

                // Unquantize all points to nbits
                out[0][0][i] = Unquantize(out[0][0][i], (unsigned char)ModePartition[mode].nbits, false);
                out[0][1][i] = Unquantize(out[0][1][i], (unsigned char)ModePartition[mode].nbits, false);
                out[1][0][i] = Unquantize(out[1][0][i], (unsigned char)ModePartition[mode].nbits, false);
                out[1][1][i] = Unquantize(out[1][1][i], (unsigned char)ModePartition[mode].nbits, false);

                // nbits to F16 format
                outf[0][0][i] = (float)finish_unquantizeF16(out[0][0][i], false);
                outf[0][1][i] = (float)finish_unquantizeF16(out[0][1][i], false);
                outf[1][0][i] = (float)finish_unquantizeF16(out[1][0][i], false);
                outf[1][1][i] = (float)finish_unquantizeF16(out[1][1][i], false);

            }
        }
        else
        {
            for (i = 0; i<NCHANNELS; i++)
            {
                out[0][0][i] = oEndPoints[0][0][i];
                out[0][1][i] = oEndPoints[0][1][i];
                out[1][0][i] = oEndPoints[1][0][i];
                out[1][1][i] = oEndPoints[1][1][i];

                // Unquantize all points to nbits
                out[0][0][i] = Unquantize(out[0][0][i], (unsigned char) ModePartition[mode].nbits, false);
                out[0][1][i] = Unquantize(out[0][1][i], (unsigned char) ModePartition[mode].nbits, false);
                out[1][0][i] = Unquantize(out[1][0][i], (unsigned char) ModePartition[mode].nbits, false);
                out[1][1][i] = Unquantize(out[1][1][i], (unsigned char) ModePartition[mode].nbits, false);

                // nbits to F16 format
                outf[0][0][i] = (float) finish_unquantizeF16(out[0][0][i], false);
                outf[0][1][i] = (float) finish_unquantizeF16(out[0][1][i], false);
                outf[1][0][i] = (float) finish_unquantizeF16(out[1][0][i], false);
                outf[1][1][i] = (float) finish_unquantizeF16(out[1][1][i], false);
            }
        }
    }
}

void BC6HBlockEncoder::AverageEndPoint(float EndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], float fEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int max_subsets, int mode)
{

    if (ModePartition[mode].nbits > 7)
    {
        for (int subset = 0; subset < max_subsets; ++subset)
        {
            fEndPoints[subset][0][0] = EndPoints[subset][0][0];    // A.Red
            fEndPoints[subset][0][1] = EndPoints[subset][0][1];    // A.Green
            fEndPoints[subset][0][2] = EndPoints[subset][0][2];    // A.Blue
            fEndPoints[subset][1][0] = EndPoints[subset][1][0];    // A.Red
            fEndPoints[subset][1][1] = EndPoints[subset][1][1];    // A.Green
            fEndPoints[subset][1][2] = EndPoints[subset][1][2];    // A.Blue
        }

        return;
    }

    float diff;
    float avr;

    // determin differance level based on lowest precision of the mode
    m_DiffLevel = (float)ModePartition[mode].lowestPrec;

    for (int subset = 0; subset < max_subsets; ++subset)
    {
        avr = (EndPoints[subset][0][0] +
            EndPoints[subset][0][1] +
            EndPoints[subset][0][2]) / 3.0f;

        // determine average diff
        diff = (abs(EndPoints[subset][0][0] - avr) +
            abs(EndPoints[subset][0][1] - avr) +
            abs(EndPoints[subset][0][2] - avr)) / 3;

        if ((diff < m_DiffLevel) && (avr > m_DiffLevel))
        {
            fEndPoints[subset][0][0] = avr;    // A.Red
            fEndPoints[subset][0][1] = avr;    // A.Green
            fEndPoints[subset][0][2] = avr;    // A.Blue
        }
        else
        {
            fEndPoints[subset][0][0] = EndPoints[subset][0][0];    // A.Red
            fEndPoints[subset][0][1] = EndPoints[subset][0][1];    // A.Green
            fEndPoints[subset][0][2] = EndPoints[subset][0][2];    // A.Blue
        }

        avr = (EndPoints[subset][1][0] +
            EndPoints[subset][1][1] +
            EndPoints[subset][1][2]) / 3.0f;

        diff = (abs(EndPoints[subset][1][0] - avr) +
            abs(EndPoints[subset][1][1] - avr) +
            abs(EndPoints[subset][1][2] - avr)) / 3;

        if ((diff < m_DiffLevel) && (avr > m_DiffLevel))
        {
            fEndPoints[subset][1][0] = avr;   // B.Red
            fEndPoints[subset][1][1] = avr;   // B.Green
            fEndPoints[subset][1][2] = avr;   // B.Blue
        }
        else
        {
            fEndPoints[subset][1][0] = EndPoints[subset][1][0];    // A.Red
            fEndPoints[subset][1][1] = EndPoints[subset][1][1];    // A.Green
            fEndPoints[subset][1][2] = EndPoints[subset][1][2];    // A.Blue
        }
    }
}

//================================================
// Mode Pathern order to try on endpoints
// The order can be rearranged to set which modes gets processed first
// for now it is set in order.
//================================================
static int ModeFitOrder[MAX_BC6H_MODES +1] =
                       {
                       0,                //0: N/A
                                         // ----  2 region lower bits ---
                       1,                // 10 5 5 5
                       2,                // 7  6 6 6
                       3,                // 11 5 4 5
                       4,                // 11 4 5 4
                       5,                // 11 4 4 5
                       6,                // 9  5 5 5
                       7,                // 8  6 5 5
                       8,                // 8  5 6 5
                       9,                // 8  5 5 6
                       10,               // 6  6 6 6
                                         //------ 1 region high bits ---
                       11,               // 10 10 10 10
                       12,               // 11 9  9  9
                       13,               // 12 8  8  8
                       14                // 16 4  4  4
};

float    BC6HBlockEncoder::EncodePattern(AMD_BC6H_Format &BC6H_data, float  error)
{
    int        max_subsets            = BC6H_data.region;

    // now we have input colors (in), output colors (outB) mapped to a line of ends (EndPoints)
    // and a set of colors on the line equally spaced (indexedcolors)
    // Lets assign indices

    //float SrcEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG];                  // temp endpoints used during calculations

    // Quantize the EndPoints
    int F16EndPoints[MAX_BC6H_MODES + 1][MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG];                    // temp endpoints used during calculations
    int quantEndPoints[MAX_BC6H_MODES + 1][MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG];                    // endpoints to save for a given mode

    // ModePartition[] starts from 1 to 14
    // If we have a shape pattern set the loop to check modes from 1 to 10 else from 11 to 14
    // of the ModePartition table
    int     min_mode = (BC6H_data.region == 2)?1:11;
    int     max_mode = (BC6H_data.region == 2)?MAX_TWOREGION_MODES: MAX_BC6H_MODES;

    bool    fits[15];
    memset(fits,0,sizeof(fits));

    int bestFit = 0;
    int bestEndpointMode = 0;
    float bestError = FLT_MAX;
    float bestEndpointsErr = FLT_MAX;
    float endPointErr = 0;

    // Try Optimization for the Mode
    float       best_EndPoints[MAX_BC6H_MODES + 1][MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG];
    int         best_Indices[MAX_BC6H_MODES + 1][MAX_SUBSETS][MAX_SUBSET_SIZE];
    float       opt_toterr[MAX_BC6H_MODES + 1];

    // for debugging
    memset(opt_toterr, 0, sizeof(opt_toterr));

    int numfits = 0;
    //
    // Notes;  Only the endpoints are varying; the indices stay fixed in values!
    // so to optimize which mode we need only check the endpoints error against our original to pick the mode to save
    //
    for (int modes = min_mode; modes <= max_mode; ++modes)
    {
            memcpy(best_EndPoints[modes], BC6H_data.fEndPoints,     sizeof(BC6H_data.fEndPoints));
            memcpy(best_Indices[modes],   BC6H_data.shape_indices, sizeof(BC6H_data.shape_indices));

            // For some modes the differances between channels can be quite small
            // typically for 6 bits 0..32 an increment of 1 in a channel can cause
            // unwanted color artifacts.
            // Check if computed channel endpoint have a wide spread between channels if not
            // scale all the channels to a avarage so that the variance is not noticed at lower bit values
            //if (m_bAverageEndPoint)
            //{
            //    AverageEndPoint(best_EndPoints[modes], SrcEndPoints, max_subsets, ModeFitOrder[modes]);
            //    QuantizeEndPointToF16Prec(SrcEndPoints, F16EndPoints[modes], max_subsets, ModePartition[ModeFitOrder[modes]].nbits);
            //}
            //else
            {
                QuantizeEndPointToF16Prec(best_EndPoints[modes], F16EndPoints[modes], max_subsets, ModePartition[ModeFitOrder[modes]].nbits);
            }

            // Indices data to save for given mode
            SwapIndices(F16EndPoints[modes], best_Indices[modes], BC6H_data.entryCount, max_subsets, ModeFitOrder[modes], BC6H_data.d_shape_index);
            bool transformfit = TransformEndPoints(BC6H_data, F16EndPoints[modes], quantEndPoints[modes], max_subsets,ModeFitOrder[modes]);
            fits[modes] = endpts_fit(F16EndPoints[modes], quantEndPoints[modes], ModeFitOrder[modes],max_subsets, m_isSigned);
            if (fits[modes] && transformfit)
            {
                numfits++;

                // The new compressed end points fit the mode
                // recalculate the error for this mode with a new set of indices
                // since we have shifted the end points from what we origially calc
                // from the find_bestpattern
                float uncompressed[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG];
                if (BC6H_data.region == 1)
                    decompress_endpoints1(BC6H_data, quantEndPoints[modes], uncompressed, ModeFitOrder[modes]);
                else
                    decompress_endpoints2(BC6H_data, quantEndPoints[modes], uncompressed, ModeFitOrder[modes]);
                // Takes the end points and creates a pallet of colors
                // based on preset weights along a vector formed by the two end points
                palitizeEndPointsF(BC6H_data, uncompressed);

                // Once we have the pallet - recalculate the optimal indices using the pallet
                // and the original image data stored in BC6H_data.din[]
                if (!m_isSigned)
                    ReIndexShapef(BC6H_data, best_Indices[modes]);

                // Calculate the error of the new tile vs the old tile data
                opt_toterr[modes] = CalcShapeError(BC6H_data, uncompressed, true);

                if (BC6H_data.region == 1)
                {
                    endPointErr = CalcOneRegionEndPtsError(BC6H_data, uncompressed, best_Indices[modes]);
                    if (endPointErr < bestEndpointsErr)
                    {
                        bestEndpointsErr = endPointErr;
                        bestEndpointMode = modes;
                    }
                }

                bool transformFit = true;
                // Save hold this mode fit data if its better than the last one checked.
                if (opt_toterr[modes] < bestError)
                {
                    if (!m_isSigned)
                    {
                        QuantizeEndPointToF16Prec(uncompressed, F16EndPoints[modes], max_subsets, ModePartition[ModeFitOrder[modes]].nbits);
                        SwapIndices(F16EndPoints[modes], best_Indices[modes], BC6H_data.entryCount, max_subsets, ModeFitOrder[modes], BC6H_data.d_shape_index);
                        transformFit = TransformEndPoints(BC6H_data, F16EndPoints[modes], quantEndPoints[modes], max_subsets, ModeFitOrder[modes]);
                    }
                    if (transformFit)
                    {
                        if (BC6H_data.region == 1)
                        {
                            bestFit = (modes == bestEndpointMode) ? modes: ((modes<bestEndpointMode)? modes:bestEndpointMode);
                        }
                        else
                        {
                            bestFit = modes;
                        }
                        bestError = opt_toterr[bestFit];
                        error = bestError;
                    }
                }

            }
    }

    if (numfits > 0)
    {
        SaveCompressedBlockData(BC6H_data, quantEndPoints[bestFit], best_Indices[bestFit], max_subsets, ModeFitOrder[bestFit]);
        return error;
    }

    // Should not get here!
    return error;
}

//==================================================================================
// CompressBlock
// in[]  is half float32 data  [0..1] for unsigned and [-1..+1] for signed
// it will be converted to 16 bit half CMP_HALFSHORT (short with signed component) for processing
//
// out is 128 bits BC6H Encoded data
//==================================================================================

//#define DEBUG_A_BLOCK
#ifdef DEBUG_A_BLOCK
float Testdin[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG] =
{
    {29440.0000, 29440.0000, 30255.0000, 0.000000000},
    {29440.0000, 29440.0000, 30123.0000, 0.000000000},
    {29440.0000, 29440.0000, 29440.0000, 0.000000000},
    {29440.0000, 29440.0000, 29440.0000, 0.000000000},
    {29440.0000, 29440.0000, 30251.0000, 0.000000000},
    {29440.0000, 29440.0000, 30105.0000, 0.000000000},
    {29440.0000, 29440.0000, 29440.0000, 0.000000000},
    {29440.0000, 29440.0000, 29440.0000, 0.000000000},
    {29440.0000, 29440.0000, 30246.0000, 0.000000000},
    {29440.0000, 29440.0000, 30086.0000, 0.000000000},
    {29440.0000, 29440.0000, 29440.0000, 0.000000000},
    {29440.0000, 29440.0000, 29440.0000, 0.000000000},
    {29440.0000, 29440.0000, 30240.0000, 0.000000000},
    {29440.0000, 29440.0000, 30047.0000, 0.000000000},
    {29440.0000, 29440.0000, 29440.0000, 0.000000000},
    {29440.0000, 29440.0000, 29440.0000, 0.000000000},
};
#endif

float BC6HBlockEncoder::CompressBlock(float in[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG], BYTE out[COMPRESSED_BLOCK_SIZE])
{
    /* Reserved feature:
    float smono[16];
    float R,G,B;
    float smin = FLT_MAX;
    */

#ifdef DEBUG_PATTERNS
    srand(100);
    // Save delta image to file
    fi = fopen("deltaImages.txt", "w");
#endif

    float    bestError = FLT_MAX;
    float    error = FLT_MAX;
    int      bestShape = 0;

    AMD_BC6H_Format            BC6H_data;

    memset(&BC6H_data, 0, sizeof(AMD_BC6H_Format));

    float normalization = 1.0;  // For future use

    for (int i = 0; i < BC6H_MAX_SUBSET_SIZE; i++)
    {

        // Our Half floats will be restricted to 0x7BFF with a sign components
        // so use 0..0x7BFF and sign bit for the floats

        // using if ( < 0.00001) to avoid case of values been -0.0 which is not processed when using if ( < 0)
        if (in[i][0] < 0.00001 || isnan(in[i][0]))
        {
            if (m_isSigned)
            {
                BC6H_data.din[i][0] = (isnan(in[i][0]))? F16NEGPREC_LIMIT_VAL : -CMP_HALF(abs(in[i][0] / normalization)).bits();
                if (BC6H_data.din[i][0] < F16NEGPREC_LIMIT_VAL) {
                    BC6H_data.din[i][0] = F16NEGPREC_LIMIT_VAL;
                }
            }
            else
                BC6H_data.din[i][0] = 0.0;
        }
        else
            BC6H_data.din[i][0] = CMP_HALF(in[i][0] / normalization).bits();

        if (in[i][1] < 0.00001 || isnan(in[i][1]))
        {
            if (m_isSigned)
            {
                BC6H_data.din[i][1] = (isnan(in[i][1])) ? F16NEGPREC_LIMIT_VAL : -CMP_HALF(abs(in[i][1] / normalization)).bits();
                if (BC6H_data.din[i][1] < F16NEGPREC_LIMIT_VAL) {
                    BC6H_data.din[i][1] = F16NEGPREC_LIMIT_VAL;
                }
            }
            else
                BC6H_data.din[i][1] = 0.0;
        }
        else
            BC6H_data.din[i][1] = CMP_HALF(in[i][1] / normalization).bits();

        if (in[i][2] < 0.00001 || isnan(in[i][2]))
        {
            if (m_isSigned)
            {
                BC6H_data.din[i][2] = (isnan(in[i][2])) ? F16NEGPREC_LIMIT_VAL : -CMP_HALF(abs(in[i][2] / normalization)).bits();
                if (BC6H_data.din[i][2] < F16NEGPREC_LIMIT_VAL) {
                    BC6H_data.din[i][2] = F16NEGPREC_LIMIT_VAL;
                }
            }
            else
                BC6H_data.din[i][2] = 0.0;
        }
        else
            BC6H_data.din[i][2] = CMP_HALF(in[i][2] / normalization).bits();

        BC6H_data.din[i][3] = 0.0;

    }

     BC6H_data.issigned = m_isSigned;
#ifdef DEBUG_A_BLOCK
    // Used for debugging blocks!
    for (int i = 0; i < BC6H_MAX_SUBSET_SIZE; i++)
    {
        BC6H_data.din[i][0] = Testdin[i][0];
        BC6H_data.din[i][1] = Testdin[i][1];
        BC6H_data.din[i][2] = Testdin[i][2];
        BC6H_data.din[i][3] = Testdin[i][3];
    }
#endif

    if (m_useMonoShapePatterns)
    {
        /*
        Reserved Feature MONOSHAPE_PATTERNS
        */
    }

    // run through no partition first
    error = FindBestPattern(BC6H_data, false, 0);
    if (error < bestError)
    {
        bestError = error;
        bestShape = -1;
        memcpy(BC6H_data.cur_best_shape_indices, BC6H_data.shape_indices, sizeof(BC6H_data.shape_indices));
        memcpy(BC6H_data.cur_best_partition, BC6H_data.partition, sizeof(BC6H_data.partition));
        memcpy(BC6H_data.cur_best_fEndPoints, BC6H_data.fEndPoints, sizeof(BC6H_data.fEndPoints));
        memcpy(BC6H_data.cur_best_entryCount, BC6H_data.entryCount, sizeof(BC6H_data.entryCount));
        BC6H_data.d_shape_index = bestShape;
    }

    // now run through all two regions shapes to find the best pattern
    for (int shape = 0; shape < MAX_BC6H_PARTITIONS; shape++)
    {
        error = FindBestPattern(BC6H_data, true, shape);
        if (error < bestError)
        {
            bestError = error;
            bestShape = shape;

            memcpy(BC6H_data.cur_best_shape_indices, BC6H_data.shape_indices, sizeof(BC6H_data.shape_indices));
            memcpy(BC6H_data.cur_best_partition, BC6H_data.partition, sizeof(BC6H_data.partition));
            memcpy(BC6H_data.cur_best_fEndPoints, BC6H_data.fEndPoints, sizeof(BC6H_data.fEndPoints));
            memcpy(BC6H_data.cur_best_entryCount, BC6H_data.entryCount, sizeof(BC6H_data.entryCount));
            BC6H_data.d_shape_index = bestShape;
        }
        else
        {
            if (bestShape != -1)
            {
                BC6H_data.d_shape_index = bestShape;
                memcpy(BC6H_data.shape_indices, BC6H_data.cur_best_shape_indices, sizeof(BC6H_data.shape_indices));
                memcpy(BC6H_data.partition, BC6H_data.cur_best_partition, sizeof(BC6H_data.partition));
                memcpy(BC6H_data.fEndPoints, BC6H_data.cur_best_fEndPoints, sizeof(BC6H_data.fEndPoints));
                memcpy(BC6H_data.entryCount, BC6H_data.cur_best_entryCount, sizeof(BC6H_data.entryCount));
            }
        }
    }

    // Optimize the result for encoding
    bestError = EncodePattern(BC6H_data, bestError);

    // used for debugging modes, set the value you want to debug with
    if (BC6H_data.m_mode != 0)
    {
        // do final encoding and save to output block
        SaveDataBlock(BC6H_data, out);
    }
    else
        memcpy(out, Cmp_Red_Block, 16);

    // do final encoding and save to output block
    // SaveDataBlock(best_BC6H_data,out);

#ifdef DEBUG_PATTERNS
    if (fi)
        fclose(fi);
#endif

    g_block++;

    return (float)bestError;
}
