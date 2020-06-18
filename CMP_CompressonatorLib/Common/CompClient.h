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

#ifndef _COMPCLIENT_H_INCLUDED_
#define _COMPCLIENT_H_INCLUDED_

#include <atlstr.h>
#include <strsafe.h>

#define BUFFER_SIZE                    512                                // bytes MAX buffer size
#define COMPRESS_DATA_HEADER_SIZE     4                                // bytes offset until first data location in COMPRESS_DATA structure
#define REMOTE_AMD_COMPRESSOR        "\\\\.\\pipe\\AMD_COMPRESS"        // Shared Pipe name of remote connection

struct COMPRESS_DATA
{
    WORD        cmd;            // 2 Bytes
    WORD        data_size;      // 2 Bytes
    // Above bytes should add up to COMPRESS_DATA_HEADER_SIZE
    union
    {
        unsigned char    data [BUFFER_SIZE];
        float           fdata[BUFFER_SIZE / sizeof(float)];
    };
};

struct COMPRESS_ACK_DATA
{
    unsigned short        cmd;                // 2 Bytes
    unsigned short        data_size = 0;      // 2 Bytes
};

class CompViewerClient
{
public:    
    CompViewerClient()
    {
        snprintf(m_strPipeName,128,REMOTE_AMD_COMPRESSOR); 
        m_hPipe = INVALID_HANDLE_VALUE;
        memset(&m_data,0,sizeof(m_data));
    }


    ~CompViewerClient()
    {
        disconnect();
    }


void disconnect()
{
    if (m_hPipe != INVALID_HANDLE_VALUE)
    {
            // Close the pipe if opened.
            CloseHandle(m_hPipe); 
            m_hPipe = INVALID_HANDLE_VALUE;
    }
}

bool connect()
{
    if (m_hPipe != INVALID_HANDLE_VALUE) return true; // Already connected

    bool connected = false;

    while (TRUE) 
    {
        m_hPipe = CreateFileA( 
            m_strPipeName,            // Pipe name 
            GENERIC_READ |            // Read and write access 
            GENERIC_WRITE,
            0,                        // No sharing 
            NULL,                    // Default security attributes
            OPEN_EXISTING,            // Opens existing pipe 
            0,                        // Default attributes 
            NULL);                    // No template file 

        // Break if the pipe handle is valid. 
        if (m_hPipe != INVALID_HANDLE_VALUE) 
            break; 
 
        if (// Exit if an error other than ERROR_PIPE_BUSY occurs
            GetLastError() != ERROR_PIPE_BUSY 
            ||
            // All pipe instances are busy, so wait for 5 seconds
            !WaitNamedPipeA(m_strPipeName, 5000)) 
        {
            m_error = GetLastError();
            m_hPipe = INVALID_HANDLE_VALUE;
            return false;
        }
    }

    if (GetResponse())
    {
        connected = true;
    }
    else
    {
        disconnect();
    }

    return connected;
}

bool Connected()
{
    return (m_hPipe != INVALID_HANDLE_VALUE);
}

bool SendData(WORD Cmd, WORD DataSize, byte *Data, bool Respond = true)
{
    if (m_hPipe == INVALID_HANDLE_VALUE) return false; // not connected

    if (DataSize > (BUFFER_SIZE - COMPRESS_DATA_HEADER_SIZE)) return false; // data size too big to handle!

    m_data.cmd = Cmd; // Darw Source pixels
    m_data.data_size = DataSize;
    memcpy(m_data.data,Data,DataSize);
    
    m_bResult = WriteFile(            // Write to the pipe.
        m_hPipe,                    // Handle of the pipe
        &m_data,                    // Message to be written
        m_data.data_size + COMPRESS_DATA_HEADER_SIZE,            // Number of bytes to write
        &m_cbBytesWritten,            // Number of bytes written
        NULL);                        // Not overlapped 

    if (!m_bResult/*Failed*/ || (m_data.data_size + (DWORD)COMPRESS_DATA_HEADER_SIZE) != m_cbBytesWritten/*Failed*/) 
    {
        m_error = GetLastError();
        return false;
    }

    // reset data size as data has been sent
    m_data.data_size = 0;

    if (Respond)
        GetResponse();

    return true;
}

private:

bool GetResponse()
{
    if (m_hPipe == INVALID_HANDLE_VALUE) return false; // not connected

    m_cbBytesRead = 0;

    // Receive the response from the server.
    m_cbReplyBytes = sizeof(COMPRESS_ACK_DATA);

    do
    {
        m_bResult = ReadFile(            // Read from the pipe.
            m_hPipe,                     // Handle of the pipe
            &m_chReply,                  // Buffer to receive the reply
            m_cbReplyBytes,              // Size of buffer 
            &m_cbBytesRead,              // Number of bytes read 
            NULL);                       // Not overlapped 

        if (!m_bResult && GetLastError() != ERROR_MORE_DATA) 
        {
            m_error = GetLastError();
            break;
        }

    } while (!m_bResult);  // Repeat loop if ERROR_MORE_DATA 

    return true;
}


protected:
    // Prepare the pipe name
    char    m_strPipeName[128];
    HANDLE  m_hPipe;
    DWORD   m_error;

    DWORD    m_cbBytesWritten;
    DWORD    m_cbBytesRead, m_cbReplyBytes;
    BOOL    m_bResult;

public:
    COMPRESS_ACK_DATA   m_chReply;
    COMPRESS_DATA       m_data;

};
#endif

