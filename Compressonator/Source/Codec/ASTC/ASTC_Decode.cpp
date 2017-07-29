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
//

#include "Common.h"
#include "ASTC/ASTC_Decode.h"
#include "ASTC_Host.h"

// //using namespace ASTC_Encoder;
// void decompress_symbolic_block2(
//     astc_decode_mode decode_mode,
//     int xdim, int ydim, int zdim,   // dimensions of block
//     int xpos, int ypos, int zpos,   // position of block
//     const symbolic_compressed_block * scb, imageblock_cpu * blk)
// {
//     blk->xpos = xpos;
//     blk->ypos = ypos;
//     blk->zpos = zpos;
// 
//     int i;
// 
//     // if we detected an error-block, blow up immediately.
//     if (scb->error_block)
//     {
//         if (decode_mode == DECODE_LDR_SRGB)
//         {
//             for (i = 0; i < xdim * ydim * zdim; i++)
//             {
//                 blk->orig_data[4 * i] = 1.0f;
//                 blk->orig_data[4 * i + 1] = 0.0f;
//                 blk->orig_data[4 * i + 2] = 1.0f;
//                 blk->orig_data[4 * i + 3] = 1.0f;
//                 blk->rgb_lns[i] = 0;
//                 blk->alpha_lns[i] = 0;
//                 blk->nan_texel[i] = 0;
//             }
//         }
//         else
//         {
//             for (i = 0; i < xdim * ydim * zdim; i++)
//             {
//                 blk->orig_data[4 * i] = 0.0f;
//                 blk->orig_data[4 * i + 1] = 0.0f;
//                 blk->orig_data[4 * i + 2] = 0.0f;
//                 blk->orig_data[4 * i + 3] = 0.0f;
//                 blk->rgb_lns[i] = 0;
//                 blk->alpha_lns[i] = 0;
//                 blk->nan_texel[i] = 1;
//             }
//         }
// 
//         imageblock_initialize_work_from_orig_cpu(blk, xdim * ydim * zdim);
//         update_imageblock_flags_cpu(blk, xdim, ydim, zdim);
//         return;
//     }
// 
// 
// 
// 
//     if (scb->block_mode < 0)
//     {
//         float red = 0, green = 0, blue = 0, alpha = 0;
//         int use_lns = 0;
//         int use_nan = 0;
// 
//         if (scb->block_mode == -2)
//         {
//             // For sRGB decoding, we should return only the top 8 bits.
//             int mask = (decode_mode == DECODE_LDR_SRGB) ? 0xFF00 : 0xFFFF;
// 
//             red = sf16_to_float(unorm16_to_sf16((uint16_t)scb->constant_color[0] & mask));
//             green = sf16_to_float(unorm16_to_sf16((uint16_t)scb->constant_color[1] & mask));
//             blue = sf16_to_float(unorm16_to_sf16((uint16_t)scb->constant_color[2] & mask));
//             alpha = sf16_to_float(unorm16_to_sf16((uint16_t)scb->constant_color[3] & mask));
//             use_lns = 0;
//             use_nan = 0;
//         }
//         else
//         {
//             switch (decode_mode)
//             {
//             case DECODE_LDR_SRGB:
//                 red = 1.0f;
//                 green = 0.0f;
//                 blue = 1.0f;
//                 alpha = 1.0f;
//                 use_lns = 0;
//                 use_nan = 0;
//                 break;
//             case DECODE_LDR:
//                 red = 0.0f;
//                 green = 0.0f;
//                 blue = 0.0f;
//                 alpha = 0.0f;
//                 use_lns = 0;
//                 use_nan = 1;
//                 break;
//             case DECODE_HDR:
//                 // constant-color block; unpack from FP16 to FP32.
//                 red = sf16_to_float((sf16)scb->constant_color[0]);
//                 green = sf16_to_float((sf16)scb->constant_color[1]);
//                 blue = sf16_to_float((sf16)scb->constant_color[2]);
//                 alpha = sf16_to_float((sf16)scb->constant_color[3]);
//                 use_lns = 1;
//                 use_nan = 0;
//                 break;
//             }
//         }
// 
//         for (i = 0; i < xdim * ydim * zdim; i++)
//         {
//             blk->orig_data[4 * i] = red;
//             blk->orig_data[4 * i + 1] = green;
//             blk->orig_data[4 * i + 2] = blue;
//             blk->orig_data[4 * i + 3] = alpha;
//             blk->rgb_lns[i] = (uint8_t)use_lns;
//             blk->alpha_lns[i] = (uint8_t)use_lns;
//             blk->nan_texel[i] = (uint8_t)use_nan;
//         }
// 
// 
//         imageblock_initialize_work_from_orig_cpu(blk, xdim * ydim * zdim);
//         update_imageblock_flags_cpu(blk, xdim, ydim, zdim);
//         return;
//     }
// 
// 
//     // get the appropriate partition-table entry
//     int partition_count = scb->partition_count;
//     const partition_info_cpu *pt = get_partition_table(xdim, ydim, zdim, partition_count);
//     pt += scb->partition_index;
// 
//     // get the appropriate block descriptor
//     const block_size_descriptor *bsd = get_block_size_descriptor(xdim, ydim, zdim);
//     const decimation_table *const *ixtab2 = bsd->decimation_tables;
// 
// 
//     const decimation_table *it = ixtab2[bsd->block_modes[scb->block_mode].decimation_mode];
// 
//     int is_dual_plane = bsd->block_modes[scb->block_mode].is_dual_plane;
// 
//     int weight_quantization_level = bsd->block_modes[scb->block_mode].quantization_mode;
// 
// 
//     // decode the color endpoints
//     ushort4 color_endpoint0[4];
//     ushort4 color_endpoint1[4];
// 
//     int rgb_hdr_endpoint[4];
//     int alpha_hdr_endpoint[4];
//     int nan_endpoint[4];
// 
//     for (i = 0; i < partition_count; i++)
//         unpack_color_endpoints(decode_mode,
//             scb->color_formats[i],
//             scb->color_quantization_level, scb->color_values[i], &(rgb_hdr_endpoint[i]), &(alpha_hdr_endpoint[i]), &(nan_endpoint[i]), &(color_endpoint0[i]), &(color_endpoint1[i]));
// 
//     // first unquantize the weights
//     int uq_plane1_weights[MAX_WEIGHTS_PER_BLOCK];
//     int uq_plane2_weights[MAX_WEIGHTS_PER_BLOCK];
//     int weight_count = it->num_weights;
// 
// 
//     const quantization_and_transfer_table *qat = &(quant_and_xfer_tables[weight_quantization_level]);
// 
//     for (i = 0; i < weight_count; i++)
//     {
//         uq_plane1_weights[i] = qat->unquantized_value[scb->plane1_weights[i]];
//     }
//     if (is_dual_plane)
//     {
//         for (i = 0; i < weight_count; i++)
//             uq_plane2_weights[i] = qat->unquantized_value[scb->plane2_weights[i]];
//     }
// 
// 
//     // then un-decimate them.
//     int weights[MAX_TEXELS_PER_BLOCK];
//     int plane2_weights[MAX_TEXELS_PER_BLOCK];
// 
// 
//     int texels_per_block = xdim * ydim * zdim;
//     for (i = 0; i < texels_per_block; i++)
//         weights[i] = compute_value_of_texel_int(i, it, uq_plane1_weights);
// 
//     if (is_dual_plane)
//         for (i = 0; i < texels_per_block; i++)
//             plane2_weights[i] = compute_value_of_texel_int(i, it, uq_plane2_weights);
// 
// 
//     int plane2_color_component = scb->plane2_color_component;
// 
// 
//     // now that we have endpoint colors and weights, we can unpack actual colors for
//     // each texel.
//     for (i = 0; i < texels_per_block; i++)
//     {
//         int partition = pt->partition_of_texel[i];
// 
//         ushort4 color = lerp_color_int(decode_mode,
//             color_endpoint0[partition],
//             color_endpoint1[partition],
//             weights[i],
//             plane2_weights[i],
//             is_dual_plane ? plane2_color_component : -1);
// 
//         blk->rgb_lns[i] = (uint8_t)rgb_hdr_endpoint[partition];
//         blk->alpha_lns[i] = (uint8_t)alpha_hdr_endpoint[partition];
//         blk->nan_texel[i] = (uint8_t)nan_endpoint[partition];
// 
//         blk->work_data[4 * i] = color.x;
//         blk->work_data[4 * i + 1] = color.y;
//         blk->work_data[4 * i + 2] = color.z;
//         blk->work_data[4 * i + 3] = color.w;
//     }
// 
//     imageblock_initialize_orig_from_work_cpu(blk, xdim * ydim * zdim);
// 
//     update_imageblock_flags_cpu(blk, xdim, ydim, zdim);
// }


void ASTCBlockDecoder::DecompressBlock( BYTE BlockWidth, 
                                        BYTE BlockHeight,
                                        BYTE bitness,
                                        float   out[][4],
                                        BYTE    in[ASTC_COMPRESSED_BLOCK_SIZE])
{
    // Results Buffer
    astc_codec_image_cpu *img = allocate_image_cpu(bitness, BlockWidth, BlockHeight, 1, 0);
    initialize_image_cpu(img);

    ASTC_Encoder::uint8_t *bp = in;
    physical_compressed_block_cpu pcb = *(physical_compressed_block_cpu *) bp;
    symbolic_compressed_block_cpu scb;
    
    physical_to_symbolic_cpu(BlockWidth, BlockHeight, 1, pcb, &scb);


    swizzlepattern_cpu swz_decode = { 0, 1, 2, 3 };
    imageblock_cpu pb;
    
    // astc_decode_mode decode_mode1 = DECODE_HDR;
    // decompress_symbolic_block((astc_decode_mode)decode_mode1, BlockWidth, BlockHeight, 1, 0, 0, 0, (symbolic_compressed_block*)&scb, (imageblock_cpu *)&pb);

    ASTC_Encoder::astc_decode_mode decode_mode = ASTC_Encoder::DECODE_HDR;
    decompress_symbolic_block_cpu(decode_mode, BlockWidth, BlockHeight, 1, 0, 0, 0, &scb, &pb);



    write_imageblock_cpu(img, &pb, BlockWidth, BlockHeight, 1, 0, 0, 0, swz_decode);

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
                    out[index][0] = img->imagedata8[z][y][4 * x];
                    out[index][1] = img->imagedata8[z][y][4 * x + 1];
                    out[index][2] = img->imagedata8[z][y][4 * x + 2];
                    out[index][3] = img->imagedata8[z][y][4 * x + 3];
                    index++;
                }
            }
    }
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

    destroy_image_cpu(img);
}
