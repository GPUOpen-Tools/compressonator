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
////
//  BC6H_3dqunt_vpc.cpp : BC6H quantizer 
//
//  Revision
//  0.1    First implementation 
//

#include <assert.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#include "BC6H_3dquant_constants.h"
#include "BC6H_3dquant_vpc.h"
#include "bc6h_definitions.h"

struct TRACE         {int  k;  double d;};
struct RACE_CONSTR   {int  k;  double d1; double m; double M;};

#define USE_TRACE_WITH_DYNAMIC_MEM

#ifdef USE_TRACE_WITH_DYNAMIC_MEM
extern struct TRACE* amd_trs [BC6H_MAX_CLUSTERS][BC6H_MAX_ENTRIES_QUANT_TRACE];
extern int *amd_codes[BC6H_MAX_CLUSTERS][BC6H_MAX_ENTRIES_QUANT_TRACE];
#else
#define MAX_TRACE 250000
extern    TRACE    amd_trs[BC6H_MAX_CLUSTERS][BC6H_MAX_ENTRIES_QUANT_TRACE][MAX_TRACE];
extern     int        amd_codes[BC6H_MAX_CLUSTERS][BC6H_MAX_ENTRIES_QUANT_TRACE][MAX_TRACE];  // was static
#endif

static int trcnts[BC6H_MAX_CLUSTERS][BC6H_MAX_ENTRIES_QUANT_TRACE];

void BC6H_sugar(void){ 
#ifdef USE_DBGTRACE
    DbgTrace(("sugar!"))
#endif
};

inline int a_compare( const void *arg1, const void *arg2 )
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    return  ((BC6H_a* )arg1)->d-((BC6H_a* )arg2)->d > 0 ?  1 :
            ((BC6H_a* )arg1)->d-((BC6H_a* )arg2)->d < 0 ? -1 : 0;
};

void BC6H_quant_AnD_Shell(double* v_, int k, int n, int *idx) { 
    // input:
    //
    // v_  points, might be uncentered
    // k - number of points in the ramp
    // n - number of points in v_
    //
    // output:
    //
    // index, uncentered, in the range 0..k-1
    //
    #define BC6H_MAX_BLOCK BC6H_MAX_ENTRIES
    int i,j;
    double v[BC6H_MAX_BLOCK];
    double z[BC6H_MAX_BLOCK];
    BC6H_a d[BC6H_MAX_BLOCK];
    double l;
    double mm;
    double r=0;
    int mi;

    assert((v_ != NULL) && (n>1) && (k>1));

    double m, M, s, dm=0.;
    m=M=v_[0]; 
    
    for (i=1; i < n;i++) {
        m = m < v_[i] ? m : v_[i];
        M = M > v_[i] ? M : v_[i];
    }
    if (M==m) {
        for (i=0; i < n;i++) 
            idx[i]=0;
        return;
    }

    assert(M-m >0);
    s = (k-1)/(M-m);
    for (i=0; i < n;i++) {
        v[i] = v_[i]*s;

        idx[i]=(int)(z[i] = floor(v[i] +0.5 /* stabilizer*/ - m *s));

        d[i].d = v[i]-z[i]- m *s;
        d[i].i = i; 
        dm+= d[i].d;
        r += d[i].d*d[i].d;
    }
    if (n*r- dm*dm >= (double)(n-1)/4 /*slack*/ /2) { 

        dm /= (double)n;

        for (i=0; i < n;i++) 
            d[i].d -= dm;

        qsort((void*)&d, n, sizeof(BC6H_a),a_compare);

    // got into fundamental simplex
    // move coordinate system origin to its center
        for (i=0; i < n;i++) 
            d[i].d -= (2.*(double)i+1-(double)n)/2./(double)n;

        mm=l=0.;
        j=-1;
        for (i=0; i < n;i++) {
            l+=d[i].d;
            if (l < mm) {
                mm =l;
                j=i;
            }
        }

    // position which should be in 0 
        j = ++j % n;

        for (i=j; i < n;i++) 
            idx[d[i].i]++;
    }
// get rid of an offset in idx
    mi=idx[0];
    for (i=1; i < n;i++) 
        mi = mi < idx[i]? mi :idx[i];

    for (i=0; i < n;i++) 
        idx[i]-=mi;
}

double BC6H_totalError_d(double data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],double data2[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],int numEntries, int dimension)
{

    double        partition[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG];
    double        partitionCmp[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG];

    memcpy(partition, data, sizeof(partition));
    memcpy(partitionCmp, data2, sizeof(partitionCmp));

#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int i,j;
    double t=0;
    for (i = 0; i < numEntries; i++)
    {
            // Get the color diff (RGB or RGBA depending on dimension = for BC6H its 3)
            float diff;
            for (j = 0; j < dimension; j++)
            {
                diff = (float)(data[i][j] - data2[i][j]);
                t += diff * diff;
            }
    }

    return t;
};

//
// We ignore the issue of ordering equal elements here, though it can affect results abit
//
void BC6H_sortProjection(double projection[BC6H_MAX_ENTRIES], int order[BC6H_MAX_ENTRIES], int numEntries) 
{
    int i;
    BC6H_a what[BC6H_MAX_ENTRIES+BC6H_MAX_PARTITIONS_TABLE];
         
    for (i=0; i < numEntries;i++) 
        what[what[i].i=i].d = projection[i];

    qsort((void*)&what, numEntries, sizeof(BC6H_a),a_compare);
    
    for (i=0; i < numEntries;i++) 
        order[i]=what[i].i;
};

void BC6H_project_d(double data[][BC6H_MAX_DIMENSION_BIG], int numEntries, double vector[BC6H_MAX_DIMENSION_BIG], double projection[BC6H_MAX_ENTRIES], int dimension)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // assume that vector is normalized already
    int i,k;

    for(k=0;k<numEntries;k++)
    {
        projection[k]=0;
        for(i=0;i<dimension;i++)
        {
             projection[k]+=data[k][i]*vector[i];
        }
    }
}

void BC6H_eigenVector_d(double cov[BC6H_MAX_DIMENSION_BIG][BC6H_MAX_DIMENSION_BIG], double vector[BC6H_MAX_DIMENSION_BIG], int dimension)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    // calculate an eigenvecto corresponding to a biggest eigenvalue
    // will work for non-zero non-negative matricies only

    #define EV_ITERATION_NUMBER 20
    #define EV_SLACK            2        /* additive for exp base 2)*/    


    int i,j,k,l, m, n,p,q;
    double c[2][BC6H_MAX_DIMENSION_BIG][BC6H_MAX_DIMENSION_BIG];
    double maxDiag;

    for(i=0;i<dimension;i++)
        for(j=0;j<dimension;j++)
            c[0][i][j] =cov[i][j];

    p = (int) floor(log( (DBL_MAX_EXP - EV_SLACK) / ceil (log((double)dimension)/log(2.)) )/log(2.)); 

    assert(p>0);

    p = p >0 ? p : 1;

    q =  (EV_ITERATION_NUMBER+p-1) / p;
    
    l=0;

    for(n=0;n<q; n++)
    {
        maxDiag = 0;

        for(i=0;i<dimension;i++)
            maxDiag = c[l][i][i] > maxDiag ? c[l][i][i] : maxDiag;

        if (maxDiag<=0)
        {
            BC6H_sugar();
            return;
        }
        assert(maxDiag >0);
    
        for(i=0;i<dimension;i++)
            for(j=0;j<dimension;j++)
                c[l][i][j] /=maxDiag;

        for(m=0;m<p;m++) {
            for(i=0;i<dimension;i++)
                for(j=0;j<dimension;j++) {
                    c[1-l][i][j]=0;
                    for(k=0;k<dimension;k++)
                        c[1-l][i][j]+=c[l][i][k]*c[l][k][j];
                }
            l=1-l;
        }
    }
    
    maxDiag = 0;
    k =0;

    for(i=0;i<dimension;i++)
    {
         k = c[l][i][i] > maxDiag ? i : k;
         maxDiag = c[l][i][i] > maxDiag ? c[l][i][i] : maxDiag;
    }
    double t;
    t=0;
    for(i=0;i<dimension;i++)
    {
        t+=c[l][k][i]*c[l][k][i];
        vector[i]=c[l][k][i];
    }
    // normalization is really optional
    t= sqrt(t);
    assert(t>0);
    if (t<=0)
    {
        BC6H_sugar();
        return;
    }    
    for(i=0;i<dimension;i++) 
        vector[i]/=t;
}

void BC6H_covariance_d(double data[][BC6H_MAX_DIMENSION_BIG], int numEntries, double cov[BC6H_MAX_DIMENSION_BIG][BC6H_MAX_DIMENSION_BIG], int dimension)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    int i,j,k;

    for(i=0;i<dimension;i++)
        for(j=0;j<=i;j++)
        {
            cov[i][j]=0;
            for(k=0;k<numEntries;k++)
                cov[i][j]+=data[k][i]*data[k][j];
        }
        
    for(i=0;i<dimension;i++)
        for(j=i+1;j<dimension;j++)
            cov[i][j] = cov[j][i];
}

void BC6H_centerInPlace_d(double data[][BC6H_MAX_DIMENSION_BIG], int numEntries, double mean[BC6H_MAX_DIMENSION_BIG], int dimension)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int i,k;

    for(i=0;i<dimension;i++)
    {
            mean[i]=0;
            for(k=0;k<numEntries;k++)
                mean[i]+=data[k][i];
    }

    if (!numEntries)
        return;

    for(i=0;i<dimension;i++)
    {
        mean[i]/=(double) numEntries;
        for(k=0;k<numEntries;k++)
            data[k][i]-=mean[i];
    }
}

void BC6H_optQuantAnD_d(
    double data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG], 
    int numEntries, int numClusters, int index[BC6H_MAX_ENTRIES],
    double out[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],
    double direction [BC6H_MAX_DIMENSION_BIG],
    double *step,
    int dimension
    )
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int index_[BC6H_MAX_ENTRIES];

    int maxTry=BC6H_MAX_TRY*10;
    int try_two=50;

    int i,j,k;
    double t,s;

    double centered[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG];

    double mean[BC6H_MAX_DIMENSION_BIG];

    double cov[BC6H_MAX_DIMENSION_BIG][BC6H_MAX_DIMENSION_BIG];

    double projected[BC6H_MAX_ENTRIES];

    int order_[BC6H_MAX_ENTRIES];


    for (i=0;i<numEntries;i++) 
       for (j=0;j<dimension;j++) 
            centered[i][j]=data[i][j];

    BC6H_centerInPlace_d(centered, numEntries, mean, dimension);
    BC6H_covariance_d(centered, numEntries, cov, dimension);

    // check if they all are the same 

    t=0;
    for (j=0;j<dimension;j++) 
         t+= cov[j][j];

    if (t<(1./256.) || numEntries==0) {
        for (i=0;i<numEntries;i++) {
            index[i]=0;
            for (j=0;j<dimension;j++) 
                out[i][j]=mean[j];
        }
        return;
    }

    BC6H_eigenVector_d(cov, direction, dimension);    
    BC6H_project_d(centered, numEntries, direction, projected, dimension); 

    for (i=0;i<maxTry;i++)
    {
        int done =0;

        if (i)
        {
            do
            {
                double q;
                q=s=t=0;

                for (k=0;k<numEntries;k++)
                { 
                    s+= index[k];
                    t+= index[k]*index[k];
                }

                for (j=0;j<dimension;j++)
                {
                    direction[j]=0;
                    for (k=0;k<numEntries;k++) 
                        direction[j]+=centered[k][j]*index[k];
                    q+= direction[j]* direction[j];

                }

                s /= (double) numEntries;
                t = t - s * s * (double) numEntries;
                assert(t !=0);
                t = (t == 0 ? 0. : 1/t);
                // We need to requantize 
                
                q = sqrt(q); 
                t *=q;

                if (q !=0)
                    for (j=0;j<dimension;j++) 
                        direction[j]/=q;

                // direction normalized

                BC6H_project_d(centered, numEntries, direction, projected, dimension); 
                BC6H_sortProjection(projected, order_, numEntries);

                int index__[BC6H_MAX_ENTRIES];

                // it's projected and centered; cluster centers are (index[i]-s)*t (*dir)
                k=0;
                for (j=0; j < numEntries;j++)
                {
                    while (projected[order_[j]] > (k+0.5 -s)*t  && k < numClusters-1) 
                        k++;
                    index__[order_[j]]=k;
                }
                done =1;
                for (j=0; j < numEntries;j++)
                {
                    done = (done && (index__[j]==index[j]));
                    index[j]=index__[j];
                }
            } while (! done && try_two--);

            if (i==1) 
                for (j=0; j < numEntries;j++) 
                    index_[j]=index[j];
            else
            {
                done =1;
                for (j=0; j < numEntries;j++)
                {
                    done = (done && (index_[j]==index[j]));
                    index_[j]=index_[j];
                }
                if (done) 
                    break;

            }
        }
   
        BC6H_quant_AnD_Shell(projected,  numClusters,numEntries, index);  
    }
    s=t=0;
    
    double q=0;

    for (k=0;k<numEntries;k++)
    { 
        s+= index[k];
        t+= index[k]*index[k];
    }

    for (j=0;j<dimension;j++)
    {
        direction[j]=0;
        for (k=0;k<numEntries;k++) 
            direction[j]+=centered[k][j]*index[k];
        q+= direction[j]* direction[j];
    }

    s /= (double) numEntries;

    t = t - s * s * (double) numEntries;

    assert(t !=0);   

    t = (t == 0 ? 0. : 1/t);
    
    for (i=0;i<numEntries;i++) 
            for (j=0;j<dimension;j++) 
                out[i][j]=mean[j]+direction[j]*t*(index[i]-s);

    // normalize direction for output
    q=sqrt(q);
    *step=t*q;
    for (j=0;j<dimension;j++) 
        direction[j]/=q;

    return;
}

void BC6H_quantTrace_d(double data[BC6H_MAX_ENTRIES_QUANT_TRACE][BC6H_MAX_DIMENSION_BIG],int numEntries, int numClusters, int index[BC6H_MAX_ENTRIES_QUANT_TRACE],int dimension)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Data should be centered, otherwise will not work  

    int i,j,k;
         
    double sdata[2*BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG];

    double  dpAcc [BC6H_MAX_DIMENSION_BIG];

    double M =0;

    struct TRACE  *tr ;
    tr=amd_trs[numClusters-1][numEntries-1];

    int trcnt =trcnts[numClusters-1][numEntries-1];

    int *code;
    code=amd_codes[numClusters-1][numEntries-1];    

    for (i=0;i<numEntries;i++)
        for (j=0;j<dimension;j++)
        {
            sdata[2*i][j]= data[i][j];
            sdata[2*i+1][j]=-data[i][j];
        }

    for (j=0;j<dimension;j++) 
        dpAcc[j]=0;

    k=-1;

#define UROLL_STEP_1(i) \
    dpAcc[0]+=sdata[tr[i].k][0];\
    {\
        double c; \
        c = (dpAcc[0]*dpAcc[0])*tr[i].d;\
        if (c > M) {k=i;M=c;};\
    };

#define UROLL_STEP_2(i) \
    dpAcc[0]+=sdata[tr[i].k][0];\
    dpAcc[1]+=sdata[tr[i].k][1];\
    { double c; \
    c = (dpAcc[0]*dpAcc[0]+dpAcc[1]*dpAcc[1])*tr[i].d;\
    if (c > M) {k=i;M=c;};};

#define UROLL_STEP_3(i) \
    dpAcc[0]+=sdata[tr[i].k][0];\
    dpAcc[1]+=sdata[tr[i].k][1];\
    dpAcc[2]+=sdata[tr[i].k][2];\
    { double c; \
    c = (dpAcc[0]*dpAcc[0]+dpAcc[1]*dpAcc[1]+dpAcc[2]*dpAcc[2])*tr[i].d;\
    if (c > M) {k=i;M=c;};};

#define UROLL_STEP_4(i) \
    dpAcc[0]+=sdata[tr[i].k][0];\
    dpAcc[1]+=sdata[tr[i].k][1];\
    dpAcc[2]+=sdata[tr[i].k][2];\
    dpAcc[3]+=sdata[tr[i].k][3];\
    { double c; \
    c = (dpAcc[0]*dpAcc[0]+dpAcc[1]*dpAcc[1]+dpAcc[2]*dpAcc[2]+dpAcc[3]*dpAcc[3])*tr[i].d;\
    if (c > M) {k=i;M=c;};};

#undef UROLL_STEP

#define UROLL_MACRO(UROLL_STEP){\
\
\
    for (i=0;i+15<trcnt;i+=16)\
    {\
        UROLL_STEP(i)\
        UROLL_STEP(i+1)\
        UROLL_STEP(i+2)\
        UROLL_STEP(i+3)\
        UROLL_STEP(i+4)\
        UROLL_STEP(i+5)\
        UROLL_STEP(i+6)\
        UROLL_STEP(i+7)\
        UROLL_STEP(i+8)\
        UROLL_STEP(i+9)\
        UROLL_STEP(i+10)\
        UROLL_STEP(i+11)\
        UROLL_STEP(i+12)\
        UROLL_STEP(i+13)\
        UROLL_STEP(i+14)\
        UROLL_STEP(i+15)\
    }\
\
    for (;i<trcnt;i++) {\
        UROLL_STEP(i)\
    }};

    switch(dimension)
    {
        case    1:
            UROLL_MACRO(UROLL_STEP_1);
            break;
        case    2:
            UROLL_MACRO(UROLL_STEP_2);
            break;
        case    3:
            UROLL_MACRO(UROLL_STEP_3);
            break;
        case    4:
            UROLL_MACRO(UROLL_STEP_4);
            break;
        default:
            return;
            break;
    }


    if (k<0)
    {

#ifdef USE_DBGTRACE
        DbgTrace(("ERROR: quatnTrace\n"));
#endif
        return;
    }

    k = code[k];
    i=0;
    for (j=0;j<numEntries;j++)
    {
        while ((k & 1) ==0)
        {
            i++;
            k>>=1;
        }
        index[j]=i;

        k>>=1;
    }
}


double BC6H_optQuantTrace_d(
    double data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG], 
    int numEntries, int numClusters, int index_[BC6H_MAX_ENTRIES],
    double out[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],
    double direction [BC6H_MAX_DIMENSION_BIG],double *step,
    int dimension
    )
{

#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int index[BC6H_MAX_ENTRIES];
    int maxTry=BC6H_MAX_TRY;
    int i,j,k;
    double t,s;
    double centered[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG];
    double ordered[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG];
    double mean[BC6H_MAX_DIMENSION_BIG];
    double cov[BC6H_DIMENSION][BC6H_MAX_DIMENSION_BIG];
    double projected[BC6H_MAX_ENTRIES];
    int order[BC6H_MAX_ENTRIES];

    for (i=0;i<numEntries;i++) 
       for (j=0;j<dimension;j++) 
            centered[i][j]=data[i][j];

    BC6H_centerInPlace_d(centered, numEntries, mean, dimension);
    BC6H_covariance_d(centered, numEntries, cov, dimension);

    // check if they all are the same 

    t=0;
    for (j=0;j<dimension;j++) 
         t+= cov[j][j];

    if (t<BC6H_EPSILON || numEntries==0) {
        for (i=0;i<numEntries;i++) {
            index_[i]=0;
            for (j=0;j<dimension;j++) 
                out[i][j]=mean[j];
        }
        return 0.;
    }


    BC6H_eigenVector_d(cov, direction, dimension);    
    BC6H_project_d(centered, numEntries, direction, projected, dimension); 

    for (i=0;i<maxTry;i++)
    {
        if (i)
        {
            t=0;
            for (j=0;j<dimension;j++)
            {
                direction[j]=0;
                for (k=0;k<numEntries;k++) 
                   direction[j]+=ordered[k][j]*index[k]; 
                t+=direction[j]*direction[j];
            }

            // Actually we don't need to normailize direction here, as the 
            // optimal quntization (index) is invariant of the scale.
            // Hence we don't care about possible degenration of the <direction> either
            // though normally it should not happen

            // However, the EPSILON should be scaled, otherwise is does not make sense

            t = sqrt(t)*BC6H_EPSILON;

            BC6H_project_d(centered, numEntries, direction, projected, dimension); 

            for (j=1; j < numEntries;j++) 
                if (projected[order[j]] < projected[order[j-1]]-t /*EPSILON*/) 
                    break;

            if (j >= numEntries)
                break;
        }

        BC6H_sortProjection(projected, order, numEntries);

        for (k=0;k<numEntries;k++)     
            for (j=0;j<dimension;j++) 
                ordered[k][j]=centered[order[k]][j];

        BC6H_quantTrace_d(ordered, numEntries, numClusters, index, dimension);
    }

    s=t=0;
    
    double q=0;

    for (k=0;k<numEntries;k++)
    { 
        s+= index[k];
        t+= index[k]*index[k];
    }

    for (j=0;j<dimension;j++)
    {
        direction[j]=0;
        for (k=0;k<numEntries;k++) 
            direction[j]+=ordered[k][j]*index[k];
        q+= direction[j]* direction[j];

    }   

    s /= (double) numEntries;

    t = t - s * s * (double) numEntries;

    assert(t !=0);   

    t = (t == 0 ? 0. : 1/t);
    
    for (i=0;i<numEntries;i++)
    {
        for (j=0;j<dimension;j++) 
            out[order[i]][j]=mean[j]+direction[j]*t*(index[i]-s);
        index_[order[i]]=index[i];
    }

    // normalize direction for output
    
    q=sqrt(q);
    *step=t*q;

    for (j=0;j<dimension;j++) 
        direction[j]/=q;
    
    return BC6H_totalError_d(data,out,numEntries, dimension);  
}


