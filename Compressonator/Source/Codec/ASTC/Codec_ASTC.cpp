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

struct ASTCEncodeThreadParam
{
    ASTCBlockEncoder   *encoder;

    // Encoder params
    astc_codec_image *input_image;
    uint8_t *bp;
    int xdim;
    int ydim;
    int zdim;
    int x;
    int y;
    int z;
    astc_decode_mode decode_mode;
    const error_weighting_params * ewp;

    volatile BOOL      run;
    volatile BOOL      exit;
};

static ASTCEncodeThreadParam *g_EncodeParameterStorage = NULL;

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_ASTC::CCodec_ASTC() : CCodec_DXTC(CT_ASTC)
{
    m_LibraryInitialized    = false;
    m_AbortRequested        = false;
    m_NumThreads            = 8;
    m_NumEncodingThreads    = m_NumThreads;
    m_EncodingThreadHandle  = NULL;

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
    m_decoder                   = NULL;
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

    // Codec Speed Setting Defaults based on Quality Settings
    //mincorrel_autoset = m_Quality;
    //bmc_autoset = (int)(m_Quality * 100);

    if (m_Quality < 0.2)
    {
        // Very Fast
        plimit_autoset = 1;
        oplimit_autoset = 1.0;
        dblimit_autoset_2d = MAX(70 - 35 * log10_texels_2d, 53 - 19 * log10_texels_2d);
        dblimit_autoset_3d = MAX(70 - 35 * log10_texels_3d, 53 - 19 * log10_texels_3d);
        bmc_autoset = 5;
        mincorrel_autoset = m_Quality;
        maxiters_autoset = 1;
    }
    else
    if (m_Quality < 0.5)
    {
        // Medium speed setting
        plimit_autoset = 2;
        oplimit_autoset = 1.0;
        mincorrel_autoset = m_Quality;
        dblimit_autoset_2d = MAX(85 - 35 * log10_texels_2d, 63 - 19 * log10_texels_2d);
        dblimit_autoset_2d = MAX(85 - 35 * log10_texels_3d, 63 - 19 * log10_texels_3d);
        bmc_autoset = 15;
        maxiters_autoset = 1;
    }
    else
    if (m_Quality < 0.8)
    {
        // Thorough
        plimit_autoset = 15;
        oplimit_autoset = 1.0;
        mincorrel_autoset = 0.5;
        dblimit_autoset_2d = MAX(85 - 35 * log10_texels_2d, 63 - 19 * log10_texels_2d);
        dblimit_autoset_2d = MAX(85 - 35 * log10_texels_3d, 63 - 19 * log10_texels_3d);
        bmc_autoset = 25;
        maxiters_autoset = 1;
    }
    else
        {
            // Exhaustive
            plimit_autoset = 10;
            oplimit_autoset = 1.0;
            mincorrel_autoset = 0.5;
            dblimit_autoset_2d = MAX(85 - 35 * log10_texels_2d, 63 - 19 * log10_texels_2d);
            dblimit_autoset_2d = MAX(85 - 35 * log10_texels_3d, 63 - 19 * log10_texels_3d);
            bmc_autoset = 50;
            maxiters_autoset = 1;
        }


//        else
//            if (m_Quality < 0.6)
//            {
//                // Medium speed setting
//                plimit_autoset = 25;
//                oplimit_autoset = 1.2f;
//                mincorrel_autoset = 0.75f;
//                dblimit_autoset_2d = MAX(95 - 35 * log10_texels_2d, 70 - 19 * log10_texels_2d);
//                dblimit_autoset_3d = MAX(95 - 35 * log10_texels_3d, 70 - 19 * log10_texels_3d);
//                bmc_autoset = 75;
//                maxiters_autoset = 2;
//            }
//            else
//                if (m_Quality < 0.8)
//                {
//                    // Thorough
//                    plimit_autoset = 100;
//                    oplimit_autoset = 2.5f;
//                    mincorrel_autoset = 0.95f;
//                    dblimit_autoset_2d = MAX(105 - 35 * log10_texels_2d, 77 - 19 * log10_texels_2d);
//                    dblimit_autoset_3d = MAX(105 - 35 * log10_texels_3d, 77 - 19 * log10_texels_3d);
//                    bmc_autoset = 95;
//                    maxiters_autoset = 4;
//                }
//                else
//                {
//                    // Exhaustive
//                    plimit_autoset = PARTITION_COUNT;
//                    oplimit_autoset = 1000.0f;
//                    mincorrel_autoset = 0.99f;
//                    dblimit_autoset_2d = 999.0f;
//                    dblimit_autoset_3d = 999.0f;
//                    bmc_autoset = 100;
//                    maxiters_autoset = 4;
//                }

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

        if (m_Use_MultiThreading)
        {
            // Tell all the live threads that they can exit when they have finished any current work
            for (int i = 0; i < m_LiveThreads; i++)
            {
                // If a thread is in the running state then we need to wait for it to finish
                // any queued work from the producer before we can tell it to exit.
                //
                // If we don't wait then there is a race condition here where we have
                // told the thread to run but it hasn't yet been scheduled - if we set
                // the exit flag before it runs then its block will not be processed.
#pragma warning(push)
#pragma warning(disable:4127) //warning C4127: conditional expression is constant
                while (1)
                {
                    if (g_EncodeParameterStorage[i].run != TRUE)
                    {
                        break;
                    }
                }
#pragma warning(pop)
                // Signal to the thread that it can exit
                g_EncodeParameterStorage[i].exit = TRUE;
            }

            // Now wait for all threads to have exited
            if (m_LiveThreads > 0)
            {
                WaitForMultipleObjects(m_LiveThreads,
                    m_EncodingThreadHandle,
                    true,
                    INFINITE);
            }

        } // MultiThreading

        for (int i = 0; i < m_LiveThreads; i++)
        {
            if (m_EncodingThreadHandle[i])
            {
                CloseHandle(m_EncodingThreadHandle[i]);
            }
            m_EncodingThreadHandle[i] = 0;
        }

        delete[] m_EncodingThreadHandle;
        m_EncodingThreadHandle = NULL;

        delete[] g_EncodeParameterStorage;
        g_EncodeParameterStorage = NULL;


        for (int i = 0; i < m_NumEncodingThreads; i++)
        {
            if (m_encoder[i])
            {
                delete m_encoder[i];
                m_encoder[i] = NULL;
            }
        }


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

void CCodec_ASTC::find_closest_blockxy_2d(int *x, int *y, int consider_illegal)
{
    int blockdims[6] = { 4, 5, 6, 8, 10, 12 };

    bool exists_x = std::find(std::begin(blockdims), std::end(blockdims), (*x)) != std::end(blockdims);
    bool exists_y = std::find(std::begin(blockdims), std::end(blockdims), (*y)) != std::end(blockdims);

    if (exists_x && exists_y)
    {
        if ((*x) < (*y))
        {
            int temp = *x;
            *x = *y;
            *y = temp;
        }
        float bitrateF = float(128.0f / ((*x)*(*y)));
        find_closest_blockdim_2d(bitrateF, x, y, 0);
    }
    else
    {
        float bitrateF = float(128.0f / ((*x)*(*y)));
        find_closest_blockdim_2d(bitrateF, x, y, 0);
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
            find_closest_blockxy_2d(&m_xdim, &m_ydim, DEBUG_ALLOW_ILLEGAL_BLOCK_SIZES);

            // Valid block sizes are for 2D support only  (3D is todo later)
            // are in cominations of {4,5,6,8,10,12}
            if ((m_xdim < 4) || (m_xdim > 12)) return false;
            if ((m_ydim < 4) || (m_ydim > 12)) return false;
            if ((m_xdim == 7) || (m_xdim == 9) || (m_xdim == 11)) return false;
            if ((m_ydim == 7) || (m_ydim == 9) || (m_ydim == 11)) return false;
        }
    }
    if (strcmp(pszParamName, "Quality") == 0)
    {
        m_Quality = std::stof(sValue);
        if ((m_Quality < 0) || (m_Quality > 1.0))
        {
            return false;
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
    if (strcmp(pszParamName, "Quality") == 0)
        m_Quality = fValue;
    else
    return CCodec_DXTC::SetParameter(pszParamName, fValue);
    return true;
}


//
// Thread procedure for encoding a block
//
// The thread stays alive, and expects blocks to be pushed to it by a producer
// process that signals it when new work is available. When the producer is finished
// it should set the exit flag in the parameters to allow the tread to quit
//

unsigned int    _stdcall ASTCThreadProcEncode(void* param)
{
    ASTCEncodeThreadParam *tp = (ASTCEncodeThreadParam*)param;

    while (tp->exit == FALSE)
    {
        if (tp->run == TRUE)
        {
            tp->encoder->CompressBlock(
                tp->input_image,
                tp->bp,
                tp->xdim,
                tp->ydim,
                tp->zdim,
                tp->x,
                tp->y,
                tp->z,
                tp->decode_mode,
                tp->ewp
            );
            tp->run = FALSE;
        }
        Sleep(0);
    }

    return 0;
}

CodecError CCodec_ASTC::InitializeASTCLibrary()
{
    if (!m_LibraryInitialized)
    {

        //====================== From ASTC main app code ================================
        // initialization routines
        prepare_angular_tables();               // ARM - ASTC code
        build_quantization_mode_table();        // ARM - ASTC code

        //====================== Threads
        for (DWORD i = 0; i < MAX_ASTC_THREADS; i++)
        {
            m_encoder[i] = NULL;
        }

        // Create threaded encoder instances
        m_LiveThreads = 0;
        m_LastThread = 0;
        m_NumEncodingThreads = min(m_NumThreads, MAX_ASTC_THREADS);
        if (m_NumEncodingThreads == 0) m_NumEncodingThreads = 1;
        m_Use_MultiThreading = m_NumEncodingThreads > 1;

        g_EncodeParameterStorage = new ASTCEncodeThreadParam[m_NumEncodingThreads];
        if (!g_EncodeParameterStorage)
        {
            return CE_Unknown;
        }

        m_EncodingThreadHandle = new HANDLE[m_NumEncodingThreads];
        if (!m_EncodingThreadHandle)
        {
            delete[] g_EncodeParameterStorage;
            g_EncodeParameterStorage = NULL;

            return CE_Unknown;
        }

        DWORD   i;

        for (i = 0; i < m_NumEncodingThreads; i++)
        {
            // Create single encoder instance
            m_encoder[i] = new ASTCBlockEncoder();


            // Cleanup if problem!
            if (!m_encoder[i])
            {

                delete[] g_EncodeParameterStorage;
                g_EncodeParameterStorage = NULL;

                delete[] m_EncodingThreadHandle;
                m_EncodingThreadHandle = NULL;

                for (DWORD j = 0; j<i; j++)
                {
                    delete m_encoder[j];
                    m_encoder[j] = NULL;
                }

                return CE_Unknown;
            }

#ifdef USE_DBGTRACE
            DbgTrace(("Encoder[%d]:ModeMask %X, Quality %f\n", i, m_ModeMask, m_Quality));
#endif

        }

        // Create the encoding threads in the suspended state
        for (i = 0; i<m_NumEncodingThreads; i++)
        {
            m_EncodingThreadHandle[i] = (HANDLE)_beginthreadex(NULL,
                0,
                ASTCThreadProcEncode,
                (void*)&g_EncodeParameterStorage[i],
                CREATE_SUSPENDED,
                NULL);
            if (m_EncodingThreadHandle[i])
            {
                g_EncodeParameterStorage[i].encoder = m_encoder[i];
                // Inform the thread that at the moment it doesn't have any work to do
                // but that it should wait for some and not exit
                g_EncodeParameterStorage[i].run = FALSE;
                g_EncodeParameterStorage[i].exit = FALSE;
                // Start the thread and have it wait for work
                ResumeThread(m_EncodingThreadHandle[i]);
                m_LiveThreads++;
            }
        }

        // Create single decoder instance
        m_decoder = new ASTCBlockDecoder();

        if (!m_decoder)
        {
            for (DWORD j = 0; j<m_NumEncodingThreads; j++)
            {
                delete m_encoder[j];
                m_encoder[j] = NULL;
            }
            return CE_Unknown;
        }

        m_LibraryInitialized = true;
    }
    return CE_OK;
}

CodecError CCodec_ASTC::EncodeASTCBlock(
    astc_codec_image *input_image,
    uint8_t *bp,
    int xdim,
    int ydim,
    int zdim,
    int x,
    int y,
    int z,
    astc_decode_mode decode_mode,
    const error_weighting_params * ewp)
{
    if (m_Use_MultiThreading)
    {
        WORD   threadIndex;

        // Loop and look for an available thread
        BOOL found = FALSE;
        threadIndex = m_LastThread;
        while (found == FALSE)
        {

            if (g_EncodeParameterStorage == NULL)
                return CE_Unknown;

            if (g_EncodeParameterStorage[threadIndex].run == FALSE)
            {
                found = TRUE;
                break;
            }

            // Increment and wrap the thread index
            threadIndex++;
            if (threadIndex == m_LiveThreads)
            {
                threadIndex = 0;
            }
        }

        m_LastThread = threadIndex;

        g_EncodeParameterStorage[threadIndex].input_image = input_image;
        g_EncodeParameterStorage[threadIndex].bp = bp;
        g_EncodeParameterStorage[threadIndex].xdim = xdim;
        g_EncodeParameterStorage[threadIndex].ydim = ydim;
        g_EncodeParameterStorage[threadIndex].zdim = zdim;
        g_EncodeParameterStorage[threadIndex].x = x;
        g_EncodeParameterStorage[threadIndex].y = y;
        g_EncodeParameterStorage[threadIndex].z = z;
        g_EncodeParameterStorage[threadIndex].decode_mode = decode_mode;
        g_EncodeParameterStorage[threadIndex].ewp = ewp;

        // Tell the thread to start working
        g_EncodeParameterStorage[threadIndex].run = TRUE;
    }
    else
    {
        m_encoder[0]->CompressBlock(
            input_image,
            bp,
            xdim,
            ydim,
            zdim,
            x,
            y,
            z,
            decode_mode,
            ewp);
    }
    return CE_OK;
}


CodecError CCodec_ASTC::FinishASTCEncoding(void)
{
    if (!m_LibraryInitialized)
    {
        return CE_Unknown;
    }

    if (!g_EncodeParameterStorage)
    {
        return CE_Unknown;
    }

    if (m_Use_MultiThreading)
    {
        // Wait for all the live threads to finish any current work
        for (DWORD i = 0; i < m_LiveThreads; i++)
        {
            // If a thread is in the running state then we need to wait for it to finish
            // its work from the producer
            while (g_EncodeParameterStorage[i].run == TRUE)
            {
                Sleep(1);
            }
        }
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
    const astc_codec_image *input_image;
    Codec_Feedback_Proc pFeedbackProc;
    DWORD_PTR pUser1;
    DWORD_PTR pUser2;
};

#define USE_ARM_CODE

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
    case CBT_BGRA8888:
    case CBT_ARGB8888:
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

// Common ARM and AMD Code
    CodecError result = CE_OK;
    int xdim = m_xdim;
    int ydim = m_ydim;
    int zdim = m_zdim;
    uint8_t *bufferOutput = bufferOut.GetData();

#ifdef USE_ARM_CODE
    astc_decode_mode decode_mode = m_decode_mode;
    const error_weighting_params *ewp = &m_ewp;
#endif

    // Common ARM and Compressonator Code
    int x, y, z, i;
    int xblocks = (xsize + xdim - 1) / xdim;
    int yblocks = (ysize + ydim - 1) / ydim;
    int zblocks = (zsize + zdim - 1) / zdim;
    float TotalBlocks = (float) (yblocks * xblocks);

    for (z = 0; z < zblocks; z++)
    {
        for (y = 0; y < yblocks; y++)
        {
            for (x = 0; x < xblocks; x++)
            {
                int offset = ((z * yblocks + y) * xblocks + x) * 16;
                uint8_t *bp = bufferOutput + offset;
                EncodeASTCBlock(input_image, bp, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, decode_mode, ewp);
            }

            if (pFeedbackProc)
            {
                float fProgress = 100.f * ((float)(y * yblocks) / TotalBlocks);
                if (pFeedbackProc(fProgress, pUser1, pUser2))
                {
                    result = CE_Aborted;
                    break;
                }
            }

        }
    }

    CodecError EncodeResult = FinishASTCEncoding();

    if (result != CE_Aborted)
        result = EncodeResult;

    destroy_image(input_image);

#ifdef ASTC_COMPDEBUGGER
    g_CompClient.disconnect();
#endif

    return result;
}

// notes:
// Slow CPU based decompression : Should look into also using HW based decompression with this interface
//
CodecError CCodec_ASTC::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
    CodecError err = InitializeASTCLibrary();
    if (err != CE_OK) return err;

    // Our Compressed data Blocks are always 128 bit long (4x4 blocks)
    const CMP_DWORD imageWidth  = bufferIn.GetWidth();
    const CMP_DWORD imageHeight = bufferIn.GetHeight();
    const CMP_DWORD imageDepth  = 1;
    const BYTE      bitness     = 8;

    const CMP_DWORD CompBlockX  = bufferIn.GetBlockWidth();
    const CMP_DWORD CompBlockY  = bufferIn.GetBlockHeight();
    CMP_BYTE  Block_Width       = bufferIn.GetBlockWidth();
    CMP_BYTE  Block_Height      = bufferIn.GetBlockHeight();

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + (CompBlockX - 1)) / CompBlockX);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight()+ (CompBlockY - 1)) / CompBlockY);
    const CMP_DWORD dwBlocksZ = 1;
    const CMP_DWORD dwBufferInDepth = 1;

    // Override the current input buffer Pitch size  (Since it will be set according to the Compressed Block Sizes
    // and not to the Compressed Codec data which is for ASTC 16 Bytes per block x Number of blocks per row
    bufferIn.SetPitch(16 * dwBlocksX);

    // Output data size Pitch
    CMP_DWORD  dwPitch = bufferOut.GetPitch();

    // Output Buffer
    BYTE *pDataOut      = bufferOut.GetData();

    const CMP_DWORD dwBlocksXY = dwBlocksX*dwBlocksY;

    for(CMP_DWORD cmpRowY = 0; cmpRowY < dwBlocksY; cmpRowY++)        // Compressed images row = height
    {
        for(CMP_DWORD cmpColX = 0; cmpColX < dwBlocksX; cmpColX++)    // Compressed images Col = width
        {
            union FBLOCKS
            {
                float decodedBlock[144][4];            // max 12x12 block size
                float destBlock[576];                  // max 12x12x4
            } DecData;
    
            union BBLOCKS
            {
                CMP_DWORD       compressedBlock[4];
                BYTE            out[16];
                BYTE            in[16];
            } CompData;

            bufferIn.ReadBlock(cmpColX*4, cmpRowY*4, CompData.compressedBlock, 4);

            // Encode to the appropriate location in the compressed image
            m_decoder->DecompressBlock(Block_Width, Block_Height, bitness, DecData.decodedBlock,CompData.in);
            
            // Now that we have a decoded block lets copy that data over to the target image buffer
            CMP_DWORD outCol = cmpColX*Block_Width;
            CMP_DWORD outRow = cmpRowY*Block_Height;
            CMP_DWORD outImgRow = outRow;
            CMP_DWORD outImgCol = outCol;

            for (int row = 0; row < Block_Height; row++)
            {
                CMP_DWORD  nextRowCol  = (outRow+row)*dwPitch + (outCol * 4);
                CMP_BYTE*  pData       = (CMP_BYTE*)(pDataOut + nextRowCol);
                if ((outImgRow + row) < imageHeight)
                {
                    outImgCol = outCol;
                    for (int col = 0; col < Block_Width; col++)
                    {
                        int w = outImgCol + col;
                        if (w < imageWidth)
                        {
                            int index = row*Block_Width + col;
                            *pData++ = (CMP_BYTE)DecData.decodedBlock[index][BC_COMP_RED];
                            *pData++ = (CMP_BYTE)DecData.decodedBlock[index][BC_COMP_GREEN];
                            *pData++ = (CMP_BYTE)DecData.decodedBlock[index][BC_COMP_BLUE];
                            *pData++ = (CMP_BYTE)DecData.decodedBlock[index][BC_COMP_ALPHA];
                        }
                        else break;
                    }
                }
            }
        }

        if (pFeedbackProc)
        {
            float fProgress = 100.f * (cmpRowY * dwBlocksX) / dwBlocksXY;
            if (pFeedbackProc(fProgress, pUser1, pUser2))
            {
                return CE_Aborted;
            }
        }
    }

    return CE_OK;
}

