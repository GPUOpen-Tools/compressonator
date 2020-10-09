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
//===========================================================================
// Copyright (c) 2014-2017  Advanced Micro Devices, Inc. All rights reserved.
//===========================================================================



//====================================== Kernal Compliant Code ============================================

#include "astc_encode_kernel.h"

namespace ASTC_Encoder {


// This is tempory and will be removed
// global2 is used to identify the ASTC_Encode arrays that need to be removed or reduced in size from OpenCL call stack!
#define __global2           __global

#ifndef __OPENCL_VERSION__
#define DEBUG(x)        //printf("%s\n",x);
#else
#define DEBUG(x)        //if ((get_global_id(0) == 0) && (get_global_id(1) == 0)) printf("%s\n",x);
#endif

#define astc_codec_internal_error(x)         printf(x);

#ifndef __OPENCL_VERSION__
float dot(float2 p, float2 q) {
    return p.x * q.x + p.y * q.y;
}

float dot(float3 p, float3 q) {
    return p.x * q.x + p.y * q.y + p.z * q.z;
}

float dot(float4 p, float4 q) {
    return p.x * q.x + p.y * q.y + p.z * q.z + p.w * q.w;
}

float3 cross(float3 p, float3 q) {
    return p.yzx * q.zxy - p.zxy * q.yzx;
}

float length(float2 p) {
    return (float)(sqrt(dot(p, p)));
}

float length(float3 p) {
    return (float)(sqrt(dot(p, p)));
}

float length(float4 p) {
    return (float)(sqrt(dot(p, p)));
}

float2 normalize(float2 p) {
    return p / length(p);
}

float3 normalize(float3 p) {
    return p / length(p);
}

float4 normalize(float4 p) {
    return p / length(p);
}

#endif

int compute_ise_bitcount(int items, quantization_method quant) {
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

//================================
// float float_to_lns(float p)
//================================
#define astc_isnan(p) ((p)!=(p))

float float_to_lns(float p) {
    if (astc_isnan(p) || p <= 1.0f / 67108864.0f) {
        // underflow or NaN value, return 0.
        // We count underflow if the input value is smaller than 2^-26.
        return 0;
    }

    if (fabs(p) >= 65536.0f) {
        // overflow, return a +INF value
        return 65535;
    }

    int expo;
    float normfrac = (float)frexp(p, &expo);
    float p1;
    if (expo < -13) {
        // input number is smaller than 2^-14. In this case, multiply by 2^25.
        p1 = p * 33554432.0f;
        expo = 0;
    } else {
        expo += 14;
        p1 = (normfrac - 0.5f) * 4096.0f;
    }

    if (p1 < 384.0f)
        p1 *= 4.0f / 3.0f;
    else if (p1 <= 1408.0f)
        p1 += 128.0f;
    else
        p1 = (p1 + 512.0f) * (4.0f / 5.0f);

    p1 += expo * 2048.0f;
    return p1 + 1.0f;
}

void imageblock_initialize_deriv_from_work_and_orig(imageblock * pb, int pixelcount) {
    float *fptr = pb->orig_data;
    float *wptr = pb->work_data;
    float *dptr = pb->deriv_data;

    for (int i = 0; i < pixelcount; i++) {

        // compute derivatives for RGB first
        if (pb->rgb_lns[i]) {
            float r = (std::max)(fptr[0], 6e-5f);
            float g = (std::max)(fptr[1], 6e-5f);
            float b = (std::max)(fptr[2], 6e-5f);

            float rderiv = (float_to_lns(r * 1.05f) - float_to_lns(r)) / (r * 0.05f);
            float gderiv = (float_to_lns(g * 1.05f) - float_to_lns(g)) / (g * 0.05f);
            float bderiv = (float_to_lns(b * 1.05f) - float_to_lns(b)) / (b * 0.05f);

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
            float a = (std::max)(fptr[3], 6e-5f);
            float aderiv = (float_to_lns(a * 1.05f) - float_to_lns(a)) / (a * 0.05f);
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

//=================================================================
// helper function to initialize the work-data from the orig-data
//=================================================================
void imageblock_initialize_work_from_orig(imageblock * pb, int pixelcount) {
    float *fptr = pb->orig_data;
    float *wptr = pb->work_data;

    for (int i = 0; i < pixelcount; i++) {
        if (pb->rgb_lns[i]) {
            wptr[0] = float_to_lns(fptr[0]);
            wptr[1] = float_to_lns(fptr[1]);
            wptr[2] = float_to_lns(fptr[2]);
        } else {
            wptr[0] = fptr[0] * 65535.0f;
            wptr[1] = fptr[1] * 65535.0f;
            wptr[2] = fptr[2] * 65535.0f;
        }

        if (pb->alpha_lns[i]) {
            wptr[3] = float_to_lns(fptr[3]);
        } else {
            wptr[3] = fptr[3] * 65535.0f;
        }
        fptr += 4;
        wptr += 4;
    }

    imageblock_initialize_deriv_from_work_and_orig(pb, pixelcount);
}

//=========================================================================
// For an imageblock, update its flags.
//
// The updating is done based on work_data, not orig_data.
//=========================================================================

void update_imageblock_flags(imageblock * pb, __global ASTC_Encode *ASTC_Encode) {
    float red_min = FLOAT_38, red_max = -FLOAT_38;
    float green_min = FLOAT_38, green_max = -FLOAT_38;
    float blue_min = FLOAT_38, blue_max = -FLOAT_38;
    float alpha_min = FLOAT_38, alpha_max = -FLOAT_38;

    int grayscale = 1;
    //printf(" m_texels_per_block %d ", ASTC_Encode->m_texels_per_block);

    for (int i = 0; i < ASTC_Encode->m_texels_per_block; i++) {
        float red = pb->work_data[4 * i];
        float green = pb->work_data[4 * i + 1];
        float blue = pb->work_data[4 * i + 2];
        float alpha = pb->work_data[4 * i + 3];

        //printf(" alpha %f ", alpha);

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

void fetch_imageblock(
    astc_codec_image *input_image,
    imageblock *blk,
    unsigned int pixelcount,
    __global ASTC_Encode *ASTC_Encode
) {
    //---------------------------------------------------------------
    // Convert the source RGBA byte pixels to float for ASTC
    //---------------------------------------------------------------
    // Note: Swizzel is set at {0,1,2,3}
    // The image source data is set to 8 bit
    unsigned int findex = 0;
    for (unsigned int px=0; px<pixelcount; px++) {
        blk->orig_data[findex++] = input_image->pixels[px].x / 255.0f;    // Red
        blk->orig_data[findex++] = input_image->pixels[px].y / 255.0f;    // Green
        blk->orig_data[findex++] = input_image->pixels[px].z / 255.0f;    // Blue
        blk->orig_data[findex++] = input_image->pixels[px].w / 255.0f;    // Alpha
    }

    //---------------------------------------------------------------
    // perform sRGB-to-linear transform on input data, if requested.
    //---------------------------------------------------------------
    // if (g_ASTCEncode.m_perform_srgb_transform)     { Not implemented }
    //---------------------------------------------------------------
    // collect color max-value, in order to determine whether to use LDR or HDR
    // interpolation.
    //---------------------------------------------------------------
    // float max_red, max_green, max_blue, max_alpha;
    // max_red = 0.0f;
    // max_green = 0.0f;
    // max_blue = 0.0f;
    // max_alpha = 0.0f;
    //
    // findex = 0;
    // for (unsigned int i = 0; i < pixelcount; i++)
    // {
    //     float r = pb->orig_data[findex++];
    //     float g = pb->orig_data[findex++];
    //     float b = pb->orig_data[findex++];
    //     float a = pb->orig_data[findex++];
    //
    //     if (r > max_red)
    //         max_red = r;
    //     if (g > max_green)
    //         max_green = g;
    //     if (b > max_blue)
    //         max_blue = b;
    //     if (a > max_alpha)
    //         max_alpha = a;
    // }
    //
    // float max_rgb = (std::max)(max_red, (std::max)(max_green, max_blue));
    // printf("max rgb = %2.2f",max_rgb);
    //---------------------------------------------------------------------------
    // use LNS if:
    // * RGB-maximum is less than 0.15
    // * RGB-maximum is greater than 1
    // * Alpha-maximum is greater than 1
    //----------------------------------------------------------------------------
    //  int rgb_lns  = (max_rgb < 0.15f || max_rgb > 1.0f || max_alpha > 1.0f) ? 1 : 0;
    //  int alpha_lns  = rgb_lns ? (max_alpha > 1.0f || max_alpha < 0.15f) : 0;

    // impose the choice on every pixel when encoding.
    for (unsigned int i = 0; i < pixelcount; i++) {
        blk->rgb_lns[i]   = (uint8_t)ASTC_Encode->m_rgb_force_use_of_hdr;
        blk->alpha_lns[i] = (uint8_t)ASTC_Encode->m_alpha_force_use_of_hdr;
        blk->nan_texel[i] = 0;
    }

    imageblock_initialize_work_from_orig(blk, pixelcount);
    update_imageblock_flags(blk, ASTC_Encode);
}


uint32_t rtne_shift32(uint32_t inp, uint32_t shamt) {
    uint32_t vl1 = (uint32_t)(1) << shamt;
    uint32_t inp2 = inp + (vl1 >> 1);    /* added 0.5 ulp */
    uint32_t msk = (inp | (uint32_t)(1)) & vl1;    /* nonzero if odd. '| 1' forces it to 1 if the shamt is 0. */
    msk--;                        /* negative if even, nonnegative if odd. */
    inp2 -= (msk >> 31);        /* subtract epsilon before shift if even. */
    inp2 >>= shamt;
    return inp2;
}

uint32_t rtna_shift32(uint32_t inp, uint32_t shamt) {
    uint32_t vl1 = ((uint32_t)(1) << shamt) >> 1;
    inp += vl1;
    inp >>= shamt;
    return inp;
}

uint32_t rtup_shift32(uint32_t inp, uint32_t shamt) {
    uint32_t vl1 = (uint32_t)(1) << shamt;
    inp += vl1;
    inp--;
    inp >>= shamt;
    return inp;
}

CGU_SHORT sf32_to_sf16(CGU_UINT inp, roundmode rmode) {

    uint32_t p;
    uint32_t idx = rmode + tab[inp >> 23];
    uint32_t vlx = tabx[idx];
    switch (idx) {
    /*
          Positive number which may be Infinity or NaN.
        We need to check whether it is NaN; if it is, quieten it by setting the top bit of the mantissa.
        (If we don't do this quieting, then a NaN  that is distinguished only by having
        its low-order bits set, would be turned into an INF. */
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
        /*
            the input value is 0x7F800000 or 0xFF800000 if it is INF.
            By subtracting 1, we get 7F7FFFFF or FF7FFFFF, that is, bit 23 becomes zero.
            For NaNs, however, this operation will keep bit 23 with the value 1.
            We can then extract bit 23, and logical-OR bit 9 of the result with this
            bit in order to quieten the NaN (a Quiet NaN is a NaN where the top bit
            of the mantissa is set.)
        */
        p = (inp - 1) & (uint32_t)(0x800000);    /* zero if INF, nonzero if NaN. */
        return (CGU_SHORT)(((inp + (CGU_SHORT)vlx) >> 13) | (p >> 14));
    /*
        positive, exponent = 0, round-mode == UP; need to check whether number actually is 0.
        If it is, then return 0, else return 1 (the smallest representable nonzero number)
    */
    case 0:
        /*
            -inp will set the MSB if the input number is nonzero.
            Thus (-inp) >> 31 will turn into 0 if the input number is 0 and 1 otherwise.
        */
        return (uint32_t) (-(int32_t) inp) >> 31;

    /*
        negative, exponent = , round-mode == DOWN, need to check whether number is
        actually 0. If it is, return 0x8000 ( float -0.0 )
        Else return the smallest negative number ( 0x8001 ) */
    case 6:
        /*
            in this case 'vlx' is 0x80000000. By subtracting the input value from it,
            we obtain a value that is 0 if the input value is in fact zero and has
            the MSB set if it isn't. We then right-shift the value by 31 places to
            get a value that is 0 if the input is -0.0 and 1 otherwise.
        */
        return ((vlx - inp) >> 31) + (uint32_t)(0x8000);

    /*
        for all other cases involving underflow/overflow, we don't need to
        do actual tests; we just return 'vlx'.
    */
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
        return (CGU_SHORT)vlx;

    /*
        for normal numbers, 'vlx' is the difference between the FP32 value of a number and the
        FP16 representation of the same number left-shifted by 13 places. In addition, a rounding constant is
        baked into 'vlx': for rounding-away-from zero, the constant is 2^13 - 1, causing roundoff away
        from zero. for round-to-nearest away, the constant is 2^12, causing roundoff away from zero.
        for round-to-nearest-even, the constant is 2^12 - 1. This causes correct round-to-nearest-even
        except for odd input numbers. For odd input numbers, we need to add 1 to the constant. */

    /* normal number, all rounding modes except round-to-nearest-even: */
    case 30:
    case 31:
    case 32:
    case 34:
    case 35:
    case 36:
    case 37:
    case 39:
        return (CGU_SHORT)((inp + vlx) >> 13);

    /* normal number, round-to-nearest-even. */
    case 33:
    case 38:
        p = inp + vlx;
        p += (inp >> 13) & 1;
        return (CGU_SHORT)(p >> 13);

    /*
        the various denormal cases. These are not expected to be common, so their performance is a bit
        less important. For each of these cases, we need to extract an exponent and a mantissa
        (including the implicit '1'!), and then right-shift the mantissa by a shift-amount that
        depends on the exponent. The shift must apply the correct rounding mode. 'vlx' is used to supply the
        sign of the resulting denormal number.
    */
    case 21:
    case 22:
    case 25:
    case 27:
        /* denormal, round towards zero. */
        p = 126 - ((inp >> 23) & 0xFF);
        return (CGU_SHORT)((((inp & (uint32_t)(0x7FFFFF)) + (uint32_t)(0x800000)) >> p) | vlx);
    case 20:
    case 26:
        /* denornal, round away from zero. */
        p = 126 - ((inp >> 23) & 0xFF);
        return (CGU_SHORT)(rtup_shift32((inp & (uint32_t)(0x7FFFFF)) + (uint32_t)(0x800000), p) | vlx);
    case 24:
    case 29:
        /* denornal, round to nearest-away */
        p = 126 - ((inp >> 23) & 0xFF);
        return (CGU_SHORT)(rtna_shift32((inp & (uint32_t)(0x7FFFFF)) + (uint32_t)(0x800000), p) | vlx);
    case 23:
    case 28:
        /* denormal, round to nearest-even. */
        p = 126 - ((inp >> 23) & 0xFF);
        return (CGU_SHORT)(rtne_shift32((inp & (uint32_t)(0x7FFFFF)) + (uint32_t)(0x800000), p) | vlx);
    }

    return 0;
}

CGU_SHORT float_to_sf16(float p, roundmode rm) {
    if32 i;
    i.f = p;
    return sf32_to_sf16(i.u, rm);
}

// napatel this code needs optimization
float prepare_error_weight_block(
    imageblock * blk,
    error_weight_block * ewb,
    error_weight_block_orig * ewbo,
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("prepare_error_weight_block");

    if (ASTCEncode->m_compress_to_mono) {
        return 1.0f;
    }

    unsigned int x, y;
    int idx = 0;

    int any_mean_stdev_weight =
        ASTCEncode->m_ewp.rgb_base_weight != 1.0 || ASTCEncode->m_ewp.alpha_base_weight != 1.0 || ASTCEncode->m_ewp.rgb_mean_weight != 0.0 || ASTCEncode->m_ewp.rgb_stdev_weight != 0.0 || ASTCEncode->m_ewp.alpha_mean_weight != 0.0 || ASTCEncode->m_ewp.alpha_stdev_weight != 0.0;

    float4 color_weights = {ASTCEncode->m_ewp.rgba_weights[0],
                            ASTCEncode->m_ewp.rgba_weights[1],
                            ASTCEncode->m_ewp.rgba_weights[2],
                            ASTCEncode->m_ewp.rgba_weights[3]
                           };

//#ifdef __OPENCL_VERSION__
//    if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//        printf("color_weights %3.3f %3.3f %3.3f %3.3f\n", color_weights.x, color_weights.y, color_weights.z, color_weights.w );

    ewb->contains_zeroweight_texels = 0;
    float4 normals = {1.0f, 1.0f, 1.0f, 1.0f};

#ifdef ENABLE_3D_SUPPORT
    for (int z = 0; z < ASTCEncode->m_zdim; z++)
#endif
        for (y = 0; y < ASTCEncode->m_ydim; y++)
            for (x = 0; x < ASTCEncode->m_xdim; x++) {
                float4 error_weight = { ASTCEncode->m_ewp.rgb_base_weight,
                                        ASTCEncode->m_ewp.rgb_base_weight,
                                        ASTCEncode->m_ewp.rgb_base_weight,
                                        ASTCEncode->m_ewp.alpha_base_weight
                                      };

                if (any_mean_stdev_weight) {
                    // napatel
                    // <=== This needs proper sizes ===> made local and should be set to image dimensions!!
                    // float4 input_averages[4][4][4];
                    //--------------------------------------

                    float4 avg = { 0.0f, 0.0f, 0.0f, 0.0f }; //  input_averages[0][0][0]; // was [zpos][ypos][xpos] need to use correct value
                    if (avg.x < 6e-5f)
                        avg.x = 6e-5f;
                    if (avg.y < 6e-5f)
                        avg.y = 6e-5f;
                    if (avg.z < 6e-5f)
                        avg.z = 6e-5f;
                    if (avg.w < 6e-5f)
                        avg.w = 6e-5f;
                    /*
                       printf("avg: %f %f %f %f\n", avg.x, avg.y, avg.z, avg.w ); */
                    avg = avg * avg;

                    //# Check this sections of code!
                    // napatel
                    // <=== This needs proper sizes ===> made local and should be set to image dimensions!!
                    // float4 input_variances[4][4][4];

                    //--------------------------------------
                    float4 variance = { 0.0f, 0.0f, 0.0f, 0.0f }; // input_variances[0][0][0]; // was [zpos][ypos][xpos] need to use correct value
                    variance = variance * variance;

                    float favg = (avg.x + avg.y + avg.z) * (1.0f / 3.0f);
                    //float fvar = (variance.x + variance.y + variance.z) * (1.0f / 3.0f);

                    float mixing = ASTCEncode->m_ewp.rgb_mean_and_stdev_mixing;
                    float3 favg3 = { favg, favg, favg };

                    avg.xyz = favg3 * mixing + avg.xyz * (1.0f - mixing);
                    variance.xyz = favg3 * mixing + variance.xyz * (1.0f - mixing);

                    float4 stdev = {(float)sqrt((std::max)(variance.x, 0.0f)),
                                    (float)sqrt((std::max)(variance.y, 0.0f)),
                                    (float)sqrt((std::max)(variance.z, 0.0f)),
                                    (float)sqrt((std::max)(variance.w, 0.0f))
                                   };

                    avg.xyz = avg.xyz * ASTCEncode->m_ewp.rgb_mean_weight;
                    avg.w = avg.w * ASTCEncode->m_ewp.alpha_mean_weight;
                    stdev.xyz = stdev.xyz * ASTCEncode->m_ewp.rgb_stdev_weight;
                    stdev.w = stdev.w * ASTCEncode->m_ewp.alpha_stdev_weight;
                    error_weight = error_weight + avg + stdev;
                    float4 onef = { 1.0f, 1.0f, 1.0f, 1.0f };
                    error_weight = onef / error_weight;
                }

#ifdef USE_RA_NORMAL_ANGULAR_SCALE
                if (ASTCEncode->m_ewp.ra_normal_angular_scale) {
                    float x1 = (blk->orig_data[4 * idx] - 0.5f) * 2.0f;
                    float y1 = (blk->orig_data[4 * idx + 3] - 0.5f) * 2.0f;
                    float denom = 1.0f - x1 * x1 - y1 * y1;
                    if (denom < 0.1f)
                        denom = 0.1f;
                    denom = 1.0f / denom;
                    error_weight.x *= 1.0f + x1 * x1 * denom;
                    error_weight.w *= 1.0f + y1 * y1 * denom;
                }
#endif

#ifdef USE_RGB_SCALE_WITH_ALPHA
                if (ASTCEncode->m_ewp.enable_rgb_scale_with_alpha) {
                    // napatel
                    // <=== This needs proper sizes ===> made local and should be set to image dimensions!!
                    // float4 input_alpha_averages[12][12][12];
                    //--------------------------------------

                    float alpha_scale;
                    if (ASTCEncode->m_ewp.alpha_radius != 0)
                        alpha_scale = 0.0f; //input_alpha_averages[0][0][0].x; // was [zpos][ypos][xpos] need to use correct value
                    else
                        alpha_scale = blk->orig_data[4 * idx + 3];
                    if (alpha_scale < 0.0001f)
                        alpha_scale = 0.0001f;
                    alpha_scale *= alpha_scale;
                    error_weight.xyz = error_weight.xyz * alpha_scale;
                }
#endif


                error_weight = error_weight * color_weights;
                error_weight = error_weight * ASTCEncode->m_ewp.block_artifact_suppression_expanded[idx];

#ifdef USE_PERFORMM_SRGB_TRANSFORM
                // if we perform a conversion from linear to sRGB, then we multiply
                // the weight with the derivative of the linear->sRGB transform function.
                if (ASTCEncode->m_perform_srgb_transform) {
                    float r = blk->orig_data[4 * idx];
                    float g = blk->orig_data[4 * idx + 1];
                    float b = blk->orig_data[4 * idx + 2];
                    if (r < 0.0031308f)
                        r = 12.92f;
                    else
                        r = 0.4396f * pow(r, -0.58333f);
                    if (g < 0.0031308f)
                        g = 12.92f;
                    else
                        g = 0.4396f * pow(g, -0.58333f);
                    if (b < 0.0031308f)
                        b = 12.92f;
                    else
                        b = 0.4396f * pow(b, -0.58333f);
                    error_weight.x *= r;
                    error_weight.y *= g;
                    error_weight.z *= b;
                }
#endif

                // when we loaded the block to begin with, we applied a transfer function
                // and computed the derivative of the transfer function. However, the
                // error-weight computation so far is based on the original color values,
                // not the transfer-function values. As such, we must multiply the
                // error weights by the derivative of the inverse of the transfer function,
                // which is equivalent to dividing by the derivative of the transfer
                // function.

                ewbo->error_weights[idx] = error_weight;

                error_weight.x /= (blk->deriv_data[4 * idx] * blk->deriv_data[4 * idx] * FLOAT_n10);
                error_weight.y /= (blk->deriv_data[4 * idx + 1] * blk->deriv_data[4 * idx + 1] * FLOAT_n10);
                error_weight.z /= (blk->deriv_data[4 * idx + 2] * blk->deriv_data[4 * idx + 2] * FLOAT_n10);
                error_weight.w /= (blk->deriv_data[4 * idx + 3] * blk->deriv_data[4 * idx + 3] * FLOAT_n10);

                /*
                    printf("--> %f %f %f %f\n", error_weight.x, error_weight.y, error_weight.z, error_weight.w );
                */

                ewb->error_weights[idx] = error_weight;
                float res = dot(error_weight, normals);
                if (res < FLOAT_n10)
                    ewb->contains_zeroweight_texels = 1;

                idx++;
            }

    int i;

    float4 error_weight_sum = { 0.0f, 0.0f, 0.0f, 0.0f };

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        error_weight_sum = error_weight_sum + ewb->error_weights[i];

        ewb->texel_weight_r[i] = ewb->error_weights[i].x;
        ewb->texel_weight_g[i] = ewb->error_weights[i].y;
        ewb->texel_weight_b[i] = ewb->error_weights[i].z;
        ewb->texel_weight_a[i] = ewb->error_weights[i].w;

        ewb->texel_weight_rg[i] = (ewb->error_weights[i].x + ewb->error_weights[i].y) * 0.5f;
        ewb->texel_weight_rb[i] = (ewb->error_weights[i].x + ewb->error_weights[i].z) * 0.5f;
        ewb->texel_weight_gb[i] = (ewb->error_weights[i].y + ewb->error_weights[i].z) * 0.5f;
        ewb->texel_weight_ra[i] = (ewb->error_weights[i].x + ewb->error_weights[i].w) * 0.5f;

        ewb->texel_weight_gba[i] = (ewb->error_weights[i].y + ewb->error_weights[i].z + ewb->error_weights[i].w) * 0.333333f;
        ewb->texel_weight_rba[i] = (ewb->error_weights[i].x + ewb->error_weights[i].z + ewb->error_weights[i].w) * 0.333333f;
        ewb->texel_weight_rga[i] = (ewb->error_weights[i].x + ewb->error_weights[i].y + ewb->error_weights[i].w) * 0.333333f;
        ewb->texel_weight_rgb[i] = (ewb->error_weights[i].x + ewb->error_weights[i].y + ewb->error_weights[i].z) * 0.333333f;
        ewb->texel_weight[i] = (ewb->error_weights[i].x + ewb->error_weights[i].y + ewb->error_weights[i].z + ewb->error_weights[i].w) * 0.25f;
    }

    return dot(error_weight_sum, normals);
}

void compute_partition_error_color_weightings(
    error_weight_block * ewb,
    __global partition_info * pi,
    float4 error_weightings[4],
    float4 color_scalefactors[4],
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_partition_error_color_weightings");

    int i;
    int pcnt = pi->partition_count;
    float4 one12f = { FLOAT_n12, FLOAT_n12, FLOAT_n12, FLOAT_n12 };
    for (i = 0; i < pcnt; i++)
        error_weightings[i] = one12f;
    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        int part = pi->partition_of_texel[i];
        error_weightings[part] = error_weightings[part] + ewb->error_weights[i];
    }
    for (i = 0; i < pcnt; i++) {
        error_weightings[i] = error_weightings[i] * (1.0f / pi->texels_per_partition[i]);
    }
    for (i = 0; i < pcnt; i++) {
        color_scalefactors[i].x = sqrt(error_weightings[i].x);
        color_scalefactors[i].y = sqrt(error_weightings[i].y);
        color_scalefactors[i].z = sqrt(error_weightings[i].z);
        color_scalefactors[i].w = sqrt(error_weightings[i].w);
    }

}

void compute_averages_and_directions_rgba(
    __global partition_info * pt,
    imageblock * blk,
    error_weight_block * ewb,
    float4 * color_scalefactors,
    float4 * averages,
    float4 * directions_rgba,
    float3 * directions_gba,
    float3 * directions_rba,
    float3 * directions_rga,
    float3 * directions_rgb) {
    int i;
    int partition_count = pt->partition_count;
    int partition;

    for (partition = 0; partition < partition_count; partition++) {
        int texelcount = pt->texels_per_partition[partition];

        float4 base_sum = {0.0f, 0.0f, 0.0f, 0.0f };
        float partition_weight = 0.0f;

        for (i = 0; i < texelcount; i++) {
            int iwt = pt->texels_of_partition[partition][i];
            float weight = ewb->texel_weight[iwt];
            float4 texel_datum = {blk->work_data[4 * iwt],
                                  blk->work_data[4 * iwt + 1],
                                  blk->work_data[4 * iwt + 2],
                                  blk->work_data[4 * iwt + 3]
                                 };
            texel_datum =  texel_datum * weight;
            partition_weight += weight;

            base_sum = base_sum + texel_datum;
        }

        float4 average = base_sum * 1.0f / MAX(partition_weight, FLOAT_n7);
        averages[partition] = average * color_scalefactors[partition];


        float4 sum_xp = {0.0f, 0.0f, 0.0f, 0.0f};
        float4 sum_yp = {0.0f, 0.0f, 0.0f, 0.0f};
        float4 sum_zp = {0.0f, 0.0f, 0.0f, 0.0f};
        float4 sum_wp = {0.0f, 0.0f, 0.0f, 0.0f};

        for (i = 0; i < texelcount; i++) {
            int iwt = pt->texels_of_partition[partition][i];
            float weight = ewb->texel_weight[iwt];
            float4 texel_datum = {blk->work_data[4 * iwt],
                                  blk->work_data[4 * iwt + 1],
                                  blk->work_data[4 * iwt + 2],
                                  blk->work_data[4 * iwt + 3]
                                 };
            texel_datum = (texel_datum - average) * weight;

            if (texel_datum.x > 0.0f)
                sum_xp = sum_xp + texel_datum;
            if (texel_datum.y > 0.0f)
                sum_yp = sum_yp + texel_datum;
            if (texel_datum.z > 0.0f)
                sum_zp = sum_zp + texel_datum;
            if (texel_datum.w > 0.0f)
                sum_wp = sum_wp + texel_datum;
        }

        float prod_xp = dot(sum_xp, sum_xp);
        float prod_yp = dot(sum_yp, sum_yp);
        float prod_zp = dot(sum_zp, sum_zp);
        float prod_wp = dot(sum_wp, sum_wp);

        float4 best_vector = sum_xp;
        float best_sum = prod_xp;
        if (prod_yp > best_sum) {
            best_vector = sum_yp;
            best_sum = prod_yp;
        }
        if (prod_zp > best_sum) {
            best_vector = sum_zp;
            best_sum = prod_zp;
        }
        if (prod_wp > best_sum) {
            best_vector = sum_wp;
            best_sum = prod_wp;
        }

        directions_rgba[partition] = best_vector;
        directions_rgb[partition] = best_vector.xyz;
        directions_rga[partition] = best_vector.xyw;
        directions_rba[partition] = best_vector.xzw;
        directions_gba[partition] = best_vector.yzw;
    }
}

void compute_endpoints_and_ideal_weights_rgba(
    __global  partition_info * pt,
    imageblock * blk,
    error_weight_block * ewb,
    endpoints_and_weights * ei,
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_endpoints_and_ideal_weights_rgba");

    int i;

    float *error_weights = ewb->texel_weight;

    int partition_count = pt->partition_count;
    float lowparam[4], highparam[4];
    for (i = 0; i < partition_count; i++) {
        lowparam[i] = FLOAT_10;
        highparam[i] = -FLOAT_10;
    }

    float4 averages[4];
    float4 directions_rgba[4];
    float3 directions_gba[4];
    float3 directions_rba[4];
    float3 directions_rga[4];
    float3 directions_rgb[4];

    line4 lines[4];

    float scale[4];
    float length_squared[4];

    float4 error_weightings[4];
    float4 color_scalefactors[4];
    float4 scalefactors[4];

    compute_partition_error_color_weightings(ewb, pt, error_weightings, color_scalefactors, ASTCEncode);

    for (i = 0; i < partition_count; i++)
        scalefactors[i] = normalize(color_scalefactors[i]) * 2.0f;



    compute_averages_and_directions_rgba(pt, blk, ewb, scalefactors, averages, directions_rgba, directions_gba, directions_rba, directions_rga, directions_rgb);

    // if the direction-vector ends up pointing from light to dark, FLIP IT!
    // this will make the first endpoint the darkest one.
    float4 zerof = { 0.0f, 0.0f, 0.0f, 0.0f};
    float4 onef = { 1.0f, 1.0f, 1.0f, 1.0f };

    for (i = 0; i < partition_count; i++) {
        float4 direc = directions_rgba[i];
        if (direc.x + direc.y + direc.z < 0.0f)
            directions_rgba[i] = zerof - direc;
    }

    for (i = 0; i < partition_count; i++) {
        lines[i].a = averages[i];
        if (dot(directions_rgba[i], directions_rgba[i]) == 0.0f)
            lines[i].b = normalize(onef);
        else
            lines[i].b = normalize(directions_rgba[i]);
    }

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        if (error_weights[i] > FLOAT_n10) {
            int partition = pt->partition_of_texel[i];

            float4 point = { blk->work_data[4 * i], blk->work_data[4 * i + 1], blk->work_data[4 * i + 2], blk->work_data[4 * i + 3] };
            point = point * scalefactors[partition];
            line4 l = lines[partition];

            float param = dot(point - l.a, l.b);
            ei->weights[i] = param;
            if (param < lowparam[partition])
                lowparam[partition] = param;
            if (param > highparam[partition])
                highparam[partition] = param;
        } else {
            ei->weights[i] = -FLOAT_38;
        }
    }

    for (i = 0; i < partition_count; i++) {
        float length = highparam[i] - lowparam[i];
        if (length < 0) {
            lowparam[i] = 0.0f;
            highparam[i] = FLOAT_n7;
        }


        // it is possible for a uniform-color partition to produce length=0; this
        // causes NaN-production and NaN-propagation later on. Set length to
        // a small value to avoid this problem.
        if (length < FLOAT_n7)
            length = FLOAT_n7;

        length_squared[i] = length * length;
        scale[i] = 1.0f / length;

        ei->ep.endpt0[i] = (lines[i].a + lines[i].b * lowparam[i]) / scalefactors[i];
        ei->ep.endpt1[i] = (lines[i].a + lines[i].b * highparam[i]) / scalefactors[i];
    }

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        int partition = pt->partition_of_texel[i];
        float idx = (ei->weights[i] - lowparam[partition]) * scale[partition];
        if (idx > 1.0f)
            idx = 1.0f;
        else if (!(idx > 0.0f))
            idx = 0.0f;
        ei->weights[i] = idx;
        ei->weight_error_scale[i] = error_weights[i] * length_squared[partition];
        if (astc_isnan(ei->weight_error_scale[i])) {
            astc_codec_internal_error("ERROR: compute_endpoints_and_ideal_weights_rgba");
        }
    }
}

void compute_averages_and_directions_rgb(__global partition_info * pt,
        imageblock * blk,
        error_weight_block * ewb,
        float4 * color_scalefactors,
        float3 * averages,
        float3 * directions_rgb,
        float2 * directions_rg,
        float2 * directions_rb,
        float2 * directions_gb) {
    int i;
    int partition_count = pt->partition_count;
    int partition;

    float *texel_weights = ewb->texel_weight_rgb;

    for (partition = 0; partition < partition_count; partition++) {
        int texelcount = pt->texels_per_partition[partition];

        float3 one3f     = { 0.0f, 0.0f, 0.0f };
        float3 base_sum  = one3f;
        float partition_weight = 0.0f;

        for (i = 0; i < texelcount; i++) {
            int iwt = pt->texels_of_partition[partition][i];
            float weight = texel_weights[iwt];

            float3 texel_datum = { blk->work_data[4 * iwt],
                                   blk->work_data[4 * iwt + 1],
                                   blk->work_data[4 * iwt + 2]
                                 };
            texel_datum = texel_datum * weight;
            partition_weight += weight;

            base_sum = base_sum + texel_datum;
        }

        float4 csf = color_scalefactors[partition];
        float3 average = base_sum * 1.0f / MAX(partition_weight, FLOAT_n7);
        averages[partition] = average * csf.xyz;


        float3 sum_xp = one3f;
        float3 sum_yp = one3f;
        float3 sum_zp = one3f;

        for (i = 0; i < texelcount; i++) {
            int iwt = pt->texels_of_partition[partition][i];
            float weight = texel_weights[iwt];
            float3 texel_datum = { blk->work_data[4 * iwt],
                                   blk->work_data[4 * iwt + 1],
                                   blk->work_data[4 * iwt + 2]
                                 };
            texel_datum = (texel_datum - average) * weight;

            if (texel_datum.x > 0.0f)
                sum_xp = sum_xp + texel_datum;
            if (texel_datum.y > 0.0f)
                sum_yp = sum_yp + texel_datum;
            if (texel_datum.z > 0.0f)
                sum_zp = sum_zp + texel_datum;
        }

        float prod_xp = dot(sum_xp, sum_xp);
        float prod_yp = dot(sum_yp, sum_yp);
        float prod_zp = dot(sum_zp, sum_zp);

        float3 best_vector = sum_xp;
        float best_sum = prod_xp;
        if (prod_yp > best_sum) {
            best_vector = sum_yp;
            best_sum = prod_yp;
        }
        if (prod_zp > best_sum) {
            best_vector = sum_zp;
            best_sum = prod_zp;
        }

        directions_rgb[partition] = best_vector;
        directions_gb[partition] = best_vector.yz;
        directions_rb[partition] = best_vector.xz;
        directions_rg[partition] = best_vector.xy;
    }
}

void compute_averages_and_directions_3_components(__global partition_info * pt,
        imageblock * blk,
        error_weight_block * ewb,
        float3 * color_scalefactors, int component1, int component2, int component3, float3 * averages, float3 * directions) {
    DEBUG("compute_averages_and_directions_3_components");

    int i;
    int partition_count = pt->partition_count;
    int partition;

    float *texel_weights;
    if (component1 == 1 && component2 == 2 && component3 == 3)
        texel_weights = ewb->texel_weight_gba;
    else if (component1 == 0 && component2 == 2 && component3 == 3)
        texel_weights = ewb->texel_weight_rba;
    else if (component1 == 0 && component2 == 1 && component3 == 3)
        texel_weights = ewb->texel_weight_rga;
    else if (component1 == 0 && component2 == 1 && component3 == 2)
        texel_weights = ewb->texel_weight_rgb;
    else {
        texel_weights = ewb->texel_weight_gba;
        astc_codec_internal_error("ERROR: compute_averages_and_directions_3_components");
    }


    for (partition = 0; partition < partition_count; partition++) {
        int texelcount = pt->texels_per_partition[partition];

        float3 base_sum = { 0.0f, 0.0f, 0.0f };
        float partition_weight = 0.0f;

        for (i = 0; i < texelcount; i++) {
            int iwt = pt->texels_of_partition[partition][i];
            float weight = texel_weights[iwt];
            float3 texel_datum = { blk->work_data[4 * iwt + component1],
                                   blk->work_data[4 * iwt + component2],
                                   blk->work_data[4 * iwt + component3]
                                 };
            texel_datum = texel_datum * weight;
            partition_weight += weight;

            base_sum = base_sum + texel_datum;
        }

        float3 csf = color_scalefactors[partition];

        float3 average = base_sum * 1.0f / MAX(partition_weight, FLOAT_n7);
        averages[partition] = average * csf.xyz;


        float3 sum_xp = {0.0f, 0.0f, 0.0f };
        float3 sum_yp = {0.0f, 0.0f, 0.0f };
        float3 sum_zp = {0.0f, 0.0f, 0.0f };

        for (i = 0; i < texelcount; i++) {
            int iwt = pt->texels_of_partition[partition][i];
            float weight = texel_weights[iwt];
            float3 texel_datum = { blk->work_data[4 * iwt + component1],
                                   blk->work_data[4 * iwt + component2],
                                   blk->work_data[4 * iwt + component3]
                                 };
            texel_datum = (texel_datum - average) * weight;

            if (texel_datum.x > 0.0f)
                sum_xp = sum_xp + texel_datum;
            if (texel_datum.y > 0.0f)
                sum_yp = sum_yp + texel_datum;
            if (texel_datum.z > 0.0f)
                sum_zp = sum_zp + texel_datum;
        }

        float prod_xp = dot(sum_xp, sum_xp);
        float prod_yp = dot(sum_yp, sum_yp);
        float prod_zp = dot(sum_zp, sum_zp);

        float3 best_vector = sum_xp;
        float best_sum = prod_xp;
        if (prod_yp > best_sum) {
            best_vector = sum_yp;
            best_sum = prod_yp;
        }
        if (prod_zp > best_sum) {
            best_vector = sum_zp;
            best_sum = prod_zp;
        }

        float3 onef = { 1.0f, 1.0f,1.0f };
        if (dot(best_vector, best_vector) < FLOAT_n18)
            best_vector = onef;
        directions[partition] = best_vector;
    }

}

void compute_endpoints_and_ideal_weights_3_components(
    __global partition_info * pt,
    imageblock * blk,
    error_weight_block * ewb,
    endpoints_and_weights * ei,
    int component1,
    int component2,
    int component3,
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_endpoints_and_ideal_weights_3_components");

    int i;

    int partition_count = pt->partition_count;
    ei->ep.partition_count = partition_count;

    float4 error_weightings[4];
    float4 color_scalefactors[4];
    float3 scalefactors[4];
    float *error_weights;

    if (component1 == 1 && component2 == 2 && component3 == 3)
        error_weights = ewb->texel_weight_gba;
    else if (component1 == 0 && component2 == 2 && component3 == 3)
        error_weights = ewb->texel_weight_rba;
    else if (component1 == 0 && component2 == 1 && component3 == 3)
        error_weights = ewb->texel_weight_rga;
    else if (component1 == 0 && component2 == 1 && component3 == 2)
        error_weights = ewb->texel_weight_rgb;
    else {
        error_weights = ewb->texel_weight_gba;
        astc_codec_internal_error("ERROR: compute_endpoints_and_ideal_weights_3_components");
    }

    compute_partition_error_color_weightings(ewb, pt, error_weightings, color_scalefactors, ASTCEncode);

    for (i = 0; i < partition_count; i++) {
        float s1 = 0, s2 = 0, s3 = 0;
        switch (component1) {
        case 0:
            s1 = color_scalefactors[i].x;
            break;
        case 1:
            s1 = color_scalefactors[i].y;
            break;
        case 2:
            s1 = color_scalefactors[i].z;
            break;
        case 3:
            s1 = color_scalefactors[i].w;
            break;
        }

        switch (component2) {
        case 0:
            s2 = color_scalefactors[i].x;
            break;
        case 1:
            s2 = color_scalefactors[i].y;
            break;
        case 2:
            s2 = color_scalefactors[i].z;
            break;
        case 3:
            s2 = color_scalefactors[i].w;
            break;
        }

        switch (component3) {
        case 0:
            s3 = color_scalefactors[i].x;
            break;
        case 1:
            s3 = color_scalefactors[i].y;
            break;
        case 2:
            s3 = color_scalefactors[i].z;
            break;
        case 3:
            s3 = color_scalefactors[i].w;
            break;
        }
        float3 sf = { s1, s2, s3 };
        scalefactors[i] = normalize(sf) * 1.73205080f;
    }


    float lowparam[4], highparam[4];

    float3 averages[4];
    float3 directions[4];

    line3 lines[4];
    float scale[4];
    float length_squared[4];


    for (i = 0; i < partition_count; i++) {
        lowparam[i] = FLOAT_10;
        highparam[i] = -FLOAT_10;
    }

    compute_averages_and_directions_3_components(pt, blk, ewb, scalefactors, component1, component2, component3, averages, directions);

    float3 onef  = { 1.0f,1.0f,1.0f };
    float3 zerof = { 0.0f, 0.0f, 0.0f };

    for (i = 0; i < partition_count; i++) {
        float3 direc = directions[i];
        if (direc.x + direc.y + direc.z < 0.0f)
            directions[i] = zerof - direc;
    }

    for (i = 0; i < partition_count; i++) {
        lines[i].a = averages[i];
        if (dot(directions[i], directions[i]) == 0.0f)
            lines[i].b = normalize(onef);
        else
            lines[i].b = normalize(directions[i]);
    }


    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        if (error_weights[i] > FLOAT_n10) {
            int partition = pt->partition_of_texel[i];
            float3 point = { blk->work_data[4 * i + component1], blk->work_data[4 * i + component2], blk->work_data[4 * i + component3] };
            point = point * scalefactors[partition];
            line3 l = lines[partition];
            float param = dot(point - l.a, l.b);
            ei->weights[i] = param;
            if (param < lowparam[partition])
                lowparam[partition] = param;
            if (param > highparam[partition])
                highparam[partition] = param;
        } else {
            ei->weights[i] = -FLOAT_38;
        }
    }

    float3 lowvalues[4];
    float3 highvalues[4];


    for (i = 0; i < partition_count; i++) {
        float length = highparam[i] - lowparam[i];
        if (length < 0) {          // case for when none of the texels had any weight
            lowparam[i] = 0.0f;
            highparam[i] = FLOAT_n7;
        }

        // it is possible for a uniform-color partition to produce length=0; this
        // causes NaN-production and NaN-propagation later on. Set length to
        // a small value to avoid this problem.
        if (length < FLOAT_n7)
            length = FLOAT_n7;

        length_squared[i] = length * length;
        scale[i] = 1.0f / length;

        float3 ep0 = lines[i].a + lines[i].b * lowparam[i];
        float3 ep1 = lines[i].a + lines[i].b * highparam[i];

        ep0 = ep0 / scalefactors[i];
        ep1 = ep1 / scalefactors[i];


        lowvalues[i] = ep0;
        highvalues[i] = ep1;
    }


    for (i = 0; i < partition_count; i++) {
        float4 ep0f = { blk->red_min, blk->green_min, blk->blue_min, blk->alpha_min };
        float4 ep1f = { blk->red_max, blk->green_max, blk->blue_max, blk->alpha_max };
        ei->ep.endpt0[i] = ep0f;
        ei->ep.endpt1[i] = ep1f;

        float3 ep0 = lowvalues[i];
        float3 ep1 = highvalues[i];

        switch (component1) {
        case 0:
            ei->ep.endpt0[i].x = ep0.x;
            ei->ep.endpt1[i].x = ep1.x;
            break;
        case 1:
            ei->ep.endpt0[i].y = ep0.x;
            ei->ep.endpt1[i].y = ep1.x;
            break;
        case 2:
            ei->ep.endpt0[i].z = ep0.x;
            ei->ep.endpt1[i].z = ep1.x;
            break;
        case 3:
            ei->ep.endpt0[i].w = ep0.x;
            ei->ep.endpt1[i].w = ep1.x;
            break;
        }
        switch (component2) {
        case 0:
            ei->ep.endpt0[i].x = ep0.y;
            ei->ep.endpt1[i].x = ep1.y;
            break;
        case 1:
            ei->ep.endpt0[i].y = ep0.y;
            ei->ep.endpt1[i].y = ep1.y;
            break;
        case 2:
            ei->ep.endpt0[i].z = ep0.y;
            ei->ep.endpt1[i].z = ep1.y;
            break;
        case 3:
            ei->ep.endpt0[i].w = ep0.y;
            ei->ep.endpt1[i].w = ep1.y;
            break;
        }
        switch (component3) {
        case 0:
            ei->ep.endpt0[i].x = ep0.z;
            ei->ep.endpt1[i].x = ep1.z;
            break;
        case 1:
            ei->ep.endpt0[i].y = ep0.z;
            ei->ep.endpt1[i].y = ep1.z;
            break;
        case 2:
            ei->ep.endpt0[i].z = ep0.z;
            ei->ep.endpt1[i].z = ep1.z;
            break;
        case 3:
            ei->ep.endpt0[i].w = ep0.z;
            ei->ep.endpt1[i].w = ep1.z;
            break;
        }
    }

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        int partition = pt->partition_of_texel[i];
        float idx = (ei->weights[i] - lowparam[partition]) * scale[partition];
        if (idx > 1.0f)
            idx = 1.0f;
        else if (!(idx > 0.0f))
            idx = 0.0f;

        ei->weights[i] = idx;
        ei->weight_error_scale[i] = length_squared[partition] * error_weights[i];
        if (astc_isnan(ei->weight_error_scale[i])) {
            astc_codec_internal_error("ERROR: compute_endpoints_and_ideal_weights_3_components: 2");
        }
    }

//#ifdef __OPENCL_VERSION__
//    if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//        printf("ei->ep.endpt0[0].x %3.3f\n", ei->ep.endpt0[0].x);
//
}

int imageblock_uses_alpha1(imageblock * blk) {
    DEBUG("imageblock_uses_alpha1");
    //printf("%2.3f",pb->alpha_max);
    //printf("%2.3f", pb->alpha_min);

    return blk->alpha_max != blk->alpha_min;
}

void compute_endpoints_and_ideal_weights_1_plane(
    __global partition_info * pt,
    imageblock * blk,
    error_weight_block * ewb,
    endpoints_and_weights * ei,
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_endpoints_and_ideal_weights_1_plane");
    int uses_alpha = imageblock_uses_alpha1(blk);
    if (uses_alpha) {
        compute_endpoints_and_ideal_weights_rgba(pt, blk, ewb, ei, ASTCEncode);
    } else {
        compute_endpoints_and_ideal_weights_3_components(pt, blk, ewb, ei, 0, 1, 2, ASTCEncode);
    }
}

// function to compute angular sums; then, from the
// angular sums, compute alignment factor and offset.
void compute_angular_offsets(
    int samplecount,
    __global2 float *samples,
    __global2 float *sample_weights,
    int max_angular_steps,
    float *offsets,
    __global ASTC_Encode *ASTCEncode) {

    int i, j;

    float anglesum_x[ANGULAR_STEPS];
    float anglesum_y[ANGULAR_STEPS];

    for (i = 0; i < max_angular_steps; i++) {
        anglesum_x[i] = 0;
        anglesum_y[i] = 0;
    }


    // compute the angle-sums.
    for (i = 0; i < samplecount; i++) {
        float sample = samples[i];
        float sample_weight = sample_weights[i];
        if32 p;
        p.f = (sample * (SINCOS_STEPS - 1.0f)) + 12582912.0f;
        unsigned int isample = p.u & 0x3F;

        __global float *sinptr = ASTCEncode->sin_table[isample];
        __global float *cosptr = ASTCEncode->cos_table[isample];

        for (j = 0; j < max_angular_steps; j++) {
            float cp = cosptr[j];
            float sp = sinptr[j];

            anglesum_x[j] += cp * sample_weight;
            anglesum_y[j] += sp * sample_weight;
        }
    }

    // postprocess the angle-sums
    for (i = 0; i < max_angular_steps; i++) {
        float angle = (float)atan2(anglesum_y[i], anglesum_x[i]);    // positive angle -> positive offset
        offsets[i] = angle * (ASTCEncode->stepsizes[i] * (1.0f / (2.0f * (float)M_PI)));
    }
}

void compute_lowest_and_highest_weight(
    int samplecount,
    __global2 float *samples,
    __global2 float *sample_weights,
    int max_angular_steps,
    float *offsets,
    int8_t * lowest_weight,
    int8_t * highest_weight,
    float *error,
    float *cut_low_weight_error,
    float *cut_high_weight_error,
    __global ASTC_Encode *ASTCEncode) {
    int i;

    int sp;

    float error_from_forcing_weight_down[60];
    float error_from_forcing_weight_either_way[60];
    for (i = 0; i < 60; i++) {
        error_from_forcing_weight_down[i] = 0;
        error_from_forcing_weight_either_way[i] = 0;
    }

    for (sp = 0; sp < max_angular_steps; sp++) {
        unsigned int minidx_bias12 = 55;
        unsigned int maxidx_bias12 = 0;

        float errval = 0.0f;

        float rcp_stepsize = angular_steppings[sp];
        float offset = offsets[sp];

        float scaled_offset = rcp_stepsize * offset;


        for (i = 0; i < samplecount - 1; i += 2) {
            float wt1 = sample_weights[i];
            float wt2 = sample_weights[i + 1];
            if32 p1, p2;
            float sval1 = (samples[i] * rcp_stepsize) - scaled_offset;
            float sval2 = (samples[i + 1] * rcp_stepsize) - scaled_offset;
            p1.f = sval1 + 12582912.0f;    // FP representation abuse to avoid floor() and float->int conversion
            p2.f = sval2 + 12582912.0f;    // FP representation abuse to avoid floor() and float->int conversion
            float isval1 = p1.f - 12582912.0f;
            float isval2 = p2.f - 12582912.0f;
            float dif1 = sval1 - isval1;
            float dif2 = sval2 - isval2;

            errval += (dif1 * wt1) * dif1;
            errval += (dif2 * wt2) * dif2;

            // table lookups that really perform a minmax function.
            unsigned int idx1_bias12 = idxtab[p1.u & 0xFF];
            unsigned int idx2_bias12 = idxtab[p2.u & 0xFF];

            if (idx1_bias12 < minidx_bias12)
                minidx_bias12 = idx1_bias12;
            if (idx1_bias12 > maxidx_bias12)
                maxidx_bias12 = idx1_bias12;
            if (idx2_bias12 < minidx_bias12)
                minidx_bias12 = idx2_bias12;
            if (idx2_bias12 > maxidx_bias12)
                maxidx_bias12 = idx2_bias12;

            error_from_forcing_weight_either_way[idx1_bias12] += wt1;
            error_from_forcing_weight_down[idx1_bias12] += (dif1 * wt1);

            error_from_forcing_weight_either_way[idx2_bias12] += wt2;
            error_from_forcing_weight_down[idx2_bias12] += (dif2 * wt2);
        }

        if (samplecount & 1) {
            i = samplecount - 1;
            float wt = sample_weights[i];
            if32 p;
            float sval = (samples[i] * rcp_stepsize) - scaled_offset;
            p.f = sval + 12582912.0f;    // FP representation abuse to avoid floor() and float->int conversion
            float isval = p.f - 12582912.0f;
            float dif = sval - isval;

            errval += (dif * wt) * dif;

            unsigned int idx_bias12 = idxtab[p.u & 0xFF];

            if (idx_bias12 < minidx_bias12)
                minidx_bias12 = idx_bias12;
            if (idx_bias12 > maxidx_bias12)
                maxidx_bias12 = idx_bias12;

            error_from_forcing_weight_either_way[idx_bias12] += wt;
            error_from_forcing_weight_down[idx_bias12] += dif * wt;
        }


        lowest_weight[sp] = (int8_t)(minidx_bias12 - 12);
        highest_weight[sp] = (int8_t)(maxidx_bias12 - 12);
        error[sp] = errval;

        // the cut_(lowest/highest)_weight_error indicate the error that results from
        // forcing samples that should have had the (lowest/highest) weight value
        // one step (up/down).
        cut_low_weight_error[sp] = error_from_forcing_weight_either_way[minidx_bias12] - 2.0f * error_from_forcing_weight_down[minidx_bias12];
        cut_high_weight_error[sp] = error_from_forcing_weight_either_way[maxidx_bias12] + 2.0f * error_from_forcing_weight_down[maxidx_bias12];

        // clear out the error-from-forcing values we actually used in this pass
        // so that these are clean for the next pass.
        unsigned int ui;
        for (ui = minidx_bias12 & ~0x3; ui <= maxidx_bias12; ui += 4) {
            error_from_forcing_weight_either_way[ui] = 0;
            error_from_forcing_weight_down[ui] = 0;
            error_from_forcing_weight_either_way[ui + 1] = 0;
            error_from_forcing_weight_down[ui + 1] = 0;
            error_from_forcing_weight_either_way[ui + 2] = 0;
            error_from_forcing_weight_down[ui + 2] = 0;
            error_from_forcing_weight_either_way[ui + 3] = 0;
            error_from_forcing_weight_down[ui + 3] = 0;
        }
    }


    for (sp = 0; sp < max_angular_steps; sp++) {
        float errscale = ASTCEncode->stepsizes_sqr[sp];
        error[sp] *= errscale;
        cut_low_weight_error[sp] *= errscale;
        cut_high_weight_error[sp] *= errscale;
    }
}

void compute_angular_endpoints_for_quantization_levels(
    int samplecount,
    __global2 float *samples,
    __global2 float *sample_weights,
    int max_quantization_level,
    float low_value[12],
    float high_value[12],
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_angular_endpoints_for_quantization_levels");

    int i;


    max_quantization_level++;    // Temporarily increase level - needs refinement

    int quantization_steps_for_level[13] = { 2, 3, 4, 5, 6, 8, 10, 12, 16, 20, 24, 33, 36 };
    int max_quantization_steps = quantization_steps_for_level[max_quantization_level];

    float offsets[ANGULAR_STEPS];

    int max_angular_steps = ASTCEncode->max_angular_steps_needed_for_quant_level[max_quantization_level];

    compute_angular_offsets(samplecount, samples, sample_weights, max_angular_steps, offsets, ASTCEncode);


    // the +4 offsets are to allow for vectorization within compute_lowest_and_highest_weight().
    int8_t lowest_weight[ANGULAR_STEPS + 4];
    int8_t highest_weight[ANGULAR_STEPS + 4];
    float error[ANGULAR_STEPS + 4];

    float cut_low_weight_error[ANGULAR_STEPS + 4];
    float cut_high_weight_error[ANGULAR_STEPS + 4];

    compute_lowest_and_highest_weight(samplecount, samples, sample_weights, max_angular_steps, offsets,
                                      lowest_weight, highest_weight, error, cut_low_weight_error, cut_high_weight_error,ASTCEncode);

    // for each quantization level, find the best error terms.
    float best_errors[40];
    int best_scale[40];
    uint8_t cut_low_weight[40];

    for (i = 0; i < (max_quantization_steps + 4); i++) {
        best_errors[i] = FLOAT_30;
        best_scale[i] = -1;    // Indicates no solution found
        cut_low_weight[i] = 0;
    }

    for (i = 0; i < max_angular_steps; i++) {
        int samplecount1 = highest_weight[i] - lowest_weight[i] + 1;
        if (samplecount1 >= (max_quantization_steps + 4)) {
            continue;
        }
        if (samplecount1 < 2)
            samplecount1 = 2;

        if (best_errors[samplecount1] > error[i]) {
            best_errors[samplecount1] = error[i];
            best_scale[samplecount1] = i;
            cut_low_weight[samplecount1] = 0;
        }

        float error_cut_low = error[i] + cut_low_weight_error[i];
        float error_cut_high = error[i] + cut_high_weight_error[i];
        float error_cut_low_high = error[i] + cut_low_weight_error[i] + cut_high_weight_error[i];

        if (best_errors[samplecount1 - 1] > error_cut_low) {
            best_errors[samplecount1 - 1] = error_cut_low;
            best_scale[samplecount1 - 1] = i;
            cut_low_weight[samplecount1 - 1] = 1;
        }

        if (best_errors[samplecount1 - 1] > error_cut_high) {
            best_errors[samplecount1 - 1] = error_cut_high;
            best_scale[samplecount1 - 1] = i;
            cut_low_weight[samplecount1 - 1] = 0;
        }

        if (best_errors[samplecount1 - 2] > error_cut_low_high) {
            best_errors[samplecount1 - 2] = error_cut_low_high;
            best_scale[samplecount1 - 2] = i;
            cut_low_weight[samplecount1 - 2] = 1;
        }

    }

    // if we got a better error-value for a low samplecount than for a high one,
    // use the low-samplecount error value for the higher samplecount as well.
    for (i = 3; i <= max_quantization_steps; i++) {
        if (best_errors[i] > best_errors[i - 1]) {
            best_errors[i] = best_errors[i - 1];
            best_scale[i] = best_scale[i - 1];
            cut_low_weight[i] = cut_low_weight[i - 1];
        }
    }


    max_quantization_level--;    // Decrease level again (see corresponding ++, above)

    int ql_weights[12] = { 2, 3, 4, 5, 6, 8, 10, 12, 16, 20, 24, 33 };
    for (i = 0; i <= max_quantization_level; i++) {
        int q = ql_weights[i];
        int bsi = best_scale[q];

        // Did we find anything?
        if(bsi < 0) {
            printf("ERROR: Unable to find an encoding within the specified error limits. Please revise the error limit values and try again.\n");
            // exit(1);
        }

        float stepsize = ASTCEncode->stepsizes[bsi];
        int lwi = lowest_weight[bsi] + cut_low_weight[q];
        int hwi = lwi + q - 1;
        float offset = offsets[bsi];

        low_value[i] = offset + lwi * stepsize;
        high_value[i] = offset + hwi * stepsize;
    }

}

float compute_value_of_texel_flt(int texel_to_get,
                                 __global decimation_table *it,
                                 __global2 float *weights) {
    return
        ( weights[it->texel_weights[texel_to_get][0]] * it->texel_weights_float[texel_to_get][0]
          +  weights[it->texel_weights[texel_to_get][1]] * it->texel_weights_float[texel_to_get][1])
        + (weights[it->texel_weights[texel_to_get][2]] * it->texel_weights_float[texel_to_get][2]
           +  weights[it->texel_weights[texel_to_get][3]] * it->texel_weights_float[texel_to_get][3]);
}

float compute_value_of_texel_flt_localVar(
    int texel_to_get,
    __global decimation_table *it,
    float *weights) {

//#ifdef __OPENCL_VERSION__
//    if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//        printf("flt_localVar %d %d %3.3f %3.3f \n",texel_to_get, it->texel_weights[texel_to_get][0],     weights[it->texel_weights[texel_to_get][0]],    it->texel_weights_float[texel_to_get][0]);

    return
        (weights[it->texel_weights[texel_to_get][0]] * it->texel_weights_float[texel_to_get][0]
         +  weights[it->texel_weights[texel_to_get][1]] * it->texel_weights_float[texel_to_get][1])
        + (weights[it->texel_weights[texel_to_get][2]] * it->texel_weights_float[texel_to_get][2]
           +  weights[it->texel_weights[texel_to_get][3]] * it->texel_weights_float[texel_to_get][3]);
}

//inline
float compute_error_of_texel(
    endpoints_and_weights * eai,
    int texel_to_get,
    __global decimation_table * it,
    __global2 float *weights) {
    float current_value = compute_value_of_texel_flt(texel_to_get, it, weights);
    float valuedif = current_value - eai->weights[texel_to_get];
    return valuedif * valuedif * eai->weight_error_scale[texel_to_get];
}

float compute_error_of_weight_set(
    endpoints_and_weights * eai,
    __global decimation_table * it,
    __global2 float *weights) {
    int i;
    int texel_count = it->num_texels;
    float error_summa = 0.0;
    for (i = 0; i < texel_count; i++)
        error_summa += compute_error_of_texel(eai, i, it, weights);
    return error_summa;
}

void compute_two_error_changes_from_perturbing_weight_infill(
    endpoints_and_weights * eai,
    __global decimation_table * it,
    float *infilled_weights, int weight_to_perturb,
    float perturbation1, float perturbation2, float *res1, float *res2) {
    int num_weights = it->weight_num_texels[weight_to_perturb];
    float error_change0 = 0.0f;
    float error_change1 = 0.0f;
    int i;

    for (i = num_weights - 1; i >= 0; i--) {
        uint8_t weight_texel = it->weight_texel[weight_to_perturb][i];
        float weights = it->weights_flt[weight_to_perturb][i];

        float scale = eai->weight_error_scale[weight_texel] * weights;
        float old_weight = infilled_weights[weight_texel];
        float ideal_weight = eai->weights[weight_texel];

        error_change0 += weights * scale;
        error_change1 += (old_weight - ideal_weight) * scale;
    }
    *res1 = error_change0 * (perturbation1 * perturbation1 * (1.0f / (TEXEL_WEIGHT_SUM * TEXEL_WEIGHT_SUM))) + error_change1 * (perturbation1 * (2.0f / TEXEL_WEIGHT_SUM));
    *res2 = error_change0 * (perturbation2 * perturbation2 * (1.0f / (TEXEL_WEIGHT_SUM * TEXEL_WEIGHT_SUM))) + error_change1 * (perturbation2 * (2.0f / TEXEL_WEIGHT_SUM));
}

void compute_ideal_weights_for_decimation_table(
    endpoints_and_weights * eai,
    __global decimation_table * it,
    __global2 float *weight_set,
    __global2 float *weights) {
    DEBUG("compute_ideal_weights_for_decimation_table");

    int i, j, k;
    int texels_per_block = it->num_texels;
    int weight_count = it->num_weights;

    // perform a shortcut in the case of a complete decimation table
    if (texels_per_block == weight_count) {

        for (i = 0; i < it->num_texels; i++) {
            int texel = it->weight_texel[i][0];
            weight_set[i] = eai->weights[texel];
            weights[i] = eai->weight_error_scale[texel];
        }
        return;
    }


    // if the shortcut is not available, we will instead compute a simple estimate
    // and perform three rounds of refinement on that estimate.

    // float initial_weight_set[MAX_WEIGHTS_PER_BLOCK];
    float infilled_weights[MAX_TEXELS_PER_BLOCK];

    // compute an initial average for each weight.
    for (i = 0; i < weight_count; i++) {
        int texel_count = it->weight_num_texels[i];

        float weight_weight = FLOAT_n10;    // to avoid 0/0 later on
        float initial_weight = 0.0f;
        for (j = 0; j < texel_count; j++) {
            int texel = it->weight_texel[i][j];
            float weight = it->weights_flt[i][j];
            float contrib_weight = weight * eai->weight_error_scale[texel];
            weight_weight += contrib_weight;
            initial_weight += eai->weights[texel] * contrib_weight;
        }

        weights[i] = weight_weight;
        weight_set[i] = initial_weight / weight_weight;    // this is the 0/0 that is to be avoided.
    }


    for (i = 0; i < texels_per_block; i++) {
        infilled_weights[i] = compute_value_of_texel_flt(i, it, weight_set);
    }

    float stepsizes[3] = { 0.25f, 0.125f, 0.0625f };

    for (j = 0; j < 2; j++) {
        float stepsize = stepsizes[j];

        for (i = 0; i < weight_count; i++) {
            float weight_val = weight_set[i];
            float error_change_up, error_change_down;
            compute_two_error_changes_from_perturbing_weight_infill(eai, it, infilled_weights, i, stepsize, -stepsize, &error_change_up, &error_change_down);

            /*
                assume that the error-change function behaves like a quadratic function in the interval examined,
                with "error_change_up" and "error_change_down" defining the function at the endpoints
                of the interval. Then, find the position where the function's derivative is zero.

                The "fabs(b) >= a" check tests several conditions in one:
                    if a is negative, then the 2nd derivative fo the function is negative;
                    in this case, f'(x)=0 will maximize error.
                If fabs(b) > fabs(a), then f'(x)=0 will lie outside the interval altogether.
                If a and b are both 0, then set step to 0;
                    otherwise, we end up computing 0/0, which produces a lethal NaN.
                We can get an a=b=0 situation if an error weight is 0 in the wrong place.
            */

            float step;
            float a = (error_change_up + error_change_down) * 2.0f;
            float b = error_change_down - error_change_up;
            if (fabs(b) >= a) {
                if (a <= 0.0f) {
                    if (error_change_up < error_change_down)
                        step = 1;
                    else if (error_change_up > error_change_down)
                        step = -1;

                    else
                        step = 0;

                } else {
                    if (a < FLOAT_n10)
                        a = FLOAT_n10;
                    step = b / a;
                    if (step < -1.0f)
                        step = -1.0f;
                    else if (step > 1.0f)
                        step = 1.0f;
                }
            } else
                step = b / a;


            step *= stepsize;
            float new_weight_val = weight_val + step;

            // update the weight
            weight_set[i] = new_weight_val;
            // update the infilled-weights
            int num_weights = it->weight_num_texels[i];
            float perturbation = (new_weight_val - weight_val) * (1.0f / TEXEL_WEIGHT_SUM);

            for (k = num_weights - 1; k >= 0; k--) {
                uint8_t weight_texel = it->weight_texel[i][k];
                float weight_weight  = it->weights_flt[i][k];
                infilled_weights[weight_texel] += perturbation * weight_weight;
            }

        }

    }


    return;
}

void compute_angular_endpoints_1plane(float mode_cutoff,
                                      __global2 float *decimated_quantized_weights,
                                      __global2 float *decimated_weights,
                                      float low_value[MAX_WEIGHT_MODES],
                                      float high_value[MAX_WEIGHT_MODES],
                                      __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_angular_endpoints_1plane");

    int i;
    float low_values[MAX_DECIMATION_MODES][12];
    float high_values[MAX_DECIMATION_MODES][12];

    for (i = 0; i < MAX_DECIMATION_MODES; i++) {
        int samplecount     = ASTCEncode->bsd.decimation_mode_samples[i];
        int quant_mode      = ASTCEncode->bsd.decimation_mode_maxprec_1plane[i];
        float percentile    = ASTCEncode->bsd.decimation_mode_percentile[i];
        int permit_encode   = ASTCEncode->bsd.permit_encode[i];
        if (permit_encode == 0 || samplecount < 1 || quant_mode < 0 || percentile > mode_cutoff)
            continue;

        compute_angular_endpoints_for_quantization_levels(samplecount,
                decimated_quantized_weights + i * MAX_WEIGHTS_PER_BLOCK,
                decimated_weights + i * MAX_WEIGHTS_PER_BLOCK, quant_mode,
                low_values[i], high_values[i],ASTCEncode);
    }

    for (i = 0; i < MAX_WEIGHT_MODES; i++) {
        if (ASTCEncode->bsd.block_modes[i].is_dual_plane != 0 || ASTCEncode->bsd.block_modes[i].percentile > mode_cutoff)
            continue;
        int quant_mode = ASTCEncode->bsd.block_modes[i].quantization_mode;
        int decim_mode = ASTCEncode->bsd.block_modes[i].decimation_mode;

        low_value[i] = low_values[decim_mode][quant_mode];
        high_value[i] = high_values[decim_mode][quant_mode];
    }

}

void compute_ideal_quantized_weights_for_decimation_table(
    endpoints_and_weights * eai,
    __global decimation_table * it,
    float low_bound, float high_bound,
    __global2 float *weight_set_in,
    __global2 float *weight_set_out,
    __global2 uint8_t * quantized_weight_set,
    int quantization_level) {
    DEBUG("compute_ideal_quantized_weights_for_decimation_table");
    int i;
    int weight_count = it->num_weights;
    int texels_per_block = it->num_texels;

    __constant quantization_and_transfer_table *qat = &(quant_and_xfer_tables[quantization_level]);

    // quantize the weight set using both the specified low/high bounds and the
    // standard 0..1 weight bounds.

    /*
       WTF issue that we need to examine some time
    */

    if (!((high_bound - low_bound) > 0.5f)) {
        low_bound = 0.0f;
        high_bound = 1.0f;
    }

    float rscale = high_bound - low_bound;
    float scale = 1.0f / rscale;

    // rescale the weights so that
    // low_bound -> 0
    // high_bound -> 1
    // OK: first, subtract low_bound, then divide by (high_bound - low_bound)

    for (i = 0; i < weight_count; i++)
        weight_set_out[i] = (weight_set_in[i] - low_bound) * scale;



    float quantization_cutoff = quantization_step_table[quantization_level] * 0.333f;


    int is_perturbable[MAX_WEIGHTS_PER_BLOCK];
    int perturbable_count = 0;

    // quantize the weight set
    for (i = 0; i < weight_count; i++) {
        float ix0 = weight_set_out[i];
        if (ix0 < 0.0f)
            ix0 = 0.0f;
        if (ix0 > 1.0f)
            ix0 = 1.0f;
        float ix = ix0;

        ix *= 1024.0f;
        int ix2 = (int)floor(ix + 0.5f);
        int weight = qat->closest_quantized_weight[ix2];

        ix = qat->unquantized_value_flt[weight];
        weight_set_out[i] = ix;
        quantized_weight_set[i] = (uint8_t)weight;

        // test whether the error of the weight is greater than 1/3 of the weight spacing;
        // if it is not, then it is flagged as "not perturbable". This causes a
        // quality loss of about 0.002 dB, which is totally worth the speedup we're getting.
        is_perturbable[i] = 0;
        if (fabs(ix - ix0) > quantization_cutoff) {
            is_perturbable[i] = 1;
            perturbable_count++;
        }
    }



    // if the decimation table is complete, the quantization above was all we needed to do,
    // so we can early-out.
    if (it->num_weights == it->num_texels) {
        // invert the weight-scaling that was done initially
        // 0 -> low_bound
        // 1 -> high_bound

        rscale = high_bound - low_bound;
        for (i = 0; i < weight_count; i++)
            weight_set_out[i] = (weight_set_out[i] * rscale) + low_bound;

        return;
    }


    int weights_tested = 0;

    // if no weights are flagged as perturbable, don't try to perturb them.
    // if only one weight is flagged as perturbable, perturbation is also pointless.
    if (perturbable_count > 1) {
        endpoints_and_weights eaix;
        for (i = 0; i < texels_per_block; i++) {
            eaix.weights[i] = (eai->weights[i] - low_bound) * scale;
            eaix.weight_error_scale[i] = eai->weight_error_scale[i];
        }

        float infilled_weights[MAX_TEXELS_PER_BLOCK];
        for (i = 0; i < texels_per_block; i++)
            infilled_weights[i] = compute_value_of_texel_flt(i, it, weight_set_out);

        int weight_to_perturb = 0;
        int weights_since_last_perturbation = 0;
        int num_weights = it->num_weights;

        while (weights_since_last_perturbation < num_weights && weights_tested < num_weights * 4) {
            int do_quant_mod = 0;
            if (is_perturbable[weight_to_perturb]) {

                int weight_val = quantized_weight_set[weight_to_perturb];
                int weight_next_up = qat->next_quantized_value[weight_val];
                int weight_next_down = qat->prev_quantized_value[weight_val];
                float flt_weight_val = qat->unquantized_value_flt[weight_val];
                float flt_weight_next_up = qat->unquantized_value_flt[weight_next_up];
                float flt_weight_next_down = qat->unquantized_value_flt[weight_next_down];


                do_quant_mod = 0;

                float error_change_up, error_change_down;

                // compute the error change from perturbing the weight either up or down.
                compute_two_error_changes_from_perturbing_weight_infill(&eaix,
                        it,
                        infilled_weights,
                        weight_to_perturb,
                        (flt_weight_next_up - flt_weight_val), (flt_weight_next_down - flt_weight_val), &error_change_up, &error_change_down);

                int new_weight_val = 0;
                float flt_new_weight_val = 0;
                if (weight_val != weight_next_up && error_change_up < 0.0f) {
                    do_quant_mod = 1;
                    new_weight_val = weight_next_up;
                    flt_new_weight_val = flt_weight_next_up;
                } else if (weight_val != weight_next_down && error_change_down < 0.0f) {
                    do_quant_mod = 1;
                    new_weight_val = weight_next_down;
                    flt_new_weight_val = flt_weight_next_down;
                }


                if (do_quant_mod) {

                    // update the weight.
                    weight_set_out[weight_to_perturb] = flt_new_weight_val;
                    quantized_weight_set[weight_to_perturb] = (uint8_t)new_weight_val;

                    // update the infilled-weights
                    int num_weights1 = it->weight_num_texels[weight_to_perturb];
                    float perturbation = (flt_new_weight_val - flt_weight_val) * (1.0f / TEXEL_WEIGHT_SUM);
                    for (i = num_weights1 - 1; i >= 0; i--) {
                        uint8_t weight_texel = it->weight_texel[weight_to_perturb][i];
                        float weights = it->weights_flt[weight_to_perturb][i];
                        infilled_weights[weight_texel] += perturbation * weights;
                    }

                }
            }

            if (do_quant_mod)
                weights_since_last_perturbation = 0;
            else
                weights_since_last_perturbation++;

            weight_to_perturb++;
            if (weight_to_perturb >= num_weights)
                weight_to_perturb -= num_weights;

            weights_tested++;
        }
    }

    // invert the weight-scaling that was done initially
    // 0 -> low_bound
    // 1 -> high_bound


    for (i = 0; i < weight_count; i++)
        weight_set_out[i] = (weight_set_out[i] * rscale) + low_bound;
}

mat4t invertMat4t(mat4t p) {
    // cross products between the bottom two rows
    float3 bpc0 = cross(p.v[2].yzw, p.v[3].yzw);
    float3 bpc1 = cross(p.v[2].xzw, p.v[3].xzw);
    float3 bpc2 = cross(p.v[2].xyw, p.v[3].xyw);
    float3 bpc3 = cross(p.v[2].xyz, p.v[3].xyz);

    // dot-products for the top rows
    float4 row1 = { dot(bpc0, p.v[1].yzw),
                    -dot(bpc1, p.v[1].xzw),
                    dot(bpc2, p.v[1].xyw),
                    -dot(bpc3, p.v[1].xyz)
                  };

    float det = dot(p.v[0], row1);
    float rdet = 1.0f / det;

    mat4t res;

    float3 tpc0 = cross(p.v[0].yzw, p.v[1].yzw);
    float4 f4f = { row1.x, -dot(bpc0, p.v[0].yzw), dot(tpc0, p.v[3].yzw), -dot(tpc0, p.v[2].yzw) };
    res.v[0] = f4f * rdet;

    float3 tpc1 = cross(p.v[0].xzw, p.v[1].xzw);
    float4 f4f1 = { row1.y, dot(bpc1, p.v[0].xzw), -dot(tpc1, p.v[3].xzw), dot(tpc1, p.v[2].xzw) };
    res.v[1] = f4f1 * rdet;
    float3 tpc2 = cross(p.v[0].xyw, p.v[1].xyw);

    float4 f4f2 = { row1.z, -dot(bpc2, p.v[0].xyw), dot(tpc2, p.v[3].xyw), -dot(tpc2, p.v[2].xyw) };
    res.v[2] = f4f2 * rdet;

    float3 tpc3 = cross(p.v[0].xyz, p.v[1].xyz);
    float4 f4f3 = { row1.w, dot(bpc3, p.v[0].xyz), -dot(tpc3, p.v[3].xyz), dot(tpc3, p.v[2].xyz) };
    res.v[3] = f4f3 * rdet;


    return res;
}

float4 transform(mat4t p, float4 q) {
    float4 tfm = { dot(p.v[0], q), dot(p.v[1], q), dot(p.v[2], q), dot(p.v[3], q) };
    return tfm;
}

float determinant(mat2t p) {
    float2 v = p.v[0].xy * p.v[1].yx;
    return v.x - v.y;
}

//static inline
float mat_square_sum(mat2t p) {
    float a = p.v[0].x;
    float b = p.v[0].y;
    float c = p.v[1].x;
    float d = p.v[1].y;
    return a * a + b * b + c * c + d * d;
}

mat2t invertMat2t(mat2t p) {
    float rdet = 1.0f / determinant(p);
    mat2t res;
    float2 p0 = { p.v[1].y, -p.v[0].y };
    float2 p1 = { -p.v[1].x, p.v[0].x };
    res.v[0] = p0 * rdet;
    res.v[1] = p1 * rdet;
    return res;
}

void recompute_ideal_colors(int weight_quantization_mode,
                            endpoints * ep,                     // contains the endpoints we wish to update
                            float4 * rgbs_vectors,              // used to return RGBS-vectors. (endpoint mode #6)
                            float4 * rgbo_vectors,              // used to return RGBO-vectors. (endpoint mode #7)
                            float2 * lum_vectors,               // used to return luminance-vectors.
                            __global2 uint8_t * weight_set8,     // the current set of weight values
                            __global2 uint8_t * plane2_weight_set8,    // 0 if plane 2 is not actually used.
                            int plane2_color_component,    // color component for 2nd plane of weights; -1 if the 2nd plane of weights is not present
                            __global partition_info * pi,
                            __global decimation_table * it,
                            imageblock * blk,    // picture-block containing the actual data.
                            error_weight_block * ewb,
                            __global ASTC_Encode *ASTCEncode
                           ) {
    DEBUG("recompute_ideal_colors");

    int i, j;
    __constant quantization_and_transfer_table *qat = &(quant_and_xfer_tables[weight_quantization_mode]);

    float weight_set[MAX_WEIGHTS_PER_BLOCK];
    float plane2_weight_set[MAX_WEIGHTS_PER_BLOCK];

    for (i = 0; i < it->num_weights; i++) {

//#ifdef __OPENCL_VERSION__
//         if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//             printf("weight_set8[%d] = %d\n", i, weight_set8[i]);

        weight_set[i] = qat->unquantized_value_flt[weight_set8[i]];
    }
    if (plane2_weight_set8) {

//#ifdef __OPENCL_VERSION__
//         if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//             printf("plane2_weight_set8\n");

        for (i = 0; i < it->num_weights; i++) {
//#ifdef __OPENCL_VERSION__
//             if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//                 printf("plane2_weight_set8[%d] = %d\n", i, plane2_weight_set8[i]);
            plane2_weight_set[i] = qat->unquantized_value_flt[plane2_weight_set8[i]];
        }
    }

    int partition_count = pi->partition_count;

//#ifdef __OPENCL_VERSION__
//     if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//         printf("Low  <%g %g %g %g>\n", ep->endpt0[0].x, ep->endpt0[0].y, ep->endpt0[0].z, ep->endpt0[0].w);


    mat2t pmat1_red[4], pmat1_green[4], pmat1_blue[4], pmat1_alpha[4], pmat1_lum[4], pmat1_scale[4];    // matrices for plane of weights 1
    mat2t pmat2_red[4], pmat2_green[4], pmat2_blue[4], pmat2_alpha[4];    // matrices for plane of weights 2
    float2 red_vec[4];
    float2 green_vec[4];
    float2 blue_vec[4];
    float2 alpha_vec[4];
    float2 lum_vec[4];
    float2 scale_vec[4];
    float2 zerof = {0.0f, 0.0f};

    for (i = 0; i < partition_count; i++) {
        for (j = 0; j < 2; j++) {
            pmat1_red[i].v[j]   = zerof;
            pmat2_red[i].v[j]   = zerof;
            pmat1_green[i].v[j] = zerof;
            pmat2_green[i].v[j] = zerof;
            pmat1_blue[i].v[j]  = zerof;
            pmat2_blue[i].v[j]  = zerof;
            pmat1_alpha[i].v[j] = zerof;
            pmat2_alpha[i].v[j] = zerof;
            pmat1_lum[i].v[j]   = zerof;
            pmat1_scale[i].v[j] = zerof;
        }
        red_vec[i]   = zerof;
        green_vec[i] = zerof;
        blue_vec[i]  = zerof;
        alpha_vec[i] = zerof;
        lum_vec[i]   = zerof;
        scale_vec[i] = zerof;
    }


    float wmin1[4], wmax1[4];
    float wmin2[4], wmax2[4];
    float red_weight_sum[4];
    float green_weight_sum[4];
    float blue_weight_sum[4];
    float alpha_weight_sum[4];
    float lum_weight_sum[4];
    float scale_weight_sum[4];

    float red_weight_weight_sum[4];
    float green_weight_weight_sum[4];
    float blue_weight_weight_sum[4];

    float psum[4];                // sum of (weight * qweight^2) across (red,green,blue)
    float qsum[4];                // sum of (weight * qweight * texelval) across (red,green,blue)


    for (i = 0; i < partition_count; i++) {
        wmin1[i] = 1.0f;
        wmax1[i] = 0.0f;
        wmin2[i] = 1.0f;
        wmax2[i] = 0.0f;
        red_weight_sum[i] = FLOAT_n17;
        green_weight_sum[i] = FLOAT_n17;
        blue_weight_sum[i] = FLOAT_n17;
        alpha_weight_sum[i] = FLOAT_n17;

        lum_weight_sum[i] = FLOAT_n17;
        scale_weight_sum[i] = FLOAT_n17;

        red_weight_weight_sum[i] = FLOAT_n17;
        green_weight_weight_sum[i] = FLOAT_n17;
        blue_weight_weight_sum[i] = FLOAT_n17;

        psum[i] = FLOAT_n17;
        qsum[i] = FLOAT_n17;
    }


    // for each partition, compute the direction that an RGB-scale color endpoint pair would have.
    float3 rgb_sum[4];
    float3 rgb_weight_sum[4];
    float3 scale_directions[4];
    float scale_min[4];
    float scale_max[4];
    float lum_min[4];
    float lum_max[4];
    float3 one17f = { FLOAT_n17, FLOAT_n17, FLOAT_n17 };

    for (i = 0; i < partition_count; i++) {
        rgb_sum[i]        = one17f;
        rgb_weight_sum[i] = one17f;
    }


    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        float3 rgb = { blk->work_data[4 * i], blk->work_data[4 * i + 1], blk->work_data[4 * i + 2]};
        float3 rgb_weight = {ewb->texel_weight_r[i],
                             ewb->texel_weight_g[i],
                             ewb->texel_weight_b[i]
                            };

        int part = pi->partition_of_texel[i];
        rgb_sum[part] = rgb_sum[part] + (rgb * rgb_weight);
        rgb_weight_sum[part] = rgb_weight_sum[part] + rgb_weight;
    }

    for (i = 0; i < partition_count; i++) {
        scale_directions[i] = normalize(rgb_sum[i] / rgb_weight_sum[i]);
        scale_max[i] = 0.0f;
        scale_min[i] = FLOAT_10;
        lum_max[i] = 0.0f;
        lum_min[i] = FLOAT_10;
    }

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        float r = blk->work_data[4 * i];
        float g = blk->work_data[4 * i + 1];
        float b = blk->work_data[4 * i + 2];
        float a = blk->work_data[4 * i + 3];

        int part = pi->partition_of_texel[i];
        float idx0 = it ? compute_value_of_texel_flt_localVar(i, it, weight_set) : weight_set[i];

//#ifdef __OPENCL_VERSION__
//         if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//             printf("i= %d om_idx0 %3.3f weight_set[i] %3.3f\n",i, idx0, weight_set[i]);


        float om_idx0 = 1.0f - idx0;

        if (idx0 > wmax1[part])
            wmax1[part] = idx0;
        if (idx0 < wmin1[part])
            wmin1[part] = idx0;

        float red_weight = ewb->texel_weight_r[i];
        float green_weight = ewb->texel_weight_g[i];
        float blue_weight = ewb->texel_weight_b[i];
        float alpha_weight = ewb->texel_weight_a[i];

        float lum_weight = (red_weight + green_weight + blue_weight);
        float scale_weight = lum_weight;

        float lum = (r * red_weight + g * green_weight + b * blue_weight) / lum_weight;
        float3 scale_direction = scale_directions[part];
        float3 rgbf = {r, g, b};
        float scale = dot(scale_direction, rgbf);
        if (lum < lum_min[part])
            lum_min[part] = scale;
        if (lum > lum_max[part])
            lum_max[part] = scale;
        if (scale < scale_min[part])
            scale_min[part] = scale;
        if (scale > scale_max[part])
            scale_max[part] = scale;


        red_weight_sum[part] += red_weight;
        green_weight_sum[part] += green_weight;
        blue_weight_sum[part] += blue_weight;
        alpha_weight_sum[part] += alpha_weight;
        lum_weight_sum[part] += lum_weight;
        scale_weight_sum[part] += scale_weight;



        pmat1_red[part].v[0].x += om_idx0 * om_idx0 * red_weight;
        pmat1_red[part].v[0].y += idx0 * om_idx0 * red_weight;
        pmat1_red[part].v[1].x += idx0 * om_idx0 * red_weight;
        pmat1_red[part].v[1].y += idx0 * idx0 * red_weight;

        pmat1_green[part].v[0].x += om_idx0 * om_idx0 * green_weight;
        pmat1_green[part].v[0].y += idx0 * om_idx0 * green_weight;
        pmat1_green[part].v[1].x += idx0 * om_idx0 * green_weight;
        pmat1_green[part].v[1].y += idx0 * idx0 * green_weight;

        pmat1_blue[part].v[0].x += om_idx0 * om_idx0 * blue_weight;
        pmat1_blue[part].v[0].y += idx0 * om_idx0 * blue_weight;
        pmat1_blue[part].v[1].x += idx0 * om_idx0 * blue_weight;
        pmat1_blue[part].v[1].y += idx0 * idx0 * blue_weight;

        pmat1_alpha[part].v[0].x += om_idx0 * om_idx0 * alpha_weight;
        pmat1_alpha[part].v[0].y += idx0 * om_idx0 * alpha_weight;
        pmat1_alpha[part].v[1].x += idx0 * om_idx0 * alpha_weight;
        pmat1_alpha[part].v[1].y += idx0 * idx0 * alpha_weight;

        pmat1_lum[part].v[0].x += om_idx0 * om_idx0 * lum_weight;
        pmat1_lum[part].v[0].y += idx0 * om_idx0 * lum_weight;
        pmat1_lum[part].v[1].x += idx0 * om_idx0 * lum_weight;
        pmat1_lum[part].v[1].y += idx0 * idx0 * lum_weight;

        pmat1_scale[part].v[0].x += om_idx0 * om_idx0 * scale_weight;
        pmat1_scale[part].v[0].y += idx0 * om_idx0 * scale_weight;
        pmat1_scale[part].v[1].x += idx0 * om_idx0 * scale_weight;
        pmat1_scale[part].v[1].y += idx0 * idx0 * scale_weight;

        float idx1 = 0.0f, om_idx1 = 0.0f;
        if (plane2_weight_set8) {
            idx1 = it ? compute_value_of_texel_flt_localVar(i, it, plane2_weight_set) : plane2_weight_set[i];
            om_idx1 = 1.0f - idx1;
            if (idx1 > wmax2[part])
                wmax2[part] = idx1;
            if (idx1 < wmin2[part])
                wmin2[part] = idx1;

            pmat2_red[part].v[0].x += om_idx1 * om_idx1 * red_weight;
            pmat2_red[part].v[0].y += idx1 * om_idx1 * red_weight;
            pmat2_red[part].v[1].x += idx1 * om_idx1 * red_weight;
            pmat2_red[part].v[1].y += idx1 * idx1 * red_weight;

            pmat2_green[part].v[0].x += om_idx1 * om_idx1 * green_weight;
            pmat2_green[part].v[0].y += idx1 * om_idx1 * green_weight;
            pmat2_green[part].v[1].x += idx1 * om_idx1 * green_weight;
            pmat2_green[part].v[1].y += idx1 * idx1 * green_weight;

            pmat2_blue[part].v[0].x += om_idx1 * om_idx1 * blue_weight;
            pmat2_blue[part].v[0].y += idx1 * om_idx1 * blue_weight;
            pmat2_blue[part].v[1].x += idx1 * om_idx1 * blue_weight;
            pmat2_blue[part].v[1].y += idx1 * idx1 * blue_weight;

            pmat2_alpha[part].v[0].x += om_idx1 * om_idx1 * alpha_weight;
            pmat2_alpha[part].v[0].y += idx1 * om_idx1 * alpha_weight;
            pmat2_alpha[part].v[1].x += idx1 * om_idx1 * alpha_weight;
            pmat2_alpha[part].v[1].y += idx1 * idx1 * alpha_weight;
        }

        float red_idx = (plane2_color_component == 0) ? idx1 : idx0;
        float green_idx = (plane2_color_component == 1) ? idx1 : idx0;
        float blue_idx = (plane2_color_component == 2) ? idx1 : idx0;
        float alpha_idx = (plane2_color_component == 3) ? idx1 : idx0;


        red_vec[part].x += (red_weight * r) * (1.0f - red_idx);
        green_vec[part].x += (green_weight * g) * (1.0f - green_idx);
        blue_vec[part].x += (blue_weight * b) * (1.0f - blue_idx);
        alpha_vec[part].x += (alpha_weight * a) * (1.0f - alpha_idx);
        lum_vec[part].x += (lum_weight * lum) * om_idx0;
        scale_vec[part].x += (scale_weight * scale) * om_idx0;

        red_vec[part].y += (red_weight * r) * red_idx;
        green_vec[part].y += (green_weight * g) * green_idx;
        blue_vec[part].y += (blue_weight * b) * blue_idx;
        alpha_vec[part].y += (alpha_weight * a) * alpha_idx;
        lum_vec[part].y += (lum_weight * lum) * idx0;
        scale_vec[part].y += (scale_weight * scale) * idx0;

        red_weight_weight_sum[part] += red_weight * red_idx;
        green_weight_weight_sum[part] += green_weight * green_idx;
        blue_weight_weight_sum[part] += blue_weight * blue_idx;

        psum[part] += red_weight * red_idx * red_idx + green_weight * green_idx * green_idx + blue_weight * blue_idx * blue_idx;

    }

    // calculations specific to mode #7, the HDR RGB-scale mode.
    float red_sum[4];
    float green_sum[4];
    float blue_sum[4];
    for (i = 0; i < partition_count; i++) {
        red_sum[i] = red_vec[i].x + red_vec[i].y;
        green_sum[i] = green_vec[i].x + green_vec[i].y;
        blue_sum[i] = blue_vec[i].x + blue_vec[i].y;
        qsum[i] = red_vec[i].y + green_vec[i].y + blue_vec[i].y;
    }

    // rgb+offset for HDR endpoint mode #7
    int rgbo_fail[4];
    for (i = 0; i < partition_count; i++) {
        mat4t mod7_mat;
        float4 matv0f = {red_weight_sum[i], 0.0f, 0.0f, red_weight_weight_sum[i]    };
        float4 matv1f = {0.0f, green_weight_sum[i], 0.0f, green_weight_weight_sum[i]};
        float4 matv2f = {0.0f, 0.0f, blue_weight_sum[i], blue_weight_weight_sum[i]  };
        float4 matv3f = {red_weight_weight_sum[i], green_weight_weight_sum[i], blue_weight_weight_sum[i], psum[i]};
        mod7_mat.v[0] = matv0f;
        mod7_mat.v[1] = matv1f;
        mod7_mat.v[2] = matv2f;
        mod7_mat.v[3] = matv3f;

        float4 vect = { red_sum[i], green_sum[i], blue_sum[i], qsum[i] };

        mat4t rmod7_mat = invertMat4t(mod7_mat);
        float4 rgbovec = transform(rmod7_mat, vect);
        rgbo_vectors[i] = rgbovec;

        // we will occasionally get a failure due to a singluar matrix. Record whether such a
        // failure has taken place; if it did, compute rgbo_vectors[] with a different method
        // later on.
        float chkval = dot(rgbovec, rgbovec);
        rgbo_fail[i] = chkval != chkval;

    }



    // initialize the luminance and scale vectors with a reasonable default,
    // just in case the subsequent calculation blows up.
    for (i = 0; i < partition_count; i++) {

        float scalediv = scale_min[i] / scale_max[i];
        if (!(scalediv > 0.0f))
            scalediv = 0.0f;    // set to zero if scalediv is zero, negative, or NaN.

        if (scalediv > 1.0f)
            scalediv = 1.0f;
        float3 tmp1 = scale_directions[i] * scale_max[i];
        float4 rgbs_vectorsf = { tmp1.x,tmp1.y, tmp1.z, scalediv};
        rgbs_vectors[i] = rgbs_vectorsf;
        float2 lumf = {lum_min[i], lum_max[i]};
        lum_vectors[i] = lumf;
    }



    for (i = 0; i < partition_count; i++) {

        if (wmin1[i] >= wmax1[i] * 0.999) {
            // if all weights in the partition were equal, then just take average
            // of all colors in the partition and use that as both endpoint colors.
            float4 avg = {(red_vec[i].x + red_vec[i].y) / red_weight_sum[i],
                          (green_vec[i].x + green_vec[i].y) / green_weight_sum[i],
                          (blue_vec[i].x + blue_vec[i].y) / blue_weight_sum[i],
                          (alpha_vec[i].x + alpha_vec[i].y) / alpha_weight_sum[i]
                         };

            if (plane2_color_component != 0 && avg.x == avg.x)
                ep->endpt0[i].x = ep->endpt1[i].x = avg.x;
            if (plane2_color_component != 1 && avg.y == avg.y)
                ep->endpt0[i].y = ep->endpt1[i].y = avg.y;
            if (plane2_color_component != 2 && avg.z == avg.z)
                ep->endpt0[i].z = ep->endpt1[i].z = avg.z;
            if (plane2_color_component != 3 && avg.w == avg.w)
                ep->endpt0[i].w = ep->endpt1[i].w = avg.w;
            float3 tmp2 = scale_directions[i] * scale_max[i];
            float4 rgbs_vectorsf = { tmp2.x,tmp2.y,tmp2.z, 1.0f};
            rgbs_vectors[i] = rgbs_vectorsf;
            float  lumval  = (red_vec[i].x + red_vec[i].y + green_vec[i].x + green_vec[i].y + blue_vec[i].x + blue_vec[i].y) / (red_weight_sum[i] + green_weight_sum[i] + blue_weight_sum[i]);
            float2 lumval2 = {lumval, lumval};
            lum_vectors[i] = lumval2;
        }

        else {

            // otherwise, complete the analytic calculation of ideal-endpoint-values
            // for the given set of texel weigths and pixel colors.

//#ifdef __OPENCL_VERSION__
//             if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//                 printf("Plane-1a partition %d pmat1_red=[%3.3f %3.3f]\n", i, pmat1_red[i].v[0].x, pmat1_red[i].v[1].x);


            float red_det1 = determinant(pmat1_red[i]);
            float green_det1 = determinant(pmat1_green[i]);
            float blue_det1 = determinant(pmat1_blue[i]);
            float alpha_det1 = determinant(pmat1_alpha[i]);
            float lum_det1 = determinant(pmat1_lum[i]);
            float scale_det1 = determinant(pmat1_scale[i]);

            float red_mss1 = mat_square_sum(pmat1_red[i]);
            float green_mss1 = mat_square_sum(pmat1_green[i]);
            float blue_mss1 = mat_square_sum(pmat1_blue[i]);
            float alpha_mss1 = mat_square_sum(pmat1_alpha[i]);
            float lum_mss1 = mat_square_sum(pmat1_lum[i]);
            float scale_mss1 = mat_square_sum(pmat1_scale[i]);

//#ifdef __OPENCL_VERSION__
//             if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//                 printf("Plane-1b partition %d determinants: R=%g G=%g B=%g A=%g L=%g S=%g\n", i, red_det1, green_det1, blue_det1, alpha_det1, lum_det1, scale_det1);

            pmat1_red[i]   = invertMat2t(pmat1_red[i]);
            pmat1_green[i] = invertMat2t(pmat1_green[i]);
            pmat1_blue[i]  = invertMat2t(pmat1_blue[i]);
            pmat1_alpha[i] = invertMat2t(pmat1_alpha[i]);
            pmat1_lum[i]   = invertMat2t(pmat1_lum[i]);
            pmat1_scale[i] = invertMat2t(pmat1_scale[i]);

            float4 ep0 = {dot(pmat1_red[i].v[0], red_vec[i]),
                          dot(pmat1_green[i].v[0], green_vec[i]),
                          dot(pmat1_blue[i].v[0], blue_vec[i]),
                          dot(pmat1_alpha[i].v[0], alpha_vec[i])
                         };

            float4 ep1 = {dot(pmat1_red[i].v[1], red_vec[i]),
                          dot(pmat1_green[i].v[1], green_vec[i]),
                          dot(pmat1_blue[i].v[1], blue_vec[i]),
                          dot(pmat1_alpha[i].v[1], alpha_vec[i])
                         };

            float lum_ep0    = dot(pmat1_lum[i].v[0], lum_vec[i]);
            float lum_ep1    = dot(pmat1_lum[i].v[1], lum_vec[i]);
            float scale_ep0  = dot(pmat1_scale[i].v[0], scale_vec[i]);
            float scale_ep1  = dot(pmat1_scale[i].v[1], scale_vec[i]);


            if (plane2_color_component != 0 && fabs(red_det1) > (red_mss1 * FLOAT_n4) && ep0.x == ep0.x && ep1.x == ep1.x) {
                ep->endpt0[i].x = ep0.x;
                ep->endpt1[i].x = ep1.x;
            }
            if (plane2_color_component != 1 && fabs(green_det1) > (green_mss1 * FLOAT_n4) && ep0.y == ep0.y && ep1.y == ep1.y) {
                ep->endpt0[i].y = ep0.y;
                ep->endpt1[i].y = ep1.y;
            }
            if (plane2_color_component != 2 && fabs(blue_det1) > (blue_mss1 * FLOAT_n4) && ep0.z == ep0.z && ep1.z == ep1.z) {
                ep->endpt0[i].z = ep0.z;
                ep->endpt1[i].z = ep1.z;
            }
            if (plane2_color_component != 3 && fabs(alpha_det1) > (alpha_mss1 * FLOAT_n4) && ep0.w == ep0.w && ep1.w == ep1.w) {
                ep->endpt0[i].w = ep0.w;
                ep->endpt1[i].w = ep1.w;
            }

            if (fabs(lum_det1) > (lum_mss1 * FLOAT_n4) && lum_ep0 == lum_ep0 && lum_ep1 == lum_ep1 && lum_ep0 < lum_ep1) {
                lum_vectors[i].x = lum_ep0;
                lum_vectors[i].y = lum_ep1;
            }
            if (fabs(scale_det1) > (scale_mss1 * FLOAT_n4) && scale_ep0 == scale_ep0 && scale_ep1 == scale_ep1 && scale_ep0 < scale_ep1) {
                float scalediv = scale_ep0 / scale_ep1;
                float3 tmp3 = scale_directions[i] * scale_ep1;
                float4 rgbs_vectorsf = { tmp3.x,tmp3.y,tmp3.z, scalediv};
                rgbs_vectors[i] = rgbs_vectorsf;
            }

        }

        if (plane2_weight_set8) {
            if (wmin2[i] >= wmax2[i] * 0.999) {
                // if all weights in the partition were equal, then just take average
                // of all colors in the partition and use that as both endpoint colors.
                float4 avg = {(red_vec[i].x + red_vec[i].y) / red_weight_sum[i],
                              (green_vec[i].x + green_vec[i].y) / green_weight_sum[i],
                              (blue_vec[i].x + blue_vec[i].y) / blue_weight_sum[i],
                              (alpha_vec[i].x + alpha_vec[i].y) / alpha_weight_sum[i]
                             };

                if (plane2_color_component == 0 && avg.x == avg.x)
                    ep->endpt0[i].x = ep->endpt1[i].x = avg.x;
                if (plane2_color_component == 1 && avg.y == avg.y)
                    ep->endpt0[i].y = ep->endpt1[i].y = avg.y;
                if (plane2_color_component == 2 && avg.z == avg.z)
                    ep->endpt0[i].z = ep->endpt1[i].z = avg.z;
                if (plane2_color_component == 3 && avg.w == avg.w)
                    ep->endpt0[i].w = ep->endpt1[i].w = avg.w;
            } else {

                // otherwise, complete the analytic calculation of ideal-endpoint-values
                // for the given set of texel weigths and pixel colors.
                float red_det2 = determinant(pmat2_red[i]);
                float green_det2 = determinant(pmat2_green[i]);
                float blue_det2 = determinant(pmat2_blue[i]);
                float alpha_det2 = determinant(pmat2_alpha[i]);

                float red_mss2 = mat_square_sum(pmat2_red[i]);
                float green_mss2 = mat_square_sum(pmat2_green[i]);
                float blue_mss2 = mat_square_sum(pmat2_blue[i]);
                float alpha_mss2 = mat_square_sum(pmat2_alpha[i]);

//#ifdef __OPENCL_VERSION__
//                 if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//                     printf("Plane-2 partition %d determinants: R=%g G=%g B=%g A=%g\n", i, red_det2, green_det2, blue_det2, alpha_det2);

                pmat2_red[i]   = invertMat2t(pmat2_red[i]);
                pmat2_green[i] = invertMat2t(pmat2_green[i]);
                pmat2_blue[i]  = invertMat2t(pmat2_blue[i]);
                pmat2_alpha[i] = invertMat2t(pmat2_alpha[i]);
                float4 ep0 = {dot(pmat2_red[i].v[0], red_vec[i]),
                              dot(pmat2_green[i].v[0], green_vec[i]),
                              dot(pmat2_blue[i].v[0], blue_vec[i]),
                              dot(pmat2_alpha[i].v[0], alpha_vec[i])
                             };
                float4 ep1 = {dot(pmat2_red[i].v[1], red_vec[i]),
                              dot(pmat2_green[i].v[1], green_vec[i]),
                              dot(pmat2_blue[i].v[1], blue_vec[i]),
                              dot(pmat2_alpha[i].v[1], alpha_vec[i])
                             };

                if (plane2_color_component == 0 && fabs(red_det2) > (red_mss2 * FLOAT_n4) && ep0.x == ep0.x && ep1.x == ep1.x) {
                    ep->endpt0[i].x = ep0.x;
                    ep->endpt1[i].x = ep1.x;
                }
                if (plane2_color_component == 1 && fabs(green_det2) > (green_mss2 * FLOAT_n4) && ep0.y == ep0.y && ep1.y == ep1.y) {
                    ep->endpt0[i].y = ep0.y;
                    ep->endpt1[i].y = ep1.y;
                }
                if (plane2_color_component == 2 && fabs(blue_det2) > (blue_mss2 * FLOAT_n4) && ep0.z == ep0.z && ep1.z == ep1.z) {
                    ep->endpt0[i].z = ep0.z;
                    ep->endpt1[i].z = ep1.z;
                }
                if (plane2_color_component == 3 && fabs(alpha_det2) > (alpha_mss2 * FLOAT_n4) && ep0.w == ep0.w && ep1.w == ep1.w) {
                    ep->endpt0[i].w = ep0.w;
                    ep->endpt1[i].w = ep1.w;
                }

            }
        }
    }

    // if the calculation of an RGB-offset vector failed, try to compute
    // a somewhat-sensible value anyway
    float3 onef = {1.0f,1.0f,1.0f};
    for (i = 0; i < partition_count; i++)
        if (rgbo_fail[i]) {
            float4 v0 = ep->endpt0[i];
            float4 v1 = ep->endpt1[i];
            float avgdif = dot(v1.xyz - v0.xyz, onef) * (1.0f / 3.0f);
            if (avgdif <= 0.0f)
                avgdif = 0.0f;
            float4 avg = (v0 + v1) * 0.5f;
            float4 ep0 = {avgdif, avgdif, avgdif, avgdif};
            ep0 = avg - ep0 * 0.5f;
            float3 tmp_ep0 = ep0.xyz;
            float4 tmp = { tmp_ep0.x,tmp_ep0.y,tmp_ep0.z, avgdif};
            rgbo_vectors[i] = tmp;
        }

//#ifdef __OPENCL_VERSION__
//     if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//         printf("post  <%g %g %g %g>\n", ep->endpt0[0].x, ep->endpt0[0].y, ep->endpt0[0].z, ep->endpt0[0].w);

}


int imageblock_uses_alpha4(imageblock * blk) {
    return blk->alpha_max != blk->alpha_min;
}

void compute_averages_and_directions_2_components(__global partition_info * pt,
        imageblock * blk,
        error_weight_block * ewb, float2 * color_scalefactors, int component1, int component2, float2 * averages, float2 * directions) {
    int i;
    int partition_count = pt->partition_count;
    int partition;

    float *texel_weights;
    if (component1 == 0 && component2 == 1)
        texel_weights = ewb->texel_weight_rg;
    else if (component1 == 0 && component2 == 2)
        texel_weights = ewb->texel_weight_rb;
    else if (component1 == 1 && component2 == 2)
        texel_weights = ewb->texel_weight_gb;
    else {
        texel_weights = ewb->texel_weight_rg;
        // unsupported set of color components.
        //exit(1);
        astc_codec_internal_error("ERROR: compute_averages_and_directions_2_components");
    }


    for (partition = 0; partition < partition_count; partition++) {
        int texelcount = pt->texels_per_partition[partition];

        float2 base_sum = { 0.0f, 0.0f };
        float partition_weight = 0.0f;

        for (i = 0; i < texelcount; i++) {
            int iwt = pt->texels_of_partition[partition][i];
            float weight = texel_weights[iwt];
            float2 texel_datum = {blk->work_data[4 * iwt + component1], blk->work_data[4 * iwt + component2]};
            texel_datum = texel_datum  * weight;
            partition_weight += weight;

            base_sum = base_sum + texel_datum;
        }

        float2 csf = color_scalefactors[partition];

        float2 average = base_sum * 1.0f / MAX(partition_weight, FLOAT_n7);
        averages[partition] = average * csf.xy;


        float2 sum_xp = { 0.0f, 0.0f };
        float2 sum_yp = { 0.0f, 0.0f };

        for (i = 0; i < texelcount; i++) {
            int iwt = pt->texels_of_partition[partition][i];
            float weight = texel_weights[iwt];
            float2 texel_datum = {blk->work_data[4 * iwt + component1], blk->work_data[4 * iwt + component2]};
            texel_datum = (texel_datum - average) * weight;

            if (texel_datum.x > 0.0f)
                sum_xp = sum_xp + texel_datum;
            if (texel_datum.y > 0.0f)
                sum_yp = sum_yp + texel_datum;
        }

        float prod_xp = dot(sum_xp, sum_xp);
        float prod_yp = dot(sum_yp, sum_yp);

        float2 best_vector = sum_xp;
        float best_sum = prod_xp;
        if (prod_yp > best_sum) {
            best_vector = sum_yp;
            best_sum = prod_yp;
        }

        directions[partition] = best_vector;
    }

}

//static
void compute_endpoints_and_ideal_weights_2_components( __global partition_info * pt,
        imageblock * blk, error_weight_block * ewb,
        endpoints_and_weights * ei, int component1, int component2,
        __global ASTC_Encode *ASTCEncode) {
    int i;

    int partition_count = pt->partition_count;
    ei->ep.partition_count = partition_count;

    float4 error_weightings[4];
    float4 color_scalefactors[4];

    float2 scalefactors[4];

    float *error_weights;
    if (component1 == 0 && component2 == 1)
        error_weights = ewb->texel_weight_rg;
    else if (component1 == 0 && component2 == 2)
        error_weights = ewb->texel_weight_rb;
    else if (component1 == 1 && component2 == 2)
        error_weights = ewb->texel_weight_gb;
    else {
        error_weights = ewb->texel_weight_rg;
        astc_codec_internal_error("ERROR: compute_endpoints_and_ideal_weights_2_components");
    }

    compute_partition_error_color_weightings(ewb, pt, error_weightings, color_scalefactors, ASTCEncode);

    for (i = 0; i < partition_count; i++) {
        float s1 = 0.0f, s2 = 0.0f;
        switch (component1) {
        case 0:
            s1 = color_scalefactors[i].x;
            break;
        case 1:
            s1 = color_scalefactors[i].y;
            break;
        case 2:
            s1 = color_scalefactors[i].z;
            break;
        case 3:
            s1 = color_scalefactors[i].w;
            break;
        }

        switch (component2) {
        case 0:
            s2 = color_scalefactors[i].x;
            break;
        case 1:
            s2 = color_scalefactors[i].y;
            break;
        case 2:
            s2 = color_scalefactors[i].z;
            break;
        case 3:
            s2 = color_scalefactors[i].w;
            break;
        }
        float2 s2f = { s1, s2 };
        scalefactors[i] = normalize(s2f) * 1.41421356f;
    }


    float lowparam[4], highparam[4];

    float2 averages[4];
    float2 directions[4];

    line2 lines[4];
    float scale[4];
    float length_squared[4];


    for (i = 0; i < partition_count; i++) {
        lowparam[i] = FLOAT_10;
        highparam[i] = -FLOAT_10;
    }


    compute_averages_and_directions_2_components(pt, blk, ewb, scalefactors, component1, component2, averages, directions);

    for (i = 0; i < partition_count; i++) {
        float2 egv = directions[i];
        float2 zero2f = {0.0f,0.0f};
        if (egv.x + egv.y < 0.0f)
            directions[i] = zero2f - egv;
    }

    for (i = 0; i < partition_count; i++) {
        float2 one2f = {1.0f, 1.0f};
        lines[i].a = averages[i];
        if (dot(directions[i], directions[i]) == 0.0f)
            lines[i].b = normalize(one2f);
        else
            lines[i].b = normalize(directions[i]);
    }


    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        if (error_weights[i] > FLOAT_n10) {
            int partition = pt->partition_of_texel[i];
            float2 point = { blk->work_data[4 * i + component1], blk->work_data[4 * i + component2] };
            point = point  * scalefactors[partition];
            line2 l = lines[partition];
            float param = dot(point - l.a, l.b);
            ei->weights[i] = param;
            if (param < lowparam[partition])
                lowparam[partition] = param;
            if (param > highparam[partition])
                highparam[partition] = param;
        } else {
            ei->weights[i] = -FLOAT_38;
        }
    }

    float2 lowvalues[4];
    float2 highvalues[4];


    for (i = 0; i < partition_count; i++) {
        float length = highparam[i] - lowparam[i];
        if (length < 0) {          // case for when none of the texels had any weight
            lowparam[i] = 0.0f;
            highparam[i] = FLOAT_n7;
        }

        // it is possible for a uniform-color partition to produce length=0; this
        // causes NaN-production and NaN-propagation later on. Set length to
        // a small value to avoid this problem.
        if (length < FLOAT_n7)
            length = FLOAT_n7;

        length_squared[i] = length * length;
        scale[i] = 1.0f / length;

        float2 ep0 = lines[i].a + lines[i].b * lowparam[i];
        float2 ep1 = lines[i].a + lines[i].b * highparam[i];

        ep0 = ep0 / scalefactors[i];
        ep1 = ep1 / scalefactors[i];

        lowvalues[i] = ep0;
        highvalues[i] = ep1;
    }


    for (i = 0; i < partition_count; i++) {
        float4 ep0f = { blk->red_min, blk->green_min, blk->blue_min, blk->alpha_min };
        float4 ep1f = { blk->red_max, blk->green_max, blk->blue_max, blk->alpha_max };
        ei->ep.endpt0[i] = ep0f;
        ei->ep.endpt1[i] = ep1f;

        float2 ep0 = lowvalues[i];
        float2 ep1 = highvalues[i];

        switch (component1) {
        case 0:
            ei->ep.endpt0[i].x = ep0.x;
            ei->ep.endpt1[i].x = ep1.x;
            break;
        case 1:
            ei->ep.endpt0[i].y = ep0.x;
            ei->ep.endpt1[i].y = ep1.x;
            break;
        case 2:
            ei->ep.endpt0[i].z = ep0.x;
            ei->ep.endpt1[i].z = ep1.x;
            break;
        case 3:
            ei->ep.endpt0[i].w = ep0.x;
            ei->ep.endpt1[i].w = ep1.x;
            break;
        }
        switch (component2) {
        case 0:
            ei->ep.endpt0[i].x = ep0.y;
            ei->ep.endpt1[i].x = ep1.y;
            break;
        case 1:
            ei->ep.endpt0[i].y = ep0.y;
            ei->ep.endpt1[i].y = ep1.y;
            break;
        case 2:
            ei->ep.endpt0[i].z = ep0.y;
            ei->ep.endpt1[i].z = ep1.y;
            break;
        case 3:
            ei->ep.endpt0[i].w = ep0.y;
            ei->ep.endpt1[i].w = ep1.y;
            break;
        }
    }

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        int partition = pt->partition_of_texel[i];
        float idx = (ei->weights[i] - lowparam[partition]) * scale[partition];
        if (idx > 1.0f)
            idx = 1.0f;
        else if (!(idx > 0.0f))
            idx = 0.0f;

        ei->weights[i] = idx;
        ei->weight_error_scale[i] = length_squared[partition] * error_weights[i];
        if (astc_isnan(ei->weight_error_scale[i])) {
            astc_codec_internal_error("ERROR: compute_endpoints_and_ideal_weights_2_components:2");
        }
    }
}


//static
void compute_endpoints_and_ideal_weights_1_component(
    __global partition_info * pt, imageblock * blk,
    error_weight_block * ewb, endpoints_and_weights * ei,
    int component,
    __global ASTC_Encode *ASTCEncode) {
    int i;

    int partition_count = pt->partition_count;
    ei->ep.partition_count = partition_count;

    float lowvalues[4], highvalues[4];
    float partition_error_scale[4];
    float linelengths_rcp[4];


    float *error_weights;
    switch (component) {
    case 0:
        error_weights = ewb->texel_weight_r;
        break;
    case 1:
        error_weights = ewb->texel_weight_g;
        break;
    case 2:
        error_weights = ewb->texel_weight_b;
        break;
    case 3:
        error_weights = ewb->texel_weight_a;
        break;
    default:
        error_weights = ewb->texel_weight_r;
        astc_codec_internal_error("ERROR:1 compute_endpoints_and_ideal_weights_1_component");
    }


    for (i = 0; i < partition_count; i++) {
        lowvalues[i] = FLOAT_10;
        highvalues[i] = -FLOAT_10;
    }

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        if (error_weights[i] > FLOAT_n10) {
            float value = blk->work_data[4 * i + component];
            int partition = pt->partition_of_texel[i];
            if (value < lowvalues[partition])
                lowvalues[partition] = value;
            if (value > highvalues[partition])
                highvalues[partition] = value;
        }
    }

    for (i = 0; i < partition_count; i++) {
        float diff = highvalues[i] - lowvalues[i];
        if (diff < 0) {
            lowvalues[i] = 0;
            highvalues[i] = 0;
        }
        if (diff < FLOAT_n7)
            diff = FLOAT_n7;
        partition_error_scale[i] = diff * diff;
        linelengths_rcp[i] = 1.0f / diff;
    }

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        float value = blk->work_data[4 * i + component];
        int partition = pt->partition_of_texel[i];
        value -= lowvalues[partition];
        value *= linelengths_rcp[partition];
        if (value > 1.0f)
            value = 1.0f;
        else if (!(value > 0.0f))
            value = 0.0f;

        ei->weights[i] = value;
        ei->weight_error_scale[i] = partition_error_scale[partition] * error_weights[i];
        if (astc_isnan(ei->weight_error_scale[i])) {
            astc_codec_internal_error("ERROR:2 compute_endpoints_and_ideal_weights_1_component");
        }
    }

    for (i = 0; i < partition_count; i++) {
        float4 ep0 = { blk->red_min, blk->green_min, blk->blue_min, blk->alpha_min };
        float4 ep1 = { blk->red_max, blk->green_max, blk->blue_max, blk->alpha_max };

        ei->ep.endpt0[i] = ep0;
        ei->ep.endpt1[i] = ep1;
        switch (component) {
        case 0:                // red/x
            ei->ep.endpt0[i].x = lowvalues[i];
            ei->ep.endpt1[i].x = highvalues[i];
            break;
        case 1:                // green/y
            ei->ep.endpt0[i].y = lowvalues[i];
            ei->ep.endpt1[i].y = highvalues[i];
            break;
        case 2:                // blue/z
            ei->ep.endpt0[i].z = lowvalues[i];
            ei->ep.endpt1[i].z = highvalues[i];
            break;
        case 3:                // alpha/w
            ei->ep.endpt0[i].w = lowvalues[i];
            ei->ep.endpt1[i].w = highvalues[i];
            break;
        }
    }
}

void compute_endpoints_and_ideal_weights_2_planes(
    __global partition_info * pt,
    imageblock * blk,
    error_weight_block * ewb,
    int separate_component,
    endpoints_and_weights * ei1,
    endpoints_and_weights * ei2,
    __global ASTC_Encode *ASTCEncode) {
    int uses_alpha = imageblock_uses_alpha4(blk);
    switch (separate_component) {
    case 0:                    // separate weights for red
        if (uses_alpha == 1)
            compute_endpoints_and_ideal_weights_3_components(pt, blk, ewb, ei1, 1, 2, 3, ASTCEncode);
        else
            compute_endpoints_and_ideal_weights_2_components(pt, blk, ewb, ei1, 1, 2, ASTCEncode);
        compute_endpoints_and_ideal_weights_1_component( pt, blk, ewb, ei2, 0, ASTCEncode);
        break;

    case 1:                    // separate weights for green
        if (uses_alpha == 1)
            compute_endpoints_and_ideal_weights_3_components(pt, blk, ewb, ei1, 0, 2, 3, ASTCEncode);
        else
            compute_endpoints_and_ideal_weights_2_components(pt, blk, ewb, ei1, 0, 2, ASTCEncode);
        compute_endpoints_and_ideal_weights_1_component(pt, blk, ewb, ei2, 1, ASTCEncode);
        break;

    case 2:                    // separate weights for blue
        if (uses_alpha == 1)
            compute_endpoints_and_ideal_weights_3_components(pt, blk, ewb, ei1, 0, 1, 3, ASTCEncode);
        else
            compute_endpoints_and_ideal_weights_2_components(pt, blk, ewb, ei1, 0, 1, ASTCEncode);
        compute_endpoints_and_ideal_weights_1_component(pt, blk, ewb, ei2, 2, ASTCEncode);
        break;

    case 3:                    // separate weights for alpha
        if (uses_alpha == 0) {
            astc_codec_internal_error("ERROR: compute_endpoints_and_ideal_weights_2_planes");
        }
        compute_endpoints_and_ideal_weights_3_components(pt, blk, ewb, ei1, 0, 1, 2, ASTCEncode);

        compute_endpoints_and_ideal_weights_1_component(pt, blk, ewb, ei2, 3, ASTCEncode);
        break;
    }

}

// function to compute the error across a tile when using a particular line for
// a particular partition.
float compute_error_squared_rgb_single_partition(int partition_to_test,
        __global partition_info * pt,    // the partition that we use when computing the squared-error.
        imageblock * blk, error_weight_block * ewb,
        processed_line3 * lin,    // the line for the partition.
        __global ASTC_Encode *ASTCEncode
                                                ) {
    int i;

    float errorsum = 0.0f;

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        int partition = pt->partition_of_texel[i];
        float texel_weight = ewb->texel_weight_rgb[i];
        if (partition != partition_to_test || texel_weight < FLOAT_n20)
            continue;
        float3 point = { blk->work_data[4 * i], blk->work_data[4 * i + 1], blk->work_data[4 * i + 2] };

        float param = dot(point, lin->bs);
        float3 rp1 = lin->amod + param * lin->bis;
        float3 dist = rp1 - point;
        float4 ews = ewb->error_weights[i];

        errorsum += dot(ews.xyz, dist * dist);
    }
    return errorsum;
}

// helper function to merge two endpoint-colors
void merge_endpoints(endpoints * ep1,    // contains three of the color components
                     endpoints * ep2,    // contains the remaining color component
                     int separate_component, endpoints * res) {
    int i;
    int partition_count = ep1->partition_count;
    res->partition_count = partition_count;
    for (i = 0; i < partition_count; i++) {
        res->endpt0[i] = ep1->endpt0[i];
        res->endpt1[i] = ep1->endpt1[i];
    }

    switch (separate_component) {
    case 0:
        for (i = 0; i < partition_count; i++) {
            res->endpt0[i].x = ep2->endpt0[i].x;
            res->endpt1[i].x = ep2->endpt1[i].x;
        }
        break;
    case 1:
        for (i = 0; i < partition_count; i++) {
            res->endpt0[i].y = ep2->endpt0[i].y;
            res->endpt1[i].y = ep2->endpt1[i].y;
        }
        break;
    case 2:
        for (i = 0; i < partition_count; i++) {
            res->endpt0[i].z = ep2->endpt0[i].z;
            res->endpt1[i].z = ep2->endpt1[i].z;
        }
        break;
    case 3:
        for (i = 0; i < partition_count; i++) {
            res->endpt0[i].w = ep2->endpt0[i].w;
            res->endpt1[i].w = ep2->endpt1[i].w;
        }
        break;
    }
}

void compute_encoding_choice_errors(
    imageblock * pb, __global partition_info * pi, error_weight_block * ewb,
    int separate_component,    // component that is separated out in 2-plane mode, -1 in 1-plane mode
    encoding_choice_errors * eci,
    __global ASTC_Encode *ASTCEncode) {
    int i;

    int partition_count = pi->partition_count;

    float3 averages[4];
    float3 directions_rgb[4];
    float2 directions_rg[4];
    float2 directions_rb[4];
    float2 directions_gb[4];

    float4 error_weightings[4];
    float4 color_scalefactors[4];
    float4 inverse_color_scalefactors[4];

    compute_partition_error_color_weightings(ewb, pi, error_weightings, color_scalefactors, ASTCEncode);

    compute_averages_and_directions_rgb(pi, pb, ewb, color_scalefactors, averages, directions_rgb, directions_rg, directions_rb, directions_gb);

    line3 uncorr_rgb_lines[4];
    line3 samechroma_rgb_lines[4];    // for LDR-RGB-scale
    line3 rgb_luma_lines[4];    // for HDR-RGB-scale
    line3 luminance_lines[4];

    processed_line3 proc_uncorr_rgb_lines[4];
    processed_line3 proc_samechroma_rgb_lines[4];    // for LDR-RGB-scale
    processed_line3 proc_rgb_luma_lines[4];    // for HDR-RGB-scale
    processed_line3 proc_luminance_lines[4];


    for (i = 0; i < partition_count; i++) {
        inverse_color_scalefactors[i].x = 1.0f / MAX(color_scalefactors[i].x, FLOAT_n7);
        inverse_color_scalefactors[i].y = 1.0f / MAX(color_scalefactors[i].y, FLOAT_n7);
        inverse_color_scalefactors[i].z = 1.0f / MAX(color_scalefactors[i].z, FLOAT_n7);
        inverse_color_scalefactors[i].w = 1.0f / MAX(color_scalefactors[i].w, FLOAT_n7);


        uncorr_rgb_lines[i].a = averages[i];
        float3 tmp3f;

        if (dot(directions_rgb[i], directions_rgb[i]) == 0.0f) {
            tmp3f = color_scalefactors[i].xyz;
            uncorr_rgb_lines[i].b = normalize(tmp3f);
        } else {
            tmp3f = directions_rgb[i];
            uncorr_rgb_lines[i].b = normalize(tmp3f);
        }

        float3 zero3f = { 0.0f, 0.0f, 0.0f } ;
        samechroma_rgb_lines[i].a = zero3f;
        if (dot(averages[i], averages[i]) < FLOAT_n20) {
            float3 tmp3af = color_scalefactors[i].xyz;
            samechroma_rgb_lines[i].b = normalize(tmp3af);
        } else
            samechroma_rgb_lines[i].b = normalize(averages[i]);

        rgb_luma_lines[i].a = averages[i];
        rgb_luma_lines[i].b = normalize(color_scalefactors[i].xyz);

        luminance_lines[i].a = zero3f;
        luminance_lines[i].b = normalize(color_scalefactors[i].xyz);

        proc_uncorr_rgb_lines[i].amod = (uncorr_rgb_lines[i].a - uncorr_rgb_lines[i].b * dot(uncorr_rgb_lines[i].a, uncorr_rgb_lines[i].b)) * inverse_color_scalefactors[i].xyz;
        proc_uncorr_rgb_lines[i].bs = uncorr_rgb_lines[i].b * color_scalefactors[i].xyz;
        proc_uncorr_rgb_lines[i].bis = uncorr_rgb_lines[i].b * inverse_color_scalefactors[i].xyz;

        proc_samechroma_rgb_lines[i].amod = (samechroma_rgb_lines[i].a - samechroma_rgb_lines[i].b * dot(samechroma_rgb_lines[i].a, samechroma_rgb_lines[i].b)) * inverse_color_scalefactors[i].xyz;
        proc_samechroma_rgb_lines[i].bs = samechroma_rgb_lines[i].b * color_scalefactors[i].xyz;
        proc_samechroma_rgb_lines[i].bis = samechroma_rgb_lines[i].b * inverse_color_scalefactors[i].xyz;

        proc_rgb_luma_lines[i].amod = (rgb_luma_lines[i].a - rgb_luma_lines[i].b * dot(rgb_luma_lines[i].a, rgb_luma_lines[i].b)) * inverse_color_scalefactors[i].xyz;
        proc_rgb_luma_lines[i].bs = rgb_luma_lines[i].b * color_scalefactors[i].xyz;
        proc_rgb_luma_lines[i].bis = rgb_luma_lines[i].b * inverse_color_scalefactors[i].xyz;

        proc_luminance_lines[i].amod = (luminance_lines[i].a - luminance_lines[i].b * dot(luminance_lines[i].a, luminance_lines[i].b)) * inverse_color_scalefactors[i].xyz;
        proc_luminance_lines[i].bs = luminance_lines[i].b * color_scalefactors[i].xyz;
        proc_luminance_lines[i].bis = luminance_lines[i].b * inverse_color_scalefactors[i].xyz;

    }

    float uncorr_rgb_error[4];
    float samechroma_rgb_error[4];
    float rgb_luma_error[4];
    float luminance_rgb_error[4];

    for (i = 0; i < partition_count; i++) {

        uncorr_rgb_error[i] = compute_error_squared_rgb_single_partition(i, pi, pb, ewb, &(proc_uncorr_rgb_lines[i]), ASTCEncode);

        samechroma_rgb_error[i] = compute_error_squared_rgb_single_partition(i, pi, pb, ewb, &(proc_samechroma_rgb_lines[i]), ASTCEncode);

        rgb_luma_error[i] = compute_error_squared_rgb_single_partition(i,  pi, pb, ewb, &(proc_rgb_luma_lines[i]), ASTCEncode);

        luminance_rgb_error[i] = compute_error_squared_rgb_single_partition(i,  pi, pb, ewb, &(proc_luminance_lines[i]), ASTCEncode);

    }

    // compute the error that arises from just ditching alpha and RGB
    float alpha_drop_error[4];
    float rgb_drop_error[4];
    for (i = 0; i < partition_count; i++) {
        alpha_drop_error[i] = 0;
        rgb_drop_error[i] = 0;
    }
    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        int partition = pi->partition_of_texel[i];
        float alpha = pb->work_data[4 * i + 3];
        float default_alpha = pb->alpha_lns[i] ? (float)0x7800 : (float)0xFFFF;

        float omalpha = alpha - default_alpha;
        alpha_drop_error[partition] += omalpha * omalpha * ewb->error_weights[i].w;
        float red = pb->work_data[4 * i];
        float green = pb->work_data[4 * i + 1];
        float blue = pb->work_data[4 * i + 2];
        rgb_drop_error[partition] += red * red * ewb->error_weights[i].x + green * green * ewb->error_weights[i].y + blue * blue * ewb->error_weights[i].z;
    }

    // check if we are eligible for blue-contraction and offset-encoding

    endpoints ep;
    if (separate_component == -1) {
        endpoints_and_weights ei;
        compute_endpoints_and_ideal_weights_1_plane(pi, pb, ewb, &ei, ASTCEncode);
        ep = ei.ep;
    } else {
        endpoints_and_weights ei1, ei2;
        compute_endpoints_and_ideal_weights_2_planes( pi, pb, ewb, separate_component, &ei1, &ei2, ASTCEncode);

        merge_endpoints(&(ei1.ep), &(ei2.ep), separate_component, &ep);
    }

    int eligible_for_offset_encode[4];
    int eligible_for_blue_contraction[4];

    for (i = 0; i < partition_count; i++) {
        float4 endpt0 = ep.endpt0[i];
        float4 endpt1 = ep.endpt1[i];
        float4 endpt_dif = endpt1 - endpt0;
        if (fabs(endpt_dif.x) < (0.12 * 65535.0f) && fabs(endpt_dif.y) < (0.12 * 65535.0f) && fabs(endpt_dif.z) < (0.12 * 65535.0f))
            eligible_for_offset_encode[i] = 1;
        else
            eligible_for_offset_encode[i] = 0;
        endpt0.x += (endpt0.x - endpt0.z);
        endpt0.y += (endpt0.y - endpt0.z);
        endpt1.x += (endpt1.x - endpt1.z);
        endpt1.y += (endpt1.y - endpt1.z);
        if (endpt0.x > (0.01f * 65535.0f) && endpt0.x < (0.99f * 65535.0f)
                && endpt1.x >(0.01f * 65535.0f) && endpt1.x < (0.99f * 65535.0f)
                && endpt0.y >(0.01f * 65535.0f) && endpt0.y < (0.99f * 65535.0f) && endpt1.y >(0.01f * 65535.0f) && endpt1.y < (0.99f * 65535.0f))
            eligible_for_blue_contraction[i] = 1;
        else
            eligible_for_blue_contraction[i] = 0;
    }


    // finally, gather up our results
    for (i = 0; i < partition_count; i++) {
        eci[i].rgb_scale_error = (samechroma_rgb_error[i] - uncorr_rgb_error[i]) * 0.7f;    // empirical
        eci[i].rgb_luma_error = (rgb_luma_error[i] - uncorr_rgb_error[i]) * 1.5f;    // wild guess
        eci[i].luminance_error = (luminance_rgb_error[i] - uncorr_rgb_error[i]) * 3.0f;    // empirical
        eci[i].alpha_drop_error = alpha_drop_error[i] * 3.0f;
        eci[i].rgb_drop_error = rgb_drop_error[i] * 3.0f;
        eci[i].can_offset_encode = eligible_for_offset_encode[i];
        eci[i].can_blue_contract = eligible_for_blue_contraction[i];
    }
}

// for a given partition, compute for every (integer-component-count, quantization-level)
// the color error.
//static
void compute_color_error_for_every_integer_count_and_quantization_level(int encode_hdr_rgb,    // 1 = perform HDR encoding, 0 = perform LDR encoding.
        int encode_hdr_alpha, int partition_index, __global partition_info * pi,
        encoding_choice_errors * eci,    // pointer to the structure for the CURRENT partition.
        endpoints * ep, float4 error_weightings[4],
        // arrays to return results back through.
        float best_error[21][4], int format_of_choice[21][4]) {
    int i;
    int partition_size = pi->texels_per_partition[partition_index];

    // float4 eps = ep->endpt_scale[partition_index];
    float4 ep0 = ep->endpt0[partition_index];    // / eps;
    float4 ep1 = ep->endpt1[partition_index];    // / eps;

    float ep0_max = MAX(MAX(ep0.x, ep0.y), ep0.z);
    float ep0_min = MIN(MIN(ep0.x, ep0.y), ep0.z);
    float ep1_max = MAX(MAX(ep1.x, ep1.y), ep1.z);
    float ep1_min = MIN(MIN(ep1.x, ep1.y), ep1.z);

    ep0_min = MAX(ep0_min, 0.0f);
    ep1_min = MAX(ep1_min, 0.0f);
    ep0_max = MAX(ep0_max, FLOAT_n10);
    ep1_max = MAX(ep1_max, FLOAT_n10);

    /*
    float lum_scale = MAX(ep0_max, ep1_max); float alpha_scale = MAX(ep0.w, ep1.w);
    */


    float4 error_weight = error_weightings[partition_index];


    float error_weight_rgbsum = error_weight.x + error_weight.y + error_weight.z;


    float range_upper_limit_rgb = encode_hdr_rgb ? 61440.0f : 65535.0f;
    float range_upper_limit_alpha = encode_hdr_alpha ? 61440.0f : 65535.0f;

    // it is possible to get endpoint colors significantly outside [0,upper-limit]
    // even if the input data are safely contained in [0,upper-limit];
    // we need to add an error term for this situation,
    float4 ep0_range_error_high;
    float4 ep1_range_error_high;
    float4 ep0_range_error_low;
    float4 ep1_range_error_low;

    ep0_range_error_high.x = MAX(0.0f, ep0.x - range_upper_limit_rgb);
    ep0_range_error_high.y = MAX(0.0f, ep0.y - range_upper_limit_rgb);
    ep0_range_error_high.z = MAX(0.0f, ep0.z - range_upper_limit_rgb);
    ep0_range_error_high.w = MAX(0.0f, ep0.w - range_upper_limit_alpha);
    ep1_range_error_high.x = MAX(0.0f, ep1.x - range_upper_limit_rgb);
    ep1_range_error_high.y = MAX(0.0f, ep1.y - range_upper_limit_rgb);
    ep1_range_error_high.z = MAX(0.0f, ep1.z - range_upper_limit_rgb);
    ep1_range_error_high.w = MAX(0.0f, ep1.w - range_upper_limit_alpha);

    ep0_range_error_low.x = MIN(0.0f, ep0.x);
    ep0_range_error_low.y = MIN(0.0f, ep0.y);
    ep0_range_error_low.z = MIN(0.0f, ep0.z);
    ep0_range_error_low.w = MIN(0.0f, ep0.w);
    ep1_range_error_low.x = MIN(0.0f, ep1.x);
    ep1_range_error_low.y = MIN(0.0f, ep1.y);
    ep1_range_error_low.z = MIN(0.0f, ep1.z);
    ep1_range_error_low.w = MIN(0.0f, ep1.w);

    float4 sum_range_error =
        (ep0_range_error_low * ep0_range_error_low) + (ep1_range_error_low * ep1_range_error_low) + (ep0_range_error_high * ep0_range_error_high) + (ep1_range_error_high * ep1_range_error_high);
    float rgb_range_error = dot(sum_range_error.xyz, error_weight.xyz) * 0.5f * partition_size;
    float alpha_range_error = sum_range_error.w * error_weight.w * 0.5f * partition_size;

    if (encode_hdr_rgb) {

        // collect some statistics
        float af, cf;
        if (ep1.x > ep1.y && ep1.x > ep1.z) {
            af = ep1.x;
            cf = ep1.x - ep0.x;
        } else if (ep1.y > ep1.z) {
            af = ep1.y;
            cf = ep1.y - ep0.y;
        } else {
            af = ep1.z;
            cf = ep1.z - ep0.z;
        }

        float bf = af - ep1_min;    // estimate of color-component spread in high endpoint color
        float3 cf3 = { cf, cf, cf };
        float3 prd = ep1.xyz - cf3;
        float3 pdif = prd - ep0.xyz;
        // estimate of color-component spread in low endpoint color
        float df = (float)MAX(MAX(fabs(pdif.x), fabs(pdif.y)), fabs(pdif.z));

        int b = (int)bf;
        int c = (int)cf;
        int d = (int)df;


        // determine which one of the 6 submodes is likely to be used in
        // case of an RGBO-mode
        int rgbo_mode = 5;        // 7 bits per component
        // mode 4: 8 7 6
        if (b < 32768 && c < 16384)
            rgbo_mode = 4;
        // mode 3: 9 6 7
        if (b < 8192 && c < 16384)
            rgbo_mode = 3;
        // mode 2: 10 5 8
        if (b < 2048 && c < 16384)
            rgbo_mode = 2;
        // mode 1: 11 6 5
        if (b < 2048 && c < 1024)
            rgbo_mode = 1;
        // mode 0: 11 5 7
        if (b < 1024 && c < 4096)
            rgbo_mode = 0;

        // determine which one of the 9 submodes is likely to be used in
        // case of an RGB-mode.
        int rgb_mode = 8;        // 8 bits per component, except 7 bits for blue

        // mode 0: 9 7 6 7
        if (b < 16384 && c < 8192 && d < 8192)
            rgb_mode = 0;
        // mode 1: 9 8 6 6
        if (b < 32768 && c < 8192 && d < 4096)
            rgb_mode = 1;
        // mode 2: 10 6 7 7
        if (b < 4096 && c < 8192 && d < 4096)
            rgb_mode = 2;
        // mode 3: 10 7 7 6
        if (b < 8192 && c < 8192 && d < 2048)
            rgb_mode = 3;
        // mode 4: 11 8 6 5
        if (b < 8192 && c < 2048 && d < 512)
            rgb_mode = 4;
        // mode 5: 11 6 8 6
        if (b < 2048 && c < 8192 && d < 1024)
            rgb_mode = 5;
        // mode 6: 12 7 7 5
        if (b < 2048 && c < 2048 && d < 256)
            rgb_mode = 6;
        // mode 7: 12 6 7 6
        if (b < 1024 && c < 2048 && d < 512)
            rgb_mode = 7;

        float mode7mult = rgbo_error_scales[rgbo_mode] * 0.0015f;    // empirically determined ....
        float mode11mult = rgb_error_scales[rgb_mode] * 0.010f;    // empirically determined ....

        float lum_high = (ep1.x + ep1.y + ep1.z) * (1.0f / 3.0f);
        float lum_low = (ep0.x + ep0.y + ep0.z) * (1.0f / 3.0f);
        float lumdif = lum_high - lum_low;
        float mode23mult = lumdif < 960 ? 4.0f : lumdif < 3968 ? 16.0f : 128.0f;

        mode23mult *= 0.0005f;    // empirically determined ....

        // pick among the available HDR endpoint modes
        for (i = 0; i < 8; i++) {
            best_error[i][3] = FLOAT_30;
            format_of_choice[i][3] = encode_hdr_alpha ? FMT_HDR_RGBA : FMT_HDR_RGB_LDR_ALPHA;
            best_error[i][2] = FLOAT_30;
            format_of_choice[i][2] = FMT_HDR_RGB;
            best_error[i][1] = FLOAT_30;
            format_of_choice[i][1] = FMT_HDR_RGB_SCALE;
            best_error[i][0] = FLOAT_30;
            format_of_choice[i][0] = FMT_HDR_LUMINANCE_LARGE_RANGE;
        }


        for (i = 8; i < 21; i++) {
            // base_quant_error should depend on the scale-factor that would be used
            // during actual encode of the color value.

            float base_quant_error = baseline_quant_error[i] * partition_size * 1.0f;
            float rgb_quantization_error = error_weight_rgbsum * base_quant_error * 2.0f;
            float alpha_quantization_error = error_weight.w * base_quant_error * 2.0f;
            float rgba_quantization_error = rgb_quantization_error + alpha_quantization_error;

            // for 8 integers, we have two encodings: one with HDR alpha and another one
            // with LDR alpha.

            float full_hdr_rgba_error = rgba_quantization_error + rgb_range_error + alpha_range_error;
            best_error[i][3] = full_hdr_rgba_error;
            format_of_choice[i][3] = encode_hdr_alpha ? FMT_HDR_RGBA : FMT_HDR_RGB_LDR_ALPHA;

            // for 6 integers, we have one HDR-RGB encoding
            float full_hdr_rgb_error = (rgb_quantization_error * mode11mult) + rgb_range_error + eci->alpha_drop_error;
            best_error[i][2] = full_hdr_rgb_error;
            format_of_choice[i][2] = FMT_HDR_RGB;

            // for 4 integers, we have one HDR-RGB-Scale encoding
            float hdr_rgb_scale_error = (rgb_quantization_error * mode7mult) + rgb_range_error + eci->alpha_drop_error + eci->rgb_luma_error;

            best_error[i][1] = hdr_rgb_scale_error;
            format_of_choice[i][1] = FMT_HDR_RGB_SCALE;

            // for 2 integers, we assume luminance-with-large-range
            float hdr_luminance_error = (rgb_quantization_error * mode23mult) + rgb_range_error + eci->alpha_drop_error + eci->luminance_error;
            best_error[i][0] = hdr_luminance_error;
            format_of_choice[i][0] = FMT_HDR_LUMINANCE_LARGE_RANGE;
        }
    }


    else {
        for (i = 0; i < 4; i++) {
            best_error[i][3] = FLOAT_30;
            best_error[i][2] = FLOAT_30;
            best_error[i][1] = FLOAT_30;
            best_error[i][0] = FLOAT_30;

            format_of_choice[i][3] = FMT_RGBA;
            format_of_choice[i][2] = FMT_RGB;
            format_of_choice[i][1] = FMT_RGB_SCALE;
            format_of_choice[i][0] = FMT_LUMINANCE;
        }


        // pick among the available LDR endpoint modes
        for (i = 4; i < 21; i++) {
            float base_quant_error = baseline_quant_error[i] * partition_size * 1.0f;
            float rgb_quantization_error = error_weight_rgbsum * base_quant_error;
            float alpha_quantization_error = error_weight.w * base_quant_error;
            float rgba_quantization_error = rgb_quantization_error + alpha_quantization_error;

            // for 8 integers, the available encodings are:
            // full LDR RGB-Alpha
            float full_ldr_rgba_error = rgba_quantization_error;
            if (eci->can_blue_contract)
                full_ldr_rgba_error *= 0.625f;
            if (eci->can_offset_encode && i <= 18)
                full_ldr_rgba_error *= 0.5f;
            full_ldr_rgba_error += rgb_range_error + alpha_range_error;

            best_error[i][3] = full_ldr_rgba_error;
            format_of_choice[i][3] = FMT_RGBA;

            // for 6 integers, we have:
            // - an LDR-RGB encoding
            // - an RGBS + Alpha encoding (LDR)

            float full_ldr_rgb_error = rgb_quantization_error;
            if (eci->can_blue_contract)
                full_ldr_rgb_error *= 0.5f;
            if (eci->can_offset_encode && i <= 18)
                full_ldr_rgb_error *= 0.25f;
            full_ldr_rgb_error += eci->alpha_drop_error + rgb_range_error;

            float rgbs_alpha_error = rgba_quantization_error + eci->rgb_scale_error + rgb_range_error + alpha_range_error;

            if (rgbs_alpha_error < full_ldr_rgb_error) {
                best_error[i][2] = rgbs_alpha_error;
                format_of_choice[i][2] = FMT_RGB_SCALE_ALPHA;
            } else {
                best_error[i][2] = full_ldr_rgb_error;
                format_of_choice[i][2] = FMT_RGB;
            }


            // for 4 integers, we have a Luminance-Alpha encoding and the RGBS encoding
            float ldr_rgbs_error = rgb_quantization_error + eci->alpha_drop_error + eci->rgb_scale_error + rgb_range_error;

            float lum_alpha_error = rgba_quantization_error + eci->luminance_error + rgb_range_error + alpha_range_error;

            if (ldr_rgbs_error < lum_alpha_error) {
                best_error[i][1] = ldr_rgbs_error;
                format_of_choice[i][1] = FMT_RGB_SCALE;
            } else {
                best_error[i][1] = lum_alpha_error;
                format_of_choice[i][1] = FMT_LUMINANCE_ALPHA;
            }


            // for 2 integers, we have a Luminance-encoding and an Alpha-encoding.
            float luminance_error = rgb_quantization_error + eci->alpha_drop_error + eci->luminance_error + rgb_range_error;

            best_error[i][0] = luminance_error;
            format_of_choice[i][0] = FMT_LUMINANCE;
        }
    }
}

// for 1 partition, find the best combination (one format + a quantization level) for a given bitcount
//static
void one_partition_find_best_combination_for_bitcount(float combined_best_error[21][4],
        int formats_of_choice[21][4], int bits_available, int *best_quantization_level, int *best_formats, float *error_of_best_combination, __global ASTC_Encode *ASTCEncode) {
    int i;
    int best_integer_count = -1;
    float best_integer_count_error = FLOAT_20;
    for (i = 0; i < 4; i++) {
        // compute the quantization level for a given number of integers and a given number of bits.
        int quantization_level = ASTCEncode->quantization_mode_table[i + 1][bits_available];
        if (quantization_level == -1)
            continue;            // used to indicate the case where we don't have enoug bits to represent a given endpoint format at all.
        if (combined_best_error[quantization_level][i] < best_integer_count_error) {
            best_integer_count_error = combined_best_error[quantization_level][i];
            best_integer_count = i;
        }
    }

    int ql = ASTCEncode->quantization_mode_table[best_integer_count + 1][bits_available];

    *best_quantization_level = ql;
    *error_of_best_combination = best_integer_count_error;
    if (ql >= 0)
        *best_formats = formats_of_choice[ql][best_integer_count];
    else
        *best_formats = FMT_LUMINANCE;

}

// for 2 partitions, find the best format combinations for every (quantization-mode, integer-count) combination
//static
void two_partitions_find_best_combination_for_every_quantization_and_integer_count(float best_error[2][21][4],    // indexed by (partition, quant-level, integer-pair-count-minus-1)
        int format_of_choice[2][21][4],
        float combined_best_error[21][7],    // indexed by (quant-level, integer-pair-count-minus-2)
        int formats_of_choice[21][7][2]) {
    int i, j;

    for (i = 0; i < 21; i++)
        for (j = 0; j < 7; j++)
            combined_best_error[i][j] = FLOAT_30;

    int quant;
    for (quant = 5; quant < 21; quant++) {
        for (i = 0; i < 4; i++) {  // integer-count for first endpoint-pair
            for (j = 0; j < 4; j++) {  // integer-count for second endpoint-pair
                int low2 = MIN(i, j);
                int high2 = MAX(i, j);
                if ((high2 - low2) > 1)
                    continue;

                int intcnt = i + j;
                float errorterm = MIN(best_error[0][quant][i] + best_error[1][quant][j], FLOAT_10);
                if (errorterm <= combined_best_error[quant][intcnt]) {
                    combined_best_error[quant][intcnt] = errorterm;
                    formats_of_choice[quant][intcnt][0] = format_of_choice[0][quant][i];
                    formats_of_choice[quant][intcnt][1] = format_of_choice[1][quant][j];
                }
            }
        }
    }
}

// for 2 partitions, find the best combination (two formats + a quantization level) for a given bitcount
//static
void two_partitions_find_best_combination_for_bitcount(float combined_best_error[21][7],
        int formats_of_choice[21][7][2],
        int bits_available, int
        *best_quantization_level,
        int *best_quantization_level_mod,
        int *best_formats,
        float *error_of_best_combination,
        __global ASTC_Encode *ASTCEncode) {
    int i;

    int best_integer_count = 0;
    float best_integer_count_error = FLOAT_20;
    int integer_count;

    for (integer_count = 2; integer_count <= 8; integer_count++) {
        // compute the quantization level for a given number of integers and a given number of bits.
        int quantization_level = ASTCEncode->quantization_mode_table[integer_count][bits_available];
        if (quantization_level == -1)
            break;                // used to indicate the case where we don't have enoug bits to represent a given endpoint format at all.
        float integer_count_error = combined_best_error[quantization_level][integer_count - 2];
        if (integer_count_error < best_integer_count_error) {
            best_integer_count_error = integer_count_error;
            best_integer_count = integer_count;
        }
    }

    int ql = ASTCEncode->quantization_mode_table[best_integer_count][bits_available];
    int ql_mod = ASTCEncode->quantization_mode_table[best_integer_count][bits_available + 2];

    *best_quantization_level = ql;
    *best_quantization_level_mod = ql_mod;
    *error_of_best_combination = best_integer_count_error;
    if (ql >= 0) {
        for (i = 0; i < 2; i++)
            best_formats[i] = formats_of_choice[ql][best_integer_count - 2][i];
    } else {
        for (i = 0; i < 2; i++)
            best_formats[i] = FMT_LUMINANCE;
    }
}

#ifdef ENABLE_3_PARTITION_CODE
// 4
// for 3 partitions, find the best format combinations for every (quantization-mode, integer-count) combination
static void three_partitions_find_best_combination_for_every_quantization_and_integer_count(float best_error[3][21][4],    // indexed by (partition, quant-level, integer-count)
        int format_of_choice[3][21][4], float combined_best_error[21][10], int formats_of_choice[21][10][3]) {
    int i, j, k;

    for (i = 0; i < 21; i++)
        for (j = 0; j < 10; j++)
            combined_best_error[i][j] = FLOAT_30;

    int quant;
    for (quant = 5; quant < 21; quant++) {
        for (i = 0; i < 4; i++) {  // integer-count for first endpoint-pair
            for (j = 0; j < 4; j++) {  // integer-count for second endpoint-pair
                int low2 = MIN(i, j);
                int high2 = MAX(i, j);
                if ((high2 - low2) > 1)
                    continue;
                for (k = 0; k < 4; k++) {  // integer-count for third endpoint-pair
                    int low3 = MIN(k, low2);
                    int high3 = MAX(k, high2);
                    if ((high3 - low3) > 1)
                        continue;

                    int intcnt = i + j + k;
                    float errorterm = MIN(best_error[0][quant][i] + best_error[1][quant][j] + best_error[2][quant][k], FLOAT_10);
                    if (errorterm <= combined_best_error[quant][intcnt]) {
                        combined_best_error[quant][intcnt] = errorterm;
                        formats_of_choice[quant][intcnt][0] = format_of_choice[0][quant][i];
                        formats_of_choice[quant][intcnt][1] = format_of_choice[1][quant][j];
                        formats_of_choice[quant][intcnt][2] = format_of_choice[2][quant][k];
                    }
                }
            }
        }
    }
}


// 4
static void three_partitions_find_best_combination_for_bitcount(float combined_best_error[21][10],
        int formats_of_choice[21][10][3],
        int bits_available, int *best_quantization_level, int *best_quantization_level_mod, int *best_formats, float *error_of_best_combination, __global ASTC_Encode *ASTCEncode) {
    int i;

    int best_integer_count = 0;
    float best_integer_count_error = FLOAT_20;
    int integer_count;

    for (integer_count = 3; integer_count <= 9; integer_count++) {
        // compute the quantization level for a given number of integers and a given number of bits.
        int quantization_level = ASTCEncode->quantization_mode_table[integer_count][bits_available];
        if (quantization_level == -1)
            break;                // used to indicate the case where we don't have enough bits to represent a given endpoint format at all.
        float integer_count_error = combined_best_error[quantization_level][integer_count - 3];
        if (integer_count_error < best_integer_count_error) {
            best_integer_count_error = integer_count_error;
            best_integer_count = integer_count;
        }
    }

    int ql = ASTCEncode->quantization_mode_table[best_integer_count][bits_available];
    int ql_mod = ASTCEncode->quantization_mode_table[best_integer_count][bits_available + 5];

    *best_quantization_level = ql;
    *best_quantization_level_mod = ql_mod;
    *error_of_best_combination = best_integer_count_error;
    if (ql >= 0) {
        for (i = 0; i < 3; i++)
            best_formats[i] = formats_of_choice[ql][best_integer_count - 3][i];
    } else {
        for (i = 0; i < 3; i++)
            best_formats[i] = FMT_LUMINANCE;
    }
}
#endif

#ifdef ENABLE_4_PARTITION_CODE
// 4
// for 4 partitions, find the best format combinations for every (quantization-mode, integer-count) combination
static void four_partitions_find_best_combination_for_every_quantization_and_integer_count(float best_error[4][21][4],    // indexed by (partition, quant-level, integer-count)
        int format_of_choice[4][21][4], float combined_best_error[21][13], int formats_of_choice[21][13][4]) {
    int i, j, k, l;

    for (i = 0; i < 21; i++)
        for (j = 0; j < 13; j++)
            combined_best_error[i][j] = FLOAT_30;

    int quant;
    for (quant = 5; quant < 21; quant++) {
        for (i = 0; i < 4; i++) {  // integer-count for first endpoint-pair
            for (j = 0; j < 4; j++) {  // integer-count for second endpoint-pair
                int low2 = MIN(i, j);
                int high2 = MAX(i, j);
                if ((high2 - low2) > 1)
                    continue;
                for (k = 0; k < 4; k++) {  // integer-count for third endpoint-pair
                    int low3 = MIN(k, low2);
                    int high3 = MAX(k, high2);
                    if ((high3 - low3) > 1)
                        continue;
                    for (l = 0; l < 4; l++) {  // integer-count for fourth endpoint-pair
                        int low4 = MIN(l, low3);
                        int high4 = MAX(l, high3);
                        if ((high4 - low4) > 1)
                            continue;

                        int intcnt = i + j + k + l;
                        float errorterm = MIN(best_error[0][quant][i] + best_error[1][quant][j] + best_error[2][quant][k] + best_error[3][quant][l], FLOAT_10);
                        if (errorterm <= combined_best_error[quant][intcnt]) {
                            combined_best_error[quant][intcnt] = errorterm;
                            formats_of_choice[quant][intcnt][0] = format_of_choice[0][quant][i];
                            formats_of_choice[quant][intcnt][1] = format_of_choice[1][quant][j];
                            formats_of_choice[quant][intcnt][2] = format_of_choice[2][quant][k];
                            formats_of_choice[quant][intcnt][3] = format_of_choice[3][quant][l];
                        }
                    }
                }
            }
        }
    }
}

// 4
// for 4 partitions, find the best combination (four formats + a quantization level) for a given bitcount
static void four_partitions_find_best_combination_for_bitcount(float combined_best_error[21][13],
        int formats_of_choice[21][13][4],
        int bits_available, int *best_quantization_level, int *best_quantization_level_mod, int *best_formats, float *error_of_best_combination, __global ASTC_Encode *ASTCEncode) {
    int i;
    int best_integer_count = -4;
    float best_integer_count_error = FLOAT_20;
    int integer_count;

    for (integer_count = 4; integer_count <= 9; integer_count++) {
        // compute the quantization level for a given number of integers and a given number of bits.
        int quantization_level = ASTCEncode->quantization_mode_table[integer_count][bits_available];
        if (quantization_level == -1)
            break;                // used to indicate the case where we don't have enoug bits to represent a given endpoint format at all.
        float integer_count_error = combined_best_error[quantization_level][integer_count - 4];
        if (integer_count_error < best_integer_count_error) {
            best_integer_count_error = integer_count_error;
            best_integer_count = integer_count;
        }
    }

    if (best_integer_count < 0) {
        best_integer_count = 0;
    }

    int ql = ASTCEncode->quantization_mode_table[best_integer_count][bits_available];
    int ql_mod = ASTCEncode->quantization_mode_table[best_integer_count][bits_available + 8];

    *best_quantization_level = ql;
    *best_quantization_level_mod = ql_mod;
    *error_of_best_combination = best_integer_count_error;
    if (ql >= 0) {
        for (i = 0; i < 4; i++)
            best_formats[i] = formats_of_choice[ql][best_integer_count - 4][i];
    } else {
        for (i = 0; i < 4; i++)
            best_formats[i] = FMT_LUMINANCE;
    }
}
#endif

void determine_optimal_set_of_endpoint_formats_to_use(
    __global partition_info * pt,
    imageblock * blk,
    error_weight_block * ewb,
    endpoints * ep,
    int separate_component,    // separate color component for 2-plane mode; -1 for single-plane mode
    // bitcounts and errors computed for the various quantization methods
    int *qwt_bitcounts, float *qwt_errors,
    // output data
    int partition_format_specifiers[4][4],
    int quantized_weight[4],
    int quantization_level[4], int quantization_level_mod[4],
    __global ASTC_Encode *ASTCEncode) {
    int i, j;
    int partition_count = pt->partition_count;

    int encode_hdr_rgb = blk->rgb_lns[0];
    int encode_hdr_alpha = blk->alpha_lns[0];


    // call a helper function to compute the errors that result from various
    // encoding choices (such as using luminance instead of RGB, discarding Alpha,
    // using RGB-scale in place of two separate RGB endpoints and so on)
    encoding_choice_errors eci[4];
    compute_encoding_choice_errors(blk, pt, ewb, separate_component, eci, ASTCEncode);

    // for each partition, compute the error weights to apply for that partition.
    float4 error_weightings[4];
    float4 dummied_color_scalefactors[4];    // only used to receive data
    compute_partition_error_color_weightings(ewb, pt, error_weightings, dummied_color_scalefactors, ASTCEncode);


    float best_error[4][21][4];
    int format_of_choice[4][21][4];
    for (i = 0; i < partition_count; i++)
        compute_color_error_for_every_integer_count_and_quantization_level(encode_hdr_rgb, encode_hdr_alpha, i, pt, &(eci[i]), ep, error_weightings, best_error[i], format_of_choice[i]);

    float errors_of_best_combination[MAX_WEIGHT_MODES];
    int best_quantization_levels[MAX_WEIGHT_MODES];
    int best_quantization_levels_mod[MAX_WEIGHT_MODES];
    int best_ep_formats[MAX_WEIGHT_MODES][4];

    // code for the case where the block contains 1 partition
    if (partition_count == 1) {
        int best_quantization_level;
        int best_format;
        float error_of_best_combination;
        for (i = 0; i < MAX_WEIGHT_MODES; i++) {
            if (qwt_errors[i] >= FLOAT_29) {
                errors_of_best_combination[i] = FLOAT_30;
                continue;
            }

            one_partition_find_best_combination_for_bitcount(best_error[0], format_of_choice[0], qwt_bitcounts[i],
                    &best_quantization_level, &best_format, &error_of_best_combination,ASTCEncode);
            error_of_best_combination += qwt_errors[i];

            errors_of_best_combination[i] = error_of_best_combination;
            best_quantization_levels[i] = best_quantization_level;
            best_quantization_levels_mod[i] = best_quantization_level;
            best_ep_formats[i][0] = best_format;
        }
    }

    // code for the case where the block contains 2 partitions
    else if (partition_count == 2) {
        int best_quantization_level;
        int best_quantization_level_mod;
        int best_formats[2];
        float error_of_best_combination;

        float combined_best_error[21][7];
        int formats_of_choice[21][7][2];

        two_partitions_find_best_combination_for_every_quantization_and_integer_count(best_error, format_of_choice, combined_best_error, formats_of_choice);


        for (i = 0; i < MAX_WEIGHT_MODES; i++) {
            if (qwt_errors[i] >= FLOAT_29) {
                errors_of_best_combination[i] = FLOAT_30;
                continue;
            }

            two_partitions_find_best_combination_for_bitcount(combined_best_error, formats_of_choice, qwt_bitcounts[i],
                    &best_quantization_level, &best_quantization_level_mod, best_formats, &error_of_best_combination,
                    ASTCEncode);

            error_of_best_combination += qwt_errors[i];

            errors_of_best_combination[i] = error_of_best_combination;
            best_quantization_levels[i] = best_quantization_level;
            best_quantization_levels_mod[i] = best_quantization_level_mod;
            best_ep_formats[i][0] = best_formats[0];
            best_ep_formats[i][1] = best_formats[1];
        }
    }

#ifdef ENABLE_3_PARTITION_CODE
    // code for the case where the block contains 3 partitions
    else if (partition_count == 3) {
        int best_quantization_level;
        int best_quantization_level_mod;
        int best_formats[3];
        float error_of_best_combination;

        float combined_best_error[21][10];
        int formats_of_choice[21][10][3];

        three_partitions_find_best_combination_for_every_quantization_and_integer_count(best_error, format_of_choice, combined_best_error, formats_of_choice);

        for (i = 0; i < MAX_WEIGHT_MODES; i++) {
            if (qwt_errors[i] >= FLOAT_29) {
                errors_of_best_combination[i] = FLOAT_30;
                continue;
            }

            three_partitions_find_best_combination_for_bitcount(combined_best_error,
                    formats_of_choice, qwt_bitcounts[i], &best_quantization_level, &best_quantization_level_mod, best_formats,
                    &error_of_best_combination, ASTCEncode);
            error_of_best_combination += qwt_errors[i];

            errors_of_best_combination[i] = error_of_best_combination;
            best_quantization_levels[i] = best_quantization_level;
            best_quantization_levels_mod[i] = best_quantization_level_mod;
            best_ep_formats[i][0] = best_formats[0];
            best_ep_formats[i][1] = best_formats[1];
            best_ep_formats[i][2] = best_formats[2];
        }
    }
#endif

#ifdef ENABLE_4_PARTITION_CODE
    // code for the case where the block contains 4 partitions
    else if (partition_count == 4) {
        int best_quantization_level;
        int best_quantization_level_mod;
        int best_formats[4];
        float error_of_best_combination;

        float combined_best_error[21][13];
        int formats_of_choice[21][13][4];

        four_partitions_find_best_combination_for_every_quantization_and_integer_count(best_error, format_of_choice, combined_best_error, formats_of_choice);

        for (i = 0; i < MAX_WEIGHT_MODES; i++) {
            if (qwt_errors[i] >= FLOAT_29) {
                errors_of_best_combination[i] = FLOAT_30;
                continue;
            }
            four_partitions_find_best_combination_for_bitcount(combined_best_error,
                    formats_of_choice, qwt_bitcounts[i], &best_quantization_level, &best_quantization_level_mod, best_formats,
                    &error_of_best_combination, ASTCEncode);
            error_of_best_combination += qwt_errors[i];

            errors_of_best_combination[i] = error_of_best_combination;
            best_quantization_levels[i] = best_quantization_level;
            best_quantization_levels_mod[i] = best_quantization_level_mod;
            best_ep_formats[i][0] = best_formats[0];
            best_ep_formats[i][1] = best_formats[1];
            best_ep_formats[i][2] = best_formats[2];
            best_ep_formats[i][3] = best_formats[3];
        }
    }
#endif

    // finally, go through the results and pick the 4 best-looking modes.

    int best_error_weights[4];

    for (i = 0; i < 4; i++) {
        float best_ep_error = FLOAT_30;
        int best_error_index = -1;
        for (j = 0; j < MAX_WEIGHT_MODES; j++) {
            if (errors_of_best_combination[j] < best_ep_error && best_quantization_levels[j] >= 5) {
                best_ep_error = errors_of_best_combination[j];
                best_error_index = j;
            }
        }
        best_error_weights[i] = best_error_index;

        if(best_error_index >= 0) {
            errors_of_best_combination[best_error_index] = FLOAT_30;
        }
    }

    for (i = 0; i < 4; i++) {
        quantized_weight[i] = best_error_weights[i];
        if (quantized_weight[i] >= 0) {
            quantization_level[i] = best_quantization_levels[best_error_weights[i]];
            quantization_level_mod[i] = best_quantization_levels_mod[best_error_weights[i]];
            for (j = 0; j < partition_count; j++) {
                partition_format_specifiers[i][j] = best_ep_formats[best_error_weights[i]][j];
            }
        }
    }
}

//==================================================================================

/*
     quantize an LDR RGB color. Since this is a fallback encoding, we cannot actually
     fail but must just go on until we can produce a sensible result.

     Due to how this encoding works, color0 cannot be larger than color1; as such,
     if color0 is actually larger than color1, then color0 is reduced and color1 is
     increased until color0 is no longer larger than color1.
*/
int cqt_lookup(int quantization_level, int value) {
    if (value < 0)
        value = 0;
    else if (value > 255)
        value = 255;
    return color_quantization_tables[quantization_level][value];
}

// clamp an input value to [0,255]; NaN is turned into 0
float clamp255(float val) {
    if (val > 255.0f)
        val = 255.0f;
    else if (val > 0.0f) {
        // deliberately empty
        // switching the order of calculation here will fail to handle 0.
    } else
        val = 0.0f;

    return val;
}

// clamp an input value to [0,1]; Nan is turned into 0.
float clamp01(float val) {
    if (val > 1.0f)
        val = 1.0f;
    else if (val > 0.0f) {
        // deliberately empty
        // switching the order of calculation here will fail to handle 0.
    } else
        val = 0.0f;

    return val;
}

void quantize_rgb(
    float4 color0,    // LDR: 0=lowest, 255=highest
    float4 color1,
    int output[6],
    int quantization_level) {
    color0.xyz = color0.xyz * (1.0f / 257.0f);
    color1.xyz = color1.xyz * (1.0f / 257.0f);


    float r0 = clamp255(color0.x);
    float g0 = clamp255(color0.y);
    float b0 = clamp255(color0.z);

    float r1 = clamp255(color1.x);
    float g1 = clamp255(color1.y);
    float b1 = clamp255(color1.z);

    int ri0, gi0, bi0, ri1, gi1, bi1;
    int ri0b, gi0b, bi0b, ri1b, gi1b, bi1b;
    float rgb0_addon = 0.5f;
    float rgb1_addon = 0.5f;
    int iters = 0;
    do {
        ri0 = cqt_lookup(quantization_level, (int)floor(r0 + rgb0_addon));
        gi0 = cqt_lookup(quantization_level, (int)floor(g0 + rgb0_addon));
        bi0 = cqt_lookup(quantization_level, (int)floor(b0 + rgb0_addon));
        ri1 = cqt_lookup(quantization_level, (int)floor(r1 + rgb1_addon));
        gi1 = cqt_lookup(quantization_level, (int)floor(g1 + rgb1_addon));
        bi1 = cqt_lookup(quantization_level, (int)floor(b1 + rgb1_addon));

        ri0b = color_unquantization_tables[quantization_level][ri0];
        gi0b = color_unquantization_tables[quantization_level][gi0];
        bi0b = color_unquantization_tables[quantization_level][bi0];
        ri1b = color_unquantization_tables[quantization_level][ri1];
        gi1b = color_unquantization_tables[quantization_level][gi1];
        bi1b = color_unquantization_tables[quantization_level][bi1];

        rgb0_addon -= 0.2f;
        rgb1_addon += 0.2f;
        iters++;
    } while (ri0b + gi0b + bi0b > ri1b + gi1b + bi1b);

    output[0] = ri0;
    output[1] = ri1;
    output[2] = gi0;
    output[3] = gi1;
    output[4] = bi0;
    output[5] = bi1;
}

/* quantize an RGBA color. */
void quantize_rgba(
    float4 color0,
    float4 color1,
    int output[8], int quantization_level) {
    color0.w = color0.w * (1.0f / 257.0f);
    color1.w = color1.w * (1.0f / 257.0f);

    float a0 = clamp255(color0.w);
    float a1 = clamp255(color1.w);
    int ai0 = color_quantization_tables[quantization_level][(int)floor(a0 + 0.5f)];
    int ai1 = color_quantization_tables[quantization_level][(int)floor(a1 + 0.5f)];

    output[6] = ai0;
    output[7] = ai1;

    quantize_rgb(color0, color1, output, quantization_level);
}

/* attempt to quantize RGB endpoint values with blue-contraction. Returns 1 on failure, 0 on success. */
int try_quantize_rgb_blue_contract(
    float4 color0,    // assumed to be the smaller color
    float4 color1,    // assumed to be the larger color
    int output[6], int quantization_level) {
    color0.xyz = color0.xyz * (1.0f / 257.0f);
    color1.xyz = color1.xyz * (1.0f / 257.0f);

    float r0 = color0.x;
    float g0 = color0.y;
    float b0 = color0.z;

    float r1 = color1.x;
    float g1 = color1.y;
    float b1 = color1.z;

    // inverse blue-contraction. This can produce an overflow;
    // just bail out immediately if this is the case.
    r0 += (r0 - b0);
    g0 += (g0 - b0);
    r1 += (r1 - b1);
    g1 += (g1 - b1);

    if (r0 < 0.0f || r0 > 255.0f || g0 < 0.0f || g0 > 255.0f || b0 < 0.0f || b0 > 255.0f ||
            r1 < 0.0f || r1 > 255.0f || g1 < 0.0f || g1 > 255.0f || b1 < 0.0f || b1 > 255.0f) {
        return 0;
    }

    // quantize the inverse-blue-contracted color
    int ri0 = color_quantization_tables[quantization_level][(int)floor(r0 + 0.5f)];
    int gi0 = color_quantization_tables[quantization_level][(int)floor(g0 + 0.5f)];
    int bi0 = color_quantization_tables[quantization_level][(int)floor(b0 + 0.5f)];
    int ri1 = color_quantization_tables[quantization_level][(int)floor(r1 + 0.5f)];
    int gi1 = color_quantization_tables[quantization_level][(int)floor(g1 + 0.5f)];
    int bi1 = color_quantization_tables[quantization_level][(int)floor(b1 + 0.5f)];

    // then unquantize again
    int ru0 = color_unquantization_tables[quantization_level][ri0];
    int gu0 = color_unquantization_tables[quantization_level][gi0];
    int bu0 = color_unquantization_tables[quantization_level][bi0];
    int ru1 = color_unquantization_tables[quantization_level][ri1];
    int gu1 = color_unquantization_tables[quantization_level][gi1];
    int bu1 = color_unquantization_tables[quantization_level][bi1];

    // if color #1 is not larger than color #0, then blue-contraction is not a valid approach.
    // note that blue-contraction and quantization may itself change this order, which is why
    // we must only test AFTER blue-contraction.
    if (ru1 + gu1 + bu1 <= ru0 + gu0 + bu0)
        return 0;

    output[0] = ri1;
    output[1] = ri0;
    output[2] = gi1;
    output[3] = gi0;
    output[4] = bi1;
    output[5] = bi0;

    return 1;
}

/* quantize an RGBA color with blue-contraction */
int try_quantize_rgba_blue_contract(float4 color0, float4 color1, int output[8], int quantization_level) {
    color0.w = color0.w * (1.0f / 257.0f);
    color1.w = color1.w * (1.0f / 257.0f);

    float a0 = clamp255(color0.w);
    float a1 = clamp255(color1.w);

    output[7] = color_quantization_tables[quantization_level][(int)floor(a0 + 0.5f)];
    output[6] = color_quantization_tables[quantization_level][(int)floor(a1 + 0.5f)];

    return try_quantize_rgb_blue_contract(color0, color1, output, quantization_level);
}

// delta-encoding:
// at decode time, we move one bit from the offset to the base and seize another bit as a sign bit;
// we then unquantize both values as if they contain one extra bit.
// if the sum of the offsets is nonnegative, then we encode a regular delta.
/* attempt to quantize an RGB endpoint value with delta-encoding. */
int try_quantize_rgb_delta(float4 color0, float4 color1, int output[6], int quantization_level) {
    color0.xyz = color0.xyz * (1.0f / 257.0f);
    color1.xyz = color1.xyz * (1.0f / 257.0f);

    float r0 = clamp255(color0.x);
    float g0 = clamp255(color0.y);
    float b0 = clamp255(color0.z);

    float r1 = clamp255(color1.x);
    float g1 = clamp255(color1.y);
    float b1 = clamp255(color1.z);

    // transform r0 to unorm9
    int r0a = (int)floor(r0 + 0.5f);
    int g0a = (int)floor(g0 + 0.5f);
    int b0a = (int)floor(b0 + 0.5f);
    r0a <<= 1;
    g0a <<= 1;
    b0a <<= 1;

    // mask off the top bit
    int r0b = r0a & 0xFF;
    int g0b = g0a & 0xFF;
    int b0b = b0a & 0xFF;

    // quantize, then unquantize in order to get a value that we take
    // differences against.
    int r0be = color_quantization_tables[quantization_level][r0b];
    int g0be = color_quantization_tables[quantization_level][g0b];
    int b0be = color_quantization_tables[quantization_level][b0b];

    r0b = color_unquantization_tables[quantization_level][r0be];
    g0b = color_unquantization_tables[quantization_level][g0be];
    b0b = color_unquantization_tables[quantization_level][b0be];
    r0b |= r0a & 0x100;            // final unquantized-values for endpoint 0.
    g0b |= g0a & 0x100;
    b0b |= b0a & 0x100;

    // then, get hold of the second value
    int r1d = (int)floor(r1 + 0.5f);
    int g1d = (int)floor(g1 + 0.5f);
    int b1d = (int)floor(b1 + 0.5f);

    r1d <<= 1;
    g1d <<= 1;
    b1d <<= 1;
    // and take differences!
    r1d -= r0b;
    g1d -= g0b;
    b1d -= b0b;

    // check if the difference is too large to be encodable.
    if (r1d > 63 || g1d > 63 || b1d > 63 || r1d < -64 || g1d < -64 || b1d < -64)
        return 0;

    // insert top bit of the base into the offset
    r1d &= 0x7F;
    g1d &= 0x7F;
    b1d &= 0x7F;

    r1d |= (r0b & 0x100) >> 1;
    g1d |= (g0b & 0x100) >> 1;
    b1d |= (b0b & 0x100) >> 1;

    // then quantize & unquantize; if this causes any of the top two bits to flip,
    // then encoding fails, since we have then corrupted either the top bit of the base
    // or the sign bit of the offset.
    int r1de = color_quantization_tables[quantization_level][r1d];
    int g1de = color_quantization_tables[quantization_level][g1d];
    int b1de = color_quantization_tables[quantization_level][b1d];

    int r1du = color_unquantization_tables[quantization_level][r1de];
    int g1du = color_unquantization_tables[quantization_level][g1de];
    int b1du = color_unquantization_tables[quantization_level][b1de];

    if (((r1d ^ r1du) | (g1d ^ g1du) | (b1d ^ b1du)) & 0xC0)
        return 0;

    // check that the sum of the encoded offsets is nonnegative, else encoding fails
    r1du &= 0x7f;
    g1du &= 0x7f;
    b1du &= 0x7f;
    if (r1du & 0x40)
        r1du -= 0x80;
    if (g1du & 0x40)
        g1du -= 0x80;
    if (b1du & 0x40)
        b1du -= 0x80;
    if (r1du + g1du + b1du < 0)
        return 0;

    // check that the offsets produce legitimate sums as well.
    r1du += r0b;
    g1du += g0b;
    b1du += b0b;
    if (r1du < 0 || r1du > 0x1FF || g1du < 0 || g1du > 0x1FF || b1du < 0 || b1du > 0x1FF)
        return 0;

    // OK, we've come this far; we can now encode legitimate values.
    output[0] = r0be;
    output[1] = r1de;
    output[2] = g0be;
    output[3] = g1de;
    output[4] = b0be;
    output[5] = b1de;

    return 1;
}

int try_quantize_rgb_delta_blue_contract(float4 color0, float4 color1, int output[6], int quantization_level) {
    color0.xyz = color0.xyz * (1.0f / 257.0f);
    color1.xyz = color1.xyz * (1.0f / 257.0f);

    // switch around endpoint colors already at start.
    float r0 = color1.x;
    float g0 = color1.y;
    float b0 = color1.z;

    float r1 = color0.x;
    float g1 = color0.y;
    float b1 = color0.z;

    // inverse blue-contraction. This step can perform an overflow, in which case
    // we will bail out immediately.
    r0 += (r0 - b0);
    g0 += (g0 - b0);
    r1 += (r1 - b1);
    g1 += (g1 - b1);

    if (r0 < 0.0f || r0 > 255.0f || g0 < 0.0f || g0 > 255.0f || b0 < 0.0f || b0 > 255.0f || r1 < 0.0f || r1 > 255.0f || g1 < 0.0f || g1 > 255.0f || b1 < 0.0f || b1 > 255.0f)
        return 0;

    // transform r0 to unorm9
    int r0a = (int)floor(r0 + 0.5f);
    int g0a = (int)floor(g0 + 0.5f);
    int b0a = (int)floor(b0 + 0.5f);
    r0a <<= 1;
    g0a <<= 1;
    b0a <<= 1;

    // mask off the top bit
    int r0b = r0a & 0xFF;
    int g0b = g0a & 0xFF;
    int b0b = b0a & 0xFF;

    // quantize, then unquantize in order to get a value that we take
    // differences against.
    int r0be = color_quantization_tables[quantization_level][r0b];
    int g0be = color_quantization_tables[quantization_level][g0b];
    int b0be = color_quantization_tables[quantization_level][b0b];

    r0b = color_unquantization_tables[quantization_level][r0be];
    g0b = color_unquantization_tables[quantization_level][g0be];
    b0b = color_unquantization_tables[quantization_level][b0be];
    r0b |= r0a & 0x100;            // final unquantized-values for endpoint 0.
    g0b |= g0a & 0x100;
    b0b |= b0a & 0x100;

    // then, get hold of the second value
    int r1d = (int)floor(r1 + 0.5f);
    int g1d = (int)floor(g1 + 0.5f);
    int b1d = (int)floor(b1 + 0.5f);

    r1d <<= 1;
    g1d <<= 1;
    b1d <<= 1;
    // and take differences!
    r1d -= r0b;
    g1d -= g0b;
    b1d -= b0b;

    // check if the difference is too large to be encodable.
    if (r1d > 63 || g1d > 63 || b1d > 63 || r1d < -64 || g1d < -64 || b1d < -64)
        return 0;

    // insert top bit of the base into the offset
    r1d &= 0x7F;
    g1d &= 0x7F;
    b1d &= 0x7F;

    r1d |= (r0b & 0x100) >> 1;
    g1d |= (g0b & 0x100) >> 1;
    b1d |= (b0b & 0x100) >> 1;

    // then quantize & unquantize; if this causes any of the top two bits to flip,
    // then encoding fails, since we have then corrupted either the top bit of the base
    // or the sign bit of the offset.
    int r1de = color_quantization_tables[quantization_level][r1d];
    int g1de = color_quantization_tables[quantization_level][g1d];
    int b1de = color_quantization_tables[quantization_level][b1d];

    int r1du = color_unquantization_tables[quantization_level][r1de];
    int g1du = color_unquantization_tables[quantization_level][g1de];
    int b1du = color_unquantization_tables[quantization_level][b1de];

    if (((r1d ^ r1du) | (g1d ^ g1du) | (b1d ^ b1du)) & 0xC0)
        return 0;

    // check that the sum of the encoded offsets is negative, else encoding fails
    // note that this is inverse of the test for non-blue-contracted RGB.
    r1du &= 0x7f;
    g1du &= 0x7f;
    b1du &= 0x7f;
    if (r1du & 0x40)
        r1du -= 0x80;
    if (g1du & 0x40)
        g1du -= 0x80;
    if (b1du & 0x40)
        b1du -= 0x80;
    if (r1du + g1du + b1du >= 0)
        return 0;

    // check that the offsets produce legitimate sums as well.
    r1du += r0b;
    g1du += g0b;
    b1du += b0b;
    if (r1du < 0 || r1du > 0x1FF || g1du < 0 || g1du > 0x1FF || b1du < 0 || b1du > 0x1FF)
        return 0;

    // OK, we've come this far; we can now encode legitimate values.
    output[0] = r0be;
    output[1] = r1de;
    output[2] = g0be;
    output[3] = g1de;
    output[4] = b0be;
    output[5] = b1de;

    return 1;
}

int try_quantize_alpha_delta(float4 color0, float4 color1, int output[8], int quantization_level) {
    color0.w = color0.w * (1.0f / 257.0f);
    color1.w = color1.w * (1.0f / 257.0f);

    // the calculation for alpha-delta is exactly the same as for RGB-delta; see
    // the RGB-delta function for comments.
    float a0 = clamp255(color0.w);
    float a1 = clamp255(color1.w);

    int a0a = (int)floor(a0 + 0.5f);
    a0a <<= 1;
    int a0b = a0a & 0xFF;
    int a0be = color_quantization_tables[quantization_level][a0b];
    a0b = color_unquantization_tables[quantization_level][a0be];
    a0b |= a0a & 0x100;
    int a1d = (int)floor(a1 + 0.5f);
    a1d <<= 1;
    a1d -= a0b;
    if (a1d > 63 || a1d < -64)
        return 0;
    a1d &= 0x7F;
    a1d |= (a0b & 0x100) >> 1;
    int a1de = color_quantization_tables[quantization_level][a1d];
    int a1du = color_unquantization_tables[quantization_level][a1de];
    if ((a1d ^ a1du) & 0xC0)
        return 0;
    a1du &= 0x7F;
    if (a1du & 0x40)
        a1du -= 0x80;
    a1du += a0b;
    if (a1du < 0 || a1du > 0x1FF)
        return 0;
    output[6] = a0be;
    output[7] = a1de;
    return 1;
}

int try_quantize_luminance_alpha_delta(float4 color0, float4 color1, int output[8], int quantization_level) {
    float l0 = clamp255((color0.x + color0.y + color0.z) * ((1.0f / 3.0f) * (1.0f / 257.0f)));
    float l1 = clamp255((color1.x + color1.y + color1.z) * ((1.0f / 3.0f) * (1.0f / 257.0f)));
    float a0 = clamp255(color0.w * (1.0f / 257.0f));
    float a1 = clamp255(color1.w * (1.0f / 257.0f));

    int l0a = (int)floor(l0 + 0.5f);
    int a0a = (int)floor(a0 + 0.5f);
    l0a <<= 1;
    a0a <<= 1;
    int l0b = l0a & 0xFF;
    int a0b = a0a & 0xFF;
    int l0be = color_quantization_tables[quantization_level][l0b];
    int a0be = color_quantization_tables[quantization_level][a0b];
    l0b = color_unquantization_tables[quantization_level][l0be];
    a0b = color_unquantization_tables[quantization_level][a0be];
    l0b |= l0a & 0x100;
    a0b |= a0a & 0x100;
    int l1d = (int)floor(l1 + 0.5f);
    int a1d = (int)floor(a1 + 0.5f);
    l1d <<= 1;
    a1d <<= 1;
    l1d -= l0b;
    a1d -= a0b;
    if (l1d > 63 || l1d < -64)
        return 0;
    if (a1d > 63 || a1d < -64)
        return 0;
    l1d &= 0x7F;
    a1d &= 0x7F;
    l1d |= (l0b & 0x100) >> 1;
    a1d |= (a0b & 0x100) >> 1;

    int l1de = color_quantization_tables[quantization_level][l1d];
    int a1de = color_quantization_tables[quantization_level][a1d];
    int l1du = color_unquantization_tables[quantization_level][l1de];
    int a1du = color_unquantization_tables[quantization_level][a1de];
    if ((l1d ^ l1du) & 0xC0)
        return 0;
    if ((a1d ^ a1du) & 0xC0)
        return 0;
    l1du &= 0x7F;
    a1du &= 0x7F;
    if (l1du & 0x40)
        l1du -= 0x80;
    if (a1du & 0x40)
        a1du -= 0x80;
    l1du += l0b;
    a1du += a0b;
    if (l1du < 0 || l1du > 0x1FF)
        return 0;
    if (a1du < 0 || a1du > 0x1FF)
        return 0;
    output[0] = l0be;
    output[1] = l1de;
    output[2] = a0be;
    output[3] = a1de;

    return 1;
}

int try_quantize_rgba_delta(float4 color0, float4 color1, int output[8], int quantization_level) {
    int alpha_delta_res = try_quantize_alpha_delta(color0, color1, output, quantization_level);

    if (alpha_delta_res == 0)
        return 0;

    return try_quantize_rgb_delta(color0, color1, output, quantization_level);
}

int try_quantize_rgba_delta_blue_contract(float4 color0, float4 color1, int output[8], int quantization_level) {
    // notice that for the alpha encoding, we are swapping around color0 and color1;
    // this is because blue-contraction involves swapping around the two colors.
    int alpha_delta_res = try_quantize_alpha_delta(color1, color0, output, quantization_level);

    if (alpha_delta_res == 0)
        return 0;

    return try_quantize_rgb_delta_blue_contract(color0, color1, output, quantization_level);
}

void quantize_rgbs_new(float4 rgbs_color,    // W component is a desired-scale to apply, in the range 0..1
                       int output[4], int quantization_level) {
    rgbs_color.xyz = rgbs_color.xyz * (1.0f / 257.0f);

    float r = clamp255(rgbs_color.x);
    float g = clamp255(rgbs_color.y);
    float b = clamp255(rgbs_color.z);

    int ri = color_quantization_tables[quantization_level][(int)floor(r + 0.5f)];
    int gi = color_quantization_tables[quantization_level][(int)floor(g + 0.5f)];
    int bi = color_quantization_tables[quantization_level][(int)floor(b + 0.5f)];

    int ru = color_unquantization_tables[quantization_level][ri];
    int gu = color_unquantization_tables[quantization_level][gi];
    int bu = color_unquantization_tables[quantization_level][bi];

    float oldcolorsum = rgbs_color.x + rgbs_color.y + rgbs_color.z;
    float newcolorsum = (float)(ru + gu + bu);

    float scale = clamp01(rgbs_color.w * (oldcolorsum + FLOAT_n10) / (newcolorsum + FLOAT_n10));

    int scale_idx = (int)floor(scale * 256.0f + 0.5f);

    if (scale_idx < 0)
        scale_idx = 0;
    else if (scale_idx > 255)
        scale_idx = 255;

    output[0] = ri;
    output[1] = gi;
    output[2] = bi;
    output[3] = color_quantization_tables[quantization_level][scale_idx];
}

void quantize_rgbs_alpha_new(float4 color0, float4 color1, float4 rgbs_color, int output[6], int quantization_level) {
    color0.w = color0.w * (1.0f / 257.0f);
    color1.w = color1.w * (1.0f / 257.0f);

    float a0 = clamp255(color0.w);
    float a1 = clamp255(color1.w);

    int ai0 = color_quantization_tables[quantization_level][(int)floor(a0 + 0.5f)];
    int ai1 = color_quantization_tables[quantization_level][(int)floor(a1 + 0.5f)];

    output[4] = ai0;
    output[5] = ai1;

    quantize_rgbs_new(rgbs_color, output, quantization_level);
}

void quantize_luminance(float4 color0, float4 color1, int output[2], int quantization_level) {
    color0.xyz = color0.xyz * (1.0f / 257.0f);
    color1.xyz = color1.xyz * (1.0f / 257.0f);

    float lum0 = clamp255((color0.x + color0.y + color0.z) * (1.0f / 3.0f));
    float lum1 = clamp255((color1.x + color1.y + color1.z) * (1.0f / 3.0f));

    if (lum0 > lum1) {
        float avg = (lum0 + lum1) * 0.5f;
        lum0 = avg;
        lum1 = avg;
    }

    output[0] = color_quantization_tables[quantization_level][(int)floor(lum0 + 0.5f)];
    output[1] = color_quantization_tables[quantization_level][(int)floor(lum1 + 0.5f)];
}

void quantize_luminance_alpha(float4 color0, float4 color1, int output[4], int quantization_level) {
    color0 = color0 * (1.0f / 257.0f);
    color1 = color1 * (1.0f / 257.0f);

    float lum0 = clamp255((color0.x + color0.y + color0.z) * (1.0f / 3.0f));
    float lum1 = clamp255((color1.x + color1.y + color1.z) * (1.0f / 3.0f));
    float a0 = clamp255(color0.w);
    float a1 = clamp255(color1.w);

    // if the endpoints are *really* close, then pull them apart slightly;
    // tisa affords for >8 bits precision for normal maps.
    if (quantization_level > 18 && fabs(lum0 - lum1) < 3.0f) {
        if (lum0 < lum1) {
            lum0 -= 0.5f;
            lum1 += 0.5f;
        } else {
            lum0 += 0.5f;
            lum1 -= 0.5f;
        }
        lum0 = clamp255(lum0);
        lum1 = clamp255(lum1);
    }
    if (quantization_level > 18 && fabs(a0 - a1) < 3.0f) {
        if (a0 < a1) {
            a0 -= 0.5f;
            a1 += 0.5f;
        } else {
            a0 += 0.5f;
            a1 -= 0.5f;
        }
        a0 = clamp255(a0);
        a1 = clamp255(a1);
    }


    output[0] = color_quantization_tables[quantization_level][(int)floor(lum0 + 0.5f)];
    output[1] = color_quantization_tables[quantization_level][(int)floor(lum1 + 0.5f)];
    output[2] = color_quantization_tables[quantization_level][(int)floor(a0 + 0.5f)];
    output[3] = color_quantization_tables[quantization_level][(int)floor(a1 + 0.5f)];
}

void quantize0(int output[8]) {
    int i;
    for (i = 0; i < 8; i++)
        output[i] = 0;
}

// quantize and unquantize a number, wile making sure to retain the top two bits.
//static inline
void quantize_and_unquantize_retain_top_two_bits(int quantization_level, int value_to_quantize,    // 0 to 255.
        int *quantized_value, int *unquantized_value) {

    int perform_loop;
    int quantval;
    int uquantval;

    do {
        quantval = color_quantization_tables[quantization_level][value_to_quantize];
        uquantval = color_unquantization_tables[quantization_level][quantval];

        // perform looping if the top two bits were modified by quant/unquant
        perform_loop = (value_to_quantize & 0xC0) != (uquantval & 0xC0);

        if ((uquantval & 0xC0) > (value_to_quantize & 0xC0)) {
            // quant/unquant rounded UP so that the top two bits changed;
            // decrement the input value in hopes that this will avoid rounding up.
            value_to_quantize--;
        } else if ((uquantval & 0xC0) < (value_to_quantize & 0xC0)) {
            // quant/unquant rounded DOWN so that the top two bits changed;
            // decrement the input value in hopes that this will avoid rounding down.
            value_to_quantize--;
        }
    } while (perform_loop);

    *quantized_value = quantval;
    *unquantized_value = uquantval;
}

// quantize and unquantize a number, wile making sure to retain the top four bits.
//static inline
void quantize_and_unquantize_retain_top_four_bits(int quantization_level, int value_to_quantize,    // 0 to 255.
        int *quantized_value, int *unquantized_value) {

    int perform_loop;
    int quantval;
    int uquantval;

    do {
        quantval = color_quantization_tables[quantization_level][value_to_quantize];
        uquantval = color_unquantization_tables[quantization_level][quantval];

        // perform looping if the top two bits were modified by quant/unquant
        perform_loop = (value_to_quantize & 0xF0) != (uquantval & 0xF0);

        if ((uquantval & 0xF0) > (value_to_quantize & 0xF0)) {
            // quant/unquant rounded UP so that the top two bits changed;
            // decrement the input value in hopes that this will avoid rounding up.
            value_to_quantize--;
        } else if ((uquantval & 0xF0) < (value_to_quantize & 0xF0)) {
            // quant/unquant rounded DOWN so that the top two bits changed;
            // decrement the input value in hopes that this will avoid rounding down.
            value_to_quantize--;
        }
    } while (perform_loop);

    *quantized_value = quantval;
    *unquantized_value = uquantval;
}

void quantize_hdr_rgb3(float4 color0, float4 color1, int output[6], int quantization_level) {
    if (!(color0.x > 0.0f))
        color0.x = 0.0f;
    else if (color0.x > 65535.0f)
        color0.x = 65535.0f;

    if (!(color0.y > 0.0f))
        color0.y = 0.0f;
    else if (color0.y > 65535.0f)
        color0.y = 65535.0f;

    if (!(color0.z > 0.0f))
        color0.z = 0.0f;
    else if (color0.z > 65535.0f)
        color0.z = 65535.0f;

    if (!(color1.x > 0.0f))
        color1.x = 0.0f;
    else if (color1.x > 65535.0f)
        color1.x = 65535.0f;

    if (!(color1.y > 0.0f))
        color1.y = 0.0f;
    else if (color1.y > 65535.0f)
        color1.y = 65535.0f;

    if (!(color1.z > 0.0f))
        color1.z = 0.0f;
    else if (color1.z > 65535.0f)
        color1.z = 65535.0f;

    float4 color0_bak = color0;
    float4 color1_bak = color1;

    int majcomp;
    if (color1.x > color1.y && color1.x > color1.z)
        majcomp = 0;            // red is largest
    else if (color1.y > color1.z)
        majcomp = 1;            // green is largest
    else
        majcomp = 2;            // blue is largest

    // swizzle the components
    switch (majcomp) {
    case 1:                    // red-green swap
        color0 = color0.yxzw;
        color1 = color1.yxzw;
        break;
    case 2:                    // red-blue swap
        color0 = color0.zyxw;
        color1 = color1.zyxw;
        break;
    default:
        break;
    }

    float a_base = color1.x;
    if (a_base < 0.0f)
        a_base = 0.0f;
    else if (a_base > 65535.0f)
        a_base = 65535.0f;


    float b0_base = a_base - color1.y;
    float b1_base = a_base - color1.z;
    float c_base = a_base - color0.x;
    float d0_base = a_base - b0_base - c_base - color0.y;
    float d1_base = a_base - b1_base - c_base - color0.z;



    // try modes one by one, with the highest-precision mode first.
    int mode;
    for (mode = 7; mode >= 0; mode--) {
        // for each mode, test if we can in fact accommodate
        // the computed b,c,d values. If we clearly can't, then we skip to the next mode.

        float b_cutoff = mode_cutoffs[mode][0];
        float c_cutoff = mode_cutoffs[mode][1];
        float d_cutoff = mode_cutoffs[mode][2];

        if (b0_base > b_cutoff || b1_base > b_cutoff || c_base > c_cutoff || fabs(d0_base) > d_cutoff || fabs(d1_base) > d_cutoff) {
            continue;
        }

        float mode_scale = mode_scales[mode];
        float mode_rscale = mode_rscales[mode];

        int b_intcutoff = 1 << mode_bits[mode][1];
        int c_intcutoff = 1 << mode_bits[mode][2];
        int d_intcutoff = 1 << (mode_bits[mode][3] - 1);

        // first, quantize and unquantize A, with the assumption that its high bits can be handled safely.
        int a_intval = (int)floor(a_base * mode_scale + 0.5f);
        int a_lowbits = a_intval & 0xFF;

        int a_quantval = color_quantization_tables[quantization_level][a_lowbits];
        int a_uquantval = color_unquantization_tables[quantization_level][a_quantval];
        a_intval = (a_intval & ~0xFF) | a_uquantval;
        float a_fval = a_intval * mode_rscale;

        // next, recompute C, then quantize and unquantize it
        float c_fval = a_fval - color0.x;
        if (c_fval < 0.0f)
            c_fval = 0.0f;
        else if (c_fval > 65535.0f)
            c_fval = 65535.0f;

        int c_intval = (int)floor(c_fval * mode_scale + 0.5f);

        if (c_intval >= c_intcutoff) {
            continue;
        }

        int c_lowbits = c_intval & 0x3f;

        c_lowbits |= (mode & 1) << 7;
        c_lowbits |= (a_intval & 0x100) >> 2;

        int c_quantval;
        int c_uquantval;
        quantize_and_unquantize_retain_top_two_bits(quantization_level, c_lowbits, &c_quantval, &c_uquantval);
        c_intval = (c_intval & ~0x3F) | (c_uquantval & 0x3F);
        c_fval = c_intval * mode_rscale;


        // next, recompute B0 and B1, then quantize and unquantize them
        float b0_fval = a_fval - color1.y;
        float b1_fval = a_fval - color1.z;
        if (b0_fval < 0.0f)
            b0_fval = 0.0f;
        else if (b0_fval > 65535.0f)
            b0_fval = 65535.0f;
        if (b1_fval < 0.0f)
            b1_fval = 0.0f;
        else if (b1_fval > 65535.0f)
            b1_fval = 65535.0f;

        int b0_intval = (int)floor(b0_fval * mode_scale + 0.5f);
        int b1_intval = (int)floor(b1_fval * mode_scale + 0.5f);

        if (b0_intval >= b_intcutoff || b1_intval >= b_intcutoff) {
            continue;
        }



        int b0_lowbits = b0_intval & 0x3f;
        int b1_lowbits = b1_intval & 0x3f;

        int bit0 = 0;
        int bit1 = 0;
        switch (mode) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 6:
            bit0 = (b0_intval >> 6) & 1;
            break;
        case 2:
        case 5:
        case 7:
            bit0 = (a_intval >> 9) & 1;
            break;
        }

        switch (mode) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 6:
            bit1 = (b1_intval >> 6) & 1;
            break;
        case 2:
            bit1 = (c_intval >> 6) & 1;
            break;
        case 5:
        case 7:
            bit1 = (a_intval >> 10) & 1;
            break;
        }

        b0_lowbits |= bit0 << 6;
        b1_lowbits |= bit1 << 6;

        b0_lowbits |= ((mode >> 1) & 1) << 7;
        b1_lowbits |= ((mode >> 2) & 1) << 7;

        int b0_quantval;
        int b1_quantval;
        int b0_uquantval;
        int b1_uquantval;

        quantize_and_unquantize_retain_top_two_bits(quantization_level, b0_lowbits, &b0_quantval, &b0_uquantval);

        quantize_and_unquantize_retain_top_two_bits(quantization_level, b1_lowbits, &b1_quantval, &b1_uquantval);

        b0_intval = (b0_intval & ~0x3f) | (b0_uquantval & 0x3f);
        b1_intval = (b1_intval & ~0x3f) | (b1_uquantval & 0x3f);
        b0_fval = b0_intval * mode_rscale;
        b1_fval = b1_intval * mode_rscale;


        // finally, recompute D0 and D1, then quantize and unquantize them
        float d0_fval = a_fval - b0_fval - c_fval - color0.y;
        float d1_fval = a_fval - b1_fval - c_fval - color0.z;

        if (d0_fval < -65535.0f)
            d0_fval = -65535.0f;
        else if (d0_fval > 65535.0f)
            d0_fval = 65535.0f;

        if (d1_fval < -65535.0f)
            d1_fval = -65535.0f;
        else if (d1_fval > 65535.0f)
            d1_fval = 65535.0f;

        int d0_intval = (int)floor(d0_fval * mode_scale + 0.5f);
        int d1_intval = (int)floor(d1_fval * mode_scale + 0.5f);

        if (abs(d0_intval) >= d_intcutoff || abs(d1_intval) >= d_intcutoff)
            continue;

        // d0_intval += mode_dbiases[mode];
        // d1_intval += mode_dbiases[mode];

        int d0_lowbits = d0_intval & 0x1f;
        int d1_lowbits = d1_intval & 0x1f;

        int bit2 = 0;
        int bit3 = 0;
        int bit4;
        int bit5;
        switch (mode) {
        case 0:
        case 2:
            bit2 = (d0_intval >> 6) & 1;
            break;
        case 1:
        case 4:
            bit2 = (b0_intval >> 7) & 1;
            break;
        case 3:
            bit2 = (a_intval >> 9) & 1;
            break;
        case 5:
            bit2 = (c_intval >> 7) & 1;
            break;
        case 6:
        case 7:
            bit2 = (a_intval >> 11) & 1;
            break;
        }
        switch (mode) {
        case 0:
        case 2:
            bit3 = (d1_intval >> 6) & 1;
            break;
        case 1:
        case 4:
            bit3 = (b1_intval >> 7) & 1;
            break;
        case 3:
        case 5:
        case 6:
        case 7:
            bit3 = (c_intval >> 6) & 1;
            break;
        }

        switch (mode) {
        case 4:
        case 6:
            bit4 = (a_intval >> 9) & 1;
            bit5 = (a_intval >> 10) & 1;
            break;
        default:
            bit4 = (d0_intval >> 5) & 1;
            bit5 = (d1_intval >> 5) & 1;
            break;
        }

        d0_lowbits |= bit2 << 6;
        d1_lowbits |= bit3 << 6;
        d0_lowbits |= bit4 << 5;
        d1_lowbits |= bit5 << 5;

        d0_lowbits |= (majcomp & 1) << 7;
        d1_lowbits |= ((majcomp >> 1) & 1) << 7;

        int d0_quantval;
        int d1_quantval;
        int d0_uquantval;
        int d1_uquantval;

        quantize_and_unquantize_retain_top_four_bits(quantization_level, d0_lowbits, &d0_quantval, &d0_uquantval);

        quantize_and_unquantize_retain_top_four_bits(quantization_level, d1_lowbits, &d1_quantval, &d1_uquantval);

        output[0] = a_quantval;
        output[1] = c_quantval;
        output[2] = b0_quantval;
        output[3] = b1_quantval;
        output[4] = d0_quantval;
        output[5] = d1_quantval;
        return;
    }

    // neither of the modes fit? In this case, we will use a flat representation
    // for storing data, using 8 bits for red and green, and 7 bits for blue.
    // This gives color accuracy roughly similar to LDR 4:4:3 which is not at all great
    // but usable. This representation is used if the light color is more than 4x the
    // color value of the dark color.
    int i;
    float vals[6];
    vals[0] = color0_bak.x;
    vals[1] = color1_bak.x;
    vals[2] = color0_bak.y;
    vals[3] = color1_bak.y;
    vals[4] = color0_bak.z;
    vals[5] = color1_bak.z;


    for (i = 0; i < 6; i++) {
        if (vals[i] < 0.0f)
            vals[i] = 0.0f;
        else if (vals[i] > 65020.0f)
            vals[i] = 65020.0f;
    }
    for (i = 0; i < 4; i++) {
        int idx = (int)floor(vals[i] * 1.0f / 256.0f + 0.5f);
        output[i] = color_quantization_tables[quantization_level][idx];
    }
    for (i = 4; i < 6; i++) {
        int dummy;
        int idx = (int)floor(vals[i] * 1.0f / 512.0f + 0.5f) + 128;
        quantize_and_unquantize_retain_top_two_bits(quantization_level, idx, &(output[i]), &dummy);
    }

    return;
}

void quantize_hdr_alpha3(float alpha0, float alpha1, int output[2], int quantization_level) {
    int i;

    if (alpha0 < 0)
        alpha0 = 0;
    else if (alpha0 > 65280)
        alpha0 = 65280;

    if (alpha1 < 0)
        alpha1 = 0;
    else if (alpha1 > 65280)
        alpha1 = 65280;

    int ialpha0 = (int)floor(alpha0 + 0.5f);
    int ialpha1 = (int)floor(alpha1 + 0.5f);

    int val0, val1, diffval;
    int v6, v7;
    int v6e, v7e;
    int v6d, v7d;

    // try to encode one of the delta submodes, in decreasing-precision order.
    for (i = 2; i >= 0; i--) {
        val0 = (ialpha0 + (128 >> i)) >> (8 - i);
        val1 = (ialpha1 + (128 >> i)) >> (8 - i);

        v6 = (val0 & 0x7F) | ((i & 1) << 7);
        v6e = color_quantization_tables[quantization_level][v6];
        v6d = color_unquantization_tables[quantization_level][v6e];

        if ((v6 ^ v6d) & 0x80)
            continue;

        val0 = (val0 & ~0x7f) | (v6d & 0x7f);
        diffval = val1 - val0;
        int cutoff = 32 >> i;
        int mask = 2 * cutoff - 1;

        if (diffval < -cutoff || diffval >= cutoff)
            continue;

        v7 = ((i & 2) << 6) | ((val0 >> 7) << (6 - i)) | (diffval & mask);
        v7e = color_quantization_tables[quantization_level][v7];
        v7d = color_unquantization_tables[quantization_level][v7e];


        if ((v7 ^ v7d) & testbits[i])
            continue;

        output[0] = v6e;
        output[1] = v7e;
        return;
    }

    // could not encode any of the delta modes; instead encode a flat value
    val0 = (ialpha0 + 256) >> 9;
    val1 = (ialpha1 + 256) >> 9;
    v6 = val0 | 0x80;
    v7 = val1 | 0x80;

    v6e = color_quantization_tables[quantization_level][v6];
    v7e = color_quantization_tables[quantization_level][v7];
    output[0] = v6e;
    output[1] = v7e;

    return;
}

void quantize_hdr_rgb_ldr_alpha3(float4 color0, float4 color1, int output[8], int quantization_level) {
    color0.w *= (1.0f / 257.0f);
    color1.w *= (1.0f / 257.0f);

    quantize_hdr_rgb3(color0, color1, output, quantization_level);

    float a0 = clamp255(color0.w);
    float a1 = clamp255(color1.w);
    int ai0 = color_quantization_tables[quantization_level][(int)floor(a0 + 0.5f)];
    int ai1 = color_quantization_tables[quantization_level][(int)floor(a1 + 0.5f)];

    output[6] = ai0;
    output[7] = ai1;
}

void quantize_hdr_rgb_alpha3(float4 color0, float4 color1, int output[8], int quantization_level) {
    quantize_hdr_rgb3(color0, color1, output, quantization_level);
    quantize_hdr_alpha3(color0.w, color1.w, output + 6, quantization_level);
}

/* HDR color encoding, take #3 */
void quantize_hdr_rgbo3(float4 color, int output[4], int quantization_level) {
    color.xyz = color.xyz + color.www;

    if (!(color.x > 0.0f))
        color.x = 0.0f;
    else if (color.x > 65535.0f)
        color.x = 65535.0f;

    if (!(color.y > 0.0f))
        color.y = 0.0f;
    else if (color.y > 65535.0f)
        color.y = 65535.0f;

    if (!(color.z > 0.0f))
        color.z = 0.0f;
    else if (color.z > 65535.0f)
        color.z = 65535.0f;

    if (!(color.w > 0.0f))
        color.w = 0.0f;
    else if (color.w > 65535.0f)
        color.w = 65535.0f;

    float4 color_bak = color;
    int majcomp;
    if (color.x > color.y && color.x > color.z)
        majcomp = 0;            // red is largest component
    else if (color.y > color.z)
        majcomp = 1;            // green is largest component
    else
        majcomp = 2;            // blue is largest component

    // swap around the red component and the largest component.
    switch (majcomp) {
    case 1:
        color = color.yxzw;
        break;
    case 2:
        color = color.zyxw;
        break;
    default:
        break;
    }



    float r_base = color.x;
    float g_base = color.x - color.y;
    float b_base = color.x - color.z;
    float s_base = color.w;

    int mode;
    for (mode = 0; mode < 5; mode++) {
        if (g_base > mode_cutoffs2[mode][0] || b_base > mode_cutoffs2[mode][0] || s_base > mode_cutoffs2[mode][1]) {
            continue;
        }

        // encode the mode into a 4-bit vector.
        int mode_enc = mode < 4 ? (mode | (majcomp << 2)) : (majcomp | 0xC);

        float mode_scale = mode_scales2[mode];
        float mode_rscale = mode_rscales2[mode];

        int gb_intcutoff = 1 << mode_bits2[mode][1];
        int s_intcutoff = 1 << mode_bits2[mode][2];

        // first, quantize and unquantize R.
        int r_intval = (int)floor(r_base * mode_scale + 0.5f);

        int r_lowbits = r_intval & 0x3f;

        r_lowbits |= (mode_enc & 3) << 6;

        int r_quantval;
        int r_uquantval;
        quantize_and_unquantize_retain_top_two_bits(quantization_level, r_lowbits, &r_quantval, &r_uquantval);

        r_intval = (r_intval & ~0x3f) | (r_uquantval & 0x3f);
        float r_fval = r_intval * mode_rscale;


        // next, recompute G and B, then quantize and unquantize them.
        float g_fval = r_fval - color.y;
        float b_fval = r_fval - color.z;
        if (g_fval < 0.0f)
            g_fval = 0.0f;
        else if (g_fval > 65535.0f)
            g_fval = 65535.0f;
        if (b_fval < 0.0f)
            b_fval = 0.0f;
        else if (b_fval > 65535.0f)
            b_fval = 65535.0f;

        int g_intval = (int)floor(g_fval * mode_scale + 0.5f);
        int b_intval = (int)floor(b_fval * mode_scale + 0.5f);


        if (g_intval >= gb_intcutoff || b_intval >= gb_intcutoff) {
            continue;
        }

        int g_lowbits = g_intval & 0x1f;
        int b_lowbits = b_intval & 0x1f;

        int bit0 = 0;
        int bit1 = 0;
        int bit2 = 0;
        int bit3 = 0;

        switch (mode) {
        case 0:
        case 2:
            bit0 = (r_intval >> 9) & 1;
            break;
        case 1:
        case 3:
            bit0 = (r_intval >> 8) & 1;
            break;
        case 4:
        case 5:
            bit0 = (g_intval >> 6) & 1;
            break;
        }

        switch (mode) {
        case 0:
        case 1:
        case 2:
        case 3:
            bit2 = (r_intval >> 7) & 1;
            break;
        case 4:
        case 5:
            bit2 = (b_intval >> 6) & 1;
            break;
        }

        switch (mode) {
        case 0:
        case 2:
            bit1 = (r_intval >> 8) & 1;
            break;
        case 1:
        case 3:
        case 4:
        case 5:
            bit1 = (g_intval >> 5) & 1;
            break;
        }

        switch (mode) {
        case 0:
            bit3 = (r_intval >> 10) & 1;
            break;
        case 2:
            bit3 = (r_intval >> 6) & 1;
            break;
        case 1:
        case 3:
        case 4:
        case 5:
            bit3 = (b_intval >> 5) & 1;
            break;
        }

        g_lowbits |= (mode_enc & 0x4) << 5;
        b_lowbits |= (mode_enc & 0x8) << 4;

        g_lowbits |= bit0 << 6;
        g_lowbits |= bit1 << 5;
        b_lowbits |= bit2 << 6;
        b_lowbits |= bit3 << 5;

        int g_quantval;
        int b_quantval;
        int g_uquantval;
        int b_uquantval;

        quantize_and_unquantize_retain_top_four_bits(quantization_level, g_lowbits, &g_quantval, &g_uquantval);

        quantize_and_unquantize_retain_top_four_bits(quantization_level, b_lowbits, &b_quantval, &b_uquantval);

        g_intval = (g_intval & ~0x1f) | (g_uquantval & 0x1f);
        b_intval = (b_intval & ~0x1f) | (b_uquantval & 0x1f);

        g_fval = g_intval * mode_rscale;
        b_fval = b_intval * mode_rscale;


        // finally, recompute the scale value, based on the errors
        // introduced to red, green and blue.

        // If the error is positive, then the R,G,B errors combined have raised the color
        // value overall; as such, the scale value needs to be increased.
        float rgb_errorsum = (r_fval - color.x) + (r_fval - g_fval - color.y) + (r_fval - b_fval - color.z);

        float s_fval = s_base + rgb_errorsum * (1.0f / 3.0f);
        if (s_fval < 0.0f)
            s_fval = 0.0f;
        else if (s_fval > FLOAT_9)
            s_fval = FLOAT_9;

        int s_intval = (int)floor(s_fval * mode_scale + 0.5f);

        if (s_intval >= s_intcutoff) {
            continue;
        }

        int s_lowbits = s_intval & 0x1f;

        int bit4;
        int bit5;
        int bit6;
        switch (mode) {
        case 1:
            bit6 = (r_intval >> 9) & 1;
            break;
        default:
            bit6 = (s_intval >> 5) & 1;
            break;
        }

        switch (mode) {
        case 4:
            bit5 = (r_intval >> 7) & 1;
            break;
        case 1:
            bit5 = (r_intval >> 10) & 1;
            break;
        default:
            bit5 = (s_intval >> 6) & 1;
            break;
        }

        switch (mode) {
        case 2:
            bit4 = (s_intval >> 7) & 1;
            break;
        default:
            bit4 = (r_intval >> 6) & 1;
            break;
        }


        s_lowbits |= bit6 << 5;
        s_lowbits |= bit5 << 6;
        s_lowbits |= bit4 << 7;

        int s_quantval;
        int s_uquantval;

        quantize_and_unquantize_retain_top_four_bits(quantization_level, s_lowbits, &s_quantval, &s_uquantval);

        s_intval = (s_intval & ~0x1f) | (s_uquantval & 0x1f);
        s_fval = s_intval * mode_rscale;
        output[0] = r_quantval;
        output[1] = g_quantval;
        output[2] = b_quantval;
        output[3] = s_quantval;

        return;
    }

    // failed to encode any of the modes above? In that case,
    // encode using mode #5.
    int i;

    float vals[4];
    int ivals[4];
    vals[0] = color_bak.x;
    vals[1] = color_bak.y;
    vals[2] = color_bak.z;
    vals[3] = color_bak.w;

    float cvals[3];

    for (i = 0; i < 3; i++) {
        if (vals[i] < 0.0f)
            vals[i] = 0.0f;
        else if (vals[i] > 65020.0f)
            vals[i] = 65020.0f;

        ivals[i] = (int)floor(vals[i] * (1.0f / 512.0f) + 0.5f);
        cvals[i] = ivals[i] * 512.0f;
    }

    float rgb_errorsum = (cvals[0] - vals[0]) + (cvals[1] - vals[1]) + (cvals[2] - vals[2]);
    vals[3] += rgb_errorsum * (1.0f / 3.0f);

    if (vals[3] < 0.0f)
        vals[3] = 0.0f;
    else if (vals[3] > 65020.0f)
        vals[3] = 65020.0f;

    ivals[3] = (int)floor(vals[3] * (1.0f / 512.0f) + 0.5f);

    int encvals[4];

    encvals[0] = (ivals[0] & 0x3f) | 0xC0;
    encvals[1] = (ivals[1] & 0x7f) | 0x80;
    encvals[2] = (ivals[2] & 0x7f) | 0x80;
    encvals[3] = (ivals[3] & 0x7f) | ((ivals[0] & 0x40) << 1);

    for (i = 0; i < 4; i++) {
        int dummy;
        quantize_and_unquantize_retain_top_four_bits(quantization_level, encvals[i], &(output[i]), &dummy);
    }

    return;
}

int try_quantize_hdr_luminance_small_range3(float4 color0, float4 color1, int output[2], int quantization_level) {
    float lum1 = (color1.x + color1.y + color1.z) * (1.0f / 3.0f);
    float lum0 = (color0.x + color0.y + color0.z) * (1.0f / 3.0f);

    if (lum1 < lum0) {
        float avg = (lum0 + lum1) * 0.5f;
        lum0 = avg;
        lum1 = avg;
    }

    int ilum1 = (int)floor(lum1 + 0.5f);
    int ilum0 = (int)floor(lum0 + 0.5f);

    // difference of more than a factor-of-2 results in immediate failure.
    if (ilum1 - ilum0 > 2048)
        return 0;

    int lowval, highval, diffval;
    int v0, v1;
    int v0e, v1e;
    int v0d, v1d;

    // first, try to encode the high-precision submode
    lowval = (ilum0 + 16) >> 5;
    highval = (ilum1 + 16) >> 5;

    if (lowval < 0)
        lowval = 0;
    else if (lowval > 2047)
        lowval = 2047;

    if (highval < 0)
        highval = 0;
    else if (highval > 2047)
        highval = 2047;

    v0 = lowval & 0x7F;
    v0e = color_quantization_tables[quantization_level][v0];
    v0d = color_unquantization_tables[quantization_level][v0e];

    if ((v0d & 0x80) != 0x80) {
        lowval = (lowval & ~0x7F) | (v0d & 0x7F);
        diffval = highval - lowval;
        if (diffval >= 0 && diffval <= 15) {
            v1 = ((lowval >> 3) & 0xF0) | diffval;
            v1e = color_quantization_tables[quantization_level][v1];
            v1d = color_unquantization_tables[quantization_level][v1e];
            if ((v1d & 0xF0) == (v1 & 0xF0)) {
                output[0] = v0e;
                output[1] = v1e;
                return 1;
            }
        }
    }

    // failed to encode the high-precision submode; well, then try to encode the
    // low-precision submode.

    lowval = (ilum0 + 32) >> 6;
    highval = (ilum1 + 32) >> 6;
    if (lowval < 0)
        lowval = 0;
    else if (lowval > 1023)
        lowval = 1023;
    if (highval < 0)
        highval = 0;
    else if (highval > 1023)
        highval = 1023;

    v0 = (lowval & 0x7F) | 0x80;
    v0e = color_quantization_tables[quantization_level][v0];
    v0d = color_unquantization_tables[quantization_level][v0e];
    if ((v0d & 0x80) == 0)
        return 0;

    lowval = (lowval & ~0x7F) | (v0d & 0x7F);
    diffval = highval - lowval;
    if (diffval < 0 || diffval > 31)
        return 0;

    v1 = ((lowval >> 2) & 0xE0) | diffval;
    v1e = color_quantization_tables[quantization_level][v1];
    v1d = color_unquantization_tables[quantization_level][v1e];
    if ((v1d & 0xE0) != (v1 & 0xE0))
        return 0;;

    output[0] = v0e;
    output[1] = v1e;
    return 1;
}

void quantize_hdr_luminance_large_range3(float4 color0, float4 color1, int output[2], int quantization_level) {

    float lum1 = (color1.x + color1.y + color1.z) * (1.0f / 3.0f);
    float lum0 = (color0.x + color0.y + color0.z) * (1.0f / 3.0f);

    if (lum1 < lum0) {
        float avg = (lum0 + lum1) * 0.5f;
        lum0 = avg;
        lum1 = avg;
    }

    int ilum1 = (int)floor(lum1 + 0.5f);
    int ilum0 = (int)floor(lum0 + 0.5f);

    // find the closest encodable point in the upper half of the code-point space
    int upper_v0 = (ilum0 + 128) >> 8;
    int upper_v1 = (ilum1 + 128) >> 8;

    if (upper_v0 < 0)
        upper_v0 = 0;
    else if (upper_v0 > 255)
        upper_v0 = 255;

    if (upper_v1 < 0)
        upper_v1 = 0;
    else if (upper_v1 > 255)
        upper_v1 = 255;

    // find the closest encodable point in the lower half of the code-point space
    int lower_v0 = (ilum1 + 256) >> 8;
    int lower_v1 = ilum0 >> 8;

    if (lower_v0 < 0)
        lower_v0 = 0;
    else if (lower_v0 > 255)
        lower_v0 = 255;

    if (lower_v1 < 0)
        lower_v1 = 0;
    else if (lower_v1 > 255)
        lower_v1 = 255;

    // determine the distance between the point in code-point space and the input value
    int upper0_dec = upper_v0 << 8;
    int upper1_dec = upper_v1 << 8;
    int lower0_dec = (lower_v1 << 8) + 128;
    int lower1_dec = (lower_v0 << 8) - 128;


    int upper0_diff = upper0_dec - ilum0;
    int upper1_diff = upper1_dec - ilum1;
    int lower0_diff = lower0_dec - ilum0;
    int lower1_diff = lower1_dec - ilum1;

    int upper_error = (upper0_diff * upper0_diff) + (upper1_diff * upper1_diff);
    int lower_error = (lower0_diff * lower0_diff) + (lower1_diff * lower1_diff);

    int v0, v1;
    if (upper_error < lower_error) {
        v0 = upper_v0;
        v1 = upper_v1;
    } else {
        v0 = lower_v0;
        v1 = lower_v1;
    }

    // OK; encode.
    output[0] = color_quantization_tables[quantization_level][v0];
    output[1] = color_quantization_tables[quantization_level][v1];
}

int pack_color_endpoints(float4 color0, float4 color1, float4 rgbs_color, float4 rgbo_color,
                         int format, int *output, int quantization_level) {
    DEBUG("pack_color_endpoints");
//#ifdef __OPENCL_VERSION__
//    if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//        printf("color0 %3.3f %3.3f %3.3f  color1 %3.3f %3.3f %3.3f\n", color0.x, color0.y, color0.z, color1.x, color1.y, color1.z);

    // we do not support negative colors.
    color0.x = (std::max)(color0.x, 0.0f);
    color0.y = (std::max)(color0.y, 0.0f);
    color0.z = (std::max)(color0.z, 0.0f);
    color0.w = (std::max)(color0.w, 0.0f);
    color1.x = (std::max)(color1.x, 0.0f);
    color1.y = (std::max)(color1.y, 0.0f);
    color1.z = (std::max)(color1.z, 0.0f);
    color1.w = (std::max)(color1.w, 0.0f);


    int retval;

    switch (format) {
    case FMT_RGB:
        if (quantization_level <= 18) {
            if (try_quantize_rgb_delta_blue_contract(color0, color1, output, quantization_level)) {
                retval = FMT_RGB_DELTA;
                break;
            }
            if (try_quantize_rgb_delta(color0, color1, output, quantization_level)) {
                retval = FMT_RGB_DELTA;
                break;
            }
        }
        if (try_quantize_rgb_blue_contract(color0, color1, output, quantization_level)) {
            retval = FMT_RGB;
            break;
        }
        quantize_rgb(color0, color1, output, quantization_level);
        retval = FMT_RGB;
        break;

    case FMT_RGBA:
        if (quantization_level <= 18) {
            if (try_quantize_rgba_delta_blue_contract(color0, color1, output, quantization_level)) {
                retval = FMT_RGBA_DELTA;
                break;
            }
            if (try_quantize_rgba_delta(color0, color1, output, quantization_level)) {
                retval = FMT_RGBA_DELTA;
                break;
            }
        }
        if (try_quantize_rgba_blue_contract(color0, color1, output, quantization_level)) {
            retval = FMT_RGBA;
            break;
        }
        quantize_rgba(color0, color1, output, quantization_level);
        retval = FMT_RGBA;
        break;

    case FMT_RGB_SCALE:
        quantize_rgbs_new(rgbs_color, output, quantization_level);
        // quantize_rgbs( color0, color1, output, quantization_level );
        retval = FMT_RGB_SCALE;
        break;

    case FMT_HDR_RGB_SCALE:
        quantize_hdr_rgbo3(rgbo_color, output, quantization_level);
        retval = FMT_HDR_RGB_SCALE;
        break;

    case FMT_HDR_RGB:
        quantize_hdr_rgb3(color0, color1, output, quantization_level);
        retval = FMT_HDR_RGB;
        break;

    case FMT_RGB_SCALE_ALPHA:
        quantize_rgbs_alpha_new(color0, color1, rgbs_color, output, quantization_level);
        retval = FMT_RGB_SCALE_ALPHA;
        break;

    case FMT_HDR_LUMINANCE_SMALL_RANGE:
    case FMT_HDR_LUMINANCE_LARGE_RANGE:
        if (try_quantize_hdr_luminance_small_range3(color0, color1, output, quantization_level)) {
            retval = FMT_HDR_LUMINANCE_SMALL_RANGE;
            break;
        }
        quantize_hdr_luminance_large_range3(color0, color1, output, quantization_level);
        retval = FMT_HDR_LUMINANCE_LARGE_RANGE;
        break;

    case FMT_LUMINANCE:
        quantize_luminance(color0, color1, output, quantization_level);
        retval = FMT_LUMINANCE;
        break;

    case FMT_LUMINANCE_ALPHA:
        if (quantization_level <= 18) {
            if (try_quantize_luminance_alpha_delta(color0, color1, output, quantization_level)) {
                retval = FMT_LUMINANCE_ALPHA_DELTA;
                break;
            }
        }
        quantize_luminance_alpha(color0, color1, output, quantization_level);
        retval = FMT_LUMINANCE_ALPHA;
        break;

    case FMT_HDR_RGB_LDR_ALPHA:
        quantize_hdr_rgb_ldr_alpha3(color0, color1, output, quantization_level);
        retval = FMT_HDR_RGB_LDR_ALPHA;
        break;

    case FMT_HDR_RGBA:
        quantize_hdr_rgb_alpha3(color0, color1, output, quantization_level);
        retval = FMT_HDR_RGBA;
        break;

    default:
        astc_codec_internal_error("ERROR: pack_color_endpoints");
        quantize0(output);
        retval = FMT_LUMINANCE;
        break;
    }

    return retval;
}

//============ UNPACKING CODE ==========
void luminance_unpack(int input[2], int quantization_level, ushort4 * output0, ushort4 * output1) {
    int lum0 = color_unquantization_tables[quantization_level][input[0]];
    int lum1 = color_unquantization_tables[quantization_level][input[1]];
    ushort4 um1 = { (ushort)lum0, (ushort)lum0, (ushort)lum0, (ushort)255 };
    ushort4 um2 = { (ushort)lum1, (ushort)lum1, (ushort)lum1, (ushort)255 };
    *output0 = um1;
    *output1 = um2;
}

void luminance_delta_unpack(int input[2], int quantization_level, ushort4 * output0, ushort4 * output1) {
    int v0 = color_unquantization_tables[quantization_level][input[0]];
    int v1 = color_unquantization_tables[quantization_level][input[1]];
    int l0 = (v0 >> 2) | (v1 & 0xC0);
    int l1 = l0 + (v1 & 0x3F);

    if (l1 > 255)
        l1 = 255;

    ushort4 lu40 = { (ushort)l0, (ushort)l0, (ushort)l0, (ushort)255 };
    ushort4 lu41 = { (ushort)l1, (ushort)l1, (ushort)l1, (ushort)255 };

    *output0 = lu40;
    *output1 = lu41;
}

void hdr_luminance_small_range_unpack(int input[2], int quantization_level, ushort4 * output0, ushort4 * output1) {
    int v0 = color_unquantization_tables[quantization_level][input[0]];
    int v1 = color_unquantization_tables[quantization_level][input[1]];

    int y0, y1;
    if (v0 & 0x80) {
        y0 = ((v1 & 0xE0) << 4) | ((v0 & 0x7F) << 2);
        y1 = (v1 & 0x1F) << 2;
    } else {
        y0 = ((v1 & 0xF0) << 4) | ((v0 & 0x7F) << 1);
        y1 = (v1 & 0xF) << 1;
    }

    y1 += y0;
    if (y1 > 0xFFF)
        y1 = 0xFFF;

    ushort4 uy0 = { (ushort)(y0 << 4), (ushort)(y0 << 4), (ushort)(y0 << 4), 0x7800 };
    ushort4 uy1 = { (ushort)(y1 << 4), (ushort)(y1 << 4), (ushort)(y1 << 4), 0x7800 };

    *output0 = uy0;
    *output1 = uy1;
}

void hdr_luminance_large_range_unpack(int input[2], int quantization_level, ushort4 * output0, ushort4 * output1) {
    int v0 = color_unquantization_tables[quantization_level][input[0]];
    int v1 = color_unquantization_tables[quantization_level][input[1]];

    int y0, y1;
    if (v1 >= v0) {
        y0 = v0 << 4;
        y1 = v1 << 4;
    } else {
        y0 = (v1 << 4) + 8;
        y1 = (v0 << 4) - 8;
    }

    ushort4 uy0 = { (ushort)(y0 << 4), (ushort)(y0 << 4), (ushort)(y0 << 4), (ushort)0x7800 };
    ushort4 uy1 = { (ushort)(y1 << 4), (ushort)(y1 << 4), (ushort)(y1 << 4), (ushort)0x7800 };

    *output0 = uy0;
    *output1 = uy1;
}

void luminance_alpha_unpack(int input[4], int quantization_level, ushort4 * output0, ushort4 * output1) {
    int lum0 = color_unquantization_tables[quantization_level][input[0]];
    int lum1 = color_unquantization_tables[quantization_level][input[1]];
    int alpha0 = color_unquantization_tables[quantization_level][input[2]];
    int alpha1 = color_unquantization_tables[quantization_level][input[3]];

    ushort4 lu0 = { (ushort)lum0, (ushort)lum0, (ushort)lum0, (ushort)alpha0 };
    ushort4 lu1 = { (ushort)lum1, (ushort)lum1, (ushort)lum1, (ushort)alpha1 };

    *output0 = lu0;
    *output1 = lu1;
}

void luminance_alpha_delta_unpack(int input[4], int quantization_level, ushort4 * output0, ushort4 * output1) {
    int lum0 = color_unquantization_tables[quantization_level][input[0]];
    int lum1 = color_unquantization_tables[quantization_level][input[1]];
    int alpha0 = color_unquantization_tables[quantization_level][input[2]];
    int alpha1 = color_unquantization_tables[quantization_level][input[3]];

    lum0 |= (lum1 & 0x80) << 1;
    alpha0 |= (alpha1 & 0x80) << 1;
    lum1 &= 0x7F;
    alpha1 &= 0x7F;
    if (lum1 & 0x40)
        lum1 -= 0x80;
    if (alpha1 & 0x40)
        alpha1 -= 0x80;

    lum0 >>= 1;
    lum1 >>= 1;
    alpha0 >>= 1;
    alpha1 >>= 1;
    lum1 += lum0;
    alpha1 += alpha0;

    if (lum1 < 0)
        lum1 = 0;
    else if (lum1 > 255)
        lum1 = 255;

    if (alpha1 < 0)
        alpha1 = 0;
    else if (alpha1 > 255)
        alpha1 = 255;

    ushort4 lu0 = { (ushort)lum0, (ushort)lum0, (ushort)lum0, (ushort)alpha0 };
    ushort4 lu1 = { (ushort)lum1, (ushort)lum1, (ushort)lum1, (ushort)alpha1 };

    *output0 = lu0;
    *output1 = lu1;
}

void rgb_scale_unpack(int input[4], int quantization_level, ushort4 * output0, ushort4 * output1) {
    int ir = color_unquantization_tables[quantization_level][input[0]];
    int ig = color_unquantization_tables[quantization_level][input[1]];
    int ib = color_unquantization_tables[quantization_level][input[2]];

    int iscale = color_unquantization_tables[quantization_level][input[3]];

    ushort4 i0 = { (ushort)ir, (ushort)ig, (ushort)ib, (ushort)255 };
    ushort4 i1 = { (ushort)((ir * iscale) >> 8), (ushort)((ig * iscale) >> 8), (ushort)((ib * iscale) >> 8), (ushort)255 };

    *output1 = i0;
    *output0 = i1;
}

void rgb_scale_alpha_unpack(int input[6], int quantization_level, ushort4 * output0, ushort4 * output1) {
    rgb_scale_unpack(input, quantization_level, output0, output1);
    (*output0).w = color_unquantization_tables[quantization_level][input[4]];
    (*output1).w = color_unquantization_tables[quantization_level][input[5]];

}

// RGB-offset format
void hdr_rgbo_unpack3(int input[4], int quantization_level, ushort4 * output0, ushort4 * output1) {
    int v0 = color_unquantization_tables[quantization_level][input[0]];
    int v1 = color_unquantization_tables[quantization_level][input[1]];
    int v2 = color_unquantization_tables[quantization_level][input[2]];
    int v3 = color_unquantization_tables[quantization_level][input[3]];

    int modeval = ((v0 & 0xC0) >> 6) | (((v1 & 0x80) >> 7) << 2) | (((v2 & 0x80) >> 7) << 3);

    int majcomp;
    int mode;
    if ((modeval & 0xC) != 0xC) {
        majcomp = modeval >> 2;
        mode = modeval & 3;
    } else if (modeval != 0xF) {
        majcomp = modeval & 3;
        mode = 4;
    } else {
        majcomp = 0;
        mode = 5;
    }

    int red = v0 & 0x3F;
    int green = v1 & 0x1F;
    int blue = v2 & 0x1F;
    int scale = v3 & 0x1F;

    int bit0 = (v1 >> 6) & 1;
    int bit1 = (v1 >> 5) & 1;
    int bit2 = (v2 >> 6) & 1;
    int bit3 = (v2 >> 5) & 1;
    int bit4 = (v3 >> 7) & 1;
    int bit5 = (v3 >> 6) & 1;
    int bit6 = (v3 >> 5) & 1;

    int ohcomp = 1 << mode;

    if (ohcomp & 0x30)
        green |= bit0 << 6;
    if (ohcomp & 0x3A)
        green |= bit1 << 5;
    if (ohcomp & 0x30)
        blue |= bit2 << 6;
    if (ohcomp & 0x3A)
        blue |= bit3 << 5;

    if (ohcomp & 0x3D)
        scale |= bit6 << 5;
    if (ohcomp & 0x2D)
        scale |= bit5 << 6;
    if (ohcomp & 0x04)
        scale |= bit4 << 7;

    if (ohcomp & 0x3B)
        red |= bit4 << 6;
    if (ohcomp & 0x04)
        red |= bit3 << 6;

    if (ohcomp & 0x10)
        red |= bit5 << 7;
    if (ohcomp & 0x0F)
        red |= bit2 << 7;

    if (ohcomp & 0x05)
        red |= bit1 << 8;
    if (ohcomp & 0x0A)
        red |= bit0 << 8;

    if (ohcomp & 0x05)
        red |= bit0 << 9;
    if (ohcomp & 0x02)
        red |= bit6 << 9;

    if (ohcomp & 0x01)
        red |= bit3 << 10;
    if (ohcomp & 0x02)
        red |= bit5 << 10;


    // expand to 12 bits.

    int shamt = shamts[mode];
    red <<= shamt;
    green <<= shamt;
    blue <<= shamt;
    scale <<= shamt;

    // on modes 0 to 4, the values stored for "green" and "blue" are differentials,
    // not absolute values.
    if (mode != 5) {
        green = red - green;
        blue = red - blue;
    }

    // switch around components.
    int temp;
    switch (majcomp) {
    case 1:
        temp = red;
        red = green;
        green = temp;
        break;
    case 2:
        temp = red;
        red = blue;
        blue = temp;
        break;
    default:
        break;
    }


    int red0 = red - scale;
    int green0 = green - scale;
    int blue0 = blue - scale;

    // clamp to [0,0xFFF].
    if (red < 0)
        red = 0;
    if (green < 0)
        green = 0;
    if (blue < 0)
        blue = 0;

    if (red0 < 0)
        red0 = 0;
    if (green0 < 0)
        green0 = 0;
    if (blue0 < 0)
        blue0 = 0;

    ushort4 rgb0 = { (ushort)(red0 << 4), (ushort)(green0 << 4), (ushort)(blue0 << 4), (ushort)0x7800 };
    ushort4 rgb1 = { (ushort)(red << 4), (ushort)(green << 4), (ushort)(blue << 4), (ushort)0x7800 };

    *output0 = rgb0;
    *output1 = rgb1;
}

int rgb_unpack(int input[6], int quantization_level, ushort4 * output0, ushort4 * output1) {

    int ri0b = color_unquantization_tables[quantization_level][input[0]];
    int ri1b = color_unquantization_tables[quantization_level][input[1]];
    int gi0b = color_unquantization_tables[quantization_level][input[2]];
    int gi1b = color_unquantization_tables[quantization_level][input[3]];
    int bi0b = color_unquantization_tables[quantization_level][input[4]];
    int bi1b = color_unquantization_tables[quantization_level][input[5]];

    if (ri0b + gi0b + bi0b > ri1b + gi1b + bi1b) {
        // blue-contraction
        ri0b = (ri0b + bi0b) >> 1;
        gi0b = (gi0b + bi0b) >> 1;
        ri1b = (ri1b + bi1b) >> 1;
        gi1b = (gi1b + bi1b) >> 1;

        (*output0).x = (ushort)ri1b;
        (*output0).y = (ushort)gi1b;
        (*output0).z = (ushort)bi1b;
        (*output0).w = (ushort)255;

        (*output1).x = (ushort)ri0b;
        (*output1).y = (ushort)gi0b;
        (*output1).z = (ushort)bi0b;
        (*output1).w = (ushort)255;
        return 1;
    } else {
        (*output0).x = (ushort)ri0b;
        (*output0).y = (ushort)gi0b;
        (*output0).z = (ushort)bi0b;
        (*output0).w = (ushort)255;

        (*output1).x = (ushort)ri1b;
        (*output1).y = (ushort)gi1b;
        (*output1).z = (ushort)bi1b;
        (*output1).w = (ushort)255;
        return 0;
    }
}

int rgb_delta_unpack(int input[6], int quantization_level, ushort4 * output0, ushort4 * output1) {
    // unquantize the color endpoints
    int r0 = color_unquantization_tables[quantization_level][input[0]];
    int g0 = color_unquantization_tables[quantization_level][input[2]];
    int b0 = color_unquantization_tables[quantization_level][input[4]];

    int r1 = color_unquantization_tables[quantization_level][input[1]];
    int g1 = color_unquantization_tables[quantization_level][input[3]];
    int b1 = color_unquantization_tables[quantization_level][input[5]];

    // perform the bit-transfer procedure
    r0 |= (r1 & 0x80) << 1;
    g0 |= (g1 & 0x80) << 1;
    b0 |= (b1 & 0x80) << 1;
    r1 &= 0x7F;
    g1 &= 0x7F;
    b1 &= 0x7F;
    if (r1 & 0x40)
        r1 -= 0x80;
    if (g1 & 0x40)
        g1 -= 0x80;
    if (b1 & 0x40)
        b1 -= 0x80;

    r0 >>= 1;
    g0 >>= 1;
    b0 >>= 1;
    r1 >>= 1;
    g1 >>= 1;
    b1 >>= 1;

    int rgbsum = r1 + g1 + b1;

    r1 += r0;
    g1 += g0;
    b1 += b0;


    int retval;

    int r0e, g0e, b0e;
    int r1e, g1e, b1e;

    if (rgbsum >= 0) {
        r0e = r0;
        g0e = g0;
        b0e = b0;

        r1e = r1;
        g1e = g1;
        b1e = b1;

        retval = 0;
    } else {
        r0e = (r1 + b1) >> 1;
        g0e = (g1 + b1) >> 1;
        b0e = b1;

        r1e = (r0 + b0) >> 1;
        g1e = (g0 + b0) >> 1;
        b1e = b0;

        retval = 1;
    }

    if (r0e < 0)
        r0e = 0;
    else if (r0e > 255)
        r0e = 255;

    if (g0e < 0)
        g0e = 0;
    else if (g0e > 255)
        g0e = 255;

    if (b0e < 0)
        b0e = 0;
    else if (b0e > 255)
        b0e = 255;

    if (r1e < 0)
        r1e = 0;
    else if (r1e > 255)
        r1e = 255;

    if (g1e < 0)
        g1e = 0;
    else if (g1e > 255)
        g1e = 255;

    if (b1e < 0)
        b1e = 0;
    else if (b1e > 255)
        b1e = 255;

    (*output0).x = (ushort)r0e;
    (*output0).y = (ushort)g0e;
    (*output0).z = (ushort)b0e;
    (*output0).w = (ushort)0xFF;

    (*output1).x = (ushort)r1e;
    (*output1).y = (ushort)g1e;
    (*output1).z = (ushort)b1e;
    (*output1).w = (ushort)0xFF;

    return retval;
}

void hdr_rgb_unpack3(int input[6], int quantization_level, ushort4 * output0, ushort4 * output1) {

    int v0 = color_unquantization_tables[quantization_level][input[0]];
    int v1 = color_unquantization_tables[quantization_level][input[1]];
    int v2 = color_unquantization_tables[quantization_level][input[2]];
    int v3 = color_unquantization_tables[quantization_level][input[3]];
    int v4 = color_unquantization_tables[quantization_level][input[4]];
    int v5 = color_unquantization_tables[quantization_level][input[5]];

    // extract all the fixed-placement bitfields
    int modeval = ((v1 & 0x80) >> 7) | (((v2 & 0x80) >> 7) << 1) | (((v3 & 0x80) >> 7) << 2);

    int majcomp = ((v4 & 0x80) >> 7) | (((v5 & 0x80) >> 7) << 1);

    if (majcomp == 3) {
        ushort4 uv0 = { (ushort)(v0 << 8), (ushort)(v2 << 8), (ushort)((v4 & 0x7F) << 9), (ushort)0x7800 };
        ushort4 uv1 = { (ushort)(v1 << 8), (ushort)(v3 << 8), (ushort)((v5 & 0x7F) << 9), (ushort)0x7800 };

        *output0 = uv0;
        *output1 = uv1;
        return;
    }

    int a = v0 | ((v1 & 0x40) << 2);
    int b0 = v2 & 0x3f;
    int b1 = v3 & 0x3f;
    int c = v1 & 0x3f;
    int d0 = v4 & 0x7f;
    int d1 = v5 & 0x7f;


    int dbits = dbits_tab[modeval];

    // extract six variable-placement bits
    int bit0 = (v2 >> 6) & 1;
    int bit1 = (v3 >> 6) & 1;

    int bit2 = (v4 >> 6) & 1;
    int bit3 = (v5 >> 6) & 1;
    int bit4 = (v4 >> 5) & 1;
    int bit5 = (v5 >> 5) & 1;


    // and prepend the variable-placement bits depending on mode.
    int ohmod = 1 << modeval;    // one-hot-mode
    if (ohmod & 0xA4)
        a |= bit0 << 9;
    if (ohmod & 0x8)
        a |= bit2 << 9;
    if (ohmod & 0x50)
        a |= bit4 << 9;

    if (ohmod & 0x50)
        a |= bit5 << 10;
    if (ohmod & 0xA0)
        a |= bit1 << 10;

    if (ohmod & 0xC0)
        a |= bit2 << 11;

    if (ohmod & 0x4)
        c |= bit1 << 6;
    if (ohmod & 0xE8)
        c |= bit3 << 6;

    if (ohmod & 0x20)
        c |= bit2 << 7;


    if (ohmod & 0x5B)
        b0 |= bit0 << 6;
    if (ohmod & 0x5B)
        b1 |= bit1 << 6;

    if (ohmod & 0x12)
        b0 |= bit2 << 7;
    if (ohmod & 0x12)
        b1 |= bit3 << 7;

    if (ohmod & 0xAF)
        d0 |= bit4 << 5;
    if (ohmod & 0xAF)
        d1 |= bit5 << 5;
    if (ohmod & 0x5)
        d0 |= bit2 << 6;
    if (ohmod & 0x5)
        d1 |= bit3 << 6;

    // sign-extend 'd0' and 'd1'
    // note: this code assumes that signed right-shift actually sign-fills, not zero-fills.
    int32_t d0x = d0;
    int32_t d1x = d1;
    int sx_shamt = 32 - dbits;
    d0x <<= sx_shamt;
    d0x >>= sx_shamt;
    d1x <<= sx_shamt;
    d1x >>= sx_shamt;
    d0 = d0x;
    d1 = d1x;

    // expand all values to 12 bits, with left-shift as needed.
    int val_shamt = (modeval >> 1) ^ 3;
    a <<= val_shamt;
    b0 <<= val_shamt;
    b1 <<= val_shamt;
    c <<= val_shamt;
    d0 <<= val_shamt;
    d1 <<= val_shamt;

    // then compute the actual color values.
    int red1 = a;
    int green1 = a - b0;
    int blue1 = a - b1;
    int red0 = a - c;
    int green0 = a - b0 - c - d0;
    int blue0 = a - b1 - c - d1;

    // clamp the color components to [0,2^12 - 1]
    if (red0 < 0)
        red0 = 0;
    else if (red0 > 0xFFF)
        red0 = 0xFFF;

    if (green0 < 0)
        green0 = 0;
    else if (green0 > 0xFFF)
        green0 = 0xFFF;

    if (blue0 < 0)
        blue0 = 0;
    else if (blue0 > 0xFFF)
        blue0 = 0xFFF;

    if (red1 < 0)
        red1 = 0;
    else if (red1 > 0xFFF)
        red1 = 0xFFF;

    if (green1 < 0)
        green1 = 0;
    else if (green1 > 0xFFF)
        green1 = 0xFFF;

    if (blue1 < 0)
        blue1 = 0;
    else if (blue1 > 0xFFF)
        blue1 = 0xFFF;


    // switch around the color components
    int temp0, temp1;
    switch (majcomp) {
    case 1:                    // switch around red and green
        temp0 = red0;
        temp1 = red1;
        red0 = green0;
        red1 = green1;
        green0 = temp0;
        green1 = temp1;
        break;
    case 2:                    // swithc around red and blue
        temp0 = red0;
        temp1 = red1;
        red0 = blue0;
        red1 = blue1;
        blue0 = temp0;
        blue1 = temp1;
        break;
    case 0:                    // no switch
        break;
    }

    ushort4 rgb0 = { (ushort)(red0 << 4), (ushort)(green0 << 4), (ushort)(blue0 << 4), (ushort)0x7800 };
    ushort4 rgb1 = { (ushort)(red1 << 4), (ushort)(green1 << 4), (ushort)(blue1 << 4), (ushort)0x7800 };

    *output0 = rgb0;
    *output1 = rgb1;
}

void rgba_unpack(int input[8], int quantization_level, ushort4 * output0, ushort4 * output1) {
    int order = rgb_unpack(input, quantization_level, output0, output1);
    if (order == 0) {
        (*output0).w = color_unquantization_tables[quantization_level][input[6]];
        (*output1).w = color_unquantization_tables[quantization_level][input[7]];
    } else {
        (*output0).w = color_unquantization_tables[quantization_level][input[7]];
        (*output1).w = color_unquantization_tables[quantization_level][input[6]];
    }
}

void hdr_rgb_ldr_alpha_unpack3(int input[8], int quantization_level, ushort4 * output0, ushort4 * output1) {
    hdr_rgb_unpack3(input, quantization_level, output0, output1);

    int v6 = color_unquantization_tables[quantization_level][input[6]];
    int v7 = color_unquantization_tables[quantization_level][input[7]];
    (*output0).w = (ushort)v6;
    (*output1).w = (ushort)v7;
}

void rgba_delta_unpack(int input[8], int quantization_level, ushort4 * output0, ushort4 * output1) {
    int a0 = color_unquantization_tables[quantization_level][input[6]];
    int a1 = color_unquantization_tables[quantization_level][input[7]];
    a0 |= (a1 & 0x80) << 1;
    a1 &= 0x7F;
    if (a1 & 0x40)
        a1 -= 0x80;
    a0 >>= 1;
    a1 >>= 1;
    a1 += a0;

    if (a1 < 0)
        a1 = 0;
    else if (a1 > 255)
        a1 = 255;

    int order = rgb_delta_unpack(input, quantization_level, output0, output1);
    if (order == 0) {
        (*output0).w = (ushort)a0;
        (*output1).w = (ushort)a1;
    } else {
        (*output0).w = (ushort)a1;
        (*output1).w = (ushort)a0;
    }
}

void hdr_alpha_unpack(int input[2], int quantization_level, int *a0, int *a1) {

    int v6 = color_unquantization_tables[quantization_level][input[0]];
    int v7 = color_unquantization_tables[quantization_level][input[1]];

    int selector = ((v6 >> 7) & 1) | ((v7 >> 6) & 2);
    v6 &= 0x7F;
    v7 &= 0x7F;
    if (selector == 3) {
        *a0 = v6 << 5;
        *a1 = v7 << 5;
    } else {
        v6 |= (v7 << (selector + 1)) & 0x780;
        v7 &= (0x3f >> selector);
        v7 ^= 32 >> selector;
        v7 -= 32 >> selector;
        v6 <<= (4 - selector);
        v7 <<= (4 - selector);
        v7 += v6;

        if (v7 < 0)
            v7 = 0;
        else if (v7 > 0xFFF)
            v7 = 0xFFF;

        *a0 = v6;
        *a1 = v7;
    }

    *a0 <<= 4;
    *a1 <<= 4;
}

void hdr_rgb_hdr_alpha_unpack3(int input[8], int quantization_level, ushort4 * output0, ushort4 * output1) {
    hdr_rgb_unpack3(input, quantization_level, output0, output1);

    int alpha0, alpha1;
    hdr_alpha_unpack(input + 6, quantization_level, &alpha0, &alpha1);

    (*output0).w = (ushort)alpha0;
    (*output1).w = (ushort)alpha1;
}

void unpack_color_endpoints(
    int format,
    int quantization_level,
    int *input,
    int *rgb_hdr,                           // out
    int *alpha_hdr,                         // out
    int *nan_endpoint,                      // out
    ushort4 * output0, ushort4 * output1,   // out
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("unpack_color_endpoints");
    *nan_endpoint = 0;

//#ifdef __OPENCL_VERSION__
//    if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//        printf("format %d quantization_level %d *input %d\n",  format, quantization_level, *input);


    switch (format) {
    case FMT_LUMINANCE:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        luminance_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_LUMINANCE_DELTA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        luminance_delta_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_HDR_LUMINANCE_SMALL_RANGE:
        *rgb_hdr = 1;
        *alpha_hdr = -1;
        hdr_luminance_small_range_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_HDR_LUMINANCE_LARGE_RANGE:
        *rgb_hdr = 1;
        *alpha_hdr = -1;
        hdr_luminance_large_range_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_LUMINANCE_ALPHA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        luminance_alpha_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_LUMINANCE_ALPHA_DELTA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        luminance_alpha_delta_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_RGB_SCALE:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        rgb_scale_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_RGB_SCALE_ALPHA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        rgb_scale_alpha_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_HDR_RGB_SCALE:
        *rgb_hdr = 1;
        *alpha_hdr = -1;
        hdr_rgbo_unpack3(input, quantization_level, output0, output1);
        break;

    case FMT_RGB:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        rgb_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_RGB_DELTA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        rgb_delta_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_HDR_RGB:
        *rgb_hdr = 1;
        *alpha_hdr = -1;
        hdr_rgb_unpack3(input, quantization_level, output0, output1);
        break;

    case FMT_RGBA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        rgba_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_RGBA_DELTA:
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        rgba_delta_unpack(input, quantization_level, output0, output1);
        break;

    case FMT_HDR_RGB_LDR_ALPHA:
        *rgb_hdr = 1;
        *alpha_hdr = 0;
        hdr_rgb_ldr_alpha_unpack3(input, quantization_level, output0, output1);
        break;

    case FMT_HDR_RGBA:
        *rgb_hdr = 1;
        *alpha_hdr = 1;
        hdr_rgb_hdr_alpha_unpack3(input, quantization_level, output0, output1);
        break;

    default:
        astc_codec_internal_error("ERROR: unpack_color_endpoints");
        break;
    }



    if (*alpha_hdr == -1) {
        if (ASTCEncode->m_alpha_force_use_of_hdr) {
            (*output0).w = 0x7800;
            (*output1).w = 0x7800;
            *alpha_hdr = 1;
        } else {
            (*output0).w = 0x00FF;
            (*output1).w = 0x00FF;
            *alpha_hdr = 0;
        }
    }



    switch (ASTCEncode->m_decode_mode) {
    case DECODE_LDR_SRGB:
        if (*rgb_hdr == 1) {
            (*output0).x = 0xFF00;
            (*output0).y = 0x0000;
            (*output0).z = 0xFF00;
            (*output0).w = 0xFF00;
            (*output1).x = 0xFF00;
            (*output1).y = 0x0000;
            (*output1).z = 0xFF00;
            (*output1).w = 0xFF00;
        } else {
            (*output0).x *= 257;
            (*output0).y *= 257;
            (*output0).z *= 257;
            (*output0).w *= 257;
            (*output1).x *= 257;
            (*output1).y *= 257;
            (*output1).z *= 257;
            (*output1).w *= 257;
        }
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        break;

    case DECODE_LDR:
        if (*rgb_hdr == 1) {
            (*output0).x = 0xFFFF;
            (*output0).y = 0xFFFF;
            (*output0).z = 0xFFFF;
            (*output0).w = 0xFFFF;
            (*output1).x = 0xFFFF;
            (*output1).y = 0xFFFF;
            (*output1).z = 0xFFFF;
            (*output1).w = 0xFFFF;
            *nan_endpoint = 1;
        } else {
            (*output0).x *= 257;
            (*output0).y *= 257;
            (*output0).z *= 257;
            (*output0).w *= 257;
            (*output1).x *= 257;
            (*output1).y *= 257;
            (*output1).z *= 257;
            (*output1).w *= 257;
        }
        *rgb_hdr = 0;
        *alpha_hdr = 0;
        break;

    case DECODE_HDR:

        if (*rgb_hdr == 0) {
            (*output0).x *= 257;
            (*output0).y *= 257;
            (*output0).z *= 257;
            (*output1).x *= 257;
            (*output1).y *= 257;
            (*output1).z *= 257;
        }
        if (*alpha_hdr == 0) {
            (*output0).w *= 257;
            (*output1).w *= 257;
        }
        break;
    }

//#ifdef __OPENCL_VERSION__
//    if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//        printf("(*output0) %d %d %d %d (*output1) %d %d %d %d \n", (*output0).x, (*output0).y, (*output0).z, (*output0).w, (*output1).x, (*output1).y, (*output1).z, (*output1).w);
//


}

ushort4 lerp_color_int(
    ushort4 color0,
    ushort4 color1,
    int4 tmp_weight1,
    __global ASTC_Encode *ASTCEncode
) {
    DEBUG("lerp_color_int");
    int4 ecolor0        = { color0.x, color0.y, color0.z, color0.w };
    int4 ecolor1        = { color1.x, color1.y, color1.z, color1.w };
    int4 tmp_weight0 = { 64, 64, 64, 64 };

    tmp_weight0 = tmp_weight0 - tmp_weight1;

    if (ASTCEncode->m_decode_mode == DECODE_LDR_SRGB) {
        ecolor0 = ecolor0 >> 8;
        ecolor1 = ecolor1 >> 8;
    }

    int4 color = { 32, 32, 32, 32 };

    ecolor0 = ecolor0 * tmp_weight0;
    ecolor1 = ecolor1 * tmp_weight1;

    color = ecolor0 + ecolor1 + color;
    color = color >> 6;
    if (ASTCEncode->m_decode_mode == DECODE_LDR_SRGB)
        color = color | (color << 8);

    ushort4 rcolor  = { (ushort)color.x, (ushort)color.y, (ushort)color.z, (ushort)color.w };

    return rcolor;
}

ushort4 COMPUTE_LRP_COLOR(
    ushort4                 color0,
    ushort4                 color1,
    float                   plane1_weight,
    float                   plane2_weight,
    int                     plane2_color_component,
    __global ASTC_Encode    *ASTCEncode
) {
    DEBUG("COMPUTE_LRP_COLOR");
    int4    tmp_weight0 = { 64,64,64,64 };
    int4    tmp_weight1;

    int int_plane1_weight = (int)floor(plane1_weight);
    int int_plane2_weight = (int)floor(plane2_weight);

    //# Patch for LLVM intrinsic space error
    tmp_weight1.x = int_plane1_weight;
    tmp_weight1.y = int_plane1_weight;
    tmp_weight1.z = int_plane1_weight;
    tmp_weight1.w = int_plane1_weight;

    switch (plane2_color_component) {
    case 0:
        tmp_weight1.x = int_plane2_weight;
        break;
    case 1:
        tmp_weight1.y = int_plane2_weight;
        break;
    case 2:
        tmp_weight1.z = int_plane2_weight;
        break;
    case 3:
        tmp_weight1.w = int_plane2_weight;
        break;
    default:
        break;
    }

//#ifdef __OPENCL_VERSION__
//    if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//        printf("color0 %d %d %d %d color1 %d %d %d %d \n", color0.x, color0.y, color0.z, color0.w, color1.x, color1.y, color1.z, color1.w);


//#ifdef __OPENCL_VERSION__
//    if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//        printf("tmp_weight %d %d %d %d\n", tmp_weight1.x, tmp_weight1.y, tmp_weight1.z, tmp_weight1.w);


    return lerp_color_int(
               color0,
               color1,
               tmp_weight1,
               ASTCEncode);
}

// for each weight, unquantize the weight, use it to compute a color and a color error.
// then, increment the weight until the color error stops decreasing
// then, decrement the weight until the color error stops increasing
void COMPUTE_ERROR(
    float                   *errorvar,
    int                     texels_to_evaluate,
    __global decimation_table        *it,
    __global partition_info          *pt,
    float                   uq_plane1_weights[MAX_WEIGHTS_PER_BLOCK],
    float                   uq_plane2_weights[MAX_WEIGHTS_PER_BLOCK],
    int                     is_dual_plane,
    ushort4                 color_endpoint0[4],
    ushort4                 color_endpoint1[4],
    int                     i,
    imageblock              *blk,
    int                     plane2_color_component,
    error_weight_block      *ewb,
    __global ASTC_Encode    *ASTCEncode) {
    DEBUG("COMPUTE_ERROR");
    *errorvar = 0.0f;
    for (int j = 0; j < texels_to_evaluate; j++) {
        int texel = it->weight_texel[i][j];
        int partition = pt->partition_of_texel[texel];
        float plane1_weight = compute_value_of_texel_flt_localVar(texel, it, uq_plane1_weights);
        float plane2_weight = 0.0f;
        if (is_dual_plane) {
            plane2_weight = compute_value_of_texel_flt_localVar(texel, it, uq_plane2_weights);
        }

//#ifdef __OPENCL_VERSION__
//        if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//        printf("plane1_weight %3.3f plane2_weight %3.3f  plane2_color_component %d\n", plane1_weight, plane2_weight, plane2_color_component);

        ushort4 lrp_color = COMPUTE_LRP_COLOR(
                                color_endpoint0[partition],
                                color_endpoint1[partition],
                                plane1_weight,
                                plane2_weight,
                                plane2_color_component,
                                ASTCEncode);

        float4 color = { (float)lrp_color.x, (float)lrp_color.y, (float)lrp_color.z, (float)lrp_color.w };
        float4 origcolor = {
            blk->work_data[4 * texel],
            blk->work_data[4 * texel + 1],
            blk->work_data[4 * texel + 2],
            blk->work_data[4 * texel + 3]
        };

//#ifdef __OPENCL_VERSION__
//        if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//            printf("color %3.3f %3.3f %3.3f %3.3f origcolor %3.3f %3.3f %3.3f %3.3f \n", color.x, color.y, color.z, color.w, origcolor.x, origcolor.y, origcolor.z, origcolor.w);

        float4 error_weight = ewb->error_weights[texel];
        float4 colordiff = color - origcolor;
        *errorvar += dot(colordiff*colordiff, error_weight);

//#ifdef __OPENCL_VERSION__
//        if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//            printf("%3.3f\n", *errorvar);

    }


}

int realign_weights(
    imageblock * blk,
    error_weight_block * ewb,
    symbolic_compressed_block * scb,
    __global2 uint8_t * weight_set8,
    __global2 uint8_t * plane2_weight_set8,
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("realign_weights");
    int i;

    // get the appropriate partition descriptor.
    int partition_count = scb->partition_count;

    // get the appropriate block descriptor
    int modeindex = ASTCEncode->bsd.block_modes[scb->block_mode].decimation_mode;

    int is_dual_plane = ASTCEncode->bsd.block_modes[scb->block_mode].is_dual_plane;

    // get quantization-parameters
    int weight_quantization_level = ASTCEncode->bsd.block_modes[scb->block_mode].quantization_mode;


    // decode the color endpoints
    ushort4 color_endpoint0[4];
    ushort4 color_endpoint1[4];
    int rgb_hdr[4];
    int alpha_hdr[4];
    int nan_endpoint[4];

    for (i = 0; i < partition_count; i++)
        unpack_color_endpoints(
            scb->color_formats[i], scb->color_quantization_level, scb->color_values[i], &rgb_hdr[i], &alpha_hdr[i],
            &nan_endpoint[i], &(color_endpoint0[i]), &(color_endpoint1[i]), ASTCEncode);

    float uq_plane1_weights[MAX_WEIGHTS_PER_BLOCK];
    float uq_plane2_weights[MAX_WEIGHTS_PER_BLOCK];
    int weight_count = ASTCEncode->bsd.decimation_tables[modeindex].num_weights;

    // read and unquantize the weights.
    __constant quantization_and_transfer_table *qat = &(quant_and_xfer_tables[weight_quantization_level]);

    for (i = 0; i < weight_count; i++) {
        uq_plane1_weights[i] = qat->unquantized_value_flt[weight_set8[i]];
    }
    if (is_dual_plane) {
        for (i = 0; i < weight_count; i++)
            uq_plane2_weights[i] = qat->unquantized_value_flt[plane2_weight_set8[i]];
    }


    int plane2_color_component = is_dual_plane ? scb->plane2_color_component : -1;

    // for each weight, unquantize the weight, use it to compute a color and a color error.
    // then, increment the weight until the color error stops decreasing
    // then, decrement the weight until the color error stops increasing

    int adjustments = 0;

    for (i = 0; i < weight_count; i++) {
        uint8_t current_wt = weight_set8[i];
        int texels_to_evaluate = ASTCEncode->bsd.decimation_tables[modeindex].weight_num_texels[i];

        float current_error;

        COMPUTE_ERROR(
            &current_error,
            texels_to_evaluate,
            &ASTCEncode->bsd.decimation_tables[modeindex],
            &ASTCEncode->partition_tables[partition_count][scb->partition_index],
            uq_plane1_weights,
            uq_plane2_weights,
            is_dual_plane,
            color_endpoint0,
            color_endpoint1,
            i,
            blk,
            plane2_color_component,
            ewb,
            ASTCEncode);

        // increment until error starts increasing.
        while (1) {
            uint8_t next_wt = qat->next_quantized_value[current_wt];
            if (next_wt == current_wt)
                break;
            uq_plane1_weights[i] = qat->unquantized_value_flt[next_wt];
            float next_error;

            COMPUTE_ERROR(
                &next_error,
                texels_to_evaluate,
                &ASTCEncode->bsd.decimation_tables[modeindex],
                &ASTCEncode->partition_tables[partition_count][scb->partition_index],
                uq_plane1_weights,
                uq_plane2_weights,
                is_dual_plane,
                color_endpoint0,
                color_endpoint1,
                i,
                blk,
                plane2_color_component,
                ewb,
                ASTCEncode);


            if (next_error < current_error) {
                // succeeded, increment the weight
                current_wt = next_wt;
                current_error = next_error;
                adjustments++;
            } else {
                // failed, back out the attempted increment
                uq_plane1_weights[i] = qat->unquantized_value_flt[current_wt];
                break;
            }
        }
        // decrement until error starts increasing
        while (1) {
            uint8_t prev_wt = qat->prev_quantized_value[current_wt];
            if (prev_wt == current_wt)
                break;
            uq_plane1_weights[i] = qat->unquantized_value_flt[prev_wt];
            float prev_error;
            COMPUTE_ERROR(
                &prev_error,
                texels_to_evaluate,
                &ASTCEncode->bsd.decimation_tables[modeindex],
                &ASTCEncode->partition_tables[partition_count][scb->partition_index],
                uq_plane1_weights,
                uq_plane2_weights,
                is_dual_plane,
                color_endpoint0,
                color_endpoint1,
                i,
                blk,
                plane2_color_component,
                ewb,
                ASTCEncode);
            if (prev_error < current_error) {
                // succeeded, decrement the weight
                current_wt = prev_wt;
                current_error = prev_error;
                adjustments++;
            } else {
                // failed, back out the attempted decrement
                uq_plane1_weights[i] = qat->unquantized_value_flt[current_wt];
                break;
            }
        }

        weight_set8[i] = current_wt;
    }

    if (!is_dual_plane)
        return adjustments;

    // processing of the second plane of weights
    for (i = 0; i < weight_count; i++) {
        uint8_t current_wt = plane2_weight_set8[i];
        int texels_to_evaluate = ASTCEncode->bsd.decimation_tables[modeindex].weight_num_texels[i];

        float current_error;

        COMPUTE_ERROR(
            &current_error,
            texels_to_evaluate,
            &ASTCEncode->bsd.decimation_tables[modeindex],
            &ASTCEncode->partition_tables[partition_count][scb->partition_index],
            uq_plane1_weights,
            uq_plane2_weights,
            is_dual_plane,
            color_endpoint0,
            color_endpoint1,
            i,
            blk,
            plane2_color_component,
            ewb,
            ASTCEncode);

        // increment until error starts increasing.
        while (1) {
            uint8_t next_wt = qat->next_quantized_value[current_wt];
            if (next_wt == current_wt)
                break;
            uq_plane2_weights[i] = qat->unquantized_value_flt[next_wt];
            float next_error;
            COMPUTE_ERROR(
                &next_error,
                texels_to_evaluate,
                &ASTCEncode->bsd.decimation_tables[modeindex],
                &ASTCEncode->partition_tables[partition_count][scb->partition_index],
                uq_plane1_weights,
                uq_plane2_weights,
                is_dual_plane,
                color_endpoint0,
                color_endpoint1,
                i,
                blk,
                plane2_color_component,
                ewb,
                ASTCEncode);

            if (next_error < current_error) {
                // succeeded, increment the weight
                current_wt = next_wt;
                current_error = next_error;
                adjustments++;
            } else {
                // failed, back out the attempted increment
                uq_plane2_weights[i] = qat->unquantized_value_flt[current_wt];
                break;
            }
        }
        // decrement until error starts increasing
        while (1) {
            uint8_t prev_wt = qat->prev_quantized_value[current_wt];
            if (prev_wt == current_wt)
                break;
            uq_plane1_weights[i] = qat->unquantized_value_flt[prev_wt];
            float prev_error;
            COMPUTE_ERROR(
                &prev_error,
                texels_to_evaluate,
                &ASTCEncode->bsd.decimation_tables[modeindex],
                &ASTCEncode->partition_tables[partition_count][scb->partition_index],
                uq_plane1_weights,
                uq_plane2_weights,
                is_dual_plane,
                color_endpoint0,
                color_endpoint1,
                i,
                blk,
                plane2_color_component,
                ewb,
                ASTCEncode);

            if (prev_error < current_error) {
                // succeeded, decrement the weight
                current_wt = prev_wt;
                current_error = prev_error;
                adjustments++;
            } else {
                // failed, back out the attempted decrement
                uq_plane2_weights[i] = qat->unquantized_value_flt[current_wt];
                break;
            }
        }

        plane2_weight_set8[i] = current_wt;
    }

    return adjustments;
}

void compress_symbolic_block_fixed_partition_1_plane(
    float mode_cutoff,
    int max_refinement_iters,
    int partition_count, int partition_index,
    imageblock * blk,
    error_weight_block * ewb,
    symbolic_compressed_block * scb,
    endpoints_and_weights *ei1,
    endpoints_and_weights *eix1,
    __global2 float   *decimated_weights,
    __global2 uint8_t *u8_quantized_decimated_quantized_weights,
    __global2 float   *decimated_quantized_weights,
    __global2 float   *flt_quantized_decimated_quantized_weights,
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("compress_symbolic_block_fixed_partition_1_plane");

    int i, j, k;
    int free_bits_for_partition_count[5] = { 0, 115 - 4, 111 - 4 - PARTITION_BITS, 108 - 4 - PARTITION_BITS, 105 - 4 - PARTITION_BITS };

    // first, compute ideal weights and endpoint colors, under thre assumption that
    // there is no quantization or decimation going on.
    compute_endpoints_and_ideal_weights_1_plane(&ASTCEncode->partition_tables[partition_count][partition_index], blk, ewb, ei1, ASTCEncode);

    // for each decimation mode, compute an ideal set of weights
    // (that is, weights computed with the assumption that they are not quantized)
    for (i = 0; i < MAX_DECIMATION_MODES; i++) {
        if (ASTCEncode->bsd.permit_encode[i] == 0 ||
                ASTCEncode->bsd.decimation_mode_maxprec_1plane[i] < 0 ||
                ASTCEncode->bsd.decimation_mode_percentile[i] > mode_cutoff)
            continue;
        eix1[i] = *ei1;
        compute_ideal_weights_for_decimation_table(&(eix1[i]),
                &ASTCEncode->bsd.decimation_tables[i],
                decimated_quantized_weights + i * MAX_WEIGHTS_PER_BLOCK,
                decimated_weights + i * MAX_WEIGHTS_PER_BLOCK);
    }

    // compute maximum colors for the endpoints and ideal weights.
    // for each endpoint-and-ideal-weight pair, compute the smallest weight value
    // that will result in a color value greater than 1.

    float4 min_ep = { 10.0f, 10.0f, 10.0f, 10.0f };
    float4 onef   = { 1.0f, 1.0f, 1.0f, 1.0f };

    for (i = 0; i < partition_count; i++) {
        float4 ep = (onef - ei1->ep.endpt0[i]) / (ei1->ep.endpt1[i] - ei1->ep.endpt0[i]);
        if (ep.x > 0.5f && ep.x < min_ep.x)
            min_ep.x = ep.x;
        if (ep.y > 0.5f && ep.y < min_ep.y)
            min_ep.y = ep.y;
        if (ep.z > 0.5f && ep.z < min_ep.z)
            min_ep.z = ep.z;
        if (ep.w > 0.5f && ep.w < min_ep.w)
            min_ep.w = ep.w;
    }

    float min_wt_cutoff = (float)(std::min)((std::min)(min_ep.x, min_ep.y), (std::min)(min_ep.z, min_ep.w));

    // for each mode, use the angular method to compute a shift.
    float weight_low_value[MAX_WEIGHT_MODES];
    float weight_high_value[MAX_WEIGHT_MODES];

    compute_angular_endpoints_1plane(mode_cutoff,
                                     decimated_quantized_weights,
                                     decimated_weights,
                                     weight_low_value, weight_high_value,
                                     ASTCEncode);

    // for each mode (which specifies a decimation and a quantization):
    // * compute number of bits needed for the quantized weights.
    // * generate an optimized set of quantized weights.
    // * compute quantization errors for the mode.

    int qwt_bitcounts[MAX_WEIGHT_MODES];
    float qwt_errors[MAX_WEIGHT_MODES];

    for (i = 0; i < MAX_WEIGHT_MODES; i++) {
        if (ASTCEncode->bsd.block_modes[i].permit_encode == 0 || ASTCEncode->bsd.block_modes[i].is_dual_plane != 0 || ASTCEncode->bsd.block_modes[i].percentile > mode_cutoff) {
            qwt_errors[i] = FLOAT_38;
            continue;
        }
        if (weight_high_value[i] > 1.02f * min_wt_cutoff)
            weight_high_value[i] = 1.0f;

        int decimation_mode = ASTCEncode->bsd.block_modes[i].decimation_mode;
        if (ASTCEncode->bsd.decimation_mode_percentile[decimation_mode] > mode_cutoff) {
            astc_codec_internal_error("ERROR: compress_symbolic_block_fixed_partition_1_plane");
        }

        // compute weight bitcount for the mode
        uint8_t qmode = ASTCEncode->bsd.block_modes[i].quantization_mode;
        int nweights = ASTCEncode->bsd.decimation_tables[decimation_mode].num_weights;
        int bits_used_by_weights = compute_ise_bitcount(nweights, (quantization_method)qmode);

        int bitcount = free_bits_for_partition_count[partition_count] - bits_used_by_weights;
        if (bitcount <= 0 || bits_used_by_weights < 24 || bits_used_by_weights > 96) {
            qwt_errors[i] = FLOAT_38;
            continue;
        }
        qwt_bitcounts[i] = bitcount;


        // then, generate the optimized set of weights for the weight mode.
        compute_ideal_quantized_weights_for_decimation_table(&(eix1[decimation_mode]),
                &ASTCEncode->bsd.decimation_tables[decimation_mode],
                weight_low_value[i], weight_high_value[i],
                decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * decimation_mode,
                flt_quantized_decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * i,
                u8_quantized_decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * i,
                ASTCEncode->bsd.block_modes[i].quantization_mode);
//#ifdef __OPENCL_VERSION__
//         if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//             printf("A1:eix1[decimation_mode].ep.endpt0[0].x %3.3f\n", eix1[decimation_mode].ep.endpt0[0].x);

        // then, compute weight-errors for the weight mode.

        qwt_errors[i] = compute_error_of_weight_set(&(eix1[decimation_mode]),
                        &ASTCEncode->bsd.decimation_tables[decimation_mode],
                        flt_quantized_decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * i);
//#ifdef __OPENCL_VERSION__
//         if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//             printf("A2:eix1[decimation_mode].ep.endpt0[0].x %3.3f\n", eix1[decimation_mode].ep.endpt0[0].x);


    }

    // for each weighting mode, determine the optimal combination of color endpoint encodings
    // and weight encodings; return results for the 4 best-looking modes.

    int partition_format_specifiers[4][4];
    int quantized_weight[4];
    int color_quantization_level[4];
    int color_quantization_level_mod[4];

    determine_optimal_set_of_endpoint_formats_to_use( &ASTCEncode->partition_tables[partition_count][partition_index],
            blk,
            ewb,
            &(ei1->ep),
            -1,    // used to flag that we are in single-weight mode
            qwt_bitcounts,
            qwt_errors,
            partition_format_specifiers,
            quantized_weight,
            color_quantization_level,
            color_quantization_level_mod,
            ASTCEncode);

    // then iterate over the 4 believed-to-be-best modes to find out which one is
    // actually best.
    for (i = 0; i < 4; i++) {
        __global2 uint8_t *u8_weight_src;
        int weights_to_copy;

        if (quantized_weight[i] < 0) {
            scb->error_block = 1;
            scb++;
            continue;
        }

        int decimation_mode = ASTCEncode->bsd.block_modes[quantized_weight[i]].decimation_mode;
        int weight_quantization_mode = ASTCEncode->bsd.block_modes[quantized_weight[i]].quantization_mode;

        u8_weight_src = u8_quantized_decimated_quantized_weights + (MAX_WEIGHTS_PER_BLOCK * quantized_weight[i]);

        weights_to_copy = ASTCEncode->bsd.decimation_tables[decimation_mode].num_weights;

        // recompute the ideal color endpoints before storing them.
        float4 rgbs_colors[4];
        float4 rgbo_colors[4];
        float2 lum_intervals[4];

        int l;
        for (l = 0; l < max_refinement_iters; l++) {
//#ifdef __OPENCL_VERSION__
//             if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//             printf("B1:eix1[decimation_mode].ep.endpt0[0].x %3.3f\n", eix1[decimation_mode].ep.endpt0[0].x);

            recompute_ideal_colors(weight_quantization_mode,
                                   &(eix1[decimation_mode].ep),
                                   rgbs_colors, rgbo_colors,
                                   lum_intervals, u8_weight_src,
                                   0, -1, &ASTCEncode->partition_tables[partition_count][partition_index],
                                   &ASTCEncode->bsd.decimation_tables[decimation_mode],
                                   blk, ewb, ASTCEncode);

            // quantize the chosen color
//#ifdef __OPENCL_VERSION__
//             if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//                 printf("B2:eix1[decimation_mode].ep.endpt0[0].x %3.3f\n", eix1[decimation_mode].ep.endpt0[0].x);

            // store the colors for the block
            for (j = 0; j < partition_count; j++) {
                scb->color_formats[j] = pack_color_endpoints(eix1[decimation_mode].ep.endpt0[j],
                                        eix1[decimation_mode].ep.endpt1[j],
                                        rgbs_colors[j], rgbo_colors[j], partition_format_specifiers[i][j],
                                        scb->color_values[j], color_quantization_level[i]);
            }


            // if all the color endpoint modes are the same, we get a few more
            // bits to store colors; let's see if we can take advantage of this:
            // requantize all the colors and see if the endpoint modes remain the same;
            // if they do, then exploit it.
            scb->color_formats_matched = 0;

            if (
                (partition_count >= 2)
                &&  ((scb->color_formats[0]) == scb->color_formats[1])
                // &&  (color_quantization_level != color_quantization_level_mod)  ASTC bug needs fixing
                &&  (color_quantization_level[i] != color_quantization_level_mod[i])  // Added [i] as a tmp fix!
                &&  ((partition_count == 2) || (scb->color_formats[0] == scb->color_formats[2]))
                &&  ((partition_count == 3)  || (scb->color_formats[0] == scb->color_formats[3]))
            ) {
                int colorvals[4][12];
                int color_formats_mod[4];
                for (j = 0; j < partition_count; j++) {
                    color_formats_mod[j] = pack_color_endpoints(eix1[decimation_mode].ep.endpt0[j],
                                           eix1[decimation_mode].ep.endpt1[j],
                                           rgbs_colors[j], rgbo_colors[j],
                                           partition_format_specifiers[i][j], colorvals[j], color_quantization_level_mod[i]);
                }


                if (color_formats_mod[0] == color_formats_mod[1]
                        && (partition_count == 2 || (color_formats_mod[0] == color_formats_mod[2] && (partition_count == 3 || (color_formats_mod[0] == color_formats_mod[3]))))) {
                    scb->color_formats_matched = 1;
                    for (j = 0; j < 4; j++)
                        for (k = 0; k < 12; k++)
                            scb->color_values[j][k] = colorvals[j][k];
                    for (j = 0; j < 4; j++)
                        scb->color_formats[j] = color_formats_mod[j];
                }
            }


            // store header fields
            scb->partition_count = partition_count;
            scb->partition_index = partition_index;
            scb->color_quantization_level = scb->color_formats_matched ? color_quantization_level_mod[i] : color_quantization_level[i];
            scb->block_mode = quantized_weight[i];
            scb->error_block = 0;

            if (scb->color_quantization_level < 4) {
                scb->error_block = 1;    // should never happen, but cannot prove it impossible.
            }

            // perform a final pass over the weights to try to improve them.
            int adjustments = realign_weights(
                                  blk, ewb, scb,
                                  u8_weight_src,
                                  0, ASTCEncode);

            if (adjustments == 0)
                break;
        }

        for (j = 0; j < weights_to_copy; j++)
            scb->plane1_weights[j] = u8_weight_src[j];

        scb++;
    }

}

int compute_value_of_texel_int(int texel_to_get, decimation_table * it, int *weights) {
    int i;
    int summed_value = 8;
    int weights_to_evaluate = it->texel_num_weights[texel_to_get];
    for (i = 0; i < weights_to_evaluate; i++) {
        summed_value += weights[it->texel_weights[texel_to_get][i]] * it->texel_weights_int[texel_to_get][i];
    }
    return summed_value >> 4;
}

int compute_value_of_texel_global(int texel_to_get, __global decimation_table * it, int *weights) {
    int i;
    int summed_value = 8;
    int weights_to_evaluate = it->texel_num_weights[texel_to_get];
    for (i = 0; i < weights_to_evaluate; i++) {
        summed_value += weights[it->texel_weights[texel_to_get][i]] * it->texel_weights_int[texel_to_get][i];
    }
    return summed_value >> 4;
}

/* 32-bit count-leading-zeroes function: use the Assembly instruction whenever possible. */
uint32_t clz32(uint32_t inp) {
    /* slow default version */
    uint32_t summa = 24;
    if (inp >= (uint32_t)(0x10000)) {
        inp >>= 16;
        summa -= 16;
    }
    if (inp >= (uint32_t)(0x100)) {
        inp >>= 8;
        summa -= 8;
    }
    return summa + clz_table[inp];
}

/* convert from FP16 to FP32. */
CGU_UINT sf16_to_sf32(CGU_SHORT inp) {
    uint32_t inpx = inp;

    int32_t res = tbl2[inpx >> 10];
    res += inpx;

    /* the normal cases: the MSB of 'res' is not set. */
    if (res >= 0)                /* signed compare */
        return res << 13;

    /* Infinity and Zero: the bottom 10 bits of 'res' are clear. */
    if ((res & (uint32_t)(0x3FF)) == 0)
        return res << 13;

    /* NaN: the exponent field of 'inp' is not zero; NaNs must be quitened. */
    if ((inpx & 0x7C00) != 0)
        return (res << 13) | (uint32_t)(0x400000);

    /* the remaining cases are Denormals. */
    {
        uint32_t sign = (inpx & (uint32_t)(0x8000)) << 16;
        uint32_t mskval = inpx & (uint32_t)(0x7FFF);
        uint32_t leadingzeroes = clz32(mskval);
        mskval <<= leadingzeroes;
        return (mskval >> 8) + ((0x85 - leadingzeroes) << 23) + sign;
    }
}

/* convert from soft-float to native-float */
float sf16_to_float(CGU_SHORT p) {
    if32 i;
    i.u = sf16_to_sf32(p);
    return i.f;
}

// conversion function from 16-bit LDR value to FP16.
// note: for LDR interpolation, it is impossible to get a denormal result;
// this simplifies the conversion.
// FALSE; we can receive a very small UNORM16 through the constant-block.
uint16_t unorm16_to_sf16(uint16_t p) {
    if (p == 0xFFFF)
        return 0x3C00;            // value of 1.0 .
    if (p < 4)
        return p << 8;

    int lz = clz32(p) - 16;
    p <<= (lz + 1);
    p >>= 6;
    p |= (14 - lz) << 10;
    return p;
}

uint16_t lns_to_sf16(uint16_t p) {

    uint16_t mc = p & 0x7FF;
    uint16_t ec = p >> 11;
    uint16_t mt;
    if (mc < 512)
        mt = 3 * mc;
    else if (mc < 1536)
        mt = 4 * mc - 512;
    else
        mt = 5 * mc - 2048;

    uint16_t res = (ec << 10) | (mt >> 3);
    if (res >= 0x7BFF)
        res = 0x7BFF;
    return res;
}

// helper function to initialize the orig-data from the work-data
void imageblock_initialize_orig_from_work(imageblock * blk, int pixelcount) {
    DEBUG("imageblock_initialize_orig_from_work");
    int i;
    float *fptr = blk->orig_data;
    float *wptr = blk->work_data;

    for (i = 0; i < pixelcount; i++) {
        if (blk->rgb_lns[i]) {
            fptr[0] = sf16_to_float(lns_to_sf16((uint16_t)wptr[0]));
            fptr[1] = sf16_to_float(lns_to_sf16((uint16_t)wptr[1]));
            fptr[2] = sf16_to_float(lns_to_sf16((uint16_t)wptr[2]));
        } else {
            fptr[0] = sf16_to_float(unorm16_to_sf16((uint16_t)wptr[0]));
            fptr[1] = sf16_to_float(unorm16_to_sf16((uint16_t)wptr[1]));
            fptr[2] = sf16_to_float(unorm16_to_sf16((uint16_t)wptr[2]));
        }

        if (blk->alpha_lns[i]) {
            fptr[3] = sf16_to_float(lns_to_sf16((uint16_t)wptr[3]));
        } else {
            fptr[3] = sf16_to_float(unorm16_to_sf16((uint16_t)wptr[3]));
        }

        fptr += 4;
        wptr += 4;
    }

    imageblock_initialize_deriv_from_work_and_orig(blk, pixelcount);
}

void decompress_symbolic_block(
    symbolic_compressed_block * scb,
    imageblock * blk,
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("decompress_symbolic_block");
    int i;

    // if we detected an error-block, blow up immediately.
    if (scb->error_block) {
        if (ASTCEncode->m_decode_mode == DECODE_LDR_SRGB) {
            for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
                blk->orig_data[4 * i] = 1.0f;
                blk->orig_data[4 * i + 1] = 0.0f;
                blk->orig_data[4 * i + 2] = 1.0f;
                blk->orig_data[4 * i + 3] = 1.0f;
                blk->rgb_lns[i] = 0;
                blk->alpha_lns[i] = 0;
                blk->nan_texel[i] = 0;
            }
        } else {
            for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
                blk->orig_data[4 * i] = 0.0f;
                blk->orig_data[4 * i + 1] = 0.0f;
                blk->orig_data[4 * i + 2] = 0.0f;
                blk->orig_data[4 * i + 3] = 0.0f;
                blk->rgb_lns[i] = 0;
                blk->alpha_lns[i] = 0;
                blk->nan_texel[i] = 1;
            }
        }

        imageblock_initialize_work_from_orig(blk, ASTCEncode->m_texels_per_block);
        update_imageblock_flags(blk, ASTCEncode);
        return;
    }

    if (scb->block_mode < 0) {
        float red = 0, green = 0, blue = 0, alpha = 0;
        int use_lns = 0;
        int use_nan = 0;

        if (scb->block_mode == -2) {
            // For sRGB decoding, we should return only the top 8 bits.
            int mask = (ASTCEncode->m_decode_mode == DECODE_LDR_SRGB) ? 0xFF00 : 0xFFFF;

            red = sf16_to_float(unorm16_to_sf16((uint16_t)scb->constant_color[0] & mask));
            green = sf16_to_float(unorm16_to_sf16((uint16_t)scb->constant_color[1] & mask));
            blue = sf16_to_float(unorm16_to_sf16((uint16_t)scb->constant_color[2] & mask));
            alpha = sf16_to_float(unorm16_to_sf16((uint16_t)scb->constant_color[3] & mask));
            use_lns = 0;
            use_nan = 0;
        } else {
            switch (ASTCEncode->m_decode_mode) {
            case DECODE_LDR_SRGB:
                red = 1.0f;
                green = 0.0f;
                blue = 1.0f;
                alpha = 1.0f;
                use_lns = 0;
                use_nan = 0;
                break;
            case DECODE_LDR:
                red = 0.0f;
                green = 0.0f;
                blue = 0.0f;
                alpha = 0.0f;
                use_lns = 0;
                use_nan = 1;
                break;
            case DECODE_HDR:
                // constant-color block; unpack from FP16 to FP32.
                red = sf16_to_float((CGU_SHORT)scb->constant_color[0]);
                green = sf16_to_float((CGU_SHORT)scb->constant_color[1]);
                blue = sf16_to_float((CGU_SHORT)scb->constant_color[2]);
                alpha = sf16_to_float((CGU_SHORT)scb->constant_color[3]);
                use_lns = 1;
                use_nan = 0;
                break;
            }
        }

        for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
            blk->orig_data[4 * i] = red;
            blk->orig_data[4 * i + 1] = green;
            blk->orig_data[4 * i + 2] = blue;
            blk->orig_data[4 * i + 3] = alpha;
            blk->rgb_lns[i] = (uint8_t)use_lns;
            blk->alpha_lns[i] = (uint8_t)use_lns;
            blk->nan_texel[i] = (uint8_t)use_nan;
        }


        imageblock_initialize_work_from_orig(blk, ASTCEncode->m_texels_per_block);
        update_imageblock_flags(blk, ASTCEncode);
        return;
    }

    // get the appropriate partition-table entry
    int partition_count = scb->partition_count;

    // get the appropriate block descriptor
    int is_dual_plane = ASTCEncode->bsd.block_modes[scb->block_mode].is_dual_plane;
    int weight_quantization_level = ASTCEncode->bsd.block_modes[scb->block_mode].quantization_mode;


    // decode the color endpoints
    ushort4 color_endpoint0[4];
    ushort4 color_endpoint1[4];
    int rgb_hdr_endpoint[4];
    int alpha_hdr_endpoint[4];
    int nan_endpoint[4];

    for (i = 0; i < partition_count; i++)
        unpack_color_endpoints(
            scb->color_formats[i],
            scb->color_quantization_level, scb->color_values[i], &(rgb_hdr_endpoint[i]), &(alpha_hdr_endpoint[i]),
            &(nan_endpoint[i]), &(color_endpoint0[i]), &(color_endpoint1[i]), ASTCEncode);

    // first unquantize the weights
    int uq_plane1_weights[MAX_WEIGHTS_PER_BLOCK];
    int uq_plane2_weights[MAX_WEIGHTS_PER_BLOCK];
    int weight_count = ASTCEncode->bsd.decimation_tables[ASTCEncode->bsd.block_modes[scb->block_mode].decimation_mode].num_weights;

    __constant quantization_and_transfer_table *qat = &(quant_and_xfer_tables[weight_quantization_level]);

    for (i = 0; i < weight_count; i++) {
        uq_plane1_weights[i] = qat->unquantized_value[scb->plane1_weights[i]];
    }
    if (is_dual_plane) {
        for (i = 0; i < weight_count; i++)
            uq_plane2_weights[i] = qat->unquantized_value[scb->plane2_weights[i]];
    }

    // then un-decimate them.
    float weights[MAX_TEXELS_PER_BLOCK];
    float plane2_weights[MAX_TEXELS_PER_BLOCK];

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++)
        weights[i] = (float)compute_value_of_texel_global(i, &ASTCEncode->bsd.decimation_tables[ASTCEncode->bsd.block_modes[scb->block_mode].decimation_mode], uq_plane1_weights);

    if (is_dual_plane)
        for (i = 0; i < ASTCEncode->m_texels_per_block; i++)
            plane2_weights[i] = (float)compute_value_of_texel_global(i, &ASTCEncode->bsd.decimation_tables[ASTCEncode->bsd.block_modes[scb->block_mode].decimation_mode], uq_plane2_weights);

    int plane2_color_component = scb->plane2_color_component;

    // now that we have endpoint colors and weights, we can unpack actual colors for
    // each texel.
    ushort4 color;
    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        int partition = ASTCEncode->partition_tables[partition_count][scb->partition_index].partition_of_texel[i];
        if (partition > 3) partition = 3;


        color = COMPUTE_LRP_COLOR(
                    color_endpoint0[partition],
                    color_endpoint1[partition],
                    weights[i],
                    plane2_weights[i],
                    is_dual_plane ? plane2_color_component : -1,
                    ASTCEncode );

        blk->rgb_lns[i] = (uint8_t)rgb_hdr_endpoint[partition];
        blk->alpha_lns[i] = (uint8_t)alpha_hdr_endpoint[partition];
        blk->nan_texel[i] = (uint8_t)nan_endpoint[partition];

        blk->work_data[4 * i] = color.x;
        blk->work_data[4 * i + 1] = color.y;
        blk->work_data[4 * i + 2] = color.z;
        blk->work_data[4 * i + 3] = color.w;
    }

    imageblock_initialize_orig_from_work(blk, ASTCEncode->m_texels_per_block);

    update_imageblock_flags(blk, ASTCEncode);
}

float compute_imageblock_difference(imageblock * p1, imageblock * p2, error_weight_block * ewb,
                                    __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_imageblock_difference");
    int i;
    float summa = 0.0f;
    float *f1 = p1->work_data;
    float *f2 = p2->work_data;
    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        float rdiff = (float)fabs(f1[4 * i]     - f2[4 * i]);
        float gdiff = (float)fabs(f1[4 * i + 1] - f2[4 * i + 1]);
        float bdiff = (float)fabs(f1[4 * i + 2] - f2[4 * i + 2]);
        float adiff = (float)fabs(f1[4 * i + 3] - f2[4 * i + 3]);
        rdiff = MIN(rdiff, FLOAT_15);
        gdiff = MIN(gdiff, FLOAT_15);
        bdiff = MIN(bdiff, FLOAT_15);
        adiff = MIN(adiff, FLOAT_15);
        summa += rdiff * rdiff * ewb->error_weights[i].x + gdiff * gdiff * ewb->error_weights[i].y + bdiff * bdiff * ewb->error_weights[i].z + adiff * adiff * ewb->error_weights[i].w;
    }
    return summa;
}

// compute averages and covariance matrices for 4 components
//static
void compute_covariance_matrix(imageblock * blk, error_weight_block * ewb, mat4t * cov_matrix, __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_covariance_matrix");
    int i;
    float r_sum = 0.0f;
    float g_sum = 0.0f;
    float b_sum = 0.0f;
    float a_sum = 0.0f;
    float rr_sum = 0.0f;
    float gg_sum = 0.0f;
    float bb_sum = 0.0f;
    float aa_sum = 0.0f;
    float rg_sum = 0.0f;
    float rb_sum = 0.0f;
    float ra_sum = 0.0f;
    float gb_sum = 0.0f;
    float ga_sum = 0.0f;
    float ba_sum = 0.0f;

    float weight_sum = 0.0f;

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        float weight = ewb->texel_weight[i];
        if (weight < 0.0f) {
            astc_codec_internal_error("ERROR: compute_covariance_matrix");
        }
        weight_sum += weight;
        float r = blk->work_data[4 * i];
        float g = blk->work_data[4 * i + 1];
        float b = blk->work_data[4 * i + 2];
        float a = blk->work_data[4 * i + 3];
        r_sum += r * weight;
        rr_sum += r * (r * weight);
        rg_sum += g * (r * weight);
        rb_sum += b * (r * weight);
        ra_sum += a * (r * weight);
        g_sum += g * weight;
        gg_sum += g * (g * weight);
        gb_sum += b * (g * weight);
        ga_sum += a * (g * weight);
        b_sum += b * weight;
        bb_sum += b * (b * weight);
        ba_sum += a * (b * weight);
        a_sum += a * weight;
        aa_sum += a * (a * weight);
    }

    float rpt = 1.0f / MAX(weight_sum, FLOAT_n7);
    float rs = r_sum;
    float gs = g_sum;
    float bs = b_sum;
    float as = a_sum;

    float4 cf0 = {rr_sum - rs * rs * rpt, rg_sum - rs * gs * rpt, rb_sum - rs * bs * rpt, ra_sum - rs * as * rpt};
    float4 cf1 = {rg_sum - rs * gs * rpt, gg_sum - gs * gs * rpt, gb_sum - gs * bs * rpt, ga_sum - gs * as * rpt};
    float4 cf2 = {rb_sum - rs * bs * rpt, gb_sum - gs * bs * rpt, bb_sum - bs * bs * rpt, ba_sum - bs * as * rpt};
    float4 cf3 = {ra_sum - rs * as * rpt, ga_sum - gs * as * rpt, ba_sum - bs * as * rpt, aa_sum - as * as * rpt};


    cov_matrix->v[0] = cf0;
    cov_matrix->v[1] = cf1;
    cov_matrix->v[2] = cf2;
    cov_matrix->v[3] = cf3;

}

void prepare_block_statistics(
    imageblock * blk,
    error_weight_block * ewb,
    int *is_normal_map,
    float *lowest_correl,
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("prepare_block_statistics");
    int i;

    mat4t cov_matrix;

    compute_covariance_matrix( blk, ewb, &cov_matrix, ASTCEncode);

    // use the covariance matrix to compute
    // correllation coefficients
    float rr_var = cov_matrix.v[0].x;
    float gg_var = cov_matrix.v[1].y;
    float bb_var = cov_matrix.v[2].z;
    float aa_var = cov_matrix.v[3].w;

    float rg_correlation = cov_matrix.v[0].y / sqrt(MAX(rr_var * gg_var, FLOAT_n30));
    float rb_correlation = cov_matrix.v[0].z / sqrt(MAX(rr_var * bb_var, FLOAT_n30));
    float ra_correlation = cov_matrix.v[0].w / sqrt(MAX(rr_var * aa_var, FLOAT_n30));
    float gb_correlation = cov_matrix.v[1].z / sqrt(MAX(gg_var * bb_var, FLOAT_n30));
    float ga_correlation = cov_matrix.v[1].w / sqrt(MAX(gg_var * aa_var, FLOAT_n30));
    float ba_correlation = cov_matrix.v[2].w / sqrt(MAX(bb_var * aa_var, FLOAT_n30));

    if (astc_isnan(rg_correlation))
        rg_correlation = 1.0f;
    if (astc_isnan(rb_correlation))
        rb_correlation = 1.0f;
    if (astc_isnan(ra_correlation))
        ra_correlation = 1.0f;
    if (astc_isnan(gb_correlation))
        gb_correlation = 1.0f;
    if (astc_isnan(ga_correlation))
        ga_correlation = 1.0f;
    if (astc_isnan(ba_correlation))
        ba_correlation = 1.0f;

    float lowest_correlation = MIN(fabs(rg_correlation), fabs(rb_correlation));
    lowest_correlation = MIN(lowest_correlation, fabs(ra_correlation));
    lowest_correlation = MIN(lowest_correlation, fabs(gb_correlation));
    lowest_correlation = MIN(lowest_correlation, fabs(ga_correlation));
    lowest_correlation = MIN(lowest_correlation, fabs(ba_correlation));
    *lowest_correl = lowest_correlation;

    // compute a "normal-map" factor
    // this factor should be exactly 0.0 for a normal map, while it may be all over the
    // place for anything that is NOT a normal map. We can probably assume that a factor
    // of less than 0.2f represents a normal map.

    float nf_sum = 0.0f;

    float3 midf = {0.5f, 0.5f, 0.5f};
    for (i = 0; i <  ASTCEncode->m_texels_per_block; i++) {
        float3 val = {blk->orig_data[4 * i],
                      blk->orig_data[4 * i + 1],
                      blk->orig_data[4 * i + 2]
                     };
        val = (val - midf) * 2.0f;
        float length_squared = dot(val, val);
        float nf = fabs(length_squared - 1.0f);
        nf_sum += nf;
    }
    float nf_avg = nf_sum / ASTCEncode->m_texels_per_block;
    *is_normal_map = nf_avg < 0.2;
}

void compute_angular_endpoints_2planes(float mode_cutoff,
                                       __global2 float *decimated_quantized_weights,
                                       __global2 float *decimated_weights,
                                       float low_value1[MAX_WEIGHT_MODES],
                                       float high_value1[MAX_WEIGHT_MODES],
                                       float low_value2[MAX_WEIGHT_MODES],
                                       float high_value2[MAX_WEIGHT_MODES],
                                       __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_angular_endpoints_2planes");
    int i;
    float low_values1[MAX_DECIMATION_MODES][12];
    float high_values1[MAX_DECIMATION_MODES][12];
    float low_values2[MAX_DECIMATION_MODES][12];
    float high_values2[MAX_DECIMATION_MODES][12];

    for (i = 0; i < MAX_DECIMATION_MODES; i++) {
        int samplecount     = ASTCEncode->bsd.decimation_mode_samples[i];
        int quant_mode      = ASTCEncode->bsd.decimation_mode_maxprec_2planes[i];
        float percentile    = ASTCEncode->bsd.decimation_mode_percentile[i];
        int permit_encode   = ASTCEncode->bsd.permit_encode[i];
        if (permit_encode == 0 || samplecount < 1 || quant_mode < 0 || percentile > mode_cutoff)
            continue;

        compute_angular_endpoints_for_quantization_levels(samplecount,
                decimated_quantized_weights + 2 * i * MAX_WEIGHTS_PER_BLOCK,
                decimated_weights + 2 * i * MAX_WEIGHTS_PER_BLOCK, quant_mode, low_values1[i], high_values1[i], ASTCEncode);

        compute_angular_endpoints_for_quantization_levels(samplecount,
                decimated_quantized_weights + (2 * i + 1) * MAX_WEIGHTS_PER_BLOCK,
                decimated_weights + (2 * i + 1) * MAX_WEIGHTS_PER_BLOCK, quant_mode, low_values2[i], high_values2[i],ASTCEncode);

    }

    for (i = 0; i < MAX_WEIGHT_MODES; i++) {
        if (ASTCEncode->bsd.block_modes[i].is_dual_plane != 1 || ASTCEncode->bsd.block_modes[i].percentile > mode_cutoff)
            continue;
        int quant_mode = ASTCEncode->bsd.block_modes[i].quantization_mode;
        int decim_mode = ASTCEncode->bsd.block_modes[i].decimation_mode;

        low_value1[i] = low_values1[decim_mode][quant_mode];
        high_value1[i] = high_values1[decim_mode][quant_mode];
        low_value2[i] = low_values2[decim_mode][quant_mode];
        high_value2[i] = high_values2[decim_mode][quant_mode];
    }
}

void compress_symbolic_block_fixed_partition_2_planes(
    float mode_cutoff,
    int max_refinement_iters,
    int partition_count,
    int partition_index,
    int separate_component,
    imageblock * blk,
    error_weight_block * ewb,
    symbolic_compressed_block * scb,
    endpoints_and_weights *ei1,
    endpoints_and_weights *ei2,
    endpoints_and_weights *eix1,
    endpoints_and_weights *eix2,
    __global2 float   *decimated_weights,
    __global2 uint8_t *u8_quantized_decimated_quantized_weights,
    __global2 float   *decimated_quantized_weights,
    __global2 float   *flt_quantized_decimated_quantized_weights,
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("compress_symbolic_block_fixed_partition_2_planes");
    int i, j, k;
    int free_bits_for_partition_count[5] =
    { 0, 113 - 4, 109 - 4 - PARTITION_BITS, 106 - 4 - PARTITION_BITS, 103 - 4 - PARTITION_BITS };

    // first, compute ideal weights and endpoint colors
    compute_endpoints_and_ideal_weights_2_planes(&ASTCEncode->partition_tables[partition_count][partition_index], blk, ewb, separate_component, ei1, ei2, ASTCEncode);

    // for each decimation mode, compute an ideal set of weights
    for (i = 0; i < MAX_DECIMATION_MODES; i++) {
        if (ASTCEncode->bsd.permit_encode[i] == 0 || ASTCEncode->bsd.decimation_mode_maxprec_2planes[i] < 0 || ASTCEncode->bsd.decimation_mode_percentile[i] > mode_cutoff)
            continue;

        eix1[i] = *ei1;
        eix2[i] = *ei2;
        compute_ideal_weights_for_decimation_table(&(eix1[i]),
                &ASTCEncode->bsd.decimation_tables[i],
                decimated_quantized_weights + (2 * i) * MAX_WEIGHTS_PER_BLOCK,
                decimated_weights + (2 * i) * MAX_WEIGHTS_PER_BLOCK);
        compute_ideal_weights_for_decimation_table(&(eix2[i]),
                &ASTCEncode->bsd.decimation_tables[i],
                decimated_quantized_weights + (2 * i + 1) * MAX_WEIGHTS_PER_BLOCK,
                decimated_weights + (2 * i + 1) * MAX_WEIGHTS_PER_BLOCK);
    }

    // compute maximum colors for the endpoints and ideal weights.
    // for each endpoint-and-ideal-weight pair, compute the smallest weight value
    // that will result in a color value greater than 1.

    float4 min_ep1 = {10.0f, 10.0f, 10.0f, 10.0f};
    float4 min_ep2 = {10.0f, 10.0f, 10.0f, 10.0f};
    for (i = 0; i < partition_count; i++) {
        float4 onef = {1.0f, 1.0f, 1.0f, 1.0f};
        float4 ep1 = (onef - ei1->ep.endpt0[i]) / (ei1->ep.endpt1[i] - ei1->ep.endpt0[i]);
        if (ep1.x > 0.5f && ep1.x < min_ep1.x)
            min_ep1.x = ep1.x;
        if (ep1.y > 0.5f && ep1.y < min_ep1.y)
            min_ep1.y = ep1.y;
        if (ep1.z > 0.5f && ep1.z < min_ep1.z)
            min_ep1.z = ep1.z;
        if (ep1.w > 0.5f && ep1.w < min_ep1.w)
            min_ep1.w = ep1.w;
        float4 ep2 = (onef - ei2->ep.endpt0[i]) / (ei2->ep.endpt1[i] - ei2->ep.endpt0[i]);
        if (ep2.x > 0.5f && ep2.x < min_ep2.x)
            min_ep2.x = ep2.x;
        if (ep2.y > 0.5f && ep2.y < min_ep2.y)
            min_ep2.y = ep2.y;
        if (ep2.z > 0.5f && ep2.z < min_ep2.z)
            min_ep2.z = ep2.z;
        if (ep2.w > 0.5f && ep2.w < min_ep2.w)
            min_ep2.w = ep2.w;
    }

    float min_wt_cutoff1, min_wt_cutoff2;
    switch (separate_component) {
    case 0:
        min_wt_cutoff2 = min_ep2.x;
        min_ep1.x = FLOAT_30;
        break;
    case 1:
        min_wt_cutoff2 = min_ep2.y;
        min_ep1.y = FLOAT_30;
        break;
    case 2:
        min_wt_cutoff2 = min_ep2.z;
        min_ep1.z = FLOAT_30;
        break;
    case 3:
        min_wt_cutoff2 = min_ep2.w;
        min_ep1.w = FLOAT_30;
        break;
    default:
        min_wt_cutoff2 = FLOAT_30;
    }

    min_wt_cutoff1 = MIN(MIN(min_ep1.x, min_ep1.y), MIN(min_ep1.z, min_ep1.w));

    float weight_low_value1[MAX_WEIGHT_MODES];
    float weight_high_value1[MAX_WEIGHT_MODES];
    float weight_low_value2[MAX_WEIGHT_MODES];
    float weight_high_value2[MAX_WEIGHT_MODES];

    compute_angular_endpoints_2planes(mode_cutoff,
                                      decimated_quantized_weights,
                                      decimated_weights, weight_low_value1,
                                      weight_high_value1, weight_low_value2, weight_high_value2,
                                      ASTCEncode);

    // for each mode (which specifies a decimation and a quantization):
    // * generate an optimized set of quantized weights.
    // * compute quantization errors for each mode
    // * compute number of bits needed for the quantized weights.

    int qwt_bitcounts[MAX_WEIGHT_MODES];
    float qwt_errors[MAX_WEIGHT_MODES];
    for (i = 0; i < MAX_WEIGHT_MODES; i++) {
        if (ASTCEncode->bsd.block_modes[i].permit_encode == 0 || ASTCEncode->bsd.block_modes[i].is_dual_plane != 1 || ASTCEncode->bsd.block_modes[i].percentile > mode_cutoff) {
            qwt_errors[i] = FLOAT_38;
            continue;
        }
        int decimation_mode = ASTCEncode->bsd.block_modes[i].decimation_mode;

        if (weight_high_value1[i] > 1.02f * min_wt_cutoff1)
            weight_high_value1[i] = 1.0f;
        if (weight_high_value2[i] > 1.02f * min_wt_cutoff2)
            weight_high_value2[i] = 1.0f;

        // compute weight bitcount for the mode
        int bits_used_by_weights = compute_ise_bitcount(2 * ASTCEncode->bsd.decimation_tables[decimation_mode].num_weights,
                                   (quantization_method)ASTCEncode->bsd.block_modes[i].quantization_mode);
        int bitcount = free_bits_for_partition_count[partition_count] - bits_used_by_weights;
        if (bitcount <= 0 || bits_used_by_weights < 24 || bits_used_by_weights > 96) {
            qwt_errors[i] = FLOAT_38;
            continue;
        }
        qwt_bitcounts[i] = bitcount;


        // then, generate the optimized set of weights for the mode.
        compute_ideal_quantized_weights_for_decimation_table(&(eix1[decimation_mode]),
                &ASTCEncode->bsd.decimation_tables[decimation_mode],
                weight_low_value1[i],
                weight_high_value1[i],
                decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * (2 * decimation_mode),
                flt_quantized_decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * (2 * i),
                u8_quantized_decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * (2 * i),
                ASTCEncode->bsd.block_modes[i].quantization_mode);

        compute_ideal_quantized_weights_for_decimation_table(&(eix2[decimation_mode]),
                &ASTCEncode->bsd.decimation_tables[decimation_mode],
                weight_low_value2[i],
                weight_high_value2[i],
                decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * (2 * decimation_mode + 1),
                flt_quantized_decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * (2 * i + 1),
                u8_quantized_decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * (2 * i + 1),
                ASTCEncode->bsd.block_modes[i].quantization_mode);


        // then, compute quantization errors for the block mode.
        qwt_errors[i] =
            compute_error_of_weight_set(&(eix1[decimation_mode]),
                                        &ASTCEncode->bsd.decimation_tables[decimation_mode],
                                        flt_quantized_decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * (2 * i))
            + compute_error_of_weight_set(&(eix2[decimation_mode]),
                                          &ASTCEncode->bsd.decimation_tables[decimation_mode],
                                          flt_quantized_decimated_quantized_weights + MAX_WEIGHTS_PER_BLOCK * (2 * i + 1));
    }


    // decide the optimal combination of color endpoint encodings and weight encoodings.
    int partition_format_specifiers[4][4];
    int quantized_weight[4];
    int color_quantization_level[4];
    int color_quantization_level_mod[4];

    endpoints epm;
    merge_endpoints(&(ei1->ep), &(ei2->ep), separate_component, &epm);

    determine_optimal_set_of_endpoint_formats_to_use(
        &ASTCEncode->partition_tables[partition_count][partition_index],
        blk,
        ewb,
        &epm, separate_component, qwt_bitcounts,
        qwt_errors,
        partition_format_specifiers,
        quantized_weight,
        color_quantization_level, color_quantization_level_mod,
        ASTCEncode);

    for (i = 0; i < 4; i++) {
        if (quantized_weight[i] < 0) {
            scb->error_block = 1;
            scb++;
            continue;
        }

        __global2 uint8_t *u8_weight1_src;
        __global2 uint8_t *u8_weight2_src;
        int weights_to_copy;

        int decimation_mode = ASTCEncode->bsd.block_modes[quantized_weight[i]].decimation_mode;
        int weight_quantization_mode = ASTCEncode->bsd.block_modes[quantized_weight[i]].quantization_mode;

        int weight1_offfset = MAX_WEIGHTS_PER_BLOCK * (2 * quantized_weight[i]);
        int weight2_offfset = MAX_WEIGHTS_PER_BLOCK * (2 * quantized_weight[i] + 1);
        u8_weight1_src = u8_quantized_decimated_quantized_weights + weight1_offfset;
        u8_weight2_src = u8_quantized_decimated_quantized_weights + weight2_offfset;


        weights_to_copy = ASTCEncode->bsd.decimation_tables[decimation_mode].num_weights;

        // recompute the ideal color endpoints before storing them.
        merge_endpoints(&(eix1[decimation_mode].ep), &(eix2[decimation_mode].ep), separate_component, &epm);

        float4 rgbs_colors[4];
        float4 rgbo_colors[4];
        float2 lum_intervals[4];

        int l;
        for (l = 0; l < max_refinement_iters; l++) {
            recompute_ideal_colors(weight_quantization_mode,
                                   &epm, rgbs_colors, rgbo_colors, lum_intervals,
                                   u8_weight1_src,
                                   u8_weight2_src,
                                   separate_component, &ASTCEncode->partition_tables[partition_count][partition_index],
                                   &ASTCEncode->bsd.decimation_tables[decimation_mode],
                                   blk, ewb,
                                   ASTCEncode);

            // store the colors for the block
            for (j = 0; j < partition_count; j++) {
                scb->color_formats[j] = pack_color_endpoints(epm.endpt0[j],
                                        epm.endpt1[j],
                                        rgbs_colors[j], rgbo_colors[j],  partition_format_specifiers[i][j], scb->color_values[j], color_quantization_level[i]);
            }
            scb->color_formats_matched = 0;

            if ((partition_count >= 2 && scb->color_formats[0] == scb->color_formats[1]
//#                && color_quantization_level != color_quantization_level_mod
                )
                    && (partition_count == 2 || (scb->color_formats[0] == scb->color_formats[2] && (partition_count == 3 || (scb->color_formats[0] == scb->color_formats[3]))))) {
                int colorvals[4][12];
                int color_formats_mod[4];
                for (j = 0; j < partition_count; j++) {
                    color_formats_mod[j] = pack_color_endpoints(epm.endpt0[j],
                                           epm.endpt1[j],
                                           rgbs_colors[j], rgbo_colors[j],  partition_format_specifiers[i][j], colorvals[j], color_quantization_level_mod[i]);
                }
                if (color_formats_mod[0] == color_formats_mod[1]
                        && (partition_count == 2 || (color_formats_mod[0] == color_formats_mod[2] && (partition_count == 3 || (color_formats_mod[0] == color_formats_mod[3]))))) {
                    scb->color_formats_matched = 1;
                    for (j = 0; j < 4; j++)
                        for (k = 0; k < 12; k++)
                            scb->color_values[j][k] = colorvals[j][k];
                    for (j = 0; j < 4; j++)
                        scb->color_formats[j] = color_formats_mod[j];
                }
            }


            // store header fields
            scb->partition_count = partition_count;
            scb->partition_index = partition_index;
            scb->color_quantization_level = scb->color_formats_matched ? color_quantization_level_mod[i] : color_quantization_level[i];
            scb->block_mode = quantized_weight[i];
            scb->plane2_color_component = separate_component;
            scb->error_block = 0;

            if (scb->color_quantization_level < 4) {
                scb->error_block = 1;    // should never happen, but cannot prove it impossible
            }

            int adjustments = realign_weights(
                                  blk, ewb, scb,
                                  u8_weight1_src,
                                  u8_weight2_src,
                                  ASTCEncode);

            if (adjustments == 0)
                break;
        }

        for (j = 0; j < weights_to_copy; j++) {
            scb->plane1_weights[j] = u8_weight1_src[j];
            scb->plane2_weights[j] = u8_weight2_src[j];
        }

        scb++;
    }

}

// for k++ means, we need pseudorandom numbers, however using random numbers directly
// results in irreproducible encoding results. As such, we will instead
// just supply a handful of numbers from random.org, and apply an algorithm similar
// to XKCD #221. (http://xkcd.com/221/)
// cluster the texels using the k++ means clustering initialization algorithm.
void kpp_initialize(int partition_count, imageblock * blk, float4 * cluster_centers, __global ASTC_Encode *ASTCEncode) {
    DEBUG("kpp_initialize");
    int i;

    int cluster_center_samples[4];
    // pick a random sample as first center-point.
    cluster_center_samples[0] = 145897 /* number from random.org */ % ASTCEncode->m_texels_per_block;
    int samples_selected = 1;

    float distances[MAX_TEXELS_PER_BLOCK];

    // compute the distance to the first point.
    int sample = cluster_center_samples[0];
    float4 center_color = { blk->work_data[4 * sample],
                            blk->work_data[4 * sample + 1],
                            blk->work_data[4 * sample + 2],
                            blk->work_data[4 * sample + 3]
                          };

    float distance_sum = 0.0f;
    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        float4 color = { blk->work_data[4 * i],
                         blk->work_data[4 * i + 1],
                         blk->work_data[4 * i + 2],
                         blk->work_data[4 * i + 3]
                       };
        float4 diff = color - center_color;
        float distance = dot(diff, diff);
        distance_sum += distance;
        distances[i] = distance;
    }


    bool isTrue = true;
    while (isTrue) {
        // pick a point in a weighted-random fashion.
        float summa = 0.0f;
        float distance_cutoff = distance_sum * cluster_cutoffs[samples_selected + 5 * partition_count];
        for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
            summa += distances[i];
            if (summa >= distance_cutoff)
                break;
        }
        sample = i;
        if (sample >= ASTCEncode->m_texels_per_block)
            sample = ASTCEncode->m_texels_per_block - 1;


        cluster_center_samples[samples_selected] = sample;
        samples_selected++;
        if (samples_selected >= partition_count)
            break;

        // update the distances with the new point.
        float4 cr = { blk->work_data[4 * sample], blk->work_data[4 * sample + 1], blk->work_data[4 * sample + 2], blk->work_data[4 * sample + 3] };
        center_color = cr;

        distance_sum = 0.0f;
        for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
            float4 color = { blk->work_data[4 * i],
                             blk->work_data[4 * i + 1],
                             blk->work_data[4 * i + 2],
                             blk->work_data[4 * i + 3]
                           };
            float4 diff = color - center_color;
            float distance = dot(diff, diff);
            distance = MIN(distance, distances[i]);
            distance_sum += distance;
            distances[i] = distance;
        }
    }

    // finally, gather up the results.
    for (i = 0; i < partition_count; i++) {
        int sample1 = cluster_center_samples[i];
        float4 color = { blk->work_data[4 * sample1],
                         blk->work_data[4 * sample1 + 1],
                         blk->work_data[4 * sample1 + 2],
                         blk->work_data[4 * sample1 + 3]
                       };
        cluster_centers[i] = color;
    }
}

// basic K-means clustering: given a set of cluster centers,
// assign each texel to a partition
void basic_kmeans_assign_pass(int partition_count, imageblock * blk, float4 * cluster_centers, int *partition_of_texel, __global ASTC_Encode *ASTCEncode) {
    DEBUG("basic_kmeans_assign_pass");
    int i, j;

    float distances[MAX_TEXELS_PER_BLOCK];
    float4 center_color = cluster_centers[0];

    int texels_per_partition[4];

    texels_per_partition[0] = ASTCEncode->m_texels_per_block;
    for (i = 1; i < partition_count; i++)
        texels_per_partition[i] = 0;


    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        float4 color = { blk->work_data[4 * i],
                         blk->work_data[4 * i + 1],
                         blk->work_data[4 * i + 2],
                         blk->work_data[4 * i + 3]
                       };
        float4 diff = color - center_color;
        float distance = dot(diff, diff);
        distances[i] = distance;
        partition_of_texel[i] = 0;
    }



    for (j = 1; j < partition_count; j++) {
        float4 center_color1 = cluster_centers[j];

        for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
            float4 color = { blk->work_data[4 * i],
                             blk->work_data[4 * i + 1],
                             blk->work_data[4 * i + 2],
                             blk->work_data[4 * i + 3]
                           };
            float4 diff = color - center_color1;
            float distance = dot(diff, diff);
            if (distance < distances[i]) {
                distances[i] = distance;
                texels_per_partition[partition_of_texel[i]]--;
                texels_per_partition[j]++;
                partition_of_texel[i] = j;
            }
        }
    }

    // it is possible to get a situation where one of the partitions ends up
    // without any texels. In this case, we assign texel N to partition N;
    // this is silly, but ensures that every partition retains at least one texel.
    // Reassigning a texel in this manner may cause another partition to go empty,
    // so if we actually did a reassignment, we run the whole loop over again.
    int problem_case;
    do {
        problem_case = 0;
        for (i = 0; i < partition_count; i++) {
            if (texels_per_partition[i] == 0) {
                texels_per_partition[partition_of_texel[i]]--;
                texels_per_partition[i]++;
                partition_of_texel[i] = i;
                problem_case = 1;
            }
        }
    } while (problem_case != 0);

}

// basic k-means clustering: given a set of cluster assignments
// for the texels, find the center position of each cluster.
void basic_kmeans_update(int partition_count, imageblock * blk, int *partition_of_texel, float4 * cluster_centers, __global ASTC_Encode *ASTCEncode) {
    DEBUG("basic_kmeans_update");
    int i;

    float4 color_sum[4];
    int weight_sum[4];
    float4 zero4f = { 0, 0, 0, 0 };
    for (i = 0; i < partition_count; i++) {
        color_sum[i] = zero4f;
        weight_sum[i] = 0;
    }


    // first, find the center-of-gravity in each cluster
    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        float4 color = { blk->work_data[4 * i],
                         blk->work_data[4 * i + 1],
                         blk->work_data[4 * i + 2],
                         blk->work_data[4 * i + 3]
                       };
        int part = partition_of_texel[i];
        color_sum[part] = color_sum[part] + color;
        weight_sum[part]++;
    }

    for (i = 0; i < partition_count; i++) {
        cluster_centers[i] = color_sum[i] * (1.0f / weight_sum[i]);
    }
}


// after a few rounds of k-means-clustering, we should have a set of 2, 3 or 4 partitions;
// we  turn this set into 2, 3 or 4 bitmaps. Then, for each of the ( 64 bit uses 1024) (32 bit uses ???) partitions,
// we try to match the bitmaps as well as possible.
//# maybe an issue for GPU to work in 64bits
//static inline
int bitcount(uint64_cl p) {
#ifdef ENABLE_64Bit_Support
    bool isTrue = sizeof(void *) > 4;
    if (isTrue) {
        uint64_cl mask1 = 0x5555555555555555ULL;
        uint64_cl mask2 = 0x3333333333333333ULL;
        uint64_cl mask3 = 0x0F0F0F0F0F0F0F0FULL;
        // best-known algorithm for 64-bit bitcount, assuming 64-bit processor
        // should probably be adapted for use with 32-bit processors and/or processors
        // with a POPCNT instruction, but leave that for later.
        p -= (p >> 1) & mask1;
        p = (p & mask2) + ((p >> 2) & mask2);
        p += p >> 4;
        p &= mask3;
        p *= 0x0101010101010101ULL;
        p >>= 56;
        return (int)p;
    } else
#endif
    {
        uint32_t mask1 = 0x55555555U;
        uint32_t mask2 = 0x33333333U;
        uint32_t mask3 = 0x0F0F0F0FU;

        // on 32-bit processor, split the 64-bit input argument in two,
        // and bitcount each half separately.
        uint32_t p1 = (uint32_t)p;

        p1 = p1 - ((p1 >> 1) & mask1);
        p1 = (p1 & mask2) + ((p1 >> 2) & mask2);
        p1 += p1 >> 4;
        p1 &= mask3;

#ifdef ENABLE_64Bit_Support
        uint32_t p2 = (uint32_t)(p >> 32);
        p2 = p2 - ((p2 >> 1) & mask1);
        p2 = (p2 & mask2) + ((p2 >> 2) & mask2);
        p2 += p2 >> 4;
        p2 &= mask3;
        p1 += p2;
#endif
        p1 *= 0x01010101U;
        p1 >>= 24;
        return (int)p1;
    }
}

//static inline
int MIN3(int a, int b, int c) {
    int d = MIN(a, b);
    return MIN(c, d);
}

// compute the bit-mismatch for a partitioning in 2-partition mode
//# maybe an issue for GPU to work in 64bits
//static inline
int partition_mismatch2(uint64_cl a0, uint64_cl a1, uint64_cl b0, uint64_cl b1) {
    int v1 = bitcount(a0 ^ b0) + bitcount(a1 ^ b1);
    int v2 = bitcount(a0 ^ b1) + bitcount(a1 ^ b0);
    return MIN(v1, v2);
}

#ifdef ENABLE_3_PARTITION_CODE
// compute the bit-mismatch for a partitioning in 3-partition mode
//# maybe an issue for GPU to work in 64bits
static inline int partition_mismatch3(uint64_cl a0, uint64_cl a1, uint64_cl a2, uint64_cl b0, uint64_cl b1, uint64_cl b2) {
    int p00 = bitcount(a0 ^ b0);
    int p01 = bitcount(a0 ^ b1);
    int p02 = bitcount(a0 ^ b2);

    int p10 = bitcount(a1 ^ b0);
    int p11 = bitcount(a1 ^ b1);
    int p12 = bitcount(a1 ^ b2);

    int p20 = bitcount(a2 ^ b0);
    int p21 = bitcount(a2 ^ b1);
    int p22 = bitcount(a2 ^ b2);

    int s0 = p11 + p22;
    int s1 = p12 + p21;
    int v0 = MIN(s0, s1) + p00;

    int s2 = p10 + p22;
    int s3 = p12 + p20;
    int v1 = MIN(s2, s3) + p01;

    int s4 = p10 + p21;
    int s5 = p11 + p20;
    int v2 = MIN(s4, s5) + p02;

    if (v1 < v0)
        v0 = v1;
    if (v2 < v0)
        v0 = v2;

    // 9 add, 5 MIN

    return v0;
}
#endif

#ifdef ENABLE_4_PARTITION_CODE
// compute the bit-mismatch for a partitioning in 4-partition mode
//# maybe an issue for GPU to work in 64bits
static inline int partition_mismatch4(uint64_cl a0, uint64_cl a1, uint64_cl a2, uint64_cl a3, uint64_cl b0, uint64_cl b1, uint64_cl b2, uint64_cl b3) {
    int p00 = bitcount(a0 ^ b0);
    int p01 = bitcount(a0 ^ b1);
    int p02 = bitcount(a0 ^ b2);
    int p03 = bitcount(a0 ^ b3);

    int p10 = bitcount(a1 ^ b0);
    int p11 = bitcount(a1 ^ b1);
    int p12 = bitcount(a1 ^ b2);
    int p13 = bitcount(a1 ^ b3);

    int p20 = bitcount(a2 ^ b0);
    int p21 = bitcount(a2 ^ b1);
    int p22 = bitcount(a2 ^ b2);
    int p23 = bitcount(a2 ^ b3);

    int p30 = bitcount(a3 ^ b0);
    int p31 = bitcount(a3 ^ b1);
    int p32 = bitcount(a3 ^ b2);
    int p33 = bitcount(a3 ^ b3);

    int mx23 = MIN(p22 + p33, p23 + p32);
    int mx13 = MIN(p21 + p33, p23 + p31);
    int mx12 = MIN(p21 + p32, p22 + p31);
    int mx03 = MIN(p20 + p33, p23 + p30);
    int mx02 = MIN(p20 + p32, p22 + p30);
    int mx01 = MIN(p21 + p30, p20 + p31);

    int v0 = p00 + MIN3(p11 + mx23, p12 + mx13, p13 + mx12);
    int v1 = p01 + MIN3(p10 + mx23, p12 + mx03, p13 + mx02);
    int v2 = p02 + MIN3(p11 + mx03, p10 + mx13, p13 + mx01);
    int v3 = p03 + MIN3(p11 + mx02, p12 + mx01, p10 + mx12);

    int x0 = MIN(v0, v1);
    int x1 = MIN(v2, v3);
    return MIN(x0, x1);

    // 16 bitcount, 17 MIN, 28 ADD
}
#endif

//# maybe an issue for GPU to work in 64bits
void count_partition_mismatch_bits( int partition_count,
                                    uint64_cl bitmaps[4],
                                    int bitcounts[PARTITION_COUNT],
                                    __global ASTC_Encode *ASTCEncode) {
    DEBUG("count_partition_mismatch_bits");
    int i;

    if (partition_count == 2) {
        uint64_cl bm0 = bitmaps[0];
        uint64_cl bm1 = bitmaps[1];
        for (i = 0; i < PARTITION_COUNT; i++) {
            if (ASTCEncode->partition_tables[partition_count][i].partition_count == 2) {
                bitcounts[i] = partition_mismatch2(bm0, bm1,
                                                   ASTCEncode->partition_tables[partition_count][i].coverage_bitmaps[0],
                                                   ASTCEncode->partition_tables[partition_count][i].coverage_bitmaps[1]);
            } else
                bitcounts[i] = 255;
        }
    }
#ifdef ENABLE_3_PARTITION_CODE
    else if (partition_count == 3) {
        uint64_cl bm0 = bitmaps[0];
        uint64_cl bm1 = bitmaps[1];
        uint64_cl bm2 = bitmaps[2];
        for (i = 0; i < PARTITION_COUNT; i++) {
            if (ASTCEncode->partition_tables[partition_count][i].partition_count == 3) {
                bitcounts[i] = partition_mismatch3(bm0, bm1, bm2,
                                                   ASTCEncode->partition_tables[partition_count][i].coverage_bitmaps[0],
                                                   ASTCEncode->partition_tables[partition_count][i].coverage_bitmaps[1],
                                                   ASTCEncode->partition_tables[partition_count][i].coverage_bitmaps[2]);
            } else
                bitcounts[i] = 255;
        }
    }
#endif
#ifdef ENABLE_4_PARTITION_CODE
    else if (partition_count == 4) {
        uint64_cl bm0 = bitmaps[0];
        uint64_cl bm1 = bitmaps[1];
        uint64_cl bm2 = bitmaps[2];
        uint64_cl bm3 = bitmaps[3];
        for (i = 0; i < PARTITION_COUNT; i++) {
            if (ASTCEncode->partition_tables[partition_count][i].partition_count == 4) {
                bitcounts[i] = partition_mismatch4(bm0, bm1, bm2, bm3,
                                                   ASTCEncode->partition_tables[partition_count][i].coverage_bitmaps[0],
                                                   ASTCEncode->partition_tables[partition_count][i].coverage_bitmaps[1],
                                                   ASTCEncode->partition_tables[partition_count][i].coverage_bitmaps[2],
                                                   ASTCEncode->partition_tables[partition_count][i].coverage_bitmaps[3]);
            } else
                bitcounts[i] = 255;
        }
    }
#endif
}

// counting-sort on the mismatch-bits, thereby
// sorting the partitions into an ordering.
void get_partition_ordering_by_mismatch_bits(int mismatch_bits[PARTITION_COUNT], int partition_ordering[PARTITION_COUNT]) {
    DEBUG("get_partition_ordering_by_mismatch_bits");
    int i;

    int mscount[256];
    for (i = 0; i < 256; i++)
        mscount[i] = 0;

    for (i = 0; i < PARTITION_COUNT; i++)
        mscount[mismatch_bits[i]]++;

    int summa = 0;
    for (i = 0; i < 256; i++) {
        int cnt = mscount[i];
        mscount[i] = summa;
        summa += cnt;
    }

    for (i = 0; i < PARTITION_COUNT; i++) {
        int idx = mscount[mismatch_bits[i]]++;
        partition_ordering[idx] = i;
    }
}

void kmeans_compute_partition_ordering(int partition_count, imageblock * blk, int *ordering, __global ASTC_Encode *ASTCEncode) {
    DEBUG("kmeans_compute_partition_ordering");
    int i;
    float4 cluster_centers[4];
    int partition_of_texel[MAX_TEXELS_PER_BLOCK];

    // 3 passes of plain k-means partitioning
    for (i = 0; i < 3; i++) {
        if (i == 0)
            kpp_initialize(partition_count, blk, cluster_centers, ASTCEncode);
        else
            basic_kmeans_update(partition_count, blk, partition_of_texel, cluster_centers, ASTCEncode);

        basic_kmeans_assign_pass( partition_count, blk, cluster_centers, partition_of_texel, ASTCEncode);
    }

    // at this point, we have a near-ideal partitioning.

//#  // construct bitmaps
    uint64_cl shiftbit = 1;
    uint64_cl bitmaps[4];
    for (i = 0; i < 4; i++)
        bitmaps[i] = 0;
//# need to check the limit of 64bit when used in GPU
    int texels_to_process = ASTCEncode->bsd.texelcount_for_bitmap_partitioning;
    if (texels_to_process > COVERAGE_BITMAPS_MAX)
        texels_to_process = COVERAGE_BITMAPS_MAX;
    for (i = 0; i < texels_to_process; i++) {
        int idx = ASTCEncode->bsd.texels_for_bitmap_partitioning[i];
        bitmaps[partition_of_texel[idx]] |= shiftbit << i;
    }

    int bitcounts[PARTITION_COUNT];
    // for each entry in the partition table, count bits of partition-mismatch.
    count_partition_mismatch_bits( partition_count, bitmaps, bitcounts, ASTCEncode);

    // finally, sort the partitions by bits-of-partition-mismatch
    get_partition_ordering_by_mismatch_bits(bitcounts, ordering);

}

float compute_error_squared_rgba(__global  partition_info * pt,    // the partition that we use when computing the squared-error.
                                 imageblock * blk, error_weight_block * ewb, processed_line4 * plines, float *length_of_lines) {
    DEBUG("compute_error_squared_rgba");
    int i;

    float errorsum = 0.0f;
    int partition;
    for (partition = 0; partition < pt->partition_count; partition++) {
        int texelcount = pt->texels_per_partition[partition];
        float lowparam = FLOAT_10;
        float highparam = -FLOAT_10;

        processed_line4 l = plines[partition];

        if (ewb->contains_zeroweight_texels) {
            for (i = 0; i < texelcount; i++) {
                int iwt = pt->texels_of_partition[partition][i];
                if (ewb->texel_weight[iwt] > FLOAT_n20) {
                    float4 point = { blk->work_data[4 * iwt],
                                     blk->work_data[4 * iwt + 1],
                                     blk->work_data[4 * iwt + 2],
                                     blk->work_data[4 * iwt + 3]
                                   };
                    float param = dot(point, l.bs);
                    float4 rp1 = l.amod + param * l.bis;
                    float4 dist = rp1 - point;
                    float4 ews = ewb->error_weights[iwt];
                    errorsum += dot(ews, dist * dist);
                    if (param < lowparam)
                        lowparam = param;
                    if (param > highparam)
                        highparam = param;
                }
            }
        } else {
            for (i = 0; i < texelcount; i++) {
                int iwt = pt->texels_of_partition[partition][i];
                float4 point = { blk->work_data[4 * iwt], blk->work_data[4 * iwt + 1], blk->work_data[4 * iwt + 2], blk->work_data[4 * iwt + 3] };
                float param = dot(point, l.bs);
                float4 rp1 = l.amod + param * l.bis;
                float4 dist = rp1 - point;
                float4 ews = ewb->error_weights[iwt];
                errorsum += dot(ews, dist * dist);
                if (param < lowparam)
                    lowparam = param;
                if (param > highparam)
                    highparam = param;
            }
        }

        float linelen = highparam - lowparam;
        if (!(linelen > FLOAT_n7))
            linelen = FLOAT_n7;
        length_of_lines[partition] = linelen;
    }

    return errorsum;
}

void compute_alpha_minmax(__global partition_info * pt, imageblock * blk, error_weight_block * ewb, float *alpha_min, float *alpha_max, __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_alpha_minmax");
    int i;
    int partition_count = pt->partition_count;

    for (i = 0; i < partition_count; i++) {
        alpha_min[i] = FLOAT_38;
        alpha_max[i] = -FLOAT_38;
    }

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        if (ewb->texel_weight[i] > FLOAT_n10) {
            int partition = pt->partition_of_texel[i];
            float alphaval = blk->work_data[4 * i + 3];
            if (alphaval > alpha_max[partition])
                alpha_max[partition] = alphaval;
            if (alphaval < alpha_min[partition])
                alpha_min[partition] = alphaval;
        }
    }

    for (i = 0; i < partition_count; i++) {
        if (alpha_min[i] >= alpha_max[i]) {
            alpha_min[i] = 0;
            alpha_max[i] = FLOAT_n10;
        }
    }
}

#define XPASTE(x,y) x##y
#define PASTE(x,y) XPASTE(x,y)

#define TWO_COMPONENT_ERROR_FUNC( funcname, c0_iwt, c1_iwt, c01_name, c01_rname ) \
float funcname( \
    __global partition_info *pt, \
    imageblock *blk, \
    error_weight_block *ewb, \
    processed_line2 *plines, \
    float *length_of_lines \
    ) \
    { \
    int i; \
    float errorsum = 0.0f; \
    int partition; \
    for(partition=0; partition<pt->partition_count; partition++) \
        { \
        int texelcount = pt->texels_per_partition[ partition ]; \
        float lowparam = FLOAT_10; \
        float highparam = -FLOAT_10; \
        processed_line2 l = plines[partition]; \
        if( ewb->contains_zeroweight_texels ) \
            { \
            for(i=0;i<texelcount;i++) \
                { \
                int iwt = pt->texels_of_partition[ partition ][i]; \
                float texel_weight = ewb-> PASTE(texel_weight_ , c01_rname) [i]; \
                if( texel_weight > FLOAT_n20 ) \
                    { \
                    float2 point = {blk->work_data[4*iwt + c0_iwt], blk->work_data[4*iwt + c1_iwt] }; \
                    float param = dot( point, l.bs ); \
                    float2 rp1 = l.amod + param*l.bis; \
                    float2 dist = rp1 - point; \
                    float4 ews = ewb->error_weights[iwt]; \
                    errorsum += dot( ews. c01_name, dist*dist ); \
                    if( param < lowparam ) lowparam = param; \
                    if( param > highparam ) highparam = param; \
                    } \
                } \
            } \
        else \
            { \
            for(i=0;i<texelcount;i++) \
                { \
                int iwt = pt->texels_of_partition[ partition ][i]; \
                float2 point = {blk->work_data[4*iwt + c0_iwt], blk->work_data[4*iwt + c1_iwt]}; \
                float param = dot( point, l.bs ); \
                float2 rp1 = l.amod + param*l.bis; \
                float2 dist = rp1 - point; \
                float4 ews = ewb->error_weights[iwt]; \
                errorsum += dot( ews. c01_name, dist*dist ); \
                if( param < lowparam ) lowparam = param; \
                if( param > highparam ) highparam = param; \
                } \
            } \
        float linelen = highparam - lowparam; \
        if( !(linelen > FLOAT_n7) ) \
            linelen = FLOAT_n7; \
        length_of_lines[partition] = linelen; \
        } \
    return errorsum; \
    }

TWO_COMPONENT_ERROR_FUNC(compute_error_squared_rg, 0, 1, xy, rg)
TWO_COMPONENT_ERROR_FUNC(compute_error_squared_rb, 0, 2, xz, rb)
TWO_COMPONENT_ERROR_FUNC(compute_error_squared_gb, 1, 2, yz, gb)
TWO_COMPONENT_ERROR_FUNC(compute_error_squared_ra, 0, 3, zw, ra)

// function to compute the error across a tile when using a particular set of
// lines for a particular partitioning. Also compute the length of each
// color-space line in each partitioning.

#define THREE_COMPONENT_ERROR_FUNC( funcname, c0_iwt, c1_iwt, c2_iwt, c012_name, c012_rname ) \
float funcname( \
    __global partition_info *pt, \
    imageblock *blk, \
    error_weight_block *ewb, \
    processed_line3 *plines, \
    float *length_of_lines \
    ) \
    { \
    int i; \
    float errorsum = 0.0f; \
    int partition; \
    for(partition=0; partition<pt->partition_count; partition++) \
        { \
        int texelcount = pt->texels_per_partition[ partition ]; \
        float lowparam = FLOAT_10; \
        float highparam = -FLOAT_10; \
        processed_line3 l = plines[partition]; \
        if( ewb->contains_zeroweight_texels ) \
            { \
            for(i=0;i<texelcount;i++) \
                { \
                int iwt = pt->texels_of_partition[ partition ][i]; \
                float texel_weight = ewb-> PASTE(texel_weight_ , c012_rname) [i]; \
                if( texel_weight > FLOAT_n20 ) \
                    { \
                    float3 point = {blk->work_data[4*iwt + c0_iwt], blk->work_data[4*iwt + c1_iwt], blk->work_data[4*iwt + c2_iwt]}; \
                    float param = dot( point, l.bs ); \
                    float3 rp1 = l.amod + param*l.bis; \
                    float3 dist = rp1 - point; \
                    float4 ews = ewb->error_weights[iwt]; \
                    errorsum += dot( ews. c012_name, dist*dist ); \
                    if( param < lowparam ) lowparam = param; \
                    if( param > highparam ) highparam = param; \
                    } \
                } \
            } \
        else \
            { \
            for(i=0;i<texelcount;i++) \
                { \
                int iwt = pt->texels_of_partition[ partition ][i]; \
                float3 point = {blk->work_data[4*iwt + c0_iwt], blk->work_data[4*iwt + c1_iwt], blk->work_data[4*iwt + c2_iwt]}; \
                float param = dot( point, l.bs ); \
                float3 rp1 = l.amod + param*l.bis; \
                float3 dist = rp1 - point; \
                float4 ews = ewb->error_weights[iwt]; \
                errorsum += dot( ews. c012_name, dist*dist ); \
                if( param < lowparam ) lowparam = param; \
                if( param > highparam ) highparam = param; \
                } \
            } \
        float linelen = highparam - lowparam; \
        if( !(linelen > FLOAT_n7) ) \
            linelen = FLOAT_n7; \
        length_of_lines[partition] = linelen; \
        } \
    return errorsum; \
    }

THREE_COMPONENT_ERROR_FUNC(compute_error_squared_gba, 1, 2, 3, yzw, gba)
THREE_COMPONENT_ERROR_FUNC(compute_error_squared_rba, 0, 2, 3, xzw, rba)
THREE_COMPONENT_ERROR_FUNC(compute_error_squared_rga, 0, 1, 3, xyw, rga)
THREE_COMPONENT_ERROR_FUNC(compute_error_squared_rgb, 0, 1, 2, xyz, rgb)


void compute_rgb_minmax(
    __global partition_info * pt,
    imageblock * blk,
    error_weight_block * ewb,
    float *red_min, float *red_max,
    float *green_min, float *green_max,
    float *blue_min, float *blue_max,
    __global ASTC_Encode *ASTCEncode) {
    DEBUG("compute_rgb_minmax");
    int i;
    int partition_count = pt->partition_count;

    for (i = 0; i < partition_count; i++) {
        red_min[i] = FLOAT_38;
        red_max[i] = -FLOAT_38;
        green_min[i] = FLOAT_38;
        green_max[i] = -FLOAT_38;
        blue_min[i] = FLOAT_38;
        blue_max[i] = -FLOAT_38;
    }

    for (i = 0; i < ASTCEncode->m_texels_per_block; i++) {
        if (ewb->texel_weight[i] > FLOAT_n10) {
            int partition = pt->partition_of_texel[i];
            float redval = blk->work_data[4 * i];
            float greenval = blk->work_data[4 * i + 1];
            float blueval = blk->work_data[4 * i + 2];
            if (redval > red_max[partition])
                red_max[partition] = redval;
            if (redval < red_min[partition])
                red_min[partition] = redval;
            if (greenval > green_max[partition])
                green_max[partition] = greenval;
            if (greenval < green_min[partition])
                green_min[partition] = greenval;
            if (blueval > blue_max[partition])
                blue_max[partition] = blueval;
            if (blueval < blue_min[partition])
                blue_min[partition] = blueval;
        }
    }
    for (i = 0; i < partition_count; i++) {
        if (red_min[i] >= red_max[i]) {
            red_min[i] = 0.0f;
            red_max[i] = FLOAT_n10;
        }
        if (green_min[i] >= green_max[i]) {
            green_min[i] = 0.0f;
            green_max[i] = FLOAT_n10;
        }
        if (blue_min[i] >= blue_max[i]) {
            blue_min[i] = 0.0f;
            blue_max[i] = FLOAT_n10;
        }
    }
}

void find_best_partitionings(int partition_search_limit, int partition_count,
                             imageblock * pb, error_weight_block * ewb, int candidates_to_return,
                             int *best_partitions_uncorrellated,     // best partitionings to use if the endpoint colors are assumed to be uncorrellated
                             int *best_partitions_samechroma,        // best partitionings to use if the endpoint colors have the same chroma
                             int *best_partitions_dual_weight_planes,// best partitionings to use if using dual plane of weightss
                             __global ASTC_Encode *ASTCEncode) {
//#
    DEBUG("find_best_partitionings");
    int i, j;

    // constant used to estimate quantization error for a given partitioning;
    // the optimal value for this constant depends on bitrate.
    // These constants have been determined empirically.

    float weight_imprecision_estim = 100;
    if (ASTCEncode->m_texels_per_block <= 20)
        weight_imprecision_estim = 0.03f;
    else if (ASTCEncode->m_texels_per_block <= 31)
        weight_imprecision_estim = 0.04f;
    else if (ASTCEncode->m_texels_per_block <= 41)
        weight_imprecision_estim = 0.05f;
    else
        weight_imprecision_estim = 0.055f;

    int partition_sequence[PARTITION_COUNT];

    kmeans_compute_partition_ordering(partition_count, pb, partition_sequence, ASTCEncode);

    float weight_imprecision_estim_squared = weight_imprecision_estim * weight_imprecision_estim;

    int uses_alpha = imageblock_uses_alpha1(pb);

    // partitioning errors assuming uncorrellated-chrominance endpoints
    float uncorr_errors[PARTITION_COUNT];
    // partitioning errors assuming same-chrominance endpoints
    float samechroma_errors[PARTITION_COUNT];

    // partitioning errors assuming that one of the color channels
    // is uncorrellated from all the other ones
    float separate_errors[4 * PARTITION_COUNT];
    float *separate_red_errors = separate_errors;
    float *separate_green_errors = separate_errors + PARTITION_COUNT;
    float *separate_blue_errors = separate_errors + 2 * PARTITION_COUNT;
    float *separate_alpha_errors = separate_errors + 3 * PARTITION_COUNT;

    int defacto_search_limit = PARTITION_COUNT - 1;

    if (uses_alpha) {

        for (i = 0; i < PARTITION_COUNT; i++) {
            int partition = partition_sequence[i];
            int bk_partition_count = ASTCEncode->partition_tables[partition_count][partition].partition_count;

            if (bk_partition_count < partition_count) {
                uncorr_errors[i] = FLOAT_35;
                samechroma_errors[i] = FLOAT_35;
                separate_red_errors[i] = FLOAT_35;
                separate_green_errors[i] = FLOAT_35;
                separate_blue_errors[i] = FLOAT_35;
                separate_alpha_errors[i] = FLOAT_35;
                continue;
            }
            // the sentinel value for partitions above the search limit must be smaller
            // than the sentinel value for invalid partitions
            if (i >= partition_search_limit) {
                defacto_search_limit = i;

                uncorr_errors[i] = FLOAT_34;
                samechroma_errors[i] = FLOAT_34;
                separate_red_errors[i] = FLOAT_34;
                separate_green_errors[i] = FLOAT_34;
                separate_blue_errors[i] = FLOAT_34;
                separate_alpha_errors[i] = FLOAT_34;
                break;
            }

            // compute the weighting to give to each color channel
            // in each partition.
            float4 error_weightings[4];
            float4 color_scalefactors[4];
            float4 inverse_color_scalefactors[4];
            compute_partition_error_color_weightings(ewb,
                    &ASTCEncode->partition_tables[partition_count][partition],
                    error_weightings, color_scalefactors, ASTCEncode);

            for (j = 0; j < partition_count; j++) {
                inverse_color_scalefactors[j].x = 1.0f / (std::max)(color_scalefactors[j].x, FLOAT_n7);
                inverse_color_scalefactors[j].y = 1.0f / (std::max)(color_scalefactors[j].y, FLOAT_n7);
                inverse_color_scalefactors[j].z = 1.0f / (std::max)(color_scalefactors[j].z, FLOAT_n7);
                inverse_color_scalefactors[j].w = 1.0f / (std::max)(color_scalefactors[j].w, FLOAT_n7);
            }

            float4 averages[4];
            float4 directions_rgba[4];
            float3 directions_gba[4];
            float3 directions_rba[4];
            float3 directions_rga[4];
            float3 directions_rgb[4];

            compute_averages_and_directions_rgba(
                &ASTCEncode->partition_tables[partition_count][partition],
                pb, ewb, color_scalefactors, averages, directions_rgba, directions_gba, directions_rba, directions_rga, directions_rgb);

            line4 uncorr_lines[4];
            line4 samechroma_lines[4];
            line3 separate_red_lines[4];
            line3 separate_green_lines[4];
            line3 separate_blue_lines[4];
            line3 separate_alpha_lines[4];

            processed_line4 proc_uncorr_lines[4];
            processed_line4 proc_samechroma_lines[4];
            processed_line3 proc_separate_red_lines[4];
            processed_line3 proc_separate_green_lines[4];
            processed_line3 proc_separate_blue_lines[4];
            processed_line3 proc_separate_alpha_lines[4];

            float uncorr_linelengths[4];
            float samechroma_linelengths[4];
            float separate_red_linelengths[4];
            float separate_green_linelengths[4];
            float separate_blue_linelengths[4];
            float separate_alpha_linelengths[4];

            float3 one3f  = { 1.0f,1.0f,1.0f};
            float4 zero4f = { 0.0f,0.0f,0.0f,0.0f };
            float4 one4f  = { 1.0f,1.0f,1.0f,1.0f };

            for (j = 0; j < partition_count; j++) {
                uncorr_lines[j].a = averages[j];
                if (dot(directions_rgba[j], directions_rgba[j]) == 0.0f) {
                    uncorr_lines[j].b = normalize(one4f);
                } else
                    uncorr_lines[j].b = normalize(directions_rgba[j]);

                proc_uncorr_lines[j].amod = (uncorr_lines[j].a - uncorr_lines[j].b * dot(uncorr_lines[j].a, uncorr_lines[j].b)) * inverse_color_scalefactors[j];
                proc_uncorr_lines[j].bs = (uncorr_lines[j].b * color_scalefactors[j]);
                proc_uncorr_lines[j].bis = (uncorr_lines[j].b * inverse_color_scalefactors[j]);


                samechroma_lines[j].a = zero4f;
                if (dot(averages[j], averages[j]) == 0) {
                    samechroma_lines[j].b = normalize(one4f);
                } else
                    samechroma_lines[j].b = normalize(averages[j]);

                proc_samechroma_lines[j].amod = (samechroma_lines[j].a - samechroma_lines[j].b * dot(samechroma_lines[j].a, samechroma_lines[j].b)) * inverse_color_scalefactors[j];
                proc_samechroma_lines[j].bs = (samechroma_lines[j].b * color_scalefactors[j]);
                proc_samechroma_lines[j].bis = (samechroma_lines[j].b * inverse_color_scalefactors[j]);

                separate_red_lines[j].a = averages[j].yzw;
                if (dot(directions_gba[j], directions_gba[j]) == 0.0f)
                    separate_red_lines[j].b = normalize(one3f);
                else
                    separate_red_lines[j].b = normalize(directions_gba[j]);

                separate_green_lines[j].a = averages[j].xzw;
                if (dot(directions_rba[j], directions_rba[j]) == 0.0f)
                    separate_green_lines[j].b = normalize(one3f);
                else
                    separate_green_lines[j].b = normalize(directions_rba[j]);

                separate_blue_lines[j].a = averages[j].xyw;
                if (dot(directions_rga[j], directions_rga[j]) == 0.0f)
                    separate_blue_lines[j].b = normalize(one3f);
                else
                    separate_blue_lines[j].b = normalize(directions_rga[j]);

                separate_alpha_lines[j].a = averages[j].xyz;
                if (dot(directions_rgb[j], directions_rgb[j]) == 0.0f) {
                    separate_alpha_lines[j].b = normalize(one3f);
                } else
                    separate_alpha_lines[j].b = normalize(directions_rgb[j]);

                proc_separate_red_lines[j].amod = (separate_red_lines[j].a - separate_red_lines[j].b * dot(separate_red_lines[j].a, separate_red_lines[j].b)) * inverse_color_scalefactors[j].yzw;
                proc_separate_red_lines[j].bs = (separate_red_lines[j].b * color_scalefactors[j].yzw);
                proc_separate_red_lines[j].bis = (separate_red_lines[j].b * inverse_color_scalefactors[j].yzw);

                proc_separate_green_lines[j].amod =
                    (separate_green_lines[j].a - separate_green_lines[j].b * dot(separate_green_lines[j].a, separate_green_lines[j].b)) * inverse_color_scalefactors[j].xzw;
                proc_separate_green_lines[j].bs = (separate_green_lines[j].b * color_scalefactors[j].xzw);
                proc_separate_green_lines[j].bis = (separate_green_lines[j].b * inverse_color_scalefactors[j].xzw);

                proc_separate_blue_lines[j].amod = (separate_blue_lines[j].a - separate_blue_lines[j].b * dot(separate_blue_lines[j].a, separate_blue_lines[j].b)) * inverse_color_scalefactors[j].xyw;
                proc_separate_blue_lines[j].bs = (separate_blue_lines[j].b * color_scalefactors[j].xyw);
                proc_separate_blue_lines[j].bis = (separate_blue_lines[j].b * inverse_color_scalefactors[j].xyw);

                proc_separate_alpha_lines[j].amod =
                    (separate_alpha_lines[j].a - separate_alpha_lines[j].b * dot(separate_alpha_lines[j].a, separate_alpha_lines[j].b)) * inverse_color_scalefactors[j].xyz;
                proc_separate_alpha_lines[j].bs = (separate_alpha_lines[j].b * color_scalefactors[j].xyz);
                proc_separate_alpha_lines[j].bis = (separate_alpha_lines[j].b * inverse_color_scalefactors[j].xyz);

            }

            float uncorr_error = compute_error_squared_rgba(
                                     &ASTCEncode->partition_tables[partition_count][partition],
                                     pb,
                                     ewb,
                                     proc_uncorr_lines,
                                     uncorr_linelengths);
            float samechroma_error = compute_error_squared_rgba(
                                         &ASTCEncode->partition_tables[partition_count][partition],
                                         pb,
                                         ewb,
                                         proc_samechroma_lines,
                                         samechroma_linelengths);

            float separate_red_error  = compute_error_squared_gba(
                                            &ASTCEncode->partition_tables[partition_count][partition],
                                            pb,
                                            ewb,
                                            proc_separate_red_lines,
                                            separate_red_linelengths);

            float separate_green_error = compute_error_squared_rba(
                                             &ASTCEncode->partition_tables[partition_count][partition],
                                             pb,
                                             ewb,
                                             proc_separate_green_lines,
                                             separate_green_linelengths);

            float separate_blue_error = compute_error_squared_rga(
                                            &ASTCEncode->partition_tables[partition_count][partition],
                                            pb,
                                            ewb,
                                            proc_separate_blue_lines,
                                            separate_blue_linelengths);

            float separate_alpha_error = compute_error_squared_rgb(
                                             &ASTCEncode->partition_tables[partition_count][partition],
                                             pb,
                                             ewb,
                                             proc_separate_alpha_lines,
                                             separate_alpha_linelengths);

            // compute minimum & maximum alpha values in each partition
            float red_min[4], red_max[4];
            float green_min[4], green_max[4];
            float blue_min[4], blue_max[4];
            float alpha_min[4], alpha_max[4];
            compute_alpha_minmax(
                &ASTCEncode->partition_tables[partition_count][partition],
                pb, ewb, alpha_min, alpha_max, ASTCEncode);

            compute_rgb_minmax(
                &ASTCEncode->partition_tables[partition_count][partition],
                pb, ewb, red_min, red_max, green_min, green_max, blue_min, blue_max, ASTCEncode);

            /*
               Compute an estimate of error introduced by weight quantization imprecision.
               This error is computed as follows, for each partition
               1: compute the principal-axis vector (full length) in error-space
               2: convert the principal-axis vector to regular RGB-space
               3: scale the vector by a constant that estimates average quantization error
               4: for each texel, square the vector, then do a dot-product with the texel's error weight;
                  sum up the results across all texels.
               4(optimized): square the vector once, then do a dot-product with the average texel error,
                  then multiply by the number of texels.
             */

            for (j = 0; j < partition_count; j++) {
                float tpp = (float)(ASTCEncode->partition_tables[partition_count][partition].texels_per_partition[j]);

                float4 ics = inverse_color_scalefactors[j];
                float4 error_weights = error_weightings[j] * (tpp * weight_imprecision_estim_squared);

                float4 uncorr_vector = (uncorr_lines[j].b * uncorr_linelengths[j]) * ics;
                float4 samechroma_vector = (samechroma_lines[j].b * samechroma_linelengths[j]) * ics;
                float3 separate_red_vector = (separate_red_lines[j].b * separate_red_linelengths[j]) * ics.yzw;
                float3 separate_green_vector = (separate_green_lines[j].b * separate_green_linelengths[j]) * ics.xzw;
                float3 separate_blue_vector = (separate_blue_lines[j].b * separate_blue_linelengths[j]) * ics.xyw;
                float3 separate_alpha_vector = (separate_alpha_lines[j].b * separate_alpha_linelengths[j]) * ics.xyz;

                uncorr_vector = uncorr_vector * uncorr_vector;
                samechroma_vector = samechroma_vector * samechroma_vector;
                separate_red_vector = separate_red_vector * separate_red_vector;
                separate_green_vector = separate_green_vector * separate_green_vector;
                separate_blue_vector = separate_blue_vector * separate_blue_vector;
                separate_alpha_vector = separate_alpha_vector * separate_alpha_vector;

                uncorr_error += dot(uncorr_vector, error_weights);
                samechroma_error += dot(samechroma_vector, error_weights);
                separate_red_error += dot(separate_red_vector, error_weights.yzw);
                separate_green_error += dot(separate_green_vector, error_weights.xzw);
                separate_blue_error += dot(separate_blue_vector, error_weights.xyw);
                separate_alpha_error += dot(separate_alpha_vector, error_weights.xyz);

                float red_scalar = (red_max[j] - red_min[j]);
                float green_scalar = (green_max[j] - green_min[j]);
                float blue_scalar = (blue_max[j] - blue_min[j]);
                float alpha_scalar = (alpha_max[j] - alpha_min[j]);
                red_scalar *= red_scalar;
                green_scalar *= green_scalar;
                blue_scalar *= blue_scalar;
                alpha_scalar *= alpha_scalar;
                separate_red_error += red_scalar * error_weights.x;
                separate_green_error += green_scalar * error_weights.y;
                separate_blue_error += blue_scalar * error_weights.z;
                separate_alpha_error += alpha_scalar * error_weights.w;
            }

            uncorr_errors[i] = uncorr_error;
            samechroma_errors[i] = samechroma_error;
            separate_red_errors[i] = separate_red_error;
            separate_green_errors[i] = separate_green_error;
            separate_blue_errors[i] = separate_blue_error;
            separate_alpha_errors[i] = separate_alpha_error;

        }
    } else {

        for (i = 0; i < PARTITION_COUNT; i++) {

            int partition = partition_sequence[i];

            int bk_partition_count = ASTCEncode->partition_tables[partition_count][partition].partition_count;
            if (bk_partition_count < partition_count) {

                uncorr_errors[i] = FLOAT_35;
                samechroma_errors[i] = FLOAT_35;
                separate_red_errors[i] = FLOAT_35;
                separate_green_errors[i] = FLOAT_35;
                separate_blue_errors[i] = FLOAT_35;
                continue;
            }

            // the sentinel value for valid partitions above the search limit must be smaller
            // than the sentinel value for invalid partitions
            if (i >= partition_search_limit) {

                defacto_search_limit = i;
                uncorr_errors[i] = FLOAT_34;
                samechroma_errors[i] = FLOAT_34;
                separate_red_errors[i] = FLOAT_34;
                separate_green_errors[i] = FLOAT_34;
                separate_blue_errors[i] = FLOAT_34;
                break;

            }

            // compute the weighting to give to each color channel
            // in each partition.
            float4 error_weightings[4];
            float4 color_scalefactors[4];
            float4 inverse_color_scalefactors[4];

            compute_partition_error_color_weightings( ewb,
                    &ASTCEncode->partition_tables[partition_count][partition],
                    error_weightings, color_scalefactors, ASTCEncode);

            for (j = 0; j < partition_count; j++) {
                inverse_color_scalefactors[j].x = 1.0f / (std::max)(color_scalefactors[j].x, FLOAT_n7);
                inverse_color_scalefactors[j].y = 1.0f / (std::max)(color_scalefactors[j].y, FLOAT_n7);
                inverse_color_scalefactors[j].z = 1.0f / (std::max)(color_scalefactors[j].z, FLOAT_n7);
                inverse_color_scalefactors[j].w = 1.0f / (std::max)(color_scalefactors[j].w, FLOAT_n7);
            }

            float3 averages[4];
            float3 directions_rgb[4];
            float2 directions_rg[4];
            float2 directions_rb[4];
            float2 directions_gb[4];

            compute_averages_and_directions_rgb(
                &ASTCEncode->partition_tables[partition_count][partition],
                pb, ewb, color_scalefactors, averages, directions_rgb, directions_rg, directions_rb, directions_gb);

            line3 uncorr_lines[4];
            line3 samechroma_lines[4];
            line2 separate_red_lines[4];
            line2 separate_green_lines[4];
            line2 separate_blue_lines[4];

            processed_line3 proc_uncorr_lines[4];
            processed_line3 proc_samechroma_lines[4];

            processed_line2 proc_separate_red_lines[4];
            processed_line2 proc_separate_green_lines[4];
            processed_line2 proc_separate_blue_lines[4];

            float uncorr_linelengths[4];
            float samechroma_linelengths[4];
            float separate_red_linelengths[4];
            float separate_green_linelengths[4];
            float separate_blue_linelengths[4];

            float2 one2f  = { 1.0f,1.0f };
            float3 one3f  = { 1.0f,1.0f,1.0f };
            float3 zero3f = { 0.0f,0.0f,0.0f };

            for (j = 0; j < partition_count; j++) {
                uncorr_lines[j].a = averages[j];
                if (dot(directions_rgb[j], directions_rgb[j]) == 0.0f)
                    uncorr_lines[j].b = normalize(one3f);
                else
                    uncorr_lines[j].b = normalize(directions_rgb[j]);


                samechroma_lines[j].a = zero3f;

                if (dot(averages[j], averages[j]) == 0.0f)
                    samechroma_lines[j].b = normalize(one3f);
                else
                    samechroma_lines[j].b = normalize(averages[j]);

                proc_uncorr_lines[j].amod = (uncorr_lines[j].a - uncorr_lines[j].b * dot(uncorr_lines[j].a, uncorr_lines[j].b)) * inverse_color_scalefactors[j].xyz;
                proc_uncorr_lines[j].bs = (uncorr_lines[j].b * color_scalefactors[j].xyz);
                proc_uncorr_lines[j].bis = (uncorr_lines[j].b * inverse_color_scalefactors[j].xyz);

                proc_samechroma_lines[j].amod = (samechroma_lines[j].a - samechroma_lines[j].b * dot(samechroma_lines[j].a, samechroma_lines[j].b)) * inverse_color_scalefactors[j].xyz;
                proc_samechroma_lines[j].bs = (samechroma_lines[j].b * color_scalefactors[j].xyz);
                proc_samechroma_lines[j].bis = (samechroma_lines[j].b * inverse_color_scalefactors[j].xyz);

                separate_red_lines[j].a = averages[j].yz;
                if (dot(directions_gb[j], directions_gb[j]) == 0.0f)
                    separate_red_lines[j].b = normalize(one2f);
                else
                    separate_red_lines[j].b = normalize(directions_gb[j]);

                separate_green_lines[j].a = averages[j].xz;
                if (dot(directions_rb[j], directions_rb[j]) == 0.0f)
                    separate_green_lines[j].b = normalize(one2f);
                else
                    separate_green_lines[j].b = normalize(directions_rb[j]);

                separate_blue_lines[j].a = averages[j].xy;
                if (dot(directions_rg[j], directions_rg[j]) == 0.0f)
                    separate_blue_lines[j].b = normalize(one2f);
                else
                    separate_blue_lines[j].b = normalize(directions_rg[j]);

                proc_separate_red_lines[j].amod = (separate_red_lines[j].a - separate_red_lines[j].b * dot(separate_red_lines[j].a, separate_red_lines[j].b)) * inverse_color_scalefactors[j].yz;
                proc_separate_red_lines[j].bs = (separate_red_lines[j].b * color_scalefactors[j].yz);
                proc_separate_red_lines[j].bis = (separate_red_lines[j].b * inverse_color_scalefactors[j].yz);

                proc_separate_green_lines[j].amod =
                    (separate_green_lines[j].a - separate_green_lines[j].b * dot(separate_green_lines[j].a, separate_green_lines[j].b)) * inverse_color_scalefactors[j].xz;
                proc_separate_green_lines[j].bs = (separate_green_lines[j].b * color_scalefactors[j].xz);
                proc_separate_green_lines[j].bis = (separate_green_lines[j].b * inverse_color_scalefactors[j].xz);

                proc_separate_blue_lines[j].amod = (separate_blue_lines[j].a - separate_blue_lines[j].b * dot(separate_blue_lines[j].a, separate_blue_lines[j].b)) * inverse_color_scalefactors[j].xy;
                proc_separate_blue_lines[j].bs = (separate_blue_lines[j].b * color_scalefactors[j].xy);
                proc_separate_blue_lines[j].bis = (separate_blue_lines[j].b * inverse_color_scalefactors[j].xy);

            }

            float uncorr_error = compute_error_squared_rgb(
                                     &ASTCEncode->partition_tables[partition_count][partition],
                                     pb,
                                     ewb,
                                     proc_uncorr_lines,
                                     uncorr_linelengths);
            float samechroma_error = compute_error_squared_rgb(
                                         &ASTCEncode->partition_tables[partition_count][partition],
                                         pb,
                                         ewb,
                                         proc_samechroma_lines,
                                         samechroma_linelengths);

            float separate_red_error = compute_error_squared_gb(
                                           &ASTCEncode->partition_tables[partition_count][partition],
                                           pb,
                                           ewb,
                                           proc_separate_red_lines,
                                           separate_red_linelengths);

            float separate_green_error = compute_error_squared_rb(
                                             &ASTCEncode->partition_tables[partition_count][partition],
                                             pb,
                                             ewb,
                                             proc_separate_green_lines,
                                             separate_green_linelengths);

            float separate_blue_error = compute_error_squared_rg(
                                            &ASTCEncode->partition_tables[partition_count][partition],
                                            pb,
                                            ewb,
                                            proc_separate_blue_lines,
                                            separate_blue_linelengths);

            float red_min[4], red_max[4];
            float green_min[4], green_max[4];
            float blue_min[4], blue_max[4];

            compute_rgb_minmax(
                &ASTCEncode->partition_tables[partition_count][partition],
                pb, ewb, red_min, red_max, green_min, green_max, blue_min, blue_max, ASTCEncode);

            /*
               compute an estimate of error introduced by weight imprecision.
               This error is computed as follows, for each partition
               1: compute the principal-axis vector (full length) in error-space
               2: convert the principal-axis vector to regular RGB-space
               3: scale the vector by a constant that estimates average quantization error.
               4: for each texel, square the vector, then do a dot-product with the texel's error weight;
                  sum up the results across all texels.
               4(optimized): square the vector once, then do a dot-product with the average texel error,
                 then multiply by the number of texels.
             */

            for (j = 0; j < partition_count; j++) {
                float tpp = (float)(ASTCEncode->partition_tables[partition_count][partition].texels_per_partition[j]);

                float3 ics = inverse_color_scalefactors[j].xyz;
                float3 error_weights = error_weightings[j].xyz * (tpp * weight_imprecision_estim_squared);

                float3 uncorr_vector = (uncorr_lines[j].b * uncorr_linelengths[j]) * ics;
                float3 samechroma_vector = (samechroma_lines[j].b * samechroma_linelengths[j]) * ics;

                float2 separate_red_vector = (separate_red_lines[j].b * separate_red_linelengths[j]) * ics.yz;
                float2 separate_green_vector = (separate_green_lines[j].b * separate_green_linelengths[j]) * ics.xz;
                float2 separate_blue_vector = (separate_blue_lines[j].b * separate_blue_linelengths[j]) * ics.xy;

                uncorr_vector = uncorr_vector * uncorr_vector;
                samechroma_vector = samechroma_vector * samechroma_vector;
                separate_red_vector = separate_red_vector * separate_red_vector;
                separate_green_vector = separate_green_vector * separate_green_vector;
                separate_blue_vector = separate_blue_vector * separate_blue_vector;

                uncorr_error += dot(uncorr_vector, error_weights);
                samechroma_error += dot(samechroma_vector, error_weights);
                separate_red_error += dot(separate_red_vector, error_weights.yz);
                separate_green_error += dot(separate_green_vector, error_weights.xz);
                separate_blue_error += dot(separate_blue_vector, error_weights.xy);

                float red_scalar = (red_max[j] - red_min[j]);
                float green_scalar = (green_max[j] - green_min[j]);
                float blue_scalar = (blue_max[j] - blue_min[j]);

                red_scalar *= red_scalar;
                green_scalar *= green_scalar;
                blue_scalar *= blue_scalar;

                separate_red_error += red_scalar * error_weights.x;
                separate_green_error += green_scalar * error_weights.y;
                separate_blue_error += blue_scalar * error_weights.z;
            }

            uncorr_errors[i] = uncorr_error;
            samechroma_errors[i] = samechroma_error;

            separate_red_errors[i] = separate_red_error;
            separate_green_errors[i] = separate_green_error;
            separate_blue_errors[i] = separate_blue_error;
        }
    }

    for (i = 0; i < candidates_to_return; i++) {
        int best_uncorr_partition = 0;
        int best_samechroma_partition = 0;
        float best_uncorr_error = FLOAT_30;
        float best_samechroma_error = FLOAT_30;
        for (j = 0; j <= defacto_search_limit; j++) {
            if (uncorr_errors[j] < best_uncorr_error) {
                best_uncorr_partition = j;
                best_uncorr_error = uncorr_errors[j];
            }
        }
        best_partitions_uncorrellated[i] = partition_sequence[best_uncorr_partition];
        uncorr_errors[best_uncorr_partition] = FLOAT_30;
        samechroma_errors[best_uncorr_partition] = FLOAT_30;

        for (j = 0; j <= defacto_search_limit; j++) {
            if (samechroma_errors[j] < best_samechroma_error) {
                best_samechroma_partition = j;
                best_samechroma_error = samechroma_errors[j];
            }
        }
        best_partitions_samechroma[i] = partition_sequence[best_samechroma_partition];
        samechroma_errors[best_samechroma_partition] = FLOAT_30;
        uncorr_errors[best_samechroma_partition] = FLOAT_30;
    }

    for (i = 0; i < 2 * candidates_to_return; i++) {
        int best_partition = 0;
        float best_partition_error = FLOAT_30;

        for (j = 0; j < defacto_search_limit; j++) {

            if (separate_errors[j] < best_partition_error) {
                best_partition = j;
                best_partition_error = separate_errors[j];
            }
            if (separate_errors[j + PARTITION_COUNT] < best_partition_error) {
                best_partition = j + PARTITION_COUNT;
                best_partition_error = separate_errors[j + PARTITION_COUNT];
            }
            if (separate_errors[j + 2 * PARTITION_COUNT] < best_partition_error) {
                best_partition = j + 2 * PARTITION_COUNT;
                best_partition_error = separate_errors[j + 2 * PARTITION_COUNT];
            }

            if (uses_alpha) {
                if (separate_errors[j + 3 * PARTITION_COUNT] < best_partition_error) {
                    best_partition = j + 3 * PARTITION_COUNT;
                    best_partition_error = separate_errors[j + 3 * PARTITION_COUNT];
                }
            }
        }

        separate_errors[best_partition] = FLOAT_30;
        best_partition = ((best_partition >> PARTITION_BITS) << PARTITION_BITS) | partition_sequence[best_partition & (PARTITION_COUNT - 1)];
        best_partitions_dual_weight_planes[i] = best_partition;
    }

}

float compress_symbolic_block(
    imageblock * blk,
    symbolic_compressed_block * scb,
    __global ASTC_Encode *  ASTCEncode
) {
    DEBUG("compress_symbolic_block");
    int i, j;
    imageblock temp;
    error_weight_block ewb;
    error_weight_block_orig ewbo;
    symbolic_compressed_block tempblocks[4];

    endpoints_and_weights ei1;
    endpoints_and_weights ei2;
    endpoints_and_weights eix1[MAX_DECIMATION_MODES];
    endpoints_and_weights eix2[MAX_DECIMATION_MODES];

    __global2 float   *decimated_weights                           = ASTCEncode->decimated_weights;
    __global2 uint8_t *u8_quantized_decimated_quantized_weights    = ASTCEncode->u8_quantized_decimated_quantized_weights;
    __global2 float   *decimated_quantized_weights                 = ASTCEncode->decimated_quantized_weights;
    __global2 float   *flt_quantized_decimated_quantized_weights   = ASTCEncode->flt_quantized_decimated_quantized_weights;

    if (blk->red_min == blk->red_max && blk->green_min == blk->green_max && blk->blue_min == blk->blue_max && blk->alpha_min == blk->alpha_max) {
        // detected a constant-color block. Encode as FP16 if using HDR
        scb->error_block = 0;

        if (ASTCEncode->m_rgb_force_use_of_hdr) {
            scb->block_mode = -1;
            scb->partition_count = 0;
            scb->constant_color[0] = float_to_sf16(blk->orig_data[0], SF_NEARESTEVEN);
            scb->constant_color[1] = float_to_sf16(blk->orig_data[1], SF_NEARESTEVEN);
            scb->constant_color[2] = float_to_sf16(blk->orig_data[2], SF_NEARESTEVEN);
            scb->constant_color[3] = float_to_sf16(blk->orig_data[3], SF_NEARESTEVEN);
        } else {
            // Encode as UNORM16 if NOT using HDR.
            scb->block_mode = -2;
            scb->partition_count = 0;
            float red = blk->orig_data[0];
            float green = blk->orig_data[1];
            float blue = blk->orig_data[2];
            float alpha = blk->orig_data[3];
            if (red < 0)
                red = 0;
            else if (red > 1)
                red = 1;
            if (green < 0)
                green = 0;
            else if (green > 1)
                green = 1;
            if (blue < 0)
                blue = 0;
            else if (blue > 1)
                blue = 1;
            if (alpha < 0)
                alpha = 0;
            else if (alpha > 1)
                alpha = 1;
            scb->constant_color[0] = (int)floor(red     * 65535.0f + 0.5f);
            scb->constant_color[1] = (int)floor(green   * 65535.0f + 0.5f);
            scb->constant_color[2] = (int)floor(blue    * 65535.0f + 0.5f);
            scb->constant_color[3] = (int)floor(alpha   * 65535.0f + 0.5f);
        }
        return 0.0f;
    }


    float error_weight_sum = prepare_error_weight_block(blk, &ewb, &ewbo,ASTCEncode);

//#ifdef __OPENCL_VERSION__
//      if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//          printf("error_weight_sum %3.3f\n", error_weight_sum);


    float error_of_best_block = FLOAT_20;

    float best_errorvals_in_modes[17];
    for (i = 0; i < 17; i++)
        best_errorvals_in_modes[i] = FLOAT_30;

    int uses_alpha = imageblock_uses_alpha1(blk);
    float mode_cutoff = ASTCEncode->m_ewp.block_mode_cutoff;

    // next, test mode #0. This mode uses 1 plane of weights and 1 partition.
    // we test it twice, first with a modecutoff of 0, then with the specified mode-cutoff.
    // This causes an early-out that speeds up encoding of "easy" content.

    float modecutoffs[2];
    float errorval_mult[2] = { 2.5, 1 };
    modecutoffs[0] = 0;
    modecutoffs[1] = mode_cutoff;

    // compute ideal weights and endpoint colors for every decimation.

    float best_errorval_in_mode;
    for (i = 0; i < 2; i++) {
        compress_symbolic_block_fixed_partition_1_plane(
            modecutoffs[i],
            ASTCEncode->m_ewp.max_refinement_iters,
            1,    // partition count
            0,    // partition indexdone
            blk,
            &ewb,
            tempblocks,
            &ei1,
            eix1,
            decimated_weights,
            u8_quantized_decimated_quantized_weights,
            decimated_quantized_weights,
            flt_quantized_decimated_quantized_weights,
            ASTCEncode
        );

//#ifdef __OPENCL_VERSION__
//        if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//        printf("1_plane : ei1.ep.endpt0[0].x %3.3f\n", ei1.ep.endpt0[0].x);


        best_errorval_in_mode = FLOAT_30;
        for (j = 0; j < 4; j++) {
            if (tempblocks[j].error_block)
                continue;

            decompress_symbolic_block(tempblocks + j, &temp, ASTCEncode);
            float errorval = compute_imageblock_difference(blk, &temp, &ewb, ASTCEncode) * errorval_mult[i];
            if (errorval < best_errorval_in_mode)
                best_errorval_in_mode = errorval;

            if (errorval < error_of_best_block) {
                error_of_best_block = errorval;
                *scb = tempblocks[j];
            }

        }


        best_errorvals_in_modes[0] = best_errorval_in_mode;
        if ((error_of_best_block / error_weight_sum) < ASTCEncode->m_ewp.texel_avg_error_limit) {
            // mean squared error per color component.
            return (error_of_best_block / ASTCEncode->m_texels_per_block);
        }
    }

    int is_normal_map;
    float lowest_correl;
    prepare_block_statistics(blk, &ewb, &is_normal_map, &lowest_correl, ASTCEncode);

    if (is_normal_map && lowest_correl < 0.99f)
        lowest_correl = 0.99f;

    // next, test the four possible 1-partition, 2-planes modes
    for (i = 0; i < 4; i++) {

        if (lowest_correl > ASTCEncode->m_ewp.lowest_correlation_cutoff)
            continue;

        if (blk->grayscale && i != 3)
            continue;

        if (!uses_alpha && i == 3)
            continue;

        compress_symbolic_block_fixed_partition_2_planes(  mode_cutoff,
                ASTCEncode->m_ewp.max_refinement_iters,
                1,    // partition count
                0,    // partition index
                i,    // the color component to test a separate plane of weights for.
                blk,
                &ewb,
                tempblocks,
                &ei1,
                &ei2,
                eix1,
                eix2,
                decimated_weights,
                u8_quantized_decimated_quantized_weights,
                decimated_quantized_weights,
                flt_quantized_decimated_quantized_weights,
                ASTCEncode
                                                        );

//#ifdef __OPENCL_VERSION__
//        if ((get_global_id(0) == 0) && (get_global_id(1) == 0))
//#endif
//            printf("2_plane : ei1.ep.endpt0[0].x %3.3f  ei2.ep.endpt0[0].x %3.3f\n", ei1.ep.endpt0[0].x, ei2.ep.endpt0[0].x);


        for (j = 0; j < 4; j++) {
            if (tempblocks[j].error_block)
                continue;
            decompress_symbolic_block(tempblocks + j, &temp, ASTCEncode);
            float errorval = compute_imageblock_difference(blk, &temp, &ewb, ASTCEncode);
            if (errorval < best_errorval_in_mode)
                best_errorval_in_mode = errorval;

            if (errorval < error_of_best_block) {
                error_of_best_block = errorval;
                *scb = tempblocks[j];
            }

            best_errorvals_in_modes[i + 1] = best_errorval_in_mode;
        }

        if ((error_of_best_block / error_weight_sum) < ASTCEncode->m_ewp.texel_avg_error_limit) {
            // mean squared error per color component.
            return (error_of_best_block / ASTCEncode->m_texels_per_block);
        }
    }


    // find best blocks for 2, 3 and 4 partitions
    int partition_count;
    int max_partitions = 2;

#ifdef ENABLE_3_PARTITION_CODE
    max_partitions++;
#endif

#ifdef ENABLE_4_PARTITION_CODE
    max_partitions++;
#endif

    for (partition_count = 2; partition_count <= max_partitions; partition_count++) {
        int partition_indices_1plane[2];
        int partition_indices_2planes[2];

        find_best_partitionings(ASTCEncode->m_ewp.partition_search_limit,
                                partition_count, blk, &ewb, 1,
                                &(partition_indices_1plane[0]), &(partition_indices_1plane[1]), &(partition_indices_2planes[0]),ASTCEncode);

        for (i = 0; i < 2; i++) {
            compress_symbolic_block_fixed_partition_1_plane(
                mode_cutoff,
                ASTCEncode->m_ewp.max_refinement_iters,
                partition_count,
                partition_indices_1plane[i],
                blk,
                &ewb,
                tempblocks,
                &ei1,
                eix1,
                decimated_weights,
                u8_quantized_decimated_quantized_weights,
                decimated_quantized_weights,
                flt_quantized_decimated_quantized_weights,
                ASTCEncode
            );

            best_errorval_in_mode = FLOAT_30;
            for (j = 0; j < 4; j++) {
                if (tempblocks[j].error_block)
                    continue;
                decompress_symbolic_block(tempblocks + j, &temp, ASTCEncode);
                float errorval = compute_imageblock_difference(blk, &temp, &ewb, ASTCEncode);
                if (errorval < best_errorval_in_mode)
                    best_errorval_in_mode = errorval;

                if (errorval < error_of_best_block) {
                    error_of_best_block = errorval;
                    *scb = tempblocks[j];
                }
            }

            best_errorvals_in_modes[4 * (partition_count - 2) + 5] = best_errorval_in_mode;

            if ((error_of_best_block / error_weight_sum) < ASTCEncode->m_ewp.texel_avg_error_limit) {
                // mean squared error per color component.
                return (error_of_best_block / ASTCEncode->m_texels_per_block);
            }
        }


        if (partition_count == 2 && !is_normal_map && (std::min)(best_errorvals_in_modes[5], best_errorvals_in_modes[6]) > (best_errorvals_in_modes[0] * ASTCEncode->m_ewp.partition_1_to_2_limit)) {
            // mean squared error per color component.
            return (error_of_best_block / ASTCEncode->m_texels_per_block);
        }

        // don't bother to check 4 partitions for dual plane of weightss, ever.
        if (partition_count == 4)
            break;

        for (i = 0; i < 2; i++) {
            if (lowest_correl > ASTCEncode->m_ewp.lowest_correlation_cutoff)
                continue;


            compress_symbolic_block_fixed_partition_2_planes(
                mode_cutoff,
                ASTCEncode->m_ewp.max_refinement_iters,
                partition_count,
                (partition_indices_2planes[i] & (PARTITION_COUNT - 1)),
                (partition_indices_2planes[i] >> PARTITION_BITS),
                blk,
                &ewb,
                tempblocks,
                &ei1,
                &ei2,
                eix1,
                eix2,
                decimated_weights,
                u8_quantized_decimated_quantized_weights,
                decimated_quantized_weights,
                flt_quantized_decimated_quantized_weights,
                ASTCEncode
            );


            best_errorval_in_mode = FLOAT_30;
            for (j = 0; j < 4; j++) {
                if (tempblocks[j].error_block)
                    continue;
                decompress_symbolic_block(tempblocks + j, &temp, ASTCEncode);

                float errorval = compute_imageblock_difference(
                                     blk, &temp, &ewb, ASTCEncode);

                if (errorval < best_errorval_in_mode)
                    best_errorval_in_mode = errorval;

                if (errorval < error_of_best_block) {
                    error_of_best_block = errorval;
                    *scb = tempblocks[j];
                }
            }

            best_errorvals_in_modes[4 * (partition_count - 2) + 5 + 2] = best_errorval_in_mode;

            if ((error_of_best_block / error_weight_sum) < ASTCEncode->m_ewp.texel_avg_error_limit) {
                // mean squared error per color component.
                return (error_of_best_block / ASTCEncode->m_texels_per_block);
            }
        }
    }

    // mean squared error per color component.
    return (error_of_best_block / ASTCEncode->m_texels_per_block);
}


//===================== SYMBOLIC TO PHYSICAL START =============================

// routine to write up to 8 bits
//static inline
void write_bits(int value, int bitcount, int bitoffset, uint8_t * ptr) {
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

uint8_t bitrev8(uint8_t p) {
    p = ((p & 0xF) << 4) | ((p >> 4) & 0xF);
    p = ((p & 0x33) << 2) | ((p >> 2) & 0x33);
    p = ((p & 0x55) << 1) | ((p >> 1) & 0x55);
    return p;
}

void find_number_of_bits_trits_quints(int quantization_level, int *bits, int *trits, int *quints) {
    *bits = 0;
    *trits = 0;
    *quints = 0;
    switch (quantization_level) {
    case QUANT_2:
        *bits = 1;
        break;
    case QUANT_3:
        *bits = 0;
        *trits = 1;
        break;
    case QUANT_4:
        *bits = 2;
        break;
    case QUANT_5:
        *bits = 0;
        *quints = 1;
        break;
    case QUANT_6:
        *bits = 1;
        *trits = 1;
        break;
    case QUANT_8:
        *bits = 3;
        break;
    case QUANT_10:
        *bits = 1;
        *quints = 1;
        break;
    case QUANT_12:
        *bits = 2;
        *trits = 1;
        break;
    case QUANT_16:
        *bits = 4;
        break;
    case QUANT_20:
        *bits = 2;
        *quints = 1;
        break;
    case QUANT_24:
        *bits = 3;
        *trits = 1;
        break;
    case QUANT_32:
        *bits = 5;
        break;
    case QUANT_40:
        *bits = 3;
        *quints = 1;
        break;
    case QUANT_48:
        *bits = 4;
        *trits = 1;
        break;
    case QUANT_64:
        *bits = 6;
        break;
    case QUANT_80:
        *bits = 4;
        *quints = 1;
        break;
    case QUANT_96:
        *bits = 5;
        *trits = 1;
        break;
    case QUANT_128:
        *bits = 7;
        break;
    case QUANT_160:
        *bits = 5;
        *quints = 1;
        break;
    case QUANT_192:
        *bits = 6;
        *trits = 1;
        break;
    case QUANT_256:
        *bits = 8;
        break;
    }
}

void encode_ise(int quantization_level, int elements, uint8_t * input_data, uint8_t * output_data, int bit_offset) {
    int i;
    uint8_t lowparts[64];
    uint8_t highparts[69];        // 64 elements + 5 elements for padding
    uint8_t tq_blocks[22];        // trit-blocks or quint-blocks

    int bits, trits, quints;
    find_number_of_bits_trits_quints(quantization_level, &bits, &trits, &quints);

    for (i = 0; i < elements; i++) {
        lowparts[i] = input_data[i] & ((1 << bits) - 1);
        highparts[i] = input_data[i] >> bits;
    }
    for (i = elements; i < elements + 5; i++)
        highparts[i] = 0;        // padding before we start constructing trit-blocks or quint-blocks

    // construct trit-blocks or quint-blocks as necessary
    if (trits) {
        int trit_blocks = (elements + 4) / 5;
        for (i = 0; i < trit_blocks; i++)
            tq_blocks[i] = integer_of_trits[highparts[5 * i + 4]][highparts[5 * i + 3]][highparts[5 * i + 2]][highparts[5 * i + 1]][highparts[5 * i]];
    }
    if (quints) {
        int quint_blocks = (elements + 2) / 3;
        for (i = 0; i < quint_blocks; i++)
            tq_blocks[i] = integer_of_quints[highparts[3 * i + 2]][highparts[3 * i + 1]][highparts[3 * i]];
    }

    // then, write out the actual bits.
    int lcounter = 0;
    int hcounter = 0;
    for (i = 0; i < elements; i++) {
        write_bits(lowparts[i], bits, bit_offset, output_data);
        bit_offset += bits;
        if (trits) {

            write_bits(tq_blocks[hcounter] >> block_shift5[lcounter], bits_to_write5[lcounter], bit_offset, output_data);
            bit_offset += bits_to_write5[lcounter];
            hcounter += hcounter_incr5[lcounter];
            lcounter = next_lcounter5[lcounter];
        }
        if (quints) {

            write_bits(tq_blocks[hcounter] >> block_shift3[lcounter], bits_to_write3[lcounter], bit_offset, output_data);
            bit_offset += bits_to_write3[lcounter];
            hcounter += hcounter_incr3[lcounter];
            lcounter = next_lcounter3[lcounter];
        }
    }
}

physical_compressed_block symbolic_to_physical(symbolic_compressed_block * sc, __global ASTC_Encode *  ASTCEncode) {
    int i, j;
    physical_compressed_block res;

    if (sc->block_mode == -2) {
        // UNORM16 constant-color block.
        // This encodes separate constant-color blocks. There is currently
        // no attempt to coalesce them into larger void-extents.

        for (i = 0; i < 8; i++)
            res.data[i] = cbytes1[i];

        for (i = 0; i < 4; i++) {
            res.data[2 * i + 8] = sc->constant_color[i] & 0xFF;
            res.data[2 * i + 9] = (sc->constant_color[i] >> 8) & 0xFF;
        }
        return res;
    }

    if (sc->block_mode == -1) {
        // FP16 constant-color block.
        // This encodes separate constant-color blocks. There is currently
        // no attempt to coalesce them into larger void-extents.

        for (i = 0; i < 8; i++)
            res.data[i] = cbytes2[i];

        for (i = 0; i < 4; i++) {
            res.data[2 * i + 8] = sc->constant_color[i] & 0xFF;
            res.data[2 * i + 9] = (sc->constant_color[i] >> 8) & 0xFF;
        }
        return res;
    }

    int partition_count = sc->partition_count;

    // first, compress the weights. They are encoded as an ordinary
    // integer-sequence, then bit-reversed
    uint8_t weightbuf[16];
    for (i = 0; i < 16; i++)
        weightbuf[i] = 0;

    int weight_count = ASTCEncode->bsd.decimation_tables[ASTCEncode->bsd.block_modes[sc->block_mode].decimation_mode].num_weights;
    int weight_quantization_method = ASTCEncode->bsd.block_modes[sc->block_mode].quantization_mode;
    int is_dual_plane = ASTCEncode->bsd.block_modes[sc->block_mode].is_dual_plane;

    int real_weight_count = is_dual_plane ? 2 * weight_count : weight_count;

    int bits_for_weights = compute_ise_bitcount(real_weight_count,
                           (quantization_method)weight_quantization_method);


    if (is_dual_plane) {
        uint8_t weights[64];
        for (i = 0; i < weight_count; i++) {
            weights[2 * i] = sc->plane1_weights[i];
            weights[2 * i + 1] = sc->plane2_weights[i];
        }
        encode_ise(weight_quantization_method, real_weight_count, weights, weightbuf, 0);
    } else {
        encode_ise(weight_quantization_method, weight_count, sc->plane1_weights, weightbuf, 0);
    }

    for (i = 0; i < 16; i++)
        res.data[i] = bitrev8(weightbuf[15 - i]);

    write_bits(sc->block_mode, 11, 0, res.data);
    write_bits(partition_count - 1, 2, 11, res.data);

    int below_weights_pos = 128 - bits_for_weights;

    // encode partition index and color endpoint types for blocks with
    // 2 or more partitions.
    if (partition_count > 1) {
        write_bits(sc->partition_index, 6, 13, res.data);
        write_bits(sc->partition_index >> 6, PARTITION_BITS - 6, 19, res.data);

        if (sc->color_formats_matched) {
            write_bits(sc->color_formats[0] << 2, 6, 13 + PARTITION_BITS, res.data);
        } else {
            // go through the selected endpoint type classes for each partition
            // in order to determine the lowest class present.
            int low_class = 4;
            for (i = 0; i < partition_count; i++) {
                int class_of_format = sc->color_formats[i] >> 2;
                if (class_of_format < low_class)
                    low_class = class_of_format;
            }
            if (low_class == 3)
                low_class = 2;
            int encoded_type = low_class + 1;
            int bitpos = 2;
            for (i = 0; i < partition_count; i++) {
                int classbit_of_format = (sc->color_formats[i] >> 2) - low_class;

                encoded_type |= classbit_of_format << bitpos;
                bitpos++;
            }
            for (i = 0; i < partition_count; i++) {
                int lowbits_of_format = sc->color_formats[i] & 3;
                encoded_type |= lowbits_of_format << bitpos;
                bitpos += 2;
            }
            int encoded_type_lowpart = encoded_type & 0x3F;
            int encoded_type_highpart = encoded_type >> 6;
            int encoded_type_highpart_size = (3 * partition_count) - 4;
            int encoded_type_highpart_pos = 128 - bits_for_weights - encoded_type_highpart_size;
            write_bits(encoded_type_lowpart, 6, 13 + PARTITION_BITS, res.data);
            write_bits(encoded_type_highpart, encoded_type_highpart_size, encoded_type_highpart_pos, res.data);

            below_weights_pos -= encoded_type_highpart_size;
        }
    }

    else
        write_bits(sc->color_formats[0], 4, 13, res.data);

    // in dual-plane mode, encode the color component of the second plane of weights
    if (is_dual_plane)
        write_bits(sc->plane2_color_component, 2, below_weights_pos - 2, res.data);

    // finally, encode the color bits
    // first, get hold of all the color components to encode
    uint8_t values_to_encode[32];
    int valuecount_to_encode = 0;
    for (i = 0; i < sc->partition_count; i++) {
        int vals = 2 * (sc->color_formats[i] >> 2) + 2;
        for (j = 0; j < vals; j++)
            values_to_encode[j + valuecount_to_encode] = (uint8_t)sc->color_values[i][j];
        valuecount_to_encode += vals;
    }
    // then, encode an ISE based on them.
    encode_ise(sc->color_quantization_level, valuecount_to_encode, values_to_encode, res.data, (sc->partition_count == 1 ? 17 : 19 + PARTITION_BITS));

    return res;
}

//===================== SYMBOLIC TO PHYSICAL END =============================

//---------------------
// ASTC use with OpenCL
//---------------------
#if defined(__OPENCL_VERSION__)
__kernel void CMP_GPUEncoder(
    __global unsigned char      *p_source_pixels,
    __global unsigned char      *p_encoded_blocks,
    __global Source_Info        *SourceInfo,
    __global ASTC_Encode        *ASTCEncode
) {
    //=================================
    // Get the Thread workspace
    //=================================
#ifdef ASPM_GPU
    int pixel_block_x = get_global_id(0);
    int pixel_block_y = get_global_id(1);
#else
    (groupOffset);
    (SourceInfo);
#endif

    //=================================
    // Check scope of work is in range
    //=================================
    if (pixel_block_x >= ASTCEncode->m_width_in_blocks) return;
    if (pixel_block_y >= ASTCEncode->m_height_in_blocks) return;

    //==================================
    // Set Destination block index
    //==================================

    CGU_UINT dest_block_index = (pixel_block_y * (ASTCEncode->m_src_width / ASTCEncode->m_ydim) + pixel_block_x)*BYTES_PER_DESTINATION_BLOCK;

    //====================
    // Source Pixel block
    //====================
    CGU_UINT stride    = ASTCEncode->m_src_width * BYTEPP;
    CGU_UINT srcOffset = (pixel_block_x*ASTCEncode->m_xdim*BYTEPP) + (pixel_block_y*stride*ASTCEncode->m_ydim);

    //=================================
    // Load the pixels for this thread.
    //=================================
    astc_codec_image input_image;

    input_image.xsize   = ASTCEncode->m_xdim;
    input_image.ysize   = ASTCEncode->m_ydim;
#ifdef ASTC_ENABLE_3D_SUPPORT
    input_image.zsize   = ASTCEncode->m_zdim;
#else
    input_image.zsize   = 1;
#endif
    input_image.padding = 0;

    CGU_UINT dest_index = 0;
    CGU_UINT srcidx;
    for (unsigned int i = 0; i < ASTCEncode->m_ydim; i++) {
        srcidx = (i*stride) + srcOffset;
        for (unsigned int j = 0; j < ASTCEncode->m_xdim; j++) {
            input_image.pixels[dest_index].x = p_source_pixels[srcidx++];
            input_image.pixels[dest_index].y = p_source_pixels[srcidx++];
            input_image.pixels[dest_index].z = p_source_pixels[srcidx++];
            input_image.pixels[dest_index].w = p_source_pixels[srcidx++];
            dest_index++;
        }
    }

    CGU_UINT pixelcount = dest_index;

    // Data is in this form AABBGGRR
    //printf("(%d %d) R = %x G = %x B = %x\n", pixel_block_x, pixel_block_y, input_image.pixels[0].x, input_image.pixels[0].y, input_image.pixels[0].z);

    //==================================================
    // fetch an imageblock from the input source
    //==================================================
    imageblock pb;
    symbolic_compressed_block  scb;

    //if ((pixel_block_x == 0) && (pixel_block_y == 0))
    //{
    //  printf("ASTCEncode partition[0][0]partition_of_texel[0]        = %3.3f\n", ASTCEncode->bsd.decimation_tables[0].texel_weights_float[0][0]);
    //  printf("ASTCEncode m_Quality          = %f", ASTCEncode->m_Quality);
    //  printf("ASTCEncode m_texels_per_block = %d", ASTCEncode->m_texels_per_block);
    //}

    fetch_imageblock(&input_image, &pb,pixelcount,ASTCEncode);

    //printf("(%d %d) orig data %f %f %f\n", pixel_block_x, pixel_block_y, pb.orig_data[0], pb.orig_data[1], pb.orig_data[2]);
    //printf("(%d %d) work data %f %f %f\n", pixel_block_x, pixel_block_y, pb.work_data[0], pb.work_data[1], pb.work_data[2]);
    //printf("(%d %d) alpha_max %.3f alpha_min %.3f\n", pixel_block_x, pixel_block_y, pb.alpha_max, pb.alpha_max);

    compress_symbolic_block(&pb,&scb,ASTCEncode );

    // Copy the compress data to destination
    physical_compressed_block   pcb;
    pcb = symbolic_to_physical(&scb, ASTCEncode);

    p_encoded_blocks = p_encoded_blocks + dest_block_index;
    int j;
    for (j = 0; j < 16; j++) {
        p_encoded_blocks[j] = pcb.data[j];
    }

    // ("Test %x %x %x %x", pcb.data[0], pcb.data[1], pcb.data[2], pcb.data[3]);
}
#endif


} // Namespace ASTC_Kernel

