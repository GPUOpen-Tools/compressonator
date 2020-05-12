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
//  BC7_Encode.cpp : A reference encoder for BC7
//

#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include "Common.h"
#include "BC7_Definitions.h"
#include "BC7_Partitions.h"
#include "BC7_Encode.h"
#include "BC7_utils.h"
#include "3dquant_vpc.h"
#include "shake.h"
#include "debug.h"

#ifdef BC7_COMPDEBUGGER
#include "compclient.h"
#endif

#ifdef USE_FILEIO
#include <stdio.h>
extern FILE * bc7_File;
#endif

// Threshold quality below which we will always run fast quality and shaking
// Selfnote: User should be able to set this?
// Default FQuality is at 0.1 < g_qFAST_THRESHOLD which will cause the SingleIndex compression to start skipping shape blocks
// during compression
// if user sets a value above this then all shapes will be used for compression scan for quality
double g_qFAST_THRESHOLD  = 0.5;    

// This limit is used for DualIndex Block and if fQuality is above this limit then Quantization shaking will always be performed
// on all indexs
double g_HIGHQULITY_THRESHOLD = 0.7;
//
// For a given block mode this sets up the data needed by the compressor
//
// Note that BC7 only uses NO_PBIT, ONE_PBIT and TWO_PBIT encodings 
// for endpoints
//

void    BC7BlockEncoder::BlockSetup(CMP_DWORD blockMode)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    switch(bti[blockMode].pBitType)
    {
        case    NO_PBIT:
            m_parityBits = CART;
            break;
        case    ONE_PBIT:
            m_parityBits = SAME_PAR;
            break;
        case    TWO_PBIT:
            m_parityBits = BCC;
            break;
        case    THREE_PBIT:
            m_parityBits = SAME_FCC;
            break;
        case    FOUR_PBIT:
            m_parityBits = FCC;
            break;
        case    FIVE_PBIT:
            m_parityBits = FCC_SAME_BCC;
            break;
    }

    if(bti[blockMode].encodingType == NO_ALPHA)
    {
        m_componentBits[COMP_RED] = bti[blockMode].vectorBits/3;
        m_componentBits[COMP_GREEN] = bti[blockMode].vectorBits/3;
        m_componentBits[COMP_BLUE] = bti[blockMode].vectorBits/3;
        m_componentBits[COMP_ALPHA] = 0;

        m_clusters[0] = 1 << bti[blockMode].indexBits[0];
        m_clusters[1] = 0;
    }
    else if(bti[blockMode].encodingType == COMBINED_ALPHA)
    {
        m_componentBits[COMP_RED] = bti[blockMode].vectorBits/4;
        m_componentBits[COMP_GREEN] = bti[blockMode].vectorBits/4;
        m_componentBits[COMP_BLUE] = bti[blockMode].vectorBits/4;
        m_componentBits[COMP_ALPHA] = bti[blockMode].vectorBits/4;

        m_clusters[0] = 1 << bti[blockMode].indexBits[0];
        m_clusters[1] = 0;
    }
    else if(bti[blockMode].encodingType == SEPARATE_ALPHA)
    {
        m_componentBits[COMP_RED] = bti[blockMode].vectorBits/3;
        m_componentBits[COMP_GREEN] = bti[blockMode].vectorBits/3;
        m_componentBits[COMP_BLUE] = bti[blockMode].vectorBits/3;
        m_componentBits[COMP_ALPHA] = bti[blockMode].scalarBits;

        m_clusters[0] = 1 << bti[blockMode].indexBits[0];
        m_clusters[1] = 1 << bti[blockMode].indexBits[1];
    }
}

//
// This function sorts out the bit encoding for the BC7 block and packs everything
// in the right order for the hardware decoder
// 
//
//

void BC7BlockEncoder::EncodeSingleIndexBlock(CMP_DWORD blockMode,
    CMP_DWORD partition,
    CMP_DWORD colour[MAX_SUBSETS][2],
                                             int   indices[MAX_SUBSETS][MAX_SUBSET_SIZE],
                                             //CMP_DWORD entryCount[MAX_SUBSETS],
    CMP_BYTE  block[COMPRESSED_BLOCK_SIZE])
{
#ifdef USE_DBGTRACE
    DbgTrace(("-> WriteBit()"));
#endif
    CMP_DWORD  i,j,k;
    CMP_DWORD   *partitionTable;
    int     bitPosition = 0;    // Position the pointer at the LSB
    CMP_BYTE    *basePtr = (CMP_BYTE*)block;
    CMP_DWORD   blockIndices[MAX_SUBSET_SIZE];

    // Generate Unary header
    for(i=0; i < (int)blockMode; i++)
    {
        WriteBit(basePtr, bitPosition++, 0);
    }
    WriteBit(basePtr, bitPosition++, 1);

    // Write partition bits
    for(i=0;i<bti[blockMode].partitionBits; i++)
    {
        WriteBit(basePtr, bitPosition++, (CMP_BYTE)(partition>>i) & 0x1);
    }

    // Extract the index bits from the partitions
    partitionTable = (CMP_DWORD*)BC7_PARTITIONS[bti[blockMode].subsetCount-1][partition];

    CMP_DWORD   idxCount[3] = {0, 0, 0};
    bool    flipColours[3] = {false, false, false};

    // Sort out the index set and tag whether we need to flip the 
    // endpoints to get the correct state in the implicit index bits
    // The implicitly encoded MSB of the fixup index must be 0
    CMP_DWORD   fixup[3] = {0, 0, 0};
    switch(bti[blockMode].subsetCount)
    {
        case    3:
            fixup[1] = BC7_FIXUPINDICES[2][partition][1];
            fixup[2] = BC7_FIXUPINDICES[2][partition][2];
            break;
        case    2:
            fixup[1] = BC7_FIXUPINDICES[1][partition][1];
            break;
        default:
            break;
    }

    // Extract indices and mark subsets that need to have their colours flipped to get the
    // right state for the implicit MSB of the fixup index
    for(i=0; i < MAX_SUBSET_SIZE; i++)
    {
        CMP_DWORD   p = partitionTable[i];
        blockIndices[i] = indices[p][idxCount[p]++];

        for(j=0;j<(int)bti[blockMode].subsetCount;j++)
        {
            if(i==fixup[j])
            {
                if(blockIndices[i] & (1<<(bti[blockMode].indexBits[0]-1)))
                {
                    flipColours[j] = true;
                }
            }
        }
     }

    // Now we must flip the endpoints where necessary so that the implicitly encoded
    // index bits have the correct state
    for(i=0;i<(int)bti[blockMode].subsetCount;i++)
    {
        if(flipColours[i])
        {
            CMP_DWORD   temp;
            temp = colour[i][0];
            colour[i][0] = colour[i][1];
            colour[i][1] = temp;
        }
    }

    // ...next flip the indices where necessary
    for(i=0; i<MAX_SUBSET_SIZE; i++)
    {
        CMP_DWORD   p = partitionTable[i];
        if(flipColours[p])
        {
            blockIndices[i] = ((1 << bti[blockMode].indexBits[0]) - 1) - blockIndices[i];
        }
    }

    CMP_DWORD   subset, ep, component;

    // Endpoints are stored in the following order RRRR GGGG BBBB (AAAA) (PPPP)
    // i.e. components are packed together
    CMP_DWORD   unpackedColours[MAX_SUBSETS][2][MAX_DIMENSION_BIG];
    CMP_DWORD   parityBits[MAX_SUBSETS][2];

    // Unpack the colour values for the subsets
    for(i=0;i<bti[blockMode].subsetCount;i++)
    {
        CMP_DWORD   packedColours[2] = {colour[i][0],
                                    colour[i][1]};

        if(bti[blockMode].pBitType == TWO_PBIT)
        {
            parityBits[i][0] = packedColours[0] & 1;
            parityBits[i][1] = packedColours[1] & 1;
            packedColours[0] >>= 1;
            packedColours[1] >>= 1;
        }
        else if(bti[blockMode].pBitType == ONE_PBIT)
        {
            parityBits[i][0] = packedColours[1] & 1;
            parityBits[i][1] = packedColours[1] & 1;
            packedColours[0] >>= 1;
            packedColours[1] >>= 1;
        }
        else
        {
            parityBits[i][0] = 0;
            parityBits[i][1] = 0;
        }

        CMP_DWORD   component1;
        for(component1=0;component1<MAX_DIMENSION_BIG;component1++)
        {
            if(m_componentBits[component1])
            {
                unpackedColours[i][0][component1] = packedColours[0] & ((1 << m_componentBits[component1]) - 1);
                unpackedColours[i][1][component1] = packedColours[1] & ((1 << m_componentBits[component1]) - 1);
                packedColours[0] >>= m_componentBits[component1];
                packedColours[1] >>= m_componentBits[component1];
            }
        }
    }

    // Loop over components
    for(component=0; component < MAX_DIMENSION_BIG; component++)
    {
        // loop over subsets
        for(subset=0; subset<(int)bti[blockMode].subsetCount; subset++)
        {
            // Loop over endpoints and write colour bits
            for(ep=0; ep<2; ep++)
            {
                // Write this component
                for(k = 0; k < m_componentBits[component]; k++)
                {
                    WriteBit(basePtr,
                             bitPosition++,
                             (CMP_BYTE)(unpackedColours[subset][ep][component] >> k) & 0x1);
                }
            }
        }
    }

    // Now write parity bits if present
    if(bti[blockMode].pBitType != NO_PBIT)
    {
        for(subset=0; subset<(int)bti[blockMode].subsetCount; subset++)
        {
            if(bti[blockMode].pBitType == ONE_PBIT)
            {
                WriteBit(basePtr,
                         bitPosition++,
                         parityBits[subset][0] & 1);
            }
            else if(bti[blockMode].pBitType == TWO_PBIT)
            {
                WriteBit(basePtr,
                         bitPosition++,
                         parityBits[subset][0] & 1);
                WriteBit(basePtr,
                         bitPosition++,
                         parityBits[subset][1] & 1);
            }
        }
    }

    // Now encode the index bits
    for(i=0; i<MAX_SUBSET_SIZE; i++)
    {
        CMP_DWORD   p = partitionTable[i];
        // If this is a fixup index then drop the MSB which is implicitly 0
        if(i==fixup[p])
        {
            for(j=0; j<(bti[blockMode].indexBits[0]-1); j++)
            {
                WriteBit(basePtr, bitPosition++,(CMP_BYTE)(blockIndices[i]>>j));
            }
        }
        else
        {
            for(j=0;j<bti[blockMode].indexBits[0]; j++)
            {
                WriteBit(basePtr, bitPosition++,(CMP_BYTE)(blockIndices[i]>>j));
            }
        }
    }

    // Check that we encoded exactly the right number of bits
    if(bitPosition != (COMPRESSED_BLOCK_SIZE * 8))
    {
#ifdef USE_DBGTRACE
        DbgTrace(("Error:Encoded incorrect number of bits"));
#endif
        return;
    }

#ifdef USE_DBGTRACE
    DbgTrace(("OUTPUT [%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x]",
        block[ 0],block[ 1],block[ 2],block[ 3],
        block[ 4],block[ 5],block[ 6],block[ 7],
        block[ 8],block[ 9],block[10],block[11],
        block[12],block[13],block[14],block[15]));
#endif

}


//
// This routine can be used to compress a block to any of the modes with a shared index set
//
// It will encode the best result for this mode into a BC7 block
//
//
//
// For debugging this is a no color 4x4 BC7 block
//BYTE BlankBC7Block[16] = { 0x40, 0xC0, 0x1F, 0xF0, 0x07, 0xFC, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


double BC7BlockEncoder::CompressSingleIndexBlock(
    double      in[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],
    CMP_BYTE    out[COMPRESSED_BLOCK_SIZE],
    CMP_DWORD   blockMode)
{
#ifdef USE_DBGTRACE
    DbgTrace(("<---------CompressSingleIndexBlock----------->"));
#endif
    CMP_DWORD   i, k, n;
    CMP_DWORD   dimension;

    // Figure out the effective dimension of this block mode
    if(bti[blockMode].encodingType == NO_ALPHA)
    {
        dimension = 3;
    }
    else
    {
        dimension = 4;
    }

    CMP_DWORD numPartitionModes = 1 << bti[blockMode].partitionBits;
    CMP_DWORD partitionsToTry = numPartitionModes;

    // Linearly reduce the number of partitions to try as the quality falls below a threshold
    if(m_quality < g_qFAST_THRESHOLD) 
    {
        partitionsToTry = (CMP_DWORD)floor((double)(partitionsToTry * m_partitionSearchSize) + 0.5);
        partitionsToTry = min(numPartitionModes, max(1, partitionsToTry));
    }

    CMP_DWORD   blockPartition;
    double      partition[MAX_SUBSETS][MAX_SUBSET_SIZE][MAX_DIMENSION_BIG];
    CMP_DWORD   entryCount[MAX_SUBSETS];
    CMP_DWORD   subset;


#ifdef    BC7_DEBUG_TO_RESULTS_TXT
    fprintf(fp,"\CompressSingleIndexBlock\n");
    fprintf(fp,"blockMode = %d\n",blockMode);
    fprintf(fp,"numPartitionModes = %d\n",numPartitionModes);
    fprintf(fp,"partitionsToTry = %d\n",partitionsToTry);
    fprintf(fp,"m_blockMaxRange =  %4.0f\n",m_blockMaxRange);
    fprintf(fp,"m_quantizerRangeThreshold = %4.0f\n",m_quantizerRangeThreshold);
    fprintf(fp,"m_clusters[0] = %d\n",m_clusters[0]);
#endif

#ifdef USE_DBGTRACE
    DbgTrace(("blockMode [%d] numPartitionModes [%d] partitionsToTry [%2d]",
        blockMode,
        numPartitionModes,
        partitionsToTry));
    DbgTrace((" m_blockMaxRange [%2d] m_quantizerRangeThreshold [%4.0f] m_clusters[0] = %d",
        m_blockMaxRange,
        m_quantizerRangeThreshold,
        m_clusters[0]));
#endif

    // Loop over the available partitions for the block mode and quantize them 
    // to figure out the best candidates for further refinement
    for(blockPartition = 0;
        blockPartition < partitionsToTry;
        blockPartition++)
    {
        Partition(blockPartition,
                  in,
                  partition,
                  entryCount,
                  blockMode,
                  dimension);


        double  error = 0.;
        double  outB[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG];
        double  direction[MAX_DIMENSION_BIG];
        double  step;

        for(subset=0; subset < bti[blockMode].subsetCount; subset++)
        {
            int     indices[MAX_SUBSETS][MAX_SUBSET_SIZE];

            if(entryCount[subset])
            {

                if((m_clusters[0] > 8) ||
                   (m_blockMaxRange <= m_quantizerRangeThreshold))
                {

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                    fprintf(fp,"\noptQuantAnD_d\n");
#endif
                    error += optQuantAnD_d(partition[subset],
                                           entryCount[subset],
                                           m_clusters[0],
                                           indices[subset],
                                           outB,
                                           direction,
                                           &step,
                                           dimension);

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                    if (blockPartition == 11)
                    {
                            fprintf(fp,"\n");
                            for (int row=0; row<16; row++)
                            {
                                fprintf(fp,"partition[%2d] = %4.2f, %4.2f, %4.2f\n",row,partition[subset][row][0],partition[subset][row][1],partition[subset][row][2]);
                            }
                    
                    
                            fprintf(fp,"\n");
                            for (int row=0; row<16; row++)
                            {
                                fprintf(fp,"indices[0][%2d] = %4.2f\n",row,indices[0][row]);
                            }
                    
                            fprintf(fp,"\n");
                            for (int row=0; row<16; row++)
                            {
                                fprintf(fp,"outB[%2d] = %4.2f, %4.2f, %4.2f\n",row,outB[row][0],outB[row][1],outB[row][2]);
                            }
                    
                            fprintf(fp,"\n");
                            fprintf(fp,"entryCount = %d\n",entryCount[subset]);
                            fprintf(fp,"m_clusters[0] = %d\n",m_clusters[0]);
                            fprintf(fp,"Direction = %4.2f, %4.2f, %4.2f\n",direction[0],direction[1],direction[2]);
                            fprintf(fp,"step = %4.2f\n",step);
                            fprintf(fp,"dimension = %4.2f\n",dimension);
                            fprintf(fp,"error = %4.2f\n",error);
                    }                
#endif
                }
                else
                {
#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                    fprintf(fp,"\optQuantTrace_d\n");
#endif
                  error += optQuantTrace_d(partition[subset],
                                             entryCount[subset],
                                             m_clusters[0],
                                             indices[subset],
                                             outB,
                                             direction,
                                             &step,
                                             dimension);

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                    if (blockPartition == 11)
                    {
                            fprintf(fp,"\n");
                            for (int row=0; row<16; row++)
                            {
                                fprintf(fp,"partition[%2d] = %4.2f, %4.2f, %4.2f\n",row,partition[subset][row][0],partition[subset][row][1],partition[subset][row][2]);
                            }
                    
                    
                            fprintf(fp,"\n");
                            for (int row=0; row<16; row++)
                            {
                                fprintf(fp,"indices[0][%2d] = %4.2f\n",row,indices[0][row]);
                            }
                    
                            fprintf(fp,"\n");
                            for (int row=0; row<16; row++)
                            {
                                fprintf(fp,"outB[%2d] = %4.2f, %4.2f, %4.2f\n",row,outB[row][0],outB[row][1],outB[row][2]);
                            }
                    
                            fprintf(fp,"\n");
                            fprintf(fp,"entryCount = %d\n",entryCount[subset]);
                            fprintf(fp,"m_clusters[0] = %d\n",m_clusters[0]);
                            fprintf(fp,"Direction = %4.2f, %4.2f, %4.2f\n",direction[0],direction[1],direction[2]);
                            fprintf(fp,"step = %4.2f\n",step);
                            fprintf(fp,"dimension = %4.2f\n",dimension);
                            fprintf(fp,"error = %4.2f\n",error);
                    }
#endif
                
                }

                // Store off the indices for later
                for(CMP_DWORD idx=0; idx < entryCount[subset]; idx++)
                {
                    m_storedIndices[blockPartition][subset][idx] = indices[subset][idx];
                }
            }
        }

        m_storedError[blockPartition] = error;
    }

    // Sort the results
    sortProjection(m_storedError,
                   m_sortedModes,
                   partitionsToTry);


    // Run shaking (endpoint refinement) pass for partitions that gave the
    // best set of errors from quantization

    // ep_shaker will take its endpoint information from bits[0-2]
    // ep_shaker_2_d will take its information from bits[3]
    int     bits[4] = {0,0,0,0};

    // ep_shaker_d needs bits specified individually per channel including parity
    bits[0] = m_componentBits[COMP_RED]   + (m_parityBits ? 1:0);
    bits[1] = m_componentBits[COMP_GREEN] + (m_parityBits ? 1:0);
    bits[2] = m_componentBits[COMP_BLUE]  + (m_parityBits ? 1:0);

    // ep_shaker_2_d needs bits specified as total bits for both endpoints including parity
    for(i=0; i < dimension; i++)
    {
        bits[3] += m_componentBits[i];
    }
    bits[3] *= 2;
    if(m_parityBits == BCC)
    {
        bits[3] += 2;
    }
    else if (m_parityBits == SAME_PAR)
    {
        bits[3] += 1;
    }

    int     epo_code[MAX_SUBSETS][2][MAX_DIMENSION_BIG];
    double  epo[2][MAX_DIMENSION_BIG];
    double  outB[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG];

    int     bestEndpoints[MAX_SUBSETS][2][MAX_DIMENSION_BIG];
    int     bestIndices[MAX_SUBSETS][MAX_SUBSET_SIZE];
    CMP_DWORD   bestEntryCount[MAX_SUBSETS];
    CMP_DWORD   bestPartition = 0;
    double  bestError = DBL_MAX;

    // Extensive shaking is most important when the ramp is short, and
    // when we have less indices. On a long ramp the quality of the
    // initial quantizing is relatively more important
    // We modulate the shake size according to the number of ramp indices
    // - the more indices we have the less shaking should be required to find a near
    // optimal match

    // shakeSize gives the size of the shake cube (for ep_shaker_2_d)
    // ep_shaker always runs on a 1x1x1 cube on both endpoints
    CMP_DWORD   shakeSize = 8 - (CMP_DWORD)floor(1.5 * bti[blockMode].indexBits[0]);
    shakeSize = max(2, min((CMP_DWORD)floor( shakeSize * m_quality + 0.5), 6));

    // Shake attempts indicates how many partitions to try to shake
    CMP_DWORD   numShakeAttempts = max(1, min((CMP_DWORD)floor(8 * m_quality + 0.5), partitionsToTry));

    // Set up all the parameters for the shakers
    // Must increase shake size if these block endpoints use parity
    if((m_parityBits == SAME_PAR) ||
       (m_parityBits == BCC))
    {
        shakeSize += 2;
    }

#ifdef USE_DBGTRACE
    DbgTrace(("%2d numPartitionModes %2d SearchSize %3.3f shakeSize %2d numShakeAttempts %2d\n",
     partitionsToTry,
     numPartitionModes,
     m_partitionSearchSize,
     shakeSize,
     numShakeAttempts));
#endif

    // Now do the endpoint shaking
    for(i=0; i < numShakeAttempts; i++)
    {
        double error = 0;

        blockPartition = m_sortedModes[i];

        Partition(blockPartition,
                  in,
                  partition,
                  entryCount,
                  blockMode,
                  dimension);

        for(subset=0; subset < bti[blockMode].subsetCount; subset++)
        {
            if(entryCount[subset])
            {
                // If quality is set low or the dimension is not compatible with
                // shaker_d then just run shaker_2_d
                if((m_blockMaxRange > m_shakerRangeThreshold) ||
                   (dimension != 3))
                {
                    error += ep_shaker_2_d(partition[subset],
                                          entryCount[subset],
                                          m_storedIndices[blockPartition][subset],
                                          outB,
                                          epo_code[subset],
                                          shakeSize,
                                          m_clusters[0]-1,
                                          bits[3],
                                          dimension,
                                          epo);
                }
                else
                {
                    double  tempError[2];
                    int     tempIndices[MAX_SUBSET_SIZE];
                    int     temp_epo_code[2][MAX_DIMENSION_BIG];

                    // Step one - run ep_shaker and ep_shaker_2 in parallel, and get the error from each

                    for(k=0; k < entryCount[subset]; k++)
                    {
                        tempIndices[k] = m_storedIndices[blockPartition][subset][k];
                    }
                    tempError[0] = ep_shaker_d(partition[subset],
                                               entryCount[subset],
                                               tempIndices,
                                               outB,
                                               temp_epo_code,
                                               m_clusters[0]-1,
                                               bits,
                                               (CMP_qt)m_parityBits,
                                               dimension);

                    tempError[1] = ep_shaker_2_d(partition[subset],
                                                 entryCount[subset],
                                                 m_storedIndices[blockPartition][subset],
                                                 outB,
                                                 epo_code[subset],
                                                 shakeSize,
                                                 m_clusters[0]-1,
                                                 bits[3],
                                                 dimension,
                                                 epo);

                    if(tempError[0] < tempError[1])
                    {
                        // If ep_shaker did better than ep_shaker_2 then we need to reshake
                        // the output from ep_shaker using ep_shaker_2 for further refinement

                        tempError[1] = ep_shaker_2_d(partition[subset],
                                                     entryCount[subset],
                                                     tempIndices,
                                                     outB,
                                                     temp_epo_code,
                                                     shakeSize,
                                                     m_clusters[0]-1,
                                                     bits[3],
                                                     dimension,
                                                     epo);

                        // Copy the results into the expected location
                        for(k=0; k<entryCount[subset]; k++)
                        {
                            m_storedIndices[blockPartition][subset][k] = tempIndices[k];
                        }

                        for(k=0; k < MAX_DIMENSION_BIG; k++)
                        {
                            epo_code[subset][0][k] = temp_epo_code[0][k];
                            epo_code[subset][1][k] = temp_epo_code[1][k];
                        }
                    }

                    error += tempError[1];
                }
            }
        }


        if(error < bestError)
        {
            bestPartition = blockPartition;

            for(subset=0; subset < bti[blockMode].subsetCount; subset++)
            {
                bestEntryCount[subset] = entryCount[subset];

                if(entryCount[subset])
                {
                    for(k=0; k < dimension; k++)
                    {
                        bestEndpoints[subset][0][k] = epo_code[subset][0][k];
                        bestEndpoints[subset][1][k] = epo_code[subset][1][k];
                    }

                    for(n=0; n < entryCount[subset]; n++)
                    {
                        bestIndices[subset][n] = m_storedIndices[blockPartition][subset][n];
                    }
                }
            }

            bestError = error;
        }

        // Early out if we  found we can compress with error below the quality threshold
        if (m_errorThreshold > 0)
        {
            if(bestError <= m_errorThreshold)
            {
                break;
            }
        }
    }

    // Now we have all the data needed to encode the block
    // We need to pack the endpoints prior to encoding
    CMP_DWORD   packedEndpoints[3][2];
    for(subset=0; subset<bti[blockMode].subsetCount; subset++)
    {
        if(bestEntryCount[subset])
        {
            CMP_DWORD   rightAlignment = 0;
            packedEndpoints[subset][0] = 0;
            packedEndpoints[subset][1] = 0;

            // Sort out parity bits
            if(m_parityBits != CART)
            {
                packedEndpoints[subset][0] = bestEndpoints[subset][0][0] & 1;
                packedEndpoints[subset][1] = bestEndpoints[subset][1][0] & 1;
                for(k=0; k<MAX_DIMENSION_BIG; k++)
                {
                    bestEndpoints[subset][0][k] >>= 1;
                    bestEndpoints[subset][1][k] >>= 1;
                }
                rightAlignment++;
            }

            // Fixup endpoints
            for(k=0; k<dimension; k++)
            {
                if(m_componentBits[k])
                {
                    packedEndpoints[subset][0] |= bestEndpoints[subset][0][k] << rightAlignment;
                    packedEndpoints[subset][1] |= bestEndpoints[subset][1][k] << rightAlignment;
                    rightAlignment += m_componentBits[k];
                }
            }
        }
    }

    // Save the data to output
    EncodeSingleIndexBlock(blockMode,
                bestPartition,
                packedEndpoints,
                bestIndices,
                out);
    return bestError;
}

static CMP_DWORD   componentRotations[4][4] =
{
    {COMP_ALPHA, COMP_RED,   COMP_GREEN, COMP_BLUE},
    {COMP_RED,   COMP_ALPHA, COMP_GREEN, COMP_BLUE},
    {COMP_GREEN, COMP_RED,   COMP_ALPHA, COMP_BLUE},
    {COMP_BLUE,  COMP_RED,   COMP_GREEN, COMP_ALPHA}
};

void BC7BlockEncoder::EncodeDualIndexBlock(CMP_DWORD blockMode,
    CMP_DWORD indexSelection,
    CMP_DWORD componentRotation,
    int endpoint[2][2][MAX_DIMENSION_BIG],
    int indices[2][MAX_SUBSET_SIZE],
    CMP_BYTE   out[COMPRESSED_BLOCK_SIZE])
{

#ifdef USE_DBGTRACE
    DbgTrace(("-> WriteBit()"));
#endif
    CMP_DWORD i,j,k;
    int   bitPosition = 0;    // Position the pointer at the LSB
    CMP_BYTE  *basePtr = out;
    CMP_DWORD idxBits[2];
    CMP_BOOL swapIndices;

    // Generate Unary header for this mode
    for(i=0;i<blockMode;i++)
    {
        WriteBit(basePtr, bitPosition++, 0);
    }
    WriteBit(basePtr, bitPosition++, 1);

    // Write rotation bits
    for(i=0; i<bti[blockMode].rotationBits; i++)
    {
        WriteBit(basePtr, bitPosition++, (CMP_BYTE)((componentRotation>>i) & 0xff));
    }

    // Write index selector bits
    for(i=0;i<bti[blockMode].indexModeBits; i++)
    {
        WriteBit(basePtr, bitPosition++, (CMP_BYTE)(indexSelection ? 1: 0));
    }

    if(indexSelection)
    {
        swapIndices = TRUE;
        idxBits[0] = bti[blockMode].indexBits[1];
        idxBits[1] = bti[blockMode].indexBits[0];
    }
    else
    {
        swapIndices = FALSE;
        idxBits[0] = bti[blockMode].indexBits[0];
        idxBits[1] = bti[blockMode].indexBits[1];
    }

    bool   flipColours[2] = {false, false};

    // Indicate if we need to fixup the indices
    if(indices[0][0] & (1<<(idxBits[0]-1)))
    {
        flipColours[0] = true;
    }
    if(indices[1][0] & (1<<(idxBits[1]-1)))
    {
        flipColours[1] = true;
    }

    // Fixup the indices
    for(i=0;i<2;i++)
    {
        if(flipColours[i])
        {
            for(j=0;j<MAX_SUBSET_SIZE;j++)
            {
                indices[i][j] = ((1 << idxBits[i]) - 1) - indices[i][j];
            }
        }
    }

    // Now fixup the endpoints so that the implicitly encoded
    // index bits have the correct state
    for(i=0; i<2; i++)
    {
        if(flipColours[i])
        {
            for(k=0;k<4;k++)
            {
                CMP_DWORD   temp;
                temp = endpoint[i][0][k];
                endpoint[i][0][k] = endpoint[i][1][k];
                endpoint[i][1][k] = temp;
            }
        }
    }

    CMP_DWORD   ep, component;
    // Encode the colour and alpha information
    CMP_DWORD   vectorComponentBits = bti[blockMode].vectorBits / 3;

    // Loop over components
    for(component=0; component < MAX_DIMENSION_BIG; component++)
    {
        if(component != COMP_ALPHA)
        {
            for(ep=0; ep<2; ep++)
            {
                for(k=0;k<vectorComponentBits; k++)
                {
                    WriteBit(basePtr,
                            bitPosition++,
                            (CMP_BYTE)((endpoint[0][ep][component] >> k) & 0x1));
                }
            }
        }
        else
        {
            for(ep=0; ep<2; ep++)
            {
                for(j=0; j<bti[blockMode].scalarBits; j++)
                {
                    WriteBit(basePtr,
                             bitPosition++,
                            (CMP_BYTE)((endpoint[1][ep][0] >> j) & 0x1));
                }
            }
        }
    }

    // Now encode the index bits
    for(i=0; i<2; i++)
    {
        CMP_DWORD   idxSelect = i;

        if(swapIndices)
        {
            idxSelect = i ^ 1;
        }
        for(j=0;j<MAX_SUBSET_SIZE;j++)
        {
            if(j==0)
            {
                for(k=0;k<(idxBits[idxSelect]-1); k++)
                {
                    WriteBit(basePtr, bitPosition++,(CMP_BYTE)(indices[idxSelect][j]>>k));
                }
            }
            else
            {
                for(k=0;k<idxBits[idxSelect]; k++)
                {
                    WriteBit(basePtr, bitPosition++,(CMP_BYTE)(indices[idxSelect][j]>>k));
                }
            }
        }
    }

    // Check that we encoded exactly the right number of bits
    if(bitPosition != (COMPRESSED_BLOCK_SIZE * 8))
    {
        return;
    }

#ifdef USE_DBGTRACE
    DbgTrace(("OUTPUT [%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x]",
        out[ 0],out[ 1],out[ 2],out[ 3],
        out[ 4],out[ 5],out[ 6],out[ 7],
        out[ 8],out[ 9],out[10],out[11],
        out[12],out[13],out[14],out[15]));
#endif

}


double BC7BlockEncoder::CompressDualIndexBlock(double in[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],
    CMP_BYTE   out[COMPRESSED_BLOCK_SIZE],
    CMP_DWORD  blockMode)
{
#ifdef USE_DBGTRACE
    DbgTrace(("<---------CompressDualIndexBlock----------->"));
#endif
    CMP_DWORD   i;
    double  cBlock[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG];
    double  aBlock[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG];

    CMP_DWORD maxRotation = 1 << bti[blockMode].rotationBits;
    CMP_DWORD rotation;

    CMP_DWORD maxIndexSelection = 1 << bti[blockMode].indexModeBits;
    CMP_DWORD indexSelection;

    int        indices[2][MAX_SUBSET_SIZE];
    double  outQ[2][MAX_SUBSET_SIZE][MAX_DIMENSION_BIG];
    double  direction[MAX_DIMENSION_BIG];
    double  step;

    double quantizerError;
    double bestQuantizerError = DBL_MAX;
    double overallError;
    double bestOverallError   = DBL_MAX;

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
    fprintf(fp,"\nCompressDualIndexBlock\n");
    fprintf(fp,"blockMode = %d\n",blockMode);
    fprintf(fp,"maxIndexSelection = %d\n",maxIndexSelection);
    fprintf(fp,"maxRotation = %d\n",maxRotation);
    fprintf(fp,"m_blockMaxRange =  %4.0f\n",m_blockMaxRange);
    fprintf(fp,"m_quantizerRangeThreshold = %4.0f\n",m_quantizerRangeThreshold);
#endif
    
    // Go through each possible rotation and selection of indices
    for(rotation = 0; rotation < maxRotation; rotation++)  
    { // A


        for(i=0; i<MAX_SUBSET_SIZE; i++)
        { 
            cBlock[i][COMP_RED]   = in[i][componentRotations[rotation][1]];
            cBlock[i][COMP_GREEN] = in[i][componentRotations[rotation][2]];
            cBlock[i][COMP_BLUE]  = in[i][componentRotations[rotation][3]];

            aBlock[i][COMP_RED]   = in[i][componentRotations[rotation][0]];
            aBlock[i][COMP_GREEN] = in[i][componentRotations[rotation][0]];
            aBlock[i][COMP_BLUE]  = in[i][componentRotations[rotation][0]];
        }

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
        fprintf(fp,"\ncBlock[16][3]\n");
        for(i=0; i<MAX_SUBSET_SIZE; i++)
        { 
            fprintf(fp,"%4.0f, %4.0f, %4.0f\n",cBlock[i][COMP_RED],cBlock[i][COMP_GREEN],cBlock[i][COMP_BLUE]);
        }
        

        fprintf(fp,"\naBlock[16][3]\n");
        for(i=0; i<MAX_SUBSET_SIZE; i++)
        { 
            fprintf(fp,"%4.0f, %4.0f, %4.0f\n",aBlock[i][COMP_RED],aBlock[i][COMP_GREEN],aBlock[i][COMP_BLUE]);
        }
#endif

        for(indexSelection = 0; indexSelection < maxIndexSelection; indexSelection++)
        { // B
            quantizerError = 0.;
            
#ifdef    BC7_DEBUG_TO_RESULTS_TXT
            fprintf(fp,"\n-------------- Quantize the vector block ----------------\n");
#endif
            // Quantize the vector block
            if(m_blockMaxRange <= m_quantizerRangeThreshold)
            {

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                fprintf(fp,"\noptQuantAnD_d\n");
                fprintf(fp,"IndexSelection = %d\n",indexSelection);
                fprintf(fp,"NumClusters = %d\n",1 << bti[blockMode].indexBits[0 ^ indexSelection]);
#endif
                quantizerError = optQuantAnD_d(cBlock,
                                    MAX_SUBSET_SIZE,
                                    (1 << bti[blockMode].indexBits[0 ^ indexSelection]),
                                    indices[0],
                                    outQ[0],
                                    direction,
                                    &step,
                                    3);
                
#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                fprintf(fp,"\n");
                for (int row=0; row<16; row++)
                {
                    fprintf(fp,"indices[0][%2d] = %4.2f\n",row,indices[0][row]);
                }
                
                fprintf(fp,"\n");
                for (int row=0; row<16; row++)
                {
                    fprintf(fp,"outQ[0][%2d] = %4.2f, %4.2f, %4.2f\n",row,outQ[0][row][0],outQ[0][row][1],outQ[0][row][2]);
                }
                
                fprintf(fp,"\n");
                fprintf(fp,"Direction = %4.2f, %4.2f, %4.2f\n",direction[0],direction[1],direction[2]);
                fprintf(fp,"step = %4.2f\n",step);
                fprintf(fp,"quantizerError = %4.2f\n",quantizerError);
#endif
            
            }
            else
            {

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                fprintf(fp,"\noptQuantTrace_d\n");
                fprintf(fp,"IndexSelection = %d\n",indexSelection);
                fprintf(fp,"NumClusters = %d\n",1 << bti[blockMode].indexBits[0 ^ indexSelection]);
#endif
                quantizerError = optQuantTrace_d(cBlock,
                                    MAX_SUBSET_SIZE,
                                    (1 << bti[blockMode].indexBits[0 ^ indexSelection]),
                                    indices[0],
                                    outQ[0],
                                    direction,
                                    &step,
                                    3);

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                fprintf(fp,"\n");
                for (int row=0; row<16; row++)
                {
                    fprintf(fp,"indices[0][%2d] = %4.2f\n",row,indices[0][row]);
                }
                
                fprintf(fp,"\n");
                for (int row=0; row<16; row++)
                {
                    fprintf(fp,"outQ[0][%2d] = %4.2f, %4.2f, %4.2f\n",row,outQ[0][row][0],outQ[0][row][1],outQ[0][row][2]);
                }
                
                fprintf(fp,"\n");
                fprintf(fp,"Direction = %4.2f, %4.2f, %4.2f\n",direction[0],direction[1],direction[2]);
                fprintf(fp,"step = %4.2f\n",step);
                fprintf(fp,"quantizerError = %4.2f\n",quantizerError);
#endif

            }

            // Quantize the scalar block
#ifdef    BC7_DEBUG_TO_RESULTS_TXT
            fprintf(fp,"\nQuantize the scalar block\n");
#endif
            if(m_blockMaxRange <= m_quantizerRangeThreshold)
            {

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                fprintf(fp,"\noptQuantAnD_d\n");
                fprintf(fp,"IndexSelection = %d\n",indexSelection);
                fprintf(fp,"NumClusters = %d\n",1 << bti[blockMode].indexBits[1 ^ indexSelection]);
#endif
                quantizerError += optQuantAnD_d(aBlock,
                                 MAX_SUBSET_SIZE,
                                 (1 << bti[blockMode].indexBits[1 ^ indexSelection]),
                                 indices[1],
                                 outQ[1],
                                 direction,
                                 &step,
                                 3) / 3.;

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                fprintf(fp,"\n");
                for (int row=0; row<16; row++)
                {
                    fprintf(fp,"indices[1][%2d] = %4.2f\n",row,indices[1][row]);
                }
                
                fprintf(fp,"\n");
                for (int row=0; row<16; row++)
                {
                    fprintf(fp,"outQ[1][%2d] = %4.2f, %4.2f, %4.2f\n",row,outQ[1][row][0],outQ[1][row][1],outQ[1][row][2]);
                }
                
                fprintf(fp,"\n");
                fprintf(fp,"Direction = %4.2f, %4.2f, %4.2f\n",direction[0],direction[1],direction[2]);
                fprintf(fp,"step = %4.2f\n",step);
                fprintf(fp,"quantizerError = %4.2f\n",quantizerError);
#endif
            }
            else
            {
#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                fprintf(fp,"\noptQuantTrace_d\n");
                fprintf(fp,"IndexSelection = %d\n",indexSelection);
                fprintf(fp,"NumClusters = %d\n",1 << bti[blockMode].indexBits[1 ^ indexSelection]);
#endif
                quantizerError += optQuantTrace_d(aBlock,
                                 MAX_SUBSET_SIZE,
                                 (1 << bti[blockMode].indexBits[1 ^ indexSelection]),
                                 indices[1],
                                 outQ[1],
                                 direction,
                                 &step,
                                 3) / 3.;

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
                fprintf(fp,"\n");
                for (int row=0; row<16; row++)
                {
                    fprintf(fp,"indices[1][%2d] = %4.2f\n",row,indices[1][row]);
                }
                
                fprintf(fp,"\n");
                for (int row=0; row<16; row++)
                {
                    fprintf(fp,"outQ[1][%2d] = %4.2f, %4.2f, %4.2f\n",row,outQ[1][row][0],outQ[1][row][1],outQ[1][row][2]);
                }
                
                fprintf(fp,"\n");
                fprintf(fp,"Direction = %4.2f, %4.2f, %4.2f\n",direction[0],direction[1],direction[2]);
                fprintf(fp,"step = %4.2f\n",step);
                fprintf(fp,"quantizerError = %4.2f\n",quantizerError);
#endif

            }

            // If quality is high then run the full shaking for this config and
            // store the result if it beats the best overall error
            // Otherwise only run the shaking if the error is better than the best
            // quantizer error
            if((m_quality > g_HIGHQULITY_THRESHOLD) || (quantizerError <= bestQuantizerError))
            {
                // Shake size gives the size of the shake cube
                CMP_DWORD   shakeSize;

                shakeSize = max(2, min( (CMP_DWORD)(6 * m_quality), 6));

                int     bits[2][4];

                // Specify number of bits for vector block
                bits[0][COMP_RED] = m_componentBits[COMP_RED];
                bits[0][COMP_GREEN] = m_componentBits[COMP_GREEN];
                bits[0][COMP_BLUE] = m_componentBits[COMP_BLUE];
                bits[0][3] = 2 * (m_componentBits[COMP_RED] + m_componentBits[COMP_GREEN] + m_componentBits[COMP_BLUE]);

                // Specify number of bits for scalar block
                bits[1][0] = m_componentBits[COMP_ALPHA];
                bits[1][1] = m_componentBits[COMP_ALPHA];
                bits[1][2] = m_componentBits[COMP_ALPHA];
                bits[1][3] = 6 * m_componentBits[COMP_ALPHA];

                overallError = 0;
                int     epo_code[2][2][MAX_DIMENSION_BIG];
                double  epo[2][MAX_DIMENSION_BIG];

                if(m_blockMaxRange > m_shakerRangeThreshold)
                {
                    overallError += ep_shaker_2_d(cBlock,
                                                  MAX_SUBSET_SIZE,
                                                  indices[0],
                                                  outQ[0],
                                                  epo_code[0],
                                                  shakeSize,
                                                  (1 << bti[blockMode].indexBits[0 ^ indexSelection])-1,
                                                  bits[0][3],
                                                  3,
                                                  epo);
                }
                else
                {
                    ep_shaker_d(cBlock,
                                MAX_SUBSET_SIZE,
                                indices[0],
                                outQ[0],
                                epo_code[0],
                                (1 << bti[blockMode].indexBits[0 ^ indexSelection])-1,
                                bits[0],
                                (CMP_qt)0,
                                3);

                     overallError += ep_shaker_2_d(cBlock,
                                            MAX_SUBSET_SIZE,
                                            indices[0],
                                            outQ[0],
                                            epo_code[0],
                                            shakeSize,
                                            (1 << bti[blockMode].indexBits[0 ^ indexSelection])-1,
                                            bits[0][3],
                                            3,
                                            epo);
                }

                if(m_blockMaxRange > m_shakerRangeThreshold)
                {
                    overallError += ep_shaker_2_d(aBlock,
                                                  MAX_SUBSET_SIZE,
                                                  indices[1],
                                                  outQ[1],
                                                  epo_code[1],
                                                  shakeSize,
                                                  (1 << bti[blockMode].indexBits[1 ^ indexSelection])-1,
                                                  bits[1][3],
                                                  3,
                                                  epo) / 3.;
                }
                else
                {
                    ep_shaker_d(aBlock,
                                MAX_SUBSET_SIZE,
                                indices[1],
                                outQ[1],
                                epo_code[1],
                                (1 << bti[blockMode].indexBits[1 ^ indexSelection])-1,
                                bits[1],
                                (CMP_qt)0,
                                3);

                    overallError += ep_shaker_2_d(aBlock,
                                        MAX_SUBSET_SIZE,
                                        indices[1],
                                        outQ[1],
                                        epo_code[1],
                                        shakeSize,
                                        (1 << bti[blockMode].indexBits[1 ^ indexSelection])-1,
                                        bits[1][3],
                                        3,
                                        epo) / 3.;
                }

                // If we beat the previous best then encode the block
                if(overallError < bestOverallError)
                {
                    EncodeDualIndexBlock(blockMode,
                                         indexSelection,
                                         rotation,
                                         epo_code,
                                         indices,
                                         out);

                    bestOverallError = overallError;
                }

                if(quantizerError < bestQuantizerError)
                {
                    bestQuantizerError = quantizerError;
                }

            }
        } // B
    } // A
    return bestOverallError;
}



//
// This routine compresses a block and returns the RMS error
//
//
//
//
#include <stdio.h>

double BC7BlockEncoder::CompressBlock(double in[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],
    CMP_BYTE   out[COMPRESSED_BLOCK_SIZE])
{

#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    CMP_DWORD   i, j;
    CMP_BOOL    blockNeedsAlpha        = FALSE;
    CMP_BOOL    blockAlphaZeroOne      = FALSE;
    CMP_DWORD   validModeMask          = m_validModeMask;
    CMP_BOOL    encodedBlock           = FALSE;

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
    fp = fopen("debugdata.txt","w");
    if (fp)
    {
        fprintf(fp,"Data INPUT\n");
        double  data[16][4];
        memcpy(data,in,sizeof(data));
        for (int row=0; row<16; row++)
            fprintf(fp,"%4.0f, %4.0f, %4.0f\n", data[row][0],data[row][1],data[row][2]);
#endif

    for(i=0;i<MAX_DIMENSION_BIG;i++)
    {
        m_blockMin[i] = DBL_MAX;
        m_blockMax[i] = 0.0;
        m_blockRange[i] = 0.0;
    }

    // Check if the input block has any alpha values that are not 1
    // We assume 8-bit input here, so 1 is mapped to 255.
    // Also check if the block encodes an explicit zero or one in the
    // alpha channel. If so then we might need also need special as the
    // block may have a thresholded or punch-through alpha
    for(i=0; i<MAX_SUBSET_SIZE; i++)
    {
        if(in[i][COMP_ALPHA] != 255.0)
        {
            blockNeedsAlpha = TRUE;
        }
        else
        if((in[i][COMP_ALPHA] == 255.0) ||
           (in[i][COMP_ALPHA] == 0.0))
        {
            blockAlphaZeroOne = TRUE;
        }
    }

    for(i=0;i<MAX_SUBSET_SIZE;i++)
    {
        for(j=0;j<MAX_DIMENSION_BIG;j++)
        {
            m_blockMin[j] = (in[i][j] < m_blockMin[j]) ? in[i][j] : m_blockMin[j];
            m_blockMax[j] = (in[i][j] > m_blockMax[j]) ? in[i][j] : m_blockMax[j];
        }
    }

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
    fprintf(fp,"m_blockMin[0] = %4.2f\n",m_blockMin[0]);
    fprintf(fp,"m_blockMin[1] = %4.2f\n",m_blockMin[1]);
    fprintf(fp,"m_blockMin[2] = %4.2f\n",m_blockMin[2]);
    fprintf(fp,"m_blockMin[3] = %4.2f\n\n",m_blockMin[3]);
    
    fprintf(fp,"m_blockMax[0] = %4.2f\n",m_blockMax[0]);
    fprintf(fp,"m_blockMax[1] = %4.2f\n",m_blockMax[1]);
    fprintf(fp,"m_blockMax[2] = %4.2f\n",m_blockMax[2]);
    fprintf(fp,"m_blockMax[3] = %4.2f\n\n",m_blockMax[3]);
#endif

    m_blockRange[0] = m_blockMax[0] - m_blockMin[0];
    m_blockRange[1] = m_blockMax[1] - m_blockMin[1];
    m_blockRange[2] = m_blockMax[2] - m_blockMin[2];
    m_blockRange[3] = m_blockMax[3] - m_blockMin[3];
    m_blockMaxRange = max(m_blockRange[0], m_blockRange[1]);
    m_blockMaxRange = max(m_blockMaxRange, m_blockRange[2]);
    m_blockMaxRange = max(m_blockMaxRange, m_blockRange[3]);


#ifdef    BC7_DEBUG_TO_RESULTS_TXT
    fprintf(fp,"m_blockRange[0] = %4.2f\n",m_blockRange[0]);
    fprintf(fp,"m_blockRange[1] = %4.2f\n",m_blockRange[1]);
    fprintf(fp,"m_blockRange[2] = %4.2f\n",m_blockRange[2]);
    fprintf(fp,"m_blockRange[3] = %4.2f\n",m_blockRange[3]);
    fprintf(fp,"m_blockMaxRange = %4.2f\n\n",m_blockMaxRange);
    
    fprintf(fp,"=========================================\n");
#endif


    // Initial loop - go through the block modes and get the ones that are valid
    for(CMP_DWORD blockMode=0; blockMode < NUM_BLOCK_TYPES; blockMode++)
    {
        // Check if this mode is allowed based on the global settings
        if(!(validModeMask & (1 << blockMode)))
        {
            continue;
        }

        // If the block needs Alpha and this mode doesn't support alpha then
        // indicate that this is not a valid mode and continue
        if((blockNeedsAlpha == TRUE) &&
           (bti[blockMode].encodingType == NO_ALPHA))
        {
            validModeMask &= ~(1<<blockMode);
        }

        // Optional restriction for colour-only blocks so that they
        // don't use modes that have combined colour+alpha - this
        // avoids the possibility that the encoder might choose an
        // alpha other than 1.0 (due to parity) and cause something to
        // become accidentally slightly transparent (it's possible that
        // when encoding 3-component texture applications will assume that
        // the 4th component can safely be assumed to be 1.0 all the time)
        if((blockNeedsAlpha == FALSE) &&
           (m_colourRestrict == TRUE) &&
           (bti[blockMode].encodingType == COMBINED_ALPHA))
        {
            validModeMask &= ~(1<<blockMode);
        }

        // Optional restriction for blocks with alpha to avoid issues with
        // punch-through or thresholded alpha encoding
        if((blockNeedsAlpha == TRUE) &&
           (m_alphaRestrict == TRUE) &&
           (blockAlphaZeroOne == TRUE) &&
           (bti[blockMode].encodingType == COMBINED_ALPHA))
        {
            validModeMask &= ~(1<<blockMode);
        }
    }

    assert(validModeMask != 0);

#ifdef USE_DBGTRACE
    DbgTrace(("validModeMask [%x]",validModeMask));
#endif
    // Try all the legal block modes that we flagged

    CMP_BYTE    temporaryOutputBlock[COMPRESSED_BLOCK_SIZE];
    double bestError = DBL_MAX;
    double thisError;
    CMP_DWORD bestblockMode=99;

    // We change the order in which we visit the block modes to try to maximize the chance
    // that we manage to early out as quickly as possible.
    // This is a significant performance optimization for the lower quality modes where the
    // exit threshold is higher, and also tends to improve quality (as the generally higher quality
    // modes are now enumerated earlier, so the first encoding that passes the threshold will
    // tend to pass by a greater margin than if we used a dumb ordering, and thus overall error will
    // be improved)
    CMP_DWORD   blockModeOrder[NUM_BLOCK_TYPES] = {6, 4, 3, 1, 0, 2, 7, 5};

    // used for debugging and mode tests
    //                76543210
    // validModeMask = 0b00100000;

    for(CMP_DWORD j1=0; j1 < NUM_BLOCK_TYPES; j1++)
    {
        CMP_DWORD blockMode = blockModeOrder[j1];
        CMP_DWORD Mode = 0x0001 << blockMode;

        if(!(validModeMask & Mode))
        {
            continue;
        }

        // CPU:HPC #1
        // Setup mode parameters for this block
        BlockSetup(blockMode);
        
        if(bti[blockMode].encodingType != SEPARATE_ALPHA)
        {
       
            #ifdef    BC7_DEBUG_TO_RESULTS_TXT
            fprintf(fp,"=================== CompressSingleIndexBlock ======================\n");
            #endif
            thisError = CompressSingleIndexBlock(in, temporaryOutputBlock, blockMode);

        }
        else
        {
            
            #ifdef    BC7_DEBUG_TO_RESULTS_TXT
            fprintf(fp,"==================  CompressDualIndexBlock =======================\n");
            #endif
       
            thisError = CompressDualIndexBlock(in, temporaryOutputBlock, blockMode);
        }

        // If this compression did better than all previous attempts then copy the result
        // to the output block
        if(thisError < bestError)
        {
            for(i=0; i < COMPRESSED_BLOCK_SIZE; i++)
            {
                out[i] = temporaryOutputBlock[i];
            }
            bestError = thisError;
            encodedBlock = TRUE;
            bestblockMode = blockMode;
        }

        // If we have achieved an error lower than the requirement threshold then just exit now
        // Early out if we  found we can compress with error below the quality threshold
        if (m_errorThreshold > 0)
        {
            if(bestError <= m_errorThreshold)
            {
                break;
            }
        }
    }

    if(bestError < m_smallestError)
    {
        m_smallestError = bestError;
    }
    if(bestError > m_largestError)
    {
        m_largestError = bestError;
    }

    if (!encodedBlock)
    {
        // return some sort of error and abort sequence!
        encodedBlock = FALSE;
    }

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
    fclose(fp);
#endif

    return bestError;

#ifdef    BC7_DEBUG_TO_RESULTS_TXT
    }
    else return (0);
#endif

}



