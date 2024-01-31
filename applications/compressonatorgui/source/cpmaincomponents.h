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

#ifndef CPMAINCOMPONENTS_H
#define CPMAINCOMPONENTS_H

#include <QtWidgets>
#include <QMainWindow>
#include <QtXml/QDomDocument>
#include "cpimagepropertyview.h"
#include "cpgenmips.h"
#include "cpsetcompressoptions.h"
#include "cpsetapplicationoptions.h"
#include "cpprojectview.h"
#include "cpimageview.h"
#include "cp3dmodelview.h"
#include "cp3dmodelcompare.h"
#include "cpwelcomepage.h"
#include "cpimagecompare.h"
#include "cpcompressstatus.h"
#include "acaboutdlg.h"
#include "acprogressdlg.h"
#include "cmdline.h"
#include "utilfuncs.h"

#include <QTimer>
#include "accustomdockwidget.h"
// //Reserved: GPUDecode

// Common Project Data Types
#include "cpprojectdata.h"

// File Menu
#include <QMenu>

#ifdef USE_3DCONVERT
#include "cp3dmodelconvert.h"
#endif
// #define ENABLE_AGS_SUPPORT
#ifdef ENABLE_AGS_SUPPORT
#include <amd_ags.h>
#ifdef _WIN64
#pragma comment(lib, "amd_ags_x64.lib")
#else
#pragma comment(lib, "amd_ags_x32.lib")
#endif
#endif

#define PROJECT_EXTENSION ".cprj"
#define ENV_COMPRESSONATOR_ROOT "COMPRESSONATOR_ROOT"

//
// This Class is defined as Static
// to enable message handling via emit signals calls from static & global functions
//
class signalMsgHandler : public QObject
{
    Q_OBJECT

public:
    signalMsgHandler(){};

Q_SIGNALS:
    void signalMessage(const char* msg);
};

class cpMainComponents : public QMainWindow
{
    Q_OBJECT

public:
    static void      msgHandler(QtMsgType type, const char* msg);
    static void      PrintStatus(char* buff);
    QVector<QString> m_projectsRecentFiles;

    cpMainComponents(QDockWidget* root_dock, QMainWindow* parent = 0);
    ~cpMainComponents();

    void setCurrentFile(const QString& fileName);
    void updateRecentFileActions();
    void OpenCHMFile(QString fileName);

    QString strippedName(const QString& fullFileName);

    void SetProjectWindowTitle();
    void showProgressBusy(QString Message);
    void hideProgressBusy(QString Message);

    void DisplayException(QString msgTitle);

#ifdef ENABLE_AGS_SUPPORT
    void AGSGetDisplayInfo(AGSDisplaySettings* settings);
    bool AGSSetDisplay(AGSDisplaySettings* settings);
#endif
#ifdef USE_3DCONVERT
    CModelConvert* m_modelConvert;
#endif

    bool                    m_ForceImageRefresh;
    QString                 m_sSettingsFile;
    QString                 curFile;
    QMenu*                  recentFilesMenu;
    CWelcomePage*           m_welcomePage;
    CImagePropertyView*     m_imagePropertyView;
    CGenMips*               m_genmips;
    CSetCompressOptions*    m_setcompressoptions;
    CSetApplicationOptions* m_setapplicationoptions;
    ProjectView*            m_projectview;
    acCustomDockWidget*     m_blankpage;
    CImageCompare*          m_imageCompare;
    QDockWidget*            m_activeImageTab;  // Current viewing Image Tab
    QDockWidget*            app_welcomepage;
    QMainWindow*            m_parent;
    QDialog*                m_frame;  // http://doc.qt.io/qt-4.8/qframe.html
    bool                    m_appclosing = false;

    enum
    {
        MaxRecentFiles = 10
    };
    int      m_numRecentFiles;  // Counts the number of recent files loaded
    QAction* recentFileActs[MaxRecentFiles];
    QAction* separatorAct;

    CompressStatusDialog* m_CompressStatusDialog;
    CHelpAboutDialog*     m_pacHelpAboutDialog;
    QString               m_apptitle;
    QString               m_copyRightCaption;
    QString               m_copyRightInformation;

    bool isCompressInProgress;

#ifdef ENABLE_AGS_SUPPORT
    // HDR & FullScreen Modes
    AGSDisplaySettings m_settings;
    bool               m_bIsHDRAvailableOnPrimary;
    bool               m_bIsFullScreenModeOn;
    AGSContext*        m_agsContext;
    int                m_DeviceIndex;
    int                m_DisplayIndex;
#endif

public slots:

    void onSetToolBarActions(int islevelType);

    void AddImageView(QString& fileName, QTreeWidgetItem* item);

    void AddImageDiff(C_Destination_Options* destination, QString& fileName1, QString& fileName2);
    void Add3DModelDiff(C_3DSubModel_Info* destination, QString& fileName1, QString& fileName2);
    void AddImageCompSettings(QTreeWidgetItem*, C_Destination_Options& m_data);
    void OnDeleteImageView(QString& fileName);
    void OnDeleteImageDiffView(QString& fileName);
    void DeleteDock(acCustomDockWidget** dock);

    void onProcessing(QString& FilePathName);
    void imageDiff();

    void                SetRaised();
    acCustomDockWidget* FindImageView(QString& file, bool findDiffs = false);
    void                menuItemClicked(QAction* triggeredAction);

    // Received when a docked Image View changes visibility
    void onDockImageVisibilityChanged(bool visible);
    void OnAddCompressSettings(QTreeWidgetItem* item);
    void onEditCompressSettings(QTreeWidgetItem* item);

    void onAboutToClose(QString& Title);

    // File IO
    void openRecentFile();

    // Updates from Property View
    void onPropertyViewSaveSetting(QString* FilePathName);
    void onPropertyViewCompressImage(QString* FilePathName);

    // Remove Tab Views and update the items icon in project view
    void removeItemTabs(QString* FilePathName);

    // file menu bar
    void onAboutToShowFileMenu();

protected:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;

private slots:
    void openNewProject();
    bool saveProjectToBatchFile();
    void openProjectFile();
    void openImageFile();
    bool saveProjectFile();
    bool saveAsProjectFile();
    bool saveImageFile();
    void settings();
    void userGuide();
    void newFeaturesGuide();
    void gettingStarted();
    void about();
    void browserMsg(const char* msg);
    void OnWelcomePageButtonClick(QString& Request, QString& Msg);

    void onGenerateMipmapsMenuClicked();
    void convertModels();  // Dialog

#ifdef ENABLE_AGS_SUPPORT
    void handleHDRon();
#endif

    void generateMipmaps(CMP_CFilterParams m_CFilterParams, const std::vector<QTreeWidgetItem*>& images);  // Generate the MIP levels on selected item(s)

    void deleteImageFile();
    void onCompressionDone();
    void onCloseAllDocuments();
    void onCompressionStart();
    void onSourceImage(int childCount);
    void onDecompressImage();
    void onProjectLoaded(int childCount);
    void onAddedCompressSettingNode();
    void onAddedImageSourceNode();
    void onShowOutput();
    void onShowWelcomePage();

    void onWriteSettings();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();

    QMenu* fileMenu;
    QMenu* settingsMenu;
    QMenu* windowMenu;
    QMenu* helpMenu;

    QToolBar* fileToolBar;
    QToolBar* CompressionToolBar;
    QToolBar* ImageViewToolBar;

    QAction* openAct;
    QAction* openImageFileAct;
    QAction* newProjectAct;
    QAction* saveToBatchFileAct;
    QAction* exportAct;
    QAction* saveAct;
    QAction* saveAsAct;
    QAction* saveImageAct;
    QAction* exitAct;
    QAction* userGuideAct;
    QAction* newFeaturesAct;
    QAction* gettingStartedAct;
    QAction* aboutAct;
    QAction* settingsAct;
    QAction* aboutQtAct;
    QAction* closeAllDocuments;
    QAction* showOutputAct;
    QAction* showWelcomePageAct;
    QAction* deleteImageAct;

    // Compression Action used for toolbar
    QAction* compressAct;

    QAction* imagediffAct;

#ifdef ENABLE_AGS_SUPPORT
    QPushButton* onHDRButton;
#endif

    // MIP Generation Action used for toolbar
    QAction* MIPGenAct;
#ifdef USE_3DCONVERT
    QAction* ConvertModelAct;
#endif

#ifdef USE_MAIN_IMAVEVIEW_TOOLBAR
    // Image View Actions
    QAction* imageview_zoomInAct;
    QAction* imageview_zoomOutAct;
    QAction* imageview_RedAct;
    QAction* imageview_GreenAct;
    QAction* imageview_BlueAct;
    QAction* imageview_AlphaAct;
    QAction* imageview_FitScreenAct;
#endif

    // Configuration options for views
    bool m_showAppSettingsDialog;
    bool m_viewDiff;

Q_SIGNALS:

    void SetCurrentItem(QString& Title);
    void OnImageLoadStart();
    void OnImageLoadDone();
};

extern QString getFileExt(QString filePathName);

extern acProgressDlg* g_pProgressDlg;
extern bool           g_bAbortCompression;
extern bool           g_bCompressing;

#endif  // CPMAINCOMPONENTS_H
