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

#ifndef _PROJECTVIEW_H
#define _PROJECTVIEW_H

#include <QtWidgets>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>
#include <QFile>
#include <QtXml/QDomDocument>
#include <QMutex>
#include <QElapsedTimer>

#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "objectcontroller.h"

// Progress Bar
#include "acprogressdlg.h"
#include "acdiffimage.h"
#include "ac3dmeshanalysis.h"
#include "cptreewidget.h"
#include "cpimageview.h"
#include "cpprojectdata.h"
#include "cpsetcompressoptions.h"
#include "cpcompressstatus.h"
#include "cpimageloader.h"
#include "cpnewproject.h"
#include "textureio.h"
#include "modeldata.h"

// JSon
#include "json/json.hpp"

#include "cmp_fileio.h"

#define MAX_PROJECTVIEW_COLUMNS 3  // Project view columns to view

#define DIFFERENCE_IMAGE_TXT "Difference: "
#define DIFFERENCE_IMAGE_VS_TXT " _VS_ "

//
// This Class is defined as Static
// to enable message handling via emit signals calls from static & global functions
//
class signalProcessMsgHandler : public QObject
{
    Q_OBJECT

public:
    signalProcessMsgHandler(){};

Q_SIGNALS:
    void signalProcessMessage();
};

class cpItemDelegate : public QItemDelegate
{
public:
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QSize m_size;
        m_size.setHeight(32);
        m_size.setWidth(32);
        //const QTreeItem* ti(static_cast<TreeItem*>(index.internalPointer()));
        //if (ti->pixmap())
        //    return ti->pixmap()->size();
        QItemDelegate::sizeHint(option, index);
        return m_size;
    }
};

class AnalysisTableWidget : public QTableWidget
{
public:
    AnalysisTableWidget();

    void AddAverageResults(QString deviceName, QString Encoder, QString Time, QString psnr, QString ssim);

    void AddTestResults(std::string processPath, QString processName, float Quality, double PerfTime, double MPxPerSec, double Time, double psnr, double ssim);
    void ClearResults();

protected:
    void keyPressEvent(QKeyEvent* event);
};

class ProjectView : public QDockWidget
{
    Q_OBJECT
public:
    ProjectView(const QString title, CompressStatusDialog* StatusDialog, QWidget* parent);
    ~ProjectView();
    bool    m_saveProjectChanges;      // Flag if any changes were made to a project file
    bool    m_processFromContext;      // Flag if users click process from context
    QString m_curProjectFilePathName;  // Current Project Full Path and Name
    QString m_curProjectName;
    void    setCurrentProjectName(QString filePathName);  // Set the Member var above using filePathName

    QList<QByteArray> m_supportedFormats;  // List of Qt and Plugin file formats
    QString           m_ImageFileFilter;   // String version of m_supportedFormats formated for File Dialogs

    QString m_curDiffSourceFile;  //For View image diff from right click context (file#1)
    QString m_curDiffDestFile;    //For View image diff from right click context (file#2)

    CompressStatusDialog*     m_CompressStatusDialog;
    acProgressDlg*            m_pProgressDlg;
    acDiffImage*              m_diffImageDialog;
    ac3DMeshAnalysis*         m_3DMeshAnalysisDlg;
    C_Global_Process_Settings m_globalProcessSetting;

    void SetupHeader();
    void SetupTreeView();

    void run3DMeshAnalysis(CMODEL_DATA* meshData, CMODEL_DATA* meshDataOri);

    void diffImageFiles();
    bool loadProjectFile(QString fileToLoad);
    bool OpenImageFile();

    int  PromptSaveChanges();
    bool userSaveProjectAndContinue();

    QString GetSourceFileNamePath(QTreeWidgetItem* item);
    QString GetDestinationFileNamePath(QTreeWidgetItem* item);

    void showProgressDialog(QString header);
    void hideProgressDialog();

    void             Tree_AddRootNode();
    QTreeWidgetItem* Tree_AddImageFile(QString filePathName, int index, C_Source_Info** m_dataout);
    void Tree_AddCompressFile(QTreeWidgetItem* parent, QString description, bool checkable, bool checked, int levelType, C_Destination_Options* m_data);
    QTreeWidgetItem* Tree_Add3DModelImageFiles(QTreeWidgetItem*  ParentItem,
                                               QString           filePathName,
                                               bool              checkable,
                                               bool              checked,
                                               int               levelType,
                                               CMP_Feedback_Proc pFeedbackProc = NULL);
    QTreeWidgetItem* Tree_Add3DModelMeshFile(QTreeWidgetItem*  ParentItem,
                                             QString           filePathName,
                                             QString           pfilePathName,
                                             bool              checkable,
                                             bool              checked,
                                             int               levelType,
                                             CMP_Feedback_Proc pFeedbackProc = NULL);
    void             Tree_Add3DSubModelFile(QTreeWidgetItem* ParentItem, QString filePathName, QList<bool>* srcDelFlags);

    void AddSettingtoEmptyTree();

    // Changes the Icon for the item if file exists or not return true if file exists
    // Red Icon or Null is used on Files that dont exist
    bool             Tree_updateCompressIcon(QTreeWidgetItem* item, QString FilePathName, bool RedIcon);
    QTreeWidgetItem* Tree_SetCurrentItem(QString FilePathName);
    void             Tree_setAllItemsSelected();

    // Returns a list of all image items that have been selected, does not contain destination items
    std::vector<QTreeWidgetItem*> GetAllSelectedImages();

    void Tree_clearAllItemsSelected();
    void SelectImageItem(QString filePathName);
    bool AnySelectedItems();
    bool saveImageAs();
    void Tree_selectAllChildItems(QTreeWidgetItem* item);

    // Get the number of items in the project tree that match one of the bitwise or'd type flags, or all items if typeFlags is 0
    int GetNumItemsByType(int typeFlags);
    // Return the number of items in the project tree, minus the "add" items
    int GetNumItems();
    // Return the number of compression destination items, either from images or 3D models
    int GetNumDestItems();

    // Get the number of selected items in the project tree that match one of the bitwise or'd type flags (checks all if typeFlags == 0)
    int GetNumSelectedItemsByType(int typeFlags);
    // Return the number of selected items in the project tree, minus the "add" items
    int GetNumSelectedItems();
    // Return the number of selected compression destination items, either from images or 3D models
    int GetNumSelectedDestItems();

    QTreeWidgetItem* GetCurrentItem(int inLevelType);
    QTreeWidgetItem* GetCurrentItem();

    QTreeWidgetItem* Tree_FindImageItem(QString filePathName, bool includeDestination);
    QTreeWidgetItem* ContextMenu_ImageItem;

    // Delete Items
    void             DeleteAllSeletedItems(bool RemoveFromDisk);
    void             clearProjectTreeView();
    void             UserDeleteItems();
    void             DeleteItemData(QTreeWidgetItem* item, bool userdeleted = false);
    QTreeWidgetItem* DeleteSelectedItemData(QTreeWidgetItem* item, bool RemoveFromDisk);
    void             SignalUpdateData(QTreeWidgetItem* item, int levelType);

    // Compression
    // TODO: this function probably shouldn't in this class? There is a lot of cleaning up that could be done here
    void CompressSelectedItems();

    QAction* actOpenContainingFolder;
    QAction* actCopyFullPath;
    QAction* actsaveProjectFile;
    QAction* actopenProjectFile;
    QAction* actAnalyseMeshData;
    QAction* actCompressProjectFiles;
    QAction* actSeperator;
    QAction* actViewImageDiff;
    QAction* actView3DModelDiff;
    QAction* actRemoveImage;

    // Tracks Compressed image items user has clicked on in project view, Null if not on item
    QTreeWidgetItem* m_CurrentCompressedImageItem;

    QMenu*           m_contextMenu;
    CImageLoader*    m_imageloader;  // Util that is used to load image files to MIP and Image view files
    QTreeWidgetItem* m_CurrentItem;

    cpNewProject* m_newProjectwindow;
    QElapsedTimer m_elapsedTimer;

    QStringList m_ImagesinProjectTrees;
    bool        m_clicked_onIcon;  // User clicked on the ICON of a treeview item

    AnalysisTableWidget m_analysisTable;

Q_SIGNALS:

    void AddedImageFile(QString& fileName);
    void ViewImageFile(QString& fileName, QTreeWidgetItem* item);
    void DeleteImageView(QString& fileName);

    void ViewImageFileDiff(C_Destination_Options* m_data, QString& file1, QString& file2);
    void View3DModelFileDiff(C_3DSubModel_Info* m_data, QString& file1, QString& file2);

    void DeleteImageViewDiff(QString& fileName);

    void AddCompressSettings(QTreeWidgetItem* item);
    void EditCompressSettings(QTreeWidgetItem* item);

    void UpdateData(QObject* data);

    // Signals when processing image for compression
    void OnCompressionDone();
    void OnProcessing(QString& FilePathName);
    void OnCompressionStart();

    // Signals when current selection is on the specified image type
    void OnSourceImage(int ChildCount);
    void OnDecompressImage();

    // Signal when a project file is loaded
    void OnProjectLoaded(int ChildCount);

    // Signal when a compressed item node is added to tree view
    void OnAddedCompressSettingNode();

    // Signal when a image source item node is added to tree view
    void OnAddedImageSourceNode();

    // Signals to main app to enable or disable tool bar actions
    // when user clicks on a project view item
    void OnSetToolBarActions(int islevelType);

public slots:

    void onTree_ItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onTree_ItemClicked(QTreeWidgetItem* item, int column);
    void onDroppedImageItem(QString& filePathName, int index);
    void onSetCurrentItem(QString& FilePathName);
    void onEntered(const QModelIndex& index);

    void OnGlobalMessage(const char* msg);
    void onSetNewProject(QString& FilePathName);

    // activated when the user right-clicks and presses the "process image" button
    void onContextMenuCompression();

    // activated when the user clicks the compression button on the menu bar
    void onMenuBarCompression();

    void OnCloseCompression();
    void onSignalProcessMessage();

    void onGlobalPropertyChanged(int&);

    void saveProjectFile();
    void saveAsProjectFile();
    void saveToBatchFile();
    void openProjectFile();
    void openNewProjectFile();
    void compressProjectFiles(QFile* file);
    void viewImageDiff();
    void analyseMeshData();
    void viewDiffImageFromChild();
    void viewDiff3DModelFromChild();
    void removeSelectedImage();
    void openContainingFolder();
    void copyFullPath();

    void onCustomContextMenu(const QPoint& point);

    void onTreeMousePress(QMouseEvent* event, bool onIcon);
    void onTreeKeyPress(QKeyEvent* event);

    void onImageLoadStart();
    void onImageLoadDone();

private:
    void AddDefaultDestToItems(const std::vector<QTreeWidgetItem*>& items);

public:
    // Common for all
    const QString    m_title;
    QWidget*         m_newWidget;
    QGridLayout*     m_layout;
    QWidget*         m_parent;
    cpTreeWidget*    m_projectTreeView;
    QTreeWidgetItem* m_treeRootItem;        // Root of Tree view Items
    QString          m_RecentImageDirOpen;  // Keep track of recent image files opened dir
    void             keyPressEvent(QKeyEvent* event);
    bool             m_EnableCheckedItemsView;  // This flags by default if set to false and enables compression of items
    //  based on checked flags along with highlited items.
    int  m_NumItems;
    bool m_AllItemsSelected;
    bool m_processBusy;
};
#endif
