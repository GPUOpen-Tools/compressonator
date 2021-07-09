/*----------------------------------------------------------------------------*/
/**
*    This confidential and proprietary software may be used only as
*    authorised by a licensing agreement from ARM Limited
*    (C) COPYRIGHT 2011-2012 ARM Limited
*    ALL RIGHTS RESERVED
*
*    The entire notice above must be reproduced on all authorised
*    copies and copies may only be made to the extent permitted
*    by a licensing agreement from ARM Limited.
*
*/
/*----------------------------------------------------------------------------*/
//=====================================================================
// Copyright (c) 2017  Advanced Micro Devices, Inc. All rights reserved
//=====================================================================

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "astc_host.h"
#include "astc_encode_kernel.h"
#include "compressonator.h"


//================================= ASTC CPU HOST CODE  ===========================================

namespace ASTC_Encoder {

//# this is added just incase the definition is missed when using only REPLACE_CPU_CODE
#ifdef REPLACE_CPU_CODE
#ifndef USE_HOST_CALLS
#define USE_HOST_CALLS
#endif
#endif


int compute_ise_bitcount2(int items, quantization_method quant) {
    switch (quant) {
    case QUANT_2:
        return items;
    case QUANT_3:
        return (8 * items + 4) / 5;
    case QUANT_4:
        return 2 * items;
    case QUANT_5:
        return (7 * items + 2) / 3;
    case QUANT_6:
        return (13 * items + 4) / 5;
    case QUANT_8:
        return 3 * items;
    case QUANT_10:
        return (10 * items + 2) / 3;
    case QUANT_12:
        return (18 * items + 4) / 5;
    case QUANT_16:
        return items * 4;
    case QUANT_20:
        return (13 * items + 2) / 3;
    case QUANT_24:
        return (23 * items + 4) / 5;
    case QUANT_32:
        return 5 * items;
    case QUANT_40:
        return (16 * items + 2) / 3;
    case QUANT_48:
        return (28 * items + 4) / 5;
    case QUANT_64:
        return 6 * items;
    case QUANT_80:
        return (19 * items + 2) / 3;
    case QUANT_96:
        return (33 * items + 4) / 5;
    case QUANT_128:
        return 7 * items;
    case QUANT_160:
        return (22 * items + 2) / 3;
    case QUANT_192:
        return (38 * items + 4) / 5;
    case QUANT_256:
        return 8 * items;
    default:
        return 100000;
    }
}

/*
float dot(float2 p, float2 q)
{
    return p.x * q.x + p.y * q.y;
}

float dot(float3 p, float3 q)
{
    return p.x * q.x + p.y * q.y + p.z * q.z;
}

float dot(float4 p, float4 q)
{
    return p.x * q.x + p.y * q.y + p.z * q.z + p.w * q.w;
}

float3 cross(float3 p, float3 q)
{
    return p.yzx * q.zxy - p.zxy * q.yzx;
}

float length(float2 p)
{
    return (float)(sqrt(dot(p, p)));
}

float length(float3 p)
{
    return (float)(sqrt(dot(p, p)));
}

float length(float4 p)
{
    return (float)(sqrt(dot(p, p)));
}

float2 normalize(float2 p)
{
    return p / length(p);
}

float3 normalize(float3 p)
{
    return p / length(p);
}

float4 normalize(float4 p)
{
    return p / length(p);
}
*/

#ifdef ASTC_ENABLE_3D_SUPPORT
// These functions use new () and should either be in CPU or changed to share a pre allocated pointer
void initialize_decimation_table_3d(
    // dimensions of the block
    int xdim, int ydim, int zdim,
    // number of grid points in 3d weight grid
    int x_weights, int y_weights, int z_weights, decimation_table * dt) {
    int i, j;
    int x, y, z;

    int texels_per_block = xdim * ydim * zdim;
    int weights_per_block = x_weights * y_weights * z_weights;

    int weightcount_of_texel[MAX_TEXELS_PER_BLOCK];
    int grid_weights_of_texel[MAX_TEXELS_PER_BLOCK][4];
    int weights_of_texel[MAX_TEXELS_PER_BLOCK][4];

    int texelcount_of_weight[MAX_WEIGHTS_PER_BLOCK];
    int texels_of_weight[MAX_WEIGHTS_PER_BLOCK][MAX_TEXELS_PER_BLOCK];
    int texelweights_of_weight[MAX_WEIGHTS_PER_BLOCK][MAX_TEXELS_PER_BLOCK];

    for (i = 0; i < weights_per_block; i++)
        texelcount_of_weight[i] = 0;
    for (i = 0; i < texels_per_block; i++)
        weightcount_of_texel[i] = 0;

    for (z = 0; z < zdim; z++)
        for (y = 0; y < ydim; y++)
            for (x = 0; x < xdim; x++) {
                int texel = (z * ydim + y) * xdim + x;

                int x_weight = (((1024 + xdim / 2) / (xdim - 1)) * x * (x_weights - 1) + 32) >> 6;
                int y_weight = (((1024 + ydim / 2) / (ydim - 1)) * y * (y_weights - 1) + 32) >> 6;
                int z_weight = (((1024 + zdim / 2) / (zdim - 1)) * z * (z_weights - 1) + 32) >> 6;

                int x_weight_frac = x_weight & 0xF;
                int y_weight_frac = y_weight & 0xF;
                int z_weight_frac = z_weight & 0xF;
                int x_weight_int = x_weight >> 4;
                int y_weight_int = y_weight >> 4;
                int z_weight_int = z_weight >> 4;
                int qweight[4];
                int weight[4];
                qweight[0] = (z_weight_int * y_weights + y_weight_int) * x_weights + x_weight_int;
                qweight[3] = ((z_weight_int + 1) * y_weights + (y_weight_int + 1)) * x_weights + (x_weight_int + 1);

                // simplex interpolation
                int fs = x_weight_frac;
                int ft = y_weight_frac;
                int fp = z_weight_frac;

                int cas = ((fs > ft) << 2) + ((ft > fp) << 1) + ((fs > fp));
                int N = x_weights;
                int NM = x_weights * y_weights;

                int s1, s2, w0, w1, w2, w3;
                switch (cas) {
                case 7:
                    s1 = 1;
                    s2 = N;
                    w0 = 16 - fs;
                    w1 = fs - ft;
                    w2 = ft - fp;
                    w3 = fp;
                    break;
                case 3:
                    s1 = N;
                    s2 = 1;
                    w0 = 16 - ft;
                    w1 = ft - fs;
                    w2 = fs - fp;
                    w3 = fp;
                    break;
                case 5:
                    s1 = 1;
                    s2 = NM;
                    w0 = 16 - fs;
                    w1 = fs - fp;
                    w2 = fp - ft;
                    w3 = ft;
                    break;
                case 4:
                    s1 = NM;
                    s2 = 1;
                    w0 = 16 - fp;
                    w1 = fp - fs;
                    w2 = fs - ft;
                    w3 = ft;
                    break;
                case 2:
                    s1 = N;
                    s2 = NM;
                    w0 = 16 - ft;
                    w1 = ft - fp;
                    w2 = fp - fs;
                    w3 = fs;
                    break;
                case 0:
                    s1 = NM;
                    s2 = N;
                    w0 = 16 - fp;
                    w1 = fp - ft;
                    w2 = ft - fs;
                    w3 = fs;
                    break;

                default:
                    s1 = NM;
                    s2 = N;
                    w0 = 16 - fp;
                    w1 = fp - ft;
                    w2 = ft - fs;
                    w3 = fs;
                    break;
                }

                qweight[1] = qweight[0] + s1;
                qweight[2] = qweight[1] + s2;
                weight[0] = w0;
                weight[1] = w1;
                weight[2] = w2;
                weight[3] = w3;

                /*
                for(i=0;i<4;i++) weight[i] <<= 4; */

                for (i = 0; i < 4; i++)
                    if (weight[i] != 0) {
                        grid_weights_of_texel[texel][weightcount_of_texel[texel]] = qweight[i];
                        weights_of_texel[texel][weightcount_of_texel[texel]] = weight[i];
                        weightcount_of_texel[texel]++;
                        texels_of_weight[qweight[i]][texelcount_of_weight[qweight[i]]] = texel;
                        texelweights_of_weight[qweight[i]][texelcount_of_weight[qweight[i]]] = weight[i];
                        texelcount_of_weight[qweight[i]]++;
                    }
            }

    for (i = 0; i < texels_per_block; i++) {
        dt->texel_num_weights[i] = (ASTC_Encoder::uint8_t)weightcount_of_texel[i];

        // ensure that all 4 entries are actually initialized.
        // This allows a branch-free implemntation of compute_value_of_texel_flt()
        for (j = 0; j < 4; j++) {
            dt->texel_weights_int[i][j] = 0;
            dt->texel_weights_float[i][j] = 0.0f;
            dt->texel_weights[i][j] = 0;
        }

        for (j = 0; j < weightcount_of_texel[i]; j++) {
            dt->texel_weights_int[i][j] = (ASTC_Encoder::uint8_t)weights_of_texel[i][j];
            dt->texel_weights_float[i][j] = weights_of_texel[i][j] * (1.0f / TEXEL_WEIGHT_SUM);
            dt->texel_weights[i][j] = (ASTC_Encoder::uint8_t)grid_weights_of_texel[i][j];
        }
    }

    for (i = 0; i < weights_per_block; i++) {
        dt->weight_num_texels[i] = (ASTC_Encoder::uint8_t)texelcount_of_weight[i];
        for (j = 0; j < texelcount_of_weight[i]; j++) {
            dt->weight_texel[i][j] = (ASTC_Encoder::uint8_t)texels_of_weight[i][j];
            dt->weights_int[i][j] = (ASTC_Encoder::uint8_t)texelweights_of_weight[i][j];
            dt->weights_flt[i][j] = (float)texelweights_of_weight[i][j];
        }
    }

    dt->num_texels = texels_per_block;
    dt->num_weights = weights_per_block;
}

int decode_block_mode_3d(int blockmode, int *Nval, int *Mval, int *Qval, int *dual_weight_plane, int *quant_mode) {
    int base_quant_mode = (blockmode >> 4) & 1;
    int H = (blockmode >> 9) & 1;
    int D = (blockmode >> 10) & 1;

    int A = (blockmode >> 5) & 0x3;

    int N = 0, M = 0, Q = 0;

    if ((blockmode & 3) != 0) {
        base_quant_mode |= (blockmode & 3) << 1;
        int B = (blockmode >> 7) & 3;
        int C = (blockmode >> 2) & 0x3;
        N = A + 2;
        M = B + 2;
        Q = C + 2;
    } else {
        base_quant_mode |= ((blockmode >> 2) & 3) << 1;
        if (((blockmode >> 2) & 3) == 0)
            return 0;
        int B = (blockmode >> 9) & 3;
        if (((blockmode >> 7) & 3) != 3) {
            D = 0;
            H = 0;
        }
        switch ((blockmode >> 7) & 3) {
        case 0:
            N = 6;
            M = B + 2;
            Q = A + 2;
            break;
        case 1:
            N = A + 2;
            M = 6;
            Q = B + 2;
            break;
        case 2:
            N = A + 2;
            M = B + 2;
            Q = 6;
            break;
        case 3:
            N = 2;
            M = 2;
            Q = 2;
            switch ((blockmode >> 5) & 3) {
            case 0:
                N = 6;
                break;
            case 1:
                M = 6;
                break;
            case 2:
                Q = 6;
                break;
            case 3:
                return 0;
            }
            break;
        }
    }

    int weight_count = N * M * Q * (D + 1);
    int qmode = (base_quant_mode - 2) + 6 * H;

    int weightbits = compute_ise_bitcount(weight_count, (quantization_method)qmode);
    if (weight_count > MAX_WEIGHTS_PER_BLOCK || weightbits < MIN_WEIGHT_BITS_PER_BLOCK || weightbits > MAX_WEIGHT_BITS_PER_BLOCK)
        return 0;

    *Nval = N;
    *Mval = M;
    *Qval = Q;
    *dual_weight_plane = D;
    *quant_mode = qmode;
    return 1;
}

// stubbed for the time being.
const float *get_3d_percentile_table_host(int blockdim_x, int blockdim_y, int blockdim_z) {
    IGNOREPARAM(blockdim_x);
    IGNOREPARAM(blockdim_y);
    IGNOREPARAM(blockdim_z);
    return dummy_percentile_table_3d;
}

void construct_block_size_descriptor_3d_host(int xdim, int ydim, int zdim, block_size_descriptor * bsd) {
    int decimation_mode_index[512];    // for each of the 512 entries in the decim_table_array, its index
    int decimation_mode_count = 0;

    int i;
    int x_weights;
    int y_weights;
    int z_weights;

    for (i = 0; i < 512; i++) {
        decimation_mode_index[i] = -1;
    }

    // gather all the infill-modes that can be used with the current block size
    for (x_weights = 2; x_weights <= 6; x_weights++)
        for (y_weights = 2; y_weights <= 6; y_weights++)
            for (z_weights = 2; z_weights <= 6; z_weights++) {
                if ((x_weights * y_weights * z_weights) > MAX_WEIGHTS_PER_BLOCK)
                    continue;
                decimation_table dt;
                decimation_mode_index[z_weights * 64 + y_weights * 8 + x_weights] = decimation_mode_count;
                initialize_decimation_table_3d(xdim, ydim, zdim, x_weights, y_weights, z_weights, &dt);

                int weight_count = x_weights * y_weights * z_weights;

                int maxprec_1plane = -1;
                int maxprec_2planes = -1;
                for (i = 0; i < 12; i++) {
                    int bits_1plane = compute_ise_bitcount(weight_count, (quantization_method)i);
                    int bits_2planes = compute_ise_bitcount(2 * weight_count, (quantization_method)i);
                    if (bits_1plane >= MIN_WEIGHT_BITS_PER_BLOCK && bits_1plane <= MAX_WEIGHT_BITS_PER_BLOCK)
                        maxprec_1plane = i;
                    if (bits_2planes >= MIN_WEIGHT_BITS_PER_BLOCK && bits_2planes <= MAX_WEIGHT_BITS_PER_BLOCK)
                        maxprec_2planes = i;
                }
                bsd->permit_encode[decimation_mode_count] = (x_weights <= xdim && y_weights <= ydim && z_weights <= zdim);

                bsd->decimation_mode_samples[decimation_mode_count] = weight_count;
                bsd->decimation_mode_maxprec_1plane[decimation_mode_count] = maxprec_1plane;
                bsd->decimation_mode_maxprec_2planes[decimation_mode_count] = maxprec_2planes;
                bsd->decimation_tables[decimation_mode_count] = dt;

                decimation_mode_count++;
            }

    for (i = 0; i < MAX_DECIMATION_MODES; i++) {
        bsd->decimation_mode_percentile[i] = 1.0f;
    }

    for (i = decimation_mode_count; i < MAX_DECIMATION_MODES; i++) {
        bsd->permit_encode[i] = 0;
        bsd->decimation_mode_samples[i] = 0;
        bsd->decimation_mode_maxprec_1plane[i] = -1;
        bsd->decimation_mode_maxprec_2planes[i] = -1;
    }

    bsd->decimation_mode_count = decimation_mode_count;

    const float *percentiles = get_3d_percentile_table_host(xdim, ydim, zdim);

    // then construct the list of block formats
    for (i = 0; i < 2048; i++) {
        int is_dual_plane;
        int quantization_mode;
        int fail = 0;
        int permit_encode = 1;

        if (decode_block_mode_3d(i, &x_weights, &y_weights, &z_weights, &is_dual_plane, &quantization_mode)) {
            if (x_weights > xdim || y_weights > ydim || z_weights > zdim)
                permit_encode = 0;
        } else {
            fail = 1;
            permit_encode = 0;
        }
        if (fail) {
            bsd->block_modes[i].decimation_mode = -1;
            bsd->block_modes[i].quantization_mode = -1;
            bsd->block_modes[i].is_dual_plane = -1;
            bsd->block_modes[i].permit_encode = 0;
            bsd->block_modes[i].permit_decode = 0;
            bsd->block_modes[i].percentile = 1.0f;
        } else {
            int decimation_mode = decimation_mode_index[z_weights * 64 + y_weights * 8 + x_weights];
            bsd->block_modes[i].decimation_mode = (ASTC_Encoder::uint8_t)decimation_mode;
            bsd->block_modes[i].quantization_mode = (ASTC_Encoder::uint8_t)quantization_mode;
            bsd->block_modes[i].is_dual_plane = (ASTC_Encoder::uint8_t)is_dual_plane;
            bsd->block_modes[i].permit_encode = (ASTC_Encoder::uint8_t)permit_encode;
            bsd->block_modes[i].permit_decode = (ASTC_Encoder::uint8_t)permit_encode;
            bsd->block_modes[i].percentile = percentiles[i];

            if (bsd->decimation_mode_percentile[decimation_mode] > percentiles[i])
                bsd->decimation_mode_percentile[decimation_mode] = percentiles[i];
        }

    }

    if (xdim * ydim * zdim <= 64) {
        bsd->texelcount_for_bitmap_partitioning = xdim * ydim * zdim;
        for (i = 0; i < xdim * ydim * zdim; i++)
            bsd->texels_for_bitmap_partitioning[i] = i;
    } else {
        // pick 64 random texels for use with bitmap partitioning.
        int arr[MAX_TEXELS_PER_BLOCK];
        for (i = 0; i < xdim * ydim * zdim; i++)
            arr[i] = 0;
        int arr_elements_set = 0;
        while (arr_elements_set < 64) {
            int idx = rand() % (xdim * ydim * zdim);
            if (arr[idx] == 0) {
                arr_elements_set++;
                arr[idx] = 1;
            }
        }
        int texel_weights_written = 0;
        int idx = 0;
        while (texel_weights_written < 64) {
            if (arr[idx])
                bsd->texels_for_bitmap_partitioning[texel_weights_written++] = idx;
            idx++;
        }
        bsd->texelcount_for_bitmap_partitioning = 64;
    }
}
#endif

// return 0 on invalid mode, 1 on valid mode.
int decode_block_mode_2d(int blockmode, int *Nval, int *Mval, int *dual_weight_plane, int *quant_mode) {
    int base_quant_mode = (blockmode >> 4) & 1;
    int H = (blockmode >> 9) & 1;
    int D = (blockmode >> 10) & 1;

    int A = (blockmode >> 5) & 0x3;

    int N = 0, M = 0;

    if ((blockmode & 3) != 0) {
        base_quant_mode |= (blockmode & 3) << 1;
        int B = (blockmode >> 7) & 3;
        switch ((blockmode >> 2) & 3) {
        case 0:
            N = B + 4;
            M = A + 2;
            break;
        case 1:
            N = B + 8;
            M = A + 2;
            break;
        case 2:
            N = A + 2;
            M = B + 8;
            break;
        case 3:
            B &= 1;
            if (blockmode & 0x100) {
                N = B + 2;
                M = A + 2;
            } else {
                N = A + 2;
                M = B + 6;
            }
            break;
        }
    } else {
        base_quant_mode |= ((blockmode >> 2) & 3) << 1;
        if (((blockmode >> 2) & 3) == 0)
            return 0;
        int B = (blockmode >> 9) & 3;
        switch ((blockmode >> 7) & 3) {
        case 0:
            N = 12;
            M = A + 2;
            break;
        case 1:
            N = A + 2;
            M = 12;
            break;
        case 2:
            N = A + 6;
            M = B + 6;
            D = 0;
            H = 0;
            break;
        case 3:
            switch ((blockmode >> 5) & 3) {
            case 0:
                N = 6;
                M = 10;
                break;
            case 1:
                N = 10;
                M = 6;
                break;
            case 2:
            case 3:
                return 0;
            }
            break;
        }
    }

    int weight_count = N * M * (D + 1);
    int qmode = (base_quant_mode - 2) + 6 * H;

    int weightbits = compute_ise_bitcount2(weight_count, (quantization_method)qmode);
    if (weight_count > MAX_WEIGHTS_PER_BLOCK || weightbits < MIN_WEIGHT_BITS_PER_BLOCK || weightbits > MAX_WEIGHT_BITS_PER_BLOCK)
        return 0;

    *Nval = N;
    *Mval = M;
    *dual_weight_plane = D;
    *quant_mode = qmode;
    return 1;
}

const float *get_2d_percentile_table_host(int blockdim_x, int blockdim_y) {
    switch (blockdim_x) {
    case 4:
        switch (blockdim_y) {
        case 4:
            return percentile_table_4x4;
        case 5:
            return percentile_table_4x5;
        case 6:
            return percentile_table_4x6;
        case 8:
            return percentile_table_4x8;
        case 10:
            return percentile_table_4x10;
        case 12:
            return percentile_table_4x12;
        }
        break;
    case 5:
        switch (blockdim_y) {
        case 4:
            return percentile_table_5x4;
        case 5:
            return percentile_table_5x5;
        case 6:
            return percentile_table_5x6;
        case 8:
            return percentile_table_5x8;
        case 10:
            return percentile_table_5x10;
        case 12:
            return percentile_table_5x12;
        }
        break;

    case 6:
        switch (blockdim_y) {
        case 4:
            return percentile_table_6x4;
        case 5:
            return percentile_table_6x5;
        case 6:
            return percentile_table_6x6;
        case 8:
            return percentile_table_6x8;
        case 10:
            return percentile_table_6x10;
        case 12:
            return percentile_table_6x12;
        }
        break;

    case 8:
        switch (blockdim_y) {
        case 4:
            return percentile_table_8x4;
        case 5:
            return percentile_table_8x5;
        case 6:
            return percentile_table_8x6;
        case 8:
            return percentile_table_8x8;
        case 10:
            return percentile_table_8x10;
        case 12:
            return percentile_table_8x12;
        }
        break;

    case 10:
        switch (blockdim_y) {
        case 4:
            return percentile_table_10x4;
        case 5:
            return percentile_table_10x5;
        case 6:
            return percentile_table_10x6;
        case 8:
            return percentile_table_10x8;
        case 10:
            return percentile_table_10x10;
        case 12:
            return percentile_table_10x12;
        }
        break;

    case 12:
        switch (blockdim_y) {
        case 4:
            return percentile_table_12x4;
        case 5:
            return percentile_table_12x5;
        case 6:
            return percentile_table_12x6;
        case 8:
            return percentile_table_12x8;
        case 10:
            return percentile_table_12x10;
        case 12:
            return percentile_table_12x12;
        }
        break;
    default:
        break;
    }

    return NULL;                // should never happen.
}

void initialize_decimation_table_2d(
    // dimensions of the block
    int xdim, int ydim,
    // number of grid points in 2d weight grid
    int x_weights, int y_weights, decimation_table * dt) {
    int i, j;
    int x, y;

    int texels_per_block = xdim * ydim;
    int weights_per_block = x_weights * y_weights;

    int weightcount_of_texel[MAX_TEXELS_PER_BLOCK];
    int grid_weights_of_texel[MAX_TEXELS_PER_BLOCK][4];
    int weights_of_texel[MAX_TEXELS_PER_BLOCK][4];

    int texelcount_of_weight[MAX_WEIGHTS_PER_BLOCK];
    int texels_of_weight[MAX_WEIGHTS_PER_BLOCK][MAX_TEXELS_PER_BLOCK];
    int texelweights_of_weight[MAX_WEIGHTS_PER_BLOCK][MAX_TEXELS_PER_BLOCK];

    for (i = 0; i < weights_per_block; i++)
        texelcount_of_weight[i] = 0;
    for (i = 0; i < texels_per_block; i++)
        weightcount_of_texel[i] = 0;

    for (y = 0; y < ydim; y++)
        for (x = 0; x < xdim; x++) {
            int texel = y * xdim + x;

            int x_weight = (((1024 + xdim / 2) / (xdim - 1)) * x * (x_weights - 1) + 32) >> 6;
            int y_weight = (((1024 + ydim / 2) / (ydim - 1)) * y * (y_weights - 1) + 32) >> 6;

            int x_weight_frac = x_weight & 0xF;
            int y_weight_frac = y_weight & 0xF;
            int x_weight_int = x_weight >> 4;
            int y_weight_int = y_weight >> 4;
            int qweight[4];
            int weight[4];
            qweight[0] = x_weight_int + y_weight_int * x_weights;
            qweight[1] = qweight[0] + 1;
            qweight[2] = qweight[0] + x_weights;
            qweight[3] = qweight[2] + 1;

            // truncated-precision bilinear interpolation.
            int prod = x_weight_frac * y_weight_frac;

            weight[3] = (prod + 8) >> 4;
            weight[1] = x_weight_frac - weight[3];
            weight[2] = y_weight_frac - weight[3];
            weight[0] = 16 - x_weight_frac - y_weight_frac + weight[3];

            for (i = 0; i < 4; i++)
                if (weight[i] != 0) {
                    grid_weights_of_texel[texel][weightcount_of_texel[texel]] = qweight[i];
                    weights_of_texel[texel][weightcount_of_texel[texel]] = weight[i];
                    weightcount_of_texel[texel]++;
                    texels_of_weight[qweight[i]][texelcount_of_weight[qweight[i]]] = texel;
                    texelweights_of_weight[qweight[i]][texelcount_of_weight[qweight[i]]] = weight[i];
                    texelcount_of_weight[qweight[i]]++;
                }
        }

    for (i = 0; i < texels_per_block; i++) {
        dt->texel_num_weights[i] = (ASTC_Encoder::uint8_t)weightcount_of_texel[i];

        // ensure that all 4 entries are actually initialized.
        // This allows a branch-free implemntation of compute_value_of_texel_flt()
        for (j = 0; j < 4; j++) {
            dt->texel_weights_int[i][j] = 0;
            dt->texel_weights_float[i][j] = 0.0f;
            dt->texel_weights[i][j] = 0;
        }

        for (j = 0; j < weightcount_of_texel[i]; j++) {
            dt->texel_weights_int[i][j] = (ASTC_Encoder::uint8_t)weights_of_texel[i][j];
            dt->texel_weights_float[i][j] = (weights_of_texel[i][j]) * (1.0f / TEXEL_WEIGHT_SUM);
            dt->texel_weights[i][j] = (ASTC_Encoder::uint8_t)grid_weights_of_texel[i][j];
        }
    }

    for (i = 0; i < weights_per_block; i++) {
        dt->weight_num_texels[i] = (ASTC_Encoder::uint8_t)texelcount_of_weight[i];


        for (j = 0; j < texelcount_of_weight[i]; j++) {
            dt->weight_texel[i][j] = (ASTC_Encoder::uint8_t)texels_of_weight[i][j];
            dt->weights_int[i][j] = (ASTC_Encoder::uint8_t)texelweights_of_weight[i][j];
            dt->weights_flt[i][j] = (float)texelweights_of_weight[i][j];
        }
    }

    dt->num_texels = texels_per_block;
    dt->num_weights = weights_per_block;


}

void construct_block_size_descriptor_2d_host(int xdim, int ydim, block_size_descriptor * bsd) {
    int decimation_mode_index[256];    // for each of the 256 entries in the decim_table_array, its index
    int decimation_mode_count = 0;

    int i;
    int x_weights;
    int y_weights;

    for (i = 0; i < 256; i++) {
        decimation_mode_index[i] = -1;
    }

    // gather all the infill-modes that can be used with the current block size
    for (x_weights = 2; x_weights <= 12; x_weights++)
        for (y_weights = 2; y_weights <= 12; y_weights++) {
            if (x_weights * y_weights > MAX_WEIGHTS_PER_BLOCK)
                continue;
            decimation_table dt;
            decimation_mode_index[y_weights * 16 + x_weights] = decimation_mode_count;
            initialize_decimation_table_2d(xdim, ydim, x_weights, y_weights, &dt);

            int weight_count = x_weights * y_weights;

            int maxprec_1plane = -1;
            int maxprec_2planes = -1;
            for (i = 0; i < 12; i++) {
                int bits_1plane = compute_ise_bitcount2(weight_count, (quantization_method)i);
                int bits_2planes = compute_ise_bitcount2(2 * weight_count, (quantization_method)i);
                if (bits_1plane >= MIN_WEIGHT_BITS_PER_BLOCK && bits_1plane <= MAX_WEIGHT_BITS_PER_BLOCK)
                    maxprec_1plane = i;
                if (bits_2planes >= MIN_WEIGHT_BITS_PER_BLOCK && bits_2planes <= MAX_WEIGHT_BITS_PER_BLOCK)
                    maxprec_2planes = i;
            }

            bsd->permit_encode[decimation_mode_count] = (x_weights <= xdim && y_weights <= ydim);

            bsd->decimation_mode_samples[decimation_mode_count] = weight_count;
            bsd->decimation_mode_maxprec_1plane[decimation_mode_count] = maxprec_1plane;
            bsd->decimation_mode_maxprec_2planes[decimation_mode_count] = maxprec_2planes;
            bsd->decimation_tables[decimation_mode_count] = dt;

            decimation_mode_count++;
        }

    for (i = 0; i < MAX_DECIMATION_MODES; i++) {
        bsd->decimation_mode_percentile[i] = 1.0f;
    }

    for (i = decimation_mode_count; i < MAX_DECIMATION_MODES; i++) {
        bsd->permit_encode[i] = 0;
        bsd->decimation_mode_samples[i] = 0;
        bsd->decimation_mode_maxprec_1plane[i] = -1;
        bsd->decimation_mode_maxprec_2planes[i] = -1;
    }

    bsd->decimation_mode_count = decimation_mode_count;

    const float *percentiles = get_2d_percentile_table_host(xdim, ydim);

    // then construct the list of block formats
    for (i = 0; i < 2048; i++) {
        int is_dual_plane;
        int quantization_mode;
        int fail = 0;
        int permit_encode = 1;

        if (decode_block_mode_2d(i, &x_weights, &y_weights, &is_dual_plane, &quantization_mode)) {
            if (x_weights > xdim || y_weights > ydim)
                permit_encode = 0;
        } else {
            fail = 1;
            permit_encode = 0;
        }

        if (fail) {
            bsd->block_modes[i].decimation_mode = -1;
            bsd->block_modes[i].quantization_mode = -1;
            bsd->block_modes[i].is_dual_plane = -1;
            bsd->block_modes[i].permit_encode = 0;
            bsd->block_modes[i].permit_decode = 0;
            bsd->block_modes[i].percentile = 1.0f;
        } else {
            int decimation_mode = decimation_mode_index[y_weights * 16 + x_weights];
            bsd->block_modes[i].decimation_mode = (ASTC_Encoder::uint8_t)decimation_mode;
            bsd->block_modes[i].quantization_mode = (ASTC_Encoder::uint8_t)quantization_mode;
            bsd->block_modes[i].is_dual_plane = (ASTC_Encoder::uint8_t)is_dual_plane;
            bsd->block_modes[i].permit_encode = (ASTC_Encoder::uint8_t)permit_encode;
            bsd->block_modes[i].permit_decode = (ASTC_Encoder::uint8_t)permit_encode;    // disallow decode of grid size larger than block size.
            bsd->block_modes[i].percentile = percentiles[i];

            if (bsd->decimation_mode_percentile[decimation_mode] > percentiles[i])
                bsd->decimation_mode_percentile[decimation_mode] = percentiles[i];
        }

    }

    if (xdim * ydim <= 64) {
        bsd->texelcount_for_bitmap_partitioning = xdim * ydim;
        for (i = 0; i < xdim * ydim; i++)
            bsd->texels_for_bitmap_partitioning[i] = i;
    }

    else {
        // pick 64 random texels for use with bitmap partitioning.
        int arr[MAX_TEXELS_PER_BLOCK];
        for (i = 0; i < xdim * ydim; i++)
            arr[i] = 0;
        int arr_elements_set = 0;
        while (arr_elements_set < 64) {
            int idx = rand() % (xdim * ydim);
            if (arr[idx] == 0) {
                arr_elements_set++;
                arr[idx] = 1;
            }
        }
        int texel_weights_written = 0;
        int idx = 0;
        while (texel_weights_written < 64) {
            if (arr[idx])
                bsd->texels_for_bitmap_partitioning[texel_weights_written++] = idx;
            idx++;
        }
        bsd->texelcount_for_bitmap_partitioning = 64;

    }
}

int compare_canonicalized_partition_tables(const uint64_cl part1[7], const uint64_cl part2[7]) {
    if (part1[0] != part2[0])
        return 0;
    if (part1[1] != part2[1])
        return 0;
    if (part1[2] != part2[2])
        return 0;
    if (part1[3] != part2[3])
        return 0;
    if (part1[4] != part2[4])
        return 0;
    if (part1[5] != part2[5])
        return 0;
    if (part1[6] != part2[6])
        return 0;
    return 1;
}

void gen_canonicalized_partition_table(int texel_count, const ASTC_Encoder::uint8_t * partition_table, uint64_cl canonicalized[7]) {
    int i;
    for (i = 0; i < 7; i++)
        canonicalized[i] = 0;

    int mapped_index[4];
    int map_weight_count = 0;
    for (i = 0; i < 4; i++)
        mapped_index[i] = -1;

    for (i = 0; i < texel_count; i++) {
        int index = partition_table[i];
        if (mapped_index[index] == -1)
            mapped_index[index] = map_weight_count++;
        uint64_cl xlat_index = mapped_index[index];
        canonicalized[i >> 5] |= xlat_index << (2 * (i & 0x1F));
    }
}

void partition_table_zap_equal_elements(int xdim, int ydim, int zdim, partition_info * pi) {
    int partition_tables_zapped = 0;

    int texel_count = xdim * ydim * zdim;

    int i, j;
    uint64_cl *canonicalizeds = new uint64_cl[PARTITION_COUNT * 7];


    for (i = 0; i < PARTITION_COUNT; i++) {
        gen_canonicalized_partition_table(texel_count, pi[i].partition_of_texel, canonicalizeds + i * 7);
    }

    for (i = 0; i < PARTITION_COUNT; i++) {
        for (j = 0; j < i; j++) {
            if (compare_canonicalized_partition_tables(canonicalizeds + 7 * i, canonicalizeds + 7 * j)) {
                pi[i].partition_count = 0;
                partition_tables_zapped++;
                break;
            }
        }
    }
    delete[]canonicalizeds;
}

ASTC_Encoder::uint32_t hash52_host(ASTC_Encoder::uint32_t inp) {
    inp ^= inp >> 15;

    inp *= 0xEEDE0891;            // (2^4+1)*(2^7+1)*(2^17-1)
    inp ^= inp >> 5;
    inp += inp << 16;
    inp ^= inp >> 7;
    inp ^= inp >> 3;
    inp ^= inp << 6;
    inp ^= inp >> 17;
    return inp;
}

int select_partition_host(int seed, int x, int y, int z, int partitioncount, int small_block) {
    if (small_block) {
        x <<= 1;
        y <<= 1;
        z <<= 1;
    }

    seed += (partitioncount - 1) * 1024;

    ASTC_Encoder::uint32_t rnum = hash52_host(seed);

    ASTC_Encoder::uint8_t seed1 = rnum & 0xF;
    ASTC_Encoder::uint8_t seed2 = (rnum >> 4) & 0xF;
    ASTC_Encoder::uint8_t seed3 = (rnum >> 8) & 0xF;
    ASTC_Encoder::uint8_t seed4 = (rnum >> 12) & 0xF;
    ASTC_Encoder::uint8_t seed5 = (rnum >> 16) & 0xF;
    ASTC_Encoder::uint8_t seed6 = (rnum >> 20) & 0xF;
    ASTC_Encoder::uint8_t seed7 = (rnum >> 24) & 0xF;
    ASTC_Encoder::uint8_t seed8 = (rnum >> 28) & 0xF;
    ASTC_Encoder::uint8_t seed9 = (rnum >> 18) & 0xF;
    ASTC_Encoder::uint8_t seed10 = (rnum >> 22) & 0xF;
    ASTC_Encoder::uint8_t seed11 = (rnum >> 26) & 0xF;
    ASTC_Encoder::uint8_t seed12 = ((rnum >> 30) | (rnum << 2)) & 0xF;

    // squaring all the seeds in order to bias their distribution
    // towards lower values.
    seed1 *= seed1;
    seed2 *= seed2;
    seed3 *= seed3;
    seed4 *= seed4;
    seed5 *= seed5;
    seed6 *= seed6;
    seed7 *= seed7;
    seed8 *= seed8;
    seed9 *= seed9;
    seed10 *= seed10;
    seed11 *= seed11;
    seed12 *= seed12;


    int sh1, sh2, sh3;
    if (seed & 1) {
        sh1 = (seed & 2 ? 4 : 5);
        sh2 = (partitioncount == 3 ? 6 : 5);
    } else {
        sh1 = (partitioncount == 3 ? 6 : 5);
        sh2 = (seed & 2 ? 4 : 5);
    }
    sh3 = (seed & 0x10) ? sh1 : sh2;

    seed1 >>= sh1;
    seed2 >>= sh2;
    seed3 >>= sh1;
    seed4 >>= sh2;
    seed5 >>= sh1;
    seed6 >>= sh2;
    seed7 >>= sh1;
    seed8 >>= sh2;

    seed9 >>= sh3;
    seed10 >>= sh3;
    seed11 >>= sh3;
    seed12 >>= sh3;



    int a = seed1 * x + seed2 * y + seed11 * z + (rnum >> 14);
    int b = seed3 * x + seed4 * y + seed12 * z + (rnum >> 10);
    int c = seed5 * x + seed6 * y + seed9 * z + (rnum >> 6);
    int d = seed7 * x + seed8 * y + seed10 * z + (rnum >> 2);


    // apply the saw
    a &= 0x3F;
    b &= 0x3F;
    c &= 0x3F;
    d &= 0x3F;

    // remove some of the components of we are to output < 4 partitions.
    if (partitioncount <= 3)
        d = 0;
    if (partitioncount <= 2)
        c = 0;
    if (partitioncount <= 1)
        b = 0;

    int partition;
    if (a >= b && a >= c && a >= d)
        partition = 0;
    else if (b >= c && b >= d)
        partition = 1;
    else if (c >= d)
        partition = 2;
    else
        partition = 3;
    return partition;
}

void generate_one_partition_table(int xdim, int ydim, int zdim, int partition_count, int partition_index, partition_info * pt, __global ASTC_Encode *ASTCEncode) {
    int small_block = (xdim * ydim * zdim) < 32;

    ASTC_Encoder::uint8_t *partition_of_texel = pt->partition_of_texel;
    int x, y, z, i;


    for (z = 0; z < zdim; z++)
        for (y = 0; y < ydim; y++)
            for (x = 0; x < xdim; x++) {
                ASTC_Encoder::uint8_t part = (ASTC_Encoder::uint8_t)select_partition_host(partition_index, x, y, z, partition_count, small_block);
                *partition_of_texel++ = part;
            }


    int texels_per_block = xdim * ydim * zdim;

    int counts[4];
    for (i = 0; i < 4; i++)
        counts[i] = 0;

    for (i = 0; i < texels_per_block; i++) {
        int partition = pt->partition_of_texel[i];
        pt->texels_of_partition[partition][counts[partition]++] = (ASTC_Encoder::uint8_t)i;
    }

    for (i = 0; i < 4; i++)
        pt->texels_per_partition[i] = (ASTC_Encoder::uint8_t)counts[i];

    if (counts[0] == 0)
        pt->partition_count = 0;
    else if (counts[1] == 0)
        pt->partition_count = 1;
    else if (counts[2] == 0)
        pt->partition_count = 2;
    else if (counts[3] == 0)
        pt->partition_count = 3;
    else
        pt->partition_count = 4;



    for (i = 0; i < 4; i++)
        pt->coverage_bitmaps[i] = 0;

    int texels_to_process = ASTCEncode->bsd.texelcount_for_bitmap_partitioning;

    //# was 64 bits changed to 32 bit
    //# this will effect results and need to be fixed for GPU use
    if (texels_to_process > COVERAGE_BITMAPS_MAX)
        texels_to_process = COVERAGE_BITMAPS_MAX;

    uint64_cl shiftbit = 1;

    for (i = 0; i < texels_to_process; i++) {
        pt->coverage_bitmaps[pt->partition_of_texel[i]] |= shiftbit << i;
    }
}

void generate_partition_tables(int xdim, int ydim, int zdim, __global ASTC_Encode *ASTCEncode) {
    int i;
    generate_one_partition_table(xdim, ydim, zdim, 1, 0, &ASTCEncode->partition_tables[1][0], ASTCEncode);
    for (i = 0; i < PARTITION_COUNT; i++) {
        generate_one_partition_table(xdim, ydim, zdim, 2, i, &ASTCEncode->partition_tables[2][i], ASTCEncode);
        generate_one_partition_table(xdim, ydim, zdim, 3, i, &ASTCEncode->partition_tables[3][i], ASTCEncode);
        generate_one_partition_table(xdim, ydim, zdim, 4, i, &ASTCEncode->partition_tables[4][i], ASTCEncode);
    }
    partition_table_zap_equal_elements(xdim, ydim, zdim, &ASTCEncode->partition_tables[2][0]);
    partition_table_zap_equal_elements(xdim, ydim, zdim, &ASTCEncode->partition_tables[3][0]);
    partition_table_zap_equal_elements(xdim, ydim, zdim, &ASTCEncode->partition_tables[4][0]);
}

void prepare_angular_tables(__global ASTC_Encode *ASTCEncode) {
    int i, j;
    int max_angular_steps_needed_for_quant_steps[40];
    for (i = 0; i < ANGULAR_STEPS; i++) {
        ASTCEncode->stepsizes[i] = 1.0f / angular_steppings[i];
        ASTCEncode->stepsizes_sqr[i] = ASTCEncode->stepsizes[i] * ASTCEncode->stepsizes[i];

        for (j = 0; j < SINCOS_STEPS; j++) {
            ASTCEncode->sin_table[j][i] = static_cast < float >(sin((2.0f * M_PI / (SINCOS_STEPS - 1.0f)) * angular_steppings[i] * j));
            ASTCEncode->cos_table[j][i] = static_cast < float >(cos((2.0f * M_PI / (SINCOS_STEPS - 1.0f)) * angular_steppings[i] * j));
        }

        int p = static_cast < int >(floor(angular_steppings[i])) + 1;
        max_angular_steps_needed_for_quant_steps[p] = MIN(i + 1, ANGULAR_STEPS - 1);
    }

    for (i = 0; i < 13; i++)
        ASTCEncode->max_angular_steps_needed_for_quant_level[i] = max_angular_steps_needed_for_quant_steps[steps_of_level[i]];

}

void build_quantization_mode_table(__global ASTC_Encode *ASTCEncode) {
    int i, j;
    for (i = 0; i <= 16; i++)
        for (j = 0; j < 128; j++)
            ASTCEncode->quantization_mode_table[i][j] = -1;

    for (i = 0; i < 21; i++)
        for (j = 1; j <= 16; j++) {
            int p = compute_ise_bitcount2(2 * j, (quantization_method)i);
            if (p < 128)
                ASTCEncode->quantization_mode_table[j][p] = i;
        }
    for (i = 0; i <= 16; i++) {
        int largest_value_so_far = -1;
        for (j = 0; j < 128; j++) {
            if (ASTCEncode->quantization_mode_table[i][j] > largest_value_so_far)
                largest_value_so_far = ASTCEncode->quantization_mode_table[i][j];
            else
                ASTCEncode->quantization_mode_table[i][j] = largest_value_so_far;
        }
    }
}

void expand_block_artifact_suppression_host(int xdim, int ydim, int zdim, error_weighting_params * ewp) {
    int x, y, z;
    float centerpos_x = (xdim - 1) * 0.5f;
    float centerpos_y = (ydim - 1) * 0.5f;
    float centerpos_z = (zdim - 1) * 0.5f;
    int   bef = 0;

    for (z = 0; z < zdim; z++)
        for (y = 0; y < ydim; y++)
            for (x = 0; x < xdim; x++) {
                float xdif = (x - centerpos_x) / xdim;
                float ydif = (y - centerpos_y) / ydim;
                float zdif = (z - centerpos_z) / zdim;

                float wdif = 0.36f;
                float dist = sqrt(xdif * xdif + ydif * ydif + zdif * zdif + wdif * wdif);
                if (bef < MAX_TEXELS_PER_BLOCK) {
                    ewp->block_artifact_suppression_expanded[bef] = pow(dist, ewp->block_artifact_suppression);
                    bef++;
                }
            }
}

void set_block_size_descriptor(int xdim, int ydim, int zdim, __global ASTC_Encode *ASTCEncode) {
#ifdef ASTC_ENABLE_3D_SUPPORT
    if (zdim > 1)
        construct_block_size_descriptor_3d_host(xdim, ydim, zdim, &ASTCEncode->bsd);
    else
#else
    IGNOREPARAM(zdim);
#endif
        construct_block_size_descriptor_2d_host(xdim, ydim, &ASTCEncode->bsd);
}

//-----------------------------------------------------
#ifdef ASTC_ENABLE_3D_SUPPORT
static void initialize_decimation_table_3d(
    // dimensions of the block
    int xdim, int ydim, int zdim,
    // number of grid points in 3d weight grid
    int x_weights, int y_weights, int z_weights, decimation_table * dt) {
    int i, j;
    int x, y, z;

    int texels_per_block = xdim * ydim * zdim;
    int weights_per_block = x_weights * y_weights * z_weights;

    int weightcount_of_texel[MAX_TEXELS_PER_BLOCK];
    int grid_weights_of_texel[MAX_TEXELS_PER_BLOCK][4];
    int weights_of_texel[MAX_TEXELS_PER_BLOCK][4];

    int texelcount_of_weight[MAX_WEIGHTS_PER_BLOCK];
    int texels_of_weight[MAX_WEIGHTS_PER_BLOCK][MAX_TEXELS_PER_BLOCK];
    int texelweights_of_weight[MAX_WEIGHTS_PER_BLOCK][MAX_TEXELS_PER_BLOCK];

    for (i = 0; i < weights_per_block; i++)
        texelcount_of_weight[i] = 0;
    for (i = 0; i < texels_per_block; i++)
        weightcount_of_texel[i] = 0;

    for (z = 0; z < zdim; z++)
        for (y = 0; y < ydim; y++)
            for (x = 0; x < xdim; x++) {
                int texel = (z * ydim + y) * xdim + x;

                int x_weight = (((1024 + xdim / 2) / (xdim - 1)) * x * (x_weights - 1) + 32) >> 6;
                int y_weight = (((1024 + ydim / 2) / (ydim - 1)) * y * (y_weights - 1) + 32) >> 6;
                int z_weight = (((1024 + zdim / 2) / (zdim - 1)) * z * (z_weights - 1) + 32) >> 6;

                int x_weight_frac = x_weight & 0xF;
                int y_weight_frac = y_weight & 0xF;
                int z_weight_frac = z_weight & 0xF;
                int x_weight_int = x_weight >> 4;
                int y_weight_int = y_weight >> 4;
                int z_weight_int = z_weight >> 4;
                int qweight[4];
                int weight[4];
                qweight[0] = (z_weight_int * y_weights + y_weight_int) * x_weights + x_weight_int;
                qweight[3] = ((z_weight_int + 1) * y_weights + (y_weight_int + 1)) * x_weights + (x_weight_int + 1);

                // simplex interpolation
                int fs = x_weight_frac;
                int ft = y_weight_frac;
                int fp = z_weight_frac;

                int cas = ((fs > ft) << 2) + ((ft > fp) << 1) + ((fs > fp));
                int N = x_weights;
                int NM = x_weights * y_weights;

                int s1, s2, w0, w1, w2, w3;
                switch (cas) {
                case 7:
                    s1 = 1;
                    s2 = N;
                    w0 = 16 - fs;
                    w1 = fs - ft;
                    w2 = ft - fp;
                    w3 = fp;
                    break;
                case 3:
                    s1 = N;
                    s2 = 1;
                    w0 = 16 - ft;
                    w1 = ft - fs;
                    w2 = fs - fp;
                    w3 = fp;
                    break;
                case 5:
                    s1 = 1;
                    s2 = NM;
                    w0 = 16 - fs;
                    w1 = fs - fp;
                    w2 = fp - ft;
                    w3 = ft;
                    break;
                case 4:
                    s1 = NM;
                    s2 = 1;
                    w0 = 16 - fp;
                    w1 = fp - fs;
                    w2 = fs - ft;
                    w3 = ft;
                    break;
                case 2:
                    s1 = N;
                    s2 = NM;
                    w0 = 16 - ft;
                    w1 = ft - fp;
                    w2 = fp - fs;
                    w3 = fs;
                    break;
                case 0:
                    s1 = NM;
                    s2 = N;
                    w0 = 16 - fp;
                    w1 = fp - ft;
                    w2 = ft - fs;
                    w3 = fs;
                    break;

                default:
                    s1 = NM;
                    s2 = N;
                    w0 = 16 - fp;
                    w1 = fp - ft;
                    w2 = ft - fs;
                    w3 = fs;
                    break;
                }

                qweight[1] = qweight[0] + s1;
                qweight[2] = qweight[1] + s2;
                weight[0] = w0;
                weight[1] = w1;
                weight[2] = w2;
                weight[3] = w3;

                /*
                for(i=0;i<4;i++) weight[i] <<= 4; */

                for (i = 0; i < 4; i++)
                    if (weight[i] != 0) {
                        grid_weights_of_texel[texel][weightcount_of_texel[texel]] = qweight[i];
                        weights_of_texel[texel][weightcount_of_texel[texel]] = weight[i];
                        weightcount_of_texel[texel]++;
                        texels_of_weight[qweight[i]][texelcount_of_weight[qweight[i]]] = texel;
                        texelweights_of_weight[qweight[i]][texelcount_of_weight[qweight[i]]] = weight[i];
                        texelcount_of_weight[qweight[i]]++;
                    }
            }

    for (i = 0; i < texels_per_block; i++) {
        dt->texel_num_weights[i] = (uint8_t)weightcount_of_texel[i];

        // ensure that all 4 entries are actually initialized.
        // This allows a branch-free implemntation of compute_value_of_texel_flt()
        for (j = 0; j < 4; j++) {
            dt->texel_weights_int[i][j] = 0;
            dt->texel_weights_float[i][j] = 0.0f;
            dt->texel_weights[i][j] = 0;
        }

        for (j = 0; j < weightcount_of_texel[i]; j++) {
            dt->texel_weights_int[i][j] = (uint8_t)weights_of_texel[i][j];
            dt->texel_weights_float[i][j] = static_cast < float >(weights_of_texel[i][j]) * (1.0f / TEXEL_WEIGHT_SUM);
            dt->texel_weights[i][j] = (uint8_t)grid_weights_of_texel[i][j];
        }
    }

    for (i = 0; i < weights_per_block; i++) {
        dt->weight_num_texels[i] = (uint8_t)texelcount_of_weight[i];
        for (j = 0; j < texelcount_of_weight[i]; j++) {
            dt->weight_texel[i][j] = (uint8_t)texels_of_weight[i][j];
            dt->weights_int[i][j] = (uint8_t)texelweights_of_weight[i][j];
            dt->weights_flt[i][j] = static_cast < float >(texelweights_of_weight[i][j]);
        }
    }

    dt->num_texels = texels_per_block;
    dt->num_weights = weights_per_block;
}
#endif

// routine to write up to 8 bits
static inline void write_bits(int value, int bitcount, int bitoffset, uint8_t * ptr) {
    int mask = (1 << bitcount) - 1;
    value &= mask;
    ptr += bitoffset >> 3;
    bitoffset &= 7;
    value <<= bitoffset;
    mask <<= bitoffset;
    mask = ~mask;

    ptr[0] &= mask;
    ptr[0] |= value;
    ptr[1] &= mask >> 8;
    ptr[1] |= value >> 8;
}


// routine to read up to 8 bits
static inline int read_bits(int bitcount, int bitoffset, const uint8_t * ptr) {
    int mask = (1 << bitcount) - 1;
    ptr += bitoffset >> 3;
    bitoffset &= 7;
    int value = ptr[0] | (ptr[1] << 8);
    value >>= bitoffset;
    value &= mask;
    return value;
}

// unpacked trit quintuplets <low,_,_,_,high> for each packed-quint value
static const uint8_t trits_of_integer[256][5] = {
    { 0, 0, 0, 0, 0 },{ 1, 0, 0, 0, 0 },{ 2, 0, 0, 0, 0 },{ 0, 0, 2, 0, 0 },
    { 0, 1, 0, 0, 0 },{ 1, 1, 0, 0, 0 },{ 2, 1, 0, 0, 0 },{ 1, 0, 2, 0, 0 },
    { 0, 2, 0, 0, 0 },{ 1, 2, 0, 0, 0 },{ 2, 2, 0, 0, 0 },{ 2, 0, 2, 0, 0 },
    { 0, 2, 2, 0, 0 },{ 1, 2, 2, 0, 0 },{ 2, 2, 2, 0, 0 },{ 2, 0, 2, 0, 0 },
    { 0, 0, 1, 0, 0 },{ 1, 0, 1, 0, 0 },{ 2, 0, 1, 0, 0 },{ 0, 1, 2, 0, 0 },
    { 0, 1, 1, 0, 0 },{ 1, 1, 1, 0, 0 },{ 2, 1, 1, 0, 0 },{ 1, 1, 2, 0, 0 },
    { 0, 2, 1, 0, 0 },{ 1, 2, 1, 0, 0 },{ 2, 2, 1, 0, 0 },{ 2, 1, 2, 0, 0 },
    { 0, 0, 0, 2, 2 },{ 1, 0, 0, 2, 2 },{ 2, 0, 0, 2, 2 },{ 0, 0, 2, 2, 2 },
    { 0, 0, 0, 1, 0 },{ 1, 0, 0, 1, 0 },{ 2, 0, 0, 1, 0 },{ 0, 0, 2, 1, 0 },
    { 0, 1, 0, 1, 0 },{ 1, 1, 0, 1, 0 },{ 2, 1, 0, 1, 0 },{ 1, 0, 2, 1, 0 },
    { 0, 2, 0, 1, 0 },{ 1, 2, 0, 1, 0 },{ 2, 2, 0, 1, 0 },{ 2, 0, 2, 1, 0 },
    { 0, 2, 2, 1, 0 },{ 1, 2, 2, 1, 0 },{ 2, 2, 2, 1, 0 },{ 2, 0, 2, 1, 0 },
    { 0, 0, 1, 1, 0 },{ 1, 0, 1, 1, 0 },{ 2, 0, 1, 1, 0 },{ 0, 1, 2, 1, 0 },
    { 0, 1, 1, 1, 0 },{ 1, 1, 1, 1, 0 },{ 2, 1, 1, 1, 0 },{ 1, 1, 2, 1, 0 },
    { 0, 2, 1, 1, 0 },{ 1, 2, 1, 1, 0 },{ 2, 2, 1, 1, 0 },{ 2, 1, 2, 1, 0 },
    { 0, 1, 0, 2, 2 },{ 1, 1, 0, 2, 2 },{ 2, 1, 0, 2, 2 },{ 1, 0, 2, 2, 2 },
    { 0, 0, 0, 2, 0 },{ 1, 0, 0, 2, 0 },{ 2, 0, 0, 2, 0 },{ 0, 0, 2, 2, 0 },
    { 0, 1, 0, 2, 0 },{ 1, 1, 0, 2, 0 },{ 2, 1, 0, 2, 0 },{ 1, 0, 2, 2, 0 },
    { 0, 2, 0, 2, 0 },{ 1, 2, 0, 2, 0 },{ 2, 2, 0, 2, 0 },{ 2, 0, 2, 2, 0 },
    { 0, 2, 2, 2, 0 },{ 1, 2, 2, 2, 0 },{ 2, 2, 2, 2, 0 },{ 2, 0, 2, 2, 0 },
    { 0, 0, 1, 2, 0 },{ 1, 0, 1, 2, 0 },{ 2, 0, 1, 2, 0 },{ 0, 1, 2, 2, 0 },
    { 0, 1, 1, 2, 0 },{ 1, 1, 1, 2, 0 },{ 2, 1, 1, 2, 0 },{ 1, 1, 2, 2, 0 },
    { 0, 2, 1, 2, 0 },{ 1, 2, 1, 2, 0 },{ 2, 2, 1, 2, 0 },{ 2, 1, 2, 2, 0 },
    { 0, 2, 0, 2, 2 },{ 1, 2, 0, 2, 2 },{ 2, 2, 0, 2, 2 },{ 2, 0, 2, 2, 2 },
    { 0, 0, 0, 0, 2 },{ 1, 0, 0, 0, 2 },{ 2, 0, 0, 0, 2 },{ 0, 0, 2, 0, 2 },
    { 0, 1, 0, 0, 2 },{ 1, 1, 0, 0, 2 },{ 2, 1, 0, 0, 2 },{ 1, 0, 2, 0, 2 },
    { 0, 2, 0, 0, 2 },{ 1, 2, 0, 0, 2 },{ 2, 2, 0, 0, 2 },{ 2, 0, 2, 0, 2 },
    { 0, 2, 2, 0, 2 },{ 1, 2, 2, 0, 2 },{ 2, 2, 2, 0, 2 },{ 2, 0, 2, 0, 2 },
    { 0, 0, 1, 0, 2 },{ 1, 0, 1, 0, 2 },{ 2, 0, 1, 0, 2 },{ 0, 1, 2, 0, 2 },
    { 0, 1, 1, 0, 2 },{ 1, 1, 1, 0, 2 },{ 2, 1, 1, 0, 2 },{ 1, 1, 2, 0, 2 },
    { 0, 2, 1, 0, 2 },{ 1, 2, 1, 0, 2 },{ 2, 2, 1, 0, 2 },{ 2, 1, 2, 0, 2 },
    { 0, 2, 2, 2, 2 },{ 1, 2, 2, 2, 2 },{ 2, 2, 2, 2, 2 },{ 2, 0, 2, 2, 2 },
    { 0, 0, 0, 0, 1 },{ 1, 0, 0, 0, 1 },{ 2, 0, 0, 0, 1 },{ 0, 0, 2, 0, 1 },
    { 0, 1, 0, 0, 1 },{ 1, 1, 0, 0, 1 },{ 2, 1, 0, 0, 1 },{ 1, 0, 2, 0, 1 },
    { 0, 2, 0, 0, 1 },{ 1, 2, 0, 0, 1 },{ 2, 2, 0, 0, 1 },{ 2, 0, 2, 0, 1 },
    { 0, 2, 2, 0, 1 },{ 1, 2, 2, 0, 1 },{ 2, 2, 2, 0, 1 },{ 2, 0, 2, 0, 1 },
    { 0, 0, 1, 0, 1 },{ 1, 0, 1, 0, 1 },{ 2, 0, 1, 0, 1 },{ 0, 1, 2, 0, 1 },
    { 0, 1, 1, 0, 1 },{ 1, 1, 1, 0, 1 },{ 2, 1, 1, 0, 1 },{ 1, 1, 2, 0, 1 },
    { 0, 2, 1, 0, 1 },{ 1, 2, 1, 0, 1 },{ 2, 2, 1, 0, 1 },{ 2, 1, 2, 0, 1 },
    { 0, 0, 1, 2, 2 },{ 1, 0, 1, 2, 2 },{ 2, 0, 1, 2, 2 },{ 0, 1, 2, 2, 2 },
    { 0, 0, 0, 1, 1 },{ 1, 0, 0, 1, 1 },{ 2, 0, 0, 1, 1 },{ 0, 0, 2, 1, 1 },
    { 0, 1, 0, 1, 1 },{ 1, 1, 0, 1, 1 },{ 2, 1, 0, 1, 1 },{ 1, 0, 2, 1, 1 },
    { 0, 2, 0, 1, 1 },{ 1, 2, 0, 1, 1 },{ 2, 2, 0, 1, 1 },{ 2, 0, 2, 1, 1 },
    { 0, 2, 2, 1, 1 },{ 1, 2, 2, 1, 1 },{ 2, 2, 2, 1, 1 },{ 2, 0, 2, 1, 1 },
    { 0, 0, 1, 1, 1 },{ 1, 0, 1, 1, 1 },{ 2, 0, 1, 1, 1 },{ 0, 1, 2, 1, 1 },
    { 0, 1, 1, 1, 1 },{ 1, 1, 1, 1, 1 },{ 2, 1, 1, 1, 1 },{ 1, 1, 2, 1, 1 },
    { 0, 2, 1, 1, 1 },{ 1, 2, 1, 1, 1 },{ 2, 2, 1, 1, 1 },{ 2, 1, 2, 1, 1 },
    { 0, 1, 1, 2, 2 },{ 1, 1, 1, 2, 2 },{ 2, 1, 1, 2, 2 },{ 1, 1, 2, 2, 2 },
    { 0, 0, 0, 2, 1 },{ 1, 0, 0, 2, 1 },{ 2, 0, 0, 2, 1 },{ 0, 0, 2, 2, 1 },
    { 0, 1, 0, 2, 1 },{ 1, 1, 0, 2, 1 },{ 2, 1, 0, 2, 1 },{ 1, 0, 2, 2, 1 },
    { 0, 2, 0, 2, 1 },{ 1, 2, 0, 2, 1 },{ 2, 2, 0, 2, 1 },{ 2, 0, 2, 2, 1 },
    { 0, 2, 2, 2, 1 },{ 1, 2, 2, 2, 1 },{ 2, 2, 2, 2, 1 },{ 2, 0, 2, 2, 1 },
    { 0, 0, 1, 2, 1 },{ 1, 0, 1, 2, 1 },{ 2, 0, 1, 2, 1 },{ 0, 1, 2, 2, 1 },
    { 0, 1, 1, 2, 1 },{ 1, 1, 1, 2, 1 },{ 2, 1, 1, 2, 1 },{ 1, 1, 2, 2, 1 },
    { 0, 2, 1, 2, 1 },{ 1, 2, 1, 2, 1 },{ 2, 2, 1, 2, 1 },{ 2, 1, 2, 2, 1 },
    { 0, 2, 1, 2, 2 },{ 1, 2, 1, 2, 2 },{ 2, 2, 1, 2, 2 },{ 2, 1, 2, 2, 2 },
    { 0, 0, 0, 1, 2 },{ 1, 0, 0, 1, 2 },{ 2, 0, 0, 1, 2 },{ 0, 0, 2, 1, 2 },
    { 0, 1, 0, 1, 2 },{ 1, 1, 0, 1, 2 },{ 2, 1, 0, 1, 2 },{ 1, 0, 2, 1, 2 },
    { 0, 2, 0, 1, 2 },{ 1, 2, 0, 1, 2 },{ 2, 2, 0, 1, 2 },{ 2, 0, 2, 1, 2 },
    { 0, 2, 2, 1, 2 },{ 1, 2, 2, 1, 2 },{ 2, 2, 2, 1, 2 },{ 2, 0, 2, 1, 2 },
    { 0, 0, 1, 1, 2 },{ 1, 0, 1, 1, 2 },{ 2, 0, 1, 1, 2 },{ 0, 1, 2, 1, 2 },
    { 0, 1, 1, 1, 2 },{ 1, 1, 1, 1, 2 },{ 2, 1, 1, 1, 2 },{ 1, 1, 2, 1, 2 },
    { 0, 2, 1, 1, 2 },{ 1, 2, 1, 1, 2 },{ 2, 2, 1, 1, 2 },{ 2, 1, 2, 1, 2 },
    { 0, 2, 2, 2, 2 },{ 1, 2, 2, 2, 2 },{ 2, 2, 2, 2, 2 },{ 2, 1, 2, 2, 2 },
};

// unpacked quint triplets <low,middle,high> for each packed-quint value
static const uint8_t quints_of_integer[128][3] = {
    { 0, 0, 0 },{ 1, 0, 0 },{ 2, 0, 0 },{ 3, 0, 0 },
    { 4, 0, 0 },{ 0, 4, 0 },{ 4, 4, 0 },{ 4, 4, 4 },
    { 0, 1, 0 },{ 1, 1, 0 },{ 2, 1, 0 },{ 3, 1, 0 },
    { 4, 1, 0 },{ 1, 4, 0 },{ 4, 4, 1 },{ 4, 4, 4 },
    { 0, 2, 0 },{ 1, 2, 0 },{ 2, 2, 0 },{ 3, 2, 0 },
    { 4, 2, 0 },{ 2, 4, 0 },{ 4, 4, 2 },{ 4, 4, 4 },
    { 0, 3, 0 },{ 1, 3, 0 },{ 2, 3, 0 },{ 3, 3, 0 },
    { 4, 3, 0 },{ 3, 4, 0 },{ 4, 4, 3 },{ 4, 4, 4 },
    { 0, 0, 1 },{ 1, 0, 1 },{ 2, 0, 1 },{ 3, 0, 1 },
    { 4, 0, 1 },{ 0, 4, 1 },{ 4, 0, 4 },{ 0, 4, 4 },
    { 0, 1, 1 },{ 1, 1, 1 },{ 2, 1, 1 },{ 3, 1, 1 },
    { 4, 1, 1 },{ 1, 4, 1 },{ 4, 1, 4 },{ 1, 4, 4 },
    { 0, 2, 1 },{ 1, 2, 1 },{ 2, 2, 1 },{ 3, 2, 1 },
    { 4, 2, 1 },{ 2, 4, 1 },{ 4, 2, 4 },{ 2, 4, 4 },
    { 0, 3, 1 },{ 1, 3, 1 },{ 2, 3, 1 },{ 3, 3, 1 },
    { 4, 3, 1 },{ 3, 4, 1 },{ 4, 3, 4 },{ 3, 4, 4 },
    { 0, 0, 2 },{ 1, 0, 2 },{ 2, 0, 2 },{ 3, 0, 2 },
    { 4, 0, 2 },{ 0, 4, 2 },{ 2, 0, 4 },{ 3, 0, 4 },
    { 0, 1, 2 },{ 1, 1, 2 },{ 2, 1, 2 },{ 3, 1, 2 },
    { 4, 1, 2 },{ 1, 4, 2 },{ 2, 1, 4 },{ 3, 1, 4 },
    { 0, 2, 2 },{ 1, 2, 2 },{ 2, 2, 2 },{ 3, 2, 2 },
    { 4, 2, 2 },{ 2, 4, 2 },{ 2, 2, 4 },{ 3, 2, 4 },
    { 0, 3, 2 },{ 1, 3, 2 },{ 2, 3, 2 },{ 3, 3, 2 },
    { 4, 3, 2 },{ 3, 4, 2 },{ 2, 3, 4 },{ 3, 3, 4 },
    { 0, 0, 3 },{ 1, 0, 3 },{ 2, 0, 3 },{ 3, 0, 3 },
    { 4, 0, 3 },{ 0, 4, 3 },{ 0, 0, 4 },{ 1, 0, 4 },
    { 0, 1, 3 },{ 1, 1, 3 },{ 2, 1, 3 },{ 3, 1, 3 },
    { 4, 1, 3 },{ 1, 4, 3 },{ 0, 1, 4 },{ 1, 1, 4 },
    { 0, 2, 3 },{ 1, 2, 3 },{ 2, 2, 3 },{ 3, 2, 3 },
    { 4, 2, 3 },{ 2, 4, 3 },{ 0, 2, 4 },{ 1, 2, 4 },
    { 0, 3, 3 },{ 1, 3, 3 },{ 2, 3, 3 },{ 3, 3, 3 },
    { 4, 3, 3 },{ 3, 4, 3 },{ 0, 3, 4 },{ 1, 3, 4 },
};

int bitrev8(int p) {
    p = ((p & 0xF) << 4) | ((p >> 4) & 0xF);
    p = ((p & 0x33) << 2) | ((p >> 2) & 0x33);
    p = ((p & 0x55) << 1) | ((p >> 1) & 0x55);
    return p;
}

void decode_ise(int quantization_level, int elements, const uint8_t * input_data, uint8_t * output_data, int bit_offset) {
    int i;
    // note: due to how the the trit/quint-block unpacking is done in this function,
    // we may write more temporary results than the number of outputs
    // The maximum actual number of results is 64 bit, but we keep 4 additional elements
    // of padding.
    uint8_t results[68];
    uint8_t tq_blocks[22];        // trit-blocks or quint-blocks

    int bits, trits, quints;
    find_number_of_bits_trits_quints(quantization_level, &bits, &trits, &quints);

    int lcounter = 0;
    int hcounter = 0;

    // trit-blocks or quint-blocks must be zeroed out before we collect them in the loop below.
    for (i = 0; i < 22; i++)
        tq_blocks[i] = 0;

    // collect bits for each element, as well as bits for any trit-blocks and quint-blocks.
    for (i = 0; i < elements; i++) {
        results[i] = (uint8_t)read_bits(bits, bit_offset, input_data);
        bit_offset += bits;
        if (trits) {
            static const int bits_to_read[5] = { 2, 2, 1, 2, 1 };
            static const int block_shift[5] = { 0, 2, 4, 5, 7 };
            static const int next_lcounter[5] = { 1, 2, 3, 4, 0 };
            static const int hcounter_incr[5] = { 0, 0, 0, 0, 1 };
            int tdata = read_bits(bits_to_read[lcounter], bit_offset, input_data);
            bit_offset += bits_to_read[lcounter];
            tq_blocks[hcounter] |= tdata << block_shift[lcounter];
            hcounter += hcounter_incr[lcounter];
            lcounter = next_lcounter[lcounter];
        }
        if (quints) {
            static const int bits_to_read[3] = { 3, 2, 2 };
            static const int block_shift[3] = { 0, 3, 5 };
            static const int next_lcounter[3] = { 1, 2, 0 };
            static const int hcounter_incr[3] = { 0, 0, 1 };
            int tdata = read_bits(bits_to_read[lcounter], bit_offset, input_data);
            bit_offset += bits_to_read[lcounter];
            tq_blocks[hcounter] |= tdata << block_shift[lcounter];
            hcounter += hcounter_incr[lcounter];
            lcounter = next_lcounter[lcounter];
        }
    }


    // unpack trit-blocks or quint-blocks as needed
    if (trits) {
        int trit_blocks = (elements + 4) / 5;
        for (i = 0; i < trit_blocks; i++) {
            const uint8_t *tritptr = trits_of_integer[tq_blocks[i]];
            results[5 * i] |= tritptr[0] << bits;
            results[5 * i + 1] |= tritptr[1] << bits;
            results[5 * i + 2] |= tritptr[2] << bits;
            results[5 * i + 3] |= tritptr[3] << bits;
            results[5 * i + 4] |= tritptr[4] << bits;
        }
    }

    if (quints) {
        int quint_blocks = (elements + 2) / 3;
        for (i = 0; i < quint_blocks; i++) {
            const uint8_t *quintptr = quints_of_integer[tq_blocks[i]];
            results[3 * i] |= quintptr[0] << bits;
            results[3 * i + 1] |= quintptr[1] << bits;
            results[3 * i + 2] |= quintptr[2] << bits;
        }
    }

    for (i = 0; i < elements; i++)
        output_data[i] = results[i];
}

void InitializeASTCSettingsForSetBlockSize(__global ASTC_Encode *ASTCEncode) {
    ASTCEncode->m_target_bitrate = 0;
    int xdim_2d = ASTCEncode->m_xdim;
    int ydim_2d = ASTCEncode->m_ydim;
    float log10_texels_2d = log((float)(xdim_2d * ydim_2d)) / log(10.0f);

#ifdef ASTC_ENABLE_3D_SUPPORT
    int xdim_3d = ASTCEncode->m_xdim;
    int ydim_3d = ASTCEncode->m_ydim;
    int zdim_3d = ASTCEncode->m_zdim;
    float log10_texels_3d = 0.0f;
    log10_texels_3d = log((float)(xdim_3d * ydim_3d * zdim_3d)) / log(10.0f);
    float dblimit_autoset_3d = 0.0;
#endif

    int     plimit_autoset      = -1;
    float   dblimit_autoset_2d  = 0.0;
    float   oplimit_autoset     = 0.0;
    float   mincorrel_autoset   = 0.0;
    float   bmc_autoset         = 0.0;
    int     maxiters_autoset    = 0;


    /**********************************************************************************
    ASTC Settingsto review for quality & perfromance, these are the setting found in
    astc_main for astcenc sample application command line tool
    fast
        plimit_autoset = 4;
        oplimit_autoset = 1.0;
        mincorrel_autoset = 0.5;
        dblimit_autoset_2d = MAX(85 - 35 * log10_texels_2d, 63 - 19 * log10_texels_2d);
        dblimit_autoset_3d = MAX(85 - 35 * log10_texels_3d, 63 - 19 * log10_texels_3d);
        bmc_autoset = 50;
        maxiters_autoset = 1;
    medium
        plimit_autoset = 25;
        oplimit_autoset = 1.2f;
        mincorrel_autoset = 0.75f;
        dblimit_autoset_2d = MAX(95 - 35 * log10_texels_2d, 70 - 19 * log10_texels_2d);
        dblimit_autoset_3d = MAX(95 - 35 * log10_texels_3d, 70 - 19 * log10_texels_3d);
        bmc_autoset = 75;
        maxiters_autoset = 2;
    thorough
        plimit_autoset = 100;
        oplimit_autoset = 2.5f;
        mincorrel_autoset = 0.95f;
        dblimit_autoset_2d = MAX(105 - 35 * log10_texels_2d, 77 - 19 * log10_texels_2d);
        dblimit_autoset_3d = MAX(105 - 35 * log10_texels_3d, 77 - 19 * log10_texels_3d);
        bmc_autoset = 95;
        maxiters_autoset
    exhaustive
        #define PARTITION_BITS 10
        #define PARTITION_COUNT (1 << PARTITION_BITS)
        plimit_autoset = PARTITION_COUNT;
        oplimit_autoset = 1000.0f;
        mincorrel_autoset = 0.99f;
        dblimit_autoset_2d = 999.0f;
        dblimit_autoset_3d = 999.0f;
        bmc_autoset = 100;
        maxiters_autoset = 4;
    ***************************************************************************************************/

    // Codec Speed Setting Defaults based on Quality Settings
    float QualityScale; // Set quality normalized per process setting with a range of 0.0 to 1.0f
    if (ASTCEncode->m_Quality < 0.02f) {
        // Very Fast
        oplimit_autoset     = 1.0;
        mincorrel_autoset   = 0.5;
        plimit_autoset      = 1;
        bmc_autoset         = 5.0f;
        maxiters_autoset    = 1;
        dblimit_autoset_2d  = MAX(70 - 35 * log10_texels_2d, 53 - 19 * log10_texels_2d);
    } else if (ASTCEncode->m_Quality < 0.05f) {
        // Fast:
        QualityScale = ASTCEncode->m_Quality/0.05f;
        oplimit_autoset     = 1.0;
        mincorrel_autoset   = 0.5;
        plimit_autoset      = 4;
        bmc_autoset         = 5.0f+(45.0f*QualityScale);  // max 50
        maxiters_autoset    = 1;
        dblimit_autoset_2d  = MAX(85 - 35 * log10_texels_2d, 63 - 19 * log10_texels_2d);
    } else if (ASTCEncode->m_Quality <= 0.20f) {
        // Medium  set to match near Compressonator BC7 Default Quality 0.05f setting
        QualityScale = ASTCEncode->m_Quality/0.20f;
        oplimit_autoset     = 1.2f;
        mincorrel_autoset   = 0.75f;
        plimit_autoset      = 15+(int)round(10.0f*QualityScale); // max around 25;
        bmc_autoset         = 57.0f+(18.0f*QualityScale);  // max 75;
        maxiters_autoset    = 2;
        dblimit_autoset_2d  = MAX(95 - 35 * log10_texels_2d, 70 - 19 * log10_texels_2d);
    } else if (ASTCEncode->m_Quality <= 0.60f) {
        // Thorough
        QualityScale = ASTCEncode->m_Quality/0.60f;
        oplimit_autoset     = 1.2f + (1.3f*QualityScale); // max 2.5f;
        mincorrel_autoset   = 0.95f;
        plimit_autoset      = 25+(int)round(75.0f*QualityScale); // max around 100
        bmc_autoset         = 75.0f+(25.0f*QualityScale);  // max 95;
        maxiters_autoset    = 4;
        dblimit_autoset_2d  = MAX(105 - 35 * log10_texels_2d, 77 - 19 * log10_texels_2d);
    } else {
        // Exhaustive
        QualityScale =  ASTCEncode->m_Quality;
        oplimit_autoset     = 2.5f+ (997.5f*QualityScale); // max 1000.0f;
        mincorrel_autoset   = 0.99f;
        plimit_autoset      = 100 + (int)round(923.0f * QualityScale);  // max 1024
        bmc_autoset         = 95.0f+(5.0f*QualityScale);  // max 100;
        maxiters_autoset    = 4;
        dblimit_autoset_2d  = 999.0f;
    }

    int partitions_to_test = plimit_autoset;
    float dblimit_2d = dblimit_autoset_2d;
    float oplimit = oplimit_autoset;
    float mincorrel = mincorrel_autoset;

#ifdef ASTC_ENABLE_3D_SUPPORT
    float dblimit_3d = dblimit_set_by_user ? dblimit_user_specified : dblimit_autoset_3d;
#endif

    ASTCEncode->m_ewp.rgb_power = 1.0f;
    ASTCEncode->m_ewp.alpha_power = 1.0f;
    ASTCEncode->m_ewp.rgb_base_weight = 1.0f;
    ASTCEncode->m_ewp.alpha_base_weight = 1.0f;
    ASTCEncode->m_ewp.rgb_mean_weight = 0.0f;
    ASTCEncode->m_ewp.rgb_stdev_weight = 0.0f;
    ASTCEncode->m_ewp.alpha_mean_weight = 0.0f;
    ASTCEncode->m_ewp.alpha_stdev_weight = 0.0f;

    ASTCEncode->m_ewp.rgb_mean_and_stdev_mixing = 0.0f;
    ASTCEncode->m_ewp.mean_stdev_radius = 0;
    ASTCEncode->m_ewp.enable_rgb_scale_with_alpha = 0;
    ASTCEncode->m_ewp.alpha_radius = 0;

    ASTCEncode->m_ewp.block_artifact_suppression = 0.0f;
    ASTCEncode->m_ewp.rgba_weights[0] = 1.0f;
    ASTCEncode->m_ewp.rgba_weights[1] = 1.0f;
    ASTCEncode->m_ewp.rgba_weights[2] = 1.0f;
    ASTCEncode->m_ewp.rgba_weights[3] = 1.0f;
    ASTCEncode->m_ewp.ra_normal_angular_scale = 0;
    ASTCEncode->m_ewp.max_refinement_iters = maxiters_autoset;

    ASTCEncode->m_ewp.block_mode_cutoff = bmc_autoset / 100.0f;

    float texel_avg_error_limit_2d;
    float texel_avg_error_limit_3d;

    if (ASTCEncode->m_rgb_force_use_of_hdr == 0) {
        texel_avg_error_limit_2d = pow(0.1f, dblimit_2d * 0.1f) * 65535.0f * 65535.0f;
#ifdef ASTC_ENABLE_3D_SUPPORT
        texel_avg_error_limit_3d = pow(0.1f, dblimit_3d * 0.1f) * 65535.0f * 65535.0f;
#endif
    } else {
        texel_avg_error_limit_2d = 0.0f;
        texel_avg_error_limit_3d = 0.0f;
    }
    ASTCEncode->m_ewp.partition_1_to_2_limit = oplimit;
    ASTCEncode->m_ewp.lowest_correlation_cutoff = mincorrel;

    if (partitions_to_test < 1)
        partitions_to_test = 1;
    else if (partitions_to_test > PARTITION_COUNT)
        partitions_to_test = PARTITION_COUNT;
    ASTCEncode->m_ewp.partition_search_limit = partitions_to_test;

    // Specifying the error weight of a color component as 0 is not allowed.
    // If weights are 0, then they are instead set to a small positive value.

    float max_color_component_weight = MAX(MAX(ASTCEncode->m_ewp.rgba_weights[0], ASTCEncode->m_ewp.rgba_weights[1]),
                                           MAX(ASTCEncode->m_ewp.rgba_weights[2], ASTCEncode->m_ewp.rgba_weights[3]));
    ASTCEncode->m_ewp.rgba_weights[0] = MAX(ASTCEncode->m_ewp.rgba_weights[0], max_color_component_weight / 1000.0f);
    ASTCEncode->m_ewp.rgba_weights[1] = MAX(ASTCEncode->m_ewp.rgba_weights[1], max_color_component_weight / 1000.0f);
    ASTCEncode->m_ewp.rgba_weights[2] = MAX(ASTCEncode->m_ewp.rgba_weights[2], max_color_component_weight / 1000.0f);
    ASTCEncode->m_ewp.rgba_weights[3] = MAX(ASTCEncode->m_ewp.rgba_weights[3], max_color_component_weight / 1000.0f);

    // Allocate arrays for image data and load results.
    ASTCEncode->m_ewp.texel_avg_error_limit = texel_avg_error_limit_2d;

    expand_block_artifact_suppression_host(ASTCEncode->m_xdim, ASTCEncode->m_ydim, ASTCEncode->m_zdim, &ASTCEncode->m_ewp);
}

bool init_ASTC(__global ASTC_Encode *ASTCEncode) {
    prepare_angular_tables(ASTCEncode);
    build_quantization_mode_table(ASTCEncode);
    InitializeASTCSettingsForSetBlockSize(ASTCEncode);
    set_block_size_descriptor(ASTCEncode->m_xdim, ASTCEncode->m_ydim, ASTCEncode->m_zdim, ASTCEncode);

#ifdef ASTC_ENABLE_3D_SUPPORT
    ASTCEncode->m_texels_per_block = ASTCEncode->m_xdim * ASTCEncode->m_ydim * ASTCEncode->m_zdim;
#else
    ASTCEncode->m_texels_per_block = ASTCEncode->m_xdim * ASTCEncode->m_ydim;
#endif
    ASTCEncode->m_ptindex = ASTCEncode->m_xdim + 16 * ASTCEncode->m_ydim + 256 * ASTCEncode->m_zdim;
    generate_partition_tables(ASTCEncode->m_xdim, ASTCEncode->m_ydim, ASTCEncode->m_zdim, ASTCEncode);
    return true;
}

}

//=====================================================================================================================================
// CPU Based Decoder code

extern ASTC_Encoder::ASTC_Encode g_ASTCEncode;

void initialize_decimation_table_2d_cpu(
    // dimensions of the block
    int xdim, int ydim,
    // number of grid points in 2d weight grid
    int x_weights, int y_weights, decimation_table_cpu * dt)

{
    int i, j;
    int x, y;

    int texels_per_block = xdim * ydim;
    int weights_per_block = x_weights * y_weights;

    int weightcount_of_texel[MAX_TEXELS_PER_BLOCK];
    int grid_weights_of_texel[MAX_TEXELS_PER_BLOCK][4];
    int weights_of_texel[MAX_TEXELS_PER_BLOCK][4];

    int texelcount_of_weight[MAX_WEIGHTS_PER_BLOCK];
    int texels_of_weight[MAX_WEIGHTS_PER_BLOCK][MAX_TEXELS_PER_BLOCK];
    int texelweights_of_weight[MAX_WEIGHTS_PER_BLOCK][MAX_TEXELS_PER_BLOCK];

    for (i = 0; i < weights_per_block; i++)
        texelcount_of_weight[i] = 0;
    for (i = 0; i < texels_per_block; i++)
        weightcount_of_texel[i] = 0;

    for (y = 0; y < ydim; y++)
        for (x = 0; x < xdim; x++) {
            int texel = y * xdim + x;

            int x_weight = (((1024 + xdim / 2) / (xdim - 1)) * x * (x_weights - 1) + 32) >> 6;
            int y_weight = (((1024 + ydim / 2) / (ydim - 1)) * y * (y_weights - 1) + 32) >> 6;

            int x_weight_frac = x_weight & 0xF;
            int y_weight_frac = y_weight & 0xF;
            int x_weight_int = x_weight >> 4;
            int y_weight_int = y_weight >> 4;
            int qweight[4];
            int weight[4];
            qweight[0] = x_weight_int + y_weight_int * x_weights;
            qweight[1] = qweight[0] + 1;
            qweight[2] = qweight[0] + x_weights;
            qweight[3] = qweight[2] + 1;

            // truncated-precision bilinear interpolation.
            int prod = x_weight_frac * y_weight_frac;

            weight[3] = (prod + 8) >> 4;
            weight[1] = x_weight_frac - weight[3];
            weight[2] = y_weight_frac - weight[3];
            weight[0] = 16 - x_weight_frac - y_weight_frac + weight[3];

            for (i = 0; i < 4; i++)
                if (weight[i] != 0) {
                    grid_weights_of_texel[texel][weightcount_of_texel[texel]] = qweight[i];
                    weights_of_texel[texel][weightcount_of_texel[texel]] = weight[i];
                    weightcount_of_texel[texel]++;
                    texels_of_weight[qweight[i]][texelcount_of_weight[qweight[i]]] = texel;
                    texelweights_of_weight[qweight[i]][texelcount_of_weight[qweight[i]]] = weight[i];
                    texelcount_of_weight[qweight[i]]++;
                }
        }

    for (i = 0; i < texels_per_block; i++) {
        dt->texel_num_weights[i] = (ASTC_Encoder::uint8_t)weightcount_of_texel[i];

        // ensure that all 4 entries are actually initialized.
        // This allows a branch-free implemntation of compute_value_of_texel_flt()
        for (j = 0; j < 4; j++) {
            dt->texel_weights_int[i][j] = 0;
            dt->texel_weights_float[i][j] = 0.0f;
            dt->texel_weights[i][j] = 0;
        }

        for (j = 0; j < weightcount_of_texel[i]; j++) {
            dt->texel_weights_int[i][j] = (ASTC_Encoder::uint8_t)weights_of_texel[i][j];
            dt->texel_weights_float[i][j] = (weights_of_texel[i][j]) * (1.0f / TEXEL_WEIGHT_SUM);
            dt->texel_weights[i][j] = (ASTC_Encoder::uint8_t)grid_weights_of_texel[i][j];
        }
    }

    for (i = 0; i < weights_per_block; i++) {
        dt->weight_num_texels[i] = (ASTC_Encoder::uint8_t)texelcount_of_weight[i];


        for (j = 0; j < texelcount_of_weight[i]; j++) {
            dt->weight_texel[i][j] = (ASTC_Encoder::uint8_t)texels_of_weight[i][j];
            dt->weights_int[i][j] = (ASTC_Encoder::uint8_t)texelweights_of_weight[i][j];
            dt->weights_flt[i][j] = (float)texelweights_of_weight[i][j];
        }
    }

    dt->num_texels = texels_per_block;
    dt->num_weights = weights_per_block;


}

void construct_block_size_descriptor_2d_cpu(int xdim, int ydim, block_size_descriptor_cpu * bsd) {
    int decimation_mode_index[256];    // for each of the 256 entries in the decim_table_array, its index
    int decimation_mode_count = 0;

    int i;
    int x_weights;
    int y_weights;

    for (i = 0; i < 256; i++) {
        decimation_mode_index[i] = -1;
    }

    // gather all the infill-modes that can be used with the current block size
    for (x_weights = 2; x_weights <= 12; x_weights++)
        for (y_weights = 2; y_weights <= 12; y_weights++) {
            if (x_weights * y_weights > MAX_WEIGHTS_PER_BLOCK)
                continue;
            decimation_table_cpu *dt = new decimation_table_cpu;
            decimation_mode_index[y_weights * 16 + x_weights] = decimation_mode_count;
            initialize_decimation_table_2d_cpu(xdim, ydim, x_weights, y_weights, dt);

            int weight_count = x_weights * y_weights;

            int maxprec_1plane = -1;
            int maxprec_2planes = -1;
            for (i = 0; i < 12; i++) {
                int bits_1plane  = ASTC_Encoder::compute_ise_bitcount2(weight_count, (ASTC_Encoder::quantization_method)i);
                int bits_2planes = ASTC_Encoder::compute_ise_bitcount2(2 * weight_count, (ASTC_Encoder::quantization_method)i);
                if (bits_1plane >= MIN_WEIGHT_BITS_PER_BLOCK && bits_1plane <= MAX_WEIGHT_BITS_PER_BLOCK)
                    maxprec_1plane = i;
                if (bits_2planes >= MIN_WEIGHT_BITS_PER_BLOCK && bits_2planes <= MAX_WEIGHT_BITS_PER_BLOCK)
                    maxprec_2planes = i;
            }

            bsd->permit_encode[decimation_mode_count] = (x_weights <= xdim && y_weights <= ydim);

            bsd->decimation_mode_samples[decimation_mode_count] = weight_count;
            bsd->decimation_mode_maxprec_1plane[decimation_mode_count] = maxprec_1plane;
            bsd->decimation_mode_maxprec_2planes[decimation_mode_count] = maxprec_2planes;
            bsd->decimation_tables[decimation_mode_count] = dt;

            decimation_mode_count++;
        }

    for (i = 0; i < MAX_DECIMATION_MODES; i++) {
        bsd->decimation_mode_percentile[i] = 1.0f;
    }

    for (i = decimation_mode_count; i < MAX_DECIMATION_MODES; i++) {
        bsd->permit_encode[i] = 0;
        bsd->decimation_mode_samples[i] = 0;
        bsd->decimation_mode_maxprec_1plane[i] = -1;
        bsd->decimation_mode_maxprec_2planes[i] = -1;
    }

    bsd->decimation_mode_count = decimation_mode_count;

    const float *percentiles = ASTC_Encoder::get_2d_percentile_table_host(xdim, ydim);

    // then construct the list of block formats
    for (i = 0; i < 2048; i++) {
        int is_dual_plane;
        int quantization_mode;
        int fail = 0;
        int permit_encode = 1;

        if (ASTC_Encoder::decode_block_mode_2d(i, &x_weights, &y_weights, &is_dual_plane, &quantization_mode)) {
            if (x_weights > xdim || y_weights > ydim)
                permit_encode = 0;
        } else {
            fail = 1;
            permit_encode = 0;
        }

        if (fail) {
            bsd->block_modes[i].decimation_mode = -1;
            bsd->block_modes[i].quantization_mode = -1;
            bsd->block_modes[i].is_dual_plane = -1;
            bsd->block_modes[i].permit_encode = 0;
            bsd->block_modes[i].permit_decode = 0;
            bsd->block_modes[i].percentile = 1.0f;
        } else {
            int decimation_mode = decimation_mode_index[y_weights * 16 + x_weights];
            bsd->block_modes[i].decimation_mode = (uint8_t)decimation_mode;
            bsd->block_modes[i].quantization_mode = (uint8_t)quantization_mode;
            bsd->block_modes[i].is_dual_plane = (uint8_t)is_dual_plane;
            bsd->block_modes[i].permit_encode = (uint8_t)permit_encode;
            bsd->block_modes[i].permit_decode = (uint8_t)permit_encode;    // disallow decode of grid size larger than block size.
            bsd->block_modes[i].percentile = percentiles[i];

            if (bsd->decimation_mode_percentile[decimation_mode] > percentiles[i])
                bsd->decimation_mode_percentile[decimation_mode] = percentiles[i];
        }

    }

    if (xdim * ydim <= 64) {
        bsd->texelcount_for_bitmap_partitioning = xdim * ydim;
        for (i = 0; i < xdim * ydim; i++)
            bsd->texels_for_bitmap_partitioning[i] = i;
    }

    else {
        // pick 64 random texels for use with bitmap partitioning.
        int arr[MAX_TEXELS_PER_BLOCK];
        for (i = 0; i < xdim * ydim; i++)
            arr[i] = 0;
        int arr_elements_set = 0;
        while (arr_elements_set < 64) {
            int idx = rand() % (xdim * ydim);
            if (arr[idx] == 0) {
                arr_elements_set++;
                arr[idx] = 1;
            }
        }
        int texel_weights_written = 0;
        int idx = 0;
        while (texel_weights_written < 64) {
            if (arr[idx])
                bsd->texels_for_bitmap_partitioning[texel_weights_written++] = idx;
            idx++;
        }
        bsd->texelcount_for_bitmap_partitioning = 64;

    }
}

#ifdef ASTC_ENABLE_3D_SUPPORT
void construct_block_size_descriptor_3d(int xdim, int ydim, int zdim, block_size_descriptor * bsd) {
    int decimation_mode_index[512];    // for each of the 512 entries in the decim_table_array, its index
    int decimation_mode_count = 0;

    int i;
    int x_weights;
    int y_weights;
    int z_weights;

    for (i = 0; i < 512; i++) {
        decimation_mode_index[i] = -1;
    }

    // gather all the infill-modes that can be used with the current block size
    for (x_weights = 2; x_weights <= 6; x_weights++)
        for (y_weights = 2; y_weights <= 6; y_weights++)
            for (z_weights = 2; z_weights <= 6; z_weights++) {
                if ((x_weights * y_weights * z_weights) > MAX_WEIGHTS_PER_BLOCK)
                    continue;
                decimation_table *dt = new decimation_table;
                decimation_mode_index[z_weights * 64 + y_weights * 8 + x_weights] = decimation_mode_count;
                initialize_decimation_table_3d(xdim, ydim, zdim, x_weights, y_weights, z_weights, dt);

                int weight_count = x_weights * y_weights * z_weights;

                int maxprec_1plane = -1;
                int maxprec_2planes = -1;
                for (i = 0; i < 12; i++) {
                    int bits_1plane = compute_ise_bitcount(weight_count, (quantization_method)i);
                    int bits_2planes = compute_ise_bitcount(2 * weight_count, (quantization_method)i);
                    if (bits_1plane >= MIN_WEIGHT_BITS_PER_BLOCK && bits_1plane <= MAX_WEIGHT_BITS_PER_BLOCK)
                        maxprec_1plane = i;
                    if (bits_2planes >= MIN_WEIGHT_BITS_PER_BLOCK && bits_2planes <= MAX_WEIGHT_BITS_PER_BLOCK)
                        maxprec_2planes = i;
                }
                bsd->permit_encode[decimation_mode_count] = (x_weights <= xdim && y_weights <= ydim && z_weights <= zdim);

                bsd->decimation_mode_samples[decimation_mode_count] = weight_count;
                bsd->decimation_mode_maxprec_1plane[decimation_mode_count] = maxprec_1plane;
                bsd->decimation_mode_maxprec_2planes[decimation_mode_count] = maxprec_2planes;
                bsd->decimation_tables[decimation_mode_count] = *dt;    // NP code change!

                decimation_mode_count++;
            }

    for (i = 0; i < MAX_DECIMATION_MODES; i++) {
        bsd->decimation_mode_percentile[i] = 1.0f;
    }

    for (i = decimation_mode_count; i < MAX_DECIMATION_MODES; i++) {
        bsd->permit_encode[i] = 0;
        bsd->decimation_mode_samples[i] = 0;
        bsd->decimation_mode_maxprec_1plane[i] = -1;
        bsd->decimation_mode_maxprec_2planes[i] = -1;
    }

    bsd->decimation_mode_count = decimation_mode_count;

    const float *percentiles = get_3d_percentile_table(xdim, ydim, zdim);

    // then construct the list of block formats
    for (i = 0; i < 2048; i++) {
        int is_dual_plane;
        int quantization_mode;
        int fail = 0;
        int permit_encode = 1;

        if (decode_block_mode_3d(i, &x_weights, &y_weights, &z_weights, &is_dual_plane, &quantization_mode)) {
            if (x_weights > xdim || y_weights > ydim || z_weights > zdim)
                permit_encode = 0;
        } else

        {
            fail = 1;
            permit_encode = 0;
        }
        if (fail) {
            bsd->block_modes[i].decimation_mode = -1;
            bsd->block_modes[i].quantization_mode = -1;
            bsd->block_modes[i].is_dual_plane = -1;
            bsd->block_modes[i].permit_encode = 0;
            bsd->block_modes[i].permit_decode = 0;
            bsd->block_modes[i].percentile = 1.0f;
        } else {
            int decimation_mode = decimation_mode_index[z_weights * 64 + y_weights * 8 + x_weights];
            bsd->block_modes[i].decimation_mode = (uint8_t)decimation_mode;
            bsd->block_modes[i].quantization_mode = (uint8_t)quantization_mode;
            bsd->block_modes[i].is_dual_plane = (uint8_t)is_dual_plane;
            bsd->block_modes[i].permit_encode = (uint8_t)permit_encode;
            bsd->block_modes[i].permit_decode = (uint8_t)permit_encode;
            bsd->block_modes[i].percentile = percentiles[i];

            if (bsd->decimation_mode_percentile[decimation_mode] > percentiles[i])
                bsd->decimation_mode_percentile[decimation_mode] = percentiles[i];
        }

    }

    if (xdim * ydim * zdim <= 64) {
        bsd->texelcount_for_bitmap_partitioning = xdim * ydim * zdim;
        for (i = 0; i < xdim * ydim * zdim; i++)
            bsd->texels_for_bitmap_partitioning[i] = i;
    }

    else {
        // pick 64 random texels for use with bitmap partitioning.
        int arr[MAX_TEXELS_PER_BLOCK];
        for (i = 0; i < xdim * ydim * zdim; i++)
            arr[i] = 0;
        int arr_elements_set = 0;
        while (arr_elements_set < 64) {
            int idx = rand() % (xdim * ydim * zdim);
            if (arr[idx] == 0) {
                arr_elements_set++;
                arr[idx] = 1;
            }
        }
        int texel_weights_written = 0;
        int idx = 0;
        while (texel_weights_written < 64) {
            if (arr[idx])
                bsd->texels_for_bitmap_partitioning[texel_weights_written++] = idx;
            idx++;
        }
        bsd->texelcount_for_bitmap_partitioning = 64;
    }
}
#endif

static block_size_descriptor_cpu *bsd_pointers[4096];

// function to obtain a block size descriptor. If the descriptor does not exist,
// it is created as needed. Should not be called from within multithreaded code.
block_size_descriptor_cpu *get_block_size_descriptor_cpu(int xdim, int ydim, int zdim) {
    int bsd_index = xdim + (ydim << 4) + (zdim << 8);
    if (bsd_pointers[bsd_index] == NULL) {
        block_size_descriptor_cpu *bsd = new block_size_descriptor_cpu;
#ifdef ASTC_ENABLE_3D_SUPPORT
        if (zdim > 1)
            construct_block_size_descriptor_3d(xdim, ydim, zdim, bsd);
        else
#endif
            construct_block_size_descriptor_2d_cpu(xdim, ydim, bsd);

        bsd_pointers[bsd_index] = bsd;
    }
    return bsd_pointers[bsd_index];
}

void physical_to_symbolic_cpu(int xdim, int ydim, int zdim, physical_compressed_block_cpu pb, symbolic_compressed_block_cpu * res) {
    uint8_t bswapped[16];
    int i, j;

    res->error_block = 0;

    // get hold of the block-size descriptor and the decimation tables.
    const block_size_descriptor_cpu *bsd = get_block_size_descriptor_cpu(xdim, ydim, zdim);
    const decimation_table_cpu *const *ixtab2 = bsd->decimation_tables;

    // extract header fields
    int block_mode = ASTC_Encoder::read_bits(11, 0, pb.data);


    if ((block_mode & 0x1FF) == 0x1FC) {
        // void-extent block!

        // check what format the data has
        if (block_mode & 0x200)
            res->block_mode = -1;    // floating-point
        else
            res->block_mode = -2;    // unorm16.

        res->partition_count = 0;
        for (i = 0; i < 4; i++) {
            res->constant_color[i] = pb.data[2 * i + 8] | (pb.data[2 * i + 9] << 8);
        }

        // additionally, check that the void-extent
        if (zdim == 1) {
            // 2D void-extent
            int rsvbits = ASTC_Encoder::read_bits(2, 10, pb.data);
            if (rsvbits != 3)
                res->error_block = 1;

            int vx_low_s = ASTC_Encoder::read_bits(8, 12, pb.data) | (ASTC_Encoder::read_bits(5, 12 + 8, pb.data) << 8);
            int vx_high_s = ASTC_Encoder::read_bits(8, 25, pb.data) | (ASTC_Encoder::read_bits(5, 25 + 8, pb.data) << 8);
            int vx_low_t = ASTC_Encoder::read_bits(8, 38, pb.data) | (ASTC_Encoder::read_bits(5, 38 + 8, pb.data) << 8);
            int vx_high_t = ASTC_Encoder::read_bits(8, 51, pb.data) | (ASTC_Encoder::read_bits(5, 51 + 8, pb.data) << 8);

            int all_ones = vx_low_s == 0x1FFF && vx_high_s == 0x1FFF && vx_low_t == 0x1FFF && vx_high_t == 0x1FFF;

            if ((vx_low_s >= vx_high_s || vx_low_t >= vx_high_t) && !all_ones)
                res->error_block = 1;
        } else {
            // 3D void-extent
            int vx_low_s = ASTC_Encoder::read_bits(9, 10, pb.data);
            int vx_high_s = ASTC_Encoder::read_bits(9, 19, pb.data);
            int vx_low_t = ASTC_Encoder::read_bits(9, 28, pb.data);
            int vx_high_t = ASTC_Encoder::read_bits(9, 37, pb.data);
            int vx_low_p = ASTC_Encoder::read_bits(9, 46, pb.data);
            int vx_high_p = ASTC_Encoder::read_bits(9, 55, pb.data);

            int all_ones = vx_low_s == 0x1FF && vx_high_s == 0x1FF && vx_low_t == 0x1FF && vx_high_t == 0x1FF && vx_low_p == 0x1FF && vx_high_p == 0x1FF;

            if ((vx_low_s >= vx_high_s || vx_low_t >= vx_high_t || vx_low_p >= vx_high_p) && !all_ones)
                res->error_block = 1;
        }

        return;
    }

    if (bsd->block_modes[block_mode].permit_decode == 0) {
        res->error_block = 1;
        return;
    }

    int weight_count = ixtab2[bsd->block_modes[block_mode].decimation_mode]->num_weights;
    int weight_quantization_method = bsd->block_modes[block_mode].quantization_mode;
    int is_dual_plane = bsd->block_modes[block_mode].is_dual_plane;

    int real_weight_count = is_dual_plane ? 2 * weight_count : weight_count;

    int partition_count = ASTC_Encoder::read_bits(2, 11, pb.data) + 1;

    res->block_mode = block_mode;
    res->partition_count = partition_count;

    for (i = 0; i < 16; i++)
        bswapped[i] = (uint8_t)ASTC_Encoder::bitrev8(pb.data[15 - i]);

    int bits_for_weights = ASTC_Encoder::compute_ise_bitcount2(real_weight_count,(ASTC_Encoder::quantization_method)weight_quantization_method);

    int below_weights_pos = 128 - bits_for_weights;

    if (is_dual_plane) {
        uint8_t indices[64];
        ASTC_Encoder::decode_ise(weight_quantization_method, real_weight_count, bswapped, indices, 0);
        for (i = 0; i < weight_count; i++) {
            res->plane1_weights[i] = indices[2 * i];
            res->plane2_weights[i] = indices[2 * i + 1];
        }
    } else {
        ASTC_Encoder::decode_ise(weight_quantization_method, weight_count, bswapped, res->plane1_weights, 0);
    }

    if (is_dual_plane && partition_count == 4)
        res->error_block = 1;



    res->color_formats_matched = 0;

    // then, determine the format of each endpoint pair
    int color_formats[4];
    int encoded_type_highpart_size = 0;
    if (partition_count == 1) {
        color_formats[0] = ASTC_Encoder::read_bits(4, 13, pb.data);
        res->partition_index = 0;
    } else {
        encoded_type_highpart_size = (3 * partition_count) - 4;
        below_weights_pos -= encoded_type_highpart_size;
        int encoded_type = ASTC_Encoder::read_bits(6, 13 + PARTITION_BITS, pb.data) | (ASTC_Encoder::read_bits(encoded_type_highpart_size, below_weights_pos, pb.data) << 6);
        int baseclass = encoded_type & 0x3;
        if (baseclass == 0) {
            for (i = 0; i < partition_count; i++) {
                color_formats[i] = (encoded_type >> 2) & 0xF;
            }
            below_weights_pos += encoded_type_highpart_size;
            res->color_formats_matched = 1;
            encoded_type_highpart_size = 0;
        } else {
            int bitpos = 2;
            baseclass--;
            for (i = 0; i < partition_count; i++) {
                color_formats[i] = (((encoded_type >> bitpos) & 1) + baseclass) << 2;
                bitpos++;
            }
            for (i = 0; i < partition_count; i++) {
                color_formats[i] |= (encoded_type >> bitpos) & 3;
                bitpos += 2;
            }
        }
        res->partition_index = ASTC_Encoder::read_bits(6, 13, pb.data) | (ASTC_Encoder::read_bits(PARTITION_BITS - 6, 19, pb.data) << 6);

    }
    for (i = 0; i < partition_count; i++)
        res->color_formats[i] = color_formats[i];


    // then, determine the number of integers we need to unpack for the endpoint pairs
    int color_integer_count = 0;
    for (i = 0; i < partition_count; i++) {
        int endpoint_class = color_formats[i] >> 2;
        color_integer_count += (endpoint_class + 1) * 2;
    }

    if (color_integer_count > 18)
        res->error_block = 1;

    // then, determine the color endpoint format to use for these integers
    static const int color_bits_arr[5] = { -1, 115 - 4, 113 - 4 - PARTITION_BITS, 113 - 4 - PARTITION_BITS, 113 - 4 - PARTITION_BITS };
    int color_bits = color_bits_arr[partition_count] - bits_for_weights - encoded_type_highpart_size;
    if (is_dual_plane)
        color_bits -= 2;
    if (color_bits < 0)
        color_bits = 0;

    int color_quantization_level = g_ASTCEncode.quantization_mode_table[color_integer_count >> 1][color_bits];
    res->color_quantization_level = color_quantization_level;
    if (color_quantization_level < 4)
        res->error_block = 1;


    // then unpack the integer-bits
    uint8_t values_to_decode[32];
    ASTC_Encoder::decode_ise(color_quantization_level, color_integer_count, pb.data, values_to_decode, (partition_count == 1 ? 17 : 19 + PARTITION_BITS));

    // and distribute them over the endpoint types
    int valuecount_to_decode = 0;

    for (i = 0; i < partition_count; i++) {
        int vals = 2 * (color_formats[i] >> 2) + 2;
        for (j = 0; j < vals; j++)
            res->color_values[i][j] = values_to_decode[j + valuecount_to_decode];
        valuecount_to_decode += vals;
    }

    // get hold of color component for second-plane in the case of dual plane of weightss.
    if (is_dual_plane)
        res->plane2_color_component = ASTC_Encoder::read_bits(2, below_weights_pos - 2, pb.data);

}

void imageblock_initialize_deriv_from_work_and_orig_cpu(imageblock_cpu * pb, int pixelcount) {
    int i;

    const float *fptr = pb->orig_data;
    const float *wptr = pb->work_data;
    float *dptr = pb->deriv_data;

    for (i = 0; i < pixelcount; i++) {

        // compute derivatives for RGB first
        if (pb->rgb_lns[i]) {
            float r = MAX(fptr[0], 6e-5f);
            float g = MAX(fptr[1], 6e-5f);
            float b = MAX(fptr[2], 6e-5f);

            float rderiv = (ASTC_Encoder::float_to_lns(r * 1.05f) - ASTC_Encoder::float_to_lns(r)) / (r * 0.05f);
            float gderiv = (ASTC_Encoder::float_to_lns(g * 1.05f) - ASTC_Encoder::float_to_lns(g)) / (g * 0.05f);
            float bderiv = (ASTC_Encoder::float_to_lns(b * 1.05f) - ASTC_Encoder::float_to_lns(b)) / (b * 0.05f);

            // the derivative may not actually take values smaller than 1/32 or larger than 2^25;
            // if it does, we clamp it.
            if (rderiv < (1.0f / 32.0f))
                rderiv = (1.0f / 32.0f);
            else if (rderiv > 33554432.0f)
                rderiv = 33554432.0f;

            if (gderiv < (1.0f / 32.0f))
                gderiv = (1.0f / 32.0f);
            else if (gderiv > 33554432.0f)
                gderiv = 33554432.0f;

            if (bderiv < (1.0f / 32.0f))
                bderiv = (1.0f / 32.0f);
            else if (bderiv > 33554432.0f)
                bderiv = 33554432.0f;

            dptr[0] = rderiv;
            dptr[1] = gderiv;
            dptr[2] = bderiv;
        } else {
            dptr[0] = 65535.0f;
            dptr[1] = 65535.0f;
            dptr[2] = 65535.0f;
        }


        // then compute derivatives for Alpha
        if (pb->alpha_lns[i]) {
            float a = MAX(fptr[3], 6e-5f);
            float aderiv = (ASTC_Encoder::float_to_lns(a * 1.05f) - ASTC_Encoder::float_to_lns(a)) / (a * 0.05f);
            // the derivative may not actually take values smaller than 1/32 or larger than 2^25;
            // if it does, we clamp it.
            if (aderiv < (1.0f / 32.0f))
                aderiv = (1.0f / 32.0f);
            else if (aderiv > 33554432.0f)
                aderiv = 33554432.0f;

            dptr[3] = aderiv;
        } else {
            dptr[3] = 65535.0f;
        }

        fptr += 4;
        wptr += 4;
        dptr += 4;
    }
}

// helper function to initialize the work-data from the orig-data
void imageblock_initialize_work_from_orig_cpu(imageblock_cpu * pb, int pixelcount) {
    int i;
    float *fptr = pb->orig_data;
    float *wptr = pb->work_data;

    for (i = 0; i < pixelcount; i++) {
        if (pb->rgb_lns[i]) {
            wptr[0] = ASTC_Encoder::float_to_lns(fptr[0]);
            wptr[1] = ASTC_Encoder::float_to_lns(fptr[1]);
            wptr[2] = ASTC_Encoder::float_to_lns(fptr[2]);
        } else {
            wptr[0] = fptr[0] * 65535.0f;
            wptr[1] = fptr[1] * 65535.0f;
            wptr[2] = fptr[2] * 65535.0f;
        }

        if (pb->alpha_lns[i]) {
            wptr[3] = ASTC_Encoder::float_to_lns(fptr[3]);
        } else {
            wptr[3] = fptr[3] * 65535.0f;
        }
        fptr += 4;
        wptr += 4;
    }

    imageblock_initialize_deriv_from_work_and_orig_cpu(pb, pixelcount);
}

void update_imageblock_flags_cpu(imageblock_cpu * pb, int xdim, int ydim, int zdim) {
    int i;
    float red_min = 1e38f, red_max = -1e38f;
    float green_min = 1e38f, green_max = -1e38f;
    float blue_min = 1e38f, blue_max = -1e38f;
    float alpha_min = 1e38f, alpha_max = -1e38f;

    int texels_per_block = xdim * ydim * zdim;

    int grayscale = 1;

    for (i = 0; i < texels_per_block; i++) {
        float red = pb->work_data[4 * i];
        float green = pb->work_data[4 * i + 1];
        float blue = pb->work_data[4 * i + 2];
        float alpha = pb->work_data[4 * i + 3];
        if (red < red_min)
            red_min = red;
        if (red > red_max)
            red_max = red;
        if (green < green_min)
            green_min = green;
        if (green > green_max)
            green_max = green;
        if (blue < blue_min)
            blue_min = blue;
        if (blue > blue_max)
            blue_max = blue;
        if (alpha < alpha_min)
            alpha_min = alpha;
        if (alpha > alpha_max)
            alpha_max = alpha;

        if (grayscale == 1 && (red != green || red != blue))
            grayscale = 0;
    }

    pb->red_min = red_min;
    pb->red_max = red_max;
    pb->green_min = green_min;
    pb->green_max = green_max;
    pb->blue_min = blue_min;
    pb->blue_max = blue_max;
    pb->alpha_min = alpha_min;
    pb->alpha_max = alpha_max;
    pb->grayscale = grayscale;
}

// fetch an imageblock from the input file.
void fetch_imageblock_cpu(
    const astc_codec_image_cpu * img,
    imageblock_cpu * pb,    // picture-block to imitialize with image data
    // block dimensions
    int xdim, int ydim, int zdim,
    // position in texture.
    int xpos, int ypos, int zpos
) {
    float *fptr = pb->orig_data;
    int xsize = img->xsize + 2 * img->padding;
    int ysize = img->ysize + 2 * img->padding;
    int zsize = (img->zsize == 1) ? 1 : img->zsize + 2 * img->padding;

    int x, y, z, i;

    pb->xpos = xpos;
    pb->ypos = ypos;
    pb->zpos = zpos;

    xpos += img->padding;
    ypos += img->padding;
    if (img->zsize > 1)
        zpos += img->padding;

    float data[6];
    data[4] = 0;
    data[5] = 1;

    if (img->imagedata8) {
        for (z = 0; z < zdim; z++)
            for (y = 0; y < ydim; y++)
                for (x = 0; x < xdim; x++) {
                    int xi = xpos + x;
                    int yi = ypos + y;
                    int zi = zpos + z;
                    // clamp XY coordinates to the picture.
                    if (xi < 0)
                        xi = 0;
                    if (yi < 0)
                        yi = 0;
                    if (zi < 0)
                        zi = 0;
                    if (xi >= xsize)
                        xi = xsize - 1;
                    if (yi >= ysize)
                        yi = ysize - 1;
                    if (zi >= zsize)
                        zi = zsize - 1;

                    int r = img->imagedata8[zi][yi][4 * xi];
                    int g = img->imagedata8[zi][yi][4 * xi + 1];
                    int b = img->imagedata8[zi][yi][4 * xi + 2];
                    int a = img->imagedata8[zi][yi][4 * xi + 3];

                    data[0] = r / 255.0f;
                    data[1] = g / 255.0f;
                    data[2] = b / 255.0f;
                    data[3] = a / 255.0f;

                    fptr[0] = data[0];
                    fptr[1] = data[1];
                    fptr[2] = data[2];
                    fptr[3] = data[3];
                    fptr += 4;
                }
    }

    //------------------------------------------
    // HDR currently not supported in code
    /*
    else if (img->imagedata16)
    {
    for (z = 0; z < zdim; z++)
    for (y = 0; y < ydim; y++)
    for (x = 0; x < xdim; x++)
    {
    int xi = xpos + x;
    int yi = ypos + y;
    int zi = zpos + z;
    // clamp XY coordinates to the picture.
    if (xi < 0)
    xi = 0;
    if (yi < 0)
    yi = 0;
    if (zi < 0)
    zi = 0;
    if (xi >= xsize)
    xi = xsize - 1;
    if (yi >= ysize)
    yi = ysize - 1;
    if (zi >= ysize)
    zi = zsize - 1;

    int r = img->imagedata16[zi][yi][4 * xi];
    int g = img->imagedata16[zi][yi][4 * xi + 1];
    int b = img->imagedata16[zi][yi][4 * xi + 2];
    int a = img->imagedata16[zi][yi][4 * xi + 3];

    float rf = sf16_to_float((sf16)r);
    float gf = sf16_to_float((sf16)g);
    float bf = sf16_to_float((sf16)b);
    float af = sf16_to_float((sf16)a);

    // equalize the color components somewhat, and get rid of negative values.

    rf = MAX(rf, 1e-8f);
    gf = MAX(gf, 1e-8f);
    bf = MAX(bf, 1e-8f);
    af = MAX(af, 1e-8f);

    data[0] = rf;
    data[1] = gf;
    data[2] = bf;
    data[3] = af;

    fptr[0] = data[0];
    fptr[1] = data[1];
    fptr[2] = data[2];
    fptr[3] = data[3];
    fptr += 4;
    }
    }
    */

    int pixelcount = xdim * ydim * zdim;

    // impose the choice on every pixel when encoding.
    for (i = 0; i < pixelcount; i++) {
        pb->rgb_lns[i]      = (uint8_t)g_ASTCEncode.m_rgb_force_use_of_hdr;
        pb->alpha_lns[i]    = (uint8_t)g_ASTCEncode.m_alpha_force_use_of_hdr;
        pb->nan_texel[i]    = 0;
    }

    imageblock_initialize_work_from_orig_cpu(pb, pixelcount);
    update_imageblock_flags_cpu(pb, xdim, ydim, zdim);
}

void destroy_image_cpu(astc_codec_image_cpu * img) {
    if (img == NULL)
        return;

    if (img->imagedata8) {
        delete[]img->imagedata8[0][0];
        delete[]img->imagedata8[0];
        delete[]img->imagedata8;
    }
    if (img->imagedata16) {
        delete[]img->imagedata16[0][0];
        delete[]img->imagedata16[0];
        delete[]img->imagedata16;
    }
    delete img;
}

astc_codec_image_cpu *allocate_image_cpu(int bitness, int xsize, int ysize, int zsize, int padding) {
    int i, j;
    astc_codec_image_cpu *img = new astc_codec_image_cpu;
    img->xsize = xsize;
    img->ysize = ysize;
    img->zsize = zsize;
    img->padding = padding;

    int exsize = xsize + 2 * padding;
    int eysize = ysize + 2 * padding;
    int ezsize = (zsize == 1) ? 1 : zsize + 2 * padding;

    if (bitness == 8) {
        img->imagedata8 = new uint8_t **[ezsize];
        img->imagedata8[0] = new uint8_t *[ezsize * eysize];
        img->imagedata8[0][0] = new uint8_t[4 * ezsize * eysize * exsize];
        for (i = 1; i < ezsize; i++) {
            img->imagedata8[i] = img->imagedata8[0] + i * eysize;
            img->imagedata8[i][0] = img->imagedata8[0][0] + 4 * i * exsize * eysize;
        }
        for (i = 0; i < ezsize; i++)
            for (j = 1; j < eysize; j++)
                img->imagedata8[i][j] = img->imagedata8[i][0] + 4 * j * exsize;

        img->imagedata16 = NULL;
    }

    else if (bitness == 16) {
        img->imagedata16 = new uint16_t **[ezsize];
        img->imagedata16[0] = new uint16_t *[ezsize * eysize];
        img->imagedata16[0][0] = new uint16_t[4 * ezsize * eysize * exsize];
        for (i = 1; i < ezsize; i++) {
            img->imagedata16[i] = img->imagedata16[0] + i * eysize;
            img->imagedata16[i][0] = img->imagedata16[0][0] + 4 * i * exsize * eysize;
        }
        for (i = 0; i < ezsize; i++)
            for (j = 1; j < eysize; j++)
                img->imagedata16[i][j] = img->imagedata16[i][0] + 4 * j * exsize;

        img->imagedata8 = NULL;
    } else {
        return nullptr;
    }

    return img;
}

void initialize_image_cpu(astc_codec_image_cpu * img) {
    int x, y, z;

    int exsize = img->xsize + 2 * img->padding;
    int eysize = img->ysize + 2 * img->padding;
    int ezsize = (img->zsize == 1) ? 1 : img->zsize + 2 * img->padding;

    if (img->imagedata8) {
        for (z = 0; z < ezsize; z++)
            for (y = 0; y < eysize; y++)
                for (x = 0; x < exsize; x++) {
                    img->imagedata8[z][y][4 * x] = 0;
                    img->imagedata8[z][y][4 * x + 1] = 0;
                    img->imagedata8[z][y][4 * x + 2] = 0;
                    img->imagedata8[z][y][4 * x + 3] = 0xFF;
                }
    } else if (img->imagedata16) {
        for (z = 0; z < ezsize; z++)
            for (y = 0; y < eysize; y++)
                for (x = 0; x < exsize; x++) {
                    img->imagedata16[z][y][4 * x] = 0;
                    img->imagedata16[z][y][4 * x + 1] = 0;
                    img->imagedata16[z][y][4 * x + 2] = 0;
                    img->imagedata16[z][y][4 * x + 3] = 0x3C00;
                }
    }
}


void write_imageblock_cpu(astc_codec_image_cpu * img, const imageblock_cpu * pb,
                          int xdim, int ydim, int zdim,
                          int xpos, int ypos, int zpos, swizzlepattern_cpu swz) {
    const float *fptr = pb->orig_data;
    const uint8_t *nptr = pb->nan_texel;
    int xsize = img->xsize;
    int ysize = img->ysize;
    int zsize = img->zsize;
    int x, y, z;


    float data[7];
    data[4] = 0.0f;
    data[5] = 1.0f;


    if (img->imagedata8) {
        for (z = 0; z < zdim; z++)
            for (y = 0; y < ydim; y++)
                for (x = 0; x < xdim; x++) {
                    int xi = xpos + x;
                    int yi = ypos + y;
                    int zi = zpos + z;

                    if (xi >= 0 && yi >= 0 && zi >= 0 && xi < xsize && yi < ysize && zi < zsize) {
                        if (*nptr) {
                            // NaN-pixel, but we can't display it. Display purple instead.
                            img->imagedata8[zi][yi][4 * xi] = 0xFF;
                            img->imagedata8[zi][yi][4 * xi + 1] = 0x00;
                            img->imagedata8[zi][yi][4 * xi + 2] = 0xFF;
                            img->imagedata8[zi][yi][4 * xi + 3] = 0xFF;
                        }

                        else {
#ifdef USE_PERFORMM_SRGB_TRANSFORM
                            // apply swizzle
                            if (g_ASTCEncode.m_perform_srgb_transform) {
                                float r = fptr[0];
                                float g = fptr[1];
                                float b = fptr[2];

                                if (r <= 0.0031308f)
                                    r = r * 12.92f;
                                else if (r <= 1)
                                    r = (float)(1.055f * pow(r, (1.0f / 2.4f)) - 0.055f);

                                if (g <= 0.0031308f)
                                    g = g * 12.92f;
                                else if (g <= 1)
                                    g = (float)(1.055f * pow(g, (1.0f / 2.4f)) - 0.055f);

                                if (b <= 0.0031308f)
                                    b = b * 12.92f;
                                else if (b <= 1)
                                    b = (float)(1.055f * pow(b, (1.0f / 2.4f)) - 0.055f);

                                data[0] = r;
                                data[1] = g;
                                data[2] = b;
                            } else
#endif
                            {

                                float r = fptr[0];
                                float g = fptr[1];
                                float b = fptr[2];

                                data[0] = r;
                                data[1] = g;
                                data[2] = b;
                            }
                            data[3] = fptr[3];




                            float xcoord = (data[0] * 2.0f) - 1.0f;
                            float ycoord = (data[3] * 2.0f) - 1.0f;
                            float zcoord = 1.0f - xcoord * xcoord - ycoord * ycoord;
                            if (zcoord < 0.0f)
                                zcoord = 0.0f;
                            data[6] = float((sqrt(zcoord) * 0.5f) + 0.5f);

                            // clamp to [0,1]
                            if (data[0] > 1.0f)
                                data[0] = 1.0f;
                            if (data[1] > 1.0f)
                                data[1] = 1.0f;
                            if (data[2] > 1.0f)
                                data[2] = 1.0f;
                            if (data[3] > 1.0f)
                                data[3] = 1.0f;


                            // pack the data
                            int ri = static_cast < int >(floor(data[swz.r] * 255.0f + 0.5f));
                            int gi = static_cast < int >(floor(data[swz.g] * 255.0f + 0.5f));
                            int bi = static_cast < int >(floor(data[swz.b] * 255.0f + 0.5f));
                            int ai = static_cast < int >(floor(data[swz.a] * 255.0f + 0.5f));

                            img->imagedata8[zi][yi][4 * xi] = (uint8_t)ri;
                            img->imagedata8[zi][yi][4 * xi + 1] = (uint8_t)gi;
                            img->imagedata8[zi][yi][4 * xi + 2] = (uint8_t)bi;
                            img->imagedata8[zi][yi][4 * xi + 3] = (uint8_t)ai;
                        }
                    }
                    fptr += 4;
                    nptr++;
                }
    } else if (img->imagedata16) {
        for (z = 0; z < zdim; z++)
            for (y = 0; y < ydim; y++)
                for (x = 0; x < xdim; x++) {
                    int xi = xpos + x;
                    int yi = ypos + y;
                    int zi = zpos + z;

                    if (xi >= 0 && yi >= 0 && zi >= 0 && xi < xsize && yi < ysize && zi < zsize) {
                        if (*nptr) {
                            img->imagedata16[zi][yi][4 * xi] = 0xFFFF;
                            img->imagedata16[zi][yi][4 * xi + 1] = 0xFFFF;
                            img->imagedata16[zi][yi][4 * xi + 2] = 0xFFFF;
                            img->imagedata16[zi][yi][4 * xi + 3] = 0xFFFF;
                        }

                        else {
#ifdef USE_PERFORMM_SRGB_TRANSFORM
                            // apply swizzle
                            if (g_ASTCEncode.m_perform_srgb_transform) {
                                float r = fptr[0];
                                float g = fptr[1];
                                float b = fptr[2];

                                if (r <= 0.0031308f)
                                    r = r * 12.92f;
                                else if (r <= 1)
                                    r = (float)(1.055f * pow(r, (1.0f / 2.4f)) - 0.055f);
                                if (g <= 0.0031308f)
                                    g = g * 12.92f;
                                else if (g <= 1)
                                    g = (float)(1.055f * pow(g, (1.0f / 2.4f)) - 0.055f);
                                if (b <= 0.0031308f)
                                    b = b * 12.92f;
                                else if (b <= 1)
                                    b = (float)(1.055f * pow(b, (1.0f / 2.4f)) - 0.055f);

                                data[0] = r;
                                data[1] = g;
                                data[2] = b;
                            } else
#endif
                            {
                                data[0] = fptr[0];
                                data[1] = fptr[1];
                                data[2] = fptr[2];
                            }
                            data[3] = fptr[3];

                            float x1 = (data[0] * 2.0f) - 1.0f;
                            float y1 = (data[3] * 2.0f) - 1.0f;
                            float z1 = 1.0f - x1 * x1 - y1 * y1;
                            if (z1 < 0.0f)
                                z1 = 0.0f;
                            data[6] = (float)((sqrt(z1) * 0.5f) + 0.5f);


                            int r = ASTC_Encoder::float_to_sf16(data[swz.r],  ASTC_Encoder::SF_NEARESTEVEN);
                            int g = ASTC_Encoder::float_to_sf16(data[swz.g],  ASTC_Encoder::SF_NEARESTEVEN);
                            int b = ASTC_Encoder::float_to_sf16(data[swz.b],  ASTC_Encoder::SF_NEARESTEVEN);
                            int a = ASTC_Encoder::float_to_sf16(data[swz.a],  ASTC_Encoder::SF_NEARESTEVEN);
                            img->imagedata16[zi][yi][4 * xi] = (uint16_t)r;
                            img->imagedata16[zi][yi][4 * xi + 1] = (uint16_t)g;
                            img->imagedata16[zi][yi][4 * xi + 2] = (uint16_t)b;
                            img->imagedata16[zi][yi][4 * xi + 3] = (uint16_t)a;
                        }
                    }
                    fptr += 4;
                    nptr++;
                }
    }
}

uint32_t hash52_cpu(uint32_t inp) {
    inp ^= inp >> 15;

    inp *= 0xEEDE0891;            // (2^4+1)*(2^7+1)*(2^17-1)
    inp ^= inp >> 5;
    inp += inp << 16;
    inp ^= inp >> 7;
    inp ^= inp >> 3;
    inp ^= inp << 6;
    inp ^= inp >> 17;
    return inp;
}

int select_partition_cpu(int seed, int x, int y, int z, int partitioncount, int small_block) {
    if (small_block) {
        x <<= 1;
        y <<= 1;
        z <<= 1;
    }

    seed += (partitioncount - 1) * 1024;

    uint32_t rnum = hash52_cpu(seed);

    uint8_t seed1 = rnum & 0xF;
    uint8_t seed2 = (rnum >> 4) & 0xF;
    uint8_t seed3 = (rnum >> 8) & 0xF;
    uint8_t seed4 = (rnum >> 12) & 0xF;
    uint8_t seed5 = (rnum >> 16) & 0xF;
    uint8_t seed6 = (rnum >> 20) & 0xF;
    uint8_t seed7 = (rnum >> 24) & 0xF;
    uint8_t seed8 = (rnum >> 28) & 0xF;
    uint8_t seed9 = (rnum >> 18) & 0xF;
    uint8_t seed10 = (rnum >> 22) & 0xF;
    uint8_t seed11 = (rnum >> 26) & 0xF;
    uint8_t seed12 = ((rnum >> 30) | (rnum << 2)) & 0xF;

    // squaring all the seeds in order to bias their distribution
    // towards lower values.
    seed1 *= seed1;
    seed2 *= seed2;
    seed3 *= seed3;
    seed4 *= seed4;
    seed5 *= seed5;
    seed6 *= seed6;
    seed7 *= seed7;
    seed8 *= seed8;
    seed9 *= seed9;
    seed10 *= seed10;
    seed11 *= seed11;
    seed12 *= seed12;


    int sh1, sh2, sh3;
    if (seed & 1) {
        sh1 = (seed & 2 ? 4 : 5);
        sh2 = (partitioncount == 3 ? 6 : 5);
    } else {
        sh1 = (partitioncount == 3 ? 6 : 5);
        sh2 = (seed & 2 ? 4 : 5);
    }
    sh3 = (seed & 0x10) ? sh1 : sh2;

    seed1 >>= sh1;
    seed2 >>= sh2;
    seed3 >>= sh1;
    seed4 >>= sh2;
    seed5 >>= sh1;
    seed6 >>= sh2;
    seed7 >>= sh1;
    seed8 >>= sh2;

    seed9 >>= sh3;
    seed10 >>= sh3;
    seed11 >>= sh3;
    seed12 >>= sh3;



    int a = seed1 * x + seed2 * y + seed11 * z + (rnum >> 14);
    int b = seed3 * x + seed4 * y + seed12 * z + (rnum >> 10);
    int c = seed5 * x + seed6 * y + seed9 * z + (rnum >> 6);
    int d = seed7 * x + seed8 * y + seed10 * z + (rnum >> 2);


    // apply the saw
    a &= 0x3F;
    b &= 0x3F;
    c &= 0x3F;
    d &= 0x3F;

    // remove some of the components of we are to output < 4 partitions.
    if (partitioncount <= 3)
        d = 0;
    if (partitioncount <= 2)
        c = 0;
    if (partitioncount <= 1)
        b = 0;

    int partition;
    if (a >= b && a >= c && a >= d)
        partition = 0;
    else if (b >= c && b >= d)
        partition = 1;
    else if (c >= d)
        partition = 2;
    else
        partition = 3;
    return partition;
}

void generate_one_partition_table_cpu(int xdim, int ydim, int zdim, int partition_count, int partition_index, partition_info_cpu * pt) {
    int small_block = (xdim * ydim * zdim) < 32;

    uint8_t *partition_of_texel = pt->partition_of_texel;
    int x, y, z, i;


    for (z = 0; z < zdim; z++)
        for (y = 0; y < ydim; y++)
            for (x = 0; x < xdim; x++) {
                uint8_t part = (uint8_t)select_partition_cpu(partition_index, x, y, z, partition_count, small_block);
                *partition_of_texel++ = part;
            }


    int texels_per_block = xdim * ydim * zdim;

    int counts[4];
    for (i = 0; i < 4; i++)
        counts[i] = 0;

    for (i = 0; i < texels_per_block; i++) {
        int partition = pt->partition_of_texel[i];
        pt->texels_of_partition[partition][counts[partition]++] = (uint8_t)i;
    }

    for (i = 0; i < 4; i++)
        pt->texels_per_partition[i] = (uint8_t)counts[i];

    if (counts[0] == 0)
        pt->partition_count = 0;
    else if (counts[1] == 0)
        pt->partition_count = 1;
    else if (counts[2] == 0)
        pt->partition_count = 2;
    else if (counts[3] == 0)
        pt->partition_count = 3;
    else
        pt->partition_count = 4;



    for (i = 0; i < 4; i++)
        pt->coverage_bitmaps[i] = 0ULL;

    const block_size_descriptor_cpu *bsd = get_block_size_descriptor_cpu(xdim, ydim, zdim);
    int texels_to_process = bsd->texelcount_for_bitmap_partitioning;
    for (i = 0; i < texels_to_process; i++) {
        pt->coverage_bitmaps[pt->partition_of_texel[i]] |= 1ULL << i;
    }

}

void imageblock_initialize_orig_from_work_cpu(imageblock_cpu * pb, int pixelcount) {
    int i;
    float *fptr = pb->orig_data;
    float *wptr = pb->work_data;

    for (i = 0; i < pixelcount; i++) {
        if (pb->rgb_lns[i]) {
            fptr[0] = ASTC_Encoder::sf16_to_float(ASTC_Encoder::lns_to_sf16((uint16_t)wptr[0]));
            fptr[1] = ASTC_Encoder::sf16_to_float(ASTC_Encoder::lns_to_sf16((uint16_t)wptr[1]));
            fptr[2] = ASTC_Encoder::sf16_to_float(ASTC_Encoder::lns_to_sf16((uint16_t)wptr[2]));
        } else {
            fptr[0] = ASTC_Encoder::sf16_to_float(ASTC_Encoder::unorm16_to_sf16((uint16_t)wptr[0]));
            fptr[1] = ASTC_Encoder::sf16_to_float(ASTC_Encoder::unorm16_to_sf16((uint16_t)wptr[1]));
            fptr[2] = ASTC_Encoder::sf16_to_float(ASTC_Encoder::unorm16_to_sf16((uint16_t)wptr[2]));
        }

        if (pb->alpha_lns[i]) {
            fptr[3] = ASTC_Encoder::sf16_to_float(ASTC_Encoder::lns_to_sf16((uint16_t)wptr[3]));
        } else {
            fptr[3] = ASTC_Encoder::sf16_to_float(ASTC_Encoder::unorm16_to_sf16((uint16_t)wptr[3]));
        }

        fptr += 4;
        wptr += 4;
    }

    imageblock_initialize_deriv_from_work_and_orig_cpu(pb, pixelcount);
}

void unpack_color_endpoints_cpu(ASTC_Encoder::astc_decode_mode decode_mode, int format, int quantization_level,  int *input, int *rgb_hdr, int *alpha_hdr, int *nan_endpoint, ASTC_Encoder::ushort4 * output0, ASTC_Encoder::ushort4 * output1) {
    *nan_endpoint = 0;

    switch (format) {
    case ASTC_Encoder::FMT_LUMINANCE:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        ASTC_Encoder::luminance_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_LUMINANCE_DELTA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        ASTC_Encoder::luminance_delta_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_HDR_LUMINANCE_SMALL_RANGE:
        *rgb_hdr = 1;
        *alpha_hdr = -1;
        ASTC_Encoder::hdr_luminance_small_range_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_HDR_LUMINANCE_LARGE_RANGE:
        *rgb_hdr = 1;
        *alpha_hdr = -1;
        ASTC_Encoder::hdr_luminance_large_range_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_LUMINANCE_ALPHA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        ASTC_Encoder::luminance_alpha_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_LUMINANCE_ALPHA_DELTA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        ASTC_Encoder::luminance_alpha_delta_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_RGB_SCALE:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        ASTC_Encoder::rgb_scale_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_RGB_SCALE_ALPHA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        ASTC_Encoder::rgb_scale_alpha_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_HDR_RGB_SCALE:
        *rgb_hdr = 1;
        *alpha_hdr = -1;
        ASTC_Encoder::hdr_rgbo_unpack3(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_RGB:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        ASTC_Encoder::rgb_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_RGB_DELTA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        ASTC_Encoder::rgb_delta_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_HDR_RGB:
        *rgb_hdr = 1;
        *alpha_hdr = -1;
        ASTC_Encoder::hdr_rgb_unpack3(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_RGBA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        ASTC_Encoder::rgba_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_RGBA_DELTA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        ASTC_Encoder::rgba_delta_unpack(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_HDR_RGB_LDR_ALPHA:
        *rgb_hdr = 1;
        *alpha_hdr = 0;
        ASTC_Encoder::hdr_rgb_ldr_alpha_unpack3(input, quantization_level, output0, output1);
        break;

    case ASTC_Encoder::FMT_HDR_RGBA:
        *rgb_hdr = 1;
        *alpha_hdr = 1;
        ASTC_Encoder::hdr_rgb_hdr_alpha_unpack3(input, quantization_level, output0, output1);
        break;

    default:
        break;;
    }



    if (*alpha_hdr == -1) {
        if (g_ASTCEncode.m_alpha_force_use_of_hdr) {
            output0->w = 0x7800;
            output1->w = 0x7800;
            *alpha_hdr = 1;
        } else {
            output0->w = 0x00FF;
            output1->w = 0x00FF;
            *alpha_hdr = 0;
        }
    }



    switch (decode_mode) {
    case ASTC_Encoder::DECODE_LDR_SRGB:
        if (*rgb_hdr == 1) {
            output0->x = 0xFF00;
            output0->y = 0x0000;
            output0->z = 0xFF00;
            output0->w = 0xFF00;
            output1->x = 0xFF00;
            output1->y = 0x0000;
            output1->z = 0xFF00;
            output1->w = 0xFF00;
        } else {
            output0->x *= 257;
            output0->y *= 257;
            output0->z *= 257;
            output0->w *= 257;
            output1->x *= 257;
            output1->y *= 257;
            output1->z *= 257;
            output1->w *= 257;
        }
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        break;

    case ASTC_Encoder::DECODE_LDR:
        if (*rgb_hdr == 1) {
            output0->x = 0xFFFF;
            output0->y = 0xFFFF;
            output0->z = 0xFFFF;
            output0->w = 0xFFFF;
            output1->x = 0xFFFF;
            output1->y = 0xFFFF;
            output1->z = 0xFFFF;
            output1->w = 0xFFFF;
            *nan_endpoint = 1;
        } else {
            output0->x *= 257;
            output0->y *= 257;
            output0->z *= 257;
            output0->w *= 257;
            output1->x *= 257;
            output1->y *= 257;
            output1->z *= 257;
            output1->w *= 257;
        }
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        break;

    case ASTC_Encoder::DECODE_HDR:

        if (*rgb_hdr == 0) {
            output0->x *= 257;
            output0->y *= 257;
            output0->z *= 257;
            output1->x *= 257;
            output1->y *= 257;
            output1->z *= 257;
        }
        if (*alpha_hdr == 0) {
            output0->w *= 257;
            output1->w *= 257;
        }
        break;
    }
}


ASTC_Encoder::ushort4 lerp_color_int(ASTC_Encoder::astc_decode_mode decode_mode, ASTC_Encoder::ushort4 color0, ASTC_Encoder::ushort4 color1, int weight, int plane2_weight, int plane2_color_component    // -1 in 1-plane mode
                                    ) {
    ASTC_Encoder::int4 ecolor0 = ASTC_Encoder::int4(color0.x, color0.y, color0.z, color0.w);
    ASTC_Encoder::int4 ecolor1 = ASTC_Encoder::int4(color1.x, color1.y, color1.z, color1.w);

    ASTC_Encoder::int4 eweight1 = ASTC_Encoder::int4(weight, weight, weight, weight);
    switch (plane2_color_component) {
    case 0:
        eweight1.x = plane2_weight;
        break;
    case 1:
        eweight1.y = plane2_weight;
        break;
    case 2:
        eweight1.z = plane2_weight;
        break;
    case 3:
        eweight1.w = plane2_weight;
        break;
    default:
        break;
    }

    ASTC_Encoder::int4 eweight0 = ASTC_Encoder::int4(64, 64, 64, 64) - eweight1;

    if (decode_mode == ASTC_Encoder::DECODE_LDR_SRGB) {
        ecolor0 = ecolor0 >> 8;
        ecolor1 = ecolor1 >> 8;
    }

    ASTC_Encoder::int4 color = (ecolor0 * eweight0) + (ecolor1 * eweight1) + ASTC_Encoder::int4(32, 32, 32, 32);
    color = color >> 6;
    if (decode_mode == ASTC_Encoder::DECODE_LDR_SRGB)
        color = color | (color << 8);

    ASTC_Encoder::ushort4 rcolor = ASTC_Encoder::ushort4((ASTC_Encoder::ushort)color.x, (ASTC_Encoder::ushort)color.y, (ASTC_Encoder::ushort)color.z, (ASTC_Encoder::ushort)color.w);
    return rcolor;
}

int compute_value_of_texel_int_cpu(int texel_to_get, const decimation_table_cpu * it, const int *weights) {
    int i;
    int summed_value = 8;
    int weights_to_evaluate = it->texel_num_weights[texel_to_get];
    for (i = 0; i < weights_to_evaluate; i++) {
        summed_value += weights[it->texel_weights[texel_to_get][i]] * it->texel_weights_int[texel_to_get][i];
    }
    return summed_value >> 4;
}

void decompress_symbolic_block_cpu(ASTC_Encoder::astc_decode_mode decode_mode,
                                   int xdim, int ydim, int zdim,   // dimensions of block
                                   int xpos, int ypos, int zpos,   // position of block
                                   symbolic_compressed_block_cpu * scb,
                                   imageblock_cpu * blk) {
    blk->xpos = xpos;
    blk->ypos = ypos;
    blk->zpos = zpos;

    int i;

    // if we detected an error-block, blow up immediately.
    if (scb->error_block) {
        if (decode_mode == ASTC_Encoder::DECODE_LDR_SRGB) {
            for (i = 0; i < xdim * ydim * zdim; i++) {
                blk->orig_data[4 * i] = 1.0f;
                blk->orig_data[4 * i + 1] = 0.0f;
                blk->orig_data[4 * i + 2] = 1.0f;
                blk->orig_data[4 * i + 3] = 1.0f;
                blk->rgb_lns[i] = 0;
                blk->alpha_lns[i] = 0;
                blk->nan_texel[i] = 0;
            }
        } else {
            for (i = 0; i < xdim * ydim * zdim; i++) {
                blk->orig_data[4 * i] = 0.0f;
                blk->orig_data[4 * i + 1] = 0.0f;
                blk->orig_data[4 * i + 2] = 0.0f;
                blk->orig_data[4 * i + 3] = 0.0f;
                blk->rgb_lns[i] = 0;
                blk->alpha_lns[i] = 0;
                blk->nan_texel[i] = 1;
            }
        }

        imageblock_initialize_work_from_orig_cpu(blk, xdim * ydim * zdim);
        update_imageblock_flags_cpu(blk, xdim, ydim, zdim);
        return;
    }

    if (scb->block_mode < 0) {
        float red = 0, green = 0, blue = 0, alpha = 0;
        int use_lns = 0;
        int use_nan = 0;

        if (scb->block_mode == -2) {
            // For sRGB decoding, we should return only the top 8 bits.
            int mask = (decode_mode == ASTC_Encoder::DECODE_LDR_SRGB) ? 0xFF00 : 0xFFFF;

            red     = ASTC_Encoder::sf16_to_float(ASTC_Encoder::unorm16_to_sf16((uint16_t)scb->constant_color[0] & mask));
            green   = ASTC_Encoder::sf16_to_float(ASTC_Encoder::unorm16_to_sf16((uint16_t)scb->constant_color[1] & mask));
            blue    = ASTC_Encoder::sf16_to_float(ASTC_Encoder::unorm16_to_sf16((uint16_t)scb->constant_color[2] & mask));
            alpha   = ASTC_Encoder::sf16_to_float(ASTC_Encoder::unorm16_to_sf16((uint16_t)scb->constant_color[3] & mask));
            use_lns = 0;
            use_nan = 0;
        } else {
            switch (decode_mode) {
            case ASTC_Encoder::DECODE_LDR_SRGB:
                red = 1.0f;
                green = 0.0f;
                blue = 1.0f;
                alpha = 1.0f;
                use_lns = 0;
                use_nan = 0;
                break;
            case ASTC_Encoder::DECODE_LDR:
                red = 0.0f;
                green = 0.0f;
                blue = 0.0f;
                alpha = 0.0f;
                use_lns = 0;
                use_nan = 1;
                break;
            case ASTC_Encoder::DECODE_HDR:
                // constant-color block; unpack from FP16 to FP32.
                red     = ASTC_Encoder::sf16_to_float((sf16)scb->constant_color[0]);
                green   = ASTC_Encoder::sf16_to_float((sf16)scb->constant_color[1]);
                blue    = ASTC_Encoder::sf16_to_float((sf16)scb->constant_color[2]);
                alpha   = ASTC_Encoder::sf16_to_float((sf16)scb->constant_color[3]);
                use_lns = 1;
                use_nan = 0;
                break;
            }
        }

        for (i = 0; i < xdim * ydim * zdim; i++) {
            blk->orig_data[4 * i] = red;
            blk->orig_data[4 * i + 1] = green;
            blk->orig_data[4 * i + 2] = blue;
            blk->orig_data[4 * i + 3] = alpha;
            blk->rgb_lns[i] = (uint8_t)use_lns;
            blk->alpha_lns[i] = (uint8_t)use_lns;
            blk->nan_texel[i] = (uint8_t)use_nan;
        }


        imageblock_initialize_work_from_orig_cpu(blk, xdim * ydim * zdim);
        update_imageblock_flags_cpu(blk, xdim, ydim, zdim);
        return;
    }

    // get the appropriate partition-table entry
    int partition_count = scb->partition_count;

    if ((partition_count > 5) || (scb->partition_index > 1024))
        return;


    // get the appropriate block descriptor
    block_size_descriptor_cpu *bsd = get_block_size_descriptor_cpu(xdim, ydim, zdim);
    decimation_table_cpu **ixtab2 = bsd->decimation_tables;

    decimation_table_cpu *it = ixtab2[bsd->block_modes[scb->block_mode].decimation_mode];

    int is_dual_plane = bsd->block_modes[scb->block_mode].is_dual_plane;

    int weight_quantization_level = bsd->block_modes[scb->block_mode].quantization_mode;

    // decode the color endpoints
    ASTC_Encoder::ushort4 color_endpoint0[4];
    ASTC_Encoder::ushort4 color_endpoint1[4];
    int rgb_hdr_endpoint[4];
    int alpha_hdr_endpoint[4];
    int nan_endpoint[4];

    for (i = 0; i < partition_count; i++)
        unpack_color_endpoints_cpu(
            decode_mode,
            scb->color_formats[i],
            scb->color_quantization_level,
            scb->color_values[i],
            &(rgb_hdr_endpoint[i]),
            &(alpha_hdr_endpoint[i]),
            &(nan_endpoint[i]),
            &(color_endpoint0[i]),
            &(color_endpoint1[i]));

    // first unquantize the weights
    int uq_plane1_weights[MAX_WEIGHTS_PER_BLOCK];
    int uq_plane2_weights[MAX_WEIGHTS_PER_BLOCK];
    int weight_count = it->num_weights;

    const ASTC_Encoder::quantization_and_transfer_table *qat = &(ASTC_Encoder::quant_and_xfer_tables[weight_quantization_level]);

    for (i = 0; i < weight_count; i++) {
        uq_plane1_weights[i] = qat->unquantized_value[scb->plane1_weights[i]];
    }
    if (is_dual_plane) {
        for (i = 0; i < weight_count; i++)
            uq_plane2_weights[i] = qat->unquantized_value[scb->plane2_weights[i]];
    }


    // then un-decimate them.
    int weights[MAX_TEXELS_PER_BLOCK];
    int plane2_weights[MAX_TEXELS_PER_BLOCK];


    int texels_per_block = xdim * ydim * zdim;
    for (i = 0; i < texels_per_block; i++)
        weights[i] = compute_value_of_texel_int_cpu(i, it, uq_plane1_weights);

    if (is_dual_plane)
        for (i = 0; i < texels_per_block; i++)
            plane2_weights[i] = compute_value_of_texel_int_cpu(i, it, uq_plane2_weights);


    int plane2_color_component = scb->plane2_color_component;


    // now that we have endpoint colors and weights, we can unpack actual colors for
    // each texel.
    for (i = 0; i < texels_per_block; i++) {
        ASTC_Encoder::uint8_t partition = g_ASTCEncode.partition_tables[partition_count][scb->partition_index].partition_of_texel[i];

        ASTC_Encoder::ushort4 color = lerp_color_int(decode_mode,
                                      color_endpoint0[partition],
                                      color_endpoint1[partition],
                                      weights[i],
                                      plane2_weights[i],
                                      is_dual_plane ? plane2_color_component : -1);

        blk->rgb_lns[i] = (uint8_t)rgb_hdr_endpoint[partition];
        blk->alpha_lns[i] = (uint8_t)alpha_hdr_endpoint[partition];
        blk->nan_texel[i] = (uint8_t)nan_endpoint[partition];

        blk->work_data[4 * i] = color.x;
        blk->work_data[4 * i + 1] = color.y;
        blk->work_data[4 * i + 2] = color.z;
        blk->work_data[4 * i + 3] = color.w;
    }

    imageblock_initialize_orig_from_work_cpu(blk, xdim * ydim * zdim);
    update_imageblock_flags_cpu(blk, xdim, ydim, zdim);
}

// End CPU Decoder Code
//-----------------------------------------------

