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

#ifndef __CPUNHANDLEDEXCEPTIONHANDLER_H
#define __CPFUNHANDLEDEXCEPTIONHANDLER_H

// Qt:
#include <QtWidgets>

// Infra:
#include <AMDTOSWrappers/Include/osUnhandledExceptionHandler.h>

// ----------------------------------------------------------------------------------
// Class Name:           GD_API cpUnhandledExceptionHandler : public osUnhandledExceptionHandler
// General Description:
//  Catches unhandles exceptions that should of cause the CodeXL application to crash
//  and enables the user to report them to the CodeXL development team.
//
// Author:               Yaki Tebeka
// Creation Date:        21/4/2009
// ----------------------------------------------------------------------------------
class cpUnhandledExceptionHandler : public QObject, public osUnhandledExceptionHandler
{
    Q_OBJECT

public:
    virtual ~cpUnhandledExceptionHandler();
    static cpUnhandledExceptionHandler& instance();

    // After this object emits the UnhandledExceptionSignal, it waits until another object
    // tells it to stop waiting. This mechanism allows us to handle scenarios where the uncaught
    // exception is thrown in a thread other than the main thread. In such scenarios, the main
    // thread (which handles the UnhandledExceptionSignal) opens up an error report dialog and 
    // interacts with the user. This object, which might be running on a thread other than the
    // main thread, must allow the main thread to handle the queued event, and must not exit
    // before the main thread has finished interacting with the user.This function allows the
    // main thread to inform the other thread that it can and should now exit.
    void StopWaiting();

signals:
    void UnhandledExceptionSignal(osExceptionCode exceptionCode, void* pExceptionContext);

protected:
    virtual void onUnhandledException(osExceptionCode exceptionCode, void* pExceptionContext);

private:

    // Only my instance method should create me:
	cpUnhandledExceptionHandler();
    bool m_shouldStopWaiting;

};

#endif //__cpUnhandledExceptionHandler_H

