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

#ifndef __CPSTARTUPPAGE_H
#define __CPSTARTUPPAGE_H

#include <QWebView>
#include <QWebPage>

// Infra:
#include <AMDTOSWrappers/Include/osFilePath.h>
// Infra:
#include <AMDTBaseTools/Include/gtAssert.h>
#include <AMDTBaseTools/Include/gtVector.h>
#include <AMDTOSWrappers/Include/osDebuggingFunctions.h>
#include <AMDTOSWrappers/Include/osFile.h>
#include <AMDTOSWrappers/Include/osFileLauncher.h>
#include <AMDTAPIClasses/Include/apFrameTerminators.h>
#include <AMDTApplicationComponents/Include/acColours.h>
#include <AMDTApplicationComponents/Include/acFunctions.h>
#include <AMDTApplicationComponents/Include/acMessageBox.h>



// ----------------------------------------------------------------------------------
// Class Name:          afWebPage: public QWebPage
// General Description: Inherit QWebPage. We need this class to override the JavaScript
//                      events since in the current HTML design, we need to use JS to 
//                      execute CodeXL actions
// Author:              Sigal Algranaty
// Creation Date:       23/9/2014
// ----------------------------------------------------------------------------------
class afWebPage : public QWebPage
{
    Q_OBJECT

public:

    // Constructor:
    afWebPage(QObject* pParent = nullptr);

Q_SIGNALS:
    void PageButtonClick(QString &Request, QString &Msg);



protected:
    // Overrides QWebPage: is used for catching requests from welcome page, and implement in Qt:
    virtual void javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID);
    virtual bool acceptNavigationRequest(QWebFrame * frame, const QNetworkRequest & request, QWebPage::NavigationType type);
};

// ----------------------------------------------------------------------------------
// Class Name:          afStartupPage: public QWebView
// General Description: Inherit QWebView and is used for displaying the CodeXL HTML welcome page
// Author:              Sigal Algranaty
// Creation Date:       23/9/2014
// ----------------------------------------------------------------------------------
class cpStartupPage : public QWebView
{
    Q_OBJECT

public:

    virtual ~cpStartupPage();
    cpStartupPage(QWidget * parent);

    // Update from current recent projects:
    bool UpdateHTML(gtVector<gtString>& projectsNames);

Q_SIGNALS:
    void PageButtonClick(QString &Request, QString &Msg);

public slots:
    virtual void setSource(const QUrl& name);
    void onPageButtonClick(QString &Request, QString &Msg);

protected:

    bool CanLinkBeClicked(const QUrl& url);

protected slots:

    /// Slot handling the link clicked signal. Will open the links in an external window:
    virtual void OnLinkClicked(const QUrl& link);

    /// Build the recently opened projects table, and replace it in the HTML text:
    /// \param htmlText the loaded HTML text (should contain dummy table for replacement)
    /// \return true for success (the text contain the expected table)
    bool BuildRecentlyOpenedProjectsTable(QString& htmlText, gtVector<gtString>& recentlyUsedProjectsNames);
};

#endif //__CPSTARTUPPAGE_H

