//=====================================================================
// Copyright (c) 2020    Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================
// Ref: GPUOpen-Tools/Compressonator
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016, Intel Corporation
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

#include "cmp_hpc.h"

#ifdef _WIN32
#include <strsafe.h>
#endif

#ifdef USE_ASPM_CODE

CompressionFunc* gCompressionFunc = CompressTexture;

bool    gMultithreaded = true;
double  gCompTime = 0.0;
double  gCompRate = 0.0;
int     gTexWidth = 0;
int     gTexHeight = 0;
double  gError = 0.0;
double  gError2 = 0.0;

// Win32 thread API
const int kMaxWinThreads = 64;

enum EThreadState {
    eThreadState_WaitForData,
    eThreadState_DataLoaded,
    eThreadState_Running,
    eThreadState_Done
};

struct WinThreadData {
    EThreadState state;
    int threadIdx;
    //void (*cmpFunc)(const BYTE* inBuf, BYTE* outBuf);
    CompressionFunc* cmpFunc;
    texture_surface input;
    BYTE *output;

    // Defaults..
    WinThreadData() :
        state(eThreadState_Done),
        threadIdx(-1),
        input(),
        output(NULL),
        cmpFunc(NULL)
    { }

} gWinThreadData[kMaxWinThreads];

HANDLE  gWinThreadWorkEvent[kMaxWinThreads];
HANDLE  gWinThreadStartEvent = NULL;
HANDLE  gWinThreadDoneEvent = NULL;
int     gNumWinThreads = 0;
DWORD   gNumProcessors = 1; // We have at least one processor.
DWORD   dwThreadIdArray[kMaxWinThreads];
HANDLE  hThreadArray[kMaxWinThreads];


void DestroyThreads() {
    if(gMultithreaded) {
        // Release all windows threads that may be active...
        for(int i=0; i < gNumWinThreads; i++) {
            gWinThreadData[i].state = eThreadState_Done;
        }

        // Send the event for the threads to start.
        if (ResetEvent(gWinThreadDoneEvent) == NULL) {
            printf("DestroyThreads Error 1\n");
        }

        if (SetEvent(gWinThreadStartEvent) == NULL) {
            printf("DestroyThreads Error 2\n");
        }

        // Wait for all the threads to finish....
        DWORD dwWaitRet = WaitForMultipleObjects(gNumWinThreads, hThreadArray, TRUE, INFINITE);
        if(WAIT_FAILED == dwWaitRet) {
            printf("DestroyThreads() Error: WaitForMultipleObjects\n");
        }

        // !HACK! This doesn't actually do anything. There is either a bug in the
        // Intel compiler or the windows run-time that causes the threads to not
        // be cleaned up properly if the following two lines of code are not present.
        // Since we're passing INFINITE to WaitForMultipleObjects, that function will
        // never time out and per-microsoft spec, should never give this return value...
        // Even with these lines, the bug does not consistently disappear unless you
        // clean and rebuild. Heigenbug?
        //
        // If we compile with MSVC, then the following two lines are not necessary.
        else if(WAIT_TIMEOUT == dwWaitRet)
            OutputDebugString("DestroyThreads() -- WaitForMultipleObjects -- TIMEOUT");

        // Reset the start event
        if (ResetEvent(gWinThreadStartEvent) == NULL) {
            printf("DestroyThreads() Error:2\n");
        }

        if (SetEvent(gWinThreadDoneEvent) == NULL) {
            printf("DestroyThreads() Error:3\n");
        }

        // Close all thread handles.
        for(int i=0; i < gNumWinThreads; i++) {
            if (CloseHandle(hThreadArray[i]) == NULL) {
                printf("DestroyThreads() Error:3\n");
            }
        }

        for(int i =0; i < kMaxWinThreads; i++ ) {
            hThreadArray[i] = NULL;
        }

        // Close all event handles...
        if (CloseHandle(gWinThreadDoneEvent) == NULL) {
            printf("DestroyThreads() Error:4\n");
        }

        gWinThreadDoneEvent = NULL;

        if (CloseHandle(gWinThreadStartEvent) == NULL) {
            printf("DestroyThreads() Error:5\n");
        }

        gWinThreadStartEvent = NULL;

        for(int i = 0; i < gNumWinThreads; i++) {
            if (CloseHandle(gWinThreadWorkEvent[i]) == NULL) {
                printf("DestroyThreads() Error:6\n");
            }
        }

        for(int i = 0; i < kMaxWinThreads; i++) {
            gWinThreadWorkEvent[i] = NULL;
        }

        gNumWinThreads = 0;
    }
}

void CompressImageST(const texture_surface* input, BYTE* output) {
    if(gCompressionFunc == NULL) {
        printf("CompressImageST Error\n");
        return;
    }

    // Do the compression.
    (*gCompressionFunc)(input, output);
}

VOID CompressImageMT(const texture_surface* input, BYTE* output) {
    const int numThreads = gNumWinThreads;
    const int bytesPerBlock = 16;  // BC3, BC7 128 bits compressed , BC1 is 64 bits = 8 Bytes

    // We want to split the data evenly among all threads.
    const int linesPerThread = (input->height + numThreads - 1) / numThreads;

    if(gCompressionFunc == NULL) {
        printf("CompressImageMT Error!\n");
        return;
    }

    // Load the threads.
    for(int threadIdx = 0; threadIdx < numThreads; threadIdx++) {
        int y_start = (linesPerThread*threadIdx)/4*4;
        int y_end = (linesPerThread*(threadIdx+1))/4*4;
        if (y_end > input->height) y_end = input->height;

        WinThreadData *data = &gWinThreadData[threadIdx];
        data->input = *input;
        data->input.ptr = input->ptr + y_start * input->stride;
        data->input.height = y_end-y_start;
        data->output = output + (y_start/4) * (input->width/4) * bytesPerBlock;
        data->cmpFunc = gCompressionFunc;
        data->state = eThreadState_DataLoaded;
        data->threadIdx = threadIdx;
    }

    // Send the event for the threads to start.
    if (ResetEvent(gWinThreadDoneEvent) == NULL) {
        printf("CompressImageMT Error 1\n");
    }

    if (SetEvent(gWinThreadStartEvent) == NULL) {
        printf("CompressImageMT Error 2\n");
    }

    // Wait for all the threads to finish
    if(WAIT_FAILED == WaitForMultipleObjects(numThreads, gWinThreadWorkEvent, TRUE, INFINITE))
        printf("CompressImageDXTWIN -- WaitForMultipleObjects\n");

    // Reset the start event
    if (ResetEvent(gWinThreadStartEvent) == NULL) {
    }

    if (SetEvent(gWinThreadDoneEvent) == NULL) {
    }
}

DWORD WINAPI CompressImageMT_Thread( LPVOID lpParam ) {
    WinThreadData *data = (WinThreadData *)lpParam;

    while(data->state != eThreadState_Done) {

        if(WAIT_FAILED == WaitForSingleObject(gWinThreadStartEvent, INFINITE))
            printf("CompressImageMT_Thread Error 1\n");

        if(data->state == eThreadState_Done)
            break;

        data->state = eThreadState_Running;
        (*(data->cmpFunc))(&data->input, data->output);

        data->state = eThreadState_WaitForData;

        HANDLE workEvent = gWinThreadWorkEvent[data->threadIdx];
        if(WAIT_FAILED == SignalObjectAndWait(workEvent, gWinThreadDoneEvent, INFINITE, FALSE))
            printf("CompressImageMT_Thread Error 2\n");
    }

    return 0;
}

void Initialize() {
    // Make sure that the event array is set to null...
    memset(gWinThreadWorkEvent, 0, sizeof(gWinThreadWorkEvent));

    // Figure out how many cores there are on this machine
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    gNumProcessors = sysinfo.dwNumberOfProcessors;

    // Make sure all of our threads are empty.
    for(int i = 0; i < kMaxWinThreads; i++) {
        hThreadArray[i] = NULL;
    }
}

void InitWin32Threads(int numThreads) {
    if(gNumWinThreads > 0) {
        return;
    }

    SetLastError(0);

    // Each Processor is assigned 2 threads by default!
    gNumWinThreads = numThreads;

    if(gNumWinThreads > MAXIMUM_WAIT_OBJECTS)
        gNumWinThreads = MAXIMUM_WAIT_OBJECTS;

    printf("Num Threads %d (Max Processors %d)\n",numThreads,gNumProcessors);


    // Create the synchronization events.
    for(int i = 0; i < gNumWinThreads; i++) {
        gWinThreadWorkEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (gWinThreadWorkEvent[i] == NULL) {
            printf("InitWin32Threads Error 1\n");
        }
    }

    gWinThreadStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (gWinThreadStartEvent == NULL) {
        printf("InitWin32Threads Error 2\n");

    }

    gWinThreadDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (gWinThreadDoneEvent == NULL) {
        printf("InitWin32Threads Error 3\n");
    }


    // Create threads
    for(int threadIdx = 0; threadIdx < gNumWinThreads; threadIdx++) {
        gWinThreadData[threadIdx].state = eThreadState_WaitForData;
        hThreadArray[threadIdx] = CreateThread(NULL, 0, CompressImageMT_Thread, &gWinThreadData[threadIdx], 0, &dwThreadIdArray[threadIdx]);
        if (  hThreadArray[threadIdx] == NULL) {
            printf("InitWin32Threads Error 4\n");
        }
    }
}


VOID CompressSTMT(const texture_surface* input, BYTE* output, int threads) {
    //printf("CompressSTMT %d %d\n",gNumProcessors,threads);
    // If we aren't multi-cored, then just run everything serially.
    if ( (gNumProcessors < 2) || (threads == 1) ) {
        gMultithreaded = false;
        CompressImageST(input, output);
    } else {
        gMultithreaded = true;
        CompressImageMT(input, output);
    }
}
#endif

