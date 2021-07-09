//=====================================================================
// Copyright (c) 2021    Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================

#include "bcn.h"

void SetUserBC15EncoderOptions( CMP_BC15Options *bc15options,KernelOptions *kerneloptions)
{
    if (kerneloptions && bc15options)
    {
        if (kerneloptions->fquality != bc15options->m_fquality)
                    bc15options->m_fquality = kerneloptions->fquality;

        if(kerneloptions->bc15.useAdaptiveWeights)
            bc15options->m_bUseAdaptiveWeighting = true;

        if (kerneloptions->bc15.useChannelWeights) {
            bc15options->m_bUseChannelWeighting = true;
            bc15options->m_fChannelWeights[0] = kerneloptions->bc15.channelWeights[0];
            bc15options->m_fChannelWeights[1] = kerneloptions->bc15.channelWeights[1];
            bc15options->m_fChannelWeights[2] = kerneloptions->bc15.channelWeights[2];
        }
        
        if (kerneloptions->bc15.useAlphaThreshold) {
            bc15options->m_bUseAlpha = true;
            bc15options->m_nAlphaThreshold = kerneloptions->bc15.alphaThreshold;
        }

        if (kerneloptions->bc15.useRefinementSteps) {
            bc15options->m_b3DRefinement = true;
            bc15options->m_nRefinementSteps = kerneloptions->bc15.refinementSteps;
        }
    }
}

