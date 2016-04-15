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

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#include "BC6H_3dquant_constants.h"
#include "BC6H_3dquant_vpc.h"
#include "BC6H_shake.h"
#include "BC6H_utils.h"
#include "debug.h"

#define BC6H_LOG_CL_BASE        2
#define BC6H_BIT_BASE            5
#define BC6H_LOG_CL_RANGE        5
#define BC6H_BIT_RANGE            9
#define BC6H_CLT(cl)   (cl-BC6H_LOG_CL_BASE)
#define BC6H_BTT(bits) (bits-BC6H_BIT_BASE)

extern double BC6H_totalError_d(double data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],double data2[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],int numEntries, int dimension);

extern int g_block;

int BC6H_npv_nd[][2*BC6H_MAX_DIMENSION_BIG]={
    {}, // 0 - for alingment        
    {}, // 1
    {}, // 2
    {1,2,4,8,16,32}, //3
    {1,2,4}          //4
};

// Used by BC6H_Decode
const double  BC6H_rampLerpWeights[5][1<<BC6H_MAX_INDEX_BITS] =
{
    {0.0},  // 0 bit index
    {0.0, 1.0}, // 1 bit index
    {0.0, 21.0/64.0, 43.0/64.0, 1.0}, // 2 bit index
    {0.0, 9.0/64.0, 18.0/64.0, 27.0/64.0, 37.0/64.0, 46.0/64.0, 55.0/64.0, 1.0}, // 3 bit index
    {0.0, 4.0/64.0, 9.0/64.0, 13.0/64.0, 17.0/64.0, 21.0/64.0, 26.0/64.0, 30.0/64.0,
     34.0/64.0, 38.0/64.0, 43.0/64.0, 47.0/64.0, 51.0/64.0, 55.0/64.0, 60.0/64.0, 1.0} // 4 bit index
};


// Reworking these tables
// Since endpoints can be flipped for the encoding (reclaims an index bit) this means
// that the parity tables must be symmetric for the two endpoints, as parity could also be flipped
// The tables above do not guarantee this behaviour, but this one does

int BC6H_par_vectors_nd[][2*BC6H_MAX_DIMENSION_BIG][(1<<(2*BC6H_MAX_DIMENSION_BIG-1))][2][BC6H_MAX_DIMENSION_BIG] =
{
    {{{}}}, // 0 - for alingment        

    {{{}}}, //1D

    {{{}}}, //2D

            {
                {{}},

// 3*n+1    BCC          3*n+1        Cartesian 3*n            //same parity
            { // SAME_PAR
            {{0,0,0},{0,0,0}},
            {{1,1,1},{1,1,1}}
            },
// 3*n+2    BCC          3*n+1        BCC          3*n+1    
            { // BCC
            {{0,0,0},{0,0,0}},
            {{0,0,0},{1,1,1}},
            {{1,1,1},{0,0,0}},
            {{1,1,1},{1,1,1}}
            },
// 3*n+3    FCC                    ???                        // ??????
// BCC with FCC same or inverted, symmetric
            { // BCC_SAME_FCC
            {{0,0,0},{0,0,0}},
            {{1,1,0},{1,1,0}},
            {{1,0,1},{1,0,1}},
            {{0,1,1},{0,1,1}},

            {{0,0,0},{1,1,1}},
            {{1,1,1},{0,0,0}},
            {{0,1,0},{0,1,0}},  // ??
            {{1,1,1},{1,1,1}},

            },
// 3*n+4    FCC          3*n+2        FCC          3*n+2
            {

            {{0,0,0},{0,0,0}},
            {{1,1,0},{0,0,0}},
            {{1,0,1},{0,0,0}},
            {{0,1,1},{0,0,0}},

            {{0,0,0},{1,1,0}},
            {{1,1,0},{1,1,0}},
            {{1,0,1},{1,1,0}},
            {{0,1,1},{1,1,0}},

            {{0,0,0},{1,0,1}},
            {{1,1,0},{1,0,1}},
            {{1,0,1},{1,0,1}},
            {{0,1,1},{1,0,1}},

            {{0,0,0},{0,1,1}},
            {{1,1,0},{0,1,1}},
            {{1,0,1},{0,1,1}},
            {{0,1,1},{0,1,1}}
            },


// 3*n+5    Cartesian 3*n+3        FCC          3*n+2            //D^*[6]  
            {

            {{0,0,0},{0,0,0}},
            {{1,1,0},{0,0,0}},
            {{1,0,1},{0,0,0}},
            {{0,1,1},{0,0,0}},

            {{0,0,0},{1,1,0}},
            {{1,1,0},{1,1,0}},
            {{1,0,1},{1,1,0}},
            {{0,1,1},{1,1,0}},

            {{0,0,0},{1,0,1}},
            {{1,1,0},{1,0,1}},
            {{1,0,1},{1,0,1}},
            {{0,1,1},{1,0,1}},

            {{0,0,0},{0,1,1}},
            {{1,1,0},{0,1,1}},
            {{1,0,1},{0,1,1}},
            {{0,1,1},{0,1,1}},


            {{1,0,0},{1,1,1}},
            {{0,1,0},{1,1,1}},
            {{0,0,1},{1,1,1}},
            {{1,1,1},{1,1,1}},

            {{1,0,0},{0,0,1}},
            {{0,1,0},{0,0,1}},
            {{0,0,1},{0,0,1}},
            {{1,1,1},{0,0,1}},

            {{1,0,0},{1,0,0}},
            {{0,1,0},{1,0,0}},
            {{0,0,1},{1,0,0}},
            {{1,1,1},{1,0,0}},

            {{1,0,0},{0,1,0}},
            {{0,1,0},{0,1,0}},
            {{0,0,1},{0,1,0}},
            {{1,1,1},{0,1,0}}
            }
            },

            {
                {{}},

// 3*n+1    BCC          3*n+1        Cartesian 3*n            //same parity
            { // SAME_PAR
                {{0,0,0,0},{0,0,0,0}},
                {{1,1,1,1},{1,1,1,1}}
            },
// 3*n+2    BCC          3*n+1        BCC          3*n+1    
            { // BCC
                {{0,0,0,0},{0,0,0,0}},
                {{0,0,0,0},{1,1,1,1}},
                {{1,1,1,1},{0,0,0,0}},
                {{1,1,1,1},{1,1,1,1}}
            },
// 3 PBIT
            {
                {{0,0,0,0},{0,0,0,0}},
                {{0,0,0,0},{0,1,1,1}},
                {{0,1,1,1},{0,0,0,0}},
                {{0,1,1,1},{0,1,1,1}},

                {{1,0,0,0},{1,0,0,0}},
                {{1,0,0,0},{1,1,1,1}},
                {{1,1,1,1},{1,0,0,0}},
                {{1,1,1,1},{1,1,1,1}}
            },

// 4 PBIT
            {
                {{0,0,0,0},{0,0,0,0}},
                {{0,0,0,0},{0,1,1,1}},
                {{0,1,1,1},{0,0,0,0}},
                {{0,1,1,1},{0,1,1,1}},

                {{1,0,0,0},{1,0,0,0}},
                {{1,0,0,0},{1,1,1,1}},
                {{1,1,1,1},{1,0,0,0}},
                {{1,1,1,1},{1,1,1,1}},

                {{0,0,0,0},{0,0,0,0}},
                {{0,0,0,0},{0,0,1,1}},
                {{0,0,1,1},{0,0,0,0}},
                {{0,1,0,1},{0,1,0,1}},

                {{1,0,0,0},{1,0,0,0}},
                {{1,0,0,0},{1,0,1,1}},
                {{1,0,1,1},{1,0,0,0}},
                {{1,1,0,1},{1,1,0,1}},

            },

    }, //4

};

static double BC6H_ramp[BC6H_LOG_CL_RANGE-BC6H_LOG_CL_BASE][BC6H_BIT_RANGE-BC6H_BIT_BASE][256][256][16];
static double BC6H_ep_d[BC6H_BIT_RANGE-BC6H_BIT_BASE][256];
// inverted table
// <log2 clusters >,  bits, value, par1, par2, <ep1>
static int      BC6H_sp_idx[BC6H_LOG_CL_RANGE-BC6H_LOG_CL_BASE][BC6H_BIT_RANGE-BC6H_BIT_BASE][256][2][2][BC6H_MAX_CLUSTERS_BIG][2];
// <log2 clusters >,  bits, value, par1, par2, 
static double   BC6H_sp_err[BC6H_LOG_CL_RANGE-BC6H_LOG_CL_BASE][BC6H_BIT_RANGE-BC6H_BIT_BASE][256][2][2][BC6H_MAX_CLUSTERS_BIG];
// 
int BC6H_expand_ (int bits, int v) {
    assert(bits >=4);
    return (  v << (8-bits) | v >> (2* bits - 8)); 
}

void BC6H_init_ramps (void) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int clog,bits;
    int p1;
    int p2;
    int i,j;
    int o1,o2;

    for (bits=BC6H_BIT_BASE;bits<BC6H_BIT_RANGE;bits++) 
        for (p1=0;p1<(1<<bits);p1++)
            BC6H_ep_d[BC6H_BTT(bits)][p1]=(double) BC6H_expand_(bits,p1);


    for (clog=BC6H_LOG_CL_BASE;clog<BC6H_LOG_CL_RANGE;clog++)
        for (bits=BC6H_BIT_BASE;bits<BC6H_BIT_RANGE;bits++) 
            for (p1=0;p1<(1<<bits);p1++)
                for (p2=0;p2<(1<<bits);p2++) {
                    for (i=0; i<(1<<clog);i++)
                        BC6H_ramp[BC6H_CLT(clog)][BC6H_BTT(bits)][p1][p2][i] = 
                        (double) BC6H_ep_d[BC6H_BTT(bits)][p1] + BC6H_rampLerpWeights[clog][i] * (double)((BC6H_ep_d[BC6H_BTT(bits)][p2]- BC6H_ep_d[BC6H_BTT(bits)][p1]));

#ifdef GIG_TABLE
                    double v;
                    int vi;
                    for (vi=0;vi<256;vi++)
                        for (i=0; i<(1<<clog);i++)
                            ramp_err[CLT(clog)][BTT(bits)][p1][p2][vi][i] = 
                            (ramp[CLT(clog)][BTT(bits)][p1][p2][i]-(double) vi) *
                            (ramp[CLT(clog)][BTT(bits)][p1][p2][i]-(double) vi);
#endif
                }


    //-----------------------------------------------------------------------------

    for (clog=BC6H_LOG_CL_BASE;clog<BC6H_LOG_CL_RANGE;clog++)
        for (bits=BC6H_BIT_BASE;bits<BC6H_BIT_RANGE;bits++) 
            for(j=0;j<256;j++)
                for (o1=0;o1<2;o1++)
                    for (o2=0;o2<2;o2++)
                        for(i=0;i<16;i++) {
                            BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][j][o1][o2][i][0]=-1;
                            BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits)][j][o1][o2][i]=DBL_MAX;
                        }

    for (clog=BC6H_LOG_CL_BASE;clog<BC6H_LOG_CL_RANGE;clog++)
        for (bits=BC6H_BIT_BASE;bits<BC6H_BIT_RANGE;bits++) 
            for (p1=0;p1<(1<<bits);p1++)
                for (p2=0;p2<(1<<bits);p2++)
                    for (i=0; i<(1<<clog);i++) {
                        BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][(int) BC6H_ramp[BC6H_CLT(clog)][BC6H_BTT(bits)][p1][p2][i]][p1 & 0x1][p2 & 0x1][i][0]=p1;
                        BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][(int) BC6H_ramp[BC6H_CLT(clog)][BC6H_BTT(bits)][p1][p2][i]][p1 & 0x1][p2 & 0x1][i][1]=p2;
                        BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits)][(int) BC6H_ramp[BC6H_CLT(clog)][BC6H_BTT(bits)][p1][p2][i]][p1 & 0x1][p2 & 0x1][i]=0.;
                    }

    for (clog=BC6H_LOG_CL_BASE;clog<BC6H_LOG_CL_RANGE;clog++)
        for (bits=BC6H_BIT_BASE;bits<BC6H_BIT_RANGE;bits++) 
            for(j=0;j<256;j++)
                for (o1=0;o1<2;o1++)
                    for (o2=0;o2<2;o2++)
                        for(i=0;i<(1<<clog);i++)
                            if (BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][j][o1][o2][i][0]<0) {
                                int k;
                                for (k=1;k<256;k++)
                                    if ( (j-k >= 0 && BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits)][j-k][o1][o2][i]==0) ||
                                         (j+k < 256 && BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits)][j+k][o1][o2][i]==0) )
                                         break;
                                 {
                                    if ( (j-k >= 0 && BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits)][j-k][o1][o2][i]==0)) {
                                        BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][j][o1][o2][i][0]=BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][j-k][o1][o2][i][0];
                                        BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][j][o1][o2][i][1]=BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][j-k][o1][o2][i][1];
                                    }
                                    else if ((j+k < 256 && BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits)][j+k][o1][o2][i]==0)){
                                        BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][j][o1][o2][i][0]=BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][j+k][o1][o2][i][0];
                                        BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][j][o1][o2][i][1]=BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits)][j+k][o1][o2][i][1];
                                    }
                                    BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits)][j][o1][o2][i]=k*k;
                                }
                            }
}

//======================================================================================================================================================
// This section of code is for Optimization of End Points
//======================================================================================================================================================

inline int BC6H_all_same_d (double d[][BC6H_MAX_DIMENSION_BIG],  int n, int dimension)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    assert(n>0);
    int i,j;
    int same = 1;
    for(i=1;i< n;i++)
        for(j=0;j< dimension;j++)
            same = same && (d[0][j] ==d[i][j]);

    return(same);
}

inline void BC6H_mean_d_d (double d[][BC6H_MAX_DIMENSION_BIG], double mean[BC6H_MAX_DIMENSION_BIG], int n, int dimension) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int i,j;
    assert(n!=0);
    for(j=0;j< dimension;j++)
        mean[j] =0;
    for(i=0;i< n;i++)
        for(j=0;j< dimension;j++)
            mean[j] +=d[i][j];
    for(j=0;j< dimension;j++)
        mean[j] /=(double) n;
}

void BC6H_index_collapse_ (int index[], int numEntries)
{
    int k;
    int d,D;

    int mi;
    int Mi;
    if (numEntries ==0)
        return;

    mi=Mi=index[0];
    for (k=1;k<numEntries;k++) { 
        mi = mi < index[k] ? mi : index[k];
        Mi = Mi > index[k] ? Mi : index[k];
    }
    D=1;
    for (d=2;d<=Mi-mi;d++) {

        for (k=0;k<numEntries;k++)  
            if ((index[k] -mi) % d !=0)
                break;
        if (k>=numEntries) 
            D =d;
    }
    for (k=0;k<numEntries;k++)  
        index[k] = (index[k]- mi)/D; 
}

inline int BC6H_max_i (int a[], int n) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    assert(n>0);
    int i,m=a[0];
    for(i=0;i< n;i++)
        m = m > a[i] ? m : a[i];
    return (m);
}

// finds "floor in the set" if exists, otherwise returns min : clamps return if negative
// optimize this for BC6H it has no parity bits!
inline int BC6H_ep_find_floor( double v, int bits, int use_par, int odd)
{
    assert(use_par==0 || use_par==1 || odd==0 || odd==1 );
    
    double *p = BC6H_ep_d[BC6H_BTT(bits)];
    int i1=0;
    int i2=1<<(bits-use_par);
    int last = 0;
    odd = use_par ? odd : 0; 
    while (i2-i1>1) {
        if (last == (i2-i1))
        {
            return(0);
        }
        last = i2-i1;
        int i = (i1+i2)/2;
        int j;
        j = (i<<use_par)+odd;
        if (v >= p[j] )
            i1=j;
        else
            i2=j;
    }
    i1 = (i1<<use_par)+odd;
    if (i1 < 0) 
        i1 = 0;
    return (i1);
}


//
// New version that will take varying dimension
//
//
//

double BC6H_quant_single_point_d
( 
    double data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG], 
    int numEntries, int index[BC6H_MAX_ENTRIES],
    double out[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],
    int epo_1[2][BC6H_MAX_DIMENSION_BIG],
    int Mi_,                // last cluster
    int bits[3],            // including parity
    int type, 
    int dimension
    ) 
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int i,j;

    double err_0=DBL_MAX;

    double err_1=DBL_MAX;
    int idx = 0;
    int idx_1 = 0;

    int epo_0[2][BC6H_MAX_DIMENSION_BIG];

    int use_par =(type !=0);

    int clog=0;
    i = Mi_+1;
    while (i>>=1) 
        clog++;
    assert((1<<clog)== Mi_+1);
         
    int pn;
    int (*pv)[BC6H_MAX_DIMENSION_BIG];
    int npv_nd = BC6H_npv_nd[dimension][type];


    for (pn=0;pn<npv_nd;pn++)
    {
     {
        pv = BC6H_par_vectors_nd[dimension][type][pn]; 

            int o1[2][BC6H_MAX_DIMENSION_BIG]={0,2};
            int o2[2][BC6H_MAX_DIMENSION_BIG]={0,2};

            for (j=0;j<dimension;j++)
            {
                o2[0][j]=o1[0][j]=0;
                o2[1][j]=o1[1][j]=2;

                if (use_par)
                {
                    if (pv[0][j])
                        o1[0][j]=1;
                    else
                        o1[1][j]=1;
                    if (pv[1][j])
                        o2[0][j]=1;
                    else
                        o2[1][j]=1;
                }
            };

            int t1,t2;
            
            int dr[BC6H_MAX_DIMENSION_BIG];
            int dr_0[BC6H_MAX_DIMENSION_BIG];
            double tr;
            
            for (i=0; i< (1<<clog);i++)
            {
                double t=0; 
                int t1o[BC6H_MAX_DIMENSION_BIG],t2o[BC6H_MAX_DIMENSION_BIG];

                for (j=0;j<dimension;j++)
                {
                    double t_=DBL_MAX;

                    for (t1=o1[0][j];t1<o1[1][j];t1++)
                    {
                        for (t2=o2[0][j];t2<o2[1][j];t2++)
                        // This is needed for non-integer mean points of "collapsed" sets
                        {
                            int tf = (int) floor(data[0][j]);
                            int tc = (int) ceil (data[0][j]);

                            if (  BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits[j])][tf][t1][t2][i]
                                > BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits[j])][tc][t1][t2][i]) 
                                // if they are not equal, the same representalbe point is used for 
                                // both of them, as all representable points are integers in the rage 
                                dr[j]=tc;
                            else if (BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits[j])][ tf ][t1][t2][i]
                                   < BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits[j])][ tc ][t1][t2][i]) 
                                dr[j]=tf;
                            else 
                                dr[j]=(int)floor(data[0][j]+0.5);

                            tr = BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits[j])][dr[j]][t1][t2][i] + 
                                 2*sqrt(BC6H_sp_err[BC6H_CLT(clog)][BC6H_BTT(bits[j])][dr[j]][t1][t2][i]) * fabs((double)dr[j]-data[0][j])+
                                 (dr[j]-data[0][j])* (dr[j]-data[0][j]);

                            if (tr < t_)
                            {
                                t_=tr;
                                t1o[j]=t1;
                                t2o[j]=t2;
                                dr_0[j]=dr[j];
                            }
                        }
                    }

                    t += t_;
                }

                if (t < err_0)
                {
                    idx=i;
                    for (j=0;j<dimension;j++)
                    {
                        epo_0[0][j]=BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits[j])][ dr_0[j]][t1o[j]][t2o[j]][i][0];
                        epo_0[1][j]=BC6H_sp_idx[BC6H_CLT(clog)][BC6H_BTT(bits[j])][ dr_0[j]][t1o[j]][t2o[j]][i][1];
                    }
                    err_0=t;
                }
                if (err_0==0)
                    break;
            }

            if (err_0 < err_1)
            {
                idx_1=idx;
                for (j=0;j<dimension;j++)
                {
                    epo_1[0][j]=epo_0[0][j];
                    epo_1[1][j]=epo_0[1][j];
                }
                err_1=err_0;
            }

            if (err_1==0)
                break;
        }
    }

    int a;
    int b;
    int c;
    int d;
    for (i=0;i< numEntries;i++)
    {
        index[i]=idx_1;
        for (j=0;j<dimension;j++) 
        {
            a = BC6H_CLT(clog);
            b = BC6H_BTT(bits[j]);
            c = epo_1[0][j];
            d = epo_1[1][j];
            if (d < 256)
                out[i][j]=BC6H_ramp[a][b][c][d][idx_1];
        }
    }
    return err_1 * numEntries;
}


inline int BC6H_cluster_mean_d_d (double d[][BC6H_MAX_DIMENSION_BIG],  double mean[][BC6H_MAX_DIMENSION_BIG], int index[],int i_comp[],int i_cnt[], int n, int dimension) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // unused index values are underfined
    int i,j,k;
    assert(n!=0);

    for(i=0;i< n;i++)
        for(j=0;j< dimension;j++) {
            assert(index[i]<BC6H_MAX_CLUSTERS_BIG);
            mean[index[i]][j] =0;
            i_cnt[index[i]]=0;
        }
    k=0;
    for(i=0;i< n;i++){
        for(j=0;j< dimension;j++) 
            mean[index[i]][j] +=d[i][j];
        if (i_cnt[index[i]]==0)
            i_comp[k++]=index[i];
        i_cnt[index[i]]++;
    }

    for(i=0;i< k;i++)
        for(j=0;j< dimension;j++)
            mean[i_comp[i]][j] /=(double) i_cnt[i_comp[i]];
    return k;
}


double BC6H_ep_shaker_2_d( 
                        double    data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG], 
                        int        numEntries, 
                        int        index_[BC6H_MAX_ENTRIES],
                        double    out[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],
                        int        epo_code[2][BC6H_MAX_DIMENSION_BIG],
                        int        size,
                        int        Mi_,             // last cluster
                        int        bits,            // total for all channels
                        int        dimension,
                        double    epo[2][BC6H_MAX_DIMENSION_BIG]
                        ) 
{
    int i,j,k;
    int max_bits[BC6H_MAX_DIMENSION_BIG];
    int type = bits % (2*dimension); 
    int use_par =(type !=0);
    for (j=0;j<dimension;j++) 
        max_bits[j] = (bits+2*dimension-1) / (2*dimension); 
    int clog=0;
    i = Mi_+1;
    while (i>>=1) 
        clog++;

    assert((1<<clog)== Mi_+1);
    double mean[BC6H_MAX_DIMENSION_BIG];
    int index[BC6H_MAX_ENTRIES];
    int Mi;
    int maxTry=8;

    for (k=0;k<numEntries;k++) 
        index[k]=index_[k];


    int done;
    int change;  
    int better;
    
    double err_o = DBL_MAX;
    int epo_0[2][BC6H_MAX_DIMENSION_BIG];
        
    double outg[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG];

    // handled below automatically
    int alls= BC6H_all_same_d(data, numEntries, dimension); 

    BC6H_mean_d_d (data, mean, numEntries, dimension); 

    do{
        BC6H_index_collapse_ (index, numEntries);

        Mi= BC6H_max_i(index, numEntries);  // index can be from requantizer

        int p, q;
        int p0=-1,q0=-1;
        
        double err_0 = DBL_MAX;

        if (Mi==0)
        {
            double t;
            // either single point from the beginning or collapsed index
            if (alls)
            {
                t =  BC6H_quant_single_point_d( data,numEntries,index, outg, epo_0,  Mi_, max_bits,type, dimension); 
            }
            else
            {
                BC6H_quant_single_point_d(&mean,numEntries,index, outg, epo_0, Mi_, max_bits, type, dimension); 
                t = BC6H_totalError_d(data,outg,numEntries, dimension);
            }

            if (t < err_o)
            {
                for (k=0;k<numEntries;k++)
                { 
                    index_[k]=index[k];
                    for (j=0;j<dimension;j++)
                    {
                        out[k][j]=outg[k][j];
                        epo_code[0][j]=epo_0[0][j];
                        epo_code[1][j]=epo_0[1][j];
                    }
                };
                err_o=t;
            }

            int a;
            int b;
            int c;
            int d;

            for(j=0;j<dimension;j++)
            {
                a = BC6H_CLT(clog);
                b = BC6H_BTT(max_bits[j]);
                c = epo_code[0][j];
                d = epo_code[1][j];
                if (d < 256)
                {
                    epo[0][j]= BC6H_ramp[a][b][c][d][0];
                    epo[1][j]= BC6H_ramp[a][b][c][d][(1<<clog)-1];
                }
            }

            return err_o;
        }

//          set stuff for  collapsed index shake (if needed)??
//          if it did collaps should we just check if it's better and get out ?

        assert(Mi <= Mi_);

        for (q=1; Mi!=0 && q*Mi <= Mi_; q++) // does not work for single point collapsed index!!! 
            for (p=0;p<=Mi_-q*Mi;p++)
            {           
                int cidx[BC6H_MAX_ENTRIES];

                for (k=0;k<numEntries;k++)
                    cidx[k]=index[k] * q + p;

            double epa[2][BC6H_MAX_DIMENSION_BIG];

            {
                //
                // solve RMS problem for center
                //

                double im [2][2] = {{0,0},{0,0}};   // matrix /inverse matrix
                double rp[2][BC6H_MAX_DIMENSION_BIG];            // right part for RMS fit problem

                // get ideal clustr centers
                double cc[BC6H_MAX_CLUSTERS_BIG][BC6H_MAX_DIMENSION_BIG];
                int i_cnt[BC6H_MAX_CLUSTERS_BIG]; // count of index entries
                int i_comp[BC6H_MAX_CLUSTERS_BIG];   // compacted index
                int ncl;                        // number of unique indexes

                ncl=BC6H_cluster_mean_d_d (data,  cc, cidx, i_comp, i_cnt, numEntries, dimension); // unrounded

                for (j=0;j<dimension;j++)
                    rp[0][j]=rp[1][j]=0;
                                                         
                // weight with cnt if runnning on compacted index
                for (k=0;k<numEntries;k++)
                {
                    im[0][0] += (Mi_-cidx[k])* (Mi_-cidx[k]);
                    im[0][1] +=      cidx[k] * (Mi_-cidx[k]);           // im is symmetric
                    im[1][1] +=      cidx[k] *      cidx[k];

                    for (j=0;j<dimension;j++)
                    {
                        rp[0][j]+=(Mi_-cidx[k]) * cc[cidx[k]][j];
                        rp[1][j]+=     cidx[k]  * cc[cidx[k]][j];
                    }
                }

                double dd = im[0][0]*im[1][1]-im[0][1]*im[0][1];

                assert(dd !=0);

                // dd=0 means that cidx[k] and (Mi_-cidx[k]) collinear which implies only one active index;
                // taken care of separately

                im[1][0]=im[0][0];
                im[0][0]=im[1][1]/dd;
                im[1][1]=im[1][0]/dd;
                im[1][0]=im[0][1]=-im[0][1]/dd;

                for (j=0;j<dimension;j++)
                {
                    epa[0][j]=(im[0][0]*rp[0][j]+im[0][1]*rp[1][j])*Mi_;
                    epa[1][j]=(im[1][0]*rp[0][j]+im[1][1]*rp[1][j])*Mi_;
                }
            }

            // shake single or                                   - cartesian
            // shake odd/odd and even/even or                    - same parity
            // shake odd/odd odd/even , even/odd and even/even   - bcc

            double err_1 = DBL_MAX;
            int epo_1[2][BC6H_MAX_DIMENSION_BIG];
            memset(epo_1, 0, sizeof(epo_1));
            
            double ed[2][2][BC6H_MAX_DIMENSION_BIG];
            int epo_2_[2][2][2][BC6H_MAX_DIMENSION_BIG];
            memset(epo_2_, 0, sizeof(epo_2_));

            for (j=0;j<dimension;j++)
            {
                double  (*rb) [256][16] =BC6H_ramp[BC6H_CLT(clog)][BC6H_BTT(max_bits[j])];

                int pp[2]={0,0};
                int rr = (use_par ? 2:1);

                int epi[2][2];  // first/second, coord, begin rage end range


                for (pp[0]=0;pp[0]<rr;pp[0]++) {
                    for (pp[1]=0;pp[1]<rr;pp[1]++) {

                        for (i=0;i<2;i++) {     // set range
                            epi[i][0]= epi[i][1]= BC6H_ep_find_floor( epa[i][j],max_bits[j], use_par, pp[i]);

                            epi[i][0]-= ( (epi[i][0]  < (size>>1)-1 ? epi[i][0] : (size>>1)-1 ) )  & (~use_par);

                            epi[i][1]+= ((1<<max_bits[j])-1 - epi[i][1]  < (size>>1) ? 
                                            (1<<max_bits[j])-1 - epi[i][1]  : (size>>1) ) & (~use_par);
                        }
                        int p1,p2, step=(1<<use_par);

                        ed[pp[0]][pp[1]][j]=DBL_MAX;

                        for (p1=epi[0][0];p1<=epi[0][1];p1+=step) 
                            for (p2=epi[1][0];p2<=epi[1][1];p2+=step)
                            {
                                double *rbp = rb[p1][p2];
                                double t=0;
                                int    *ci=cidx;
                                int    m =numEntries;


                                int _mc = m;

                                while(_mc > 0)
                                {
                                    t += (rbp[ci[_mc-1]]-data[_mc-1][j])*(rbp[ci[_mc-1]]-data[_mc-1][j]);
                                    _mc--;
                                }

                                if (t<ed[pp[0]][pp[1]][j]) {
                                    ed[pp[0]][pp[1]][j]=t;
                                    epo_2_[pp[0]][pp[1]][0][j]=p1;
                                    epo_2_[pp[0]][pp[1]][1][j]=p2;
                                }
                            }
                    }
                }
            }

            int pn;
            int (*pv)[BC6H_MAX_DIMENSION_BIG];
            for (pn=0;pn<BC6H_npv_nd[dimension][type];pn++)
            {
                pv = BC6H_par_vectors_nd[dimension][type][pn]; 
                int j1;
                double err_2=0;

                for (j1=0;j1<dimension;j1++) 
                    err_2+=ed[pv[0][j1]][pv[1][j1]][j1];

                if (err_2 < err_1) {
                    err_1 = err_2;
                    for (j1=0;j<dimension;j1++) {
                        epo_1[0][j1]=epo_2_[pv[0][j1]][pv[1][j1]][0][j1];
                        epo_1[1][j1]=epo_2_[pv[0][j1]][pv[1][j1]][1][j1];
                    }
                }
            }



            if (err_1 <= err_0) { // we'd want to get expanded index; 
                err_0 = err_1;
                p0=p;
                q0=q;
                for (j=0;j<dimension;j++) {
                    epo_0[0][j]=epo_1[0][j];
                    epo_0[1][j]=epo_1[1][j];
                }
            }
        }

        // requantize
        double *r[BC6H_MAX_DIMENSION_BIG];
        int idg[BC6H_MAX_ENTRIES];

        double err_r=0;

        for (j = 0; j < dimension; j++)
        {
            int a3   = BC6H_CLT(clog);
            int b4   = BC6H_BTT(max_bits[j]);
            int c256 = epo_0[0][j];
            int d256 = epo_0[1][j];

            r[j] = BC6H_ramp[a3][b4][c256][d256];
        }

        for (i=0;i<numEntries;i++)
        {
            double  cmin = DBL_MAX;
            int        ci = 0;
            double    *d=data[i];

            for(j=0; j < (1<<clog); j++)
            {
                double t_=0.;

                for(k=0;k<dimension;k++)
                {
                    if (r[k][j])
                    {
                        double rd = r[k][j];
                        rd = rd - d[k];
                        t_+=(rd)*(rd);
                    }
                }

                if(t_<cmin)
                {
                    cmin = t_;
                    ci = j;
                }
            }

            idg[i]=ci;
            for(k=0;k<dimension;k++)
            {
                outg[i][k]=r[k][ci];
            }

            err_r+=cmin;
        }

        // change/better
        change =0;
        for (k=0;k<numEntries;k++)
            change = change || (index[k] * q0 + p0!=idg[k]);

        better = err_r < err_o;

        if (better)
        {
            for (k=0;k<numEntries;k++)
            { 
                index_[k]=index[k]=idg[k];
                for (j=0;j<dimension;j++)
                {
                    out[k][j]=outg[k][j];
                    epo_code[0][j]=epo_0[0][j];
                    epo_code[1][j]=epo_0[1][j];
                }
            }
            err_o=err_r;
        }
        
    done = !(change  &&  better) ;

    } 
    while (! done && maxTry--);

    for(j=0;j<dimension;j++)
    {
        epo[0][j]= BC6H_ramp[BC6H_CLT(clog)][BC6H_BTT(max_bits[j])][epo_code[0][j]][epo_code[1][j]][0];
        epo[1][j]= BC6H_ramp[BC6H_CLT(clog)][BC6H_BTT(max_bits[j])][epo_code[0][j]][epo_code[1][j]][(1<<clog)-1];
    }

    return err_o;
}


double BC6H_ep_shaker_d( 
    double data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG], 
    int numEntries, 
    int index_[BC6H_MAX_ENTRIES],
    double out[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],
    int epo_code[2][BC6H_MAX_DIMENSION_BIG],
    int Mi_,                // last cluster
    int bits[4],            // including parity
    BC6H_qt type,
    int dimension
    ) 
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    //###############################
    int i,j,k;
    
    int use_par =(type == BC6H_BCC || type == BC6H_SAME_PAR);
    int bcc = (type == BC6H_BCC);
    int clog=0;
    i = Mi_+1;
    while (i>>=1) 
        clog++;

    assert((1<<clog)== Mi_+1);

    //###########################
    double mean[BC6H_MAX_DIMENSION_BIG];
    int index[BC6H_MAX_ENTRIES];
    int Mi;

    int maxTry=1;

    for (k=0;k<numEntries;k++) 
        index[k]=index_[k];

    int done;
    int change;  

    int better;  

    double err_o = DBL_MAX;

    // handled below automatically
    int alls= BC6H_all_same_d(data, numEntries, dimension); 

    BC6H_mean_d_d(data, mean, numEntries, dimension); 

    do{
        BC6H_index_collapse_ (index, numEntries);

        Mi= BC6H_max_i(index, numEntries);  // index can be from requantizer

        int p, q;
        int p0=-1,q0=-1;
        
        double err_2 = DBL_MAX;
        double out_2[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG];
        int idx_2[BC6H_MAX_ENTRIES];
        int    epo_2[2][BC6H_MAX_DIMENSION_BIG];

        if (Mi==0) {
            double t;
            int    epo_0[2][BC6H_MAX_DIMENSION_BIG];
            // either sinle point from the beginning or collapsed index
            if (alls) {
                t =  BC6H_quant_single_point_d( data,numEntries,index, out_2, epo_0, Mi_, bits,  type, dimension); 
            }
            else
            {
                BC6H_quant_single_point_d(&mean,numEntries,index, out_2, epo_0, Mi_, bits,  type, dimension); 
                t = BC6H_totalError_d(data,out_2,numEntries, dimension);
            }

            if (t < err_o) {
                for (k=0;k<numEntries;k++) { 
                    index_[k]=index[k];
                    for (j=0;j<dimension;j++) {
                        out[k][j]=out_2[k][j];
                        epo_code[0][j]=epo_0[0][j];
                        epo_code[1][j]=epo_0[1][j];
                    }
                };
                err_o=t;
            }
            return err_o;
        }


//          set stuff for  collapsed index shake (if needed)??
//            if it did collaps should we just check if it's better and get out ?

        for (q=1; Mi!=0 && q*Mi <= Mi_; q++) // does not work for single point collapsed index!!!
        {
            for (p=0;p<=Mi_-q*Mi;p++)
            {
                int cidx[BC6H_MAX_ENTRIES];
                
                for (k=0;k<numEntries;k++)
                {
                    cidx[k]=index[k] * q + p;
                }

                double epa[2][BC6H_MAX_DIMENSION_BIG];

                {   //
                    // solve RMS problem for center
                    //

                    double im [2][2] = {{0,0},{0,0}};   // matrix /inverse matrix
                    double rp[2][BC6H_MAX_DIMENSION_BIG];            // right part for RMS fit problem

                    // get ideal clustr centers
                    double cc[BC6H_MAX_CLUSTERS_BIG][BC6H_MAX_DIMENSION_BIG];
                    int i_cnt[BC6H_MAX_CLUSTERS_BIG]; // count of index entries
                    int i_comp[BC6H_MAX_CLUSTERS_BIG];   // compacted index
                    int ncl;                        // number of unique indexes

                    ncl=BC6H_cluster_mean_d_d (data,  cc, cidx, i_comp, i_cnt, numEntries, dimension); // unrounded

                    for (j=0;j<dimension;j++)
                    {
                        rp[0][j]=rp[1][j]=0;
                    }
                                                             
                    // weight with cnt if runnning on compacted index
                    for (k=0;k<numEntries;k++)
                    {
                        im[0][0] += (Mi_-cidx[k])* (Mi_-cidx[k]);
                        im[0][1] +=      cidx[k] * (Mi_-cidx[k]);           // im is symmetric
                        im[1][1] +=      cidx[k] *      cidx[k];

                        for (j=0;j<dimension;j++)
                        {
                            rp[0][j]+=(Mi_-cidx[k]) * cc[cidx[k]][j];
                            rp[1][j]+=     cidx[k]  * cc[cidx[k]][j];
                        }
                    }

                    double dd = im[0][0]*im[1][1]-im[0][1]*im[0][1];

                    assert(dd !=0);

                    // dd=0 means that cidx[k] and (Mi_-cidx[k]) collinear which implies only one active index;
                    // taken care of separately

                    im[1][0]=im[0][0];
                    im[0][0]=im[1][1]/dd;
                    im[1][1]=im[1][0]/dd;
                    im[1][0]=im[0][1]=-im[0][1]/dd;

                    for (j=0;j<dimension;j++) {
                        epa[0][j]=(im[0][0]*rp[0][j]+im[0][1]*rp[1][j])*Mi_;
                        epa[1][j]=(im[1][0]*rp[0][j]+im[1][1]*rp[1][j])*Mi_;
                    }
                }
                // shake single or                                   - cartesian
                // shake odd/odd and even/even or                    - same parity
                // shake odd/odd odd/even , even/odd and even/even   - bcc
                int odd,flip1;

                double err_1 = DBL_MAX;
                double out_1[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG];
                int idx_1[BC6H_MAX_ENTRIES];
                int epo_1[2][BC6H_MAX_DIMENSION_BIG];
                int s1 = 0;       

                  for (odd=0;odd<=use_par;odd++)
                {
                    for (flip1=0;flip1 <= bcc;flip1++)
                    {
                        // flip partiy on the second point
                        int epi[2][BC6H_MAX_DIMENSION_BIG][2];   // first second, coord, begin rage end range

                        for (j=0;j<dimension;j++)
                        {
                            int range; // this should be a double 
                            for (i=0;i<2;i++)
                            {   // set range
                                range = BC6H_ep_find_floor( epa[i][j], bits[j], use_par, (odd ^ (flip1 & i)) & 0x1);
                                epi[i][j][0]= epi[i][j][1] = range;

                                epi[i][j][1]+= ((1<<bits[j])-1 - epi[i][j][1]  < (1<<use_par) ? 
                                                (1<<bits[j])-1 - epi[i][j][1]  : (1<<use_par) ) & (~use_par);
                            }
                        }

                        double *r[BC6H_MAX_DIMENSION_BIG];

                        double ce[BC6H_MAX_ENTRIES][BC6H_MAX_CLUSTERS_BIG][BC6H_MAX_DIMENSION_BIG];

                        int a;
                        int b;
                        int c;
                        int d;

                        
                        for (j=0;j<dimension;j++) 
                        {
                            a = BC6H_CLT(clog);
                            b = BC6H_BTT(bits[j]);
                            c = epi[0][j][0];
                            if (c < 0) 
                                c = 0;
                            d = epi[1][j][0];
                            r[j]= BC6H_ramp[a][b][c][d];
                        }

                        double err_0 = 0;
                        double out_0[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG];
                        int idx_0[BC6H_MAX_ENTRIES];


                        for(i=0;i<numEntries;i++)
                        {
                            double *d1=data[i];
                            for(j=0;j<(1<<clog);j++)
                                for(k=0;k<dimension;k++)
                                {
                                    if (r[k][j])
                                    {
                                        double rd = r[k][j];
                                        rd = rd -d1[k];
                                        ce[i][j][k] = (rd)*(rd);
                                    }
                                }
                        }

                        int s=0, p1, g; 
                        int ei0=0, ei1=0;

                        for (p1 =0;p1<64 ;p1++)
                        {
                            int j0=0;

                            // Gray code increment
                            g = p1 & (-p1);

                            err_0=0;

                            for (j=0;j<dimension;j++)
                            {
                                if ( ((g >> (2 *j)) & 0x3) !=0)
                                {
                                    j0 =j;
                                    // new cords
                                    ei0 = ( ( (s^g) >>(2 *j))    & 0x1); 
                                    ei1 = ( ( (s^g) >>(2 *j+1))    & 0x1); 
                                }
                            }
                            s = s ^ g;
                            r[j0]= BC6H_ramp[BC6H_CLT(clog)][BC6H_BTT(bits[j0])][epi[0][j0][ei0]][epi[1][j0][ei1]];

                            err_0 = 0;

                            for (i=0;i<numEntries;i++)
                            {
                                double *d1=data[i];
                                int    ci = 0;
                                double cmin = DBL_MAX;

                                for(j=0;j<(1<<clog);j++)
                                {
                                    double t_ = 0.;
                                    ce[i][j][j0] = (r[j0][j]-d1[j0])*(r[j0][j]-d1[j0]);

                                    for(k=0;k<dimension;k++)
                                    {
                                        t_ += ce[i][j][k];
                                    }

                                    if(t_< cmin)
                                    {
                                        cmin = t_;
                                        ci = j;
                                    }
                                }

                                idx_0[i]=ci;
                                for(k=0;k<dimension;k++)
                                {
                                    out_0[i][k]=r[k][ci];
                                }
                                err_0+=cmin;
                            }

                            if (err_0 < err_1)
                            {
                            // best in the curent ep cube run
                                for (i=0; i < numEntries;i++)
                                {
                                    idx_1[i]=idx_0[i];
                                    for (j=0;j<dimension;j++) 
                                        out_1[i][j]=out_0[i][j];
                                }
                                err_1=err_0;

                                s1=s; // epo coding             
                            }
                        }

                        // reconstruct epo
                        for (j=0;j<dimension;j++)
                        {
                            {
                                // new cords
                                ei0 = ( ( s1 >>(2 *j)  )    & 0x1); 
                                ei1 = ( ( s1 >>(2 *j+1))    & 0x1); 
                                epo_1[0][j]=epi[0][j][ei0];
                                epo_1[1][j]=epi[1][j][ei1];
                            }
                        }
                    }
                }
                if (err_1 < err_2)
                {
                // best in the curent ep cube run
                    for (i=0; i < numEntries;i++)
                    {
                        idx_2[i]=idx_1[i];
                        for (j=0;j<dimension;j++) 
                            out_2[i][j]=out_1[i][j];
                    }
                    err_2=err_1;
                    for (j=0;j<dimension;j++)
                    {
                        epo_2[0][j]=epo_1[0][j];
                        epo_2[1][j]=epo_1[1][j];
                    }
                    p0=p;
                    q0=q;
                }
            }
        }

        // change/better
        change =0;
        for (k=0;k<numEntries;k++)  
            change = change || (index[k] * q0 + p0!=idx_2[k]);  

        better = err_2 < err_o;

        if (better)
        {
            for (k=0;k<numEntries;k++)
            { 
                index_[k]=index[k]=idx_2[k];
                for (j=0;j<dimension;j++)
                {
                    out[k][j]=out_2[k][j];
                    epo_code[0][j]=epo_2[0][j];
                    epo_code[1][j]=epo_2[1][j];
                }
            }
            err_o=err_2;
        }

    done = !(change  &&  better) ;

    } while (! done && maxTry--);

    return err_o;
}


