//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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

// Qt
#include <QtWidgets>

// Infra:
#include <AMDTBaseTools/Include/AMDTDefinitions.h>
#include <AMDTBaseTools/Include/gtAssert.h>
#include <AMDTOSWrappers/Include/osDebugLog.h>
#include <AMDTOSWrappers/Include/osCallStack.h>
#include <AMDTOSWrappers/Include/osCallsStackReader.h>
#include <AMDTOSWrappers/Include/osProcess.h>
#include <AMDTOSWrappers/Include/osThread.h>

// AMDTApplicationFramework:
//#include <AMDTApplicationFramework/Include/afApplicationCommands.h>

// AMDTApplicationComponents:
#include <AMDTApplicationComponents/Include/acMessageBox.h>

// local
#include <cpUnhandledExceptionHandler.h>



// ---------------------------------------------------------------------------
// Name:        cpUnhandledExceptionHandler::cpUnhandledExceptionHandler
// Description: Constructor.
// Author:      Yaki Tebeka
// Date:        21/4/2009
// ---------------------------------------------------------------------------
cpUnhandledExceptionHandler::cpUnhandledExceptionHandler() : m_shouldStopWaiting(false)
{
}


// ---------------------------------------------------------------------------
// Name:        cpUnhandledExceptionHandler::~cpUnhandledExceptionHandler
// Description: Destructor
// Author:      Yaki Tebeka
// Date:        21/4/2009
// ---------------------------------------------------------------------------
cpUnhandledExceptionHandler::~cpUnhandledExceptionHandler()
{
}


// ---------------------------------------------------------------------------
// Name:        cpUnhandledExceptionHandler::instance
// Description: Returns this class' single instance
// Author:      Yaki Tebeka
// Date:        21/4/2009
// ---------------------------------------------------------------------------
cpUnhandledExceptionHandler& cpUnhandledExceptionHandler::instance()
{
    // If my single instance was not already created:
	cpUnhandledExceptionHandler* pSignleInstance = (cpUnhandledExceptionHandler*)osUnhandledExceptionHandler::instance();

    if (pSignleInstance == nullptr)
    {
        pSignleInstance = new cpUnhandledExceptionHandler;
        osUnhandledExceptionHandler::registerSingleInstance((osUnhandledExceptionHandler&)*pSignleInstance);
    }

    return *pSignleInstance;
}


// ---------------------------------------------------------------------------
// Name:        cpUnhandledExceptionHandler::onUnhandledException
// Description: Is called when an unhandled exception is caught.
// Arguments: exceptionCode - The exception code.
//            pExceptionContext - A pointer to the exception context (cast into void*)
// Author:      Yaki Tebeka
// Date:        21/4/2009
// ---------------------------------------------------------------------------
void cpUnhandledExceptionHandler::onUnhandledException(osExceptionCode exceptionCode, void* pExceptionContext)
{
    const unsigned SLEEP_INTERVAL_MS = 500;

    // Notify the handlers about an unhandled exception scenario.
    // emit osUnhandledExceptionHandler(exceptionCode, pExceptionContext);

    // Wait for the handler to inform us that it is finished and that we can and should exit.
    while (!m_shouldStopWaiting)
    {
        osSleep(SLEEP_INTERVAL_MS);
    }
}

void cpUnhandledExceptionHandler::StopWaiting()
{
    m_shouldStopWaiting = true;
}


