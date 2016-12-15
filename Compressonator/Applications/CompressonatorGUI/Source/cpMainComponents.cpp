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

#include "cpMainComponents.h"
#include "Version.h"

static signalMsgHandler static_msghandler;
int g_OpenGLMajorVersion = 0;
int g_OpenGLMinorVersion = 0;

#define STR_WELCOME_PAGE    "Welcome Page"

C_Application_Options         g_Application_Options;

//=========================================================
cpMainComponents::cpMainComponents(QDockWidget *root_dock, QMainWindow *parent)
    : QMainWindow(parent)
{
    if (parent == NULL)
        m_parent = this;
    else
        m_parent        = parent;

    //============================================
    fileMenu                = NULL;
    helpMenu                = NULL;
    fileToolBar             = NULL;
    CompressionToolBar      = NULL;
    newProjectAct           = NULL;
    saveToBatchFileAct      = NULL;
    openAct                 = NULL;
    saveAct                 = NULL;
    saveAsAct                = NULL;
    openImageFileAct        = NULL;
    exportAct               = NULL;
    exitAct                 = NULL;
    userGuideAct            = NULL;
    gettingStartedAct       = NULL;
    aboutAct                = NULL;
    settingsAct             = NULL;
    aboutQtAct              = NULL;
    compressAct             = NULL;
    imagediffAct            = NULL;
    MIPGenAct               = NULL;
    deleteImageAct          = NULL;
    closeAllDocuments       = NULL;
    showOutputAct           = NULL;
    showWelcomePageAct      = NULL;
    windowMenu              = NULL;
    isCompressInProgress    = false;
    m_ForceImageRefresh     = false;


#ifdef USE_MAINA_IMAVEVIEW_TOOLBAR
    ImageViewToolBar        = NULL;
    imageview_zoomInAct     = NULL;
    imageview_zoomOutAct    = NULL;
    imageview_RedAct        = NULL;
    imageview_GreenAct      = NULL;
    imageview_BlueAct       = NULL;
    imageview_AlphaAct      = NULL;
    imageview_FitScreenAct  = NULL;
#endif

    m_welcomePage           = NULL;
    m_imagePropertyView     = NULL;
    m_genmips               = NULL;
    m_setcompressoptions    = NULL;
    m_projectview           = NULL;
    m_imageview             = NULL;
    m_imageCompare          = NULL;
    m_activeImageTab        = NULL;
    app_welcomepage         = NULL;
    m_frame                 = NULL;

    m_numRecentFiles        = 0;
    m_projectsRecentFiles.clear();

    m_showAppSettingsDialog = true;
    m_viewDiff = false;
    m_sSettingsFile = "CompressSettings.ini";

    //------------------------------------------------------------------
    // find an alternative way to get the version or call glFunctions!
    // look into using 
    //------------------------------------------------------------------

    //Reserved: GPUDecode

    if (m_showAppSettingsDialog)
    {
        m_setapplicationoptions = new CSetApplicationOptions("Application Settings", this);
        m_setapplicationoptions->hide();
    }


    //============================================
    app_welcomepage = root_dock;

    m_parent->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);


    //
    m_blankpage = new acCustomDockWidget("", this);
    m_blankpage->setTitleBarWidget(NULL);
    m_blankpage->m_fileName = "";
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    m_blankpage->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_parent->addDockWidget(Qt::RightDockWidgetArea, m_blankpage);
    if (m_blankpage->custTitleBar)
    {
        m_blankpage->custTitleBar->setButtonCloseEnabled(false);
        m_blankpage->custTitleBar->setButtonToolBarShow(false);
        m_blankpage->lower();
        //m_blankpage->setStyleSheet("QTabWidget::tab:disabled { width: 0; height: 0; margin: 0; padding: 0; border: none; }");
        // setTabEnabled(false);
    }

    // Status View when compressing
    m_CompressStatusDialog = new CompressStatusDialog("Output", this);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);
    m_CompressStatusDialog->resize(300, 290);
    m_parent->addDockWidget(Qt::BottomDockWidgetArea, m_CompressStatusDialog);
    m_CompressStatusDialog->hideOutput();

    connect(&static_msghandler, SIGNAL(signalMessage(const char *)), this, SLOT(browserMsg(const char *)));

    m_projectview = new ProjectView("Project", m_CompressStatusDialog, this);
    m_projectview->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_projectview->resize(300, 500);
    m_projectview->setMaximumWidth(300);
    m_projectview->setMinimumWidth(200);
    m_parent->addDockWidget(Qt::LeftDockWidgetArea, m_projectview);

    QString tempSetting = m_sSettingsFile;
    QFileInfo fileInfo(tempSetting);
    if (!fileInfo.isWritable())
    {
        QFileInfo fileInfo2(m_projectview->m_curProjectFilePathName);
        m_sSettingsFile = fileInfo2.dir().path();
        m_sSettingsFile.append(QDir::separator());
        m_sSettingsFile.append(tempSetting);
        m_sSettingsFile.replace("/", "\\");
    }
    //
    m_imagePropertyView = new CImagePropertyView("  Properties", this);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    m_imagePropertyView->resize(300, 290);
    m_parent->addDockWidget(Qt::LeftDockWidgetArea, m_imagePropertyView);


    m_welcomePage = new CWelcomePage(STR_WELCOME_PAGE, this);
    m_welcomePage->m_fileName = "";
    m_parent->tabifyDockWidget(m_blankpage, m_welcomePage);
    m_welcomePage->resize(600,400);
    if (m_welcomePage->custTitleBar)
        m_welcomePage->custTitleBar->setTitle(STR_WELCOME_PAGE);
    m_welcomePage->setAllowedAreas(Qt::RightDockWidgetArea);
    connect(m_welcomePage, SIGNAL(WebPageButtonClick(QString &, QString &)), this, SLOT(OnWelcomePageButtonClick(QString &, QString &)));

    // Get the blank page tab and set disable property to hide it
    QTabBar *tabBar = this->findChild<QTabBar *>();
    if (tabBar)
    {
        tabBar->setStyleSheet("QTabBar::tab:disabled { width: 0; height: 0; margin: 0; padding: 0; border: none; }");
        tabBar->setTabEnabled(0, false);
    }

    m_setcompressoptions = new CSetCompressOptions("Destination Setting", this);
    m_setcompressoptions->hide();

    connect(m_projectview, SIGNAL(ViewImageFile(QString &, QTreeWidgetItem *)), this, SLOT(AddImageView(QString &, QTreeWidgetItem *)));
    connect(m_projectview, SIGNAL(ViewImageFileDiff(C_Destination_Options *)), this, SLOT(AddImageDiff(C_Destination_Options *)));
    connect(m_projectview, SIGNAL(DeleteImageView(QString &)), this, SLOT(OnDeleteImageView(QString &)));
    connect(m_projectview, SIGNAL(UpdateData(QObject *)), m_imagePropertyView, SLOT(OnUpdateData(QObject *)));
    connect(m_projectview, SIGNAL(AddCompressSettings(QTreeWidgetItem *)), this, SLOT(OnAddCompressSettings(QTreeWidgetItem *)));
    connect(m_projectview, SIGNAL(EditCompressSettings(QTreeWidgetItem *)), this, SLOT(onEditCompressSettings(QTreeWidgetItem *)));

    connect(m_projectview, SIGNAL(OnAddedCompressSettingNode()), this, SLOT(onAddedCompressSettingNode()));


    connect(m_projectview, SIGNAL(OnCompressionStart()), this, SLOT(onCompressionStart()));
    connect(m_projectview, SIGNAL(OnCompressionDone()),  this, SLOT(onCompressionDone()));

    connect(m_projectview, SIGNAL(OnCompressionStart()), m_imagePropertyView, SLOT(onCompressionStart()));
    connect(m_projectview, SIGNAL(OnProcessing(QString &)), this, SLOT(onProcessing(QString &)));
    connect(m_projectview, SIGNAL(OnCompressionDone()), m_imagePropertyView, SLOT(onCompressionDone()));

    connect(m_projectview, SIGNAL(OnSourceImage(int)), this, SLOT(onSourceImage(int)));
    connect(m_projectview, SIGNAL(OnDecompressImage()) , this, SLOT(onDecompressImage()));

    connect(m_projectview, SIGNAL(OnSourceImage(int)), m_imagePropertyView, SLOT(onSourceImage(int)));
    connect(m_projectview, SIGNAL(OnProjectLoaded(int)), this, SLOT(onProjectLoaded(int)));


    connect(this, SIGNAL(OnImageLoadStart()), m_projectview, SLOT(onImageLoadStart()));
    connect(this, SIGNAL(OnImageLoadDone()),  m_projectview, SLOT(onImageLoadDone()));
    connect(this, SIGNAL(OnImageLoadStart()), m_imagePropertyView, SLOT(onImageLoadStart()));
    connect(this, SIGNAL(OnImageLoadDone()),  m_imagePropertyView, SLOT(onImageLoadDone()));



#ifdef USE_MSGHANDLER
    connect(&static_msghandler, SIGNAL(signalMessage(const char *)), m_projectview, SLOT(OnGlobalMessage(const char *)));
#endif

    connect(this, SIGNAL(SetCurrentItem(QString &)), m_projectview, SLOT(onSetCurrentItem(QString &)));
    connect(m_setcompressoptions, SIGNAL(SaveCompressSettings(QTreeWidgetItem *, C_Destination_Options &)), this, SLOT(AddImageCompSettings(QTreeWidgetItem *, C_Destination_Options &)));

    // QRect scr = QApplication::desktop()->screenGeometry();
    // 
    m_genmips = new CGenMips("Generate MIP Maps",NULL);
    m_genmips->hide();
    connect(m_genmips, SIGNAL(generateMIPMap(int)), this, SLOT(onGenerateMIPMap(int)));
        
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    
    readSettings();

    // Set some global setting 
    #ifdef  ENABLED_USER_GPUVIEW
    g_useCPUDecode = (g_Application_Options.m_ImageViewDecode == g_Application_Options.ImageEncodeDecodeWith::CPU);
    #else
    g_useCPUDecode = true;
    #endif
#ifdef USE_COMPUTE
    g_useCPUEncode = g_Application_Options.m_ImageEncode == g_Application_Options.ImageEncodeDecodeWith::CPU;
#endif
    setUnifiedTitleAndToolBarOnMac(true);

    if (m_showAppSettingsDialog)
    {
        // Act on read settings for application startup
        if (g_Application_Options.m_loadRecentFile)
        {
            if (m_numRecentFiles > 0)
            {
                recentFileActs[0]->trigger();
            }
        }
    }

    m_welcomePage->GoToAMDHomePage(m_projectsRecentFiles);

    //=============================
    // Help About
    //=============================
    m_pacHelpAboutDialog = new CHelpAboutDialog(this);
    // Sett current project name on app title bar
    SetProjectWindowTitle();

    // Get the product version:
    m_apptitle = "Compressonator";
   
    QString ver  = QString("%1.%2.%3.%4").arg(
        QString::number(VERSION_MAJOR_MAJOR),
        QString::number(VERSION_MAJOR_MINOR),
        QString::number(VERSION_MINOR_MAJOR),
        QString::number(0)
        );

    // Compression Connections

    connect(m_imagePropertyView, SIGNAL(saveSetting(QString *)), this, SLOT(onPropertyViewSaveSetting(QString *)));
    connect(m_imagePropertyView, SIGNAL(compressImage(QString *)), this, SLOT(onPropertyViewCompressImage(QString *)));


}

void cpMainComponents::SetProjectWindowTitle()
{
    if (m_projectview)
    {
        setWindowTitle(m_projectview->m_curProjectName);
    }
}

void cpMainComponents::OnWelcomePageButtonClick(QString &Request, QString &Msg)
{
    #define PROJECT_DIR "/Projects/"

    if (Request.compare("new_project") == 0)
    {
        openNewProject();
    }
    else
    // qDebug() << Request << " Msg: " << Msg;
    if (Request.compare("open_project") == 0)
    {

        if (!m_projectview) return;
        if (!m_projectview->userSaveProjectAndContinue()) return;


        bool found = false;
        if(Msg.indexOf(".cprj")==-1)
            Msg.append(PROJECT_EXTENSION);

        // Try the Current Path
        if (!found)
        {
            QString ProjectFile = QDir::currentPath();
            ProjectFile.append(PROJECT_DIR);
            ProjectFile.append(Msg);
            QFile Fout(ProjectFile);
            if (Fout.exists())
            {
                m_projectview->loadProjectFile(ProjectFile);
                found = true;
            }
        }

        // Try the Application Dir
        if (!found)
        {
            // First Try looking for the project in sample folder
            QString ProjectFile = qApp->applicationDirPath();
            ProjectFile.append(PROJECT_DIR);
            ProjectFile.append(Msg);
            QFile Fout(ProjectFile);
            if (Fout.exists())
            {
                m_projectview->loadProjectFile(ProjectFile);
                found = true;
            }
        }

        // Try the Working Dir
        if (!found)
        {
            // This is windows specific!
            QString pwd("");
            char * ENV;
            // Check if user set our envniornment var
            ENV = getenv(ENV_COMPRESSONATOR_ROOT);
            if (ENV)
                pwd.append(ENV);
            else
            {   // check Pathname of the current working dir
                ENV = getenv("PWD");
            }
            if (ENV)
            {
                pwd.append(PROJECT_DIR);
                pwd.append(Msg);
                QFile Fout(pwd);
                if (Fout.exists())
                {
                    m_projectview->loadProjectFile(pwd);
                    found = true;
                }
            }
        }

        // Try Recent Files
        if (!found)
        {
            for (int i = 0; i < m_numRecentFiles; i++)
            {
                // The list is in &%1 %2 format so the first 3 char are removed
                // Our max recents is 5 which is 1 char size
                QString proj = recentFileActs[i]->text();
                proj.remove(0, 3);
                if (Msg.compare(proj) == 0)
                {
                    recentFileActs[i]->trigger();
                    found = true;
                    break;
                }
            }
        }

        if (found)
            setCurrentFile(m_projectview->m_curProjectFilePathName);
    }
    else
    if (Request.compare("show_quick_start") == 0)
    {
        OpenCHMFile(COMPRESSONATOR_GETTING_STARTED);
    }
    else
    if (Request.compare("show_help") == 0)
    {
        OpenCHMFile(COMPRESSONATOR_USER_GUIDE);
    }
}

void cpMainComponents::closeEvent(QCloseEvent *event)
{
    if (m_projectview)
    {
        if (!m_projectview->userSaveProjectAndContinue())
        {
            event->ignore();
            return;
        }
        setCurrentFile(m_projectview->m_curProjectFilePathName);
        m_projectview->clearProjectTreeView();
    }

   writeSettings();
   qApp->quit();
   event->accept();
}

void cpMainComponents::openProjectFile()
{
    if (m_projectview)
    {
        m_projectview->openProjectFile();
        setCurrentFile(m_projectview->m_curProjectFilePathName);
    }
}

void cpMainComponents::openNewProject()
{
    if (m_projectview)
    {
        m_projectview->openNewProjectFile();
        setCurrentFile(m_projectview->m_curProjectFilePathName);
    }
}

bool cpMainComponents::saveProjectToBatchFile()
{
    if (m_projectview)
    {
        m_projectview->saveToBatchFile();
    }
    return true;
}

void cpMainComponents::openImageFile()
{
    if (m_projectview)
    {
        m_projectview->OpenImageFile();
    }
}

void cpMainComponents::deleteImageFile()
{
    if (m_projectview)
    {
        m_projectview->UserDeleteItems();
    }
}

bool cpMainComponents::saveProjectFile()
{
    if (m_projectview)
    {
        m_projectview->saveProjectFile();
        setCurrentFile(m_projectview->m_curProjectFilePathName);
    }
    return true;
}

bool cpMainComponents::saveAsProjectFile()
{
    if (m_projectview)
    {
        m_projectview->saveAsProjectFile();
        setCurrentFile(m_projectview->m_curProjectFilePathName);
    }
    return true;
}

void cpMainComponents::settings()
{
    if (m_showAppSettingsDialog)
    {
        if (m_setapplicationoptions)
        {
            m_setapplicationoptions->UpdateViewData();
            m_setapplicationoptions->show();
            m_setapplicationoptions->raise();
        }
    }
}

void cpMainComponents::about()
{
    if (m_pacHelpAboutDialog)
        m_pacHelpAboutDialog->show();
}


void cpMainComponents::onShowWelcomePage()
{
    if (m_welcomePage)
    {
        m_welcomePage->show();
        m_welcomePage->raise();
    }
}

void cpMainComponents::onShowOutput()
{
   if (m_CompressStatusDialog)
   {
       m_CompressStatusDialog->showOutput();
       m_CompressStatusDialog->raise();
   }
}


void cpMainComponents::onCloseAllDocuments()
{
    QList<acCustomDockWidget *> dockWidgets = m_parent->findChildren<acCustomDockWidget *>();
    QListIterator<acCustomDockWidget *> iter(dockWidgets);
    acCustomDockWidget *dock;
    int i = 0;
    while (iter.hasNext())
    {
        dock = iter.next();
        i++;
        if (dock)
        {
            QString FileName = dock->m_fileName;
            if (FileName.size() > 0)
            {
                dock->close();
                if (dock)
                {
                    if (dock->custTitleBar)
                    {
                        // we have a image diff with 3 other sub classes of type acCustomDockWidget
                        // that will be deleted.
                        if (dock->custTitleBar->m_close)
                        {
                            if (iter.hasNext())
                                iter.next();
                            if (iter.hasNext())
                                iter.next();
                            if (iter.hasNext())
                                iter.next();
                        }
                    }
                    delete dock;
                    dock = NULL;
                }
            }
        }
    }
}



void cpMainComponents::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        if (m_projectview)
        {
            if (m_projectview->loadProjectFile(action->data().toString()))
            {
                curFile = m_projectview->m_curProjectFilePathName;
            }

        }

    }
}

void cpMainComponents::createActions()
{
    newProjectAct = new QAction(QIcon(":/CompressonatorGUI/Images/filenew.png"), tr("&New Project..."), this);
    if (newProjectAct)
    {
        newProjectAct->setShortcuts(QKeySequence::New);
        newProjectAct->setStatusTip(tr("Create a new project file"));
        connect(newProjectAct, SIGNAL(triggered()), this, SLOT(openNewProject()));
    }

    openAct = new QAction(QIcon(":/CompressonatorGUI/Images/open.png"), tr("&Open project..."), this);
    if (openAct)
    {
        openAct->setShortcuts(QKeySequence::Open);
        openAct->setStatusTip(tr("Open an existing project file"));
        connect(openAct, SIGNAL(triggered()), this, SLOT(openProjectFile()));
    }

    saveAct = new QAction(QIcon(":/CompressonatorGUI/Images/save.png"), tr("&Save project"), this);
    if (saveAct)
    {
        saveAct->setShortcuts(QKeySequence::Save);
        saveAct->setStatusTip(tr("Save project file"));
        connect(saveAct, SIGNAL(triggered()), this, SLOT(saveProjectFile()));
    }

    saveAsAct = new QAction(QIcon(""), tr("&Save project as..."), this);
    if (saveAsAct)
    {
        saveAsAct->setShortcuts(QKeySequence::SaveAs);
        saveAsAct->setStatusTip(tr("Save project as ..."));
        connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAsProjectFile()));
    }

    
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
            this, SLOT(openRecentFile()));
    }

    saveToBatchFileAct = new QAction(QIcon(""), tr("&Export to batch file..."), this);
    if (saveToBatchFileAct)
    {
        saveToBatchFileAct->setStatusTip(tr("Export the project file to a command line batch file"));
        connect(saveToBatchFileAct, SIGNAL(triggered()), this, SLOT(saveProjectToBatchFile()));
        saveToBatchFileAct->setEnabled(false);
    }

    openImageFileAct = new QAction(QIcon(":/CompressonatorGUI/Images/file.png"), tr("&Open Image File..."), this);
    if (openImageFileAct)
    {
        //ToDo :: openImageFileAct->setShortcuts();
        openImageFileAct->setStatusTip(tr("Open an image file"));
        connect(openImageFileAct, SIGNAL(triggered()), this, SLOT(openImageFile()));
    }

    deleteImageAct = new QAction(QIcon(":/CompressonatorGUI/Images/delete.png"), tr("&Delete current image"), this);
    if (deleteImageAct)
    {
        //ToDo :: deleteImageAct->setShortcuts();
        deleteImageAct->setStatusTip(tr("Delete selected image file"));
        connect(deleteImageAct, SIGNAL(triggered()), this, SLOT(deleteImageFile()));
        deleteImageAct->setEnabled(false);
    }

    compressAct = new QAction(QIcon(":/CompressonatorGUI/Images/compress.png"), tr("&Process selected images"), this);
    if (compressAct)
    {
        compressAct->setStatusTip(tr("Compress all selected items"));
        connect(compressAct, SIGNAL(triggered()), m_projectview, SLOT(OnStartCompression()));
        compressAct->setEnabled(false);
    }

    imagediffAct = new QAction(QIcon(":/CompressonatorGUI/Images/imagediff.png"), tr("&View Image Diff"), this);
    if (imagediffAct)
    {
        imagediffAct->setStatusTip(tr("View Image Diff"));
        connect(imagediffAct, SIGNAL(triggered()), m_projectview, SLOT(viewImageDiff()));
        imagediffAct->setEnabled(false);
    }


    MIPGenAct = new QAction(QIcon(":/CompressonatorGUI/Images/MIP.png"), tr("&Generate MIP maps for current source image"), this);
    if (MIPGenAct)
    {
        MIPGenAct->setStatusTip(tr("Generate MIP maps on current source image"));
        connect(MIPGenAct, SIGNAL(triggered()), this, SLOT(genMIPMaps()));
        MIPGenAct->setEnabled(false);
    }

#ifdef USE_MAIN_IMAVEVIEW_TOOLBAR
    imageview_zoomInAct    = new QAction(QIcon(":/CompressonatorGUI/Images/ZoomIn.png"), tr("&Zoom into Image "), this);
    imageview_zoomOutAct   = new QAction(QIcon(":/CompressonatorGUI/Images/ZoomOut.png"), tr("&Zoom out of Image"), this);
    imageview_RedAct       = new QAction(QIcon(":/CompressonatorGUI/Images/redStone.png"), tr("Show or Hide  Red channel"), this);
    imageview_GreenAct     = new QAction(QIcon(":/CompressonatorGUI/Images/greenStone.png"), tr("Show or Hide Green channel"), this);
    imageview_BlueAct      = new QAction(QIcon(":/CompressonatorGUI/Images/blueStone.png"), tr("Show or Hide Blue channel"), this);
    imageview_AlphaAct     = new QAction(QIcon(":/CompressonatorGUI/Images/circle.png"), tr("Show or Hide Alpha channel"), this);
    imageview_FitScreenAct = new QAction(QIcon(":/CompressonatorGUI/Images/expand.png"), tr("&Fit in Window"), this);
#endif

    exitAct = new QAction(QIcon(""), tr("&Exit"), this);
    if (exitAct)
    {
        exitAct->setStatusTip(tr("Exit Application"));
        connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    }

    if (m_showAppSettingsDialog)
    {
        settingsAct = new QAction(QIcon(":/CompressonatorGUI/Images/Gear.png"), tr("&Set Application Options"), this);
        if (settingsAct)
        {
            settingsAct->setStatusTip(tr("Set Application Options"));
            connect(settingsAct, SIGNAL(triggered()), this, SLOT(settings()));
        }
    }

    showWelcomePageAct = new QAction(tr("Welcome Page"), this);
    if (showWelcomePageAct)
    {
        showWelcomePageAct->setStatusTip(tr("View Welcome Page"));
        connect(showWelcomePageAct, SIGNAL(triggered()), this, SLOT(onShowWelcomePage()));
    }

    showOutputAct = new QAction(tr("Output"), this);
    if (showOutputAct)
    {
        showOutputAct->setStatusTip(tr("View Output Window"));
        connect(showOutputAct, SIGNAL(triggered()), this, SLOT(onShowOutput()));
    }

    closeAllDocuments = new QAction(tr("Close all Image Views"), this);
    if (closeAllDocuments)
    {
        closeAllDocuments->setStatusTip(tr("Close all opened image views"));
        connect(closeAllDocuments, SIGNAL(triggered()), this, SLOT(onCloseAllDocuments()));
    }

    gettingStartedAct = new QAction(tr("Getting Started ..."), this);
    if (gettingStartedAct)
    {
        gettingStartedAct->setStatusTip(tr("Getting Started"));
        connect(gettingStartedAct, SIGNAL(triggered()), this, SLOT(gettingStarted()));
    }

    userGuideAct = new QAction(tr("User Guide ..."), this);
    if (userGuideAct)
    {
        userGuideAct->setStatusTip(tr("User Guide"));
        connect(userGuideAct, SIGNAL(triggered()), this, SLOT(userGuide()));
    }

    aboutAct = new QAction(tr("About Compressonator"), this);
    if (aboutAct)
    {
        aboutAct->setStatusTip(tr("About Compressonator"));
        connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    }
}

void cpMainComponents::onGenerateMIPMap(int nMinSize)
{
    if (!m_projectview) return;

    QTreeWidgetItem *item = m_projectview->GetCurrentItem(TREETYPE_IMAGEFILE_DATA);
    if (item)
    {
        QVariant v = item->data(1, Qt::UserRole);
        C_Source_Image *data = v.value<C_Source_Image *>();
    
        if (data->m_MipImages)
            if (data->m_MipImages->mipset)
                if (data->m_MipImages->mipset->m_compressed)
                {
                    if (m_CompressStatusDialog)
                    {
                        m_CompressStatusDialog->onClearText();
                        m_CompressStatusDialog->showOutput();
                    }
                    PrintInfo("Mipmap generation is not supported for compressed image.");
                    return;
                }
    }
   
    if (nMinSize <= 0) nMinSize = 1;

    PluginInterface_Filters *plugin_Filter;
    plugin_Filter = reinterpret_cast<PluginInterface_Filters *>(g_pluginManager.GetPlugin("FILTER", "BOXFILTER"));
    if (plugin_Filter)
    {
        if (item)
        {
            QVariant v = item->data(1, Qt::UserRole);
            C_Source_Image *data = v.value<C_Source_Image *>();
            if (data)
            {
                if (m_CompressStatusDialog)
                {
                    m_CompressStatusDialog->onClearText();
                    m_CompressStatusDialog->showOutput();
                }

                // Quick Check to see if lowest level is lower then current image size
                int min = data->m_Width;
                if (min > data->m_Height) min = data->m_Height;
                if (nMinSize < min)
                {

                    if (data->m_MipImages)
                        if (data->m_MipImages->mipset)
                        {
                            if (m_CompressStatusDialog)
                            {
                                m_CompressStatusDialog->onClearText();
                                m_CompressStatusDialog->show();
                            }

                            // Generate the MIP levels
                            plugin_Filter->TC_GenerateMIPLevels(data->m_MipImages->mipset, nMinSize);

                            // Create Image views for the levels
                            CImageLoader ImageLoader;
                            ImageLoader.UpdateMIPMapImages(data->m_MipImages);

                            if (m_CompressStatusDialog)
                            {
                                QString msg = "<b>Generated : ";
                                msg.append(QString::number(data->m_MipImages->mipset->m_nMipLevels));
                                msg.append(" MIP level(s)</b>");
                                msg.append(" with a minimum size set to ");
                                msg.append(QString::number(nMinSize));
                                msg.append(" px");
                                m_CompressStatusDialog->appendText(msg);
                                m_ForceImageRefresh = true;
                            }

                            m_projectview->SignalUpdateData(item, TREETYPE_IMAGEFILE_DATA);
                            m_projectview->onTree_ItemClicked(item, 0);
                        }
                }
                else
                {
                    if (m_CompressStatusDialog)
                        m_CompressStatusDialog->appendText("No MIP levels generated: Please select a level lower than current image size");
                }
            }
            delete plugin_Filter;
        }
    }
}

void cpMainComponents::genMIPMaps()
{
    if (m_projectview)
    {
        QTreeWidgetItem *item = m_projectview->GetCurrentItem(TREETYPE_IMAGEFILE_DATA);
        if (item)
        {
            QVariant v = item->data(1, Qt::UserRole);
            C_Source_Image *data = v.value<C_Source_Image *>();
            if (data)
            {
                // regenrate mip map
                if (data->m_MipImages->mipset->m_nMipLevels > 1 || data->m_MipImages->Image_list.count()>1)
                {
                    int n = data->m_MipImages->Image_list.count();
                    for (int i = 1; i < n; i++)
                    {
                        data->m_MipImages->Image_list.removeLast();
                    }

                    data->m_MipImages->mipset->m_nMipLevels = 1;
                }

                m_genmips->setMipLevelDisplay(data->m_Width, data->m_Height);
                m_genmips->show();

                // Generate mipmap only once- no regenerate then uncomment code block below
                //if (data->m_MipImages->mipset->m_nMipLevels <= 1)
                //{
                //    m_genmips->setMipLevelDisplay(data->m_Width, data->m_Height);
                //    m_genmips->show();
                //}
                //else
                //{
                //    QMessageBox msgBox;
                //    msgBox.setText("The image already has MIP levels!");
                //    msgBox.setStandardButtons(QMessageBox::Ok);
                //    msgBox.exec();
                //}
            }
        }
    }
}

void cpMainComponents::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);
    settings.sync();
    SetProjectWindowTitle();
}

void cpMainComponents::updateRecentFileActions()
{
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    QStringList files = settings.value("recentFileList").toStringList();
    QStringList UpdatedList;

    int numRecentFile = 0;
    int scan_numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
    
    UpdatedList.clear();
    for (int i = 0; i < scan_numRecentFiles; ++i)
    {

        m_projectsRecentFiles.push_back(files[i]);

        QFile file(files[i]);
        if (file.exists())
        {
            QString text = tr("&%1 %2").arg(numRecentFile + 1).arg(strippedName(files[i]));
            recentFileActs[numRecentFile]->setText(text);
            recentFileActs[numRecentFile]->setData(files[i]);
            recentFileActs[numRecentFile]->setStatusTip(files[i]);
            recentFileActs[numRecentFile]->setVisible(true);
            numRecentFile++;
            UpdatedList.append(files[i]);
        }
    }

    // Save back a cleaned up list of existing project files
    settings.setValue("recentFileList", UpdatedList);

    for (int j = numRecentFile; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
    
    m_numRecentFiles = numRecentFile;
    separatorAct->setVisible(m_numRecentFiles > 0);

}

QString cpMainComponents::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void cpMainComponents::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    if (fileMenu)
    {
        if (newProjectAct) fileMenu->addAction(newProjectAct);
        if (openAct) fileMenu->addAction(openAct);
        if (saveAct) fileMenu->addAction(saveAct);
        if (saveAsAct) fileMenu->addAction(saveAsAct);
        fileMenu->addSeparator();
        if (openImageFileAct) fileMenu->addAction(openImageFileAct);
        if (saveToBatchFileAct) fileMenu->addAction(saveToBatchFileAct);
        fileMenu->addSeparator();
        if (exitAct) fileMenu->addAction(exitAct);
        separatorAct = menuBar()->addSeparator();
        for (int i = 0; i < MaxRecentFiles; ++i)
            fileMenu->addAction(recentFileActs[i]);
        updateRecentFileActions();
        fileMenu->addSeparator();
        fileMenu->addAction(exitAct);
    }

    if (m_showAppSettingsDialog)
    {
        settingsMenu = menuBar()->addMenu(tr("&Settings"));
        if (settingsMenu)
        {
            if (settingsAct) settingsMenu->addAction(settingsAct);
        }
    }

    windowMenu = menuBar()->addMenu(tr("&Window"));
    if (windowMenu)
    {
        if (showWelcomePageAct)     windowMenu->addAction(showWelcomePageAct);
        if (showOutputAct)          windowMenu->addAction(showOutputAct);
        if (closeAllDocuments)      windowMenu->addAction(closeAllDocuments);
    }

    helpMenu = menuBar()->addMenu(tr("&Help"));
    if (helpMenu)
    {
        if (gettingStartedAct) helpMenu->addAction(gettingStartedAct);
        if (userGuideAct) helpMenu->addAction(userGuideAct);
        if (aboutAct) helpMenu->addAction(aboutAct);
    }
}

void cpMainComponents::menuItemClicked(QAction* triggeredAction)
{
    // use either the action itself... or an offset
    int value = triggeredAction->data().toInt();
    Q_UNUSED(value);
}

void cpMainComponents::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    if (fileToolBar)
    {
        if (newProjectAct) fileToolBar->addAction(newProjectAct);
        if (openAct) fileToolBar->addAction(openAct);
        if (saveAct) fileToolBar->addAction(saveAct);
        fileToolBar->addSeparator();
        if (openImageFileAct) fileToolBar->addAction(openImageFileAct);
        if (settingsAct) fileToolBar->addAction(settingsAct);
        fileToolBar->addSeparator();
        if (deleteImageAct) fileToolBar->addAction(deleteImageAct);
    }

    CompressionToolBar = addToolBar(tr("Compression"));
    if (CompressionToolBar)
    {
        if (compressAct) CompressionToolBar->addAction(compressAct);
        if (imagediffAct) CompressionToolBar->addAction(imagediffAct);
        if (MIPGenAct) CompressionToolBar->addAction(MIPGenAct);
    }

#ifdef USE_MAIN_IMAVEVIEW_TOOLBAR
    ImageViewToolBar = addToolBar(tr("Image View"));
    if (ImageViewToolBar)
    {
        if (imageview_zoomInAct) ImageViewToolBar->addAction(imageview_zoomInAct);
        if (imageview_zoomOutAct) ImageViewToolBar->addAction(imageview_zoomOutAct);
        if (imageview_RedAct) ImageViewToolBar->addAction(imageview_RedAct);
        if (imageview_GreenAct) ImageViewToolBar->addAction(imageview_GreenAct);
        if (imageview_BlueAct) ImageViewToolBar->addAction(imageview_BlueAct);
        if (imageview_AlphaAct) ImageViewToolBar->addAction(imageview_AlphaAct);
        if (imageview_FitScreenAct) ImageViewToolBar->addAction(imageview_FitScreenAct);
    }
    
    ImageViewToolBar->setEnabled(false);
#endif

}

void cpMainComponents::createStatusBar()
{
    statusBar()->setStyleSheet("QStatusBar{border-top: 1px outset grey;}");
    statusBar()->showMessage(tr("Ready"));
}

void cpMainComponents::showProgressBusy(QString Message)
{
    statusBar()->showMessage(Message);
    qApp->setOverrideCursor(Qt::BusyCursor);
    if (m_projectview)
        m_projectview->m_processBusy = true;
}

void cpMainComponents::hideProgressBusy(QString Message)
{
    statusBar()->showMessage(Message);
    qApp->restoreOverrideCursor();
    if (m_projectview)
        m_projectview->m_processBusy = false;
}

void cpMainComponents::readSettings()
{
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(800, 600)).toSize();
    resize(size);
    move(pos);
    if (m_showAppSettingsDialog)
    {
        m_setapplicationoptions->LoadSettings(m_sSettingsFile, QSettings::IniFormat);
    }
}

void cpMainComponents::writeSettings()
{
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    if (m_showAppSettingsDialog)
    {
        m_setapplicationoptions->SaveSettings(m_sSettingsFile, QSettings::IniFormat);
    }

}

acCustomDockWidget *cpMainComponents::FindImageView(QString &Title)
{
    QList<acCustomDockWidget *> dockWidgets = m_parent->findChildren<acCustomDockWidget *>();
    QListIterator<acCustomDockWidget *> iter(dockWidgets);
    acCustomDockWidget *dock;
    while (iter.hasNext())
    {
        dock = iter.next();
        QString m_fileName = dock->m_fileName;
        //qDebug() << "Match " << Title <<  "with TAB NAMES [" << m_fileName << "]";
        int res = Title.compare(m_fileName);
        if (res == 0) {
            //qDebug() << "--- FOUND ---";
            return dock;
        }
    }

    return NULL;
}

// Active when user selects any Docked Tabbed Item 
void cpMainComponents::onDockImageVisibilityChanged(bool visible)
{
    Q_UNUSED(visible);

    QTabBar *tabBar = this->findChild<QTabBar *>();
    if (tabBar)
    {
        int currentIndex = tabBar->currentIndex();
        QString tabText = tabBar->tabText(currentIndex);
        int numTab = tabBar->count();

        for (int i = 0; i < numTab; i++)
        {
            if (tabBar->tabText(i).compare("") == 0)
            {
                tabBar->setStyleSheet("QTabBar::tab:disabled { width: 0; height: 0; margin: 0; padding: 0; border: none; }");
                tabBar->setTabEnabled(i, false);
            }
        }

        const QList<QDockWidget *> tabedWidgets = tabifiedDockWidgets(m_blankpage);
        QDockWidget *item;
        foreach(item, tabedWidgets) {

            // Get our custom Dock Widget 
            if (item->titleBarWidget())
            {
                acCustomDockWidget *imageItem = reinterpret_cast<acCustomDockWidget *> (item);
                if (imageItem && !m_viewDiff)
                {
                    if (imageItem->m_tabName.compare("") == 0)
                    {
                        imageItem->lower();
                    }

                    if (imageItem->m_tabName.compare(tabText) == 0)
                    {
                        emit SetCurrentItem(imageItem->m_fileName);
                    }
                }
            }
        }
    }
}

void cpMainComponents::AddImageCompSettings(QTreeWidgetItem *item, C_Destination_Options &data)
{
    Q_UNUSED(item);

    if (data.m_editing)
    {
        QVariant v = item->data(1, Qt::UserRole);
        C_Destination_Options *m_data = v.value<C_Destination_Options *>();
        *m_data << data;
        item->setText(0, m_data->m_compname);

        QFileInfo fileinfo(m_data->m_destFileNamePath);
        QFile file(m_data->m_destFileNamePath);
        m_data->m_FileSize = file.size();
        if (m_data->m_FileSize > 1024000)
            m_data->m_FileSizeStr = QString().number((double)m_data->m_FileSize / 1024000, 'f', 2) + " MB";
        else
            if (m_data->m_FileSize > 1024)
                m_data->m_FileSizeStr = QString().number((double)m_data->m_FileSize / 1024, 'f', 1) + " KB";
            else
                m_data->m_FileSizeStr = QString().number(m_data->m_FileSize) + " Bytes";
        if (file.exists() && (fileinfo.suffix().length() > 0))
            item->setIcon(0, QIcon(":/CompressonatorGUI/Images/smallGrayStone.png"));
        else
            item->setIcon(0, QIcon(":/CompressonatorGUI/Images/smallWhiteBlank.png"));

        // refresh the current Image property view (It may or maynot be pointing to The compression data 
        // That was edited.
        m_imagePropertyView->refreshView();

        // Refresh the image view
        m_projectview->onTree_ItemClicked(item, 0);

    }
    else
    {
        C_Destination_Options *m_data = new C_Destination_Options;
        // copythe new data from comsettings dialog data
        *m_data << data;
        m_projectview->Tree_AddCompressFile(item, m_data->m_compname, true, true, TREETYPE_COMPRESSION_DATA, m_data);
    }
}

void cpMainComponents::AddImageView(QString &fileName, QTreeWidgetItem * item)
{
    bool isComp = true, isDel = true;
    bool doRefreshCompressView = false;

    try
    {
        if (isCompressInProgress) return;

        if (!item) return;

        emit OnImageLoadStart();
        if (compressAct)
        {
            isComp = compressAct->isEnabled();
            compressAct->setEnabled(false);
        }

        if (imagediffAct)
        {
            imagediffAct->setEnabled(false);
        }

        if (deleteImageAct)
        {
            isDel = deleteImageAct->isEnabled();
            deleteImageAct->setEnabled(false);
        }

        // Determin File Type
        QVariant v = item->data(0, Qt::UserRole);
        int levelType = v.toInt();

        acCustomDockWidget      *dock = NULL;
        C_Destination_Options   *m_compressdata;

        if (levelType == TREETYPE_COMPRESSION_DATA)
        {
            QVariant v = item->data(1, Qt::UserRole);
            m_compressdata = v.value<C_Destination_Options *>();
            if (m_compressdata->m_data_has_been_changed)
            {
                // Find the old image and remove it
                OnDeleteImageView(fileName);
                m_compressdata->m_data_has_been_changed = false;
                doRefreshCompressView = true;
            }
        }


        if ((g_Application_Options.m_useNewImageViews || doRefreshCompressView || g_Application_Options.m_refreshCurrentView || m_ForceImageRefresh))
        {
            // Find the old image and remove it
            OnDeleteImageView(fileName);

            if (doRefreshCompressView)
                m_compressdata->m_data_has_been_changed = false;

            m_ForceImageRefresh = false;
        }
        else
        {
            // Make sure we are not not already viewing this image file
            dock = FindImageView(fileName);
        }

        if (dock) {
            // We found the image in our list of existing views
            m_activeImageTab = dock;
        }

        showProgressBusy("Loading Image... Please wait");

        if (!g_Application_Options.m_useNewImageViews)
        {
            if (dock) {
                // check if the view is uptodate 
                // or needs to be refreshed
                m_activeImageTab = dock;

                if (levelType == TREETYPE_COMPRESSION_DATA)
                {
                    if (doRefreshCompressView)
                    {
                        // Flag project we have new project settings to save on exit!
                        if (m_projectview)
                            m_projectview->m_saveProjectChanges = true;

                        // Remove the old tab 
                        if (dock)
                        {
                            delete dock;
                            dock = NULL;
                        }
                    }
                }
            }
        }

        if (dock == NULL)
        {
            QString ImageType;
            Setting *setting = new Setting();
            setting->onBrightness = false;

            if (levelType == TREETYPE_COMPRESSION_DATA)
            {
                // Get ImageFile Data
                QVariant v = item->data(1, Qt::UserRole);
                C_Destination_Options *m_filedata = v.value<C_Destination_Options *>();

                if (m_filedata)
                {
                    QFile file(fileName);
                    m_filedata->m_FileSize = file.size();
                    if (m_filedata->m_FileSize > 1024000)
                        m_filedata->m_FileSizeStr = QString().number((double)m_filedata->m_FileSize / 1024000, 'f', 2) + " MB";
                    else
                        if (m_filedata->m_FileSize > 1024)
                            m_filedata->m_FileSizeStr = QString().number((double)m_filedata->m_FileSize / 1024, 'f', 1) + " KB";
                        else
                            m_filedata->m_FileSizeStr = QString().number(m_filedata->m_FileSize) + " Bytes";
                    // Create a new view image
                    ImageType = " Image file";
                    m_imageview = new cpImageView(fileName, ImageType, m_parent, m_filedata->m_MipImages, setting);
                }
            }
            else
            {
                // Get ImageFile Data
                QVariant v = item->data(1, Qt::UserRole);
                C_Source_Image *m_filedata = v.value<C_Source_Image *>();

                if (m_filedata)
                {
                    // Create a new view image
                    ImageType = " Original Image file";
                    setting->reloadImage = g_Application_Options.m_useNewImageViews;
                    if (m_filedata->m_MipImages->Image_list.count() > 1)
                        setting->generateMips = true;

                    m_imageview = new cpImageView(fileName, ImageType, m_parent, m_filedata->m_MipImages, setting);

                    setting->generateMips = false;
                }
            }

            if (m_imageview)
            {
                m_imageview->showToobar(true);
                m_imageview->showToobarButton(true);
                m_imageview->setAllowedAreas(Qt::RightDockWidgetArea);
                m_parent->addDockWidget(Qt::RightDockWidgetArea, m_imageview);
                m_parent->tabifyDockWidget(m_blankpage, m_imageview);
                m_viewDiff = false;
                connect(m_imageview, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockImageVisibilityChanged(bool)));
                connect(m_imageview, SIGNAL(UpdateData(QObject *)), m_imagePropertyView, SLOT(OnUpdateData(QObject *)));
                m_activeImageTab = m_imageview;
            }
        }

        // =================================
        // Place the TAB on view if hidden!
        // =================================
        if (m_activeImageTab)
        {
            // check its visability: User is requesting view
            if (!m_activeImageTab->isVisible())
            {
                m_activeImageTab->setVisible(true);
            }

            m_activeImageTab->raise();
            //QTimer::singleShot(10, this, SLOT(SetRaised()));
        }
    }
    catch (...)
    {
        if (m_CompressStatusDialog)
        {
            m_CompressStatusDialog->showOutput();
            m_CompressStatusDialog->raise();
            //QTimer::singleShot(10, this, SLOT(SetRaised()));
        }
          // do some message 
    }


    emit OnImageLoadDone();
    if (compressAct)
        compressAct->setEnabled(isComp);
    if (imagediffAct)
        imagediffAct->setEnabled(isComp);
    if (deleteImageAct)
        deleteImageAct->setEnabled(isDel);
    hideProgressBusy("Ready");
}

void cpMainComponents::AddImageDiff(C_Destination_Options *destination)
{
    try
    {
        if (isCompressInProgress) return;

        bool isComp = true, isDel = true;

        emit OnImageLoadStart();
        if (compressAct)
        {
            isComp = compressAct->isEnabled();
            compressAct->setEnabled(false);
        }

        if (imagediffAct)
        {
            imagediffAct->setEnabled(false);
        }

        if (deleteImageAct)
        {
            isDel = deleteImageAct->isEnabled();
            deleteImageAct->setEnabled(false);
        }


        showProgressBusy("Loading Image Differance...Please wait");

        QString originalFileName = destination->m_sourceFileNamePath;
        QString compressedFileName = destination->m_destFileNamePath;

        // Find the old image diff and remove it
        // User may have selected a newer Decompression Option
        // for the image diff view
        QString Title = DIFFERENCE_IMAGE_TXT + compressedFileName;
        OnDeleteImageDiffView(compressedFileName);

        // Create a new view image
        m_imageCompare = new CImageCompare(Title, destination, m_parent);
        CMipImages  *m_diffMips = m_imageCompare->getMdiffMips();
        if (m_diffMips == NULL)
        {
            delete m_imageCompare;
            PrintInfo("Image Diff Error: Diff Image cannot be found\n");
        }
        else
        {
            m_imageCompare->setAllowedAreas(Qt::RightDockWidgetArea);
            m_parent->addDockWidget(Qt::RightDockWidgetArea, m_imageCompare);
            m_parent->tabifyDockWidget(m_blankpage, m_imageCompare);
            if (m_imageCompare->custTitleBar)
            {
                m_imageCompare->custTitleBar->m_close = true;
                connect(m_imageCompare->custTitleBar, SIGNAL(OnAboutToClose(QString &)), this, SLOT(onAboutToClose(QString &)));

            }
            m_viewDiff = true;
            connect(m_imageCompare, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockImageVisibilityChanged(bool)));
            connect(m_imageCompare, SIGNAL(UpdateData(QObject *)), m_imagePropertyView, SLOT(OnUpdateData(QObject *)));
            m_activeImageTab = m_imageCompare;

            // check its visability: User is requesting view
            if (!m_activeImageTab->isVisible())
            {
                m_activeImageTab->setVisible(true);
            }

            m_activeImageTab->raise();
            QTimer::singleShot(30, this, SLOT(SetRaised()));
        }


        hideProgressBusy("Ready");

        emit OnImageLoadDone();

        if (compressAct)
            compressAct->setEnabled(isComp);
        if (imagediffAct)
            imagediffAct->setEnabled(isComp);
        if (deleteImageAct)
            deleteImageAct->setEnabled(isDel);
    }
    catch (...)
    {
        // Do some message
    }

}

void cpMainComponents::OnDeleteImageView(QString &fileName)
{
    showProgressBusy("Removing Image view ... Please wait");
    // Make sure we are not not already viewing this image file
    acCustomDockWidget *dock = (acCustomDockWidget *)FindImageView(fileName);

    if (dock)
    {
        delete dock;
        dock = NULL;
    }

    // Also Remove any Image Diff Tabs
    OnDeleteImageDiffView(fileName);

    hideProgressBusy("Ready");
}

void cpMainComponents::OnDeleteImageDiffView(QString &fileName)
{
    showProgressBusy("Removing Image Differance view ... Please wait");

    QString Title = DIFFERENCE_IMAGE_TXT + fileName;

    // Make sure we are not not already viewing this image file
    acCustomDockWidget *dock = (acCustomDockWidget *) FindImageView(Title);

    if (dock)
    {
        delete dock;
        dock = NULL;
    }

    hideProgressBusy("Ready");
}

void cpMainComponents::SetRaised()
{
    if (m_activeImageTab)
        m_activeImageTab->raise();
}

cpMainComponents::~cpMainComponents()
{
    CMP_ShutdownDecompessLibrary();
}

void cpMainComponents::OnAddCompressSettings(QTreeWidgetItem *item)
{
    //int setting = 1;
    QString CompProjectName = "New";

    m_setcompressoptions->m_data.init();

    emit m_setcompressoptions->m_data.compressionChanged((QVariant &)m_setcompressoptions->m_data.m_Compression);

    // Obtain the Parent and its data
    QTreeWidgetItem *parent = item->parent();
    if (parent)
    {
        // Varify its root
        QVariant v = parent->data(0, Qt::UserRole);
        int ParentlevelType = v.toInt();
        if (ParentlevelType == TREETYPE_IMAGEFILE_DATA)
        {
            QVariant v = parent->data(1, Qt::UserRole);
            C_Source_Image *m_imagefile = v.value<C_Source_Image *>();
            QFileInfo fileinfo(m_imagefile->m_Name);
            CompProjectName = fileinfo.baseName();
            m_setcompressoptions->m_data.m_sourceFileNamePath = m_imagefile->m_Full_Path;
            m_setcompressoptions->m_data.m_SourceImageSize    = m_imagefile->m_ImageSize;
            m_setcompressoptions->m_data.m_SourceIscompressedFormat = CompressedFormat(m_imagefile->m_Format);

            // Used to append to name - for unique name
            // There is still chances of duplucate names, but it will not effect
            // compression unless target file is also of the same name as any other child 
            // compression settings
            int parentcount = parent->childCount();

            if (m_imagefile->m_extnum <= parentcount)
                    m_imagefile->m_extnum = parentcount;

            //setting = m_imagefile->m_extnum++;
            m_setcompressoptions->m_extnum = m_imagefile->m_extnum++;
            m_setcompressoptions->m_data.m_Width  = m_imagefile->m_Width;
            m_setcompressoptions->m_data.m_Height = m_imagefile->m_Height;
        }
    }

    m_setcompressoptions->m_data.m_compname = CompProjectName;
    //m_setcompressoptions->m_data.m_compname.append("_");
    //m_setcompressoptions->m_data.m_compname.append(QString::number(setting));


    m_setcompressoptions->m_data.m_editing = false;
    m_setcompressoptions->m_item = item;                    

    if (m_setcompressoptions->updateDisplayContent())
    {
        if (!m_setcompressoptions->isVisible())
        {
            QPoint pos = QCursor::pos();
            m_setcompressoptions->move(pos);
            m_setcompressoptions->show();
        }
    }

}


void cpMainComponents::onAddedCompressSettingNode()
{
    if (compressAct)
        compressAct->setEnabled(true);
    m_setcompressoptions->m_destFilePath = m_setcompressoptions->m_DestinationFolder->text();
}

void cpMainComponents::onEditCompressSettings(QTreeWidgetItem *item)
{
    QVariant v = item->data(1, Qt::UserRole);
    C_Destination_Options *m_data = v.value<C_Destination_Options *>();
    if (m_data)
    {
        m_setcompressoptions->m_item = item;
        m_setcompressoptions->m_data << (const C_Destination_Options &)*m_data;

        if (m_setcompressoptions->updateDisplayContent())
        {
            if (!m_setcompressoptions->isVisible())
            {
                QPoint pos = QCursor::pos();
                m_setcompressoptions->move(pos);
                m_setcompressoptions->show();
            }

        }
    }
}

//==========================================
// Static Members used to redirect 
// Messages to GUI
//==========================================
bool isCompressMSG = false;
QString comError = "Failed to initialize COM";

// ----------------------------------
// Messages from command line prints
// ----------------------------------

void cpMainComponents::PrintStatus(char *buff)
{
    //qDebug() << buff;
    isCompressMSG = true;
    QString msg = buff;
    if (msg.contains(comError) )
                            return;

    emit static_msghandler.signalMessage(buff);
}

// ----------------------------------
// Messages from qDebug()
// ----------------------------------

void cpMainComponents::msgHandler(QtMsgType type, const char* msg)
{
    Q_UNUSED(type);
    //emit static_msghandler.signalMessage(msg);
}

void cpMainComponents::browserMsg(const char *msg)
{
    //statusBar()->showMessage(msg);
    if (m_CompressStatusDialog && isCompressMSG)
    {
        QString qmsg = msg;
        qmsg.remove(QRegExp("[\\n\\r]"));
        m_CompressStatusDialog->appendText(qmsg);
        isCompressMSG = false;
    }
}

//================================================


void cpMainComponents::removeItemTabs(QString *FilePathName)
{
    QTreeWidgetItem *item = m_projectview->GetCurrentItem(TREETYPE_COMPRESSION_DATA);
    if (item)
    {
        // qDebug() << "Delete this Tab: " << *FilePathName;
        // view image
        QVariant v = item->data(1, Qt::UserRole);
        C_Destination_Options *m_data = v.value<C_Destination_Options *>();
        if (m_data)
        {
            if ((m_data->m_destFileNamePath.compare(*FilePathName) == 0) ||
                (m_data->m_destFileNamePath.compare(DIFFERENCE_IMAGE_TXT + *FilePathName) == 0))
            {
                OnDeleteImageView(*FilePathName);
                OnDeleteImageDiffView(*FilePathName);
                m_activeImageTab = NULL;
                if (m_projectview)
                {
                    m_projectview->Tree_updateCompressIcon(item, *FilePathName, false);
                    m_projectview->m_saveProjectChanges = true;
                }

            }
        }
    }
}

void cpMainComponents::onPropertyViewSaveSetting(QString *FilePathName)
{
    if (m_projectview)
    {
        QFile::remove(*FilePathName);
        removeItemTabs(FilePathName);
        m_projectview->Tree_SetCurrentItem(*FilePathName);
    }
}

void cpMainComponents::onPropertyViewCompressImage(QString *FilePathName)
{
    if (m_projectview)
    {
        removeItemTabs(FilePathName);
        m_projectview->Tree_clearAllItemsSetected();
        QTreeWidgetItem *item = m_projectview->Tree_SetCurrentItem(*FilePathName);
        if (item != NULL)
        {
            QVariant v = item->data(1, Qt::UserRole);
            C_Destination_Options *m_data = v.value<C_Destination_Options *>();
            if (m_data)
            {
                m_data->m_isselected = true;
                m_projectview->OnStartCompression();
            }
        }
    }
}

void cpMainComponents::onCompressionStart()
{
    // Disable relavent tool bar options
    if (compressAct)
        compressAct->setEnabled(false);
    if (imagediffAct)
        imagediffAct->setEnabled(false);
    if (deleteImageAct)
        deleteImageAct->setEnabled(false);
    if (MIPGenAct)
        MIPGenAct->setEnabled(false);

    // Free up as much memory as we can 
    // prior to processing
    if (g_Application_Options.m_closeAllDocuments)
        onCloseAllDocuments();

    isCompressInProgress = true;
}


void cpMainComponents::onCompressionDone()
{
    isCompressInProgress = false;

    // Called when compression from Project view is completed.
    QTreeWidgetItem *item = m_projectview->GetCurrentItem(TREETYPE_COMPRESSION_DATA);
    if (item)
    {
        // view image
        QVariant v = item->data(1, Qt::UserRole);
        C_Destination_Options *m_data = v.value<C_Destination_Options *>();
        if (m_data)
        {
            // Refresh any changes in current items data
            // like compression time
            if (m_imagePropertyView)
                m_imagePropertyView->OnUpdateData(m_data);
        #ifdef SHOW_DECOMPRESS_IMAGE
            // This can cause issue if image decompresion takes a long time!
            AddImageView(m_data->m_destFileNamePath, item);
        #endif
        }
    }

    // Re-enable relavent tool bar options
    if (compressAct)
        compressAct->setEnabled(true);
    if (imagediffAct)
        imagediffAct->setEnabled(true);
    if (deleteImageAct)
        deleteImageAct->setEnabled(true);

}

void cpMainComponents::onSourceImage(int childCount)
{
    if (imagediffAct)
        imagediffAct->setEnabled(false);
    if (MIPGenAct)
        MIPGenAct->setEnabled(true);
    if (deleteImageAct)
        deleteImageAct->setEnabled(true);
    if (compressAct)
        compressAct->setEnabled(childCount > 1);

    if (saveToBatchFileAct)
    {
        if (!saveToBatchFileAct->isEnabled())
            saveToBatchFileAct->setEnabled(childCount > 1);
    }
}


void cpMainComponents::onProjectLoaded(int childCount)
{
    if (saveToBatchFileAct)
    {
        saveToBatchFileAct->setEnabled(childCount > 1);
    }

    if (compressAct)
        compressAct->setEnabled(childCount > 1);

    if (imagediffAct)
        imagediffAct->setEnabled(false);

    if (m_imagePropertyView)
        m_imagePropertyView->OnUpdateData(NULL);
}


void cpMainComponents::onDecompressImage()
{
    if (imagediffAct)
        imagediffAct->setEnabled(true);
    if (MIPGenAct)
        MIPGenAct->setEnabled(false);
    if (deleteImageAct)
        deleteImageAct->setEnabled(true);
    if (compressAct)
        compressAct->setEnabled(true);

    if (saveToBatchFileAct)
    {
        if (!saveToBatchFileAct->isEnabled())
            saveToBatchFileAct->setEnabled(true);
    }

}


void cpMainComponents::OpenCHMFile(QString fileName)
{
    QString str;

    str.append(URL_FILE);
    str.append(qApp->applicationDirPath());
    str.append("/");
    str.append(fileName);
    if (!QDesktopServices::openUrl(QUrl(str)))
    {
        char * ENV;
        str = "";
        ENV = getenv("COMPRESSONATOR_ROOT");
        if (ENV)
        {
            str.append(URL_FILE);
            str.append(ENV);
            str.append("/");
            str.append(fileName);
            QDesktopServices::openUrl(QUrl(str));
        }
    }
}

void cpMainComponents::userGuide()
{
    OpenCHMFile(COMPRESSONATOR_USER_GUIDE);
}


void cpMainComponents::gettingStarted()
{
    OpenCHMFile(COMPRESSONATOR_GETTING_STARTED);
}


void cpMainComponents::onProcessing(QString &FilePathName)
{
    // Reserved for future use 
    // to handle any action just before a file is processed 
    // by command line.
}

void cpMainComponents::onAboutToClose(QString &Title)
{
    QList<acCustomDockWidget *> dockWidgets = m_parent->findChildren<acCustomDockWidget *>();
    QListIterator<acCustomDockWidget *> iter(dockWidgets);
    acCustomDockWidget *dock;
    while (iter.hasNext())
    {
        dock = iter.next();
        if (dock)
        {
            QString DockTitle = dock->custTitleBar->getTitle();
            if (DockTitle.compare(Title) == 0)
            {
                delete dock;
                dock = NULL;
                break;
            }
        }
    }
}


