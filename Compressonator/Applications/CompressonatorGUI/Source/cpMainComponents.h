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

#ifndef CPMAINCOMPONENTS_H
#define CPMAINCOMPONENTS_H

#include <QtWidgets>
#include <QMainWindow>
#include <QtXml/QDomDocument>
#include "cpImagePropertyView.h"
#include "cpGenMips.h"
#include "cpSetCompressOptions.h"
#include "cpSetApplicationOptions.h"
#include "cpProjectView.h"
#include "cpImageView.h"
#include "cpWelcomePage.h"
#include "cpImageCompare.h"
#include "cpCompressStatus.h"
#include "cmdline.h"

#include <QTimer>
#include "acCustomDockWidget.h"

// Progress Bar
#include <AMDTApplicationComponents/Include/acProgressDlg.h>

// CodeXL Help Dialog
#include <AMDTOSWrappers/Include/osProductVersion.h>
//#include <AMDTApplicationFramework/Include/afGlobalVariablesManager.h>
#include <AMDTApplicationComponents/Include/acHelpAboutDialog.h>

// //Reserved: GPUDecode

// Common Project Data Types
#include "cpProjectData.h"


#define PROJECT_EXTENSION   ".cprj"
#define ENV_COMPRESSONATOR_ROOT "COMPRESSONATOR_ROOT"

//
// This Class is defined as Static 
// to enable message handling via emit signals calls from static & global functions
// 
class signalMsgHandler : public QObject
{
    Q_OBJECT

public:
    signalMsgHandler() { };

Q_SIGNALS:
    void  signalMessage(const char *msg);
};


class cpMainComponents : public QMainWindow
{
    Q_OBJECT

public:
    static void  msgHandler(QtMsgType type, const char* msg);
    static void PrintStatus(char *buff);

    gtVector<gtString>   m_projectsRecentFiles;

    cpMainComponents(QDockWidget *root_dock, QMainWindow *parent = 0);
    ~cpMainComponents();

    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    void OpenCHMFile(QString fileName);

    QString strippedName(const QString &fullFileName);

    void SetProjectWindowTitle();
    void showProgressBusy(QString Message);
    void hideProgressBusy(QString Message);

    bool                    m_ForceImageRefresh;
    QString                 m_sSettingsFile;
    QString                 curFile;
    QMenu                   *recentFilesMenu;
    CWelcomePage            *m_welcomePage;
    CImagePropertyView      *m_imagePropertyView;
    CGenMips                *m_genmips;
    CSetCompressOptions     *m_setcompressoptions;
    CSetApplicationOptions  *m_setapplicationoptions;
    ProjectView             *m_projectview;
    cpImageView             *m_imageview;
    acCustomDockWidget      *m_blankpage;
    CImageCompare           *m_imageCompare;
    QDockWidget             *m_activeImageTab;      // Current viewing Image Tab
    QDockWidget             *app_welcomepage;
    QMainWindow             *m_parent;
    QDialog                 *m_frame;               // http://doc.qt.io/qt-4.8/qframe.html

    enum { MaxRecentFiles = 5 };
    int     m_numRecentFiles;                       // Counts the number of recent files loaded
    QAction *recentFileActs[MaxRecentFiles];
    QAction *separatorAct;

    CompressStatusDialog *  m_CompressStatusDialog;
    acHelpAboutDialog*      m_pacHelpAboutDialog;

    // from CodeXL 
    osExecutedApplicationType m_executionApplicationType;
    QString m_apptitle;
    QString  m_copyRightCaption;
    QString  m_copyRightInformation;
    acIconId m_productIconId;
    gtString m_versionCaption;
    gtString m_companyLogoBitmapString;
    osProductVersion m_appVersion;

    bool    isCompressInProgress;

public slots:
    void AddImageView(QString &fileName, QTreeWidgetItem * item);

    void AddImageDiff(C_Destination_Options *destination);
    void AddImageCompSettings(QTreeWidgetItem *, C_Destination_Options &m_data);
    void OnDeleteImageView(QString &fileName);
    void OnDeleteImageDiffView(QString &fileName);
    void onProcessing(QString &FilePathName);

    void SetRaised();
    acCustomDockWidget *FindImageView(QString &Title);
    void menuItemClicked(QAction* triggeredAction);

    // Received when a docked Image View changes visibility
    void onDockImageVisibilityChanged(bool visible);
    void OnAddCompressSettings(QTreeWidgetItem *item);
    void onEditCompressSettings(QTreeWidgetItem *item);

    void onAboutToClose(QString &Title);

    // File IO
    void openRecentFile();

    // Updates from Property View
    void onPropertyViewSaveSetting(QString *FilePathName);
    void onPropertyViewCompressImage(QString *FilePathName);

    // Remove Tab Views and update the items icon in project view
    void removeItemTabs(QString *FilePathName);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void openNewProject();
    bool saveProjectToBatchFile();
    void openProjectFile();
    void openImageFile();
    bool saveProjectFile();
    bool saveAsProjectFile();
    void settings();
    void userGuide();
    void gettingStarted();
    void about();
    void browserMsg(const char *msg);
    void OnWelcomePageButtonClick(QString &Request, QString &Msg);
    void genMIPMaps(); // Dialog
    void onGenerateMIPMap(int nMinSize); // Generate the MIP levels on selected item(s)
    void deleteImageFile();
    void onCompressionDone();
    void onCloseAllDocuments();
    void onCompressionStart();
    void onSourceImage(int childCount);
    void onDecompressImage();
    void onProjectLoaded(int childCount);
    void onAddedCompressSettingNode();
    void onShowOutput();
    void onShowWelcomePage();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();

    QMenu *fileMenu;
    QMenu *settingsMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;

    QToolBar *fileToolBar;
    QToolBar *CompressionToolBar;
    QToolBar *ImageViewToolBar;

    QAction *openAct;
    QAction *openImageFileAct;
    QAction *newProjectAct;
    QAction *saveToBatchFileAct;
    QAction *exportAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *userGuideAct;
    QAction *gettingStartedAct;
    QAction *aboutAct;
    QAction *settingsAct;
    QAction *aboutQtAct;
    QAction *closeAllDocuments;
    QAction *showOutputAct;
    QAction *showWelcomePageAct;

    QAction *deleteImageAct;

    // Compression Action used for toolbar
    QAction *compressAct;

    QAction *imagediffAct;

    // MIP Generation Action used for toolbar
    QAction *MIPGenAct;


#ifdef USE_MAIN_IMAVEVIEW_TOOLBAR
    // Image View Actions
    QAction *imageview_zoomInAct;
    QAction *imageview_zoomOutAct;
    QAction *imageview_RedAct;
    QAction *imageview_GreenAct;
    QAction *imageview_BlueAct;
    QAction *imageview_AlphaAct;
    QAction *imageview_FitScreenAct;
#endif

    // Configuration options for views
    bool  m_showAppSettingsDialog;
    bool  m_viewDiff;

Q_SIGNALS:
    
    void SetCurrentItem(QString &Title);
    void OnImageLoadStart();
    void OnImageLoadDone();

};

#endif // CPMAINCOMPONENTS_H
