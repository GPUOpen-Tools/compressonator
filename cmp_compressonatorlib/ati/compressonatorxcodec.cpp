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
//  File Name:   ATIXCodec.cpp
//  Description: Performs the DXT-style block compression
//
//
//////////////////////////////////////////////////////////////////////////////

#include "common.h"

using namespace CMP;

#include <algorithm>

#ifdef USE_SSE
#include <xmmintrin.h>
#include <emmintrin.h>
#endif // USE_SSE

#include "codec.h"

//#define USE_REFINE_CODE
//#define USE_CMP_CORE_API

#ifdef USE_CMP_CORE_API
#include "bcn_common_kernel.h"
#include "bcn_common_api.h"
#include "bc1_cmp.h"
#endif

using namespace CMP;

#include "compressonatorxcodec.h"

using namespace std;

#if defined(WIN32) || defined(_WIN64)
#    define ALIGN_16 __declspec(align(16))
#else // !WIN32 && !_WIN64
#    define ALIGN_16
#endif // !WIN32 && !_WIN64

#define MAX_BLOCK 64
#define MAX_POINTS 16

#define NUM_CHANNELS 4
#define NUM_ENDPOINTS 2

#define BLOCK_SIZE MAX_BLOCK

#define EPS        (2.f / 255.f) * (2.f / 255.f)
#define EPS2    3.f * (2.f / 255.f) * (2.f / 255.f)
#ifndef MAX_ERROR
#define MAX_ERROR 128000.f
#endif

#ifndef GBL_SCH_STEP
#define GBL_SCH_STEP_MXS 0.018f
#define GBL_SCH_EXT_MXS 0.1f
#define LCL_SCH_STEP_MXS 0.6f
#define GBL_SCH_STEP_MXQ 0.0175f
#define GBL_SCH_EXT_MXQ 0.154f
#define LCL_SCH_STEP_MXQ 0.45f

#define GBL_SCH_STEP GBL_SCH_STEP_MXS
#define GBL_SCH_EXT  GBL_SCH_EXT_MXS
#define LCL_SCH_STEP LCL_SCH_STEP_MXS
#endif

// Channel indexes
#define AC 3
#define RC 2
#define GC 1
#define BC 0

// Grid precision
#define PIX_GRID 8

#define ConstructColour(r, g, b)  (((r) << 11) | ((g) << 5) | (b))

BYTE nByteBitsMask[] = {
    0x00,
    0x80,
    0xc0,
    0xe0,
    0xf0,
    0xf8,
    0xfc,
    0xfe,
    0xff,
};

DWORD ConstructColor(BYTE R, BYTE nRedBits, BYTE G, BYTE nGreenBits, BYTE B, BYTE nBlueBits) {
    return (    ((R & nByteBitsMask[nRedBits])    << (nGreenBits + nBlueBits - (PIX_GRID - nRedBits))) |
                ((G & nByteBitsMask[nGreenBits])<< (nBlueBits - (PIX_GRID - nGreenBits))) |
                ((B & nByteBitsMask[nBlueBits]) >> ((PIX_GRID - nBlueBits))));
}

/*--------------------------------------------------------------------------
 3 DIM VECTOR CASE
---------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------*/
#ifndef USE_CMP_CORE_API
static int QSortIntCmp(const void * Elem1, const void * Elem2) {
    return (*(int*)Elem1 - *(int*)Elem2);
}
#endif

static int QSortFloatCmp(const void * Elem1, const void * Elem2) {
    DWORD* pdwElem1 = (DWORD*) Elem1;
    DWORD* pdwElem2 = (DWORD*) Elem2;
    if(    (pdwElem1[2] > pdwElem2[2]) ||
            (pdwElem1[2] == pdwElem2[2] && pdwElem1[1] > pdwElem2[1]) ||
            (pdwElem1[2] == pdwElem2[2] && pdwElem1[1] == pdwElem2[1] && pdwElem1[0] > pdwElem2[0]))
        return 1;
    else if((pdwElem1[2] < pdwElem2[2]) ||
            (pdwElem1[2] == pdwElem2[2] && pdwElem1[1] < pdwElem2[1]) ||
            (pdwElem1[2] == pdwElem2[2] && pdwElem1[1] == pdwElem2[1] && pdwElem1[0] < pdwElem2[0]))
        return -1;
    else
        return 0;
}

/*------------------------------------------------------------------------------------------------
// this is how the end points is going to be rounded in compressed format
------------------------------------------------------------------------------------------------*/
static void MkRmpOnGrid(CODECFLOAT _RmpF[NUM_CHANNELS][NUM_ENDPOINTS], CODECFLOAT _MnMx[NUM_CHANNELS][NUM_ENDPOINTS],
                        CODECFLOAT _Min, CODECFLOAT _Max, CMP_BYTE nRedBits, CMP_BYTE nGreenBits, CMP_BYTE nBlueBits) {
    CODECFLOAT Fctrs0[3];
    CODECFLOAT Fctrs1[3];

    Fctrs1[RC] = (CODECFLOAT)(1 << nRedBits);
    Fctrs1[GC] = (CODECFLOAT)(1 << nGreenBits);
    Fctrs1[BC] = (CODECFLOAT)(1 << nBlueBits);
    Fctrs0[RC] = (CODECFLOAT)(1 << (PIX_GRID-nRedBits));
    Fctrs0[GC] = (CODECFLOAT)(1 << (PIX_GRID-nGreenBits));
    Fctrs0[BC] = (CODECFLOAT)(1 << (PIX_GRID-nBlueBits));

    for(int j = 0; j < 3; j++) {
        for(int k = 0; k < 2; k++) {
            _RmpF[j][k] = floor(_MnMx[j][k]);
            if(_RmpF[j][k] <= _Min)
                _RmpF[j][k] = _Min;
            else {
                _RmpF[j][k] += floor(128.f / Fctrs1[j]) - floor(_RmpF[j][k] / Fctrs1[j]);
                _RmpF[j][k] = std::min(_RmpF[j][k], _Max);
            }

            _RmpF[j][k] = floor(_RmpF[j][k] / Fctrs0[j]) * Fctrs0[j];
        }
    }
}


/*------------------------------------------------------------------------------------------------
// this is how the end points is going to be look like when decompressed
------------------------------------------------------------------------------------------------*/

inline void MkWkRmpPts(bool *_bEq, CODECFLOAT _OutRmpPts[NUM_CHANNELS][NUM_ENDPOINTS],
                       CODECFLOAT _InpRmpPts[NUM_CHANNELS][NUM_ENDPOINTS], CMP_BYTE nRedBits, CMP_BYTE nGreenBits, CMP_BYTE nBlueBits) {
    CODECFLOAT Fctrs[3];
    Fctrs[RC] = (CODECFLOAT)(1 << nRedBits);
    Fctrs[GC] = (CODECFLOAT)(1 << nGreenBits);
    Fctrs[BC] = (CODECFLOAT)(1 << nBlueBits);

    *_bEq = true;
    // find whether input ramp is flat
    for(int j = 0; j < 3; j++)
        *_bEq  &= (_InpRmpPts[j][0] == _InpRmpPts[j][1]);

    // end points on the integer grid
    for(int j = 0; j <3; j++) {
        for(int k = 0; k <2; k++) {
            // Apply the lower bit replication to give full dynamic range
            _OutRmpPts[j][k] = _InpRmpPts[j][k] + floor(_InpRmpPts[j][k] / Fctrs[j]);
            _OutRmpPts[j][k] = std::max(_OutRmpPts[j][k], 0.f);
            _OutRmpPts[j][k] = std::min(_OutRmpPts[j][k], 255.f);
        }
    }
}


DWORD dwRndAmount[] = {0, 0, 0, 0, 1, 1, 2, 2, 3};

/*------------------------------------------------------------------------------------------------
1 DIM ramp
------------------------------------------------------------------------------------------------*/
inline void BldClrRmp(CODECFLOAT _Rmp[MAX_POINTS], CODECFLOAT _InpRmp[NUM_ENDPOINTS], CMP_BYTE dwNumPoints) {
    // linear interpolate end points to get the ramp
    _Rmp[0] = _InpRmp[0];
    _Rmp[dwNumPoints - 1] = _InpRmp[1];
    if(dwNumPoints % 2)
        _Rmp[dwNumPoints] = 1000000.f; // for 3 point ramp; not to select the 4th point as min
    for(int e = 1; e < dwNumPoints - 1; e++)
        _Rmp[e] = floor((_Rmp[0] * (dwNumPoints - 1 - e) + _Rmp[dwNumPoints - 1] * e + dwRndAmount[dwNumPoints])/ (CODECFLOAT)(dwNumPoints - 1));
}

/*------------------------------------------------------------------------------------------------
// build 3D ramp
------------------------------------------------------------------------------------------------*/
inline void BldRmp(CODECFLOAT _Rmp[NUM_CHANNELS][MAX_POINTS], CODECFLOAT _InpRmp[NUM_CHANNELS][NUM_ENDPOINTS],
                   CMP_BYTE dwNumPoints) {
    for(int j = 0; j < 3; j++)
        BldClrRmp(_Rmp[j], _InpRmp[j], dwNumPoints);
}



/*------------------------------------------------------------------------------------------------
Compute cumulative error for the current cluster
------------------------------------------------------------------------------------------------*/
static CODECFLOAT ClstrErr(CODECFLOAT _Blk[MAX_BLOCK][NUM_CHANNELS], CODECFLOAT _Rpt[MAX_BLOCK],
                           CODECFLOAT _Rmp[NUM_CHANNELS][MAX_POINTS], int _NmbClrs, int _blcktp,
                           bool _ConstRamp, CODECFLOAT* _pfWeights) {
    CODECFLOAT fError = 0.f;
    int rmp_l = (_ConstRamp) ? 1 : _blcktp;

    // For each colour in the original block, find the closest cluster
    // and compute the comulative error
    for(int i=0; i<_NmbClrs; i++) {
        CODECFLOAT fShortest = 99999999999.f;

        if(_pfWeights)
            for(int r=0; r < rmp_l; r++) {
                // calculate the distance for each component
                CODECFLOAT fDistance =    (_Blk[i][RC] - _Rmp[RC][r]) * (_Blk[i][RC] - _Rmp[RC][r]) * _pfWeights[0] +
                                          (_Blk[i][GC] - _Rmp[GC][r]) * (_Blk[i][GC] - _Rmp[GC][r]) * _pfWeights[1] +
                                          (_Blk[i][BC] - _Rmp[BC][r]) * (_Blk[i][BC] - _Rmp[BC][r]) * _pfWeights[2];

                if(fDistance < fShortest)
                    fShortest = fDistance;
            } else
            for(int r=0; r < rmp_l; r++) {
                // calculate the distance for each component
                CODECFLOAT fDistance =    (_Blk[i][RC] - _Rmp[RC][r]) * (_Blk[i][RC] - _Rmp[RC][r]) +
                                          (_Blk[i][GC] - _Rmp[GC][r]) * (_Blk[i][GC] - _Rmp[GC][r]) +
                                          (_Blk[i][BC] - _Rmp[BC][r]) * (_Blk[i][BC] - _Rmp[BC][r]);

                if(fDistance < fShortest)
                    fShortest = fDistance;
            }

        // accumulate the error
        fError += fShortest * _Rpt[i];
    }

    return fError;
}

// Compute error and find DXTC indexes for the current cluster
static CODECFLOAT ClstrIntnl(CODECFLOAT _Blk[MAX_BLOCK][NUM_CHANNELS], 
                             CMP_BYTE* _Indxs,
                             CODECFLOAT _Rmp[NUM_CHANNELS][MAX_POINTS], 
                             int dwBlockSize, 
                             CMP_BYTE dwNumPoints,
                             bool _ConstRamp, 
                             CODECFLOAT* _pfWeights, 
                             bool _bUseAlpha) 
{
    CODECFLOAT Err = 0.f;
    CMP_BYTE rmp_l = (_ConstRamp) ? 1 : dwNumPoints;

    // For each colour in the original block assign it
    // to the closest cluster and compute the cumulative error
    for(int i=0; i< dwBlockSize; i++) {
        if(_bUseAlpha && *((DWORD*) &_Blk[i][AC]) == 0)
            _Indxs[i] = dwNumPoints;
        else {
            CODECFLOAT shortest = 99999999999.f;
            CMP_BYTE shortestIndex = 0;
            if(_pfWeights)
                for(CMP_BYTE r=0; r < rmp_l; r++) {
                    // calculate the distance for each component
                    CODECFLOAT distance =    (_Blk[i][RC] - _Rmp[RC][r]) * (_Blk[i][RC] - _Rmp[RC][r]) * _pfWeights[0] +
                                             (_Blk[i][GC] - _Rmp[GC][r]) * (_Blk[i][GC] - _Rmp[GC][r]) * _pfWeights[1] +
                                             (_Blk[i][BC] - _Rmp[BC][r]) * (_Blk[i][BC] - _Rmp[BC][r]) * _pfWeights[2];

                    if(distance < shortest) {
                        shortest = distance;
                        shortestIndex = r;
                    }
                } else
                for(CMP_BYTE r=0; r < rmp_l; r++) {
                    // calculate the distance for each component
                    CODECFLOAT distance =    (_Blk[i][RC] - _Rmp[RC][r]) * (_Blk[i][RC] - _Rmp[RC][r]) +
                                             (_Blk[i][GC] - _Rmp[GC][r]) * (_Blk[i][GC] - _Rmp[GC][r]) +
                                             (_Blk[i][BC] - _Rmp[BC][r]) * (_Blk[i][BC] - _Rmp[BC][r]);

                    if(distance < shortest) {
                        shortest = distance;
                        shortestIndex = r;
                    }
                }

            Err += shortest;

            // We have the index of the best cluster, so assign this in the block
            // Reorder indices to match correct DXTC ordering
            if(shortestIndex == dwNumPoints - 1)
                shortestIndex = 1;
            else if(shortestIndex)
                shortestIndex++;
            _Indxs[i] = shortestIndex;
        }
    }

    return Err;
}

/*------------------------------------------------------------------------------------------------
// input ramp is on the coarse grid
------------------------------------------------------------------------------------------------*/
static CODECFLOAT ClstrBas( CMP_BYTE* _Indxs, 
                            CODECFLOAT _Blk[MAX_BLOCK][NUM_CHANNELS],
                            CODECFLOAT _InpRmp[NUM_CHANNELS][NUM_ENDPOINTS], 
                            int dwBlockSize, 
                            CMP_BYTE dwNumPoints, 
                            CODECFLOAT* _pfWeights,
                            bool _bUseAlpha, 
                            CMP_BYTE nRedBits, 
                            CMP_BYTE nGreenBits, 
                            CMP_BYTE nBlueBits) 
{
    // make ramp endpoints the way they'll going to be decompressed
    bool Eq = true;
    CODECFLOAT InpRmp[NUM_CHANNELS][NUM_ENDPOINTS];
    MkWkRmpPts(&Eq, InpRmp, _InpRmp, nRedBits, nGreenBits, nBlueBits);

    // build ramp as it would be built by decompressor
    CODECFLOAT Rmp[NUM_CHANNELS][MAX_POINTS];
    BldRmp(Rmp, InpRmp, dwNumPoints);

    // clusterize and find a cumulative error
    return ClstrIntnl(_Blk, _Indxs, Rmp, dwBlockSize, dwNumPoints, Eq,  _pfWeights, _bUseAlpha);
}

/*------------------------------------------------------------------------------------------------
Clusterization the way it looks from the DXTC decompressor
------------------------------------------------------------------------------------------------*/

CODECFLOAT Clstr(CMP_DWORD block_32[MAX_BLOCK], 
                 CMP_WORD dwBlockSize,
                 CMP_BYTE nEndpoints[3][NUM_ENDPOINTS],
                 CMP_BYTE* pcIndices, 
                 CMP_BYTE dwNumPoints,
                 CODECFLOAT* _pfWeights, 
                 bool _bUseAlpha, 
                 CMP_BYTE _nAlphaThreshold,
                 CMP_BYTE nRedBits, 
                 CMP_BYTE nGreenBits, 
                 CMP_BYTE nBlueBits)
{
    unsigned int c0 = ConstructColor(nEndpoints[RC][0], nRedBits, nEndpoints[GC][0], nGreenBits, nEndpoints[BC][0], nBlueBits);
    unsigned int c1 = ConstructColor(nEndpoints[RC][1], nRedBits, nEndpoints[GC][1], nGreenBits, nEndpoints[BC][1], nBlueBits);
    unsigned int nEndpointIndex0 = 0;
    unsigned int nEndpointIndex1 = 1;
    if((!(dwNumPoints & 0x1) && c0 <= c1) || ((dwNumPoints & 0x1) && c0 > c1)) {
        nEndpointIndex0 = 1;
        nEndpointIndex1 = 0;
    }

    CODECFLOAT InpRmp[NUM_CHANNELS][NUM_ENDPOINTS];
    InpRmp[RC][0] = (CODECFLOAT)nEndpoints[RC][nEndpointIndex0];
    InpRmp[RC][1] = (CODECFLOAT)nEndpoints[RC][nEndpointIndex1];
    InpRmp[GC][0] = (CODECFLOAT)nEndpoints[GC][nEndpointIndex0];
    InpRmp[GC][1] = (CODECFLOAT)nEndpoints[GC][nEndpointIndex1];
    InpRmp[BC][0] = (CODECFLOAT)nEndpoints[BC][nEndpointIndex0];
    InpRmp[BC][1] = (CODECFLOAT)nEndpoints[BC][nEndpointIndex1];

    DWORD dwAlphaThreshold = _nAlphaThreshold << 24;
    CODECFLOAT Blk[MAX_BLOCK][NUM_CHANNELS];
    for(int i = 0; i < dwBlockSize; i++) {
        Blk[i][RC] = (CODECFLOAT)((block_32[i] & 0xff0000) >> 16);
        Blk[i][GC] = (CODECFLOAT)((block_32[i] & 0xff00) >> 8);
        Blk[i][BC] = (CODECFLOAT)(block_32[i] & 0xff);
        if(_bUseAlpha)
            Blk[i][AC] = ((block_32[i] & 0xff000000) >= dwAlphaThreshold) ? 1.f : 0.f;
    }

    return ClstrBas(pcIndices, Blk, InpRmp, dwBlockSize, dwNumPoints, _pfWeights, _bUseAlpha, nRedBits, nGreenBits, nBlueBits);
}

CODECFLOAT Clstr(CODECFLOAT block_32[MAX_BLOCK*4], CMP_WORD dwBlockSize,
                 CMP_BYTE nEndpoints[3][NUM_ENDPOINTS],
                 CMP_BYTE* pcIndices, CMP_BYTE dwNumPoints,
                 CODECFLOAT* _pfWeights, bool _bUseAlpha, CODECFLOAT _fAlphaThreshold,
                 CMP_BYTE nRedBits, CMP_BYTE nGreenBits, CMP_BYTE nBlueBits) {
    unsigned int c0 = ConstructColor(nEndpoints[RC][0], nRedBits, nEndpoints[GC][0], nGreenBits, nEndpoints[BC][0], nBlueBits);
    unsigned int c1 = ConstructColor(nEndpoints[RC][1], nRedBits, nEndpoints[GC][1], nGreenBits, nEndpoints[BC][1], nBlueBits);
    unsigned int nEndpointIndex0 = 0;
    unsigned int nEndpointIndex1 = 1;
    if((!(dwNumPoints & 0x1) && c0 <= c1) || ((dwNumPoints & 0x1) && c0 > c1)) {
        nEndpointIndex0 = 1;
        nEndpointIndex1 = 0;
    }

    CODECFLOAT InpRmp[NUM_CHANNELS][NUM_ENDPOINTS];
    InpRmp[RC][0] = (CODECFLOAT)nEndpoints[RC][nEndpointIndex0];
    InpRmp[RC][1] = (CODECFLOAT)nEndpoints[RC][nEndpointIndex1];
    InpRmp[GC][0] = (CODECFLOAT)nEndpoints[GC][nEndpointIndex0];
    InpRmp[GC][1] = (CODECFLOAT)nEndpoints[GC][nEndpointIndex1];
    InpRmp[BC][0] = (CODECFLOAT)nEndpoints[BC][nEndpointIndex0];
    InpRmp[BC][1] = (CODECFLOAT)nEndpoints[BC][nEndpointIndex1];

    CODECFLOAT fAlphaThreshold = _fAlphaThreshold * 255.f;
    CODECFLOAT Blk[MAX_BLOCK][NUM_CHANNELS];
    for(int i = 0; i < dwBlockSize; i++) {
        Blk[i][RC] = block_32[(i * 4) + 2];
        Blk[i][GC] = block_32[(i * 4) + 1];
        Blk[i][BC] = block_32[(i * 4)];
        if(_bUseAlpha)
            Blk[i][AC] = (block_32[(i * 4) + 3] >= fAlphaThreshold) ? 1.f : 0.f;
    }

    return ClstrBas(pcIndices, Blk, InpRmp, dwBlockSize, dwNumPoints, _pfWeights, _bUseAlpha, nRedBits, nGreenBits, nBlueBits);
}

static CODECFLOAT Refine(CODECFLOAT _OutRmpPnts[NUM_CHANNELS][NUM_ENDPOINTS],
                         CODECFLOAT _InpRmpPnts[NUM_CHANNELS][NUM_ENDPOINTS],
                         CODECFLOAT _Blk[MAX_BLOCK][NUM_CHANNELS], CODECFLOAT _Rpt[MAX_BLOCK],
                         int _NmrClrs, CMP_BYTE dwNumPoints, CODECFLOAT* _pfWeights,
                         CMP_BYTE nRedBits, CMP_BYTE nGreenBits, CMP_BYTE nBlueBits, CMP_BYTE nRefineSteps);

static CODECFLOAT Refine3D(CODECFLOAT _OutRmpPnts[NUM_CHANNELS][NUM_ENDPOINTS],
                           CODECFLOAT _InpRmpPnts[NUM_CHANNELS][NUM_ENDPOINTS],
                           CODECFLOAT _Blk[MAX_BLOCK][NUM_CHANNELS], CODECFLOAT _Rpt[MAX_BLOCK],
                           int _NmrClrs, CMP_BYTE dwNumPoints, CODECFLOAT* _pfWeights,
                           CMP_BYTE nRedBits, CMP_BYTE nGreenBits, CMP_BYTE nBlueBits, CMP_BYTE nRefineSteps);

/*------------------------------------------------------------------------------------------------
1 dim error
------------------------------------------------------------------------------------------------*/
static CODECFLOAT RampSrchW(CODECFLOAT Prj[MAX_BLOCK],
                            CODECFLOAT PrjErr[MAX_BLOCK],
                            CODECFLOAT PreMRep[MAX_BLOCK],
                            CODECFLOAT StepErr, 
                            CODECFLOAT lowPosStep, 
                            CODECFLOAT highPosStep,
                            int dwUniqueColors,
                            int dwNumPoints) {
    CODECFLOAT error = 0;
    CODECFLOAT step = (highPosStep - lowPosStep) / (dwNumPoints - 1);
    CODECFLOAT step_h = step * (CODECFLOAT)0.5;
    CODECFLOAT rstep = (CODECFLOAT)1.0f / step;

    for(int i=0; i < dwUniqueColors; i++) {
        CODECFLOAT v;
        // Work out which value in the block this select
        CODECFLOAT del;

        if((del = Prj[i] - lowPosStep) <= 0)
            v = lowPosStep;
        else if(Prj[i] -  highPosStep >= 0)
            v = highPosStep;
        else
            v = floor((del + step_h) * rstep) * step + lowPosStep;

        // And accumulate the error
        CODECFLOAT d = (Prj[i] - v);
        d *= d;
        CODECFLOAT err = PreMRep[i] * d + PrjErr[i];
        error += err;
        if(StepErr < error) {
            error  = StepErr;
            break;
        }
    }
    return error;
}

// Find the first approximation of the line
// Assume there is a linear relation
//   Z = a * X_In
//   Z = b * Y_In
// Find a,b to minimize MSE between Z and Z_In
static void FindAxis(CODECFLOAT BlkSh[MAX_BLOCK][NUM_CHANNELS],     // output
                     CODECFLOAT LineDir0[NUM_CHANNELS],
                     CODECFLOAT fBlockCenter[NUM_CHANNELS], 
                     bool       *AxisIsSmall, 
                     CODECFLOAT BlkUV[MAX_BLOCK][NUM_CHANNELS],
                     CODECFLOAT _inpRpt[MAX_BLOCK], 
                    int         nDimensions, 
                    int         dwUniqueColors) 
{
    CODECFLOAT Crrl[NUM_CHANNELS];
    CODECFLOAT RGB2[NUM_CHANNELS];

    LineDir0[0] = LineDir0[1] = LineDir0[2] = RGB2[0] = RGB2[1] = RGB2[2] =
    Crrl[0] = Crrl[1] = Crrl[2] = fBlockCenter[0] = fBlockCenter[1] = fBlockCenter[2] = 0.f;

    // sum position of all points
    CODECFLOAT fNumPoints = 0.f;
    for(int i=0; i < dwUniqueColors; i++) {
        fBlockCenter[0] += BlkUV[i][0] * _inpRpt[i];
        fBlockCenter[1] += BlkUV[i][1] * _inpRpt[i];
        fBlockCenter[2] += BlkUV[i][2] * _inpRpt[i];
        fNumPoints += _inpRpt[i];
    }

    // and then average to calculate center coordinate of block
    fBlockCenter[0] /= fNumPoints;
    fBlockCenter[1] /= fNumPoints;
    fBlockCenter[2] /= fNumPoints;

    for(int i = 0; i < dwUniqueColors; i++) {
        // calculate output block as offsets around block center
        BlkSh[i][0] = BlkUV[i][0] - fBlockCenter[0];
        BlkSh[i][1] = BlkUV[i][1] - fBlockCenter[1];
        BlkSh[i][2] = BlkUV[i][2] - fBlockCenter[2];

        // compute correlation matrix
        // RGB2 = sum of ((distance from point from center) squared)
        // Crrl = ???????. Seems to be be some calculation based on distance from point center in two dimensions
        for(int j = 0; j < nDimensions; j++) {
            RGB2[j] += BlkSh[i][j] * BlkSh[i][j] * _inpRpt[i];
            Crrl[j] += BlkSh[i][j] * BlkSh[i][(j+1)%3] * _inpRpt[i];
        }
    }

    // if set's diameter is small
    int i0 = 0, i1 = 1;
    CODECFLOAT mxRGB2 = 0.f;
    int k = 0, j = 0;
    CODECFLOAT fEPS = fNumPoints * EPS;
    for(k = 0, j = 0; j < 3; j++) {
        if(RGB2[j] >= fEPS)
            k++;
        else
            RGB2[j] = 0.f;

        if(mxRGB2 < RGB2[j]) {
            mxRGB2 = RGB2[j];
            i0 = j;
        }
    }

    CODECFLOAT fEPS2 = fNumPoints * EPS2;
    *AxisIsSmall = true;
    for(j = 0; j < 3; j++)
        *AxisIsSmall &= (RGB2[j] < fEPS2);

    if(*AxisIsSmall) // all are very small to avoid division on the small determinant
        return;

    if(k == 1) // really only 1 dimension
        LineDir0[i0]= 1.;
    else if(k == 2) { // really only 2 dimensions
        i1 = (RGB2[(i0+1)%3] > 0.f) ? (i0+1)%3 : (i0+2)%3;
        CODECFLOAT Crl = (i1 == (i0+1)%3) ? Crrl[i0] : Crrl[(i0+2)%3];
        LineDir0[i1] = Crl/ RGB2[i0];
        LineDir0[i0]= 1.;
    } else {
        CODECFLOAT maxDet = 100000.f;
        CODECFLOAT Cs[3];
        // select max det for precision
        for(j = 0; j < nDimensions; j++) {
            CODECFLOAT Det = RGB2[j] * RGB2[(j+1)%3] - Crrl[j] * Crrl[j];
            Cs[j] = abs(Crrl[j]/sqrt(RGB2[j] * RGB2[(j+1)%3]));
            if(maxDet < Det) {
                maxDet = Det;
                i0 = j;
            }
        }

        // inverse correl matrix
        //  --      --       --      --
        //  |  A   B |       |  C  -B |
        //  |  B   C |  =>   | -B   A |
        //  --      --       --     --
        CODECFLOAT mtrx1[2][2];
        CODECFLOAT vc1[2];
        CODECFLOAT vc[2];
        vc1[0] = Crrl[(i0 + 2) %3];
        vc1[1] = Crrl[(i0 + 1) %3];
        // C
        mtrx1[0][0] = RGB2[(i0+1)%3];
        // A
        mtrx1[1][1] = RGB2[i0];
        // -B
        mtrx1[1][0] = mtrx1[0][1] = -Crrl[i0];
        // find a solution
        vc[0] = mtrx1[0][0] * vc1[0] + mtrx1[0][1] * vc1[1];
        vc[1] = mtrx1[1][0] * vc1[0] + mtrx1[1][1] * vc1[1];
        // normalize
        vc[0] /= maxDet;
        vc[1] /= maxDet;
        // find a line direction vector
        LineDir0[i0] = 1.;
        LineDir0[(i0 + 1) %3] = 1.;
        LineDir0[(i0 + 2) %3] = vc[0] + vc[1];
    }

    // normalize direction vector
    CODECFLOAT Len = LineDir0[0] * LineDir0[0] + LineDir0[1] * LineDir0[1] + LineDir0[2] * LineDir0[2];
    Len = sqrt(Len);

    for(j = 0; j < 3; j++)
        LineDir0[j] = (Len > 0.f) ? LineDir0[j] / Len : 0.f;
}

/*-------------------------------------------------------------------------------------------

This C version. For more performance
--------------------------------------------------------------------------------------------*/
#define SCH_STPS 3 // number of search steps to make at each end of interval

// coefficient defining a number of grid units in one search step
#ifndef USE_CMP_CORE_API
static const CODECFLOAT sMvF[] = { 0.f, -1.f, 1.f, -2.f, 2.f, -3.f, 3.f, -4.f, 4.f, -5.f, 5.f, -6.f, 6.f, -7.f, 7.f, -8.f, 8.f};
#endif

CODECFLOAT Refine(CODECFLOAT _OutRmpPnts[NUM_CHANNELS][NUM_ENDPOINTS],
                  CODECFLOAT _InpRmpPnts[NUM_CHANNELS][NUM_ENDPOINTS],
                  CODECFLOAT _Blk[MAX_BLOCK][NUM_CHANNELS], 
                  CODECFLOAT _Rpt[MAX_BLOCK],
                  int _NmrClrs, 
                  CMP_BYTE dwNumPoints, 
                  CODECFLOAT* _pfWeights,
                  CMP_BYTE nRedBits, 
                  CMP_BYTE nGreenBits, 
                  CMP_BYTE nBlueBits, 
                  CMP_BYTE nRefineSteps )
{
    ALIGN_16 CODECFLOAT Rmp[NUM_CHANNELS][MAX_POINTS];

    if (nRefineSteps == 0) nRefineSteps = 1;

    CODECFLOAT Blk[MAX_BLOCK][NUM_CHANNELS];
    for(int i = 0; i < _NmrClrs; i++)
        for(int j = 0; j < 3; j++)
            Blk[i][j] = _Blk[i][j];

    CODECFLOAT fWeightRed = _pfWeights ? _pfWeights[0] : 1.f;
    CODECFLOAT fWeightGreen = _pfWeights ? _pfWeights[1] : 1.f;
    CODECFLOAT fWeightBlue = _pfWeights ? _pfWeights[2] : 1.f;

    // here is our grid
    CODECFLOAT Fctrs[3];
    Fctrs[RC] = (CODECFLOAT)(1 << (PIX_GRID-nRedBits));
    Fctrs[GC] = (CODECFLOAT)(1 << (PIX_GRID-nGreenBits));
    Fctrs[BC] = (CODECFLOAT)(1 << (PIX_GRID-nBlueBits));

    CODECFLOAT InpRmp0[NUM_CHANNELS][NUM_ENDPOINTS];
    CODECFLOAT InpRmp[NUM_CHANNELS][NUM_ENDPOINTS];
    for(int k = 0; k < 2; k++)
        for(int j = 0; j < 3; j++)
            InpRmp0[j][k] = InpRmp[j][k] = _OutRmpPnts[j][k] = _InpRmpPnts[j][k];

    // make ramp endpoints the way they'll going to be decompressed
    // plus check whether the ramp is flat
    bool Eq;
    CODECFLOAT WkRmpPts[NUM_CHANNELS][NUM_ENDPOINTS];
    MkWkRmpPts(&Eq, WkRmpPts, InpRmp, nRedBits, nGreenBits, nBlueBits);

    // build ramp for all 3 colors
    BldRmp(Rmp, WkRmpPts, dwNumPoints);

    // clusterize for the current ramp
    CODECFLOAT bestE = ClstrErr(Blk, _Rpt, Rmp, _NmrClrs, dwNumPoints, Eq, _pfWeights);
    if(bestE == 0.f)    // if exact, we've done
        return bestE;

    // Tweak each component in isolation and get the best values

    // precompute ramp errors for Green and Blue
    CODECFLOAT RmpErr[MAX_POINTS][MAX_BLOCK];
    for(int i=0; i < _NmrClrs; i++) {
        for(int r = 0; r < dwNumPoints; r++) {
            CODECFLOAT DistG = (Rmp[GC][r] - Blk[i][GC]);
            CODECFLOAT DistB = (Rmp[BC][r] - Blk[i][BC]);
            RmpErr[r][i] = DistG * DistG * fWeightGreen + DistB * DistB * fWeightBlue;
        }
    }

    // First Red
    CODECFLOAT bstC0 = InpRmp0[RC][0];
    CODECFLOAT bstC1 = InpRmp0[RC][1];
    int nRefineStart = 0 - (std::min(nRefineSteps, (CMP_BYTE)8));
    int nRefineEnd =std::min(nRefineSteps, (CMP_BYTE)8);
    for(int i = nRefineStart; i <= nRefineEnd; i++) {
        for(int j = nRefineStart; j <= nRefineEnd; j++) {
            // make a move; both sides of interval.
            InpRmp[RC][0] = std::min(std::max(InpRmp0[RC][0] + i * Fctrs[RC], 0.f), 255.f);
            InpRmp[RC][1] = std::min(std::max(InpRmp0[RC][1] + j * Fctrs[RC], 0.f), 255.f);

            // make ramp endpoints the way they'll going to be decompressed
            // plus check whether the ramp is flat
            MkWkRmpPts(&Eq, WkRmpPts, InpRmp, nRedBits, nGreenBits, nBlueBits);

            // build ramp only for red
            BldClrRmp(Rmp[RC], WkRmpPts[RC], dwNumPoints);

            // compute cumulative error
            CODECFLOAT mse = 0.f;
            int rmp_l = (Eq) ? 1 : dwNumPoints;
            for(int k = 0; k < _NmrClrs; k++) {
                CODECFLOAT MinErr = 10000000.f;
                for(int r = 0; r < rmp_l; r++) {
                    CODECFLOAT Dist = (Rmp[RC][r] - Blk[k][RC]);
                    CODECFLOAT Err = RmpErr[r][k] + Dist * Dist * fWeightRed;
                    MinErr = std::min(MinErr, Err);
                }
                mse += MinErr * _Rpt[k];
            }

            // save if we achieve better result
            if(mse < bestE) {
                bstC0 = InpRmp[RC][0];
                bstC1 = InpRmp[RC][1];
                bestE = mse;
            }
        }
    }

    // our best REDs
    InpRmp[RC][0] = bstC0;
    InpRmp[RC][1] = bstC1;

    // make ramp endpoints the way they'll going to be decompressed
    // plus check whether the ramp is flat
    MkWkRmpPts(&Eq, WkRmpPts, InpRmp, nRedBits, nGreenBits, nBlueBits);

    // build ramp only for green
    BldRmp(Rmp, WkRmpPts, dwNumPoints);

    // precompute ramp errors for Red and Blue
    for(int i=0; i < _NmrClrs; i++) {
        for(int r = 0; r < dwNumPoints; r++) {
            CODECFLOAT DistR = (Rmp[RC][r] - Blk[i][RC]);
            CODECFLOAT DistB = (Rmp[BC][r] - Blk[i][BC]);
            RmpErr[r][i] = DistR * DistR * fWeightRed + DistB * DistB * fWeightBlue;
        }
    }

    // Now green
    bstC0 = InpRmp0[GC][0];
    bstC1 = InpRmp0[GC][1];
    for(int i = nRefineStart; i <= nRefineEnd; i++) {
        for(int j = nRefineStart; j <= nRefineEnd; j++) {
            InpRmp[GC][0] = std::min(std::max(InpRmp0[GC][0] + i * Fctrs[GC], 0.f), 255.f);
            InpRmp[GC][1] = std::min(std::max(InpRmp0[GC][1] + j * Fctrs[GC], 0.f), 255.f);

            MkWkRmpPts(&Eq, WkRmpPts, InpRmp, nRedBits, nGreenBits, nBlueBits);
            BldClrRmp(Rmp[GC], WkRmpPts[GC], dwNumPoints);

            CODECFLOAT mse = 0.f;
            int rmp_l = (Eq) ? 1 : dwNumPoints;
            for(int k = 0; k < _NmrClrs; k++) {
                CODECFLOAT MinErr = 10000000.f;
                for(int r = 0; r < rmp_l; r++) {
                    CODECFLOAT Dist = (Rmp[GC][r] - Blk[k][GC]);
                    CODECFLOAT Err = RmpErr[r][k] +  Dist * Dist * fWeightGreen;
                    MinErr = std::min(MinErr, Err);
                }
                mse += MinErr * _Rpt[k];
            }

            if(mse < bestE) {
                bstC0 = InpRmp[GC][0];
                bstC1 = InpRmp[GC][1];
                bestE = mse;
            }
        }
    }

    // our best GREENs
    InpRmp[GC][0] = bstC0;
    InpRmp[GC][1] = bstC1;

    MkWkRmpPts(&Eq, WkRmpPts, InpRmp, nRedBits, nGreenBits, nBlueBits);
    BldRmp(Rmp, WkRmpPts, dwNumPoints);

    // ramp err for Red and Green
    for(int i=0; i < _NmrClrs; i++) {
        for(int r = 0; r < dwNumPoints; r++) {
            CODECFLOAT DistR = (Rmp[RC][r] - Blk[i][RC]);
            CODECFLOAT DistG = (Rmp[GC][r] - Blk[i][GC]);
            RmpErr[r][i] = DistR * DistR * fWeightRed + DistG * DistG * fWeightGreen;
        }
    }

    bstC0 = InpRmp0[BC][0];
    bstC1 = InpRmp0[BC][1];
    // Now blue
    for(int i = nRefineStart; i <= nRefineEnd; i++) {
        for(int j = nRefineStart; j <= nRefineEnd; j++) {
            InpRmp[BC][0] = min(max(InpRmp0[BC][0] + i * Fctrs[BC], 0.f), 255.f);
            InpRmp[BC][1] = min(max(InpRmp0[BC][1] + j * Fctrs[BC], 0.f), 255.f);

            MkWkRmpPts(&Eq, WkRmpPts, InpRmp, nRedBits, nGreenBits, nBlueBits);
            BldClrRmp(Rmp[BC], WkRmpPts[BC], dwNumPoints);

            CODECFLOAT mse = 0.f;
            int rmp_l = (Eq) ? 1 : dwNumPoints;
            for(int k = 0; k < _NmrClrs; k++) {
                CODECFLOAT MinErr = 10000000.f;
                for(int r = 0; r < rmp_l; r++) {
                    CODECFLOAT Dist = (Rmp[BC][r] - Blk[k][BC]);
                    CODECFLOAT Err = RmpErr[r][k] +  Dist * Dist * fWeightBlue;
                    MinErr = min(MinErr, Err);
                }
                mse += MinErr * _Rpt[k];
            }

            if(mse < bestE) {
                bstC0 = InpRmp[BC][0];
                bstC1 = InpRmp[BC][1];
                bestE = mse;
            }
        }
    }

    // our best BLUEs
    InpRmp[BC][0] = bstC0;
    InpRmp[BC][1] = bstC1;

    // return our best choice
    for(int j = 0; j < 3; j++)
        for(int k = 0; k < 2; k++)
            _OutRmpPnts[j][k] = InpRmp[j][k];

    return bestE;
}

CODECFLOAT Refine3D(CODECFLOAT _OutRmpPnts[NUM_CHANNELS][NUM_ENDPOINTS],
                    CODECFLOAT _InpRmpPnts[NUM_CHANNELS][NUM_ENDPOINTS],
                    CODECFLOAT _Blk[MAX_BLOCK][NUM_CHANNELS], 
                    CODECFLOAT _Rpt[MAX_BLOCK],
                    int _NmrClrs, 
                    CMP_BYTE dwNumPoints, 
                    CODECFLOAT* _pfWeights,
                    CMP_BYTE nRedBits, 
                    CMP_BYTE nGreenBits, 
                    CMP_BYTE nBlueBits, 
                    CMP_BYTE nRefineSteps)
{
    ALIGN_16 CODECFLOAT Rmp[NUM_CHANNELS][MAX_POINTS];

    CODECFLOAT Blk[MAX_BLOCK][NUM_CHANNELS];
    for(int i = 0; i < _NmrClrs; i++)
        for(int j = 0; j < 3; j++)
            Blk[i][j] = _Blk[i][j];

    CODECFLOAT fWeightRed   = _pfWeights ? _pfWeights[0] : 1.f;
    CODECFLOAT fWeightGreen = _pfWeights ? _pfWeights[1] : 1.f;
    CODECFLOAT fWeightBlue  = _pfWeights ? _pfWeights[2] : 1.f;

    // here is our grid
    CODECFLOAT Fctrs[3];
    Fctrs[RC] = (CODECFLOAT)(1 << (PIX_GRID-nRedBits));
    Fctrs[GC] = (CODECFLOAT)(1 << (PIX_GRID-nGreenBits));
    Fctrs[BC] = (CODECFLOAT)(1 << (PIX_GRID-nBlueBits));

    CODECFLOAT InpRmp0[NUM_CHANNELS][NUM_ENDPOINTS];
    CODECFLOAT InpRmp[NUM_CHANNELS][NUM_ENDPOINTS];
    for(int k = 0; k < 2; k++)
        for(int j = 0; j < 3; j++)
            InpRmp0[j][k] = InpRmp[j][k] = _OutRmpPnts[j][k] = _InpRmpPnts[j][k];

    // make ramp endpoints the way they'll going to be decompressed
    // plus check whether the ramp is flat
    bool Eq;
    CODECFLOAT WkRmpPts[NUM_CHANNELS][NUM_ENDPOINTS];
    MkWkRmpPts(&Eq, WkRmpPts, InpRmp, nRedBits, nGreenBits, nBlueBits);

    // build ramp for all 3 colors
    BldRmp(Rmp, WkRmpPts, dwNumPoints);

    // clusterize for the current ramp
    CODECFLOAT bestE = ClstrErr(Blk, _Rpt, Rmp, _NmrClrs, dwNumPoints, Eq, _pfWeights);
    if(bestE == 0.f || !nRefineSteps)    // if exact, we've done
        return bestE;

    // Jitter endpoints in each direction
    int nRefineStart = 0 - (min(nRefineSteps, (CMP_BYTE)8));
    int nRefineEnd = min(nRefineSteps, (CMP_BYTE)8);
    for(int nJitterG0 = nRefineStart; nJitterG0 <= nRefineEnd; nJitterG0++) {
        InpRmp[GC][0] = min(max(InpRmp0[GC][0] + nJitterG0 * Fctrs[GC], 0.f), 255.f);
        for(int nJitterG1 = nRefineStart; nJitterG1 <= nRefineEnd; nJitterG1++) {
            InpRmp[GC][1] = min(max(InpRmp0[GC][1] + nJitterG1 * Fctrs[GC], 0.f), 255.f);
            MkWkRmpPts(&Eq, WkRmpPts, InpRmp, nRedBits, nGreenBits, nBlueBits);
            BldClrRmp(Rmp[GC], WkRmpPts[GC], dwNumPoints);

            CODECFLOAT RmpErrG[MAX_POINTS][MAX_BLOCK];
            for(int i=0; i < _NmrClrs; i++) {
                for(int r = 0; r < dwNumPoints; r++) {
                    CODECFLOAT DistG = (Rmp[GC][r] - Blk[i][GC]);
                    RmpErrG[r][i] = DistG * DistG * fWeightGreen;
                }
            }

            for(int nJitterB0 = nRefineStart; nJitterB0 <= nRefineEnd; nJitterB0++) {
                InpRmp[BC][0] = min(max(InpRmp0[BC][0] + nJitterB0 * Fctrs[BC], 0.f), 255.f);
                for(int nJitterB1 = nRefineStart; nJitterB1 <= nRefineEnd; nJitterB1++) {
                    InpRmp[BC][1] = min(max(InpRmp0[BC][1] + nJitterB1 * Fctrs[BC], 0.f), 255.f);
                    MkWkRmpPts(&Eq, WkRmpPts, InpRmp, nRedBits, nGreenBits, nBlueBits);
                    BldClrRmp(Rmp[BC], WkRmpPts[BC], dwNumPoints);

                    CODECFLOAT RmpErr[MAX_POINTS][MAX_BLOCK];
                    for(int i=0; i < _NmrClrs; i++) {
                        for(int r = 0; r < dwNumPoints; r++) {
                            CODECFLOAT DistB = (Rmp[BC][r] - Blk[i][BC]);
                            RmpErr[r][i] = RmpErrG[r][i] + DistB * DistB * fWeightBlue;
                        }
                    }

                    for(int nJitterR0 = nRefineStart; nJitterR0 <= nRefineEnd; nJitterR0++) {
                        InpRmp[RC][0] = min(max(InpRmp0[RC][0] + nJitterR0 * Fctrs[RC], 0.f), 255.f);
                        for(int nJitterR1 = nRefineStart; nJitterR1 <= nRefineEnd; nJitterR1++) {
                            InpRmp[RC][1] = min(max(InpRmp0[RC][1] + nJitterR1 * Fctrs[RC], 0.f), 255.f);
                            MkWkRmpPts(&Eq, WkRmpPts, InpRmp, nRedBits, nGreenBits, nBlueBits);
                            BldClrRmp(Rmp[RC], WkRmpPts[RC], dwNumPoints);

                            // compute cumulative error
                            CODECFLOAT mse = 0.f;
                            int rmp_l = (Eq) ? 1 : dwNumPoints;
                            for(int k = 0; k < _NmrClrs; k++) {
                                CODECFLOAT MinErr = 10000000.f;
                                for(int r = 0; r < rmp_l; r++) {
                                    CODECFLOAT Dist = (Rmp[RC][r] - Blk[k][RC]);
                                    CODECFLOAT Err = RmpErr[r][k] + Dist * Dist * fWeightRed;
                                    MinErr = min(MinErr, Err);
                                }
                                mse += MinErr * _Rpt[k];
                            }

                            // save if we achieve better result
                            if(mse < bestE) {
                                bestE = mse;
                                for(int k = 0; k < 2; k++)
                                    for(int j = 0; j < 3; j++)
                                        _OutRmpPnts[j][k] = InpRmp[j][k];
                            }
                        }
                    }
                }
            }
        }
    }

    return bestE;
}

//    This is a float point-based compression
//    it assumes that the number of unique colors is already known; input is in [0., 255.] range.
//    This is C version.
static bool CompressRGBBlockX(CODECFLOAT    _RsltRmpPnts[NUM_CHANNELS][NUM_ENDPOINTS],
                              CODECFLOAT    src_image[MAX_BLOCK][NUM_CHANNELS],
                              CODECFLOAT    Rpt[MAX_BLOCK],
                              int           dwUniqueColors,
                              CMP_BYTE      dwNumPoints, 
                              bool          b3DRefinement, 
                              CMP_BYTE      nRefinementSteps,
                              CODECFLOAT*   _pfWeights,
                              CMP_BYTE      nRedBits, 
                              CMP_BYTE      nGreenBits, 
                              CMP_BYTE      nBlueBits,
                              CODECFLOAT    fquality
                              ) 
{
    ALIGN_16 CODECFLOAT Prj0[MAX_BLOCK];
    ALIGN_16 CODECFLOAT Prj[MAX_BLOCK];
    ALIGN_16 CODECFLOAT PrjErr[MAX_BLOCK];
    ALIGN_16 CODECFLOAT LineDir[NUM_CHANNELS];
    ALIGN_16 CODECFLOAT RmpIndxs[MAX_BLOCK];
#ifdef USE_CMP_CORE_API
    CMP_UNUSED(fquality);
    CMP_UNUSED(b3DRefinement);
#endif
    CODECFLOAT LineDirG[NUM_CHANNELS];
    CODECFLOAT PosG[NUM_ENDPOINTS];
    CODECFLOAT BlkUV[MAX_BLOCK][NUM_CHANNELS];
    CODECFLOAT BlkSh[MAX_BLOCK][NUM_CHANNELS];
    CODECFLOAT LineDir0[NUM_CHANNELS];
    CODECFLOAT Mdl[NUM_CHANNELS];

    CODECFLOAT rsltC[NUM_CHANNELS][NUM_ENDPOINTS];
    int i, j, k;

    // down to [0., 1.]
    for(i = 0; i < dwUniqueColors; i++)
        for(j = 0; j < 3; j++)
            BlkUV[i][j] = src_image[i][j] / 255.f;

    bool isDONE         = false;

    // as usual if not more then 2 different colors, we've done
    if(dwUniqueColors <= 2) {
        for(j = 0; j < 3; j++) {
            rsltC[j][0] = src_image[0][j];
            rsltC[j][1] = src_image[dwUniqueColors - 1][j];
        }
        isDONE = true;
    }

    if ( !isDONE ) {
        //    This is our first attempt to find an axis we will go along.
        //    The cumulation is done to find a line minimizing the MSE from the input 3D points.
        bool bSmall = true;
        FindAxis(BlkSh, LineDir0, Mdl, &bSmall, BlkUV, Rpt, 3, dwUniqueColors);

        //    While trying to find the axis we found that the diameter of the input set is quite small.
        //    Do not bother.
        if(bSmall) {
            for(j = 0; j < 3; j++) {
                rsltC[j][0] = src_image[0][j];
                rsltC[j][1] = src_image[dwUniqueColors - 1][j];
            }
            isDONE = true;
        }
    }

    // GCC is being an awful being when it comes to goto-jumps.
    // So please bear with this.
    if ( !isDONE ) {
        CODECFLOAT ErrG = 10000000.f;
        CODECFLOAT PrjBnd[NUM_ENDPOINTS];
        ALIGN_16 CODECFLOAT PreMRep[MAX_BLOCK];
        for(j =0; j < 3; j++)
            LineDir[j] = LineDir0[j];

       //    Here is the main loop.
        //    1. Project input set on the axis in consideration.
        //    2. Run 1 dimensional search (see scalar case) to find an (sub) optimal pair of end points.
        //    3. Compute the vector of indexes (or clusters) for the current approximate ramp.
        //    4. Present our color channels as 3 16DIM vectors.
        //    5. Find closest approximation of each of 16DIM color vector with the projection of the 16DIM index vector.
        //    6. Plug the projections as a new directional vector for the axis.
        //    7. Goto 1.
        //    D - is 16 dim "index" vector (or 16 DIM vector of indexes - {0, 1/3, 2/3, 0, ...,}, but shifted and normalized).
        //    Ci - is a 16 dim vector of color i.
        //    for each Ci find a scalar Ai such that
        //    (Ai * D - Ci) (Ai * D - Ci) -> min , i.e distance between vector AiD and C is min.
        //    You can think of D as a unit interval(vector) "clusterizer",
        //    and Ai is a scale you need to apply to the clusterizer to
        //    approximate the Ci vector instead of the unit vector.
        //    Solution is
        //    Ai = (D . Ci) / (D . D); . - is a dot product.
        //    in 3 dim space Ai(s) represent a line direction, along which
        //    we again try to find (sub)optimal quantizer.
        
        //    That's what our for(;;) loop is about.
        for(;;) {
            //  1. Project input set on the axis in consideration.
            // From Foley & Van Dam: Closest point of approach of a line (P + v) to a point (R) is
            //                            P + ((R-P).v) / (v.v))v
            // The distance along v is therefore (R-P).v / (v.v)
            // (v.v) is 1 if v is a unit vector.
            //
            PrjBnd[0] = 1000.;
            PrjBnd[1] = -1000.;
            for(i = 0; i < MAX_BLOCK; i++)
                Prj0[i] = Prj[i] = PrjErr[i] = PreMRep[i] = 0.f;

            for(i = 0; i < dwUniqueColors; i++) {
                Prj0[i] = Prj[i] = BlkSh[i][0] * LineDir[0] + BlkSh[i][1] * LineDir[1] + BlkSh[i][2] * LineDir[2];

                PrjErr[i] = (BlkSh[i][0] - LineDir[0] * Prj[i]) * (BlkSh[i][0] - LineDir[0] * Prj[i])
                            + (BlkSh[i][1] - LineDir[1] * Prj[i]) * (BlkSh[i][1] - LineDir[1] * Prj[i])
                            + (BlkSh[i][2] - LineDir[2] * Prj[i]) * (BlkSh[i][2] - LineDir[2] * Prj[i]);

                PrjBnd[0] = min(PrjBnd[0], Prj[i]);
                PrjBnd[1] = max(PrjBnd[1], Prj[i]);
            }

            //  2. Run 1 dimensional search (see scalar case) to find an (sub) optimal pair of end points.

            // min and max of the search interval
            CODECFLOAT Scl[NUM_ENDPOINTS];
            Scl[0] = PrjBnd[0] - (PrjBnd[1] - PrjBnd[0]) * 0.125f;;
            Scl[1] = PrjBnd[1] + (PrjBnd[1] - PrjBnd[0]) * 0.125f;;

#ifdef USE_CMP_CORE_API
            // No range found exit
            if (Scl[0] == Scl[1]) {
                return false;
            }
#endif

            // compute scaling factor to scale down the search interval to [0.,1]
            const CODECFLOAT Scl2 = (Scl[1] - Scl[0]) * (Scl[1] - Scl[0]);
            const CODECFLOAT overScl = 1.f/(Scl[1] - Scl[0]);

            for(i = 0; i < dwUniqueColors; i++) {
                // scale them
                Prj[i] = (Prj[i] - Scl[0]) * overScl;
                // premultiply the scale squire to plug into error computation later
                PreMRep[i] = Rpt[i] * Scl2;
            }

            // scale first approximation of end points
            for(k = 0; k <2; k++)
                PrjBnd[k] = (PrjBnd[k] - Scl[0]) * overScl;

            CODECFLOAT StepErr = MAX_ERROR;

            // search step
            static const CODECFLOAT searchStep = 0.025f;

            // low Start/End; high Start/End
            const CODECFLOAT lowStartEnd  = (PrjBnd[0] - 2.f * searchStep > 0.f) ?  PrjBnd[0] - 2.f * searchStep : 0.f;
            const CODECFLOAT highStartEnd = (PrjBnd[1] + 2.f * searchStep < 1.f) ?  PrjBnd[1] + 2.f * searchStep : 1.f;

            // find the best endpoints
            CODECFLOAT Pos[NUM_ENDPOINTS];
            CODECFLOAT lowPosStep, highPosStep;
            CODECFLOAT err;

            int l, h;
            for(l = 0, lowPosStep = lowStartEnd; l < 8; l++, lowPosStep += searchStep) {
                for(h = 0, highPosStep = highStartEnd; h < 8; h++, highPosStep -= searchStep) {
                    // compute an error for the current pair of end points.
                    err = RampSrchW(Prj, PrjErr, PreMRep, StepErr, lowPosStep, highPosStep, dwUniqueColors, dwNumPoints);

                    if(err < StepErr) {
                        // save better result
                        StepErr = err;
                        Pos[0] = lowPosStep;
                        Pos[1] = highPosStep;
                    }
                }
            }

            // inverse the scaling
            for(k = 0; k < 2; k++)
                Pos[k] = Pos[k] * (Scl[1] - Scl[0])+ Scl[0];

            // did we find somthing better from the previous run?
            if(StepErr + 0.001 < ErrG) {
                // yes, remember it
                ErrG = StepErr;
                LineDirG[0] =  LineDir[0];
                LineDirG[1] =  LineDir[1];
                LineDirG[2] =  LineDir[2];
                PosG[0] = Pos[0];
                PosG[1] = Pos[1];
                //  3. Compute the vector of indexes (or clusters) for the current approximate ramp.
                // indexes
                const CODECFLOAT step = (Pos[1] - Pos[0]) / (CODECFLOAT)(dwNumPoints - 1);
                const CODECFLOAT step_h = step * (CODECFLOAT)0.5;
                const CODECFLOAT rstep = (CODECFLOAT)1.0f / step;
                const CODECFLOAT overBlkTp = 1.f/  (CODECFLOAT)(dwNumPoints - 1) ;

                // here the index vector is computed,
                // shifted and normalized
                CODECFLOAT indxAvrg = (CODECFLOAT)(dwNumPoints - 1) / 2.f;

                for(i=0; i < dwUniqueColors; i++) {
                    CODECFLOAT del;
                    //int n = (int)((b - _min_ex + (step*0.5f)) * rstep);
                    if((del = Prj0[i] - Pos[0]) <= 0)
                        RmpIndxs[i] = 0.f;
                    else if(Prj0[i] -  Pos[1] >= 0)
                        RmpIndxs[i] = (CODECFLOAT)(dwNumPoints - 1);
                    else
                        RmpIndxs[i] = floor((del + step_h) * rstep);
                    // shift and normalization
                    RmpIndxs[i] = (RmpIndxs[i] - indxAvrg) * overBlkTp;
                }

                //  4. Present our color channels as 3 16DIM vectors.
                //  5. Find closest aproximation of each of 16DIM color vector with the pojection of the 16DIM index vector.
                CODECFLOAT Crs[3], Len, Len2;
                for(i = 0, Crs[0] = Crs[1] = Crs[2] = Len = 0.f; i < dwUniqueColors; i++) {
                    const CODECFLOAT PreMlt = RmpIndxs[i] * Rpt[i];
                    Len += RmpIndxs[i] * PreMlt;
                    for(j = 0; j < 3; j++)
                        Crs[j] += BlkSh[i][j] * PreMlt;
                }

                LineDir[0] = LineDir[1] = LineDir[2] = 0.f;
                if(Len > 0.f) {
                    LineDir[0] = Crs[0]/ Len;
                    LineDir[1] = Crs[1]/ Len;
                    LineDir[2] = Crs[2]/ Len;

                    //  6. Plug the projections as a new directional vector for the axis.
                    //  7. Goto 1.
                    Len2 = LineDir[0] * LineDir[0] + LineDir[1] * LineDir[1] + LineDir[2] * LineDir[2];
                    Len2 = sqrt(Len2);

                    LineDir[0] /= Len2;
                    LineDir[1] /= Len2;
                    LineDir[2] /= Len2;
                }
            } else // We was not able to find anything better.  Drop dead.
                break;
        }

        // inverse transform to find end-points of 3-color ramp
        for(k = 0; k < 2; k++)
            for(j = 0; j < 3; j++)
                rsltC[j][k] = (PosG[k] * LineDirG[j]  + Mdl[j]) * 255.f;
    }

    // We've dealt with (almost) unrestricted full precision realm.
    // Now back to the dirty digital world.
    
    // round the end points to make them look like compressed ones
    CODECFLOAT inpRmpEndPts[NUM_CHANNELS][NUM_ENDPOINTS];
    MkRmpOnGrid(inpRmpEndPts, rsltC, 0.f, 255.f, nRedBits, nGreenBits, nBlueBits);

    switch (nRefinementSteps) {
        case 1:
            Refine(_RsltRmpPnts, inpRmpEndPts, src_image, Rpt, dwUniqueColors, dwNumPoints, _pfWeights, nRedBits, nGreenBits, nBlueBits, 3);
            break;
        case 2:
            if (dwUniqueColors > 2)
                Refine3D(_RsltRmpPnts, inpRmpEndPts, src_image, Rpt, dwUniqueColors, dwNumPoints, _pfWeights, nRedBits, nGreenBits, nBlueBits, 1);
            else
                Refine(_RsltRmpPnts, inpRmpEndPts, src_image, Rpt, dwUniqueColors, dwNumPoints, _pfWeights, nRedBits, nGreenBits, nBlueBits, 3);
            break;
        default:
            Refine(_RsltRmpPnts, inpRmpEndPts, src_image, Rpt, dwUniqueColors, dwNumPoints, _pfWeights, nRedBits, nGreenBits, nBlueBits, 1);
            break;
    }
    return true;
}

CODECFLOAT CompRGBBlock(CODECFLOAT* block_32, 
                        CMP_DWORD compressedBlock[2],
                        CMP_WORD dwBlockSize,
                        CMP_BYTE nRedBits,
                        CMP_BYTE nGreenBits, 
                        CMP_BYTE nBlueBits,
                        CMP_BYTE nEndpoints[3][NUM_ENDPOINTS], 
                        CMP_BYTE* pcIndices, 
                        CMP_BYTE dwNumPoints,
                        bool _bUseSSE2, 
                        bool b3DRefinement, 
                        CMP_BYTE nRefinementSteps, 
                        CODECFLOAT* _pfChannelWeights,
                        bool _bUseAlpha, 
                        CODECFLOAT _fAlphaThreshold) 
{
#ifdef USE_CMP_CORE_API
    CMP_UNUSED(_bUseSSE2);
    CMP_UNUSED(compressedBlock);
#endif
    ALIGN_16 CODECFLOAT Rpt[MAX_BLOCK];
    ALIGN_16 CODECFLOAT BlkIn[MAX_BLOCK][NUM_CHANNELS];

    memset(Rpt, 0, sizeof(Rpt));
    memset(BlkIn, 0, sizeof(BlkIn));

    DWORD dwColors = 0;
    CODECFLOAT fBlk[BLOCK_SIZE*4];
    for(DWORD i = 0; i < dwBlockSize; i++)
        if(!_bUseAlpha || (block_32[(i* 4) + 3] >= _fAlphaThreshold)) {
            fBlk[(dwColors* 4) + 0] = block_32[(i*4) + 2];
            fBlk[(dwColors* 4) + 1] = block_32[(i*4) + 1];
            fBlk[(dwColors* 4) + 2] = block_32[(i*4) + 0];
            fBlk[(dwColors* 4) + 3] = 0.f;
            dwColors++;
        }

    // Do we have any colors ?
    if(dwColors) {
        bool bHasAlpha = (dwColors != dwBlockSize);
        if(bHasAlpha && _bUseAlpha && !(dwNumPoints & 0x1))
            return FLT_MAX;

        //  Here we are computing an uniq number of colors.
        //  For each uniq value we compute the number of it appearences.
        qsort((void *)fBlk, (size_t)dwColors, 4 * sizeof(CODECFLOAT), QSortFloatCmp);

        CODECFLOAT new_p[NUM_CHANNELS];
        DWORD dwUniqueColors = 0;
        memcpy(&BlkIn[0], &fBlk[0], 4 * sizeof(CODECFLOAT));
        memcpy(&new_p, &fBlk[0], 4 * sizeof(CODECFLOAT));
        Rpt[dwUniqueColors] = 1.f;
        for(DWORD i = 1; i < dwColors; i++) {
            if(memcmp(&new_p, &fBlk[i*4], 4 * sizeof(CODECFLOAT)) != 0) {
                dwUniqueColors++;
                memcpy(&BlkIn[dwUniqueColors], &fBlk[i*4], 4 * sizeof(CODECFLOAT));
                memcpy(&new_p, &fBlk[i*4], 4 * sizeof(CODECFLOAT));
                Rpt[dwUniqueColors] = 1.f;
            } else
                Rpt[dwUniqueColors] += 1.f;
        }
        dwUniqueColors++;

        // switch to float
        for(DWORD i=0; i < dwUniqueColors; i++)
            for(DWORD j=0; j < 4; j++)
                BlkIn[i][j] *= 255.0;

        CODECFLOAT rsltC[NUM_CHANNELS][NUM_ENDPOINTS];
        CompressRGBBlockX(rsltC, 
                          BlkIn, 
                          Rpt, 
                          dwUniqueColors, 
                          dwNumPoints, 
                          b3DRefinement, 
                          nRefinementSteps,
                          _pfChannelWeights, 
                          nRedBits, 
                          nGreenBits, 
                          nBlueBits,
                          1.0f);

        // return to integer realm
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 2; j++)
                nEndpoints[i][j] =  (CMP_BYTE)rsltC[i][j];

        for(DWORD i = 0; i < dwBlockSize; i++) {
            fBlk[(i* 4) + 0] = block_32[(i*4) + 2] * 255.0f;
            fBlk[(i* 4) + 1] = block_32[(i*4) + 1] * 255.0f;
            fBlk[(i* 4) + 2] = block_32[(i*4) + 0] * 255.0f;
            fBlk[(i* 4) + 3] = block_32[(i*4) + 3] * 255.0f;
        }

        return Clstr(fBlk, dwBlockSize, nEndpoints, pcIndices, dwNumPoints, _pfChannelWeights, _bUseAlpha, _fAlphaThreshold,
                     nRedBits, nGreenBits, nBlueBits);
    } else {
        // All colors transparent
        nEndpoints[0][0] = nEndpoints[1][0] = nEndpoints[2][0] = 0;
        nEndpoints[0][1] = nEndpoints[1][1] = nEndpoints[2][1] = 0xff;
        memset(pcIndices, 0xff, dwBlockSize);
        return 0.0;
    }
}



CODECFLOAT CompRGBBlock(CMP_DWORD* block_32, 
                        CMP_DWORD compressedBlock[2],
                        CMP_WORD dwBlockSize,
                        CMP_BYTE nRedBits, 
                        CMP_BYTE nGreenBits, 
                        CMP_BYTE nBlueBits,
                        CMP_BYTE nEndpoints[3][NUM_ENDPOINTS], 
                        CMP_BYTE* pcIndices, 
                        CMP_BYTE dwNumPoints,
                        bool _bUseSSE2, 
                        bool b3DRefinement, 
                        CMP_BYTE nRefinementSteps, 
                        CODECFLOAT* _pfChannelWeights,
                        bool _bUseAlpha, 
                        CMP_BYTE _nAlphaThreshold)
{
    ALIGN_16 CODECFLOAT Rpt[BLOCK_SIZE];
    ALIGN_16 CODECFLOAT BlkIn[BLOCK_SIZE][NUM_CHANNELS];
#ifdef USE_CMP_CORE_API
    CMP_UNUSED(_bUseSSE2);
#endif

    memset(Rpt, 0, sizeof(Rpt));
    memset(BlkIn, 0, sizeof(BlkIn));
    compressedBlock[0] = 0;

    DWORD dwAlphaThreshold = _nAlphaThreshold << 24;
    DWORD dwColors = 0;
    CMP_DWORD dwBlk[BLOCK_SIZE];
    for(DWORD i = 0; i < dwBlockSize; i++)
        if(!_bUseAlpha || (block_32[i] & 0xff000000) >= dwAlphaThreshold)
            dwBlk[dwColors++] = block_32[i] | 0xff000000;

    // Do we have any colors ?
    static int id=0;
    if(dwColors) {
        bool bHasAlpha = (dwColors != dwBlockSize);
        if(bHasAlpha && _bUseAlpha && !(dwNumPoints & 0x1))
            return FLT_MAX;

        // Here we are computing an unique number of colors.
        // For each unique value we compute the number of it appearences.
        qsort((void *)dwBlk, (size_t)dwColors, sizeof(CMP_DWORD), QSortIntCmp);

        CMP_DWORD new_p;
        CMP_DWORD dwBlkU[BLOCK_SIZE];
        DWORD dwUniqueColors = 0;
        new_p = dwBlkU[0] = dwBlk[0];
        Rpt[dwUniqueColors] = 1.f;
        for(DWORD i = 1; i < dwColors; i++) {
            if(new_p != dwBlk[i]) {
                dwUniqueColors++;
                new_p = dwBlkU[dwUniqueColors] = dwBlk[i];
                Rpt[dwUniqueColors] = 1.f;
            } else
                Rpt[dwUniqueColors] += 1.f;
        }
        dwUniqueColors++;

        // switch to float
        for(DWORD i=0; i<dwUniqueColors; i++) {
            BlkIn[i][RC] = (CODECFLOAT)((dwBlkU[i] >> 16) & 0xff); // R
            BlkIn[i][GC] = (CODECFLOAT)((dwBlkU[i] >> 8)  & 0xff); // G
            BlkIn[i][BC] = (CODECFLOAT)((dwBlkU[i] >> 0)  & 0xff); // B
            BlkIn[i][AC] =  255.f; // A
        }

        CODECFLOAT rsltC[NUM_CHANNELS][NUM_ENDPOINTS];
        if (CompressRGBBlockX(rsltC,                            //  CMP_EndPoints = CompressRGBBlock_Slow2 (
                          BlkIn,                                //  CGU_Vec3f  src_imageNorm[BLOCK_SIZE_4X4]
                          Rpt,                                  //  CGU_FLOAT  Rpt[BLOCK_SIZE_4X4],
                          dwUniqueColors,                       //  CGU_UINT32 dwUniqueColors,
                          dwNumPoints,                          //  CGU_UINT32 dwNumPoints,
                          b3DRefinement,                        //
                          nRefinementSteps,                     //  CGU_UINT32 m_nRefinementSteps,
                          _pfChannelWeights,                    //  CGU_Vec3f  channelWeightsBGR,
                          nRedBits,                             //  );
                          nGreenBits, 
                          nBlueBits,
                          1.0f) )
        {
            // return to integer realm
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 2; j++)
                    nEndpoints[i][j] =  (CMP_BYTE)rsltC[i][j];
            // printf("Endpoints {%3d,%3d,%3d} {%3d,%3d,%3d} ", nEndpoints[0][0],nEndpoints[1][0],nEndpoints[2][0],
            //                                                   nEndpoints[0][1],nEndpoints[1][1],nEndpoints[2][1]);
            // Now get the indices using the new end points
            return Clstr(block_32, dwBlockSize, nEndpoints, pcIndices, dwNumPoints, _pfChannelWeights, _bUseAlpha,_nAlphaThreshold, nRedBits, nGreenBits, nBlueBits);
        }
        else {
#ifdef USE_CMP_CORE_API
            CGU_FLOAT        CompErr = FLT_MAX;
            if (dwNumPoints < 4) {
                CGU_Vec3f        src_imageNorm[BLOCK_SIZE_4X4];
    
                for (CGU_UINT32 px = 0; px < 16; px++)
                {
                   src_imageNorm[px].r = (CODECFLOAT)((block_32[px] >> 16) & 0xff)/ 255.0f;
                   src_imageNorm[px].g = (CODECFLOAT)((block_32[px] >> 8)  & 0xff)/ 255.0f;
                   src_imageNorm[px].b = (CODECFLOAT)((block_32[px] >> 0)  & 0xff)/ 255.0f;
                }
    
                // Do a quick compression test
                CGU_Vec3f srcRGB[16];      // The list of source colors with blue channel altered
                CGU_Vec3f average_rgb;     // The centrepoint of the axis
                CGU_FLOAT  errLQ    = CMP_FLT_MAX;
                cgu_CompressRGBBlock_MinMax(src_imageNorm, 1.0f, false,srcRGB, average_rgb, errLQ);
                CGU_Vec2ui cmp =  cgu_CompressRGBBlock_Fast(src_imageNorm, 1.0f, false,srcRGB, average_rgb, CompErr);

                compressedBlock[0] = cmp.x;
                compressedBlock[1] = cmp.y;
            }
            return CompErr;
#else
            return FLT_MAX;
 #endif
        }

    } else {
        // All colors transparent
        nEndpoints[0][0] = nEndpoints[1][0] = nEndpoints[2][0] = 0;
        nEndpoints[0][1] = nEndpoints[1][1] = nEndpoints[2][1] = 0xff;
        memset(pcIndices, 0xff, dwBlockSize);
        return 0.0;
    }
}

/*----------------------------------------------------------------------------
  END of 3 COLOR case
-----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
SCALAR CASE
---------------------------------------------------------------------------*/

static CODECFLOAT CompBlock1(CODECFLOAT _RmpPnts[NUM_ENDPOINTS],
                             CODECFLOAT _Blk[MAX_BLOCK], 
                             int _Nmbr,
                             CMP_BYTE dwNumPoints, 
                             bool bFixedRampPoints,
                             int _IntPrc = 8,
                             int _FracPrc = 0,
                             bool _bFixedRamp = true,
                             bool _bUseSSE2 = true
                            );

static  CODECFLOAT   Clstr1(CMP_BYTE* pcIndices,
                            CODECFLOAT _blockIn[MAX_BLOCK],
                            CODECFLOAT _ramp[NUM_ENDPOINTS],
                            int _NmbrClrs,
                            int nNumPoints,
                            bool bFixedRampPoints,
                            int _intPrec = 8,
                            int _fracPrec = 0,
                            bool _bFixedRamp = true
                           );


static void BldRmp1(CODECFLOAT _Rmp[MAX_POINTS], CODECFLOAT _InpRmp[NUM_ENDPOINTS], int nNumPoints) {
    // for 3 point ramp; not to select the 4th point in min
    for(int e = nNumPoints; e < MAX_POINTS; e++)
        _Rmp[e] = 100000.f;

    _Rmp[0] = _InpRmp[0];
    _Rmp[1] = _InpRmp[1];
    for(int e = 1; e < nNumPoints - 1; e++)
        _Rmp[e + 1] = (_Rmp[0] * (nNumPoints - 1 - e) + _Rmp[1] * e)/(CODECFLOAT)(nNumPoints - 1);
}

static void GetRmp1(CODECFLOAT _rampDat[MAX_POINTS], CODECFLOAT _ramp[NUM_ENDPOINTS], int nNumPoints,
                    bool bFixedRampPoints, int _intPrec, int _fracPrec, bool _bFixedRamp) {
    if(_ramp[0] == _ramp[1])
        return;

    if(!bFixedRampPoints  && _ramp[0] <= _ramp[1] || bFixedRampPoints && _ramp[0] > _ramp[1]) {
        CODECFLOAT t = _ramp[0];
        _ramp[0] = _ramp[1];
        _ramp[1] = t;
    }

    _rampDat[0] = _ramp[0];
    _rampDat[1] = _ramp[1];

    CODECFLOAT IntFctr = (CODECFLOAT) (1 << _intPrec);
    CODECFLOAT FracFctr = (CODECFLOAT) (1 << _fracPrec);

    CODECFLOAT ramp[NUM_ENDPOINTS];
    ramp[0] = _ramp[0] * FracFctr;
    ramp[1] = _ramp[1] * FracFctr;

    BldRmp1(_rampDat, ramp, nNumPoints);
    if(bFixedRampPoints) {
        _rampDat[nNumPoints] = 0.;
        _rampDat[nNumPoints+1] = FracFctr * IntFctr - 1.f;
    }

    if(_bFixedRamp) {
        for(int i = 0; i < nNumPoints; i++) {
            _rampDat[i] = floor(_rampDat[i] + 0.5f);
            _rampDat[i] /= FracFctr;
        }
    }
}

static CODECFLOAT Clstr1(CMP_BYTE* pcIndices,
                         CODECFLOAT _blockIn[MAX_BLOCK], 
                         CODECFLOAT _ramp[NUM_ENDPOINTS],
                         int _NmbrClrs, 
                         int nNumPoints, 
                         bool bFixedRampPoints, 
                         int _intPrec, 
                         int _fracPrec, 
                         bool _bFixedRamp) {
    CODECFLOAT Err = 0.f;
    CODECFLOAT alpha[MAX_POINTS];

    for(int i = 0; i < _NmbrClrs; i++)
        pcIndices[i] = 0;

    if(_ramp[0] == _ramp[1])
        return Err;

    if(!_bFixedRamp) {
        _intPrec = 8;
        _fracPrec = 0;
    }

    GetRmp1(alpha, _ramp, nNumPoints, bFixedRampPoints, _intPrec, _fracPrec, _bFixedRamp);

    if(bFixedRampPoints)
        nNumPoints += 2;

    const CODECFLOAT OverIntFctr = 1.f / ((CODECFLOAT) (1 << _intPrec) - 1.f);
    for(int i = 0; i < nNumPoints; i++)
        alpha[i] *= OverIntFctr;

    // For each colour in the original block, calculate its weighted
    // distance from each point in the original and assign it
    // to the closest cluster
    for(int i = 0; i < _NmbrClrs; i++) {
        CODECFLOAT shortest = 10000000.f;

        // Get the original alpha
        CODECFLOAT acur = _blockIn[i];

        for(BYTE j = 0; j < nNumPoints; j++) {
            CODECFLOAT adist = (acur - alpha[j]);
            adist *= adist;

            if(adist < shortest) {
                shortest = adist;
                pcIndices[i] = j;
            }
        }

        Err += shortest;
    }

    return Err;
}

static CODECFLOAT RmpSrch1(CODECFLOAT _Blk[MAX_BLOCK],
                           CODECFLOAT _Rpt[MAX_BLOCK],
                           CODECFLOAT _maxerror,
                           CODECFLOAT _min_ex,
                           CODECFLOAT _max_ex,
                           int _NmbrClrs,
                           CMP_BYTE nNumPoints) {
    CODECFLOAT error = 0;
    const CODECFLOAT step = (_max_ex - _min_ex) / (CODECFLOAT)(nNumPoints - 1);
    const CODECFLOAT step_h = step * 0.5f;
    const CODECFLOAT rstep = 1.0f / step;

    for(int i=0; i< _NmbrClrs; i++) {
        CODECFLOAT v;
        // Work out which value in the block this select
        CODECFLOAT del;

        if((del = _Blk[i] - _min_ex) <= 0)
            v = _min_ex;
        else if(_Blk[i] -  _max_ex >= 0)
            v = _max_ex;
        else
            v = (floor((del + step_h) * rstep) * step) + _min_ex;

        // And accumulate the error
        CODECFLOAT del2 = (_Blk[i] - v);
        error += del2 * del2 * _Rpt[i];

        // if we've already lost to the previous step bail out
        if(_maxerror < error) {
            error  = _maxerror;
            break;
        }
    }
    return error;
}


static CODECFLOAT Refine1(CODECFLOAT _Blk[MAX_BLOCK], 
                          CODECFLOAT _Rpt[MAX_BLOCK],
                          CODECFLOAT _MaxError, 
                          CODECFLOAT& _min_ex, 
                          CODECFLOAT& _max_ex, 
                          CODECFLOAT _m_step,
                          CODECFLOAT _min_bnd, 
                          CODECFLOAT _max_bnd, 
                          int _NmbrClrs,
                          CMP_BYTE dwNumPoints, 
                          bool _bUseSSE2) 
{
#ifdef USE_CMP_CORE_API
    CMP_UNUSED(_bUseSSE2);
#endif
    // Start out assuming our endpoints are the min and max values we've determined
    // Attempt a (simple) progressive refinement step to reduce noise in the
    // output image by trying to find a better overall match for the endpoints.

    CODECFLOAT maxerror = _MaxError;
    CODECFLOAT min_ex = _min_ex;
    CODECFLOAT max_ex = _max_ex;

    int mode, bestmode;
    do {
        CODECFLOAT cr_min0 = min_ex;
        CODECFLOAT cr_max0 = max_ex;
        for(bestmode = -1, mode = 0; mode < SCH_STPS * SCH_STPS; mode++) {
            // check each move (see sStep for direction)
            CODECFLOAT cr_min =  min_ex + _m_step * sMvF[mode / SCH_STPS];
            CODECFLOAT cr_max =  max_ex + _m_step * sMvF[mode % SCH_STPS];

            cr_min = max(cr_min, _min_bnd);
            cr_max = min(cr_max, _max_bnd);

            CODECFLOAT error;
            error = RmpSrch1(_Blk, _Rpt, maxerror, cr_min, cr_max, _NmbrClrs, dwNumPoints);

            if(error < maxerror) {
                maxerror = error;
                bestmode = mode;
                cr_min0 = cr_min;
                cr_max0 = cr_max;
            }
        }

        if(bestmode != -1) {
            // make move (see sStep for direction)
            min_ex = cr_min0;
            max_ex = cr_max0;
        }
    } while(bestmode != -1);

    _min_ex = min_ex;
    _max_ex = max_ex;

    return maxerror;
}

#ifndef USE_CMP_CORE_API
static int QSortFCmp(const void * Elem1, const void * Elem2) {
    int ret = 0;

    if(*(CODECFLOAT*)Elem1 - *(CODECFLOAT*)Elem2 < 0.)
        ret = -1;
    else if(*(CODECFLOAT*)Elem1 - *(CODECFLOAT*)Elem2 > 0.)
        ret = 1;
    return ret;
}
#endif

/*--------------------------------------------------------------------------------------------
// input [0,1]
   this is the case when the input data and possible ramp values are all on integer grid.
*/
#define _INT_GRID (_bFixedRamp && _FracPrc == 0)

static CODECFLOAT CompBlock1(CODECFLOAT _RmpPnts[NUM_ENDPOINTS], 
                             CODECFLOAT _Blk[MAX_BLOCK], 
                             int _Nmbr,
                             CMP_BYTE dwNumPoints, 
                             bool bFixedRampPoints,
                             int _IntPrc, 
                             int _FracPrc, 
                             bool _bFixedRamp, 
                             bool _bUseSSE2,
                             bool isSigned
                             ) 
{
#ifdef USE_CMP_CORE_API
    CMP_UNUSED(isSigned);
#endif

    CODECFLOAT fMaxError = 0.f;
    CODECFLOAT Ramp[NUM_ENDPOINTS];
    CODECFLOAT IntFctr = (CODECFLOAT)(1 << _IntPrc);

    ALIGN_16 CODECFLOAT afUniqueValues[MAX_BLOCK];
    ALIGN_16 CODECFLOAT afValueRepeats[MAX_BLOCK];
    for(int i = 0; i < MAX_BLOCK; i++)
        afUniqueValues[i] = afValueRepeats[i] = 0.f;

    // For each unique value we compute the number of it appearances.
    CODECFLOAT fBlk[MAX_BLOCK];
    std::memcpy(fBlk, _Blk, _Nmbr * sizeof(CODECFLOAT));

    // sort the input
    qsort((void *)fBlk, (size_t)_Nmbr, sizeof(CODECFLOAT), QSortFCmp);

    CODECFLOAT new_p = -2.;

    int N0s = 0, N1s = 0;
    DWORD dwUniqueValues = 0;
    afUniqueValues[0] = 0.f;

    bool requiresCalculation = true;

    if(bFixedRampPoints) {
        for(int i = 0; i < _Nmbr; i++) {
            if(new_p != fBlk[i]) {
                new_p = fBlk[i];
                if(new_p <= 1.5/255.)
                    N0s++;
                else if(new_p >= 253.5/255.)
                    N1s++;
                else {
                    afUniqueValues[dwUniqueValues] = fBlk[i];
                    afValueRepeats[dwUniqueValues] = 1.f;
                    dwUniqueValues++;
                }
            } else if(dwUniqueValues > 0 && afUniqueValues[dwUniqueValues - 1] == new_p)
                afValueRepeats[dwUniqueValues - 1] += 1.f;
        }

        // if number of unique colors is less or eq 2 we've done either, but we know that we may have 0s and/or 1s as well.
        // To avoid for the ramp to be considered flat we invented couple entries on the way.
        if(dwUniqueValues <= 2) {
            if(dwUniqueValues == 2) { // if 2, take them
                Ramp[0]  = floor(afUniqueValues[0] * (IntFctr - 1) + 0.5f);
                Ramp[1]  = floor(afUniqueValues[1] * (IntFctr - 1) + 0.5f);
            } else if(dwUniqueValues == 1) { // if 1, add another one
                Ramp[0]  = floor(afUniqueValues[0] * (IntFctr - 1) + 0.5f);
                Ramp[1] = Ramp[0] + 1.f;
            } else { // if 0, invent them
                Ramp[0]  = 128.f;
                Ramp[1] = Ramp[0] + 1.f;
            }

            fMaxError = 0.f;
            requiresCalculation = false;
        }
    } else {
        for(int i = 0; i < _Nmbr; i++) {
            if(new_p != fBlk[i]) {
                afUniqueValues[dwUniqueValues] = new_p = fBlk[i];
                afValueRepeats[dwUniqueValues] = 1.f;
                dwUniqueValues++;
            } else
                afValueRepeats[dwUniqueValues - 1] += 1.f;
        }

        // if number of unique colors is less or eq 2, we've done
        if(dwUniqueValues <= 2) {
            Ramp[0] = floor(afUniqueValues[0] * (IntFctr - 1) + 0.5f);
            if(dwUniqueValues == 1)
                Ramp[1] = Ramp[0] + 1.f;
            else
                Ramp[1] = floor(afUniqueValues[1] * (IntFctr - 1) + 0.5f);
            fMaxError = 0.f;
            requiresCalculation = false;
        }
    }

    if ( requiresCalculation ) {
        CODECFLOAT min_ex  = afUniqueValues[0];
        CODECFLOAT max_ex  = afUniqueValues[dwUniqueValues - 1];
        CODECFLOAT min_bnd = 0, max_bnd = 1.;
        CODECFLOAT min_r = min_ex, max_r = max_ex;
        CODECFLOAT gbl_l = 0, gbl_r = 0;
        CODECFLOAT cntr = (min_r + max_r)/2;

        CODECFLOAT gbl_err = MAX_ERROR;
        // Trying to avoid unnecessary calculations. Heuristics: after some analisis it appears
        // that in integer case, if the input interval not more then 48 we won't get much better

        bool wantsSearch = !( _INT_GRID && max_ex - min_ex <= 48.f / IntFctr );

        if ( wantsSearch ) {
            // Search.
            // 1. take the vicinities of both low and high bound of the input interval.
            // 2. setup some search step
            // 3. find the new low and high bound which provides an (sub) optimal (infinite precision) clusterization.
            CODECFLOAT gbl_llb = (min_bnd >  min_r - GBL_SCH_EXT) ? min_bnd : min_r - GBL_SCH_EXT;
            CODECFLOAT gbl_rrb = (max_bnd <  max_r + GBL_SCH_EXT) ? max_bnd : max_r + GBL_SCH_EXT;
            CODECFLOAT gbl_lrb = (cntr <  min_r + GBL_SCH_EXT) ? cntr : min_r + GBL_SCH_EXT;
            CODECFLOAT gbl_rlb = (cntr >  max_r - GBL_SCH_EXT) ? cntr : max_r - GBL_SCH_EXT;
            for(CODECFLOAT step_l = gbl_llb; step_l < gbl_lrb ; step_l+= GBL_SCH_STEP) {
                for(CODECFLOAT step_r = gbl_rrb; gbl_rlb <= step_r; step_r-=GBL_SCH_STEP) {
                    CODECFLOAT sch_err;
                        sch_err = RmpSrch1(afUniqueValues, afValueRepeats, gbl_err, step_l, step_r, dwUniqueValues, dwNumPoints);
                    if(sch_err < gbl_err) {
                        gbl_err = sch_err;
                        gbl_l = step_l;
                        gbl_r = step_r;
                    }
                }
            }

            min_r = gbl_l;
            max_r = gbl_r;
        }

        // This is a refinement call. The function tries to make several small stretches or squashes to
        // minimize quantization error.
        CODECFLOAT m_step = LCL_SCH_STEP/ IntFctr;
        fMaxError = Refine1(afUniqueValues, afValueRepeats, gbl_err, min_r, max_r, m_step, min_bnd, max_bnd, dwUniqueValues,
                            dwNumPoints, _bUseSSE2);

        min_ex = min_r;
        max_ex = max_r;

        max_ex *= (IntFctr - 1);
        min_ex *= (IntFctr - 1);
        /*
        this one is tricky. for the float or high fractional precision ramp it tries to avoid
        for the ramp to be collapsed into one integer number after rounding.
        Notice the condition. There is a difference between max_ex and min_ex but after rounding
        they may collapse into the same integer.

        So we try to run the same refinement procedure but with starting position on the integer grid
        and step equal 1.
        */
        if(!_INT_GRID && max_ex - min_ex > 0. && floor(min_ex + 0.5f) == floor(max_ex + 0.5f)) {
            m_step = 1.;
            gbl_err = MAX_ERROR;
            for(DWORD i = 0; i < dwUniqueValues; i++)
                afUniqueValues[i] *= (IntFctr - 1);

            max_ex = min_ex = floor(min_ex + 0.5f);

            gbl_err = Refine1(afUniqueValues, afValueRepeats, gbl_err, min_ex, max_ex, m_step, 0.f, 255.f, dwUniqueValues, dwNumPoints, _bUseSSE2);

            fMaxError = gbl_err;

        }
        Ramp[1] = floor(max_ex + 0.5f);
        Ramp[0] = floor(min_ex + 0.5f);
    }

    // Ensure that the two endpoints are not the same
    // This is legal but serves no need & can break some optimizations in the compressor
    if(Ramp[0] == Ramp[1]) {
        if(Ramp[1] < 255.f)
            Ramp[1]++;
        else
            Ramp[1]--;
    }
    _RmpPnts[0] = Ramp[0];
    _RmpPnts[1] = Ramp[1];

    return fMaxError;
}

/*--------------------------------------------------------------------------------------------
// input [0,1]
void CompBlock1X(CODECFLOAT* _Blk, [IN] scalar data block (alphas or normals) in float format
                 CMP_DWORD blockCompressed[NUM_ENDPOINTS],  [OUT] compressed data in DXT5 alpha foramt
                 int _NbrClrs,              [IN] actual number of elements in the block
                 int _intPrec,              [IN} integer precision; it applies both to the input data and
                                                 to the ramp points
                 int _fracPrec,             [IN] fractional precision of the ramp points
                 bool _bFixedRamp,          [IN] non-fixed ramp means we have input and generate
                                                 output as float. fixed ramp means that they are fractional numbers.
               )
---------------------------------------------------------------------------------------------*/

CODECFLOAT CompBlock1X( CODECFLOAT* _Blk, 
                        CMP_WORD dwBlockSize, 
                        CMP_BYTE nEndpoints[2], 
                        CMP_BYTE* pcIndices,
                        CMP_BYTE dwNumPoints, 
                        bool bFixedRampPoints, 
                        bool _bUseSSE2, 
                        int _intPrec, 
                        int _fracPrec, 
                        bool _bFixedRamp)
{
    // just to make them initialized
    if(!_bFixedRamp) {
        _intPrec = 8;
        _fracPrec = 0;
    }

    // this one makes the bulk of the work
    CODECFLOAT Ramp[NUM_ENDPOINTS];
    CompBlock1(Ramp, _Blk, dwBlockSize, dwNumPoints, bFixedRampPoints, _intPrec, _fracPrec, _bFixedRamp, _bUseSSE2, false);

    // final clusterization applied
    CODECFLOAT fError = Clstr1(pcIndices, _Blk, Ramp, dwBlockSize, dwNumPoints, bFixedRampPoints, _intPrec, _fracPrec, _bFixedRamp);
    nEndpoints[0] = (BYTE)Ramp[0];
    nEndpoints[1] = (BYTE)Ramp[1];

    return fError;
}

CODECFLOAT CompBlock1XS(CODECFLOAT* _Blk,
                       CMP_WORD    dwBlockSize,
                       CMP_BYTE    nEndpoints[2],
                       CMP_BYTE*   pcIndices,
                       CMP_BYTE    dwNumPoints,
                       bool        bFixedRampPoints,
                       bool        _bUseSSE2,
                       int         _intPrec,
                       int         _fracPrec,
                       bool        _bFixedRamp)
{
    // just to make them initialized
    if (!_bFixedRamp)
    {
        _intPrec  = 8;
        _fracPrec = 0;
    }

    // this one makes the bulk of the work
    CODECFLOAT Ramp[NUM_ENDPOINTS];
    CompBlock1(Ramp, _Blk, dwBlockSize, dwNumPoints, bFixedRampPoints, _intPrec, _fracPrec, _bFixedRamp, _bUseSSE2, true);

    // final clusterization applied
    CODECFLOAT fError = Clstr1(pcIndices, _Blk, Ramp, dwBlockSize, dwNumPoints, bFixedRampPoints, _intPrec, _fracPrec, _bFixedRamp);
    nEndpoints[0]     = (BYTE)Ramp[0];
    nEndpoints[1]     = (BYTE)Ramp[1];

    return fError;
}

/*--------------------------------------------------------------------------------------------
// input [0,255]
void CompBlock1X(CMP_BYTE* _Blk, [IN] scalar data block (alphas or normals) in 8 bits format
                 CMP_DWORD blockCompressed[NUM_ENDPOINTS],  [OUT] compressed data in DXT5 alpha foramt
                 int _NbrClrs,              [IN] actual number of elements in the block
                 int _intPrec,              [IN} integer precision; it applies both to the input data and
                                                 to the ramp points
                 int _fracPrec,             [IN] fractional precision of the ramp points
                 bool _bFixedRamp,          [IN] always true at this point
               )
---------------------------------------------------------------------------------------------*/

CODECFLOAT CompBlock1X( CMP_BYTE* _Blk, 
                        CMP_WORD dwBlockSize, 
                        CMP_BYTE nEndpoints[2], 
                        CMP_BYTE* pcIndices,
                        CMP_BYTE dwNumPoints, 
                        bool bFixedRampPoints, 
                        bool _bUseSSE2, 
                        int _intPrec, 
                        int _fracPrec, 
                        bool _bFixedRamp) 
{
    // convert the input and call the float equivalent.
    CODECFLOAT fBlk[MAX_BLOCK];
    for(int i = 0; i < dwBlockSize; i++)
        fBlk[i] = (CODECFLOAT)_Blk[i] / 255.f;

    return CompBlock1X(fBlk, dwBlockSize, nEndpoints, pcIndices, dwNumPoints, bFixedRampPoints, _bUseSSE2, _intPrec, _fracPrec, _bFixedRamp);
}

CODECFLOAT CompBlock1XS(CMP_SBYTE* _Blk,
                       CMP_WORD  dwBlockSize,
                       CMP_BYTE  nEndpoints[2],
                       CMP_BYTE* pcIndices,
                       CMP_BYTE  dwNumPoints,
                       bool      bFixedRampPoints,
                       bool      _bUseSSE2,
                       int       _intPrec,
                       int       _fracPrec,
                       bool      _bFixedRamp)
{
    // convert the input and call the float equivalent.
    CODECFLOAT fBlk[MAX_BLOCK];
    for (int i = 0; i < dwBlockSize; i++)
        fBlk[i] = (CODECFLOAT)_Blk[i] / 128.f;

    return CompBlock1X(fBlk, dwBlockSize, nEndpoints, pcIndices, dwNumPoints, bFixedRampPoints, _bUseSSE2, _intPrec, _fracPrec, _bFixedRamp);
}
