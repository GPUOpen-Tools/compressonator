///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019, Intel Corporation
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of
// the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CMP_HPC_H
#define _CMP_HPC_H

#include <stdint.h>

#ifdef _WIN32
#include <tchar.h>
#endif

#include "common_def.h"

struct bc7_enc_settings {
    bool mode_selection[4];
    CGV_INT refineIterations[8];

    bool skip_mode2;
    CGV_INT fastSkipTreshold_mode1;
    CGV_INT fastSkipTreshold_mode3;
    CGV_INT fastSkipTreshold_mode7;

    CGV_INT mode45_channel0;
    CGV_INT refineIterations_channel;

    CGV_INT channels;
};

typedef void (CompressionFunc)(const texture_surface* input, uint8_t * output);

extern CompressionFunc* gCompressionFunc;

extern double   gCompTime;
extern double   gCompRate;
extern int      gTexWidth;
extern int      gTexHeight;
extern double   gError;
extern double   gError2;

void InitWin32Threads(int numThreads);
void DestroyThreads();

void CompressSTMT(const texture_surface* input, unsigned char* output, int threads);
extern void CompressTexture(const texture_surface* input, unsigned char* output);

#endif
