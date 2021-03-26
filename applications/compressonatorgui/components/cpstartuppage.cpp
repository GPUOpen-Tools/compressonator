//=====================================================================
// Copyright 2020 (c), Advanced Micro Devices, Inc. All rights reserved.
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
#include "version.h"
#include <QtWidgets>

#ifndef USE_QTWEBENGINE
#include <QNetworkRequest>
#endif

#include <qfileinfo.h>

// Local
#include "cpstartuppage.h"

#define CP_STR_startup_page_openProjectLink "open_project"
#define CP_STR_startup_page_openProjectLinkPrefix CP_STR_startup_page_openProjectLink "_"
#define CP_STR_startup_page_noRecentProjects "No recent projects"

#ifdef USE_QTWEBENGINE
afWebPage::afWebPage(QObject* pParent)
    : QWebEnginePage(pParent)
{
}

void afWebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID)
{
    (void)(level);
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

bool afWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
    (void)url;
    (void)isMainFrame;

    bool retVal = true;

    // Do not allow navigation clicks:
    if (type == QWebEnginePage::NavigationTypeLinkClicked)
    {
        retVal = false;
    }
    return retVal;
}
#endif

cpStartupPage::cpStartupPage(QWidget* parent)
#ifdef USE_QTWEBENGINE
    : QWebEngineView(parent)
#endif
{
#ifdef USE_QTWEBENGINE
    // Allow focus in this widget:
    setFocusPolicy(Qt::ClickFocus);

    afWebPage* pPage = new afWebPage(this);
    setPage(pPage);
    connect(pPage, SIGNAL(PageButtonClick(QString&, QString&)), this, SLOT(onPageButtonClick(QString&, QString&)));

    // Hide context menu:
    setContextMenuPolicy(Qt::NoContextMenu);
#else
    QString FontFamily = "helvetica";  // "helvetica, arial, sans-serif";
    QString PageListStyle  = {
        "border-style: none;"
        "background-color:#F0F0F0;"
        "color:#551a8b;"
        "QListWidget::item {"
        "border-style: none;"
        "background-color:#F0F0F0;"
        "color:#551a8b;"
        "}"
        "QListWidget::item:selected {"
        "background-color:#F0F0F0;"
        "color:#551a8b;"
        "}"
        };


    QFont   HeaderFont(FontFamily, 14, QFont::Bold, true);
    QFont   Titlefont(FontFamily, 14, QFont::Bold, true);
    QFont   Selectfont(FontFamily, 10, QFont::Bold, false);
    QFont   Messagefont(FontFamily, 10);

    QVBoxLayout* m_Vlayout         = new QVBoxLayout();
    QHBoxLayout* m_HlayoutLinks    = new QHBoxLayout();
    QHBoxLayout* m_HComp           = new QHBoxLayout();
    QHBoxLayout* m_HRecentProjects = new QHBoxLayout();

    QLabel* m_LBlankSpace = new QLabel("", this);
    m_LBlankSpace->setFixedHeight(20);

    QLabel* m_LComp = new QLabel("Compressonator", this);
    m_LComp->setFont(HeaderFont);
    m_LComp->setStyleSheet("color:white; background:qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 1, stop : 0 #1F708F, stop : 1 #000000);");

    m_LComp->setFixedHeight(40);
    m_HComp->addWidget(m_LComp, 0, Qt::AlignTop);

    QHBoxLayout* m_HSampleProjectsAndVersion = new QHBoxLayout();
    Selectfont.setUnderline(true);
    {
        QVBoxLayout* tInnerVB = new QVBoxLayout;
        {
            QLabel* m_LSampleProjects = new QLabel("Sample Projects", this);
            m_LSampleProjects->setFont(Titlefont);
            tInnerVB->addWidget(m_LSampleProjects);

            m_Projectlist.append("bc7_compression");
            m_Projectlist.append("bc6h_compression");
            m_Projectlistview = new QListWidget();
            m_Projectlistview->addItems(m_Projectlist);
            m_Projectlistview->setFont(Selectfont);
            m_Projectlistview->setStyleSheet(PageListStyle);
            m_Projectlistview->setEditTriggers(QAbstractItemView::NoEditTriggers);
            m_Projectlistview->setFixedHeight(100);
            tInnerVB->addWidget(m_Projectlistview);
            connect(m_Projectlistview, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onProjectlistviewClicked(QListWidgetItem*)));

            QLabel* m_LRecentProjects = new QLabel("Recent Projects", this);
            m_LRecentProjects->setFont(Titlefont);
            tInnerVB->addWidget(m_LRecentProjects);

            m_LNoRecentProjects = new QLabel("No recent projects", this);
            m_LNoRecentProjects->setFont(Messagefont);
            tInnerVB->addWidget(m_LNoRecentProjects);
            m_LNoRecentProjects->hide();

            m_RecentProjectlistFullPath.clear();
            m_RecentProjectlist.clear();
            m_RecentProjectsview = new QListWidget();
            m_RecentProjectsview->addItems(m_RecentProjectlist);
            m_RecentProjectsview->setStyleSheet(PageListStyle);
            m_RecentProjectsview->setFont(Selectfont);
            m_RecentProjectsview->setEditTriggers(QAbstractItemView::NoEditTriggers);
            tInnerVB->addWidget(m_RecentProjectsview);
            connect(m_RecentProjectsview, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onRecentProjectsviewClicked(QListWidgetItem*)));
        }

        m_HSampleProjectsAndVersion->addLayout(tInnerVB);

        tInnerVB = new QVBoxLayout;
        {
            QString version = "Version ";
            version.append(VERSION_TEXT_SHORT);
            QLabel* m_LVersion = new QLabel(version, this);
            m_LVersion->setFont(Titlefont);
            tInnerVB->addWidget(m_LVersion);

            m_NewFeatureslist << "New Features";
            m_NewFeaturesview = new QListWidget();
            m_NewFeaturesview->addItems(m_NewFeatureslist);
            m_NewFeaturesview->setFont(Selectfont);
            m_NewFeaturesview->setStyleSheet(PageListStyle);
            m_NewFeaturesview->setEditTriggers(QAbstractItemView::NoEditTriggers);
            tInnerVB->addWidget(m_NewFeaturesview);
            connect(m_NewFeaturesview, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onNewFeaturesviewClicked(QListWidgetItem*)));
        }
        m_HSampleProjectsAndVersion->addLayout(tInnerVB);
    }

    m_PButtonUserGuide         = new QPushButton("User Guide", this);
    QLabel* m_LBar1            = new QLabel("|", this);
    m_PButtonGettingStarted    = new QPushButton("Getting Started", this);
    QLabel* m_LBar2            = new QLabel("|", this);
    m_PButtonCompressonatorWeb = new QPushButton("Compressonator Web Site", this);

    m_LBar1->setFixedWidth(10);
    m_LBar2->setFixedWidth(10);

    m_PButtonUserGuide->setMouseTracking(true);

    m_PButtonUserGuide->setFont(Selectfont);
    m_PButtonGettingStarted->setFont(Selectfont);
    m_PButtonCompressonatorWeb->setFont(Selectfont);
    m_PButtonUserGuide->setStyleSheet("border-style: none;color:#551a8b;");
    m_PButtonGettingStarted->setStyleSheet("border-style: none;color:#551a8b;");
    m_PButtonCompressonatorWeb->setStyleSheet("border-style: none;color:#551a8b;");

    m_HlayoutLinks->addWidget(m_PButtonUserGuide);
    m_HlayoutLinks->addWidget(m_LBar1);
    m_HlayoutLinks->addWidget(m_PButtonGettingStarted);
    m_HlayoutLinks->addWidget(m_LBar2);
    m_HlayoutLinks->addWidget(m_PButtonCompressonatorWeb);

    m_HlayoutLinks->setAlignment(Qt::AlignLeft);

    connect(m_PButtonUserGuide, SIGNAL(pressed()), this, SLOT(onPButtonUserGuide()));
    connect(m_PButtonGettingStarted, SIGNAL(pressed()), this, SLOT(onPButtonGettingStarted()));
    connect(m_PButtonCompressonatorWeb, SIGNAL(pressed()), this, SLOT(onPButtonCompressonatorWeb()));

    m_Vlayout->addLayout(m_HComp);

    m_Vlayout->addWidget(m_LBlankSpace);
    m_Vlayout->addLayout(m_HSampleProjectsAndVersion);
    m_Vlayout->addLayout(m_HlayoutLinks);
    m_Vlayout->addWidget(m_LBlankSpace);

    setLayout(m_Vlayout);

   // parent->setStyleSheet("background-color:rgb(250, 250, 250);border:none;");
#endif
}


#ifndef USE_QTWEBENGINE

void cpStartupPage::onPageButtonClick(QString& Request, QString& Msg)
{
    emit PageButtonClick(Request, Msg);
}

void cpStartupPage::onPButtonUserGuide()
{
    QString Request = "show_help";
    QString Msg     = "";
    emit    PageButtonClick(Request, Msg);
}

void cpStartupPage::onPButtonGettingStarted()
{
    QString Request = "show_quick_start";
    QString Msg     = "";
    emit    PageButtonClick(Request, Msg);
}

void cpStartupPage::onPButtonCompressonatorWeb()
{
    QString Request = "show_website";
    QString Msg     = "";
    emit    PageButtonClick(Request, Msg);
}

void cpStartupPage::onProjectlistviewClicked(QListWidgetItem* item)
{
    QModelIndex index = m_Projectlistview->currentIndex();
    if (index.row() < m_Projectlist.size())
    {
        QString Request = "open_project";
        QString Msg     = m_Projectlist.at(index.row());
        emit    PageButtonClick(Request, Msg);
    }
}

void cpStartupPage::onNewFeaturesviewClicked(QListWidgetItem* item)
{
    QModelIndex index = m_NewFeaturesview->currentIndex();
    if (index.row() < m_NewFeatureslist.size())
    {
        QString Request = "show_newfeatures";
        QString Msg     = m_NewFeatureslist.at(index.row());
        emit    PageButtonClick(Request, Msg);
    }
}

void cpStartupPage::onRecentProjectsviewClicked(QListWidgetItem* item)
{
    QModelIndex index = m_RecentProjectsview->currentIndex();
    if (index.row() < m_RecentProjectlist.size())
    {
        QString Request = "open_project";
        QString Msg     = m_RecentProjectlist.at(index.row());
        emit    PageButtonClick(Request, Msg);
    }
}
#endif

cpStartupPage::~cpStartupPage()
{
}


bool cpStartupPage::UpdateHTML(QVector<QString>& projectsNames)
{
    bool retVal = true;

#ifndef USE_QTWEBENGINE
    m_RecentProjectlist.clear();
    for (int i = 0; i < projectsNames.size(); i++)
    {
        QString rproject = projectsNames.at(i);
        m_RecentProjectlistFullPath.append(rproject);
        QFileInfo filePath(rproject);
        // Get the project name:
        QString wsName = filePath.baseName();
        QString str    = wsName.trimmed();
        if (wsName.size() > 0)
            m_RecentProjectlist.append(wsName);
    }
    m_RecentProjectsview->clear();
    if (m_RecentProjectlist.size() < 1)
        m_LNoRecentProjects->show();
    else
    {
        m_LNoRecentProjects->hide();
        m_RecentProjectsview->addItems(m_RecentProjectlist);
    }

#else
    // Find the right HTML file name:
    QString fileName = "Welcome.html";

    // Get the HTML welcome page from  binary application folder:
    QString compWelcomePagePath;
    QString appDir = qApp->applicationDirPath();
    QString htmlText, cssText;

    // Complete the welcome HTML file path:
#if defined(_WIN32)
    compWelcomePagePath.append("\\WelcomePage\\");
#elif defined(__APPLE__)
    compWelcomePagePath.append("/../Resources/WelcomePage/");
#else
    compWelcomePagePath.append("/WelcomePage/");
#endif
    compWelcomePagePath.append(fileName);

    // Load the file into a QString:
    appDir.append(compWelcomePagePath);
    std::string current_locale_text = appDir.toLocal8Bit().constData();
    QFile       file(appDir);
    bool        rc = file.open(QIODevice::ReadOnly | QIODevice::Text);
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

#ifdef USE_QTWEBENGINE
        // Set the html text:
        setHtml(htmlText, baseUrl);
#endif
    }
#endif
    return retVal;
}

void cpStartupPage::setSource(const QUrl& name)
{
    (name);
    //qDebug() << "cpStartupPage::setSource";
}

bool cpStartupPage::BuildRecentlyOpenedProjectsTable(QString& htmlText, QVector<QString>& recentlyUsedProjectsNames)
{
    bool retVal = false;

#ifndef USE_QTWEBENGINE
    QString recentProjectsTableStr;
    QString appName;

    int numberOfRecentProjects   = (int)recentlyUsedProjectsNames.size();
    int projectsForDisplayNumber = (std::min)(numberOfRecentProjects, 10);
    if (0 < projectsForDisplayNumber)
    {
        for (int i = 0; i < projectsForDisplayNumber; i++)
        {
            // Get the current recent project path:
            QString currentProjectPath = recentlyUsedProjectsNames[i];
            if (QFile(recentlyUsedProjectsNames[i]).exists())
            {
                // Build the file path from the project path:
                QFileInfo filePath(currentProjectPath);

                // Get the project name:
                QString wsName   = filePath.fileName();
                QString tooltip1 = QString("<tr><td><a title=\"");
                tooltip1.append(currentProjectPath);
                tooltip1.append(QString("\" "));
                QString currentProjectCell =
                    tooltip1.append(QString("href=\"\" onclick = 'clickButton(\"" CP_STR_startup_page_openProjectLinkPrefix "%1\")'>%2</a></td></tr>"))
                        .arg(wsName)
                        .arg(wsName);
                recentProjectsTableStr.append(currentProjectCell);
            }
        }
    }
    else
    {  // 0 >= numberOfRecentProjects
        recentProjectsTableStr.append("<p class='indentlargetext'>" CP_STR_startup_page_noRecentProjects "</p>");
    }

    // Find the position of the recent projects table in the original HTML text:
    int tableHeadPos = htmlText.indexOf("<table id=\"recent_table\">", 0);
    if (tableHeadPos > 0)
    {
        // Find the position of the table body:
        int tableBodyStartPos = htmlText.indexOf("<tbody>", tableHeadPos + 1);
        int tableBodyEndPos   = htmlText.indexOf("</tbody>", tableBodyStartPos + 1);

        if ((tableBodyStartPos > 0) && (tableBodyEndPos > 0) && (tableBodyEndPos > tableBodyStartPos))
        {
            // Get the length of the replaced string:
            int len  = tableBodyEndPos - tableBodyStartPos;
            htmlText = htmlText.replace(tableBodyStartPos + 8, len, recentProjectsTableStr);
            retVal   = true;
        }
    }
#endif
    return retVal;
}

bool cpStartupPage::CanLinkBeClicked(const QUrl& url)
{
    Q_UNUSED(url);
    bool retVal = true;
    return retVal;
}
