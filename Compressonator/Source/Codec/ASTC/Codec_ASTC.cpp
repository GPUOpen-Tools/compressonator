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
//  File Name:   Codec_ASTC.cpp
//  Description: implementation of the CCodec_ASTC class
//
//////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4100)    // Ignore warnings of unreferenced formal parameters
#pragma warning(disable:4101)    // Ignore warnings of unreferenced local variable
#pragma warning(disable:4996)   // This function or variable may be unsafe

#include "Common.h"
#include "ASTC\Codec_ASTC.h"
#include "ASTC\ASTC_library.h"

#include "ASTC\ARM\arm_stdint.h"
#include "ASTC\ARM\astc_codec_internals.h"
#include "process.h"

#ifdef ASTC_USE_OPENCL
#include "OCL_Common.h"
#endif

#ifdef DEBUG_PRINT_DIAGNOSTICS
    int print_diagnostics = 0;
#endif

int progress_counter_divider = 1;

int rgb_force_use_of_hdr = 0;
int alpha_force_use_of_hdr = 0;
int perform_srgb_transform = 0;

#ifdef ASTC_COMPDEBUGGER
#include "CompClient.h"
extern     CompViewerClient g_CompClient;
#endif

//======================================================================================
#define USE_MULTITHREADING  1

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_ASTC::CCodec_ASTC() : CCodec_DXTC(CT_ASTC)
{
    m_LibraryInitialized    = false;
    m_AbortRequested = false;
    m_NumThreads = 8;
    m_NumEncodingThreads = m_NumThreads;
    m_EncodingThreadHandle = NULL;

#ifdef USE_OPENCL
    m_OCLInitialized            = false;
    m_user_selected_plat_index  = -1;
    m_user_selected_dev_index   = -1;
    m_opencl_context            = NULL;
    m_opencl_program            = NULL;
#endif

    batch_size                  = ASTC_MAX_BLOCKS_BATCH_SIZE;

    m_xdim                      = 4;
    m_ydim                      = 4;
    m_zdim                      = 1;
    m_decode_mode               = DECODE_HDR;

}


void CCodec_ASTC::InitializeASTCSettingsForSetBlockSize()
{
    m_target_bitrate = 0;

    int xdim_2d = m_xdim;
    int ydim_2d = m_ydim;
    int xdim_3d = m_xdim;
    int ydim_3d = m_ydim;
    int zdim_3d = m_zdim;

    float log10_texels_2d = 0.0f;
    float log10_texels_3d = 0.0f;

    log10_texels_2d = log((float)(xdim_2d * ydim_2d)) / log(10.0f);
    log10_texels_3d = log((float)(xdim_3d * ydim_3d * zdim_3d)) / log(10.0f);

    int plimit_autoset = -1;
    int plimit_user_specified = -1;
    int plimit_set_by_user = 0;

    float dblimit_autoset_2d = 0.0;
    float dblimit_autoset_3d = 0.0;
    float dblimit_user_specified = 0.0;
    int dblimit_set_by_user = 0;

    float oplimit_autoset = 0.0;
    float oplimit_user_specified = 0.0;
    int oplimit_set_by_user = 0;

    float mincorrel_autoset = 0.0;
    float mincorrel_user_specified = 0.0;
    int mincorrel_set_by_user = 0;

    float bmc_user_specified = 0.0;
    float bmc_autoset = 0.0;
    int bmc_set_by_user = 0;

    int maxiters_user_specified = 0;
    int maxiters_autoset = 0;
    int maxiters_set_by_user = 0;

    // Medium speed setting
    plimit_autoset = 25;
    oplimit_autoset = 1.2f;
    mincorrel_autoset = 0.75f;
    dblimit_autoset_2d = MAX(95 - 35 * log10_texels_2d, 70 - 19 * log10_texels_2d);
    dblimit_autoset_3d = MAX(95 - 35 * log10_texels_3d, 70 - 19 * log10_texels_3d);
    bmc_autoset = 75;
    maxiters_autoset = 2;

    float texel_avg_error_limit_2d = 0.0f;
    float texel_avg_error_limit_3d = 0.0f;

    int partitions_to_test = plimit_set_by_user ? plimit_user_specified : plimit_autoset;
    float dblimit_2d = dblimit_set_by_user ? dblimit_user_specified : dblimit_autoset_2d;
    float dblimit_3d = dblimit_set_by_user ? dblimit_user_specified : dblimit_autoset_3d;
    float oplimit = oplimit_set_by_user ? oplimit_user_specified : oplimit_autoset;
    float mincorrel = mincorrel_set_by_user ? mincorrel_user_specified : mincorrel_autoset;

    m_ewp.rgb_power = 1.0f;
    m_ewp.alpha_power = 1.0f;
    m_ewp.rgb_base_weight = 1.0f;
    m_ewp.alpha_base_weight = 1.0f;
    m_ewp.rgb_mean_weight = 0.0f;
    m_ewp.rgb_stdev_weight = 0.0f;
    m_ewp.alpha_mean_weight = 0.0f;
    m_ewp.alpha_stdev_weight = 0.0f;

    m_ewp.rgb_mean_and_stdev_mixing = 0.0f;
    m_ewp.mean_stdev_radius = 0;
    m_ewp.enable_rgb_scale_with_alpha = 0;
    m_ewp.alpha_radius = 0;

    m_ewp.block_artifact_suppression = 0.0f;
    m_ewp.rgba_weights[0] = 1.0f;
    m_ewp.rgba_weights[1] = 1.0f;
    m_ewp.rgba_weights[2] = 1.0f;
    m_ewp.rgba_weights[3] = 1.0f;
    m_ewp.ra_normal_angular_scale = 0;
    
    int maxiters = maxiters_set_by_user ? maxiters_user_specified : maxiters_autoset;
    m_ewp.max_refinement_iters = maxiters;

    m_ewp.block_mode_cutoff = (bmc_set_by_user ? bmc_user_specified : bmc_autoset) / 100.0f;

    if (rgb_force_use_of_hdr == 0)
    {
        texel_avg_error_limit_2d = pow(0.1f, dblimit_2d * 0.1f) * 65535.0f * 65535.0f;
        texel_avg_error_limit_3d = pow(0.1f, dblimit_3d * 0.1f) * 65535.0f * 65535.0f;
    }
    else
    {
        texel_avg_error_limit_2d = 0.0f;
        texel_avg_error_limit_3d = 0.0f;
    }
    m_ewp.partition_1_to_2_limit = oplimit;
    m_ewp.lowest_correlation_cutoff = mincorrel;

    if (partitions_to_test < 1)
        partitions_to_test = 1;
    else if (partitions_to_test > PARTITION_COUNT)
        partitions_to_test = PARTITION_COUNT;
    m_ewp.partition_search_limit = partitions_to_test;

    // Specifying the error weight of a color component as 0 is not allowed.
    // If weights are 0, then they are instead set to a small positive value.

    float max_color_component_weight = MAX(MAX(m_ewp.rgba_weights[0], m_ewp.rgba_weights[1]), MAX(m_ewp.rgba_weights[2], m_ewp.rgba_weights[3]));
    m_ewp.rgba_weights[0] = MAX(m_ewp.rgba_weights[0], max_color_component_weight / 1000.0f);
    m_ewp.rgba_weights[1] = MAX(m_ewp.rgba_weights[1], max_color_component_weight / 1000.0f);
    m_ewp.rgba_weights[2] = MAX(m_ewp.rgba_weights[2], max_color_component_weight / 1000.0f);
    m_ewp.rgba_weights[3] = MAX(m_ewp.rgba_weights[3], max_color_component_weight / 1000.0f);

    // Allocate arrays for image data and load results.
    m_ewp.texel_avg_error_limit = texel_avg_error_limit_2d;

    expand_block_artifact_suppression(m_xdim, m_ydim, m_zdim, &m_ewp);
}

CCodec_ASTC::~CCodec_ASTC()
{
    if (m_LibraryInitialized)
    {
        if (m_decoder)
        {
            delete m_decoder;
            m_decoder = NULL;
        }

        m_LibraryInitialized = false;
    }
}

void CCodec_ASTC::ReleaseOCL()
{
#ifdef ASTC_USE_OPENCL
#endif
}

CodecError CCodec_ASTC::InitializeOCL()
{
#ifdef ASTC_USE_OPENCL
    if (!m_OCLInitialized)
    {
        m_OCLInitialized = true;
    }
#endif
    return CE_OK;
}

//  notes: Investigate these interfaces to Implement these for alternate methods : IE GPU & HSA based compression!!

CodecError CCodec_ASTC::Compress_Fast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
    return CE_OK;
}

CodecError CCodec_ASTC::Compress_SuperFast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
    return CE_OK;
}

void CCodec_ASTC::find_closest_blockdim_2d(float target_bitrate, int *x, int *y, int consider_illegal)
{
    int blockdims[6] = { 4, 5, 6, 8, 10, 12 };

    float best_error = 1000;
    float aspect_of_best = 1;
    int i, j;

    // Y dimension
    for (i = 0; i < 6; i++)
    {
        // X dimension
        for (j = i; j < 6; j++)
        {
            //              NxN       MxN         8x5               10x5              10x6
            int is_legal = (j==i) || (j==i+1) || (j==3 && j==1) || (j==4 && j==1) || (j==4 && j==2);

            if(consider_illegal || is_legal)
            {
                float bitrate = 128.0f / (blockdims[i] * blockdims[j]);
                float bitrate_error = fabs(bitrate - target_bitrate);
                float aspect = (float)blockdims[j] / blockdims[i];
                if (bitrate_error < best_error || (bitrate_error == best_error && aspect < aspect_of_best))
                {
                    *x = blockdims[j];
                    *y = blockdims[i];
                    best_error = bitrate_error;
                    aspect_of_best = aspect;
                }
            }
        }
    }
}

void CCodec_ASTC::find_closest_blockdim_3d(float target_bitrate, int *x, int *y, int *z, int consider_illegal)
{
    int blockdims[4] = { 3, 4, 5, 6 };

    float best_error = 1000;
    float aspect_of_best = 1;
    int i, j, k;

    for (i = 0; i < 4; i++)    // Z
        for (j = i; j < 4; j++) // Y
            for (k = j; k < 4; k++) // X
            {
                //              NxNxN              MxNxN                  MxMxN
                int is_legal = ((k==j)&&(j==i)) || ((k==j+1)&&(j==i)) || ((k==j)&&(j==i+1));

                if(consider_illegal || is_legal)
                {
                    float bitrate = 128.0f / (blockdims[i] * blockdims[j] * blockdims[k]);
                    float bitrate_error = fabs(bitrate - target_bitrate);
                    float aspect = (float)blockdims[k] / blockdims[j] + (float)blockdims[j] / blockdims[i] + (float)blockdims[k] / blockdims[i];

                    if (bitrate_error < best_error || (bitrate_error == best_error && aspect < aspect_of_best))
                    {
                        *x = blockdims[k];
                        *y = blockdims[j];
                        *z = blockdims[i];
                        best_error = bitrate_error;
                        aspect_of_best = aspect;
                    }
                }
            }
}

bool CCodec_ASTC::SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue)
{
    if (sValue == NULL) return false;

    if(strcmp(pszParamName, "NumThreads") == 0)
    {
        m_NumThreads = (CMP_BYTE) std::stoi(sValue) & 0xFF;
    }
    if(strcmp(pszParamName, "BlockRate") == 0)
    {

        // BlockRate can be a bit value or dimension

        if (strchr(sValue, '.') != NULL)
        {
            m_target_bitrate = static_cast < float >(atof(sValue));
            find_closest_blockdim_2d(m_target_bitrate, &m_xdim, &m_ydim, DEBUG_ALLOW_ILLEGAL_BLOCK_SIZES);
        }
        else
        {
            int dimensions = sscanf(sValue, "%dx%dx", &m_xdim, &m_ydim);
            if (dimensions < 2) return false;
            // Valid block sizes are for 2D support only  (3D is todo later)
            // are in cominations of {4,5,6,8,10,12}
            if ((m_xdim < 4) || (m_xdim > 12)) return false;
            if ((m_ydim < 4) || (m_ydim > 12)) return false;
            if ((m_xdim == 7) || (m_xdim == 9) || (m_xdim == 11)) return false;
            if ((m_ydim == 7) || (m_ydim == 9) || (m_ydim == 11)) return false;
        }
    }
    else
        return CCodec_DXTC::SetParameter(pszParamName, sValue);
    return true;
}

bool CCodec_ASTC::SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue)
{
    if(strcmp(pszParamName, "NumThreads") == 0)
    {
        m_NumThreads = (CMP_BYTE) dwValue;
    }
    else
        return CCodec_DXTC::SetParameter(pszParamName, dwValue);
    return true;
}

bool CCodec_ASTC::SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue)
{
    return CCodec_DXTC::SetParameter(pszParamName, fValue);
}

CodecError CCodec_ASTC::InitializeASTCLibrary()
{
    if (!m_LibraryInitialized)
    {

        //====================== From ASTC main app code ================================
        // initialization routines
        prepare_angular_tables();               // ARM - ASTC code
        build_quantization_mode_table();        // ARM - ASTC code

        // Create single decoder instance
        m_decoder = new ASTCBlockDecoder();
        m_LibraryInitialized = true;
    }
    return CE_OK;
}

struct encode_astc_image_info
{
    int xdim;
    int ydim;
    int zdim;
    const error_weighting_params *ewp;
    uint8_t *buffer;
    int thread_id;
    int threadcount;
    astc_decode_mode decode_mode;
    swizzlepattern swz_encode;
    volatile int *counters;
    volatile int *threads_completed;
    volatile bool *abort_requested;
    const astc_codec_image *input_image;
    Codec_Feedback_Proc pFeedbackProc;
    DWORD_PTR pUser1;
    DWORD_PTR pUser2;
};

#define USE_ARM_CODE

void *encode_astc_image_threadfunc(void *vblk)
{
    // Common ARM and AMD Code
    const encode_astc_image_info *blk = (const encode_astc_image_info *)vblk;
    int xdim = blk->xdim;
    int ydim = blk->ydim;
    int zdim = blk->zdim;
    uint8_t *buffer = blk->buffer;
    int thread_id = blk->thread_id;
    int threadcount = blk->threadcount;
    volatile int *counters = blk->counters;
    swizzlepattern swz_encode = blk->swz_encode;
    volatile int *threads_completed = blk->threads_completed;
    volatile bool *abort_requested = blk->abort_requested;
    const astc_codec_image *input_image = blk->input_image;

#ifdef USE_ARM_CODE
    astc_decode_mode decode_mode = blk->decode_mode;
    const error_weighting_params *ewp = blk->ewp;
#endif

#ifdef USE_ARM_CODE
    imageblock pb;
    int ctr = thread_id;
#else
    // Reserved for new code
#endif

    int pctr = 0;

    // AMD Added code
    Codec_Feedback_Proc pFeedbackProc = blk->pFeedbackProc;


    // Common ARM and AMD Code
    int x, y, z, i;
    int xsize = input_image->xsize;
    int ysize = input_image->ysize;
    int zsize = input_image->zsize;
    int xblocks = (xsize + xdim - 1) / xdim;
    int yblocks = (ysize + ydim - 1) / ydim;
    int zblocks = (zsize + zdim - 1) / zdim;
    int owns_progress_counter = 0;

    // AMD Added Code
    int total_blocks = xblocks * yblocks * zblocks;
    #define NEXT_BLOCK_IN_BATCH(dx, dy, dz) if (++dx >= xblocks){ dx = 0; if (++dy >= yblocks){ dy = 0; dz++; }    }

#ifdef USE_ARM_CODE
    for (z = 0; z < zblocks; z++)
        for (y = 0; y < yblocks; y++)
            for (x = 0; x < xblocks; x++)
            {
                if (ctr == 0)
                {
                    int offset = ((z * yblocks + y) * xblocks + x) * 16;
                    uint8_t *bp = buffer + offset;
                    fetch_imageblock(input_image, &pb, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, swz_encode);
                    symbolic_compressed_block scb;
                    compress_symbolic_block(input_image, decode_mode, xdim, ydim, zdim, ewp, &pb, &scb);
                    physical_compressed_block pcb;
                    pcb = symbolic_to_physical(xdim, ydim, zdim, &scb);
                    *(physical_compressed_block *)bp = pcb;
                    counters[thread_id]++;
                    ctr = threadcount - 1;
                    pctr++;

                    // New AMD Implemented code for feedback
                    // routine to call user FeedbackProc
                    if (pFeedbackProc && (pctr % progress_counter_divider) == 0)
                    {
                        int do_call_feedback_proc = 1;
                        // the current thread has the responsibility for calling user FeedbackProc
                        // if every previous thread has completed. Also, if we have ever received the
                        // responsibility to call user FeedbackProc, we are going to keep it
                        // until the thread is completed.
                        if (!owns_progress_counter)
                        {
                            for (i = thread_id - 1; i >= 0; i--)
                            {
                                if (threads_completed[i] == 0)
                                {
                                    do_call_feedback_proc = 0;
                                    break;
                                }
                            }
                        }
                        if (do_call_feedback_proc)
                        {
                            owns_progress_counter = 1;
                            int summa = 0;
                            for (i = 0; i < threadcount; i++)
                                summa += counters[i];
                            float fProgress = (100.0f * summa) / total_blocks;
                            if (pFeedbackProc(fProgress, blk->pUser1, blk->pUser2))
                            {
                                *abort_requested = true;
                            }
                        }
                    }

                }
                else
                    ctr--;
            }
#else
    // Reserved for new code
#endif

    threads_completed[thread_id] = 1;
    return NULL;
}


CodecError CCodec_ASTC::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
    m_AbortRequested = false;

    CodecError err = InitializeASTCLibrary();
    if (err != CE_OK) return err;

#ifdef ASTC_USE_OPENCL
    err = InitializeOCL();
    if (err != CE_OK) return err;
#endif

#ifdef ASTC_COMPDEBUGGER
    CompViewerClient    g_CompClient;
    if (g_CompClient.connect())
    {
        #ifdef USE_DBGTRACE
            DbgTrace(("-------> Remote Server Connected"));
        #endif
    }
#endif


#ifdef USE_DBGTRACE
    DbgTrace(("IN : BufferType %d ChannelCount %d ChannelDepth %d", bufferIn.GetBufferType(), bufferIn.GetChannelCount(), bufferIn.GetChannelDepth()));
    DbgTrace(("   : Height %d Width %d Pitch %d isFloat %d", bufferIn.GetHeight(), bufferIn.GetWidth(), bufferIn.GetWidth(), bufferIn.IsFloat()));

    DbgTrace(("OUT: BufferType %d ChannelCount %d ChannelDepth %d", bufferOut.GetBufferType(), bufferOut.GetChannelCount(), bufferOut.GetChannelDepth()));
    DbgTrace(("   : Height %d Width %d Pitch %d isFloat %d", bufferOut.GetHeight(), bufferOut.GetWidth(), bufferOut.GetWidth(), bufferOut.IsFloat()));
#endif;

    int xsize = bufferIn.GetWidth();
    int ysize = bufferIn.GetHeight();
    int zsize = 1; //todo: add depth to support 3d textures


    int bitness = 0; //todo: replace astc_codec_image with bufferIn and rewrite fetch_imageblock()
    switch (bufferIn.GetBufferType())
    {
    case CBT_RGBA8888:
    case CBT_RGB888:
    case CBT_RG8:
    case CBT_R8:
        bitness = 8;
        break;
    case CBT_RGBA2101010:
        break;
    case CBT_RGBA16:
    case CBT_RG16:
    case CBT_R16:
        break;
    case CBT_RGBA32:
    case CBT_RG32:
    case CBT_R32:
        break;
    case CBT_RGBA16F:
    case CBT_RG16F:
    case CBT_R16F:
        break;
    case CBT_RGBA32F:
    case CBT_RG32F:
    case CBT_R32F:
        break;
    default:
        break;
    }

    if (bitness != 8)
        assert("Unsupported type of input buffer");

    astc_codec_image *input_image = allocate_image(bitness, xsize, ysize, zsize, 0);

    if (!input_image)
        assert("Unable to allocate image buffer");

    swizzlepattern swz_encode = { 0, 1, 2, 3 };

    InitializeASTCSettingsForSetBlockSize();

    // Loop through the original input image and setup compression threads for each 
    // block to encode  we will load the buffer to pass to ASTC code as 8 bit 4x4 blocks
    // the fill in source image. ASTC code will then use the adaptive sizes for process on the input
    BYTE *pData = bufferIn.GetData();
    int ii = 0;
    for (int y = 0; y < ysize; y++) {
        for (int x = 0; x < xsize; x++) {
            input_image->imagedata8[0][y][4*x      ] = pData[ii];      // Red
            ii++;
            input_image->imagedata8[0][y][4 * x + 1] = pData[ii];      // Green
            ii++;
            input_image->imagedata8[0][y][4 * x + 2] = pData[ii];      // Blue
            ii++;
            input_image->imagedata8[0][y][4 * x + 3] = pData[ii];      // Alpha
            ii++;
        }
    }

    m_NumEncodingThreads = min(m_NumThreads, MAX_ASTC_THREADS);
    if (m_NumEncodingThreads == 0) m_NumEncodingThreads = 1;

    int *counters = new int[m_NumEncodingThreads];
    int *threads_completed = new int[m_NumEncodingThreads];

    // before entering into the multithreadeed routine, ensure that the block size descriptors
    // and the partition table descriptors needed actually exist.
    get_block_size_descriptor(m_xdim, m_ydim, m_zdim);
    get_partition_table(m_xdim, m_ydim, m_zdim, 0);

    encode_astc_image_info *ai = new encode_astc_image_info[m_NumEncodingThreads];
    if (!ai)
    {
        assert("Unable to allocate encoder thread");
    }

    for (int i = 0; i < m_NumEncodingThreads; i++)
    {
        ai[i].xdim = m_xdim;
        ai[i].ydim = m_ydim;
        ai[i].zdim = m_zdim;
        ai[i].buffer = bufferOut.GetData();
        ai[i].ewp = &m_ewp;
        ai[i].counters = counters;
        ai[i].thread_id = i;
        ai[i].threadcount = m_NumEncodingThreads;
        ai[i].decode_mode = m_decode_mode;
        ai[i].swz_encode = swz_encode;
        ai[i].threads_completed = threads_completed;
        ai[i].input_image = input_image;
        counters[i] = 0;
        threads_completed[i] = 0;

        // AMD Added Code
#ifdef USE_AMD_ENCODER
        ai[i].batch_compressor = new ASTCBatchBlockEncoder;
        ai[i].batch_compressor->SetParams(input_image, m_decode_mode, m_xdim, m_ydim, m_zdim, &m_ewp, batch_size);
#endif
        ai[i].pFeedbackProc = pFeedbackProc;
        ai[i].pUser1 = pUser1;
        ai[i].pUser2 = pUser2;
        ai[i].abort_requested = &m_AbortRequested;
    }

    if (m_NumEncodingThreads == 1)
        encode_astc_image_threadfunc(&ai[0]);
    else
    {
        HANDLE *threads = new HANDLE[m_NumEncodingThreads];
        for (int i = 0; i < m_NumEncodingThreads; i++)
            threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)encode_astc_image_threadfunc, &(ai[i]), 0, NULL);

        WaitForMultipleObjects(m_NumEncodingThreads, threads, true, INFINITE);
        delete[]threads;
    }

#ifdef USE_AMD_ENCODER
    for (int i = 0; i < m_NumEncodingThreads; i++)
    {
        delete ai[i].batch_compressor;
    }
#endif

    destroy_image(input_image);

    delete[]ai;
    delete[]counters;
    delete[]threads_completed;

    #ifdef ASTC_COMPDEBUGGER
    g_CompClient.disconnect();
    #endif

    return CE_OK;
}

// notes:
// Slow CPU based decompression : Should look into also using HW based decompression with this interface
// The diminsions for input block sizes and output block sizes needs further work
// we are set to 4x4 by default on this code, question is should it use a dynamic range 
//
CodecError CCodec_ASTC::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2)
{

    CodecError err = InitializeASTCLibrary();

    if (err != CE_OK) return err;
    
    CMP_BYTE  Block_Width  = 4;    
    CMP_BYTE  Block_Height = 4;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + (Block_Width - 1)) / Block_Width);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight()+ (Block_Height- 1)) / Block_Height);
    const CMP_DWORD dwBlocksXY = dwBlocksX*dwBlocksY;

    //
    //  notes: All of the block sizes must be variable and not fixed to 4x4
    //
    for(CMP_DWORD j = 0; j < dwBlocksY; j++)        // row = height
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)    // Col = width
        {
            union FBLOCKS
            {
                float decodedBlock[144][4];            // max 12x12 block size
                float destBlock[576];                // max 12x12x4
            } DecData;

            union BBLOCKS
            {
                CMP_DWORD    compressedBlock[4];
                BYTE            out[16];
                BYTE            in[16];
            } CompData;

            CMP_BYTE destBlock[576];    // Max decompressed data size 12x12x4 (bitness = 8 bits per channel!!)
            
            bufferIn.ReadBlock(i*Block_Width, j*Block_Height, CompData.compressedBlock, 4);

            // Encode to the appropriate location in the compressed image
            m_decoder->DecompressBlock(DecData.decodedBlock,CompData.in);

            // Create the block for decoding
            // napate; notes: We need to figure out proper dimensions or output buffer!!
            int srcIndex = 0;
            for(int row=0; row < Block_Width; row++)
            {
                for(int col=0; col < Block_Height; col++)
                {
                    destBlock[srcIndex]   = (CMP_BYTE)DecData.decodedBlock[row*Block_Width+col][BC_COMP_RED];         
                    destBlock[srcIndex+1] = (CMP_BYTE)DecData.decodedBlock[row*Block_Width+col][BC_COMP_GREEN];     
                    destBlock[srcIndex+2] = (CMP_BYTE)DecData.decodedBlock[row*Block_Width+col][BC_COMP_BLUE];     
                    destBlock[srcIndex+3] = (CMP_BYTE)DecData.decodedBlock[row*Block_Width+col][BC_COMP_ALPHA];     
                    srcIndex+=4;    // Go to next target pixel (RGBA data)
                }
            }

            bufferOut.WriteBlockRGBA(i*Block_Width, j*Block_Height, Block_Width, Block_Height, destBlock);

        }

        if (pFeedbackProc)
        {
            float fProgress = 100.f * (j * dwBlocksX) / dwBlocksXY;
            if (pFeedbackProc(fProgress, pUser1, pUser2))
            {
                return CE_Aborted;
            }
        }
    }
    return CE_OK;
}

