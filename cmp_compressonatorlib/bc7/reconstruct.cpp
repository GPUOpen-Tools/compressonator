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

#include <math.h>
#include <string.h>
#include <assert.h>
#include "3dquant_constants.h"
#include "3dquant_vpc.h"
#include "reconstruct.h"
#include "debug.h"


static double maxStep[MAX_CLUSTERS] = {0,0,0,0,0,0,0,0};
static double maxStep1[MAX_CLUSTERS]= {0,0,0,0,0,0,0,0};
static double minStep[MAX_CLUSTERS] = {1000,1000,1000,1000,1000,1000,1000,1000};
static double minStep1[MAX_CLUSTERS]= {1000,1000,1000,1000,1000,1000,1000,1000};

static int stepHisto[512*16];
static int stepHisto1[512*16];
static int stepHistoI[MAX_SUBSETS][512*16];

static int histoInit =0;

static double NShakeCnt=0;
static double PCnt=0;

void histoStep (double step, double step1) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    if (histoInit==0) {
        int i;
        for (i=0; i<512*16; i++)
            stepHisto[i]=stepHisto1[i]=0;
        histoInit=1;
    }
    step *= 16;
    step = step <0 ? 0 : step;
    step = step >512.*16 ? 512.*16 : step;
    stepHisto[(int)floor(step+1/2.)]++;

    step1 *= 16;
    step1 = step1 <0 ? 0 : step1;
    step1 = step1 >512.*16 ? 512.*16 : step1;
    stepHisto1[(int)floor(step1+1/2.)]++;
};


void histoStepCnt (int id, int cnt, double step1) {
    if (histoInit==0) {
        int i;
        for (i=0; i<512*16; i++)
            stepHisto[i]=stepHisto1[i]=0;
        histoInit=1;
    }

    step1 *= 16;
    step1 = step1 <0 ? 0 : step1;
    step1 = step1 >512.*16 ? 512.*16 : step1;
    stepHistoI[id][(int)floor(step1+1/2.)]+=cnt;
};


void printStepHistoI (void) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int i,j,k,l;

    for (l=0; l<MAX_SUBSETS; l++) {
        k=0;
        printf("[\n");
        for (i=0; i<512*16/32; i++) {
            for (j=0; j<32; j++)
                printf("%d, ",stepHistoI[l][k++]);
            printf("\n");
        }

        printf("]\n");
    }
}

void printStepHisto (void) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int i,j,k;
    k=0;
    printf("[\n");
    for (i=0; i<512*16/32; i++) {
        for (j=0; j<32; j++)
            printf("%d, ",stepHisto[k++]);
        printf("\n");
    }

    printf("]\n");

    k=0;
    printf("[\n");
    for (i=0; i<512*16/32; i++) {
        for (j=0; j<32; j++)
            printf("%d, ",stepHisto1[k++]);
        printf("\n");
    }

    printf("]\n");
}


void printStep (void) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int i;
    for(i=0; i<MAX_CLUSTERS; i++)
        printf("step %2d %10g %10g  %10g %10g \n",i,minStep[i],minStep1[i],maxStep[i],maxStep1[i]);
}
void printCnt (void) {
    printf("total pix %g rounded %g percentage %g\n",PCnt,PCnt-NShakeCnt,(PCnt-NShakeCnt)/PCnt);
}


void index_collapse  // assymtric of x->n-x, but this does not matter below
(int index[], int numEntries) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int k;
    int d,D;

    int mi;
    int Mi;

    if (numEntries ==0)
        return;

    mi=Mi=index[0];

    for (k=0; k<numEntries; k++) {
        mi = mi < index[k] ? mi : index[k];
        Mi = Mi > index[k] ? Mi : index[k];
    }

    D=1;

    for (d=2; d<=Mi; d++) {

        for (k=0; k<numEntries; k++)
            if ((index[k] -mi) % d !=0)
                break;

        if (k>=numEntries)
            D =d;
    }

    for (k=0; k<numEntries; k++)
        index[k] = (index[k]- mi)/D;
}

void index_expand  // assymtric of x->n-x, but this does not matter below
(int index[], int numEntries, int  max_clusters) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int k;
    int d;

    int Mi=0,mi=max_clusters-1;

    for (k=0; k<numEntries; k++) {
        mi = mi < index[k] ? mi : index[k];
        Mi = Mi > index[k] ? Mi : index[k];
    }


    d = Mi-mi == 0 ? 1 : (max_clusters-1)/(Mi-mi);

    for (k=0; k<numEntries; k++)
        index[k] = (index[k]-mi)*d;
}

void sHisto (int index[], int numEntries, double step, double step1) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int Mi=0;
    int k;
    for (k=0; k<numEntries; k++)
        Mi = Mi > index[k] ? Mi : index[k];


    if (step!=0.)
        minStep[Mi]  = minStep[Mi] <  step ? minStep[Mi] : step;

    maxStep[Mi]  = maxStep[Mi] >  step ? maxStep[Mi] : step;


    if (step1!=0.)
        minStep1[Mi] = minStep1[Mi] < step1 ? minStep1[Mi] : step1;

    if (step1!=0.)
        maxStep1[Mi] = maxStep1[Mi] > step1 ? maxStep1[Mi] : step1;
}




double reconstruct(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],int ns,
    double direction [DIMENSION],double *step
) {

#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    double s=0,t=0, q=0;
    int i,j,k;
    double mean[DIMENSION];
    int index[MAX_ENTRIES];

    for (k=0; k<numEntries; k++)
        index[k]=index_[k];

// expand/collapse if needed

    if (ns ==1) // 3 bit index
        index_expand (index, numEntries, 16);
    else
        index_expand (index, numEntries, MAX_CLUSTERS);

    for (j=0; j<DIMENSION; j++) {
        for (mean[j]=k=0; k<numEntries; k++)
            mean[j]+=data[k][j];
        mean[j]/=(double) numEntries;
    }


    for (k=0; k<numEntries; k++) {
        s+= index[k];
        t+= index[k]*index[k];
    }

    double q1 =0;

    for (j=0; j<DIMENSION; j++) {
        direction[j]=0;
        for (k=0; k<numEntries; k++)
            direction[j]+=(data[k][j]-mean[j])*index[k];

        q+= direction[j]* direction[j];
        q1 = fabs(direction[j]) > q1 ? fabs(direction[j]) : q1;

    }

    s /= (double) numEntries;

    t = t - s * s * (double) numEntries;

    t = (t == 0 ? 0. : 1/t);

    q=sqrt(q);

    *step=t*q;

    for (j=0; j<DIMENSION; j++) {
        direction[j]/= (q1 == 0 ? 1:q1) ;
        if (direction[j]>=1.0001)
            printf("error\n");
    }

    if (*step !=0)
        *step*=q1/q;

    for (i=0; i<numEntries; i++)
        for (j=0; j<DIMENSION; j++)
            out[i][j]=mean[j]+direction[j]* (*step) *(index[i]-s);

    // normalize direction for output

    return totalError(data,out,numEntries);
}

double reconstruct_new(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],int ns,
    double direction [DIMENSION],double *step
) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    double s=0,t=0, q=0;
    int i,j,k;
    double mean[DIMENSION];

// expand/collapse if needed

    if (ns ==1) // 3 bit indes
        index_expand (index, numEntries, 16);
    else
        index_expand (index, numEntries, MAX_CLUSTERS);

    for (j=0; j<DIMENSION; j++) {
        for (mean[j]=k=0; k<numEntries; k++)
            mean[j]+=data[k][j];
        mean[j]/=(double) numEntries;
    }


    for (k=0; k<numEntries; k++) {
        s+= index[k];
        t+= index[k]*index[k];
    }

    double q1 =0;

    for (j=0; j<DIMENSION; j++) {
        direction[j]=0;
        for (k=0; k<numEntries; k++)
            direction[j]+=(data[k][j]-mean[j])*index[k];

        q+= direction[j]* direction[j];
        q1 = fabs(direction[j]) > q1 ? fabs(direction[j]) : q1;

    }

    s /= (double) numEntries;

    t = t - s * s * (double) numEntries;

    t = (t == 0 ? 0. : 1/t);

    q=sqrt(q);

    *step=t*q;


    for (j=0; j<DIMENSION; j++) {
        direction[j]/= (q1 == 0 ? 1:q1) ;
        if (direction[j]>=1.0001)
            printf("error\n");
    }

    double step_l2=*step;

    if (*step !=0)
        *step*=q1/q;

    double step_li=*step;

    sHisto ( index,  numEntries,  step_l2,  step_li);
    histoStep (step_l2, step_li);

    for (i=0; i<numEntries; i++)
        for (j=0; j<DIMENSION; j++)
            out[i][j]=mean[j]+direction[j]* (*step) *(index[i]-s);

    return totalError(data,out,numEntries);
}

void shake(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, double idx_mean,

    double dir_r[4][DIMENSION],
    double mean_r[8][DIMENSION],
    double step_r[2],

    int index_r[MAX_ENTRIES],
    double out [MAX_ENTRIES][DIMENSION])

{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    double c[MAX_CLUSTERS][DIMENSION];

    double t2[MAX_CLUSTERS];

    int index_t[MAX_ENTRIES];

    double s=idx_mean;

    double me=512*512*3.*16.;

    int p,q,r;
    int i,j,k;

    for (p=0; p<4; p++)
        for (q=0; q<8; q++)
            for (r=0; r<2; r++) {
                // generate sequence
                for (i=0; i<MAX_CLUSTERS; i++) {
                    t2[i] = 0;
                    for (j=0; j<DIMENSION; j++) {
                        c[i][j]=floor(mean_r[q][j]+dir_r[p][j]* step_r[r]*(i-s) +1/2.);

                        t2[i]+=c[i][j]*c[i][j];
                    }
                }
                double ea = 0;

                for (i=0; i < numEntries; i++) {
                    double e = t2[0]+data[i][0]*c[0][0]+data[i][1]*c[0][1]+data[i][2]*c[0][2];
                    index_t[i]=0;

                    for (k=1; k<MAX_CLUSTERS; k++) {
                        double t = t2[0]+data[i][0]*c[k][0]+data[i][1]*c[k][1]+data[i][2]*c[k][2];

                        if (t<e) {
                            index_t[i]=i;
                            e =t;
                        }
                    }
                    ea +=e;
                }
                if (ea < me) {
                    me =ea;
                    for (i=0; i <numEntries; i++)
                        index_r[i]=index_t[i];
                }
            }
    for (i=0; i <numEntries; i++)
        for (j=0; j<DIMENSION; j++)
            out[i][j]=c[index_r[i]][j];
}


void shake_d_s(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, double idx_mean,

    double dir_step_r[2][4][DIMENSION],
    double mean_r[8][DIMENSION],

    int index_r[MAX_ENTRIES],
    double out [MAX_ENTRIES][DIMENSION])

{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
#define MAX_CLUSTERS_1_16 16

    double c[MAX_CLUSTERS_1_16][DIMENSION];

    double t2[MAX_CLUSTERS_1_16];

    int index_t[MAX_ENTRIES];

    double s=idx_mean;

    double me=512*512*3.*16.;
    double ea = 0;
    int p,q,r;
    int i,j,k;

    for (p=0; p<4; p++)
        for (q=0; q<8; q++)
            for (r=0; r<2; r++) {
                // generate sequence
                for (i=0; i<MAX_CLUSTERS_1_16; i++) {
                    t2[i] = 0;
                    for (j=0; j<DIMENSION; j++) {
                        c[i][j]=floor(mean_r[q][j]+dir_step_r[r][p][j]*(i-s) +1./2.);
                        c[i][j]= c[i][j]>0 ? c[i][j]:0;
                        c[i][j]= c[i][j]<255. ? c[i][j]:255.;


                        t2[i]+=c[i][j]*c[i][j]/2; // negative
                    }
                }

                ea=0;

                for (i=0; i < numEntries; i++) {
                    double e = t2[0]-data[i][0]*c[0][0]-data[i][1]*c[0][1]-data[i][2]*c[0][2];
                    index_t[i]=0;

                    for (k=1; k<MAX_CLUSTERS_1_16; k++) {
                        double t = t2[k]-data[i][0]*c[k][0]-data[i][1]*c[k][1]-data[i][2]*c[k][2];

                        if (t<e) {
                            index_t[i]=k;
                            e =t;
                        }
                    }
                    ea +=e;
                }
                if (ea < me) {
                    me =ea;
                    int i1,j1;

                    for (i1=0; i1 <numEntries; i1++) {
                        index_r[i1]=index_t[i1];
                        for (j1=0; j1<DIMENSION; j1++)
                            out[i1][j1]=c[index_r[i1]][j1];
                    }

                }
            }
}


void shake_d_s_s(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int numClusters, double idx_mean,

    double dir_step_r[2][4][DIMENSION],
    double mean_step_r[2][8][DIMENSION],

    int index_r[MAX_ENTRIES],
    double out [MAX_ENTRIES][DIMENSION])

{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    double c[MAX_CLUSTERS][DIMENSION];

    double t2[MAX_CLUSTERS];

    int index_t[MAX_ENTRIES];

    double s=idx_mean;

    double me=512*512*3.*16.;
    double ea = 0;
    int p,q,r;
    int i,j,k;

    for (p=0; p<4; p++)
        for (q=0; q<8; q++)
            for (r=0; r<2; r++) {
                // generate sequence
                for (i=0; i<numClusters; i++) {
                    t2[i] = 0;
                    for (j=0; j<DIMENSION; j++) {
                        c[i][j]=floor(mean_step_r[r][q][j]+dir_step_r[r][p][j]*(i-s) +1./2.);
                        c[i][j]= c[i][j]>0 ? c[i][j]:0;
                        c[i][j]= c[i][j]<255. ? c[i][j]:255.;


                        t2[i]+=c[i][j]*c[i][j]/2; // negative
                    }
                }
                ea=0;

                for (i=0; i < numEntries; i++) {
                    double e = t2[0]-data[i][0]*c[0][0]-data[i][1]*c[0][1]-data[i][2]*c[0][2];
                    index_t[i]=0;

                    for (k=1; k<numClusters; k++) {
                        double t = t2[k]-data[i][0]*c[k][0]-data[i][1]*c[k][1]-data[i][2]*c[k][2];

                        if (t<e) {
                            index_t[i]=k;
                            e =t;
                        }
                    }
                    ea +=e;
                }
                if (ea < me) {
                    me =ea;
                    int i1,j1;

                    for (i1=0; i1 <numEntries; i1++) {
                        index_r[i1]=index_t[i1];
                        for (j1=0; j1<DIMENSION; j1++)
                            out[i1][j1]=c[index_r[i1]][j1];
                    }

                }
            }
}


void mds (
    double data[MAX_ENTRIES][DIMENSION],int numEntries, int index[MAX_ENTRIES],
    double mean[DIMENSION], double *idx_mean, double direction [DIMENSION],double *step) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int j,k;
    double t=0,s=0,q1=0;

    for (j=0; j<DIMENSION; j++) {
        for (mean[j]=k=0; k<numEntries; k++)
            mean[j]+=data[k][j];
        mean[j]/=(double) numEntries;
    }

    for (k=0; k<numEntries; k++) {
        s+= index[k];
        t+= index[k]*index[k];
    }

    for (j=0; j<DIMENSION; j++) {
        direction[j]=0;
        for (k=0; k<numEntries; k++)
            direction[j]+=(data[k][j]-mean[j])*index[k];

        q1 = fabs(direction[j]) > q1 ? fabs(direction[j]) : q1;

    }

    *idx_mean= (s /= (double) numEntries);
    t = t - s * s * (double) numEntries;
    t = (t == 0 ? 0. : 1/t);

    *step=t*q1;

    for (j=0; j<DIMENSION; j++) {
        direction[j]/= (q1 == 0 ? 1:q1) ;
        if (direction[j]>=1.0001)
            printf("error\n");
    }
}


void mds_d (
    double data[MAX_ENTRIES][DIMENSION],int numEntries, int index[MAX_ENTRIES],
    double mean[DIMENSION], double *idx_mean, double direction [DIMENSION],double *step) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int i,j,k;
    double t=0,s=0,q1=0;
    double cc[MAX_CLUSTERS_BIG][DIMENSION];
    int cnt[MAX_CLUSTERS_BIG];


    for (i=0; i<MAX_CLUSTERS_BIG; i++) {
        cnt[i]=0;
        for (j=0; j<DIMENSION; j++)
            cc[i][j]=0;
    }

    for (k=0; k<numEntries; k++) {
        cnt[index[k]]++;
        s+= index[k];
        t+= index[k]*index[k];
        for (j=0; j<DIMENSION; j++)
            cc[index[k]][j]+=data[k][j];
    }

    for (i=0; i<MAX_CLUSTERS_BIG; i++)
        if (cnt[i])
            for (j=0; j<DIMENSION; j++) {
                cc[i][j]/=(double)cnt[i];
                cc[i][j]=floor(cc[i][j]+0.5);    // target discrete color
            }

    for (j=0; j<DIMENSION; j++) {
        for (mean[j]=i=0; i<MAX_CLUSTERS_BIG; i++)
            mean[j]+=cc[i][j]*(double)cnt[i];
        mean[j]/=(double) numEntries;
    }


    for (j=0; j<DIMENSION; j++) {
        direction[j]=0;
        for (i=0; i<MAX_CLUSTERS_BIG; i++)
            direction[j]+=(cc[i][j]-mean[j])*(double)cnt[i]*(double)i;

        q1 = fabs(direction[j]) > q1 ? fabs(direction[j]) : q1;

    }

    *idx_mean= (s /= (double) numEntries);
    t = t - s * s * (double) numEntries;
    t = (t == 0 ? 0. : 1/t);

    *step=t*q1;

    for (j=0; j<DIMENSION; j++) {
        direction[j]/= (q1 == 0 ? 1:q1) ;
        if (direction[j]>=1.0001)
            printf("error\n");
    }
}

//mean rounding setup
// cube model
void setMean(
    double mean[DIMENSION],
    double mr[8][DIMENSION],
    int div
) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    double idiv=1.;        // full scale divider per component
    int    sdiv=1 ;    // Z/fcc/bcc swithc

    int j,k;

    while (div >=8) {
        div /=8;
        idiv *=2.;
    }
    sdiv = div;

    // bcc*2 and cartesian; starting points for fcc
    mr[4][0]=(mr[0][0]=floor(mean[0]/idiv))+1.;
    if (sdiv==4) { // switch to search in bcc*2
        for (k=0; k<5; k+=4) {
            if ( (int) floor(mr[k][0]+0.5) & 1 ) {
                mr[k][1]=floor((mean[1]/idiv-1.)/2.)+1./2.;
                mr[k][2]=floor((mean[2]/idiv-1.)/2.)+1./2.;
            } else {
                mr[k][1]=floor(mean[1]/idiv/2.);
                mr[k][2]=floor(mean[2]/idiv/2.);
            }
        }
    } else {
        mr[4][1]=mr[0][1]=floor(mean[1]/idiv);
        mr[4][2]=mr[0][2]=floor(mean[2]/idiv);
    }

    // search in fcc
    for (k=0; k<5; k+=4) {
        if (sdiv==2) {
            if ((int) floor(mr[k][0]+mr[k][1]+mr[k][2] +0.5) & 1) {
                //latice point, leave [0] be,
                if (-mr[k][1]+mr[k][2] < -(mean[1]/idiv)+(mean[2]/idiv)) {
                    // [-1,1]
                    mr[k][1]--;
                    mr[k][2]++;
                } // else Ok to go
            } else {
                if (mr[k][1]+ 1 + mr[k][2] < (mean[1]/idiv)+(mean[2]/idiv))
                    // get to [0,1] and go right
                    mr[k][2]++;
                else
                    // get to [-1,0] and go right
                    mr[k][1]--;
            }
        }
    }


    if (sdiv==2) {
        mr[1][0] =mr[0][0];
        mr[1][1] =mr[0][1]+1.;
        mr[1][2] =mr[0][2]+1.;

        mr[2][0] =mr[0][0];
        mr[2][1] =mr[0][1]+1.;
        mr[2][2] =mr[0][2]-1.;

        mr[3][0] =mr[0][0];
        mr[3][1] =mr[0][1]+1.;
        mr[3][2] =mr[0][2];

        mr[5][0] =mr[4][0];
        mr[5][1] =mr[4][1]+1.;
        mr[5][2] =mr[4][2]+1.;

        mr[6][0] =mr[4][0];
        mr[6][1] =mr[4][1]+1.;
        mr[6][2] =mr[4][2]-1.;

        mr[7][0] =mr[4][0];
        mr[7][1] =mr[4][1]+1.;
        mr[7][2] =mr[4][2];
    } else {
        mr[1][0] =mr[0][0];
        mr[1][1] =mr[0][1]+1.;
        mr[1][2] =mr[0][2];

        mr[2][0] =mr[0][0];
        mr[2][1] =mr[0][1];
        mr[2][2] =mr[0][2]+1.;

        mr[3][0] =mr[0][0];
        mr[3][1] =mr[0][1]+1.;
        mr[3][2] =mr[0][2]+1.;


        mr[5][0] =mr[4][0];
        mr[5][1] =mr[4][1]+1.;
        mr[5][2] =mr[4][2];

        mr[6][0] =mr[4][0];
        mr[6][1] =mr[4][1];
        mr[6][2] =mr[4][2]+1.;

        mr[7][0] =mr[4][0];
        mr[7][1] =mr[4][1]+1.;
        mr[7][2] =mr[4][2]+1.;
    }

    if (sdiv==4) // search in bcc*2
        for (k=0; k<8; k++)
            for (j=1; j<DIMENSION; j++)
                mr[k][j]*=2;

    for (k=0; k<8; k++)
        for (j=0; j<DIMENSION; j++) {
            mr[k][j]*=idiv;
            mr[k][j] = mr[k][j] < 0 ? 0. : (mr[k][j] > 255. ? 255. :mr[k][j]);
        }
}



double reconstruct_rnd(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],int ns,
    double direction [DIMENSION],double *step
) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
#define NP1 31

    static int code1[NP1][3]=    // step, mean div,diamond
        /*
        GOOD
        */
    {
        {7, 1, 2},   {11, 1, 2}, {15, 1, 2}, {19, 1, 4},
        {24, 1, 4},  {29, 1, 4}, {34, 1, 4}, {40, 1, 4},
        {46, 1, 4}, {53, 1, 4},  {60, 2, 1},
        {67, 2, 1},  {75, 2, 1}, {83, 2, 1}, {92, 2, 2}, {101, 2, 2}, {111, 2, 2}, {122, 2, 2}, {133, 2, 2}, {145, 2, 2}, {158, 2, 2},
        {172, 2, 2},{187, 2, 2}, {204, 2, 4}, {223, 2, 4}, {245, 2, 4}, {270, 2, 4}, {301, 4, 1}, {339, 4, 1}, {392, 4, 1}, {448, 4, 1}
    };

#define NP2 7

    static int code2[NP2][3]=    // step, mean div,diamond
    {{7,4,2}, {13, 8, 1},{26, 8, 1},{48, 8, 1},{79, 8, 2},{111, 16, 1},{158, 16, 1}};

    int dir_limit1=13;
    int dir_limit2=6;

    static int report[3]= {0,0,0};

    int (*code)[3];
    int dir_limit=dir_limit1;
    int np;
//##############################
    if (ns==3) {
//##############################
        code = code2;
        dir_limit=dir_limit2;
        np=NP2;
    } else {
        code = code1;
        dir_limit=dir_limit1;
        np=NP1;
    }

    if (! report[ns]) {
        report[ns]=1;

        int bd=1;
        int s,i;

        printf("Quantizer report, sets %d\n",ns);

        for (i=0; i<np; i++) {
            int td = code[i][1]*code[i][1]*code[i][1] * code[i][2];
            if (td < bd)
                printf("non-monotonic diver codes?? was  %d  now  %d  at %d\n", bd, td,i);
            bd =td;
        }
        printf("Max divider %d \n",bd);
        for (s=i=0; i<np; i++)
            s+= bd /(code[i][1]*code[i][1]*code[i][1] * code[i][2]);
        printf("Soft estimate; position capacity (scaled by max divider) %d, direction %d \n",
               s,24*dir_limit*dir_limit+2);

        for (s=i=0; i<np; i++)
            s+= bd /(code[i][1]*code[i][1]*code[i][1] * code[i][2]) *
                (dir_limit < code[i][0] ? 24*dir_limit*dir_limit+2 : 24*code[i][0]*code[i][0]+2);

        printf("Tight estimate, all+divider %d, bits %f;\n sets left total %d, full sets left assuming full directions %d\n",
               s,log((double)s)/log(2.),
               (int) floor(pow(2., ceil(log((double)s)/log(2.)))+0.5)-s,
               ((int) floor(pow(2., ceil(log((double)s)/log(2.)))+0.5)-s) /(24*dir_limit*dir_limit+2 )

              );
        double b =log((double)s)/log(2.)+24;

        while (bd >1) {
            b--;
            bd/=2;
        }

        printf("Tight estimate, TOTAL  %g\n",b);

    }


    int i,j,k;
    static double mean[DIMENSION];

    int ok;
    int maxTry=10;

    static int index[MAX_ENTRIES];
    static int index_r[MAX_ENTRIES];

    for (i=0; i<numEntries; i++)
        index[i]=index_[i];


    double err = 1512*512*3*16;
    double er1;


    double out1[MAX_ENTRIES][DIMENSION];
    // expand/collapse if needed

    if (ns ==1) // 3 bit indes
        index_expand (index, numEntries, 16);
    else
        index_expand (index, numEntries, MAX_CLUSTERS);

    PCnt+=numEntries;


    do {

        double idx_mean;
        mds_d(data, numEntries, index, mean, &idx_mean, direction, step);

        double step_li=*step;

        static double dir_r[4][DIMENSION];

        static double dir_step_r[2][4][DIMENSION]; // alternative for dir_r

        static double mean_r[8][DIMENSION];
        static double mean_step_r[2][8][DIMENSION];
        static double step_r[2];

        if (step_li > 200000 ) {// bypass {
            NShakeCnt+=numEntries;
            for (i=0; i<numEntries; i++)
                for (j=0; j<DIMENSION; j++)
                    out[i][j]=mean[j]+direction[j]* (*step) *(index[i]-idx_mean);
            return totalError(data,out,numEntries);
        }
#define STEP_FACTOR 7.

        if (ns==1) {


#define STEP_DIV 1.
#define MEAN_DIV 1.
            for (j=0; j<DIMENSION; j++)
                mean_step_r[0][0][j]=mean_step_r[1][0][j]=mean_r[0][j]=floor(mean[j]/MEAN_DIV)*MEAN_DIV;

            int ii[3];

            // cartesian shake
            for (ii[0]=0; ii[0]<2; ii[0]++)
                for (ii[1]=0; ii[1]<2; ii[1]++)
                    for (ii[2]=0; ii[2]<2; ii[2]++)
                        for (j=0; j<DIMENSION; j++)
                            mean_step_r[0][ii[0]+ii[1]*2+ii[2]*4][j]=mean_step_r[1][ii[0]+ii[1]*2+ii[2]*4][j]=
                                        mean_r[ii[0]+ii[1]*2+ii[2]*4][j]=mean_r[0][j]+MEAN_DIV*ii[j] >255 ? 255:mean_r[0][j]+MEAN_DIV*ii[j];

            step_li = floor((step_li*STEP_FACTOR+.5));

            step_li = step_li >256*STEP_FACTOR ? 256*STEP_FACTOR : step_li;

            int mc =-1;
            double tt = 2;

            for (i=0; i<DIMENSION; i++)
                if (fabs(fabs(direction[i])-1) < tt) {
                    tt = fabs(fabs(direction[i])-1);
                    mc = i;
                }
            if (mc < 0 && step_li !=0) {
                printf("dir mormalizing error");
            }

            for (i=0; i<2; i++)
                for (ii[0]=0; ii[0]<2; ii[0]++)
                    for (ii[1]=0; ii[1]<2; ii[1]++)
                        for (j=0; j<DIMENSION; j++)
                            dir_step_r[i][ii[0]+2*ii[1]][j] =
                                (j == mc) ? direction[j]*(step_li+i)/STEP_FACTOR : ((floor(direction[j]*(step_li+i))) + (j< mc ? ii[j] : ii[j-1]))/STEP_FACTOR;
            shake_d_s(data, numEntries, idx_mean,
                      dir_step_r, mean_r, index_r, out1 );
        } else {
            // step quantizer in ramp
            for (i=0; i< np; i++)
                if (step_li *STEP_FACTOR<= code[i][0])
                    break;
            if (i!=0)
                i--;
            if (i==np-1)
                i--;

            //"low step"
            // bcc*2 and cartesian; starting points for fcc
            for (j=0; j<2; j++)
                setMean(mean, mean_step_r[j], code[i+j][1]*code[i+j][1]*code[i+j][1]*code[i+j][2]);

            step_li=code[i][0];

            double df[2];

            df[0]=code[i][0]<  dir_limit?code[i][0]  :dir_limit;
            df[1]=code[i+1][0]<dir_limit?code[i+1][0]:dir_limit;

            int mc =-1;
            double tt = 2;

            for (j=0; j<DIMENSION; j++)
                if (fabs(fabs(direction[j])-1) < tt) {
                    tt = fabs(fabs(direction[j])-1);
                    mc = j;
                }
            if (mc < 0 && step_li !=0) {
                printf("dir normalizing error");
            }

            int ii[3];
            for (k=0; k<2; k++)
                for (ii[0]=0; ii[0]<2; ii[0]++)
                    for (ii[1]=0; ii[1]<2; ii[1]++)
                        for (j=0; j<DIMENSION; j++)
                            dir_step_r[k][ii[0]+2*ii[1]][j] =
                                (j == mc) ? direction[j]*(double)code[i+k][0] /STEP_FACTOR:
                                (floor(direction[j]*df[k]) + (j< mc ? ii[j] : ii[j-1]) )/df[k]*(double)code[i+k][0]/STEP_FACTOR ;
            shake_d_s_s(data, numEntries, MAX_CLUSTERS, idx_mean,
                        dir_step_r, mean_step_r, index_r, out1 );


        }

        er1 = totalError(data,out1,numEntries);

        ok =1;

        if (er1 < err) {
            for (i=0; i<numEntries; i++)
                ok = ok && (index[i]==index_r[i]);

            for (i=0; i<numEntries; i++)
                index[i]=index_r[i];

            for (i=0; i<numEntries; i++)
                for (j=0; j<DIMENSION; j++)
                    out [i][j]=out1[i][j];
            err=er1;
        }
    } while (!ok && maxTry-- > 0 );

    return totalError(data,out,numEntries);
};

double reconstruct_rnd_mean_clip(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],int ns,
    double direction [DIMENSION],
    double *step,
    double in_mean[DIMENSION]
) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
#define NP1 31

    static int code1[NP1][3]=    // step, mean div,diamond
        /*
        GOOD
        */
    {
        {7, 1, 2},   {11, 1, 2}, {15, 1, 2}, {19, 1, 4},
        {24, 1, 4},  {29, 1, 4}, {34, 1, 4}, {40, 1, 4},
        {46, 1, 4}, {53, 1, 4},  {60, 2, 1},
        {67, 2, 1},  {75, 2, 1}, {83, 2, 1}, {92, 2, 2}, {101, 2, 2}, {111, 2, 2}, {122, 2, 2}, {133, 2, 2}, {145, 2, 2}, {158, 2, 2},
        {172, 2, 2},{187, 2, 2}, {204, 2, 4}, {223, 2, 4}, {245, 2, 4}, {270, 2, 4}, {301, 4, 1}, {339, 4, 1}, {392, 4, 1}, {448, 4, 1}
    };

#undef NP2
#define NP2 8

    static int code2[NP2][3]=    // step, mean div,diamond

    {{7,8,1}, {24, 8, 1},{46, 8, 1}, {75, 8, 1}, {111, 8, 1},{158, 8, 1}, {223, 8, 1}, {448, 8, 1}};

    int dir_limit1=13;
    int dir_limit2=6;

    static int report[3]= {0,0,0};

    int (*code)[3];
    int dir_limit=dir_limit1;
    int np;

    if (ns==3) {
        code = code2;
        dir_limit=dir_limit2;
        np=NP2;
    } else {
        code = code1;
        dir_limit=dir_limit1;
        np=NP1;
    }

    if (! report[ns]) {
        report[ns]=1;

        int bd=1;
        int s,i;

        printf("Quantizer report, sets %d\n",ns);

        for (i=0; i<np; i++) {
            int td = code[i][1]*code[i][1]*code[i][1] * code[i][2];
            if (td < bd)
                printf("non-monotonic diver codes?? was  %d  now  %d  at %d\n", bd, td,i);
            bd =td;
        }
        printf("Max divider %d \n",bd);
        for (s=i=0; i<np; i++)
            s+= bd /(code[i][1]*code[i][1]*code[i][1] * code[i][2]);
        printf("Soft estimate; position capacity (scaled by max divider) %d, direction %d \n",
               s,24*dir_limit*dir_limit+2);

        for (s=i=0; i<np; i++)
            s+= bd /(code[i][1]*code[i][1]*code[i][1] * code[i][2]) *
                (dir_limit < code[i][0] ? 24*dir_limit*dir_limit+2 : 24*code[i][0]*code[i][0]+2);

        printf("Tight estimate, all+divider %d, bits %f;\n sets left total %d, full sets left assuming full directions %d\n",
               s,log((double)s)/log(2.),
               (int) floor(pow(2., ceil(log((double)s)/log(2.)))+0.5)-s,
               ((int) floor(pow(2., ceil(log((double)s)/log(2.)))+0.5)-s) /(24*dir_limit*dir_limit+2 )

              );
        double b =log((double)s)/log(2.)+24;

        while (bd >1) {
            b--;
            bd/=2;
        }

        printf("Tight estimate, TOTAL  %g\n",b);

    }


    int i,j,k;
    static double mean[DIMENSION];

    int ok;
    int maxTry=10;

    static int index[MAX_ENTRIES];
    static int index_r[MAX_ENTRIES];

    for (i=0; i<numEntries; i++)
        index[i]=index_[i];


    double err = 1512*512*3*16;
    double er1;


    double out1[MAX_ENTRIES][DIMENSION];
    // expand/collapse if needed

    if (ns ==1) // 3 bit indes
        index_expand (index, numEntries, 16);
    else
        index_expand (index, numEntries, MAX_CLUSTERS);

    PCnt+=numEntries;


    do {

        double idx_mean;

        mds(data, numEntries, index, mean, &idx_mean, direction, step);

        double s=0,t=0, q=0, q1 =0;

        for (j=0; j<DIMENSION; j++) {
            for (mean[j]=k=0; k<numEntries; k++)
                mean[j]+=data[k][j];
            mean[j]/=(double) numEntries;
        }


        for (k=0; k<numEntries; k++) {
            s+= index[k];
            t+= index[k]*index[k];
        }


        for (j=0; j<DIMENSION; j++) {
            direction[j]=0;
            for (k=0; k<numEntries; k++)
                direction[j]+=(data[k][j]-mean[j])*index[k];

            q+= direction[j]* direction[j];
            q1 = fabs(direction[j]) > q1 ? fabs(direction[j]) : q1;

        }

        s /= (double) numEntries;

        t = t - s * s * (double) numEntries;

        t = (t == 0 ? 0. : 1/t);

        q=sqrt(q);

        *step=t*q;


        for (j=0; j<DIMENSION; j++) {
            direction[j]/= (q1 == 0 ? 1:q1) ;
            if (direction[j]>=1.0001)
                printf("error\n");
        }

        if (*step !=0)
            *step*=q1/q;

        double step_li=*step;

        static double dir_r[4][DIMENSION];

        static double dir_step_r[2][4][DIMENSION]; // alternative for dir_r

        static double mean_r[8][DIMENSION];
        static double mean_step_r[2][8][DIMENSION];
        static double step_r[2];

        if (step_li > 200000 ) {// bypass {
            NShakeCnt+=numEntries;
            for (i=0; i<numEntries; i++)
                for (j=0; j<DIMENSION; j++)
                    out[i][j]=mean[j]+direction[j]* (*step) *(index[i]-idx_mean);
            return totalError(data,out,numEntries);
        }
#define STEP_FACTOR 7.

        if (ns==1) {


#define STEP_DIV 1.
#define MEAN_DIV 1.
            for (j=0; j<DIMENSION; j++)
                mean_step_r[0][0][j]=mean_step_r[1][0][j]=mean_r[0][j]=floor(mean[j]/MEAN_DIV)*MEAN_DIV;

            int ii[3];

            // cartesian shake
            for (ii[0]=0; ii[0]<2; ii[0]++)
                for (ii[1]=0; ii[1]<2; ii[1]++)
                    for (ii[2]=0; ii[2]<2; ii[2]++)
                        for (j=0; j<DIMENSION; j++)
                            mean_step_r[0][ii[0]+ii[1]*2+ii[2]*4][j]=mean_step_r[1][ii[0]+ii[1]*2+ii[2]*4][j]=
                                        mean_r[ii[0]+ii[1]*2+ii[2]*4][j]=mean_r[0][j]+MEAN_DIV*ii[j] >255 ? 255:mean_r[0][j]+MEAN_DIV*ii[j];

            step_li = floor((step_li*STEP_FACTOR - STEP_FACTOR)/STEP_DIV)*STEP_DIV+STEP_FACTOR;

            step_li = step_li < STEP_FACTOR ? STEP_FACTOR :step_li;

            step_li = step_li >256 ? 256 : step_li;

            int mc =-1;
            double tt = 2;

            for (i=0; i<DIMENSION; i++)
                if (fabs(fabs(direction[i])-1) < tt) {
                    tt = fabs(fabs(direction[i])-1);
                    mc = i;
                }
            if (mc < 0 && step_li !=0) {
                printf("dir mormalizing error");
            }

            for (i=0; i<2; i++)
                for (ii[0]=0; ii[0]<2; ii[0]++)
                    for (ii[1]=0; ii[1]<2; ii[1]++)
                        for (j=0; j<DIMENSION; j++)
                            dir_step_r[i][ii[0]+2*ii[1]][j] =
                                (j == mc) ? direction[j]*(step_li+STEP_DIV*i)/STEP_FACTOR : ((floor(direction[j]*(step_li+STEP_DIV*i))) + (j< mc ? ii[j] : ii[j-1]))/STEP_FACTOR;
            shake_d_s_s(data, numEntries,  MAX_CLUSTERS, idx_mean,
                        dir_step_r, mean_step_r, index_r, out1 );
        } else {

// step quantizer in ramp
            for (i=0; i< np; i++)
                if (step_li *STEP_FACTOR<= code[i][0])
                    break;
            if (i!=0)
                i--;
            if (i==np-1)
                i--;

            //"low step"

            // bcc*2 and cartesian; starting points for fcc
            for (j=0; j<2; j++) {
                for (j=0; j<DIMENSION; j++)
                    mean[j]-=in_mean[j];

                setMean(mean, mean_step_r[j], code[i+j][1]*code[i+j][1]*code[i+j][1]*code[i+j][2]);

                for (k=0; k<8; k++)
                    for (j=0; j<DIMENSION; j++)
                        mean_step_r[j][k][j]+=in_mean[j];
            }

            step_li=code[i][0];

            double df[2];

            df[0]=code[i][0]<  dir_limit?code[i][0]  :dir_limit;
            df[1]=code[i+1][0]<dir_limit?code[i+1][0]:dir_limit;


            int mc =-1;
            double tt = 2;

            for (j=0; j<DIMENSION; j++)
                if (fabs(fabs(direction[j])-1) < tt) {
                    tt = fabs(fabs(direction[j])-1);
                    mc = j;
                }
            if (mc < 0 && step_li !=0) {
                printf("dir normalizing error");
            }

            int ii[3];
            for (k=0; k<2; k++)
                for (ii[0]=0; ii[0]<2; ii[0]++)
                    for (ii[1]=0; ii[1]<2; ii[1]++)
                        for (j=0; j<DIMENSION; j++)
                            dir_step_r[k][ii[0]+2*ii[1]][j] =
                                (j == mc) ? direction[j]*(double)code[i+k][0] /STEP_FACTOR:
                                (floor(direction[j]*df[k]) + (j< mc ? ii[j] : ii[j-1]) )/df[k]*(double)code[i+k][0]/STEP_FACTOR ;

            shake_d_s_s(data, numEntries, MAX_CLUSTERS, idx_mean,
                        dir_step_r, mean_step_r, index_r, out1 );

        }

        er1 = totalError(data,out1,numEntries);

        ok =1;

        if (er1 < err) {
            for (i=0; i<numEntries; i++)
                ok = ok && (index[i]==index_r[i]);

            for (i=0; i<numEntries; i++)
                index[i]=index_r[i];

            for (i=0; i<numEntries; i++)
                for (j=0; j<DIMENSION; j++)
                    out [i][j]=out1[i][j];
            err=er1;
        }
    } while (!ok && maxTry-- > 0 );

    return totalError(data,out,numEntries);
}






inline int getns(int partition[MAX_ENTRIES], int numEntries) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int i,c;
    int id[MAX_ENTRIES];

    for (i=0; i<MAX_SUBSETS; i++)
        id[i]=0;

    for (i=0; i<numEntries; i++)
        id[partition[i]]=1;


    for (c=i=0; i<MAX_SUBSETS; i++)
        c += (id[i]!= 0);

    return (c);
}

int block_mean_rnd(
    double data_[MAX_ENTRIES][DIMENSION],
    int numEntries,
    int partition[MAX_ENTRIES],
    int ns,

    double mean[MAX_SUBSETS][DIMENSION],
    double *clip,
    double mm[DIMENSION],
    double ni[MAX_SUBSETS]  // norm


) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int i,j,k,l;


    double n [MAX_SUBSETS];  // number per set

    if (ns !=3)
        printf("ns problems\n");

    for (i=0; i<ns; i++) {
        n[i]=0;
        for (j=0; j<DIMENSION; j++)
            mean[i][j]=0;
    }

    //    set index compaction
    for (i=0; i<numEntries; i++) {
        int ptr = partition[i];
        for (j=0; j<DIMENSION; j++) {
            mean[ptr][j]        +=data_[i][j];
        }
        n[ptr]++;
    };

    for (j=0; j<DIMENSION; j++) {
        mm[j]=0;
        for (i=0; i<ns; i++) {
            mm[j]+=mean[i][j];
        }
        mm[j]/=(double)numEntries;
    }

    for (i=0; i<ns; i++) {
        for (j=0; j<DIMENSION; j++)
            if (n[i]!=0)
                mean[i][j]/=(double) n[i];
    }





    for (l=0; l<ns; l++) {
        ni[l]=0;
        for (k=0; k<DIMENSION; k++) {
            ni[l]= ni[l]>fabs(mean[l][k]- mean[(l+1) %3][k]) ? ni[l]:fabs(mean[l][k]- mean[(l+1) %3][k]) ;
        }
    };


    for (l=0; l<ns; l++)
        for (i=l+1; i<ns; i++) {
            if (ni[i] < ni[l]) {
                double t = ni[l];
                ni[l]=ni[i];
                ni[i]=t;
            }
        }

    for (j=0; j<DIMENSION; j++) {
        for (i=0; i<ns; i++) {
            mean[i][j] = floor(mean[i][j]/8)*8. ;
        }
        mm[j] = floor(mm[j]/8)*8. ;

    }

    *clip = 8.;
    return(ni[1] < 16. );
}

double ep_shaker(
#define EP_CLUMP

    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],int ns,
    double direction [DIMENSION],double *step,
    int lock
)

{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    double s=0,t=0;
    int i,j,k;
    double mean[DIMENSION];
    int index[MAX_ENTRIES];

    int maxTry=5;

    int Mi=0;

    int Mi_=(ns==1?16 :(ns==2?8:8));;

    int bits;

    bits= (ns==1?0 :(ns==2?2:4));


    for (k=0; k<numEntries; k++)
        index[k]=index_[k];

    for (k=0; k<numEntries; k++)
        if (index[k]>=Mi_) {
            printf("index error\n");
            for (i=0; i<numEntries; i++)
                index[i]=0;
            // work-around for constant block quantizer bug
            break;
        }


    // run on actually used exreme points
    index_collapse (index, numEntries);

    int done;
    int change;
    int start=1;
    int better;

    double of = 0.0;

    double m[DIMENSION];
    double M[DIMENSION];

    double mb[DIMENSION];
    double Mb[DIMENSION];

    do {

        for (j=0; j<DIMENSION; j++) {
            for (mean[j]=k=0; k<numEntries; k++)
                mean[j]+=data[k][j];
            mean[j]/=(double) numEntries;
        }


        if (lock)
            Mi = Mi_-1;
        else
            for (k=0; k<numEntries; k++)
                Mi = index[k]>Mi ?  index[k] :Mi;

        for (k=0; k<numEntries; k++) {
            s+= index[k];
            t+= index[k]*index[k];
        }



        for (j=0; j<DIMENSION; j++) {
            direction[j]=0;
            for (k=0; k<numEntries; k++)
                direction[j]+=(data[k][j]-mean[j])*index[k];

        }

        s /= (double) numEntries;
        t = t - s * s * (double) numEntries;
        *step =t = (t == 0 ? 0. : 1/t);

        for (j=0; j<DIMENSION; j++) {
            m[j]=floor( (mean[j]+direction[j]* (*step) *(0-s)) /( (double) (1<<bits)))*(1<<bits);
#ifdef EP_CLUMP
            m[j]= m[j] > 0 ? m[j] : 0 ;
            m[j]= m[j] < 255. ? m[j] : 255. ;
#endif
            M[j]=floor( (mean[j]+direction[j]* (*step) *(Mi-s))/(  (double) (1<<bits)))*(1<<bits);
#ifdef EP_CLUMP
            M[j]= M[j] > 0 ? M[j] : 0 ;
            M[j]= M[j] < 255. ? M[j] : 255. ;
#endif
        }

        int glue =1;
        for (j=0; j<DIMENSION; j++)
            glue = glue && (m[j]==M[j]);

        if (glue) {
            if (start ) {
                for (i=0; i<numEntries; i++)
                    for (j=0; j<DIMENSION; j++)
                        out[i][j]=m[j];
                return  totalError(data,out,numEntries);
            } else
                return  totalError(data,out,numEntries);
        }

        double pp[64][DIMENSION];
        double c[MAX_ENTRIES][64][DIMENSION];

        for (i=0; i<=Mi; i++)
            for (j=0; j<DIMENSION; j++) {
                pp[i][j] = floor(m[j]+(M[j]-m[j])* (i/ (double) Mi) + 0.5);
                pp[i][j]= pp[i][j] > 0 ? pp[i][j] : 0 ;
                pp[i][j]= pp[i][j] < 255. ? pp[i][j] : 255. ;
            }



        for (k=0; k<numEntries; k++)
            for (i=0; i<=Mi; i++)
                for (j=0; j<DIMENSION; j++) {
                    double t1=(data[k][j]-pp[i][j]);
                    c[k][i][j]=t1*t1;
                }

        double  outg[MAX_ENTRIES][DIMENSION];
        int idg[MAX_ENTRIES];
        int id[MAX_ENTRIES];

        double mag=0;
        double ma=0;

        for (k=0; k<numEntries; k++) {
            idg[k]=0;
            double t1=c[k][0][0]+c[k][0][1]+c[k][0][2];
            for (i=1; i<=Mi; i++) {
                double s1=c[k][i][0]+c[k][i][1]+c[k][i][2];
                if (s1 < t1) {
                    t1=s1;
                    idg[k]=i;
                }
            }
            ma+=t1;
        }
        mag=ma;
        for (k=0; k<numEntries; k++) {
            for (j=0; j<DIMENSION; j++)
                outg[k][j]=pp[idg[k]][j];
        }


        int ss =0;
        int p,q,r;

        for (p=1; p<64; p++) {
            // Gray code increment bit
            q=p^(p>>1) ^ (p-1)^((p-1)>>1);
            r=p & (-p);
            if (q != r)
                printf("Gray code problem");


            int di;

            di =  (ss & q) ? -(1<< bits) : (1<<bits);
            ss = ss ^ q;
            int i0=0,j0=0;
            if (q >=8) {
                i0++;
                q >>=3;
            };
            for (q>>=1 ; q!=0; q >>=1)
                j0++;

            if (i0==0) {
                m[j0]+=di;
#ifdef EP_CLUMP
                m[j0]= m[j0] > 0 ? m[j0] : 0 ;
                m[j0]= m[j0] < 255. ? m[j0] : 255. ;
#endif
            } else {
                M[j0]+=di;
#ifdef EP_CLUMP
                M[j0]= M[j0] > 0 ? M[j0] : 0 ;
                M[j0]= M[j0] < 255. ? M[j0] : 255. ;
#endif
            }

            for (i=0; i<=Mi; i++) {
                pp[i][j0] = floor(m[j0]+(M[j0]-m[j0])* (i/ (double) Mi) + 0.5);
                pp[i][j0]= pp[i][j0] > 0 ? pp[i][j0] : 0 ;
                pp[i][j0]= pp[i][j0] < 255. ? pp[i][j0] : 255. ;
            }



            for (k=0; k<numEntries; k++)
                for (i=0; i<=Mi; i++) {
                    double t1=(data[k][j0]-pp[i][j0]);
                    c[k][i][j0]=t1*t1;
                }


            ma=0;

            for (k=0; k<numEntries; k++) {
                id[k]=0;
                double t1=c[k][0][0]+c[k][0][1]+c[k][0][2];
                for (i=1; i<=Mi; i++) {
                    double s1=c[k][i][0]+c[k][i][1]+c[k][i][2];
                    if (s1 < t1) {
                        t=s1;
                        id[k]=i;
                    }
                }
                ma+=t1;
            }
            if (ma < mag) {
                mag=ma;
                for (k=0; k<numEntries; k++) {
                    idg[k]=id[k];
                    for (j=0; j<DIMENSION; j++) {
                        outg[k][j]=pp[idg[k]][j];
                        mb[j]=m[j];
                        Mb[j]=M[j];


                    }
                }
            }
        }
        double nf = totalError(data,outg,numEntries);

        change =0;
        for (k=0; k<numEntries; k++) {
            change = change || (index[k]!=idg[k]);
        }
        if (! start) {
            better = nf < of;
        } else
            better =1;

        if (better) {
            for (k=0; k<numEntries; k++) {
                index_[k]=index[k]=idg[k];
                for (j=0; j<DIMENSION; j++)
                    out[k][j]=outg[k][j];
            }
            of=nf;
        }
        done = !(change  &&  better);
    } while (! done && maxTry--);
    return totalError(data,out,numEntries);
}

#define SIZE 6


int expand (int bits, int v) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    return (  v << (8-bits) | v >> (2* bits - 8));
}


void getRndRamp(int bits, double v, int size, double out[], int *parity, int range[2], int ep[] ) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    int m;
    m = (int)floor(v / (double) (1 << (8-bits)));
    m = m < 0 ?  0 :m ;
    m = m > (1<<bits)-1 ? (1<<bits)-1  : m;

    while (expand(bits, m+1) < v  && m <  (1<<bits)-1)
        m++;

    m -=((size+1)/2-1); // normally size is even, save size == 1


    *parity = m & 0x1;

    int i;
    range[0]=0;
    range[1]=size;

    for (i=0; i< size; i++) {
        if (m >=0) {
            if (m < (1<<bits)) {
                out[i] = m; // no expansion here, as single point bcc handling is outside
                ep[i]=m;
            } else if (range[1] > i) {
                range[1] = i;
            }
        } else {
            range[0]=i+1;
        }
        m++;
    }
}

void getRndRampN(int bits[DIMENSION], double v[DIMENSION], int size, double out[ MAX_SHAKE_SIZE][DIMENSION], int parity[DIMENSION], int range[2][DIMENSION], int ep[ MAX_SHAKE_SIZE][DIMENSION],int bcc) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    static double   out_[DIMENSION][MAX_SHAKE_SIZE];
    int   range_[DIMENSION][2];
    int      ep_[DIMENSION][MAX_SHAKE_SIZE];

    int i,j;
    for (j=0; j<DIMENSION; j++)
        getRndRamp(bits[j]+bcc, v[j], size,  out_[j], parity+j, range_[j], ep_[j]) ;
//
// If bcc, we expand bits by 1, quantizer should take care about partity
//
    if (size >1) {
        for (j=0; j<DIMENSION; j++) {
            for (i=0; i<size; i++)
                out[i][j]=expand(bits[j]+bcc,(int)out_[j][i]);
            for (i=0; i<2; i++)
                range[i][j]=range_[j][i];
            for (i=0; i<size; i++)
                ep[i][j]=ep_[j][i];
        }
    } else if (size ==1) { // m in range
        for (j=0; j<DIMENSION; j++) {
            range[0][j]=0;
            range[1][j]=1;
        }
        if (bcc) {
            double t0=0,t1=0;
            int ok0=1,ok1=1;
            int p = 0;

            for (j=0; j<DIMENSION; j++)
                out_[j][1]=out_[j][0]+1;
            for (j=0; j<DIMENSION; j++) {
                // partity 0
                t0 += (expand(bits[j]+bcc,(int)(out_[j][parity[j]]-v[j]))) *
                      (expand(bits[j]+bcc,(int)(out_[j][parity[j]]-v[j])));
                ok0 = ok0 && out_[j][parity[j]]<  ((1<<(bits[j]+bcc)));
                // partity 0
                t1 += (expand(bits[j]+bcc,(int)(out_[j][(parity[j] ^ 1) & 1 ]-v[j])))*
                      (expand(bits[j]+bcc,(int)(out_[j][(parity[j] ^ 1) & 1 ]-v[j])));

                ok1 = ok1 && out_[j][(parity[j] ^ 1) & 1 ]<  ((1<<(bits[j]+bcc)));
            }
            if (ok0 && ok1 )
                if (t0<t1)
                    p=0;
                else
                    p=1;
            else if (ok0)
                p=0;
            else if (ok1)
                p=1;
            else
                printf("problems with single point bcc clumping %g %g  %g %g  %g %g \n",
                       out_[0][0],out_[0][1],
                       out_[1][0],out_[1][1],
                       out_[2][0],out_[2][1]);
            for (j=0; j<DIMENSION; j++) {
                out[0][j]=expand(bits[j]+bcc,(int)(out_[j][(parity[j] ^ p)&1]));
                ep[0][j]=(int)out_[j][(parity[j] ^ p)&1];

            }
        } else {
            for (j=0; j<DIMENSION; j++) {
                if ( out_[j][0] +1 <  ((1<<(bits[j]))) &&  // bcc == 0
                        (abs(expand(bits[j]+bcc, (int)(out_[j][0]+1 -v[j]))) <
                         abs(expand(bits[j]+bcc, (int)(out_[j][0]   -v[j]))))) {
                    out[0][j]=expand(bits[j]+bcc, (int)(out_[j][0]+1));
                    ep[0][j]=(int)(out_[j][0]+1);
                } else {
                    out[0][j]=expand(bits[j]+bcc, (int)(out_[j][0]));
                    ep[0][j]=(int)(out_[j][0]);
                }
            }
        }
    } else
        printf("problems with size %d\n",size);
}


double ep_shaker_2(

    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],
    int epo_code[2][DIMENSION],
    int bits[3],
    int bcc,
    int nClusters,
    int size
) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    int i,j,k;
    double mean[DIMENSION];
    int index[MAX_ENTRIES];

    int maxTry=8;

    int Mi=0;

    int Mi_= nClusters-1; // this is last cluster, not max number 0f clusters

    for (k=0; k<numEntries; k++)
        index[k]=index_[k];

    for (k=0; k<numEntries; k++)
        if (index[k]> Mi_) {
            printf("index error\n");
            for (i=0; i<numEntries; i++)
                index[i]=0;
            // work-around for constant block quantizer bug
            break;
        }


    index_collapse (index, numEntries);

    int done;
    int change;
    int start=1;
    int better;

    double of = 0.0;
    double epo[2][DIMENSION];
    int ep[2][ MAX_SHAKE_SIZE][DIMENSION];
    int parity[2][DIMENSION];

    int range[2][2][DIMENSION];
    double epd[2][ MAX_SHAKE_SIZE][DIMENSION];

    do {

        for (j=0; j<DIMENSION; j++) {
            for (mean[j]=k=0; k<numEntries; k++)
                mean[j]+=data[k][j];
            mean[j]/=(double) numEntries;
        }


        for (k=0; k<numEntries; k++)
            Mi = index[k]>Mi ?  index[k] :Mi;

        // shift/scale

        int p, q;

        int p0=-1,q0=-1;

        double ee = MAX_ENTRIES* 256.*256.*DIMENSION*8.; // 8 = slack


        if (Mi==0) {
            getRndRampN(bits, mean, 1/* size*/, epo+0, parity[0], range[0], ep[0],bcc) ;
            getRndRampN(bits, mean, 1/* size*/, epo+1, parity[1], range[1], ep[1],bcc)  ;
            for (j=0; j<DIMENSION; j++) {
                epo_code[0][j]=ep[0][0][j];
                epo_code[1][j]=ep[0][0][j];
            }

        }

        for (q=1; Mi!=0 && q*Mi <= Mi_; q++)
            for (p=q*Mi; p<=Mi_; p++)  {
                //index * q + p-Mi*q

                int cidx[MAX_ENTRIES];

                for (k=0; k<numEntries; k++)
                    cidx[k]=index[k] * q + p-Mi*q;

                double im [2][2] = {{0,0},{0,0}};    // matrix /inverse matrix
                double rp[2][DIMENSION];            // right part for RMS fit problem
                double epa[2][DIMENSION];

                // get ideal clustr centers
                double cc[MAX_CLUSTERS_BIG][DIMENSION];
                double cnt[MAX_CLUSTERS_BIG];

                for (j=0; j<DIMENSION; j++) {
                    for (i=0; i<=Mi_; i++)
                        cnt[i]=cc[i][j]=0;
                    rp[0][j]=rp[1][j]=0;
                }

                for (k=0; k<numEntries; k++) {
                    cnt[cidx[k]]+=1;
                    for (j=0; j<DIMENSION; j++)
                        cc[cidx[k]][j]+=data[k][j];
                }

                for (i=0; i<=Mi_; i++)
                    for (j=0; j<DIMENSION; j++)
                        if (cnt[i]!=0)
                            cc[i][j]=floor(cc[i][j]/cnt[i]+0.5); // more or less ideal location
                        else
                            cc[i][j]=0;

                // for cluster centers

                // weight with cnt
                for (k=0; k<numEntries; k++) {
                    im[0][0] += cidx[k]*cidx[k]      ;
                    im[0][1] += cidx[k]*(Mi_-cidx[k]);            // im is symmetric
                    im[1][1] += (Mi_-cidx[k])*(Mi_-cidx[k]);

                    for (j=0; j<DIMENSION; j++) {
                        rp[0][j]+= cidx[k] * cc[cidx[k]][j];
                        rp[1][j]+=(Mi_-cidx[k]) * cc[cidx[k]][j];
                    }
                }

                double dd = im[0][0]*im[1][1]-im[0][1]*im[0][1];


                for (i=0; i<2; i++)
                    for (j=0; j<DIMENSION; j++) {
                        range[i][0][j]=0;
                        range[i][1][j]=size;
                    }


                if (dd==0) {
//############## degenerate, usually the same index
                    for (i=0; i<=Mi_; i++)
                        if (cnt[i]!=0)
                            break;
                    // Make it into ramp
                    for (j=0; j<DIMENSION; j++) {
                        epa[0][j]=cc[i][j];
                        epa[1][j]=cc[i][j];
                    }
                    getRndRampN(bits, epa[0], size, epd[0], parity[0], range[0], ep[0],bcc)  ;
                    getRndRampN(bits, epa[1], size, epd[1], parity[1], range[1], ep[1],bcc)  ;

                } else {

                    im[1][0]=im[0][0];
                    im[0][0]=im[1][1]/dd;
                    im[1][1]=im[1][0]/dd;
                    im[1][0]=im[0][1]=-im[0][1]/dd;


                    for (j=0; j<DIMENSION; j++) {
                        epa[0][j]=(im[1][0]*rp[0][j]+im[1][1]*rp[1][j])*Mi_;
                        epa[1][j]=(im[0][0]*rp[0][j]+im[0][1]*rp[1][j])*Mi_;
                    }
                    getRndRampN(bits, epa[0], size, epd[0], parity[0], range[0], ep[0],bcc);
                    getRndRampN(bits, epa[1], size, epd[1], parity[1], range[1], ep[1],bcc) ;

                }
                int best_ep [2][2][2][DIMENSION];// point, parity0,parity1, coordinate
                double e[2][2][DIMENSION]; //  parity0, parity1, coordinate
                int l0,l1;

                for (j=0; j<DIMENSION; j++) {

                    e[0][0][j] = e[0][1][j] = e[1][0][j] = e[1][1][j] = MAX_ENTRIES*256*256*4.;

                    for (l0=range[0][0][j]; l0<range[0][1][j]; l0++)
                        for (l1=range[1][0][j]; l1<range[1][1][j]; l1++) {
// ran on points or clusters ??
                            double cv [MAX_CLUSTERS_BIG];
                            double t=0;
                            int i1;
                            for (i1=0; i1<=Mi_; i1++) {
                                cv[i1]=floor( (epd[1][l1][j]*i1+ epd[0][l0][j]*(Mi_-i1))/((double)Mi_)+0.5);
                                cv[i1]=cv[i1]<0 ? 0 :(cv[i1]>255 ? 255: cv[i1]);
                            }

                            for (i1=0; i1< numEntries; i1++)
                                t+=(cv[cidx[i1]]-data[i1][j])*(cv[cidx[i1]]-data[i1][j]);
                            if (bcc) {
                                if (t < e[(parity[0][j] ^ l0)& 1] [(parity[1][j] ^ l1)& 1][j] ) {
                                    e[(parity[0][j] ^ l0)& 1] [(parity[1][j] ^ l1)& 1][j] =t;
                                    best_ep[0][(parity[0][j] ^ l0)& 1] [(parity[1][j] ^ l1)& 1][j]=l0;
                                    best_ep[1][(parity[0][j] ^ l0)& 1] [(parity[1][j] ^ l1)& 1][j]=l1;
                                }
                            } else {
                                if (t < e[0][0][j]) {
                                    e[0][0][j] =t;
                                    best_ep[0][0][0][j]=l0;
                                    best_ep[1][0][0][j]=l1;
                                }
                            }
                        }
                }
                if (bcc) {
                    double t[2][2]= {0,0,0,0};
                    double tt;
                    int a,b,a0,b0;
                    for (a=0; a<2; a++)
                        for (b=0; b<2; b++)
                            for (j=0; j<DIMENSION; j++)
                                t[a][b]+=e[a][b][j];
                    a0=b0=0;
                    tt = t[0][0];
                    for (a=0; a<2; a++)
                        for (b=0; b<2; b++)
                            if (tt > t[a][b]) {
                                a0=a;
                                b0=b;
                                tt = t[a][b];
                            }


                    if (tt< ee) {
                        ee = tt;
                        p0=p;
                        q0=q;
                        for (j=0; j<DIMENSION; j++) {
                            epo[0][j]= epd[0][best_ep[0][a0][b0][j]][j];
                            epo[1][j]= epd[1][best_ep[1][a0][b0][j]][j];
                            epo_code[0][j]= ep[0][best_ep[0][a0][b0][j]][j];
                            epo_code[1][j]= ep[1][best_ep[1][a0][b0][j]][j];

                        }
                    }
                } else {
                    double t=0;
                    for (j=0; j<DIMENSION; j++)
                        t+=e[0][0][j];
                    if (t< ee) {
                        ee = t;
                        p0=p;
                        q0=q;
                        for (j=0; j<DIMENSION; j++) {
                            epo[0][j]= epd[0][best_ep[0][0][0][j]][j];
                            epo[1][j]= epd[1][best_ep[1][0][0][j]][j];
                            epo_code[0][j]= ep[0][best_ep[0][0][0][j]][j];
                            epo_code[1][j]= ep[1][best_ep[1][0][0][j]][j];
                        }
                    }
                }
            }
        // reconstruct new ep

        // requantize
        // #############

        double pp[64][DIMENSION];
        double c[MAX_ENTRIES][64][DIMENSION];

        for (i=0; i<=Mi_; i++)
            for (j=0; j<DIMENSION; j++) {
                pp[i][j] = floor(epo[0][j]+(epo[1][j]-epo[0][j])* (i/ (double) Mi_) + 0.5);
                pp[i][j]= pp[i][j] > 0 ? pp[i][j] : 0 ;
                pp[i][j]= pp[i][j] < 255. ? pp[i][j] : 255. ;
            }

        for (k=0; k<numEntries; k++)
            for (i=0; i<=Mi_; i++)
                for (j=0; j<DIMENSION; j++) {
                    double t=(data[k][j]-pp[i][j]);
                    c[k][i][j]=t*t;
                }

        double  outg[MAX_ENTRIES][DIMENSION];
        int idg[MAX_ENTRIES];

        double mag=0;
        double ma=0;

        for (k=0; k<numEntries; k++) {
            idg[k]=0;
            double t=c[k][0][0]+c[k][0][1]+c[k][0][2];
            for (i=1; i<=Mi_; i++) {
                double s=c[k][i][0]+c[k][i][1]+c[k][i][2];
                if (s < t) {
                    t=s;
                    idg[k]=i;
                }
            }
            ma+=t;
        }
        mag=ma;
        for (k=0; k<numEntries; k++) {
            for (j=0; j<DIMENSION; j++)
                outg[k][j]=pp[idg[k]][j];
        }

        // change/better

        double nf = totalError(data,outg,numEntries);

        change =0;
        for (k=0; k<numEntries; k++) {
            change = change || (index[k] * q0 + p0-Mi*q0!=idg[k]);
        }
        if (! start) {
            better = nf < of;
        } else
            better =1;

        if (better) {
            for (k=0; k<numEntries; k++) {
                index_[k]=index[k]=idg[k];
                for (j=0; j<DIMENSION; j++)
                    out[k][j]=outg[k][j];
            }
            of=nf;
        }
        done = !(change  &&  better) ;
    } while (! done && maxTry--);
#if 1
    if (bcc)
        if (
            ((epo_code[0][0] ^ epo_code[0][1] )& 1) != 0 ||
            ((epo_code[0][1] ^ epo_code[0][2] )& 1) != 0 ||

            ((epo_code[1][0] ^ epo_code[1][1] )& 1) != 0 ||
            ((epo_code[1][1] ^ epo_code[1][2] )& 1) != 0 )
            printf("bcc parity error %d  %d  %d  %d  %d  %d \n",
                   epo_code[0][0], epo_code[0][1], epo_code[0][2],
                   epo_code[1][0], epo_code[1][1], epo_code[1][2]);

#endif
    return totalError(data,out,numEntries);
};


double ep_shaker_2__(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],int ns
) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
#define SIZE 6
    int i,j,k;
    double mean[DIMENSION];
    int index[MAX_ENTRIES];

    int maxTry=8;

    int Mi=0;

    int Mi_=(ns==1 ? 15 :(ns==2 ? 7 : 7)); // this is last cluster, not max number 0f clusters
    int bits;

    bits= (ns==1?0 :(ns==2?2:4));

    for (k=0; k<numEntries; k++)
        index[k]=index_[k];

    for (k=0; k<numEntries; k++)
        if (index[k]> Mi_) {
            printf("index error\n");
            for (i=0; i<numEntries; i++)
                index[i]=0;
            // work-around for constant block quantizer bug
            break;
        }


    index_collapse (index, numEntries);

    int done;
    int change;
    int start=1;
    int better;

    double of = 0.0;
    double epo[2][DIMENSION];

    do {

        for (j=0; j<DIMENSION; j++) {
            for (mean[j]=k=0; k<numEntries; k++)
                mean[j]+=data[k][j];
            mean[j]/=(double) numEntries;
        }


        for (k=0; k<numEntries; k++)
            Mi = index[k]>Mi ?  index[k] :Mi;

        // shift/scale

        int p, q;

        int p0=-1,q0=-1;

        double ee = MAX_ENTRIES* 256.*256.*DIMENSION*8.; // 8 = slack


        if (Mi==0)
            for (j=0; j<DIMENSION; j++) {
                epo[0][j]= floor(mean[j] /((double)(1<<bits)))*(1<<bits);
                epo[1][j]= floor(mean[j] /((double)(1<<bits)))*(1<<bits);
                epo[0][j]= epo[0][j] < 0 ?0:epo[0][j] ;
                epo[0][j]= epo[0][j] > 255 ?255:epo[0][j] ;
                epo[1][j]= epo[1][j] < 0 ?0:epo[1][j] ;
                epo[1][j]= epo[1][j] > 255 ?255:epo[1][j] ;

            }


        for (q=1; Mi!=0 && q*Mi <= Mi_; q++)
            for (p=q*Mi; p<=Mi_; p++)  {
                //index * q + p-Mi*q

                int cidx[MAX_ENTRIES];

                for (k=0; k<numEntries; k++)
                    cidx[k]=index[k] * q + p-Mi*q;

                double im [2][2] = {{0,0},{0,0}};    // matrix /inverse matrix
                double rp[2][DIMENSION];            // right part for RMS fit problem
                double epa[2][DIMENSION];

                // get ideal clustr centers
                double cc[MAX_CLUSTERS_BIG][DIMENSION];
                double cnt[MAX_CLUSTERS_BIG];

                for (j=0; j<DIMENSION; j++) {
                    for (i=0; i<=Mi_; i++)
                        cnt[i]=cc[i][j]=0;
                    rp[0][j]=rp[1][j]=0;
                }

                for (k=0; k<numEntries; k++) {
                    cnt[cidx[k]]+=1;
                    for (j=0; j<DIMENSION; j++)
                        cc[cidx[k]][j]+=data[k][j];
                }

                for (i=0; i<=Mi_; i++)
                    for (j=0; j<DIMENSION; j++)
                        if (cnt[i]!=0)
                            cc[i][j]=floor(cc[i][j]/cnt[i]+0.5); // more or less ideal location
                        else
                            cc[i][j]=0;

                // for cluster centers

                // weight with cnt
                for (k=0; k<numEntries; k++) {
                    im[0][0] += cidx[k]*cidx[k]      ;
                    im[0][1] += cidx[k]*(Mi_-cidx[k]);            // im is symmetric
                    im[1][1] += (Mi_-cidx[k])*(Mi_-cidx[k]);

                    for (j=0; j<DIMENSION; j++) {
                        rp[0][j]+= cidx[k] * cc[cidx[k]][j];
                        rp[1][j]+=(Mi_-cidx[k]) * cc[cidx[k]][j];
                    }
                }

                double dd = im[0][0]*im[1][1]-im[0][1]*im[0][1];
                double epd[2][SIZE][DIMENSION];

                int range[2][2][DIMENSION];
                for (i=0; i<2; i++)
                    for (j=0; j<DIMENSION; j++) {
                        range[i][0][j]=0;
                        range[i][1][j]=SIZE;
                    }




                if (dd==0) {
//############## degenerate, usually the same index
                    for (i=0; i<=Mi_; i++)
                        if (cnt[i]!=0)
                            break;
                    // Make it into ramp
                    for (j=0; j<DIMENSION; j++) {
                        epa[0][j]=cc[i][j];
                        epa[1][j]=cc[i][j];

                        for (i=0; i<SIZE; i++) {
                            epd[0][i][j]= floor(epa[0][j] /((double)(1<<bits)))*(1<<bits) + (i-(SIZE/2-1))*(1<<bits);
                            epd[1][i][j]= floor(epa[1][j] /((double)(1<<bits)))*(1<<bits) + (i-(SIZE/2-1))*(1<<bits);
                            epd[0][i][j]=epd[0][i][j] <0 ?     0: epd[0][i][j];
                            epd[0][i][j]=epd[0][i][j] >255 ? 255: epd[0][i][j];
                            epd[1][i][j]=epd[1][i][j] <0 ?     0: epd[1][i][j];
                            epd[1][i][j]=epd[1][i][j] >255 ? 255: epd[1][i][j];


                        }
                        {
                            for (i=1; i<SIZE; i++) {
                                if (epd[0][i-1][j]==epd[0][i][j])
                                    range[0][0][j]=i;
                                if (epd[1][i-1][j]==epd[1][i][j])
                                    range[1][0][j]=i;
                            }
                            for (i=SIZE-2; i>=0; i--) {
                                if (epd[0][i+1][j]==epd[0][i][j])
                                    range[0][0][j]=i+1;
                                if (epd[1][i+1][j]==epd[1][i][j])
                                    range[1][0][j]=i+1;
                            }
                        }
                    }
#if 1
                    for (k=0; k<numEntries; k++) {
                        index_[k]=index[k]=0;
                        for (j=0; j<DIMENSION; j++)
                            out[k][j]=floor(mean[j] /((double)(1<<bits)))*(1<<bits) + (i-1)*(1<<bits);
                    }
#endif
                } else {

                    im[1][0]=im[0][0];
                    im[0][0]=im[1][1]/dd;
                    im[1][1]=im[1][0]/dd;
                    im[1][0]=im[0][1]=-im[0][1]/dd;


                    for (j=0; j<DIMENSION; j++) {
                        epa[0][j]=(im[1][0]*rp[0][j]+im[1][1]*rp[1][j])*Mi_;
                        epa[1][j]=(im[0][0]*rp[0][j]+im[0][1]*rp[1][j])*Mi_;

                        for (i=0; i<SIZE; i++) {
                            epd[0][i][j]= floor(epa[0][j] /((double)(1<<bits)))*(1<<bits) + (i-(SIZE/2-1))*(1<<bits);
                            epd[1][i][j]= floor(epa[1][j] /((double)(1<<bits)))*(1<<bits) + (i-(SIZE/2-1))*(1<<bits);

                            epd[0][i][j]=epd[0][i][j] <0 ?     0: epd[0][i][j];
                            epd[0][i][j]=epd[0][i][j] >255 ? 255: epd[0][i][j];
                            epd[1][i][j]=epd[1][i][j] <0 ?     0: epd[1][i][j];
                            epd[1][i][j]=epd[1][i][j] >255 ? 255: epd[1][i][j];


                        }
                        {
                            for (i=1; i<SIZE; i++) {
                                if (epd[0][i-1][j]==epd[0][i][j])
                                    range[0][0][j]=i;
                                if (epd[1][i-1][j]==epd[1][i][j])
                                    range[1][0][j]=i;
                            }
                            for (i=SIZE-2; i>=0; i--) {
                                if (epd[0][i+1][j]==epd[0][i][j])
                                    range[0][0][j]=i+1;
                                if (epd[1][i+1][j]==epd[1][i][j])
                                    range[1][0][j]=i+1;
                            }
                        }

                    }
                }
                int best_ep [2][DIMENSION];
                double e[DIMENSION];
                int l0,l1;

                for (j=0; j<DIMENSION; j++) {
                    e[j] = MAX_ENTRIES*256*256*4.;
                    for (l0=range[0][0][j]; l0<range[0][1][j]; l0++)
                        for (l1=range[1][0][j]; l1<range[1][1][j]; l1++) {
// ran on points or clusters ??
                            double cv [MAX_CLUSTERS_BIG];
                            double t=0;
                            int i1;
                            for (i1=0; i1<=Mi_; i1++) {
                                cv[i1]=floor( (epd[1][l1][j]*i1+ epd[0][l0][j]*(Mi_-i1))/((double)Mi_)+0.5);
                                cv[i1]=cv[i1]<0 ? 0 :(cv[i1]>255 ? 255: cv[i1]);
                            }

                            for (i1=0; i1< numEntries; i1++)
                                t+=(cv[cidx[i1]]-data[i1][j])*(cv[cidx[i1]]-data[i1][j]);
                            if (t < e[j]) {
                                e[j] =t;
                                best_ep[0][j]=l0;
                                best_ep[1][j]=l1;
                            }
                        }
                }
                {
                    double t=0;
                    for (j=0; j<DIMENSION; j++)
                        t+=e[j];
                    if (t< ee) {
                        ee = t;
                        p0=p;
                        q0=q;
                        for (j=0; j<DIMENSION; j++) {
                            epo[0][j]= epd[0][best_ep[0][j]][j];
                            epo[1][j]= epd[1][best_ep[1][j]][j];

                        }
                    }
                }
            }
        // reconstruct new ep

        // requantize
        // #############

        double pp[64][DIMENSION];
        double c[MAX_ENTRIES][64][DIMENSION];

        for (i=0; i<=Mi_; i++)
            for (j=0; j<DIMENSION; j++) {
                pp[i][j] = floor(epo[0][j]+(epo[1][j]-epo[0][j])* (i/ (double) Mi_) + 0.5);
                pp[i][j]= pp[i][j] > 0 ? pp[i][j] : 0 ;
                pp[i][j]= pp[i][j] < 255. ? pp[i][j] : 255. ;
            }

        for (k=0; k<numEntries; k++)
            for (i=0; i<=Mi_; i++)
                for (j=0; j<DIMENSION; j++) {
                    double t=(data[k][j]-pp[i][j]);
                    c[k][i][j]=t*t;
                }

        double  outg[MAX_ENTRIES][DIMENSION];
        int idg[MAX_ENTRIES];

        double mag=0;
        double ma=0;

        for (k=0; k<numEntries; k++) {
            idg[k]=0;
            double t=c[k][0][0]+c[k][0][1]+c[k][0][2];
            for (i=1; i<=Mi_; i++) {
                double s=c[k][i][0]+c[k][i][1]+c[k][i][2];
                if (s < t) {
                    t=s;
                    idg[k]=i;
                }
            }
            ma+=t;
        }
        mag=ma;
        for (k=0; k<numEntries; k++) {
            for (j=0; j<DIMENSION; j++)
                outg[k][j]=pp[idg[k]][j];
        }

        double nf = totalError(data,outg,numEntries);

        change =0;
        for (k=0; k<numEntries; k++) {
            change = change || (index[k] * q0 + p0-Mi*q0!=idg[k]);
        }
        if (! start) {
            better = nf < of;
        } else
            better =1;

        if (better) {
            for (k=0; k<numEntries; k++) {
                index_[k]=index[k]=idg[k];
                for (j=0; j<DIMENSION; j++)
                    out[k][j]=outg[k][j];
            }
            of=nf;
        }
        done = !(change  &&  better) ;
    } while (! done && maxTry--);
    return totalError(data,out,numEntries);
}


//################################
double reconstruct_rnd__(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],int ns,
    double direction [DIMENSION],double *step
) {
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
#define NP1 31

    static int code1[NP1][3]=    // step, mean div,diamond
        /*
        GOOD
        */
    {
        {7, 1, 2},   {11, 1, 2}, {15, 1, 2}, {19, 1, 4},
        {24, 1, 4},  {29, 1, 4}, {34, 1, 4}, {40, 1, 4},
        {46, 1, 4}, {53, 1, 4},  {60, 2, 1},
        {67, 2, 1},  {75, 2, 1}, {83, 2, 1}, {92, 2, 2}, {101, 2, 2}, {111, 2, 2}, {122, 2, 2}, {133, 2, 2}, {145, 2, 2}, {158, 2, 2},
        {172, 2, 2},{187, 2, 2}, {204, 2, 4}, {223, 2, 4}, {245, 2, 4}, {270, 2, 4}, {301, 4, 1}, {339, 4, 1}, {392, 4, 1}, {448, 4, 1}
    };

#undef NP2
#define NP2 3

    static int code2[NP2][3]=    // step, mean div,diamond
//#######################

    {{7,4,2}, {12, 4, 2},{24, 4, 2}};

    int dir_limit1=13;
    int dir_limit2=6;

    static int report[3]= {0,0,0};

    int (*code)[3];
    int dir_limit=dir_limit1;
    int np;
//#######################
    if (ns==3) {
//#######################
        code = code2;
        dir_limit=dir_limit2;
        np=NP2;
    } else {
        code = code1;
        dir_limit=dir_limit1;
        np=NP1;
    }

    if (! report[ns]) {
        report[ns]=1;

        int bd=1;
        int s,i;

        printf("Quantizer report, sets %d\n",ns);

        for (i=0; i<np; i++) {
            int td = code[i][1]*code[i][1]*code[i][1] * code[i][2];
            if (td < bd)
                printf("non-monotonic diver codes?? was  %d  now  %d  at %d\n", bd, td,i);
            bd =td;
        }
        printf("Max divider %d \n",bd);
        for (s=i=0; i<np; i++)
            s+= bd /(code[i][1]*code[i][1]*code[i][1] * code[i][2]);
        printf("Soft estimate; position capacity (scaled by max divider) %d, direction %d \n",
               s,24*dir_limit*dir_limit+2);

        for (s=i=0; i<np; i++)
            s+= bd /(code[i][1]*code[i][1]*code[i][1] * code[i][2]) *
                (dir_limit < code[i][0] ? 24*dir_limit*dir_limit+2 : 24*code[i][0]*code[i][0]+2);

        printf("Tight estimate, all+divider %d, bits %f;\n sets left total %d, full sets left assuming full directions %d\n",
               s,log((double)s)/log(2.),
               (int) floor(pow(2., ceil(log((double)s)/log(2.)))+0.5)-s,
               ((int) floor(pow(2., ceil(log((double)s)/log(2.)))+0.5)-s) /(24*dir_limit*dir_limit+2 )

              );
        double b =log((double)s)/log(2.)+24;

        while (bd >1) {
            b--;
            bd/=2;
        }

        printf("Tight estimate, TOTAL  %g\n",b);

    }


    int i,j,k;
    static double mean[DIMENSION];

    int ok;
    int maxTry=10;

    static int index[MAX_ENTRIES];
    static int index_r[MAX_ENTRIES];

    for (i=0; i<numEntries; i++)
        index[i]=index_[i];


    double err = 1512*512*3*16;
    double er1;


    double out1[MAX_ENTRIES][DIMENSION];
    // expand/collapse if needed

    if (ns ==1) // 3 bit indes
        index_expand (index, numEntries, 16);
    else
        index_expand (index, numEntries, MAX_CLUSTERS);

    PCnt+=numEntries;


    do {

        double idx_mean;

        mds(data, numEntries, index, mean, &idx_mean, direction, step);

        double s=0,t=0, q=0, q1 =0;

        for (j=0; j<DIMENSION; j++) {
            for (mean[j]=k=0; k<numEntries; k++)
                mean[j]+=data[k][j];
            mean[j]/=(double) numEntries;
        }


        for (k=0; k<numEntries; k++) {
            s+= index[k];
            t+= index[k]*index[k];
        }


        for (j=0; j<DIMENSION; j++) {
            direction[j]=0;
            for (k=0; k<numEntries; k++)
                direction[j]+=(data[k][j]-mean[j])*index[k];

            q+= direction[j]* direction[j];
            q1 = fabs(direction[j]) > q1 ? fabs(direction[j]) : q1;

        }

        s /= (double) numEntries;

        t = t - s * s * (double) numEntries;

        t = (t == 0 ? 0. : 1/t);

        q=sqrt(q);

        *step=t*q;


        for (j=0; j<DIMENSION; j++) {
            direction[j]/= (q1 == 0 ? 1:q1) ;
            if (direction[j]>=1.0001)
                printf("error\n");
        }

        if (*step !=0)
            *step*=q1/q;

        double step_li=*step;

        static double dir_r[4][DIMENSION];

        static double dir_step_r[2][4][DIMENSION]; // alternative for dir_r

        static double mean_r[8][DIMENSION];
        static double mean_step_r[2][8][DIMENSION];
        static double step_r[2];

        if (step_li > 200000 ) {// bypass {
            NShakeCnt+=numEntries;
            for (i=0; i<numEntries; i++)
                for (j=0; j<DIMENSION; j++)
                    out[i][j]=mean[j]+direction[j]* (*step) *(index[i]-idx_mean);
            return totalError(data,out,numEntries);
        }
#define STEP_FACTOR 7.

        if (ns==1) {


#define STEP_DIV 1.
#define MEAN_DIV 1.
            for (j=0; j<DIMENSION; j++)
                mean_step_r[0][0][j]=mean_step_r[1][0][j]=mean_r[0][j]=floor(mean[j]/MEAN_DIV)*MEAN_DIV;

            int ii[3];

            // cartesian shake
            for (ii[0]=0; ii[0]<2; ii[0]++)
                for (ii[1]=0; ii[1]<2; ii[1]++)
                    for (ii[2]=0; ii[2]<2; ii[2]++)
                        for (j=0; j<DIMENSION; j++)
                            mean_step_r[0][ii[0]+ii[1]*2+ii[2]*4][j]=mean_step_r[1][ii[0]+ii[1]*2+ii[2]*4][j]=
                                        mean_r[ii[0]+ii[1]*2+ii[2]*4][j]=mean_r[0][j]+MEAN_DIV*ii[j] >255 ? 255:mean_r[0][j]+MEAN_DIV*ii[j];

            step_li = floor((step_li*STEP_FACTOR+.5));

            step_li = step_li >256*STEP_FACTOR ? 256*STEP_FACTOR : step_li;

            int mc =-1;
            double tt = 2;

            for (i=0; i<DIMENSION; i++)
                if (fabs(fabs(direction[i])-1) < tt) {
                    tt = fabs(fabs(direction[i])-1);
                    mc = i;
                }
            if (mc < 0 && step_li !=0) {
                printf("dir mormalizing error");
            }

            for (i=0; i<2; i++)
                for (ii[0]=0; ii[0]<2; ii[0]++)
                    for (ii[1]=0; ii[1]<2; ii[1]++)
                        for (j=0; j<DIMENSION; j++)
                            dir_step_r[i][ii[0]+2*ii[1]][j] =
                                (j == mc) ? direction[j]*(step_li+i)/STEP_FACTOR : ((floor(direction[j]*(step_li+i))) + (j< mc ? ii[j] : ii[j-1]))/STEP_FACTOR;
            shake_d_s(data, numEntries, idx_mean,
                      dir_step_r, mean_r, index_r, out1 );
        } else {

// step quantizer in ramp
            for (i=0; i< np; i++)
                if (step_li *STEP_FACTOR<= code[i][0])
                    break;
            if (i!=0)
                i--;
            if (i==np-1)
                i--;

            //"low step"

            // bcc*2 and cartesian; starting points for fcc
            for (j=0; j<2; j++)
                setMean(mean, mean_step_r[j], code[i+j][1]*code[i+j][1]*code[i+j][1]*code[i+j][2]);

            step_li=code[i][0];

            double df[2];

            df[0]=code[i][0]<  dir_limit?code[i][0]  :dir_limit;
            df[1]=code[i+1][0]<dir_limit?code[i+1][0]:dir_limit;


            int mc =-1;
            double tt = 2;

            for (j=0; j<DIMENSION; j++)
                if (fabs(fabs(direction[j])-1) < tt) {
                    tt = fabs(fabs(direction[j])-1);
                    mc = j;
                }
            if (mc < 0 && step_li !=0) {
                printf("dir normalizing error");
            }

            int ii[3];
            for (k=0; k<2; k++)
                for (ii[0]=0; ii[0]<2; ii[0]++)
                    for (ii[1]=0; ii[1]<2; ii[1]++)
                        for (j=0; j<DIMENSION; j++)
                            dir_step_r[k][ii[0]+2*ii[1]][j] =
                                (j == mc) ? direction[j]*(double)code[i+k][0] /STEP_FACTOR:
                                (floor(direction[j]*df[k]) + (j< mc ? ii[j] : ii[j-1]) )/df[k]*(double)code[i+k][0]/STEP_FACTOR ;
            shake_d_s_s(data, numEntries, MAX_CLUSTERS, idx_mean,
                        dir_step_r, mean_step_r, index_r, out1 );


        }

        er1 = totalError(data,out1,numEntries);

        ok =1;

        if (er1 < err) {
            for (i=0; i<numEntries; i++)
                ok = ok && (index[i]==index_r[i]);

            for (i=0; i<numEntries; i++)
                index[i]=index_r[i];

            for (i=0; i<numEntries; i++)
                for (j=0; j<DIMENSION; j++)
                    out [i][j]=out1[i][j];
            err=er1;
        }
    } while (!ok && maxTry-- > 0 );

    return totalError(data,out,numEntries);
}



