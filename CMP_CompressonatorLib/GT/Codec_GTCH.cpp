//===============================================================================
// Copyright (c) 2014-2018  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   Codec_GT.cpp
//  Description: implementation of the CCodec_GT class
//
//////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4100)    // Ignore warnings of unreferenced formal parameters

#ifdef _WIN32
#include "Common.h"
#include "Codec_GTCH.h"
#include <process.h>

#ifdef GT_COMPDEBUGGER
#include "CompClient.h"
#endif

//======================================================================================
//#define USE_PRINTF
//#define USE_NOMULTITHREADING 

#ifdef USE_FILEIO
    #include <stdio.h>
    FILE * gt_File = NULL;
    int gt_blockcount = 0;
    int gt_total_MSE = 0;
#endif

//
// Thread procedure for encoding a block
//
// The thread stays alive, and expects blocks to be pushed to it by a producer
// process that signals it when new work is available. When the producer is finished
// it should set the exit flag in the parameters to allow the tread to quit
//

unsigned int    _stdcall GTCHThreadProcEncode(void* param)
{
    GTCHEncodeThreadParam *tp = (GTCHEncodeThreadParam*)param;

    while(tp->exit == FALSE)
    {
        if(tp->run == TRUE)
        {
            tp->encoder->CompressBlock(tp->in, tp->out);
            tp->run = false;
        }
        using namespace std::chrono;

        std::this_thread::sleep_for(0ms);
    }

    return 0;
}


static GTCHEncodeThreadParam *g_EncodeParameterStorage = NULL;


//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_GTCH::CCodec_GTCH() : CCodec_DXTC(CT_GTC)
{
    m_LibraryInitialized   = false;

    m_Use_MultiThreading = true;
    m_NumThreads = 8;
    m_NumEncodingThreads   = m_NumThreads;
    m_EncodingThreadHandle = NULL;
    m_LiveThreads          = 0;
    m_LastThread           = 0;

    m_quality = 0.05f;
    m_performance = 0.0f;
    m_errorThreshold = 0.0f;

}


bool CCodec_GTCH::SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue)
{
    if (sValue == NULL) return false;

    if(strcmp(pszParamName, "NumThreads") == 0)
    {
        m_NumThreads = (CMP_BYTE) std::stoi(sValue) & 0xFF;
        m_Use_MultiThreading = m_NumThreads > 1;
    }
    else
        if (strcmp(pszParamName, "Quality") == 0)
        {
            m_quality = std::stof(sValue);
            if ((m_quality < 0) || (m_quality > 1.0))
            {
                return false;
            }
        }
        else
        return CCodec_DXTC::SetParameter(pszParamName, sValue);
    return true;
}



bool CCodec_GTCH::SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue)
{
    if(strcmp(pszParamName, "NumThreads") == 0)
    {
        m_NumThreads = (CMP_BYTE) dwValue;
        m_Use_MultiThreading = m_NumThreads > 1;
    }
    else
        return CCodec_DXTC::SetParameter(pszParamName, dwValue);
    return true;
}

bool CCodec_GTCH::SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue)
{
    if (strcmp(pszParamName, "Quality") == 0)
        m_quality = fValue;
    else
        return CCodec_DXTC::SetParameter(pszParamName, fValue);

    return true;
}


CCodec_GTCH::~CCodec_GTCH()
{
    if (m_LibraryInitialized)
    {

        if (m_Use_MultiThreading)
        {
            // Tell all the live threads that they can exit when they have finished any current work
            for(int i=0; i < m_LiveThreads; i++)
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
                    if (m_EncodeParameterStorage[i].run != TRUE)
                    {
                        break;
                    }
                }
#pragma warning(pop)
                // Signal to the thread that it can exit
                m_EncodeParameterStorage[i].exit = TRUE;
            }

            // Now wait for all threads to have exited
            if (m_LiveThreads > 0)
            {
                for (CMP_DWORD dwThread = 0; dwThread < m_LiveThreads; dwThread++)
                {
                    std::thread& curThread = m_EncodingThreadHandle[dwThread];

                    curThread.join();
                }
            }

            for (unsigned int i = 0; i < m_LiveThreads; i++)
            {
                std::thread& curThread = m_EncodingThreadHandle[i];

                curThread = std::thread();
            }


        } // MultiThreading

        m_EncodingThreadHandle = NULL;

        if (m_EncodeParameterStorage)
            delete[] m_EncodeParameterStorage;
        m_EncodeParameterStorage = NULL;


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



CodecError CCodec_GTCH::InitializeGTCHLibrary()
{
    if (!m_LibraryInitialized)
    {

        for(CMP_DWORD i=0; i < MAX_GT_THREADS; i++)
        {
            m_encoder[i] = NULL;
        }

        // Create threaded encoder instances
        m_LiveThreads = 0;
        m_LastThread  = 0;
        m_NumEncodingThreads = min(m_NumThreads, MAX_GT_THREADS);
        if (m_NumEncodingThreads == 0) m_NumEncodingThreads = 1; 
        m_Use_MultiThreading = m_NumEncodingThreads > 1;

        m_EncodeParameterStorage = new GTCHEncodeThreadParam[m_NumEncodingThreads];
        if (!m_EncodeParameterStorage)
        {
            return CE_Unknown;
        }

        m_EncodingThreadHandle = new std::thread[m_NumEncodingThreads];
        if (!m_EncodingThreadHandle)
        {
            delete[] m_EncodeParameterStorage;
            m_EncodeParameterStorage = NULL;

            return CE_Unknown;
        }

        CMP_DWORD   i;

        for(i=0; i < m_NumEncodingThreads; i++)
        {
            // Create single encoder instance
            m_encoder[i] = new GTCHBlockEncoder(m_quality, m_performance, m_errorThreshold);

            
            // Cleanup if problem!
            if (!m_encoder[i])
            {

                delete[] m_EncodeParameterStorage;
                m_EncodeParameterStorage = NULL;

                delete[] m_EncodingThreadHandle;
                m_EncodingThreadHandle = NULL;

                for (CMP_DWORD j = 0; j<i; j++)
                {
                    delete m_encoder[j];
                    m_encoder[j] = NULL;
                }

                return CE_Unknown;
            }

            #ifdef USE_DBGTRACE
            DbgTrace(("Encoder[%d]:ModeMask %X, Quality %f\n",i,m_ModeMask,m_Quality));
            #endif

        }

        // Create the encoding threads in the suspended state
        for (i = 0; i<m_NumEncodingThreads; i++)
        {
            // Initialize thread parameters.
            m_EncodeParameterStorage[i].encoder = m_encoder[i];
            // Inform the thread that at the moment it doesn't have any work to do
            // but that it should wait for some and not exit
            m_EncodeParameterStorage[i].run = FALSE;
            m_EncodeParameterStorage[i].exit = FALSE;

            m_EncodingThreadHandle[i] = std::thread(
                GTCHThreadProcEncode,
                (void*)&m_EncodeParameterStorage[i]
            );
            m_LiveThreads++;
        }


        // Create single decoder instance
        m_decoder = new GTCHBlockDecoder();
        if(!m_decoder)
        {
            for (CMP_DWORD j = 0; j<m_NumEncodingThreads; j++)
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


CodecError CCodec_GTCH::EncodeGTCHBlock(
    CMP_FLOAT  in[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG], 
    CMP_BYTE    *out)
{

#ifdef USE_NOMULTITHREADING
    m_Use_MultiThreading = false;
#endif

if (m_Use_MultiThreading)
{
    CMP_WORD   threadIndex;

    // Loop and look for an available thread
    CMP_BOOL found = FALSE;
    threadIndex = m_LastThread;
    while (found == FALSE)
    {

        if (m_EncodeParameterStorage == NULL)
            return CE_Unknown;

        if(m_EncodeParameterStorage[threadIndex].run == FALSE)
        {
            found = TRUE;
            break;
        }

        // Increment and wrap the thread index
        threadIndex++;
        if(threadIndex == m_LiveThreads)
        {
            threadIndex = 0;
        }
    }

    m_LastThread = threadIndex;

    // Copy the input data into the thread storage
    memcpy(m_EncodeParameterStorage[threadIndex].in,
           in,
           16 * 4);

    // Set the output pointer for the thread to the provided location
    m_EncodeParameterStorage[threadIndex].out = out;

    // Tell the thread to start working
    m_EncodeParameterStorage[threadIndex].run = TRUE;
}
else 
{
        // Copy the input data into the thread storage
        memcpy(m_EncodeParameterStorage[0].in, in, MAX_SUBSET_SIZE * MAX_DIMENSION_BIG * sizeof(CMP_FLOAT));
        // Set the output pointer for the thread to write
        m_EncodeParameterStorage[0].out = out;
        m_encoder[0]->CompressBlock(m_EncodeParameterStorage[0].in,m_EncodeParameterStorage[0].out);
}
    return CE_OK;
}

CodecError CCodec_GTCH::FinishGTCHEncoding(void)
{
    if(!m_LibraryInitialized)
    {
        return CE_Unknown;
    }

    if (!m_EncodeParameterStorage)
    {
        return CE_Unknown;
    }

if (m_Use_MultiThreading)
{
    using namespace std::chrono;

    // Wait for all the live threads to finish any current work
    for(CMP_DWORD i=0; i < m_LiveThreads; i++)
    {

        // If a thread is in the running state then we need to wait for it to finish
        // its work from the producer
        while (m_EncodeParameterStorage[i].run == TRUE)
        {
            std::this_thread::sleep_for(1ms);
        }
    }
}
return CE_OK;
}

CodecError CCodec_GTCH::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    assert(bufferIn.GetWidth()    == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    CodecError err = InitializeGTCHLibrary();
    if (err != CE_OK) return err;

#ifdef GT_COMPDEBUGGER
    CompViewerClient    g_CompClient;
    if (g_CompClient.connect())
    {
        #ifdef USE_DBGTRACE
        DbgTrace(("-------> Remote Server Connected"));
        #endif
    }
#endif

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);
    const CMP_DWORD dwBlocksXY = dwBlocksX*dwBlocksY;


    #ifdef USE_DBGTRACE
    DbgTrace(("IN : BufferType %d ChannelCount %d ChannelDepth %d",bufferIn.GetBufferType(),bufferIn.GetChannelCount(),bufferIn.GetChannelDepth()));
    DbgTrace(("   : Height %d Width %d Pitch %d isFloat %d",bufferIn.GetHeight(),bufferIn.GetWidth(),bufferIn.GetWidth(),bufferIn.IsFloat()));

    DbgTrace(("OUT: BufferType %d ChannelCount %d ChannelDepth %d",bufferOut.GetBufferType(),bufferOut.GetChannelCount(),bufferOut.GetChannelDepth()));
    DbgTrace(("   : Height %d Width %d Pitch %d isFloat %d",bufferOut.GetHeight(),bufferOut.GetWidth(),bufferOut.GetWidth(),bufferOut.IsFloat()));
    #endif

    char            row,col,srcIndex;

    CMP_BYTE    *pOutBuffer;
    pOutBuffer    = bufferOut.GetData();

    CMP_BYTE*    pInBuffer;
    pInBuffer    =  bufferIn.GetData();

#ifdef USE_FILEIO
    gt_File = fopen("gt_report.txt","w");
    gt_blockcount = 0;
    gt_total_MSE  = 0;
#endif
#ifdef USE_PRINTF
    printf("\n----------------\n");
#endif

    CMP_DWORD block = 0;

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {

        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {

#ifdef USE_PRINTF
            printf("\nBlock:x=%3d y=%3d  ",i, j);
#endif

#ifdef USE_FILEIO
            if (gt_File)
                fprintf(gt_File, "\nBlock:%4d x=%3d y=%3d\n", gt_blockcount++, i, j);
#endif

            CMP_FLOAT blockToEncode[BLOCK_SIZE_4X4][CHANNEL_SIZE_ARGB];
            CMP_FLOAT srcBlock[BLOCK_SIZE_4X4X4];


            memset(srcBlock,0,sizeof(srcBlock));
            bufferIn.ReadBlockRGBA(i*4, j*4, 4, 4, srcBlock);

            #ifdef GT_COMPDEBUGGER
            g_CompClient.SendData(1,sizeof(srcBlock),srcBlock);
            #endif

            // Create the block for encoding
            srcIndex = 0;
            for(row=0; row < BLOCK_SIZE_4; row++)
            {
                for(col=0; col < BLOCK_SIZE_4; col++)
                {
                    blockToEncode[row*BLOCK_SIZE_4+col][BC_COMP_RED]        = srcBlock[srcIndex];
                    blockToEncode[row*BLOCK_SIZE_4+col][BC_COMP_GREEN]      = srcBlock[srcIndex+1];
                    blockToEncode[row*BLOCK_SIZE_4+col][BC_COMP_BLUE]       = srcBlock[srcIndex+2];
                    blockToEncode[row*BLOCK_SIZE_4+col][BC_COMP_ALPHA]      = srcBlock[srcIndex+3];
                    srcIndex+=4;
                }
            }

            EncodeGTCHBlock(blockToEncode,pOutBuffer+block);

#ifdef USE_FILEIO
            // Analysis
            CMP_BYTE      blockToSave[16][4];
            CMP_BYTE      cmp[16];
            memcpy(cmp,pOutBuffer + block,16);

            m_decoder->DecompressBlock(blockToSave, cmp);

            double bMSE = 0, gMSE = 0, rMSE = 0;
            int    MSE = 0, PSNR = 0;

            for (int y = 0; y < 16; y++) {
                    bMSE += pow(blockToSave[y][2] - blockToEncode[y][2], 2.0);
                    gMSE += pow(blockToSave[y][1] - blockToEncode[y][1], 2.0);
                    rMSE += pow(blockToSave[y][0] - blockToEncode[y][0], 2.0);
                }

            bMSE *= (1.0 / 16);
            gMSE *= (1.0 / 16);
            rMSE *= (1.0 / 16);
            MSE = int((bMSE + gMSE + rMSE) / 3);

            if (MSE == 0)
            {
                if (gt_File)
                    fprintf(gt_File, "Mode = %2d MSE 000 PSNR ---\n",cmp[15],MSE);
            }
            else
            {
                gt_total_MSE += MSE;
                PSNR = (int)(20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(MSE));
                if (gt_File)
                    fprintf(gt_File, "Mode = %2d MSE %3d PSNR %3d\n", cmp[15], MSE, PSNR);
            }
#endif

#ifdef USE_PRINTF
            // Analysis
            CMP_BYTE      blockToSave[16][4];
            CMP_BYTE      cmp[16];
            memcpy(cmp, pOutBuffer + block, 16);

            m_decoder->DecompressBlock(blockToSave, cmp);

            double bMSE = 0, gMSE = 0, rMSE = 0;
            int    MSE = 0, PSNR = 0;

            for (int y = 0; y < 16; y++) {
                bMSE += pow(blockToSave[y][2] - blockToEncode[y][2], 2.0);
                gMSE += pow(blockToSave[y][1] - blockToEncode[y][1], 2.0);
                rMSE += pow(blockToSave[y][0] - blockToEncode[y][0], 2.0);
            }

            bMSE *= (1.0 / 16);
            gMSE *= (1.0 / 16);
            rMSE *= (1.0 / 16);
            MSE = int((bMSE + gMSE + rMSE) / 3);

            if (MSE == 0)
            {
                    printf("Mode = %2d MSE 000 PSNR ---\n", cmp[15], MSE);
            }
            else
            {
                PSNR = (int)(20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(MSE));
                printf("Mode = %2d MSE %3d PSNR %3d\n", cmp[15], MSE, PSNR);
            }
#endif


            block += 16;


            #ifdef GT_COMPDEBUGGER // Checks decompression it should match or be close to source
            union BBLOCKS
            {
                CMP_DWORD    compressedBlock[4];
                CMP_BYTE            out[16];
                CMP_BYTE            in[16];
            } data;
            
            memset(data.in,0,sizeof(data));
            
            union DBLOCKS
            {
                double            blockToSave[16][4];
                double            block[64];
                CMP_BYTE          Bblock[16][4];
            } savedata;
        
            CMP_BYTE destBlock[BLOCK_SIZE_4X4X4];
            memset(savedata.block,0,sizeof(savedata));
            m_decoder->DecompressBlock(savedata.Bblock,data.in);

            for (row=0; row<64; row++)
            {
                destBlock[row] = (CMP_BYTE)savedata.block[row];
            }
            g_CompClient.SendData(3,sizeof(destBlock),destBlock);
            #endif

        }

        if(pFeedbackProc)
        {
            float fProgress = 100.f * (j * dwBlocksX) / dwBlocksXY;
            if(pFeedbackProc(fProgress, pUser1, pUser2))
            {
                #ifdef GT_COMPDEBUGGER
                    g_CompClient.disconnect();
                #endif
                FinishGTCHEncoding();
                return CE_Aborted;
            }
        }
    }

#ifdef USE_FILEIO
    if (gt_File)
    {
        if (gt_blockcount == 0) gt_blockcount = 1;
        fprintf(gt_File, "Total MSE %6d AVG_MSE %6d\n", gt_total_MSE, gt_total_MSE / gt_blockcount);
        fclose(gt_File);
        gt_File = NULL;
    }
#endif

    #ifdef GT_COMPDEBUGGER
    g_CompClient.disconnect();
    #endif

    return FinishGTCHEncoding();
}


CodecError CCodec_GTCH::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());
    
    CodecError err = InitializeGTCHLibrary();
    if (err != CE_OK) return err;
    
    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);
    const CMP_DWORD dwBlocksXY = dwBlocksX*dwBlocksY;

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {

            union FBLOCKS
            {
                float decodedBlock[16][4];
                float destBlock[BLOCK_SIZE_4X4X4];
            } DecData;

            union BBLOCKS
            {
                CMP_DWORD           compressedBlock[4];
                CMP_BYTE            out[16];
                CMP_BYTE            in[16];
            } CompData;

            CMP_FLOAT destBlock[BLOCK_SIZE_4X4X4];
            bufferIn.ReadBlock(i*4, j*4, CompData.compressedBlock, 4);

            // Encode to the appropriate location in the compressed image
            m_decoder->DecompressBlock(DecData.decodedBlock,CompData.in);

            // Create the block for decoding
            float R, G, B, A;

            int srcIndex = 0;
            for(int row=0; row < BLOCK_SIZE_4; row++)
            {
                for(int col=0; col<BLOCK_SIZE_4; col++)
                {
                    R = (CMP_FLOAT)DecData.decodedBlock[row*BLOCK_SIZE_4 + col][0];
                    G = (CMP_FLOAT)DecData.decodedBlock[row*BLOCK_SIZE_4 + col][1];
                    B = (CMP_FLOAT)DecData.decodedBlock[row*BLOCK_SIZE_4 + col][2];
                    A = (CMP_FLOAT)DecData.decodedBlock[row*BLOCK_SIZE_4 + col][3];
                    destBlock[srcIndex]     = R;
                    destBlock[srcIndex + 1] = G;
                    destBlock[srcIndex + 2] = B;
                    destBlock[srcIndex + 3] = A;
                    srcIndex+=4;
                }
            }

            bufferOut.WriteBlockRGBA(i*4, j*4, 4, 4, destBlock);

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

// Not implemented
CodecError CCodec_GTCH::Compress_Fast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    return CE_OK;
}

// Not implemented
CodecError CCodec_GTCH::Compress_SuperFast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
   return CE_OK;
}
#endif
