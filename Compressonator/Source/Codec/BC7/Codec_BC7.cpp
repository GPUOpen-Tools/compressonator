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
//
//  File Name:   Codec_BC7.cpp
//  Description: implementation of the CCodec_BC7 class
//
//////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4100)    // Ignore warnings of unreferenced formal parameters
#include "Common.h"
#include "Codec_BC7.h"
#include "BC7_Library.h"
#include <chrono>

#ifdef BC7_COMPDEBUGGER
#include "CompClient.h"
#endif

#ifdef BC7_COMPDEBUGGER
#ifndef _DEBUG
#undef BC7_COMPDEBUGGER
#endif
#endif


//======================================================================================
#define USE_MULTITHREADING  1



//
// Thread procedure for encoding a block
//
// The thread stays alive, and expects blocks to be pushed to it by a producer
// process that signals it when new work is available. When the producer is finished
// it should set the exit flag in the parameters to allow the tread to quit
//

unsigned int BC7ThreadProcEncode(void* param)
{
    BC7EncodeThreadParam *tp = (BC7EncodeThreadParam*)param;
    
    while(tp->exit == FALSE)
    {
        if(tp->run == TRUE)
        {
            tp->encoder->CompressBlock(tp->in, tp->out);
            tp->run = FALSE;
        }

        using namespace chrono;

        std::this_thread::sleep_for(0ms);
    }
    
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_BC7::CCodec_BC7() : CCodec_DXTC(CT_BC7)
{
    m_LibraryInitialized   = false;

    m_Use_MultiThreading   = true;
    m_ModeMask             = 0xCF;  // If you reset this default: seach for comments with dwmodeMask and change the values also
    m_Quality              = AMD_CODEC_QUALITY_DEFAULT;
    m_Performance          = 1.00;
    m_ColourRestrict       = FALSE;
    m_AlphaRestrict        = FALSE;
    m_ImageNeedsAlpha      = TRUE;
    m_NumThreads           = 8;

    m_NumEncodingThreads   = m_NumThreads;
    m_EncodingThreadHandle = NULL;
    m_LiveThreads          = 0;
    m_LastThread           = 0;

}


bool CCodec_BC7::SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue)
{
    if (sValue == NULL) return false;

    if (strcmp(pszParamName, "ModeMask") == 0)
    {
        m_ModeMask = (CMP_BYTE)std::stoi(sValue) & 0xFF;
        if (m_ModeMask <= 0) m_ModeMask = 0xCF;
    }
    else
    if(strcmp(pszParamName, "ColourRestrict") == 0)
        m_ColourRestrict    = (CMP_BOOL) std::stoi(sValue) > 0;
    else
    if(strcmp(pszParamName, "AlphaRestrict") == 0)
        m_AlphaRestrict        = (CMP_BOOL) std::stoi(sValue) > 0;
    else
    if(strcmp(pszParamName, "ImageNeedsAlpha") == 0)
        m_ImageNeedsAlpha     = (CMP_BOOL) std::stoi(sValue) > 0;
    else
    if(strcmp(pszParamName, "NumThreads") == 0)
    {
        m_NumThreads = (CMP_BYTE) std::stoi(sValue) & 0xFF;
        m_Use_MultiThreading = m_NumThreads > 1;
    }
    if(strcmp(pszParamName, "Quality") == 0)
    {
        m_Quality = std::stof(sValue);
        if ((m_Quality < 0) || (m_Quality > 1.0))
        {
            return false;
        }
    }
    else
    if(strcmp(pszParamName, "Performance") == 0)
    {
        m_Performance = std::stof(sValue);
        if ((m_Performance < 0) || (m_Performance > 1.0))
        {
            return false;
        }
    }
    else
        return CCodec_DXTC::SetParameter(pszParamName, sValue);
    return true;
}



bool CCodec_BC7::SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue)
{
    if(strcmp(pszParamName, "ModeMask") == 0)
        m_ModeMask            = (CMP_BYTE) dwValue & 0xFF;
    else
    if(strcmp(pszParamName, "ColourRestrict") == 0)
        m_ColourRestrict    = (CMP_BOOL) dwValue & 1;
    else
    if(strcmp(pszParamName, "AlphaRestrict") == 0)
        m_AlphaRestrict        = (CMP_BOOL) dwValue & 1;
    else
    if(strcmp(pszParamName, "ImageNeedsAlpha") == 0)
        m_ImageNeedsAlpha     = (CMP_BOOL) dwValue & 1;
    else
    if(strcmp(pszParamName, "NumThreads") == 0)
    {
        m_NumThreads = (CMP_BYTE) dwValue;
        m_Use_MultiThreading = m_NumThreads > 1;
    }
    else
        return CCodec_DXTC::SetParameter(pszParamName, dwValue);
    return true;
}

bool CCodec_BC7::SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue)
{
    if(strcmp(pszParamName, "Quality") == 0)
        m_Quality = fValue;
    else
    if(strcmp(pszParamName, "Performance") == 0)
        m_Performance = fValue;
    else
        return CCodec_DXTC::SetParameter(pszParamName, fValue);
    return true;
}


CCodec_BC7::~CCodec_BC7()
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
                while(1)
                {
                    if(m_EncodeParameterStorage[i].run != TRUE)
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

            delete[] m_EncodingThreadHandle;
        } // MultiThreading

        m_EncodingThreadHandle = NULL;

        if (m_EncodeParameterStorage)
            delete[] m_EncodeParameterStorage;
        m_EncodeParameterStorage = NULL;

        
        for(int i=0; i < m_NumEncodingThreads; i++)
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

        Quant_DeInit();

        m_LibraryInitialized = false;
    }
}



CodecError CCodec_BC7::InitializeBC7Library()
{
    if (!m_LibraryInitialized)
    {

        // One time initialisation for quantizer and shaker
        Quant_Init();
        init_ramps();


        for(CMP_DWORD i=0; i < MAX_BC7_THREADS; i++)
        {
            m_encoder[i] = NULL;
        }

        // Create threaded encoder instances
        m_LiveThreads = 0;
        m_LastThread  = 0;
        m_NumEncodingThreads = min(m_NumThreads, MAX_BC7_THREADS);
        if (m_NumEncodingThreads == 0) m_NumEncodingThreads = 1; 
        m_Use_MultiThreading = m_NumEncodingThreads > 1;

        m_EncodeParameterStorage = new BC7EncodeThreadParam[m_NumEncodingThreads];
        if(!m_EncodeParameterStorage)
        {
            return CE_Unknown;
        }

        m_EncodingThreadHandle = new std::thread[m_NumEncodingThreads];
        if(!m_EncodingThreadHandle)
        {
            delete[] m_EncodeParameterStorage;
            m_EncodeParameterStorage = NULL;

            return CE_Unknown;
        }

        CMP_DWORD   i;

        for(i=0; i < m_NumEncodingThreads; i++)
        {
            // Create single encoder instance
            m_encoder[i] = new BC7BlockEncoder( m_ModeMask,
                                                m_ImageNeedsAlpha,
                                                m_Quality,
                                                m_ColourRestrict,
                                                m_AlphaRestrict,
                                                m_Performance);

            
            // Cleanup if problem!
            if(!m_encoder[i])
            {

                delete[] m_EncodeParameterStorage;
                m_EncodeParameterStorage = NULL;

                delete[] m_EncodingThreadHandle;
                m_EncodingThreadHandle = NULL;

                for(CMP_DWORD j=0; j<i; j++)
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

        // Create the encoding threads
        for (i = 0; i<m_NumEncodingThreads; i++)
        {
            // Initialize thread parameters.
            m_EncodeParameterStorage[i].encoder = m_encoder[i];
            // Inform the thread that at the moment it doesn't have any work to do
            // but that it should wait for some and not exit
            m_EncodeParameterStorage[i].run = FALSE;
            m_EncodeParameterStorage[i].exit = FALSE;

            m_EncodingThreadHandle[i] = std::thread(
                BC7ThreadProcEncode,
                (void*)&m_EncodeParameterStorage[i]
            );
            m_LiveThreads++;
        }

        // Create single decoder instance
        m_decoder = new BC7BlockDecoder();
        if(!m_decoder)
        {
            for(CMP_DWORD j=0; j<m_NumEncodingThreads; j++)
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


CodecError CCodec_BC7::EncodeBC7Block(double  in[BC7_BLOCK_PIXELS][MAX_DIMENSION_BIG],
    CMP_BYTE    *out)
{
if (m_Use_MultiThreading)
{
    CMP_WORD   threadIndex;

    if((!m_LibraryInitialized) ||
        (!in) ||
        (!out))
    {
        return CE_Unknown;
    }

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
           MAX_SUBSET_SIZE * MAX_DIMENSION_BIG * sizeof(double));

    // Set the output pointer for the thread to the provided location
    m_EncodeParameterStorage[threadIndex].out = out;

    // Tell the thread to start working
    m_EncodeParameterStorage[threadIndex].run = TRUE;
}
else 
{
        // Copy the input data into the thread storage
        memcpy(m_EncodeParameterStorage[0].in, in, MAX_SUBSET_SIZE * MAX_DIMENSION_BIG * sizeof(double));
        // Set the output pointer for the thread to write
        m_EncodeParameterStorage[0].out = out;
        m_encoder[0]->CompressBlock(m_EncodeParameterStorage[0].in, m_EncodeParameterStorage[0].out);
}
    return CE_OK;
}

CodecError CCodec_BC7::FinishBC7Encoding(void)
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
    // Wait for all the live threads to finish any current work
    for (CMP_DWORD i = 0; i < m_LiveThreads; i++)
    {
        using namespace chrono;

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

CodecError CCodec_BC7::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    assert(bufferIn.GetWidth()    == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    CodecError err = InitializeBC7Library();
    if (err != CE_OK) return err;

#ifdef BC7_COMPDEBUGGER
    CompViewerClient    CompClient;
    bool hold_UseMultitheading = m_Use_MultiThreading;
    if (CompClient.connect())
    {
        m_Use_MultiThreading = false;
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
    #endif;

    char            row,col,srcIndex;

    CMP_BYTE    *pOutBuffer;
    pOutBuffer    = bufferOut.GetData();

    CMP_BYTE*    pInBuffer;
    pInBuffer    =  bufferIn.GetData();

    CMP_DWORD block = 0;

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {

        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            double blockToEncode[BLOCK_SIZE_4X4][CHANNEL_SIZE_ARGB];
            CMP_BYTE srcBlock[BLOCK_SIZE_4X4X4];

            memset(srcBlock,0,sizeof(srcBlock));
            bufferIn.ReadBlockRGBA(i*4, j*4, 4, 4, srcBlock);

            #ifdef BC7_COMPDEBUGGER
            if (CompClient.Connected())
            {
                CompClient.SendData(1, sizeof(srcBlock), srcBlock);
            }
            #endif

            // Create the block for encoding
            srcIndex = 0;
            for(row=0; row < BLOCK_SIZE_4; row++)
            {
                for(col=0; col < BLOCK_SIZE_4; col++)
                {
                    blockToEncode[row*BLOCK_SIZE_4+col][BC_COMP_RED]        = (double)srcBlock[srcIndex];
                    blockToEncode[row*BLOCK_SIZE_4+col][BC_COMP_GREEN]        = (double)srcBlock[srcIndex+1];
                    blockToEncode[row*BLOCK_SIZE_4+col][BC_COMP_BLUE]        = (double)srcBlock[srcIndex+2];
                    blockToEncode[row*BLOCK_SIZE_4+col][BC_COMP_ALPHA]        = (double)srcBlock[srcIndex+3];
                    srcIndex+=4;
                }
            }

            EncodeBC7Block(blockToEncode, pOutBuffer + block);

#ifdef BC7_COMPDEBUGGER // Checks decompression it should match or be close to source
            if (CompClient.Connected())
            {
                CMP_BYTE destBlock[BLOCK_SIZE_4X4X4];

                union FBLOCKS
                {
                    double decodedBlock[16][4];
                    double destBlock[BLOCK_SIZE_4X4X4];
                } DecData;

                // Encode to the appropriate location in the compressed image
                m_decoder->DecompressBlock(DecData.decodedBlock, pOutBuffer + block);

                // Create the block for decoding
                int srcIndex = 0;
                for (int row = 0; row < BLOCK_SIZE_4; row++)
                {
                    for (int col = 0; col<BLOCK_SIZE_4; col++)
                    {
                        destBlock[srcIndex]     = (CMP_BYTE)DecData.decodedBlock[row*BLOCK_SIZE_4 + col][BC_COMP_RED];
                        destBlock[srcIndex + 1] = (CMP_BYTE)DecData.decodedBlock[row*BLOCK_SIZE_4 + col][BC_COMP_GREEN];
                        destBlock[srcIndex + 2] = (CMP_BYTE)DecData.decodedBlock[row*BLOCK_SIZE_4 + col][BC_COMP_BLUE];
                        destBlock[srcIndex + 3] = (CMP_BYTE)DecData.decodedBlock[row*BLOCK_SIZE_4 + col][BC_COMP_ALPHA];
                        srcIndex += 4;
                    }
                }
                CompClient.SendData(2, sizeof(destBlock), destBlock);
            }
#endif

            block += 16;
        }

        if(pFeedbackProc)
        {
            float fProgress = 100.f * (j * dwBlocksX) / dwBlocksXY;
            if(pFeedbackProc(fProgress, pUser1, pUser2))
            {
                #ifdef BC7_COMPDEBUGGER
                    CompClient.disconnect();
                #endif
                FinishBC7Encoding();
                return CE_Aborted;
            }
        }

    }


    #ifdef BC7_COMPDEBUGGER
    CompClient.disconnect();
    m_Use_MultiThreading = hold_UseMultitheading;
    #endif

    return FinishBC7Encoding();
}


CodecError CCodec_BC7::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());
    
    CodecError err = InitializeBC7Library();
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
                double decodedBlock[16][4];
                double destBlock[BLOCK_SIZE_4X4X4];
            } DecData;

            union BBLOCKS
            {
                CMP_DWORD    compressedBlock[4];
                CMP_BYTE            out[16];
                CMP_BYTE            in[16];
            } CompData;

            CMP_BYTE destBlock[BLOCK_SIZE_4X4X4];

            bufferIn.ReadBlock(i*4, j*4, CompData.compressedBlock, 4);

            // Encode to the appropriate location in the compressed image
            m_decoder->DecompressBlock(DecData.decodedBlock,CompData.in);

            // Create the block for decoding
            int srcIndex = 0;
            for(int row=0; row < BLOCK_SIZE_4; row++)
            {
                for(int col=0; col<BLOCK_SIZE_4; col++)
                {
                    destBlock[srcIndex]   = (CMP_BYTE)DecData.decodedBlock[row*BLOCK_SIZE_4+col][BC_COMP_RED];
                    destBlock[srcIndex+1] = (CMP_BYTE)DecData.decodedBlock[row*BLOCK_SIZE_4+col][BC_COMP_GREEN];
                    destBlock[srcIndex+2] = (CMP_BYTE)DecData.decodedBlock[row*BLOCK_SIZE_4+col][BC_COMP_BLUE];
                    destBlock[srcIndex+3] = (CMP_BYTE)DecData.decodedBlock[row*BLOCK_SIZE_4+col][BC_COMP_ALPHA];
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
CodecError CCodec_BC7::Compress_Fast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    return CE_OK;
}

// Not implemented
CodecError CCodec_BC7::Compress_SuperFast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
   return CE_OK;
}
