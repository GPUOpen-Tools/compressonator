//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//===============================================================================

// ASTC_AMD_find_best_partitioning.cpp : finding partition candidates
//

#include <assert.h>

#include "ASTC_Encode.h"
#include "astc_codec_internals.h"
#include "softfloat.h"

#ifdef ASTC_COMPDEBUGGER
#include "compclient.h"
#endif

void ASTCBatchBlockEncoder::find_best_partitionings_batch(int * best_partitions_1plane, int * best_partitions_2planes)
{
    for (int blk_offset = 0; blk_offset < cur_batch_size; blk_offset++)
    {
        if (blk_finished[blk_offset])
            continue;

        int p1_offset = 0;
        int p2_offset = 0;
        int partition_count = 2;
        p1_offset = blk_offset * ASTC_PARTITIONS_CANDIDATES_1PLANE + (partition_count - 2) * ASTC_PARTITION_CANDIDATES_PER_MODE;
        p2_offset = blk_offset * ASTC_PARTITIONS_CANDIDATES_2PLANES + (partition_count - 2) * ASTC_PARTITION_CANDIDATES_PER_MODE;
        find_best_partitionings(ewp->partition_search_limit, xdim, ydim, zdim, partition_count, &pb[blk_offset], &ewb[blk_offset], 1,
            &(best_partitions_1plane[p1_offset]), &(best_partitions_1plane[p1_offset + 1]), &(best_partitions_2planes[p2_offset]));

        partition_count = 3;
        p1_offset = blk_offset * ASTC_PARTITIONS_CANDIDATES_1PLANE + (partition_count - 2) * ASTC_PARTITION_CANDIDATES_PER_MODE;
        p2_offset = blk_offset * ASTC_PARTITIONS_CANDIDATES_2PLANES + (partition_count - 2) * ASTC_PARTITION_CANDIDATES_PER_MODE;
        find_best_partitionings(ewp->partition_search_limit, xdim, ydim, zdim, partition_count, &pb[blk_offset], &ewb[blk_offset], 1,
            &(best_partitions_1plane[p1_offset]), &(best_partitions_1plane[p1_offset + 1]), &(best_partitions_2planes[p2_offset]));

        partition_count = 4;
        p1_offset = blk_offset * ASTC_PARTITIONS_CANDIDATES_1PLANE + (partition_count - 2) * ASTC_PARTITION_CANDIDATES_PER_MODE;
        p2_offset = 0;
        find_best_partitionings(ewp->partition_search_limit, xdim, ydim, zdim, partition_count, &pb[blk_offset], &ewb[blk_offset], 1,
            &(best_partitions_1plane[p1_offset]), &(best_partitions_1plane[p1_offset + 1]), &(best_partitions_2planes[p2_offset]));
    }


    //todo: kill it
    int blk_offset = 0;
    int partition_count = 2;
    find_best_partitionings(ewp->partition_search_limit, xdim, ydim, zdim, partition_count, &pb[blk_offset], &ewb[blk_offset], 1,
        &(best_partitions_1plane[0]), &(best_partitions_1plane[1]), &(best_partitions_2planes[0]));
}