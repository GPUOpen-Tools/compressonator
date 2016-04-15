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

#ifndef __APPQTAPPLICATION_H
#define __APPQTAPPLICATION_H

// Qt:
#include <QApplication>

// Compiler warnings:
#include <CXLBaseTools/Include/gtIgnoreCompilerWarnings.h>

// Qt:
#include <QtWidgets>

// Infra:
#include <CXLBaseTools/Include/gtAssert.h>
#include <CXLOSWrappers/Include/osDebugLog.h>
#include <CXLOSWrappers/Include/osCallStack.h>
#include <CXLOSWrappers/Include/osCallsStackReader.h>
#include <CXLOSWrappers/Include/osProcess.h>
#include <CXLApplicationComponents/Include/acMessageBox.h>
#include <CXLApplicationComponents/Include/acSendErrorReportDialog.h>

// Components:
#include <CXLApplicationComponents/Include/acSendErrorReportDialog.h>

// Declare osExceptionCode as a meta types for Qt to be able to queue its instances:
Q_DECLARE_METATYPE(osExceptionCode);

// ----------------------------------------------------------------------------------
// Class Name:          appQtApplication : public QApplication
// General Description: inherits the QApplication to enable overwriting virtual functions
//
// Author:                Gilad Yarnitzky
// Creation Date(DD/MM/YYYY):        30/5/2012
// ----------------------------------------------------------------------------------
class appQtApplication : public QApplication
{
    Q_OBJECT

public:
    appQtApplication(int& argc, char** argv);
    virtual ~appQtApplication();

    virtual bool notify(QObject* pReceiver, QEvent* pEvent);

    /// Static function for handling the out of memory. the set_new_handler handler
    static void AppMemAllocFailureHandler();

    /// Static function for handling client the out of memory. the set_new_handler handler
    static void ClientMemAllocFailureHandler();

Q_SIGNALS:
    /// App memory allocation failure signal:
    void AppMemAllocFailureSignal();

    /// client memory allocation failure signal:
    void ClientMemAllocFailureSignal();

public slots:
    void UnhandledExceptionHandler(osExceptionCode exceptionCode, void* pExceptionContext);

    // Handle the signal for AppMemAllocFailureSignal
    void OnAppMemAllocFailureSignal();

    // Handle the client ClientMemAllocFailureSignal
    void OnClientMemAllocFailureSignal();

    void initErrorReportDialog();

private:
    bool m_allowDifferentSystemPath;
    // Error report
    acSendErrorReportDialog* m_pSendErrorReportDialog;

};


#endif // __APPQTAPPLICATION_H
