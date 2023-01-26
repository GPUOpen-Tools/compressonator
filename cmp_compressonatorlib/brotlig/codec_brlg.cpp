//===============================================================================
// Copyright (c) 2022  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   Codec_brlg.cpp
//  Description: implementation of the CCodec_BRLG class
//
//////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4100)    // Ignore warnings of unreferenced formal parameters

#ifdef _WIN32
#include "common.h"
#include "codec_brlg.h"
#include <process.h>
#include "debug.h"
#include "brlg_sdk_wrapper.h"

// Gets the total numver of active processor cores on the running host system
extern CMP_INT CMP_GetNumberOfProcessors();

//
// Thread procedure for encoding a block
//
// The thread stays alive, and expects blocks to be pushed to it by a producer
// process that signals it when new work is available. When the producer is finished
// it should set the exit flag in the parameters to allow the tread to quit
//

unsigned int    _stdcall BRLGThreadProcEncode(void* param) {
    BRLGEncodeThreadParam* tp = (BRLGEncodeThreadParam*)param;

    while(tp->exit == FALSE) {
        if(tp->run == TRUE) {
            tp->encoder->CompressBlock(tp->in, tp->out);
            tp->run = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(0));
    }

    return 0;
}


static BRLGEncodeThreadParam* g_EncodeParameterStorage = NULL;


//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_BRLG::CCodec_BRLG()
    : CCodec_DXTC(CT_BRLG)
{
    m_LibraryInitialized   = false;

    m_Use_MultiThreading    = true;
    m_NumThreads            = 0;
    m_UseGPUDecompression = false;
    m_NumEncodingThreads    = 0;
    m_EncodingThreadHandle  = NULL;
    m_LiveThreads           = 0;
    m_LastThread            = 0;
    m_PageSize              = 65536;    // Fixed max size for v1.0

}

bool CCodec_BRLG::SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue)
{
    if (sValue == NULL) return false;

    if(strcmp(pszParamName, CodecParameters::NumThreads) == 0)
    {
        m_NumThreads = (CMP_BYTE) std::stoi(sValue) & 0xFF;
        m_Use_MultiThreading = m_NumThreads != 1;
    }
    else if (strcmp(pszParamName, CodecParameters::PageSize) == 0)
    {
        m_PageSize = std::stoi(sValue);
    }
    else
        return CCodec_DXTC::SetParameter(pszParamName, sValue);
    
    return true;
}

bool CCodec_BRLG::SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue)
{
    if(strcmp(pszParamName, CodecParameters::NumThreads) == 0)
    {
        m_NumThreads = (CMP_BYTE) dwValue;
        m_Use_MultiThreading = m_NumThreads != 1;
    }
    else if (strcmp(pszParamName, CodecParameters::UseGPUDecompression) == 0)
    {
        m_UseGPUDecompression = dwValue != 0;
    }
    else if (strcmp(pszParamName, CodecParameters::PageSize) == 0)
    {
        m_PageSize = dwValue;
    }
    else
        return CCodec_DXTC::SetParameter(pszParamName, dwValue);
    return true;
}

bool CCodec_BRLG::SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue)
{
    return CCodec_DXTC::SetParameter(pszParamName, fValue);
}


CCodec_BRLG::~CCodec_BRLG()
{
    if (m_LibraryInitialized) {

        if (m_Use_MultiThreading) {
            // Tell all the live threads that they can exit when they have finished any current work
            for(int i=0; i < m_LiveThreads; i++) {
                // If a thread is in the running state then we need to wait for it to finish
                // any queued work from the producer before we can tell it to exit.
                //
                // If we don't wait then there is a race condition here where we have
                // told the thread to run but it hasn't yet been scheduled - if we set
                // the exit flag before it runs then its block will not be processed.
#pragma warning(push)
#pragma warning(disable:4127) //warning C4127: conditional expression is constant
                while (1) {
                    if (m_EncodeParameterStorage[i].run != TRUE) {
                        break;
                    }
                }
#pragma warning(pop)
                // Signal to the thread that it can exit
                m_EncodeParameterStorage[i].exit = TRUE;
            }

            // Now wait for all threads to have exited
            if (m_LiveThreads > 0) {
                for (CMP_DWORD dwThread = 0; dwThread < m_LiveThreads; dwThread++) {
                    std::thread& curThread = m_EncodingThreadHandle[dwThread];

                    curThread.join();
                }
            }

            for (unsigned int i = 0; i < m_LiveThreads; i++) {
                std::thread& curThread = m_EncodingThreadHandle[i];

                curThread = std::thread();
            }


        } // MultiThreading

        m_EncodingThreadHandle = NULL;

        if (m_EncodeParameterStorage)
            delete[] m_EncodeParameterStorage;
        m_EncodeParameterStorage = NULL;


        for (int i = 0; i < m_NumEncodingThreads; i++) {
            if (m_encoder[i]) {
                delete m_encoder[i];
                m_encoder[i] = NULL;
            }
        }

        if (m_decoder) {
            delete m_decoder;
            m_decoder = NULL;
        }

        m_LibraryInitialized = false;
    }
}



CodecError CCodec_BRLG::InitializeBRLGLibrary()
{
    if (!m_LibraryInitialized) {

        for(CMP_DWORD i=0; i < MAX_BRLG_THREADS; i++) {
            m_encoder[i] = NULL;
        }

        // Create threaded encoder instances
        m_LiveThreads = 0;
        m_LastThread  = 0;
        m_NumEncodingThreads = min(m_NumThreads, MAX_BRLG_THREADS);
        if (m_NumEncodingThreads == 0) {
            m_NumEncodingThreads = CMP_GetNumberOfProcessors();
            if (m_NumEncodingThreads <= 2)
                m_NumEncodingThreads = 8; // fallback to a default!
            if (m_NumEncodingThreads > 128)
                m_NumEncodingThreads = 128;

        }
        m_Use_MultiThreading = (m_NumEncodingThreads != 1);

        m_EncodeParameterStorage = new BRLGEncodeThreadParam[m_NumEncodingThreads];
        if (!m_EncodeParameterStorage) {
            return CE_Unknown;
        }

        m_EncodingThreadHandle = new std::thread[m_NumEncodingThreads];
        if (!m_EncodingThreadHandle) {
            delete[] m_EncodeParameterStorage;
            m_EncodeParameterStorage = NULL;

            return CE_Unknown;
        }

        CMP_INT   i;

        for(i=0; i < m_NumEncodingThreads; i++) {
            // Create single encoder instance
            m_encoder[i] = new BRLGBlockEncoder(1.0);


            // Cleanup if problem!
            if (!m_encoder[i]) {

                delete[] m_EncodeParameterStorage;
                m_EncodeParameterStorage = NULL;

                delete[] m_EncodingThreadHandle;
                m_EncodingThreadHandle = NULL;

                for (CMP_INT j = 0; j<i; j++) {
                    delete m_encoder[j];
                    m_encoder[j] = NULL;
                }

                return CE_Unknown;
            }

        }

        // Create the encoding threads in the suspended state
        for (i = 0; i<m_NumEncodingThreads; i++) {
            // Initialize thread parameters.
            m_EncodeParameterStorage[i].encoder = m_encoder[i];
            // Inform the thread that at the moment it doesn't have any work to do
            // but that it should wait for some and not exit
            m_EncodeParameterStorage[i].run = FALSE;
            m_EncodeParameterStorage[i].exit = FALSE;

            m_EncodingThreadHandle[i] = std::thread(BRLGThreadProcEncode,
                                            (void*)&m_EncodeParameterStorage[i]
                                        );
            m_LiveThreads++;
        }


        // Create single decoder instance
        m_decoder = new BRLGBlockDecoder();
        if(!m_decoder) {
            for (CMP_INT j = 0; j<m_NumEncodingThreads; j++) {
                delete m_encoder[j];
                m_encoder[j] = NULL;
            }
            return CE_Unknown;
        }

        m_LibraryInitialized = true;
    }
    return CE_OK;
}


CodecError CCodec_BRLG::EncodeBRLGBlock(CMP_BYTE* in, CMP_BYTE* out)
{
#ifdef USE_NOMULTITHREADING
    m_Use_MultiThreading = false;
#endif

    if (m_Use_MultiThreading) {
        CMP_WORD   threadIndex;

        // Loop and look for an available thread
        CMP_BOOL found = FALSE;
        threadIndex = m_LastThread;
        while (found == FALSE) {

            if (m_EncodeParameterStorage == NULL)
                return CE_Unknown;

            if(m_EncodeParameterStorage[threadIndex].run == FALSE) {
                found = TRUE;
                break;
            }

            // Increment and wrap the thread index
            threadIndex++;
            if(threadIndex == m_LiveThreads) {
                threadIndex = 0;
            }
        }

        m_LastThread = threadIndex;

        // Copy the input data into the thread storage
        memcpy(m_EncodeParameterStorage[threadIndex].in,in,m_xdim * m_ydim * 4 * sizeof(CMP_BYTE));

        // Set the output pointer for the thread to the provided location
        m_EncodeParameterStorage[threadIndex].out = out;

        // Tell the thread to start working
        m_EncodeParameterStorage[threadIndex].run = TRUE;
    } else {
        // Copy the input data into the thread storage
        memcpy(m_EncodeParameterStorage[0].in, in, m_xdim * m_ydim * 4  * sizeof(CMP_BYTE));

        // Set the output pointer for the thread to write
        m_EncodeParameterStorage[0].out = out;
        m_encoder[0]->CompressBlock(m_EncodeParameterStorage[0].in,m_EncodeParameterStorage[0].out);
    }
    return CE_OK;
}

CodecError CCodec_BRLG::FinishBRLGEncoding(void)
{
    if(!m_LibraryInitialized) {
        return CE_Unknown;
    }

    if (!m_EncodeParameterStorage) {
        return CE_Unknown;
    }

    if (m_Use_MultiThreading) {
        // Wait for all the live threads to finish any current work
        for(CMP_DWORD i=0; i < m_LiveThreads; i++) {

            // If a thread is in the running state then we need to wait for it to finish
            // its work from the producer
            while (m_EncodeParameterStorage[i].run == TRUE) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
    return CE_OK;
}

CodecError CCodec_BRLG::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    uint32_t outputSize = 0;

    bool result = BRLG::EncodeDataStream(bufferIn.GetData(), bufferIn.GetDataSize(), bufferOut.GetData(), &outputSize, m_PageSize);

    if (!result)
        return CE_Unknown;

    bufferOut.SetDataSize(outputSize);
    return CE_OK;
}


CodecError CCodec_BRLG::Decompress(CCodecBuffer&       bufferIn,
                                   CCodecBuffer&       bufferOut,
                                   Codec_Feedback_Proc pFeedbackProc,
                                   CMP_DWORD_PTR       pUser1,
                                   CMP_DWORD_PTR       pUser2)
{
    bool result = true;

    uint32_t outputSize = bufferOut.GetDataSize();

    if (m_UseGPUDecompression)
        result = BRLG::DecodeDataStreamGPU(bufferIn.GetData(), bufferIn.GetDataSize(), bufferOut.GetData(), &outputSize);
    else
        result = BRLG::DecodeDataStreamCPU(bufferIn.GetData(), bufferIn.GetDataSize(), bufferOut.GetData(), &outputSize);

    if (!result)
        return CE_Unknown;

    return CE_OK;
}

// Not implemented
CodecError CCodec_BRLG::Compress_Fast(CCodecBuffer&       bufferIn,
                                      CCodecBuffer&       bufferOut,
                                      Codec_Feedback_Proc pFeedbackProc,
                                      CMP_DWORD_PTR       pUser1,
                                      CMP_DWORD_PTR       pUser2)
{
    return CE_OK;
}

// Not implemented
CodecError CCodec_BRLG::Compress_SuperFast(CCodecBuffer&       bufferIn,
                                           CCodecBuffer&       bufferOut,
                                           Codec_Feedback_Proc pFeedbackProc,
                                           CMP_DWORD_PTR       pUser1,
                                           CMP_DWORD_PTR       pUser2)
{
    return CE_OK;
}
#endif
