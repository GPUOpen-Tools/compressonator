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

#include <QtWidgets>
#include <QNetworkRequest>
#include <qfileinfo.h>

// Local
#include "cpStartupPage.h"

#define CP_STR_startup_page_openProjectLink                         "open_project"
#define CP_STR_startup_page_openProjectLinkPrefix                   CP_STR_startup_page_openProjectLink "_"
#define CP_STR_startup_page_noRecentProjects                        "No recent projects"

afWebPage::afWebPage(QObject* pParent) : QWebPage(pParent)
{
}

void afWebPage::javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID)
{
    (void)(sourceID);
    (void)(lineNumber);

    // This function is called whenever the buttonClick implemented in JS is called in the welcome page HTML.
    // Check the message and perform the action requested by the user
    //qDebug() << message;
    QString Request;
    QString Msg = message;

    if (message.contains(CP_STR_startup_page_openProjectLinkPrefix))
    {
        Request = CP_STR_startup_page_openProjectLink;
        Msg.remove(CP_STR_startup_page_openProjectLinkPrefix);
    }
    else
        Request = message;

    emit PageButtonClick(Request, Msg);

}

bool afWebPage::acceptNavigationRequest(QWebFrame * frame, const QNetworkRequest & request, QWebPage::NavigationType type)
{
    GT_UNREFERENCED_PARAMETER(frame);
    GT_UNREFERENCED_PARAMETER(request);
    bool retVal = true;

    // Do not allow navigation clicks:
    if (type == QWebPage::NavigationTypeLinkClicked)
    {
        retVal = false;
    }
    return retVal;
}

// ---------------------------------------------------------------------------
// Name:        afStartupPage::afStartupPage
// Description: Constructor
// Arguments:   QWidget* pParent
// Author:      Sigal Algranaty
// Date:        21/2/2012
// ---------------------------------------------------------------------------
cpStartupPage::cpStartupPage(QWidget * parent) : QWebView(parent)
{
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    bool rc = connect(this, SIGNAL(linkClicked(const QUrl&)), this, SLOT(OnLinkClicked(const QUrl&)));
    GT_ASSERT(rc);

    // Allow focus in this widget:
    setFocusPolicy(Qt::ClickFocus);

    afWebPage* pPage = new afWebPage(this);
    setPage(pPage);
    rc = connect(pPage, SIGNAL(PageButtonClick(QString &, QString &)), this, SLOT(onPageButtonClick(QString &, QString &)));
    GT_ASSERT(rc);

    // Hide context menu:
    setContextMenuPolicy(Qt::NoContextMenu);
}

void cpStartupPage::onPageButtonClick(QString &Request, QString &Msg)
{
    emit PageButtonClick(Request, Msg);
}

// ---------------------------------------------------------------------------
// Name:        afStartupPage::~afStartupPage
// Description: Destructor
// Author:      Sigal Algranaty
// Date:        22/2/2012
// ---------------------------------------------------------------------------
cpStartupPage::~cpStartupPage()
{

}

// ---------------------------------------------------------------------------
// Name:        afStartupPage::updateHTML
// Description: Update the HTML text with the current recently used projects
// Return Val:  bool - Success / failure.
// Author:      Sigal Algranaty
// Date:        21/2/2012
// ---------------------------------------------------------------------------
bool cpStartupPage::UpdateHTML(gtVector<gtString>& projectsNames)
{
    bool retVal = true;

    // Find the right HTML file name:
    QString fileName = "Welcome.html";

    // Get the HTML welcome page from  binary application folder:
    osFilePath codeXLWelcomePagePath;
    QString appDir = qApp->applicationDirPath();
    QString htmlText, cssText, htmlFilePath;
   
    // Complete the welcome HTML file path:
    codeXLWelcomePagePath.appendSubDirectory(L"WelcomePage");
    codeXLWelcomePagePath.setFileName(acQStringToGTString(fileName));

    // Load the file into a QString:
    gtString fileNameWithExt = codeXLWelcomePagePath.asString();
    htmlFilePath = acGTStringToQString(fileNameWithExt);
    appDir.append(htmlFilePath);

    QFile file(appDir);
    bool rc = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (rc)
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            htmlText.append(line);
        }
        file.close();

        // Build the recent projects table:
        BuildRecentlyOpenedProjectsTable(htmlText, projectsNames);

        // Find the base URL (to enable links):
        QUrl baseUrl = QUrl::fromLocalFile(appDir);

        // Set the html text:
        setHtml(htmlText, baseUrl);
    }

    return retVal;
}

// ---------------------------------------------------------------------------
// Name:        afStartupPage::onLinkClick
// Description: Implement the startup page click event
// Return Val:  void
// Author:      Sigal Algranaty
// Date:        21/2/2012
// ---------------------------------------------------------------------------
void cpStartupPage::OnLinkClicked(const QUrl& url)
{
    Q_UNUSED(url);
}

// ---------------------------------------------------------------------------
// Name:        afStartupPage::setSource
// Description: Prevent Qt from activating links
// Arguments:   const QUrl & name
// Author:      Sigal Algranaty
// Date:        28/2/2012
// ---------------------------------------------------------------------------
void cpStartupPage::setSource(const QUrl& name)
{
    GT_UNREFERENCED_PARAMETER(name);
    //qDebug() << "cpStartupPage::setSource";
}


// ---------------------------------------------------------------------------
// Name:        afStartupPage::BuildRecentlyOpenedProjectsTable
// Description: Append a table with the recently opened projects
// Arguments:   QString& tableStr
// Author:      Sigal Algranaty
// Date:        28/2/2012
// ---------------------------------------------------------------------------
bool cpStartupPage::BuildRecentlyOpenedProjectsTable(QString& htmlText, gtVector<gtString>& recentlyUsedProjectsNames)
{
    bool retVal = false;


    QString recentProjectsTableStr;
    gtString appName;

    int numberOfRecentProjects = (int)recentlyUsedProjectsNames.size();
    int projectsForDisplayNumber = min(numberOfRecentProjects, 5);
    if (0 < projectsForDisplayNumber)
    {
        for (int i = 0; i < projectsForDisplayNumber; i++)
        {
            // Get the current recent project path:
            gtString currentProjectPath = recentlyUsedProjectsNames[i];
            if (QFile(acGTStringToQString(recentlyUsedProjectsNames[i])).exists())
            {
                // Build the file path from the project path:
                osFilePath filePath(currentProjectPath);

                // Get the project name:
                gtString currentProjectName;
                filePath.getFileName(currentProjectName);
                QString wsName = acGTStringToQString(currentProjectName);
                QString tooltip1 = QString("<tr><td><a title=\"");
                tooltip1.append(acGTStringToQString(currentProjectPath));
                tooltip1.append(QString("\" "));
                QString currentProjectCell = tooltip1.append(QString("href=\"\" onclick = 'clickButton(\"" CP_STR_startup_page_openProjectLinkPrefix "%1\")'>%2</a></td></tr>")).arg(wsName).arg(wsName);
                recentProjectsTableStr.append(currentProjectCell);
            }
        }
    }
    else // 0 >= numberOfRecentProjects
    {
        recentProjectsTableStr.append("<p class='indentlargetext'>" CP_STR_startup_page_noRecentProjects "</p>");
    }

    // Find the position of the recent projects table in the original HTML text:
    int tableHeadPos = htmlText.indexOf("<table id=\"recent_table\">", 0);
    if (tableHeadPos > 0)
    {
        // Find the position of the table body:
        int tableBodyStartPos = htmlText.indexOf("<tbody>", tableHeadPos + 1);
        int tableBodyEndPos = htmlText.indexOf("</tbody>", tableBodyStartPos + 1);

        GT_IF_WITH_ASSERT((tableBodyStartPos > 0) && (tableBodyEndPos > 0) && (tableBodyEndPos > tableBodyStartPos))
        {
            // Get the length of the replaced string:
            int len = tableBodyEndPos - tableBodyStartPos;
            htmlText = htmlText.replace(tableBodyStartPos + 8, len, recentProjectsTableStr);
            retVal = true;
        }
    }

    return retVal;
}


// ---------------------------------------------------------------------------
// Name:        afStartupPage::canLinkBeClicked
// Description: If the operation requested by url requires stop debugging, ask
//              the user how to behave
// Arguments:   const QUrl& url
// Return Val:  bool - true if operation can continue (debug/profile is stopped or wasn't on), false otherwise
// Author:      Sigal Algranaty
// Date:        27/3/2012
// ---------------------------------------------------------------------------
bool cpStartupPage::CanLinkBeClicked(const QUrl& url)
{
    Q_UNUSED(url);
    bool retVal = true;
    return retVal;
}


