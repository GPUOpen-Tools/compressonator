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
//
//  File Name:   Codec_DXTC.cpp
//  Description: implementation of the CCodec_DXTC class
//
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codec_dxtc.h"
#include "compressonatorxcodec.h"
#include "dxtc_v11_compress.h"

#include "common_def.h"

#ifdef _WIN32
#pragma warning(disable : 4201)
#endif

CodecError CCodec_DXTC::CompressAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]) {
    BYTE nEndpoints[2][2];
    BYTE nIndices[2][BLOCK_SIZE_4X4];
    float fError8 = CompBlock1X(alphaBlock, BLOCK_SIZE_4X4, nEndpoints[0], nIndices[0], 8, false, m_bUseSSE2, 8, 0, true);
    float fError6 = (fError8 == 0.f) ? FLT_MAX : CompBlock1X(alphaBlock, BLOCK_SIZE_4X4, nEndpoints[1], nIndices[1], 6, true, m_bUseSSE2, 8, 0, true);
    if(fError8 <= fError6)
        EncodeAlphaBlock(compressedBlock, nEndpoints[0], nIndices[0]);
    else
        EncodeAlphaBlock(compressedBlock, nEndpoints[1], nIndices[1]);
    return CE_OK;
}

// //========================== CMP_CORE Code ===================================
// #define CMP_QUALITY2 0.601f
// #ifndef MAX_ERROR
// #define MAX_ERROR 128000.f
// #endif
// #ifndef GBL_SCH_STEP
// #define GBL_SCH_STEP_MXS 0.018f
// #define GBL_SCH_EXT_MXS 0.1f
// #define LCL_SCH_STEP_MXS 0.6f
// #define GBL_SCH_STEP_MXQ 0.0175f
// #define GBL_SCH_EXT_MXQ 0.154f
// #define LCL_SCH_STEP_MXQ 0.45f
// 
// #define GBL_SCH_STEP GBL_SCH_STEP_MXS
// #define GBL_SCH_EXT GBL_SCH_EXT_MXS
// #define LCL_SCH_STEP LCL_SCH_STEP_MXS
// #endif
// 
// #define SCH_STPS 3  // number of search steps to make at each end of interval
// static CMP_CONSTANT CGU_FLOAT sMvF[] = {0.f, -1.f, 1.f, -2.f, 2.f, -3.f, 3.f, -4.f, 4.f, -5.f, 5.f, -6.f, 6.f, -7.f, 7.f, -8.f, 8.f};
// #define MAX_POINTS 16
// #define NUM_ENDPOINTS 2
// #define CMP_ALPHA_RAMP 8
// 
// 
// static CGU_INT QSortFCmp(const void* Elem1, const void* Elem2)
// {
//     CGU_INT ret = 0;
// 
//     if (*(CGU_FLOAT*)Elem1 < *(CGU_FLOAT*)Elem2)
//         ret = -1;
//     else if (*(CGU_FLOAT*)Elem1 > *(CGU_FLOAT*)Elem2)
//         ret = 1;
//     return ret;
// }
// 
// static CGU_FLOAT cmp_getRampError(CGU_FLOAT _Blk[BLOCK_SIZE_4X4],
//                                   CGU_FLOAT _Rpt[BLOCK_SIZE_4X4],
//                                   CGU_FLOAT _maxerror,
//                                   CGU_FLOAT _min_ex,
//                                   CGU_FLOAT _max_ex,
//                                   CGU_INT   _NmbrClrs)
// {  // Max 16
//     CGU_INT         i;
//     CGU_FLOAT       error  = 0;
//     const CGU_FLOAT step   = (_max_ex - _min_ex) / 7;  // (CGU_FLOAT)(dwNumPoints - 1);
//     const CGU_FLOAT step_h = step * 0.5f;
//     const CGU_FLOAT rstep  = 1.0f / step;
// 
//     for (i = 0; i < _NmbrClrs; i++)
//     {
//         CGU_FLOAT v;
//         // Work out which value in the block this select
//         CGU_FLOAT del;
// 
//         if ((del = _Blk[i] - _min_ex) <= 0)
//             v = _min_ex;
//         else if (_Blk[i] - _max_ex >= 0)
//             v = _max_ex;
//         else
//             v = (floor((del + step_h) * rstep) * step) + _min_ex;
// 
//         // And accumulate the error
//         CGU_FLOAT del2 = (_Blk[i] - v);
//         error += del2 * del2 * _Rpt[i];
// 
//         // if we've already lost to the previous step bail out
//         if (_maxerror < error)
//         {
//             error = _maxerror;
//             break;
//         }
//     }
//     return error;
// }
// 
// 
// static CGU_FLOAT cmp_linearBlockRefine(CGU_FLOAT _Blk[BLOCK_SIZE_4X4],
//                                        CGU_FLOAT _Rpt[BLOCK_SIZE_4X4],
//                                        CGU_FLOAT _MaxError,
//                                        CMP_INOUT CGU_FLOAT CMP_PTRINOUT _min_ex,
//                                        CMP_INOUT CGU_FLOAT CMP_PTRINOUT _max_ex,
//                                        CGU_FLOAT                        _m_step,
//                                        CGU_FLOAT                        _min_bnd,
//                                        CGU_FLOAT                        _max_bnd,
//                                        CGU_INT                          _NmbrClrs)
// {
//     // Start out assuming our endpoints are the min and max values we've
//     // determined
// 
//     // Attempt a (simple) progressive refinement step to reduce noise in the
//     // output image by trying to find a better overall match for the endpoints.
// 
//     CGU_FLOAT maxerror = _MaxError;
//     CGU_FLOAT min_ex   = CMP_PTRINOUT _min_ex;
//     CGU_FLOAT max_ex   = CMP_PTRINOUT _max_ex;
// 
//     CGU_INT mode, bestmode;
// 
//     do
//     {
//         CGU_FLOAT cr_min0 = min_ex;
//         CGU_FLOAT cr_max0 = max_ex;
//         for (bestmode = -1, mode = 0; mode < SCH_STPS * SCH_STPS; mode++)
//         {
//             // check each move (see sStep for direction)
//             CGU_FLOAT cr_min = min_ex + _m_step * sMvF[mode / SCH_STPS];
//             CGU_FLOAT cr_max = max_ex + _m_step * sMvF[mode % SCH_STPS];
// 
//             cr_min = max(cr_min, _min_bnd);
//             cr_max = min(cr_max, _max_bnd);
// 
//             CGU_FLOAT error;
//             error = cmp_getRampError(_Blk, _Rpt, maxerror, cr_min, cr_max, _NmbrClrs);
// 
//             if (error < maxerror)
//             {
//                 maxerror = error;
//                 bestmode = mode;
//                 cr_min0  = cr_min;
//                 cr_max0  = cr_max;
//             }
//         }
// 
//         if (bestmode != -1)
//         {
//             // make move (see sStep for direction)
//             min_ex = cr_min0;
//             max_ex = cr_max0;
//         }
//     } while (bestmode != -1);
// 
//     CMP_PTRINOUT _min_ex = min_ex;
//     CMP_PTRINOUT _max_ex = max_ex;
// 
//     return maxerror;
// }
// 
// static CGU_Vec2i cmp_getLinearEndPoints(CGU_FLOAT _Blk[BLOCK_SIZE_4X4], CMP_IN CGU_FLOAT fquality, CMP_IN CGU_BOOL isSigned)
// {
//     CGU_UINT32 i;
//     CGU_Vec2i  cmpMinMax;
// 
//     CGU_FLOAT scalePts      = 1.0f; //isSigned ? 128.0f : 255.0f;
//     CGU_FLOAT scaleOffset   = isSigned ? 0.25f  : 0.5f;
// 
//     //================================================================
//     // Bounding Box
//     // lowest quality calculation to get min and max value to use
//     //================================================================
//     if (fquality < CMP_QUALITY2)
//     {
//         cmpMinMax.x = _Blk[0];
//         cmpMinMax.y = _Blk[0];
//         for (i = 1; i < BLOCK_SIZE_4X4; ++i)
//         {
//             cmpMinMax.x = min(cmpMinMax.x, _Blk[i]);
//             cmpMinMax.y = max(cmpMinMax.y, _Blk[i]);
//         }
//         return cmpMinMax;
//     }
// 
//     //================================================================
//     // Do more calculations to get the best min and max values to use
//     //================================================================
//     CGU_FLOAT Ramp[2];
// 
//     // Result defaults for SNORM or UNORM
//     Ramp[0] = isSigned ? -1.0f : 0.0f;
//     Ramp[1] = 1.0f;
// 
//     CGU_FLOAT afUniqueValues[BLOCK_SIZE_4X4];
//     CGU_FLOAT afValueRepeats[BLOCK_SIZE_4X4];
//     for (i = 0; i < BLOCK_SIZE_4X4; i++)
//         afUniqueValues[i] = afValueRepeats[i] = 0.f;
// 
//     // For each unique value we compute the number of it appearances.
//     CGU_FLOAT fBlk[BLOCK_SIZE_4X4];
// 
//     memcpy(fBlk, _Blk, BLOCK_SIZE_4X4 * sizeof(CGU_FLOAT));
//     qsort((void*)fBlk, (size_t)BLOCK_SIZE_4X4, sizeof(CGU_FLOAT), QSortFCmp);
// 
//     CGU_FLOAT new_p = -2.0f;
// 
//     CGU_UINT32 dwUniqueValues    = 0;
//     afUniqueValues[0]            = 0.0f;
//     CGU_BOOL requiresCalculation = true;
// 
//     {
//         // Ramp not fixed
//         for (i = 0; i < BLOCK_SIZE_4X4; i++)
//         {
//             if (new_p != fBlk[i])
//             {
//                 afUniqueValues[dwUniqueValues] = new_p = fBlk[i];
//                 afValueRepeats[dwUniqueValues]         = 1.f;
//                 dwUniqueValues++;
//             }
//             else if (dwUniqueValues)
//                 afValueRepeats[dwUniqueValues - 1] += 1.f;
//         }
// 
//         // if number of unique colors is less or eq 2, we've done
//         if (dwUniqueValues <= 2)
//         {
//             Ramp[0] = floor(afUniqueValues[0] * scalePts + scaleOffset);
//             if (dwUniqueValues == 1)
//                 Ramp[1] = Ramp[0] + 1.f;
//             else
//                 Ramp[1] = floor(afUniqueValues[1] * scalePts + scaleOffset);
//             requiresCalculation = false;
//         }
//     }  // Ramp not fixed
// 
//     if (requiresCalculation)
//     {
//         CGU_FLOAT min_ex  = afUniqueValues[0];
//         CGU_FLOAT max_ex  = afUniqueValues[dwUniqueValues - 1];
//         CGU_FLOAT min_bnd = 0, max_bnd = 1.;
//         CGU_FLOAT min_r = min_ex, max_r = max_ex;
//         CGU_FLOAT gbl_l = 0, gbl_r = 0;
//         CGU_FLOAT cntr = (min_r + max_r) / 2;
// 
//         CGU_FLOAT gbl_err = MAX_ERROR;
//         // Trying to avoid unnecessary calculations. Heuristics: after some analisis
//         // it appears that in integer case, if the input interval not more then 48
//         // we won't get much better
//         bool wantsSearch = !((max_ex - min_ex) <= (48.f / scalePts));
// 
//         if (wantsSearch)
//         {
//             // Search.
//             // 1. take the vicinities of both low and high bound of the input
//             // interval.
//             // 2. setup some search step
//             // 3. find the new low and high bound which provides an (sub) optimal
//             // (infinite precision) clusterization.
//             CGU_FLOAT gbl_llb = (min_bnd > min_r - GBL_SCH_EXT) ? min_bnd : min_r - GBL_SCH_EXT;
//             CGU_FLOAT gbl_rrb = (max_bnd < max_r + GBL_SCH_EXT) ? max_bnd : max_r + GBL_SCH_EXT;
//             CGU_FLOAT gbl_lrb = (cntr < min_r + GBL_SCH_EXT) ? cntr : min_r + GBL_SCH_EXT;
//             CGU_FLOAT gbl_rlb = (cntr > max_r - GBL_SCH_EXT) ? cntr : max_r - GBL_SCH_EXT;
// 
//             for (CGU_FLOAT step_l = gbl_llb; step_l < gbl_lrb; step_l += GBL_SCH_STEP)
//             {
//                 for (CGU_FLOAT step_r = gbl_rrb; gbl_rlb <= step_r; step_r -= GBL_SCH_STEP)
//                 {
//                     CGU_FLOAT sch_err;
//                     // an sse version is avaiable
//                     sch_err = cmp_getRampError(afUniqueValues, afValueRepeats, gbl_err, step_l, step_r, dwUniqueValues);
//                     if (sch_err < gbl_err)
//                     {
//                         gbl_err = sch_err;
//                         gbl_l   = step_l;
//                         gbl_r   = step_r;
//                     }
//                 }
//             }
// 
//             min_r = gbl_l;
//             max_r = gbl_r;
//         }  // want search
// 
//         // This is a refinement call. The function tries to make several small
//         // stretches or squashes to minimize quantization error.
//         CGU_FLOAT m_step = LCL_SCH_STEP / scalePts;
//         cmp_linearBlockRefine(afUniqueValues, afValueRepeats, gbl_err, CMP_REFINOUT min_r, CMP_REFINOUT max_r, m_step, min_bnd, max_bnd, dwUniqueValues);
// 
//         min_ex = min_r;
//         max_ex = max_r;
//         max_ex *= scalePts;
//         min_ex *= scalePts;
// 
//         Ramp[0] = floor(min_ex + scaleOffset);
//         Ramp[1] = floor(max_ex + scaleOffset);
//     }
// 
//     // Ensure that the two endpoints are not the same
//     // This is legal but serves no need & can break some optimizations in the compressor
//     if (Ramp[0] == Ramp[1])
//     {
//         if (Ramp[1] < scalePts)
//             Ramp[1] = Ramp[1] + .1f;
//         else if (Ramp[1] > 0.0f)
//             Ramp[1] = Ramp[1] - .1f;
//     }
// 
//     cmpMinMax.x = Ramp[0];
//     cmpMinMax.y = Ramp[1];
// 
//     return cmpMinMax;
// }



static CGU_INT8 cmp_SNormFloatToSInt(CGU_FLOAT fsnorm)
{
    if (isnan(fsnorm))
        fsnorm = 0;
    else if (fsnorm > 1)
        fsnorm = 1;  // Clamp to 1
    else if (fsnorm < -1)
        fsnorm = -1;  // Clamp to -1

    fsnorm = fsnorm * 127U;

    // shift round up or down
    if (fsnorm >= 0)
        fsnorm += .5f;
    else
        fsnorm -= .5f;

    CGU_INT8 res = static_cast<CGU_INT8>(fsnorm);

    return (res);
}

static CGU_Vec2f cmp_OptimizeEndPoints(CGU_FLOAT* pPoints, CGU_INT8 cSteps, CGU_BOOL isSigned)
{
    CGU_Vec2f fendpoints;
    CGU_FLOAT MAX_VALUE = 1.0f;
    CGU_FLOAT MIN_VALUE = isSigned ? -1.0f : 0.0f;

    // Find Min and Max points, as starting point
    CGU_FLOAT fX = MAX_VALUE;
    CGU_FLOAT fY = MIN_VALUE;

    if (8 == cSteps)
    {
        for (CGU_INT8 iPoint = 0; iPoint < BLOCK_SIZE_4X4; iPoint++)
        {
            if (pPoints[iPoint] < fX)
                fX = pPoints[iPoint];
    
            if (pPoints[iPoint] > fY)
                fY = pPoints[iPoint];
        }
    }
    else
    {
        for (CGU_INT8 iPoint = 0; iPoint < BLOCK_SIZE_4X4; iPoint++)
        {
            if (pPoints[iPoint] < fX && pPoints[iPoint] > MIN_VALUE)
                fX = pPoints[iPoint];
    
            if (pPoints[iPoint] > fY && pPoints[iPoint] < MAX_VALUE)
                fY = pPoints[iPoint];
        }
    
        if (fX == fY)
        {
            fY = MAX_VALUE;
        }
    }

    //===================
    // Use Newton Method
    //===================
    CGU_FLOAT cStepsDiv = static_cast<CGU_FLOAT>(cSteps - 1);
    CGU_FLOAT pSteps[8];
    CGU_FLOAT fc;
    CGU_FLOAT fd;

    for (CGU_UINT8 iIteration = 0; iIteration < 8; iIteration++)
    {
        // reach minimum threashold break
        if ((fY - fX) < (1.0f / 256.0f))
            break;

        CGU_FLOAT fScale = cStepsDiv / (fY - fX);

        // Calculate new steps
        for (CGU_INT8 iStep = 0; iStep < cSteps; iStep++)
        {
            fc            = (cStepsDiv - (CGU_FLOAT)iStep) / cStepsDiv;
            fd            = (CGU_FLOAT)iStep / cStepsDiv;
            pSteps[iStep] = fc * fX + fd * fY;
        }

        if (6 == cSteps)
        {
            pSteps[6] = MIN_VALUE;
            pSteps[7] = MAX_VALUE;
        }

        // Evaluate function, and derivatives
        CGU_FLOAT dX  = 0.0f;
        CGU_FLOAT dY  = 0.0f;
        CGU_FLOAT d2X = 0.0f;
        CGU_FLOAT d2Y = 0.0f;

        for (CGU_UINT8 iPoint = 0; iPoint < BLOCK_SIZE_4X4; iPoint++)
        {
            float fDot = (pPoints[iPoint] - fX) * fScale;

            CGU_INT8 iStep;
            if (fDot <= 0.0f)
            {
                iStep = ((6 == cSteps) && (pPoints[iPoint] <= (fX + MIN_VALUE) * 0.5f)) ? 6u : 0u;
            }
            else if (fDot >= cStepsDiv)
            {
                iStep = ((6 == cSteps) && (pPoints[iPoint] >= (fY + MAX_VALUE) * 0.5f)) ? 7u : (cSteps - 1);
            }
            else
            {
                iStep = CGU_INT8(fDot + 0.5f);
            }

            // steps to improve quality
            if (iStep < cSteps)
            {
                fc          = (cStepsDiv - (CGU_FLOAT)iStep) / cStepsDiv;
                fd          = (CGU_FLOAT)iStep / cStepsDiv;
                CGU_FLOAT fDiff = pSteps[iStep] - pPoints[iPoint];
                dX  += fc * fDiff;
                d2X += fc * fc;
                dY  += fd * fDiff;
                d2Y += fd * fd;
            }
        }

        // Move endpoints
        if (d2X > 0.0f)
            fX -= dX / d2X;

        if (d2Y > 0.0f)
            fY -= dY / d2Y;

        if (fX > fY)
        {
            float f = fX;
            fX      = fY;
            fY      = f;
        }

        if ((dX * dX < (1.0f / 64.0f)) && (dY * dY < (1.0f / 64.0f)))
            break;
    }

    fendpoints.x = (fX < MIN_VALUE) ? MIN_VALUE : (fX > MAX_VALUE) ? MAX_VALUE : fX;
    fendpoints.y = (fY < MIN_VALUE) ? MIN_VALUE : (fY > MAX_VALUE) ? MAX_VALUE : fY;

    return fendpoints;
}

static CGU_Vec2i CMP_FindEndpointsAlphaBlockSnorm(CGU_FLOAT alphaBlockSnorm[])
{

    //================================================================
    // Bounding Box
    // lowest quality calculation to get min and max value to use
    //================================================================
    CGU_Vec2f cmpMinMax;
    cmpMinMax.x  = alphaBlockSnorm[0];
    cmpMinMax.y  = alphaBlockSnorm[0];

        for (CGU_UINT8 i = 0; i < BLOCK_SIZE_4X4; ++i)
        {
            if (alphaBlockSnorm[i] < cmpMinMax.x)
            {
                cmpMinMax.x = alphaBlockSnorm[i];
            }
            else if (alphaBlockSnorm[i] > cmpMinMax.y)
            {
                cmpMinMax.y = alphaBlockSnorm[i];
            }
        }

    CGU_Vec2i endpoints;
    CGU_Vec2f fendpoints;

    // Are we done for lowest quality setting!
    // CGU_FLOAT fquality = 1.0f;
    // 
    // if (fquality < CMP_QUALITY2) {
    //     endpoints.x = (CGU_INT8)(cmpMinMax.x);
    //     endpoints.y = (CGU_INT8)(cmpMinMax.y);
    //     return endpoints;
    // }

    //================================================================
    // Do more calculations to get the best min and max values to use
    //================================================================
    if ((-1.0f == cmpMinMax.x || 1.0f == cmpMinMax.y))
    {
        fendpoints = cmp_OptimizeEndPoints(alphaBlockSnorm, 6, true);
        endpoints.x = cmp_SNormFloatToSInt(fendpoints.x);
        endpoints.y = cmp_SNormFloatToSInt(fendpoints.y);
    }
    else
    {
        fendpoints = cmp_OptimizeEndPoints(alphaBlockSnorm, 8, true);
        endpoints.x = cmp_SNormFloatToSInt(fendpoints.y);
        endpoints.y = cmp_SNormFloatToSInt(fendpoints.x);
    }

    return endpoints;
}

static uint64_t cmp_getBlockPackedIndicesSNorm(CGU_Vec2f alphaMinMax, const float alphaBlockSnorm[], uint64_t data)
{
    CGU_FLOAT alpha[8];
    alpha[0] = alphaMinMax.x;
    alpha[1] = alphaMinMax.y;

    if (alphaMinMax.x > alphaMinMax.y)
    {
        // 8-alpha block:  derive the other six alphas.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = (alpha[0] * 6.0f + alpha[1]) / 7.0f;
        alpha[3] = (alpha[0] * 5.0f + alpha[1] * 2.0f) / 7.0f;
        alpha[4] = (alpha[0] * 4.0f + alpha[1] * 3.0f) / 7.0f;
        alpha[5] = (alpha[0] * 3.0f + alpha[1] * 4.0f) / 7.0f;
        alpha[6] = (alpha[0] * 2.0f + alpha[1] * 5.0f) / 7.0f;
        alpha[7] = (alpha[0] + alpha[1] * 6.0f) / 7.0f;
    }
    else
    {
        // 6-alpha block.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = (alpha[0] * 4.0f + alpha[1]) / 5.0f;
        alpha[3] = (alpha[0] * 3.0f + alpha[1] * 2.0f) / 5.0f;
        alpha[4] = (alpha[0] * 2.0f + alpha[1] * 3.0f) / 5.0f;
        alpha[5] = (alpha[0] + alpha[1] * 4.0f) / 5.0f;
        alpha[6] = -1.0f;
        alpha[7] = 1.0f;
    }

    // Index all colors using best alpha value
    for (CGU_UINT8 i = 0; i < BLOCK_SIZE_4X4; ++i)
    {
        CGU_UINT8 uBestIndex = 0;
        CGU_FLOAT fBestDelta = CMP_FLOAT_MAX;
        for (CGU_INT32 uIndex = 0; uIndex < 8; uIndex++)
        {
            CGU_FLOAT fCurrentDelta = fabsf(alpha[uIndex] - alphaBlockSnorm[i]);
            if (fCurrentDelta < fBestDelta)
            {
                uBestIndex = (CGU_UINT8)(uIndex);
                fBestDelta = fCurrentDelta;
            }
        }

        data &= ~(uint64_t(0x07)       << (3 * i + 16));
        data |=  (uint64_t(uBestIndex) << (3 * i + 16));
    }

    return data;
}

//=============================================================================

CodecError CCodec_DXTC::CompressAlphaBlockSNorm(CMP_FLOAT alphaBlockSnorm[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    union
    {
        CMP_DWORD compressedBlock[2];
        struct
        {
            int8_t  red_0;
            int8_t  red_1;
            uint8_t indices[6];
        };
        uint64_t data;
    } BC4_Snorm_block;

    BC4_Snorm_block.data = 0LL;

    CGU_Vec2i reds;
    reds = CMP_FindEndpointsAlphaBlockSnorm(alphaBlockSnorm);

    BC4_Snorm_block.red_0 = reds.x & 0xFF;
    BC4_Snorm_block.red_1 = reds.y & 0xFF;

    // check low end boundaries
    if (BC4_Snorm_block.red_0 == -128)
        BC4_Snorm_block.red_0 = -127;
    if (BC4_Snorm_block.red_1 == -128)
        BC4_Snorm_block.red_1 = -127;

    // Normalize signed int -128..127 to float -1..1
    CGU_Vec2f alphaMinMax;
    alphaMinMax.x = CGU_FLOAT(BC4_Snorm_block.red_0) / 127.0f;
    alphaMinMax.y = CGU_FLOAT(BC4_Snorm_block.red_1) / 127.0f;

    BC4_Snorm_block.data = cmp_getBlockPackedIndicesSNorm(alphaMinMax, alphaBlockSnorm, BC4_Snorm_block.data);

    compressedBlock[0] = BC4_Snorm_block.compressedBlock[0];
    compressedBlock[1] = BC4_Snorm_block.compressedBlock[1];

    return CE_OK;
}

CodecError CCodec_DXTC::CompressAlphaBlock_Fast(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]) {
#ifdef _WIN64
    CompressAlphaBlock(alphaBlock, compressedBlock);
#else // !_WIN64
    DXTCV11CompressAlphaBlock(alphaBlock, compressedBlock);
#endif
    return CE_OK;
}

CodecError CCodec_DXTC::CompressAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]) {
    BYTE nEndpoints[2][2];
    BYTE nIndices[2][BLOCK_SIZE_4X4];
    float fError8 = CompBlock1X(alphaBlock, BLOCK_SIZE_4X4, nEndpoints[0], nIndices[0], 8, false, m_bUseSSE2, 8, 0, true);
    float fError6 = (fError8 == 0.f) ? FLT_MAX : CompBlock1X(alphaBlock, BLOCK_SIZE_4X4, nEndpoints[1], nIndices[1], 6, true, m_bUseSSE2, 8, 0, true);
    if(fError8 <= fError6)
        EncodeAlphaBlock(compressedBlock, nEndpoints[0], nIndices[0]);
    else
        EncodeAlphaBlock(compressedBlock, nEndpoints[1], nIndices[1]);
    return CE_OK;
}

void CCodec_DXTC::EncodeAlphaBlock(CMP_DWORD compressedBlock[2], BYTE nEndpoints[2], BYTE nIndices[BLOCK_SIZE_4X4]) {
    compressedBlock[0] = ((int)nEndpoints[0]) | (((int)nEndpoints[1])<<8);
    compressedBlock[1] = 0;

    for(int i = 0; i < BLOCK_SIZE_4X4; i++) {
        if(i < 5)
            compressedBlock[0] |= (nIndices[i] & 0x7) << (16 + (i * 3));
        else if(i > 5)
            compressedBlock[1] |= (nIndices[i] & 0x7) << (2 + (i-6) * 3);
        else {
            compressedBlock[0] |= (nIndices[i] & 0x1) << 31;
            compressedBlock[1] |= (nIndices[i] & 0x6) >> 1;
        }
    }
}

//
// This function decompresses a block
//
void CCodec_DXTC::DecompressAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]) {
    CMP_BYTE alpha[8];
    GetCompressedAlphaRamp(alpha, compressedBlock);

    for(int i = 0; i < BLOCK_SIZE_4X4; i++) {
        CMP_DWORD index;
        if(i < 5)
            index = (compressedBlock[0] & (0x7 << (16 + (i * 3)))) >> (16 + (i * 3));
        else if(i > 5)
            index = (compressedBlock[1] & (0x7 << (2 + (i-6)*3))) >> (2 + (i-6)*3);
        else {
            index = (compressedBlock[0] & 0x80000000) >> 31;
            index |= (compressedBlock[1] & 0x3) << 1;
        }

        alphaBlock[i] = alpha[index];
    }
}

//
// This function decompresses a signed block
//
void CCodec_DXTC::DecompressAlphaBlockInt8(CMP_SBYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    CMP_SBYTE alpha[8];
    GetCompressedAlphaRampS(alpha, compressedBlock);

    for (int i = 0; i < BLOCK_SIZE_4X4; i++)
    {
        CMP_DWORD index;
        if (i < 5)
            index = (compressedBlock[0] & (0x7 << (16 + (i * 3)))) >> (16 + (i * 3));
        else if (i > 5)
            index = (compressedBlock[1] & (0x7 << (2 + (i - 6) * 3))) >> (2 + (i - 6) * 3);
        else
        {
            index = (compressedBlock[0] & 0x80000000) >> 31;
            index |= (compressedBlock[1] & 0x3) << 1;
        }

        alphaBlock[i] = alpha[index];
    }
}

void CCodec_DXTC::DecompressAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]) {
    CODECFLOAT alpha[8];
    GetCompressedAlphaRamp(alpha, compressedBlock);

    for(int i = 0; i < BLOCK_SIZE_4X4; i++) {
        CMP_DWORD index;
        if(i < 5)
            index = (compressedBlock[0] & (0x7 << (16 + (i * 3)))) >> (16 + (i * 3));
        else if(i > 5)
            index = (compressedBlock[1] & (0x7 << (2 + (i-6)*3))) >> (2 + (i-6)*3);
        else {
            index = (compressedBlock[0] & 0x80000000) >> 31;
            index |= (compressedBlock[1] & 0x3) << 1;
        }

        alphaBlock[i] = alpha[index];
    }
}

#define EXPLICIT_ALPHA_PIXEL_MASK 0xf
#define EXPLICIT_ALPHA_PIXEL_BPP 4
CodecError CCodec_DXTC::CompressExplicitAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]) {
    DXTCV11CompressExplicitAlphaBlock(alphaBlock, compressedBlock);

    return CE_OK;
}

CodecError CCodec_DXTC::CompressExplicitAlphaBlock_Fast(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]) {
    // Should remove or update this:  DXTCV11CompressExplicitAlphaBlockMMX(alphaBlock, compressedBlock);
    CompressExplicitAlphaBlock(alphaBlock, compressedBlock);
    return CE_OK;
}

CodecError CCodec_DXTC::CompressExplicitAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]) {
    compressedBlock[0] = compressedBlock[1] = 0;
    for(int i=0; i<16; i++) {
        int nBlock = i < 8 ? 0 : 1;
        CMP_BYTE cAlpha = CONVERT_FLOAT_TO_BYTE(alphaBlock[i]);
        cAlpha = (CMP_BYTE) ((cAlpha + ((cAlpha >> EXPLICIT_ALPHA_PIXEL_BPP) < 0x8 ? 7 : 8) - (cAlpha >> EXPLICIT_ALPHA_PIXEL_BPP)) >> EXPLICIT_ALPHA_PIXEL_BPP);
        if(cAlpha > EXPLICIT_ALPHA_PIXEL_MASK)
            cAlpha = EXPLICIT_ALPHA_PIXEL_MASK;
        compressedBlock[nBlock] |= (cAlpha << ((i % 8) * EXPLICIT_ALPHA_PIXEL_BPP));
    }

    return CE_OK;
}

//
// This function decompresses an explicit alpha block (DXT3)
//
void CCodec_DXTC::DecompressExplicitAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]) {
    for(int i=0; i<16; i++) {
        int nBlock = i < 8 ? 0 : 1;
        CMP_BYTE cAlpha = (CMP_BYTE) ((compressedBlock[nBlock] >> ((i % 8) * EXPLICIT_ALPHA_PIXEL_BPP)) & EXPLICIT_ALPHA_PIXEL_MASK);
        alphaBlock[i] = (CMP_BYTE) ((cAlpha << EXPLICIT_ALPHA_PIXEL_BPP) | cAlpha);
    }
}

void CCodec_DXTC::DecompressExplicitAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]) {
    for(int i=0; i<16; i++) {
        int nBlock = i < 8 ? 0 : 1;
        CMP_BYTE cAlpha = (CMP_BYTE) ((compressedBlock[nBlock] >> ((i % 8) * EXPLICIT_ALPHA_PIXEL_BPP)) & EXPLICIT_ALPHA_PIXEL_MASK);
        alphaBlock[i] = CONVERT_BYTE_TO_FLOAT((cAlpha << EXPLICIT_ALPHA_PIXEL_BPP) | cAlpha);
    }
}

void CCodec_DXTC::GetCompressedAlphaRamp(CMP_BYTE alpha[8], CMP_DWORD compressedBlock[2]) {
    alpha[0] = (CMP_BYTE)(compressedBlock[0] & 0xff);
    alpha[1] = (CMP_BYTE)((compressedBlock[0]>>8) & 0xff);

    if (alpha[0] > alpha[1]) {
        // 8-alpha block:  derive the other six alphas.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = static_cast<CMP_BYTE>((6 * alpha[0] + 1 * alpha[1] + 3) / 7);    // bit code 010
        alpha[3] = static_cast<CMP_BYTE>((5 * alpha[0] + 2 * alpha[1] + 3) / 7);    // bit code 011
        alpha[4] = static_cast<CMP_BYTE>((4 * alpha[0] + 3 * alpha[1] + 3) / 7);    // bit code 100
        alpha[5] = static_cast<CMP_BYTE>((3 * alpha[0] + 4 * alpha[1] + 3) / 7);    // bit code 101
        alpha[6] = static_cast<CMP_BYTE>((2 * alpha[0] + 5 * alpha[1] + 3) / 7);    // bit code 110
        alpha[7] = static_cast<CMP_BYTE>((1 * alpha[0] + 6 * alpha[1] + 3) / 7);    // bit code 111
    } else {
        // 6-alpha block.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = static_cast<CMP_BYTE>((4 * alpha[0] + 1 * alpha[1] + 2) / 5);  // Bit code 010
        alpha[3] = static_cast<CMP_BYTE>((3 * alpha[0] + 2 * alpha[1] + 2) / 5);  // Bit code 011
        alpha[4] = static_cast<CMP_BYTE>((2 * alpha[0] + 3 * alpha[1] + 2) / 5);  // Bit code 100
        alpha[5] = static_cast<CMP_BYTE>((1 * alpha[0] + 4 * alpha[1] + 2) / 5);  // Bit code 101
        alpha[6] = 0;                                      // Bit code 110
        alpha[7] = 255;                                    // Bit code 111
    }
}

void CCodec_DXTC::GetCompressedAlphaRamp(CODECFLOAT alpha[8], CMP_DWORD compressedBlock[2]) {
    alpha[0] = CONVERT_BYTE_TO_FLOAT(compressedBlock[0] & 0xff);
    alpha[1] = CONVERT_BYTE_TO_FLOAT((compressedBlock[0]>>8) & 0xff);

    if (alpha[0] > alpha[1]) {
        // 8-alpha block:  derive the other six alphas.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = (6 * alpha[0] + 1 * alpha[1]) / 7;    // bit code 010
        alpha[3] = (5 * alpha[0] + 2 * alpha[1]) / 7;    // bit code 011
        alpha[4] = (4 * alpha[0] + 3 * alpha[1]) / 7;    // bit code 100
        alpha[5] = (3 * alpha[0] + 4 * alpha[1]) / 7;    // bit code 101
        alpha[6] = (2 * alpha[0] + 5 * alpha[1]) / 7;    // bit code 110
        alpha[7] = (1 * alpha[0] + 6 * alpha[1]) / 7;    // bit code 111
    } else {
        // 6-alpha block.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = (4 * alpha[0] + 1 * alpha[1]) / 5;  // Bit code 010
        alpha[3] = (3 * alpha[0] + 2 * alpha[1]) / 5;  // Bit code 011
        alpha[4] = (2 * alpha[0] + 3 * alpha[1]) / 5;  // Bit code 100
        alpha[5] = (1 * alpha[0] + 4 * alpha[1]) / 5;  // Bit code 101
        alpha[6] = 0;                                      // Bit code 110
        alpha[7] = 1.0f;                                   // Bit code 111
    }
}

void CCodec_DXTC::GetCompressedAlphaRampS(CMP_SBYTE alpha[8], CMP_DWORD compressedBlock[2])
{
    alpha[0] = (CMP_SBYTE)(compressedBlock[0] & 0xff);
    alpha[1] = (CMP_SBYTE)((compressedBlock[0] >> 8) & 0xff);

    if (alpha[0] > alpha[1])
    {
        // 8-alpha block:  derive the other six alphas.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = static_cast<CMP_SBYTE>((6 * alpha[0] + 1 * alpha[1] + 3) / 7);  // bit code 010
        alpha[3] = static_cast<CMP_SBYTE>((5 * alpha[0] + 2 * alpha[1] + 3) / 7);  // bit code 011
        alpha[4] = static_cast<CMP_SBYTE>((4 * alpha[0] + 3 * alpha[1] + 3) / 7);  // bit code 100
        alpha[5] = static_cast<CMP_SBYTE>((3 * alpha[0] + 4 * alpha[1] + 3) / 7);  // bit code 101
        alpha[6] = static_cast<CMP_SBYTE>((2 * alpha[0] + 5 * alpha[1] + 3) / 7);  // bit code 110
        alpha[7] = static_cast<CMP_SBYTE>((1 * alpha[0] + 6 * alpha[1] + 3) / 7);  // bit code 111
    }
    else
    {
        // 6-alpha block.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = static_cast<CMP_SBYTE>((4 * alpha[0] + 1 * alpha[1] + 2) / 5);  // Bit code 010
        alpha[3] = static_cast<CMP_SBYTE>((3 * alpha[0] + 2 * alpha[1] + 2) / 5);  // Bit code 011
        alpha[4] = static_cast<CMP_SBYTE>((2 * alpha[0] + 3 * alpha[1] + 2) / 5);  // Bit code 100
        alpha[5] = static_cast<CMP_SBYTE>((1 * alpha[0] + 4 * alpha[1] + 2) / 5);  // Bit code 101
        alpha[6] = -127;                                                           // Bit code 110
        alpha[7] = 127;                                                            // Bit code 111
    }
}



