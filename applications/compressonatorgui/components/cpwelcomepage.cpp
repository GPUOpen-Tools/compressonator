//=====================================================================
// Copyright 2016-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>
#include <QDesktopServices>
#include "cpwelcomepage.h"

CWelcomePage::CWelcomePage(const QString title, QWidget* parent)
    : acCustomDockWidget(title, parent)
    , m_startUpPage(nullptr)
    , custTitleBar(nullptr)
    , m_newWidget(nullptr)
    , m_layout(nullptr)
    , m_title(title)
    , m_parent(parent)
{
    custTitleBar = new acDockWidgetTitlebar(this);
    custTitleBar->setTitle(title);
    custTitleBar->setButtonToolBarEnabled(false);
    setTitleBarWidget(custTitleBar);

    m_newWidget   = new QWidget(parent);
    m_startUpPage = new cpStartupPage(this);

    connect(m_startUpPage, SIGNAL(PageButtonClick(QString&, QString&)), this, SLOT(onWebPageButtonClick(QString&, QString&)));

    m_layout = new QGridLayout(m_newWidget);
    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    if (m_startUpPage)
    {
        m_layout->addWidget(m_startUpPage, 0, 0);
    }

    m_newWidget->setLayout(m_layout);

    setWidget(m_newWidget);
}

void CWelcomePage::GoToAMDHomePage(QVector<QString>& projectsNames)
{
    if (m_startUpPage)
    {
        m_startUpPage->UpdateHTML(projectsNames);
    }
}

CWelcomePage::~CWelcomePage()
{
}

void CWelcomePage::onWebPageButtonClick(QString& Request, QString& Msg)
{
    bool requestContinueToApp = false;

    // skip unsupported requests
    // and process local web page request

    if (Request.compare("open_project") == 0)
        requestContinueToApp = true;
    else if (Request.compare("new_project") == 0)
        requestContinueToApp = true;
    else if (Request.compare("show_quick_start") == 0)
        requestContinueToApp = true;
    else if (Request.compare("show_help") == 0)
        requestContinueToApp = true;
    else if (Request.compare("show_newfeatures") == 0)
        requestContinueToApp = true;
    else if (Request.compare("show_website") == 0)
    {
        QDesktopServices::openUrl(QUrl(COMPRESSONATOR_HOME));
    }

    if (requestContinueToApp)
        emit WebPageButtonClick(Request, Msg);
}
