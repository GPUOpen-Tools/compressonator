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
//
// ASTC_Decode.cpp : A reference decoder for ASTC
// Current decoder only supports 4x4 blocks - future releases will support upto 12x12
//

#include "ASTC\ASTC_Decode.h"

void ASTCBlockDecoder::DecompressBlock(float   out[][4],
                                       BYTE    in[ASTC_COMPRESSED_BLOCK_SIZE])
{
    swizzlepattern swz_decode = { 0, 1, 2, 3 };
    astc_codec_image *img = allocate_image(8, 4, 4, 1, 0);
    initialize_image(img);

    uint8_t *bp = in;
    physical_compressed_block pcb = *(physical_compressed_block *) bp;
    symbolic_compressed_block scb;
    
    physical_to_symbolic(4, 4, 1, pcb, &scb);

    imageblock pb;
    astc_decode_mode decode_mode = DECODE_HDR;
    decompress_symbolic_block(decode_mode, 4, 4, 1, 0,0,0, &scb, &pb);
    write_imageblock(img, &pb, 4, 4, 1, 0, 0, 0, swz_decode);


    // copy results to our output buffer
    int x, y, z;
    int index=0;

    int exsize = img->xsize + 2 * img->padding;
    int eysize = img->ysize + 2 * img->padding;
    int ezsize = (img->zsize == 1) ? 1 : img->zsize + 2 * img->padding;

    if (img->imagedata8)
    {
        for (z = 0; z < ezsize; z++)
            for (y = 0; y < eysize; y++)
            {
                for (x = 0; x < exsize; x++)
                {
                    if (index < 16) 
                    {
                        out[index][0] = img->imagedata8[z][y][4 * x];
                        out[index][1] = img->imagedata8[z][y][4 * x + 1];
                        out[index][2] = img->imagedata8[z][y][4 * x + 2];
                        out[index][3] = img->imagedata8[z][y][4 * x + 3];
                    }
                index++;
                }
            }
    }
    // No supported in this version
    else if (img->imagedata16)
    {
        for (z = 0; z < ezsize; z++)
            for (y = 0; y < eysize; y++)
                for (x = 0; x < exsize; x++)
                {
                     out[index][0] = img->imagedata16[z][y][4 * x];        
                     out[index][1] = img->imagedata16[z][y][4 * x + 1];    
                     out[index][2] = img->imagedata16[z][y][4 * x + 2];    
                     out[index][3] = img->imagedata16[z][y][4 * x + 3];    
                }
    }

    destroy_image(img);
}
