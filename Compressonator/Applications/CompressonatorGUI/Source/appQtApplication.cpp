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

#include "appQtApplication.h"
#include "cpUnhandledExceptionHandler.h"

// This variable is used in a static version in order to get access to the app so we can emit an event from there:
appQtApplication* gpQtApplicationForEmit = NULL;

// This is the csBuffer that will store the out of memory call stack. easier to store it this way then start passing it in an Qt event
osCallStack gOutOfMemoryCsBuffer;

// ---------------------------------------------------------------------------
// Name:        appQtApplication::appQtApplication
// Description: constructor
// Author:      Gilad Yarnitzky
// Date:        30/5/2012
// ---------------------------------------------------------------------------
appQtApplication::appQtApplication(int& argc, char** argv) : QApplication(argc, argv)
{
    // Register osExceptionCode as a meta-type for Qt. This allows Qt to queue instances
    //  of that type, which will be required to handle the exceptions across threads.
    qRegisterMetaType<osExceptionCode>("osExceptionCode");
    // Register this object which runs in the main thread as the top-level handler of exceptions in CodeXL.
    cpUnhandledExceptionHandler& unhandledExceptionsHandler = cpUnhandledExceptionHandler::instance();
    bool rc = connect(&unhandledExceptionsHandler, SIGNAL(UnhandledExceptionSignal(osExceptionCode, void*)),
        this, SLOT(UnhandledExceptionHandler(osExceptionCode, void*)), Qt::AutoConnection);
    GT_ASSERT(rc);

    gpQtApplicationForEmit      = this;
    m_allowDifferentSystemPath  = false;
}

// ---------------------------------------------------------------------------
// Name:        appQtApplication::~appQtApplication
// Description: destructor
// Author:      Gilad Yarnitzky
// Date:        30/5/2012
// ---------------------------------------------------------------------------
appQtApplication::~appQtApplication()
{

}

// ---------------------------------------------------------------------------
// Name:        appQtApplication::notify
// Description: overwrite notify to capture the keyevent for handling shortcut updates
// Author:      Gilad Yarnitzky
// Date:        31/5/2012
// ---------------------------------------------------------------------------
bool appQtApplication::notify(QObject* pReceiver, QEvent* pEvent)
{
    bool result = false;

    try {
        result = QApplication::notify(pReceiver, pEvent);
    }

    catch (std::exception &e) {
            qFatal("Error %s sending event %s to object %s (%s)",
            e.what(), typeid(*pEvent).name(), qPrintable(pReceiver->objectName()),
            typeid(*pReceiver).name());
    }
    catch (...) {
            qFatal("Error <unknown> sending event %s to object %s (%s)",
            typeid(*pEvent).name(), qPrintable(pReceiver->objectName()),
            typeid(*pReceiver).name());
    }

    // qFatal aborts, so this isn't really necessary
    // but you might continue if you use a different logging lib
    return result;
}



// ---------------------------------------------------------------------------
// Name:        appQtApplication::UnhandledExceptionHandler
// Description: This handler will be the application's top-level exception handler
// Arguments:   osExceptionCode exceptionCode - the unhandled exception's code 
//              void* pExceptionContext - the unhandled exception's context
// Return Val:  void
// Author:      Amit Ben-Moshe
// Date:        19/10/2014
// ---------------------------------------------------------------------------
void appQtApplication::UnhandledExceptionHandler(osExceptionCode exceptionCode, void* pExceptionContext)
{
    // Display the exception to the user:
    QIcon iconPixMap;
    acSendErrorReportDialog* pSendErrorReportDialog = new acSendErrorReportDialog(NULL, "Compressonator GUI", iconPixMap);
    GT_IF_WITH_ASSERT(pSendErrorReportDialog != NULL)
    {
        bool allowDifferentSystemPath = false;
#if GR_BUILD_TARGET == GR_WINDOWS_OS
        // On windows, the remote process debugger is used for 64-bit debugging. We should allow the different system file path in this case:
        allowDifferentSystemPath = true;
#endif // GR_BUILD_TARGET == GR_WINDOWS_OS

        // Display the dialog to the user:
        pSendErrorReportDialog->onUnhandledException(exceptionCode, pExceptionContext, allowDifferentSystemPath);

        // Clean up:
        delete pSendErrorReportDialog;

        // Exit the application (this prevents the OS error report dialog from appearing):
        osExitCurrentProcess(0);
    }

    // Inform the global exceptions handler to stop waiting.
    cpUnhandledExceptionHandler::instance().StopWaiting();
}

// ---------------------------------------------------------------------------
void appQtApplication::AppMemAllocFailureHandler()
{
    // First, free the reserved memory so that we don't get stuck.
    gtFreeReservedMemory();

    // Retrieve the call stack for the current thread.
    osCallsStackReader csReader;
    bool isOk = csReader.getCurrentCallsStack(gOutOfMemoryCsBuffer, true, true);
    GT_ASSERT(isOk);

    // emits the signal os it will be handled in the main thread since this can happen in
    // a different thread:
    // We can't use here GT_ASSERT because dialog are not allowed to be displayed here
    if (NULL != gpQtApplicationForEmit)
    {
        gpQtApplicationForEmit->emit AppMemAllocFailureSignal();
    }
}

// ---------------------------------------------------------------------------
void appQtApplication::ClientMemAllocFailureHandler()
{
    // emits the signal os it will be handled in the main thread since this can happen in
    // a different thread:
    // We can't use here GT_ASSERT because dialog are not allowed to be displayed here
    if (NULL != gpQtApplicationForEmit)
    {
        gpQtApplicationForEmit->emit ClientMemAllocFailureSignal();
    }
}

void appQtApplication::initErrorReportDialog()
{
    
#if GR_BUILD_TARGET == GR_WINDOWS_OS
    // On windows, the remote process debugger is used for 64-bit debugging. We should allow the different system file path in this case:
    m_allowDifferentSystemPath = true;
#endif // GR_BUILD_TARGET == GR_WINDOWS_OS

    // Application Send Error Dialog
    QIcon icon("");
    QString title;
    
    title = "Compressonator GUI";
    m_pSendErrorReportDialog = new acSendErrorReportDialog(NULL, title, icon);
   
    m_pSendErrorReportDialog->registerForRecievingDebuggedProcessEvents();
    
}


// ---------------------------------------------------------------------------
void appQtApplication::OnAppMemAllocFailureSignal()
{
    // Warn the user.
    acMessageBox::instance().critical("APP_STR_MEM_ALLOC_FAILURE_HEADLINE", "APP_STR_MEM_ALLOC_FAILURE_EXIT_MSG");

    m_pSendErrorReportDialog->onMemoryAllocationFailure(gOutOfMemoryCsBuffer, m_allowDifferentSystemPath);

    // Exit.
    osExitCurrentProcess(-1);
}

// ---------------------------------------------------------------------------
void appQtApplication::OnClientMemAllocFailureSignal()
{
    acMessageBox::instance().critical("AF_STR_memAllocFailureHeadline", "AF_STR_memAllocFailureClientMsg");
}


