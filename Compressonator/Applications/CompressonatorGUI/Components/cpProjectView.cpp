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

#include "cpProjectView.h"
#include "cmdline.h"

#include "cpMainComponents.h"

// ToDo(s)
// Clean up allocated memory for each tree nodes m_data


#define STR_AddDestinationSetting "Add destination settings..."


extern bool        g_bAbortCompression;
extern C_Application_Options::ImageEncodeWith encodewith;
bool g_useCPUEncode = true;
static signalProcessMsgHandler static_processmsghandler;
extern void GetSupportedFileFormats(QList<QByteArray> &g_supportedFormats);
extern PluginManager g_pluginManager;

void OnCancel()
{
    g_bAbortCompression = true;
}

ProjectView::ProjectView(const QString title, CompressStatusDialog *StatusDialog, QWidget *parent)
{
    setWindowTitle(title);
    m_parent = parent;
    m_CompressStatusDialog = StatusDialog;

    m_processBusy = false;

    // Tracks number of items added to the Project View
    // Includes (+) Add ... items

    m_NumItems = 0;

    // True when any changes were made to the project

    m_saveProjectChanges  = false;
    m_AllItemsSelected    = false;
    ContextMenu_ImageItem = NULL;

    // Tracks what Compressed Image Tree Item has been clicked on
    m_CurrentCompressedImageItem = NULL;
    m_bCompressing = false;

    // Enables diplay of checked box next to items
    m_EnableCheckedItemsView = false;

    m_processFromContext = false;

    m_newWidget = new QWidget(parent);
    m_layout    = new QGridLayout(m_newWidget);
    SetupTreeView();
    m_layout->addWidget(m_projectTreeView);
    m_newWidget->setLayout(m_layout);
    setWidget(m_newWidget);

    m_curProjectFilePathName = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if(!QDir(m_curProjectFilePathName).exists())
        QDir().mkdir(m_curProjectFilePathName);

    m_curProjectFilePathName.append("/");
    m_curProjectFilePathName.append("NewProject");
    m_curProjectName = "NewProject";

    // For view image diff from selected child file
    m_curDiffSourceFile = "";
    m_curDiffDestFile = "";

    // Get Qt image read formats
    GetSupportedFileFormats(m_supportedFormats);
    // add list to Image Dialog Filter variable
    m_ImageFileFilter = "Images ("; //  "Image Files (*.*);;";

    QString imageList ="";

    QList<QByteArray>::Iterator i;
    for (i = m_supportedFormats.begin(); i != m_supportedFormats.end(); ++i)
    {
        QByteArray fformat = (*i);
        m_ImageFileFilter.append("*.");
        m_ImageFileFilter.append(fformat);
        m_ImageFileFilter.append(" ");
        
        imageList.append("*.");
        imageList.append(fformat);
        imageList.append(";;");
    }

    m_ImageFileFilter.append(");;");
    m_ImageFileFilter.append(imageList);

    connect(&static_processmsghandler, SIGNAL(signalProcessMessage()), this, SLOT(onSignalProcessMessage()));

    // Diff any 2 image file dialog 
    m_diffImageDialog = new acDiffImage(this);
    m_diffImageDialog->hide();

    // Image loader
    m_imageloader       = new CImageLoader();
    m_newProjectwindow  = new cpNewProject();
    connect(m_newProjectwindow, SIGNAL(OnSetNewProject(QString &)), this, SLOT(onSetNewProject(QString &)));

}

void ProjectView::SetupTreeView()
{
    m_projectTreeView = new cpTreeWidget(this);

#ifdef USE_DELEGATE    
    cpItemDelegate *Linedelegate = new cpItemDelegate;
    m_projectTreeView->setItemDelegate(Linedelegate);
#endif

    m_projectTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_projectTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_projectTreeView->setColumnCount(MAX_PROJECTVIEW_COLUMNS);
    m_projectTreeView->hideColumn(1);
    m_CurrentItem = NULL;
        
    // Set Icon size 
    QSize size;
    size.setHeight(32);
    size.setWidth(32);
    m_projectTreeView->setIconSize(size);

    // Hide the tree view header and allow first column to stretch
    QHeaderView *Header = m_projectTreeView->header();

    Header->setResizeMode(0, QHeaderView::ResizeToContents);
    Header->hide();

    m_projectTreeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_projectTreeView->setAcceptDrops(true);
    
    connect(m_projectTreeView, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onTree_ItemClicked(QTreeWidgetItem *, int)));
    connect(m_projectTreeView, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(onTree_ItemDoubleClicked(QTreeWidgetItem *, int)));
    connect(m_projectTreeView, SIGNAL(DroppedImageItem(QString &, int)), this, SLOT(onDroppedImageItem(QString &, int)));
    connect(m_projectTreeView, SIGNAL(entered(const QModelIndex &)), this, SLOT(onEntered(const QModelIndex &)));
    connect(m_projectTreeView, SIGNAL(event_mousePress(QMouseEvent  *)), this, SLOT(onTreeMousePress(QMouseEvent  *)));
    connect(m_projectTreeView, SIGNAL(event_keyPress(QKeyEvent  *)), this, SLOT(onTreeKeyPress(QKeyEvent  *)));

    // Top level Root Node
    Tree_AddRootNode();

    // Context Menu Items
    // m_projectTreeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_projectTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_projectTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));


    actOpenContainingFolder = new QAction("Open containing folder", this);
    actCopyFullPath            = new QAction("Copy full path", this);

#ifdef USE_CONTEXT_PROJECT
    actsaveProjectFile      = new QAction("Save Project", this);
    actopenProjectFile      = new QAction("Open Project", this);
#endif

    actCompressProjectFiles = new QAction("Compress selected images", this);
    actViewImageDiff        = new QAction("View Image Diff", this);
    actViewImageDiff->setEnabled(false);
    actRemoveImage          = new QAction("Delete selected image", this);
    actSeperator = new QAction(this);
    actSeperator->setSeparator(true);


    connect(actOpenContainingFolder, SIGNAL(triggered()), this, SLOT(openContainingFolder()));
    connect(actCopyFullPath, SIGNAL(triggered()), this, SLOT(copyFullPath()));

#ifdef USE_CONTEXT_PROJECT
    connect(actsaveProjectFile, SIGNAL(triggered()), this, SLOT(saveProjectFile()));
    connect(actopenProjectFile, SIGNAL(triggered()), this, SLOT(openProjectFile()));
#endif

    connect(actCompressProjectFiles, SIGNAL(triggered()), this, SLOT(onShowCompressStatus()));
    connect(actViewImageDiff, SIGNAL(triggered()), this, SLOT(viewDiffImageFromChild()));
    connect(actRemoveImage, SIGNAL(triggered()), this, SLOT(removeSelectedImage()));

    m_contextMenu = new QMenu(m_projectTreeView);


    m_contextMenu->addAction(actCompressProjectFiles);
    m_contextMenu->addAction(actSeperator);
    m_contextMenu->addAction(actOpenContainingFolder);
    m_contextMenu->addAction(actCopyFullPath);
#ifdef USE_CONTEXT_PROJECT
    m_contextMenu->addAction(actsaveProjectFile);
    m_contextMenu->addAction(actopenProjectFile);
#endif
    m_contextMenu->addAction(actSeperator);
    m_contextMenu->addAction(actViewImageDiff);
    m_contextMenu->addAction(actSeperator);
    m_contextMenu->addAction(actRemoveImage);
    
    // Progress Dialog During Compression
    m_pProgressDlg = new acProgressDlg();
    m_pProgressDlg->setParent(this);
    m_pProgressDlg->setWindowFlags(Qt::FramelessWindowHint|Qt::Window);
    m_pProgressDlg->ShowCancelButton(true, &OnCancel);
    m_pProgressDlg->SetHeader("");
    m_pProgressDlg->SetLabelText("");
    m_pProgressDlg->SetRange(0, 100);
    m_pProgressDlg->hide();


}

void ProjectView::onDroppedImageItem(QString &filePathName, int index)
{
    C_Source_Image *m_dataout = NULL;
    Tree_AddImageFile(filePathName, index, &m_dataout);
    m_saveProjectChanges = true;
}

void ProjectView::Tree_AddRootNode()
{
    m_treeRootItem = new QTreeWidgetItem(m_projectTreeView);
    m_treeRootItem->setFlags(Qt::ItemIsEnabled);
    m_NumItems++;

    // Keep first char as space so when treeview is sorted by file path its aways on top of view
    m_treeRootItem->setText(0, "Double Click here to add files...");
    m_treeRootItem->setIcon(0, QIcon(":/CompressonatorGUI/Images/plus.png"));
    // This item has gray color
    QFont font("", 9, QFont::Bold);
    QBrush b(Qt::gray);
    m_treeRootItem->setForeground(0, b);
    m_treeRootItem->setFont(0, font);

    m_treeRootItem->setData(0, Qt::UserRole, QVariant::fromValue(TREETYPE_ADD_IMAGE_NODE));

}

QTreeWidgetItem * ProjectView::Tree_AddImageFile(QString filePathName, int index, C_Source_Image **m_dataout)
{
    Q_UNUSED(index);
    
    QTreeWidgetItem *treeItem = NULL;
    QString filename;
    QFile f(filePathName);
    
    // file not found!
    if (!f.exists())
    {
        return NULL;
    }

    // Check if item already exist if so just exit
    if (Tree_FindImageItem(filePathName, false))
    {
        return NULL;
    }

    // Get file Extension and check if it can be loaded by our AMD plugin or is supported by QT, return if is unsupported format
    QFileInfo fi(filePathName.toUpper());
    QString name = fi.fileName();
    QStringList list1 = name.split(".");
    QString PlugInType = list1[list1.size() - 1];
    QByteArray ba = PlugInType.toLatin1();
    const char *Ext = ba.data();
    QImageReader imageFormat(filePathName);
    if (!(imageFormat.canRead()) && !(g_pluginManager.PluginSupported("IMAGE", (char *)Ext)))
    {
        return NULL;
    }

    //.cprj is detected as .svg file by qt support format so alway can be drop, below is the fix
    if (strcmp(Ext,"CPRJ")==0)
        return NULL;

    //check the support format detected by QT
    //QByteArray supportFormat = QImageReader::imageFormat(filePathName);

    treeItem = new QTreeWidgetItem(m_projectTreeView);
    m_NumItems++;

    // --------------------------------------------
    // Set the Image items size to  32x32 
    // for the original image on the Project View
    //---------------------------------------------
    QPixmap filepixmap(filePathName);
    if (filepixmap.size().height() == 0)
    {
        filepixmap.load(":/CompressonatorGUI/Images/file.png");
    }

    QPixmap newPixmap = filepixmap.scaled(QSize(32, 32), Qt::IgnoreAspectRatio);
    QIcon treeicon;

    treeicon.addPixmap(newPixmap, QIcon::Normal, QIcon::On);

    treeItem->setSizeHint(0, QSize(33, 33));
    treeItem->setIcon(0, treeicon); //  QIcon(filePathName));

    treeItem->setToolTip(0, filePathName);


    QFileInfo fileInfo(f.fileName());
    filename = fileInfo.fileName();
    treeItem->setText(0, filename);
    treeItem->setData(0, Qt::UserRole, QVariant::fromValue(TREETYPE_IMAGEFILE_DATA));

    // Create Data for this file and set current know valaes
    C_Source_Image *m_data = new C_Source_Image();
    if (m_data == NULL)
    {
        return NULL;
    }

    *m_dataout = m_data;

    m_data->m_Name = filename;
    m_data->m_Full_Path = filePathName;
    QFile file(filePathName);
    m_data->m_FileSize = file.size();

    if (m_data->m_FileSize > 1024000)
        m_data->m_FileSizeStr = QString().number((double)m_data->m_FileSize / 1024000, 'f', 2) + " MB";
    else
        if (m_data->m_FileSize > 1024)
        m_data->m_FileSizeStr = QString().number((double)m_data->m_FileSize / 1024,'f',1) + " KB";
    else
        m_data->m_FileSizeStr = QString().number(m_data->m_FileSize) + " Bytes";

    // Load the Image File and MIP data!
    m_data->m_MipImages = m_imageloader->LoadPluginImage(filePathName);

    // Mip levels
    if (m_data->m_MipImages)
    {
        if (m_data->m_MipImages->mipset)
        {
            if (m_data->m_MipImages->mipset->m_nMipLevels > 1)
                m_data->m_Mip_Levels = m_data->m_MipImages->mipset->m_nMipLevels - 1;

            m_data->m_Height        = m_data->m_MipImages->mipset->m_nHeight;
            m_data->m_Width         = m_data->m_MipImages->mipset->m_nWidth;
            m_data->m_HeightStr     = QString().number(m_data->m_Height) + " px";
            m_data->m_WidthStr      = QString().number(m_data->m_Width) + " px";
            m_data->m_FormatStr     = GetFormatDesc(m_data->m_MipImages->mipset->m_format);
            m_data->m_Format        = m_data->m_MipImages->mipset->m_format;

            CMIPS CMips;
            MipLevel* pInMipLevel = CMips.GetMipLevel(m_data->m_MipImages->mipset, 0, 0);
            if (pInMipLevel)
            {
                m_data->m_ImageSize = pInMipLevel->m_dwLinearSize;

                if (m_data->m_ImageSize > 1024000)
                    m_data->m_ImageSizeStr = QString().number((double)m_data->m_ImageSize / 1024000, 'f', 2) + " MB";
                else
                    if (m_data->m_ImageSize > 1024)
                        m_data->m_ImageSizeStr = QString().number((double)m_data->m_ImageSize / 1024, 'f', 1) + " KB";
                    else
                        m_data->m_ImageSizeStr = QString().number(m_data->m_ImageSize) + " Bytes";
            }
            else
                m_data->m_ImageSizeStr = UNKNOWN_IMAGE;

        }
    }

    treeItem->setData(1, Qt::UserRole, QVariant::fromValue(m_data));

    // Add compression setting option under the new item
    Tree_AddCompressFile(treeItem, STR_AddDestinationSetting, false, false, TREETYPE_IMAGEFILE_DATA_NODE, NULL);

    // Add the image to the diff image list if it is not in the list
    if(!(m_ImagesinProjectTrees.contains(filePathName)))
        m_ImagesinProjectTrees.append(filePathName);

    return treeItem;
}

bool ProjectView::Tree_updateCompressIcon(QTreeWidgetItem *item, QString FileNamePath, bool RedIcon)
{
   if (!item) return false;

   bool result = false;
   QFileInfo fileinfo(FileNamePath);
   QFile file(FileNamePath);
   if (file.exists() && (fileinfo.suffix().length() > 0))
   {
       item->setIcon(0, QIcon(QStringLiteral(":/CompressonatorGUI/Images/smallGreenStone.png")));
       result = true;
   }
   else
   {
       if (RedIcon)
            item->setIcon(0, QIcon(QStringLiteral(":/CompressonatorGUI/Images/smallRedStone.png")));
       else
           item->setIcon(0, QIcon(QStringLiteral(":/CompressonatorGUI/Images/smallWhiteBlank.png")));
   }

   return result;
}

void ProjectView::Tree_AddCompressFile(QTreeWidgetItem *ParentItem, QString description, bool checkable, bool checked, int levelType, C_Destination_Options *m_data)
{
    if (ParentItem == NULL) return;

    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    m_NumItems++;

    treeItem->setText(0, description);
    treeItem->setData(0, Qt::UserRole, QVariant::fromValue(levelType));

    if (levelType == TREETYPE_IMAGEFILE_DATA_NODE)
    {
        // This item has gray color
        QFont font("", 9, QFont::Bold);
        QBrush b(Qt::gray);
        treeItem->setForeground(0, b);
        treeItem->setFont(0, font);
        ParentItem->addChild(treeItem);
        treeItem->setFlags(Qt::ItemIsEnabled);
        treeItem->setIcon(0, QIcon(":/CompressonatorGUI/Images/plusSettings.png"));
    }

    // we are adding a compression setting.
    if ((levelType == TREETYPE_COMPRESSION_DATA) && (m_data))
    {

#ifdef SAVE_TEMP_FILE
        // Create a temp decompress file for analysis later 
        m_data->CreateTempFile();
#endif

        // Check source file extension for special cases
        QFileInfo fi(m_data->m_sourceFileNamePath);
        QString ext = fi.suffix().toUpper();
        // if (ext.compare("EXR") == 0)
        // {
        //     m_data->m_settoUseOnlyBC6 = true;
        //     if (
        //         (m_data->m_Compression != C_Destination_Options::BC6H) ||
        //         (m_data->m_Compression != C_Destination_Options::ASTC) )
        //                     m_data->m_Compression = C_Destination_Options::BC6H;
        // }

        if (checkable && m_EnableCheckedItemsView)
        {
            treeItem->setFlags(treeItem->flags() | Qt::ItemIsUserCheckable);
            if (checked)
                treeItem->setCheckState(0, Qt::Checked);
            else
                treeItem->setCheckState(0, Qt::Unchecked);
        }
        else treeItem->setFlags(treeItem->flags() | Qt::ItemIsSelectable);
        
        // treeItem->setFlags(treeItem->flags() | Qt::ItemIsEditable);
        treeItem->setData(1, Qt::UserRole, QVariant::fromValue(m_data));
        treeItem->setToolTip(0, m_data->m_destFileNamePath);
        Tree_updateCompressIcon(treeItem, m_data->m_destFileNamePath, false);

        // parent is always 
        // TREETYPE_IMAGEFILE_DATA
        QTreeWidgetItem *parent = ParentItem->parent();

        // if parent is null, image itself is parent
        if (!parent)
        {
            parent = ParentItem;
        }

        if (parent)
        {
            parent->addChild(treeItem);
            QVariant v = parent->data(1, Qt::UserRole);
            C_Source_Image *imagedata = v.value<C_Source_Image *>();
            if (imagedata)
            {
                m_data->m_Width  = imagedata->m_Width;
                m_data->m_Height = imagedata->m_Height;
                m_data->m_HeightStr = QString().number(m_data->m_Height) + " px";
                m_data->m_WidthStr  = QString().number(m_data->m_Width)  + " px";
                m_data->m_SourceIscompressedFormat = CompressedFormat(imagedata->m_Format);
                m_data->m_SourceIsFloatFormat = FloatFormat(imagedata->m_Format);
                m_data->m_OriginalMipImages = imagedata->m_MipImages;

            }
            
        }
        ParentItem->setExpanded(true);
        treeItem->setExpanded(true);

        emit OnAddedCompressSettingNode();
}

#ifdef ADD_TREEWIDGET_ITEM
    QLabel *label = new QLabel("");
    m_projectTreeView->setItemWidget(treeItem, 1, label);
#endif

}

void ProjectView::setCurrentProjectName(QString filePathName)
{
    QFileInfo fileInfo(filePathName);
    QString filename;
    if (filePathName.contains(".cprj"))
        filename = fileInfo.completeBaseName();
    else
    {
        QString fitempo = QString(filePathName + ".cprj");
        QFileInfo fileInfo2(fitempo);
        filename = fileInfo2.completeBaseName();
    }

    if (fileInfo.isWritable())
    {
        m_curProjectFilePathName = filePathName;
    }
    else
    {
        QFileInfo fileInfo(m_curProjectFilePathName);
        m_curProjectFilePathName  = fileInfo.dir().path();
        m_curProjectFilePathName.append(QDir::separator());
        m_curProjectFilePathName.append(filename);
        m_curProjectFilePathName.replace("/", "\\");
    }

    m_curProjectName = filename;

    if (m_parent)
        m_parent->setWindowTitle(filename);
}

void ProjectView::onSetCurrentItem(QString &FilePathName)
{
    SelectImageItem(FilePathName);
}

void ProjectView::SignalUpdateData(QTreeWidgetItem * item, int levelType)
{
    if (!item) return;

    QVariant v = item->data(1, Qt::UserRole);
    switch (levelType)
    {
    case TREETYPE_COMPRESSION_DATA:
    {
        C_Destination_Options *m_data = v.value<C_Destination_Options *>();
        if (m_data)
            emit UpdateData(m_data);
    }
        break;
    case TREETYPE_IMAGEFILE_DATA:
    default:
    {
        C_Source_Image *imagedata = v.value<C_Source_Image *>();
        if (imagedata)
        {
            // Need a better way to do this via Objects DATA Properties
            if (imagedata->m_MipImages->mipset)
            {
                if (imagedata->m_MipImages->mipset->m_nMipLevels >= 1)
                    imagedata->m_Mip_Levels = imagedata->m_MipImages->mipset->m_nMipLevels;
            }
            emit UpdateData(imagedata);
        }
    }
        break;
    }
}

void ProjectView::onCustomContextMenu(const QPoint &point)
{
    QModelIndex index = m_projectTreeView->indexAt(point);

    // Show or Hide thes Context menu items based on location in the Project View
    actOpenContainingFolder->setVisible(false);
    actCopyFullPath->setVisible(false);
#ifdef USE_CONTEXT_PROJECT
    actsaveProjectFile->setVisible(false);
    actopenProjectFile->setVisible(false);
#endif
    actCompressProjectFiles->setVisible(false);
    actViewImageDiff->setVisible(false);
    actRemoveImage->setVisible(false);
    ContextMenu_ImageItem = NULL;

    if (index.isValid() ) 
    {
        QString text;
        text = "Process ";

        // Get the item user right clicked on
        QTreeWidgetItem *item = m_projectTreeView->itemAt(point);
        if (item)
        {
            actViewImageDiff->setEnabled(false);

            // Get number of setected Items
            QList<QTreeWidgetItem *> selected = m_projectTreeView->selectedItems();

            int ItemsCount;
            int NumCompressItems;
            int numSelected = Tree_numCompresstemsSelected(ItemsCount, NumCompressItems);

            if (NumCompressItems == 0)
                actCompressProjectFiles->setVisible(false);
            else
                actCompressProjectFiles->setVisible(true);
            
            if (numSelected > 1)
            {
                text.append(QString::number(numSelected));
                text.append(" selected image(s)");
                actCompressProjectFiles->setText(text);
            }

            QVariant v = item->data(0, Qt::UserRole);
            int levelType = v.toInt();
            switch (levelType)
            {
                case TREETYPE_ADD_IMAGE_NODE     : // [+] Add Image item
                case TREETYPE_IMAGEFILE_DATA_NODE: // STR_AddDestinationSetting
                {
                    if (numSelected > 1)
                    {
                        actCompressProjectFiles->setText("Process selected images");
                    }
                    else
                        actCompressProjectFiles->setVisible(false);
                    break;
                }

                case TREETYPE_IMAGEFILE_DATA:         // Original Image item
                {
#ifdef USE_CONTEXT_PROJECT
                    actsaveProjectFile->setVisible(true);
                    actopenProjectFile->setVisible(true);
#endif
                    actOpenContainingFolder->setVisible(true);
                    actCopyFullPath->setVisible(true);
                    ContextMenu_ImageItem = item;
                    QVariant fv = item->data(1, Qt::UserRole);
                    C_Source_Image *m_data = fv.value<C_Source_Image *>();

                    if ((NumCompressItems > 0) && (item->childCount() > 1))
                    {
                        if (m_data && (numSelected < (item->childCount() -1)))
                        {
                            text = "Process all setting for ";
                            text.append(m_data->m_Name);
                            actCompressProjectFiles->setText(text);
                            Tree_selectAllChildItems(item);
                        }
                        actCompressProjectFiles->setVisible(true);
                    }
                    break;
                }

                case TREETYPE_COMPRESSION_DATA  : // Compress Image item
                {
                    if (NumCompressItems > 0)
                    {
                        actOpenContainingFolder->setVisible(true);
                        actCopyFullPath->setVisible(true);
                        m_CurrentCompressedImageItem = item;
                        ContextMenu_ImageItem = item;
                        QVariant cv = item->data(1, Qt::UserRole);
                        C_Destination_Options *m_data = cv.value<C_Destination_Options *>();
                        if (m_data)
                        {
                            QFileInfo fileinfo(m_data->m_destFileNamePath);
                            QFile file(m_data->m_destFileNamePath);
                            bool fileexist = file.exists();
                            actViewImageDiff->setVisible(fileexist);
                            actViewImageDiff->setEnabled(fileexist);
                        }
                        if (m_data)
                        {
                            text.append(m_data->m_compname);
                            actCompressProjectFiles->setText(text);
                        }
                        actCompressProjectFiles->setVisible(true);
                        actRemoveImage->setVisible(true);
                    }
                    break;
                }
            }
            m_contextMenu->exec(m_projectTreeView->mapToGlobal(point));
        }
    }
}

//=====================================================
// This gets call many times (for all qDebug messages and printf's)
void ProjectView::OnGlobalMessage(const char *msg)
{
    if (m_CompressStatusDialog && m_bCompressing)
    {
        m_CompressStatusDialog->appendText(msg);
    }
}

void ProjectView::onShowCompressStatus()
{
    m_processFromContext = true;
    if (m_CompressStatusDialog)
    {
        g_bAbortCompression = false;
        m_CompressStatusDialog->onClearText();
        m_CompressStatusDialog->showOutput();
        OnStartCompression();
    }
}

void ProjectView::OnCloseCompression()
{
    if (m_CompressStatusDialog)
    {
        g_bAbortCompression = true;
        m_CompressStatusDialog->hideOutput();
    }
}


bool ProjectView::AnySelectedItems()
{
    
    bool userSelected = false;

    int ItemsCount;
    int NumCompressItems;
    int numSelected = Tree_numCompresstemsSelected(ItemsCount, NumCompressItems);

    if (numSelected > 0)
    {
        userSelected = true;
    }

    if (m_CompressStatusDialog)
    {
        m_CompressStatusDialog->onClearText();
        m_CompressStatusDialog->showOutput();
    }

    if (NumCompressItems == 0)
    {
        if (m_CompressStatusDialog)
        {
            m_CompressStatusDialog->appendText("Please open or create a project file and add destination images to process!");
        }
        return false;
    }
    else

    if (!userSelected)
    {
        // We did not find any selected compress images 
        // Try the current selected item
        QTreeWidgetItem *item = m_projectTreeView->currentItem();

        // Try project view items
        if (!item)
        {
            // Parse the Project view tree
            QTreeWidgetItemIterator it(m_projectTreeView);
            while (*it) {

                QString     name = (*it)->text(0);
                QVariant v = (*it)->data(0, Qt::UserRole);
                int levelType = v.toInt();
                int childcount = (*it)->childCount();

                if (levelType == TREETYPE_IMAGEFILE_DATA)
                {
                    // "Source File" = FilePathName);
                    if (childcount >= 1)
                    {
                        item = (*it);
                        break;
                    }
                }

                // last item should be a vaild one unless we ended in a 
                // child loop that was the last element in the tree if so break out were done..
                if (*it)
                    ++it;
                else
                    break;
            }
        }

        if (item->childCount() > 1)
        {
            Tree_selectAllChildItems(item);
        }
        else
        {
            Tree_setAllItemsSetected();
            m_AllItemsSelected = true;
        }
    }

    return true;
}

bool ProjectView::saveImageAs()
{
    QTreeWidgetItem *item = m_projectTreeView->currentItem();
    if (!item)
    {
        if (m_CompressStatusDialog)
        {
            m_CompressStatusDialog->appendText("Please load or click to select the image that you would like to save."); 
        }
        return false;
    }
    else
    {
            QVariant v = item->data(1, Qt::UserRole);
            QString tempName = item->text(0);
            QFileInfo fileInfo(tempName);
            QString imgFileName = fileInfo.completeBaseName();
            imgFileName.append("_saved");
            C_Source_Image *data = v.value<C_Source_Image *>();

            if (data->m_MipImages)
                if (data->m_MipImages->mipset)
                {
                  
                    QString filePathName = QFileDialog::getSaveFileName(this, tr("Save image as"), imgFileName, tr("Image files (*.dds)"));
                    if (filePathName.length() == 0) return false;
                    
                    if (m_parent)
                        m_parent->setWindowTitle(imgFileName);
                    
                    PluginInterface_Image *plugin_Image;
                    plugin_Image = reinterpret_cast<PluginInterface_Image *>(g_pluginManager.GetPlugin("IMAGE", "DDS"));
                    imgFileName.append(".dds");
                    
                    if (AMDSaveMIPSTextureImage(filePathName.toStdString().c_str(), data->m_MipImages->mipset, false) != 0)
                    {
                        if (m_CompressStatusDialog)
                        {
                            m_CompressStatusDialog->onClearText();
                            m_CompressStatusDialog->showOutput();
                        }
                        PrintInfo("Error: saving image fail.\n");
                        return false;
                    }
                    if (m_CompressStatusDialog)
                    {
                        m_CompressStatusDialog->onClearText();
                        m_CompressStatusDialog->showOutput();
                    }
                    PrintInfo("Image file: %s saved successfully.\n", imgFileName.toStdString().c_str());
                }
                else
                {
                    if (m_CompressStatusDialog)
                    {
                        m_CompressStatusDialog->onClearText();
                        m_CompressStatusDialog->showOutput();
                    }
                    PrintInfo("Error: Image mipset not found. Saving failed.\n");
                    return false;
                }

    }
    return true;
}

void ProjectView::OnStartCompression()
{
    if (!AnySelectedItems())
    {
        AddSettingtoEmptyTree();
        if (g_bAbortCompression) return;
        Tree_setAllItemsSetected();
        m_AllItemsSelected = true;
    }
       
    if (m_CompressStatusDialog)
    {
        saveProjectFile();
        compressProjectFiles(NULL);
        if (m_AllItemsSelected)
        {
            // Reset the list of selections
            Tree_clearAllItemsSetected();
            m_AllItemsSelected = false;
        }
    }
}

ProjectView::~ProjectView()
{
    if (m_imageloader)
    {
        delete m_imageloader;
        m_imageloader = NULL;
    }

    if (m_newProjectwindow)
    {
        delete m_newProjectwindow;
        m_newProjectwindow = NULL;
    }

    if (m_diffImageDialog)
    {
        delete m_diffImageDialog;
        m_diffImageDialog = NULL;
    }

}

void ProjectView::diffImageFiles()
{
    if (m_diffImageDialog)
    {
        m_diffImageDialog->m_RecentImageDir = m_RecentImageDirOpen;
        m_diffImageDialog->m_SupportedImageFormat = m_ImageFileFilter;

        m_diffImageDialog->m_file1Name->clear();
        m_diffImageDialog->m_file1Name->addItem("");
        m_diffImageDialog->m_file1Name->addItems(m_ImagesinProjectTrees);

        // find the index of the selected diff file from the drop down list
        int index1 = m_diffImageDialog->m_file1Name->findText(m_curDiffSourceFile);
        if (index1 != -1)
        {
            m_diffImageDialog->m_file1Name->setCurrentIndex(index1);
        }
        else
        {
            m_diffImageDialog->m_file1Name->setCurrentText(m_curDiffSourceFile);
        }
        m_diffImageDialog->m_file2Name->clear();
        m_diffImageDialog->m_file2Name->addItem("");
        m_diffImageDialog->m_file2Name->addItems(m_ImagesinProjectTrees);
        int index2 = m_diffImageDialog->m_file1Name->findText(m_curDiffDestFile);
        if (index2 != -1)
        {
            m_diffImageDialog->m_file2Name->setCurrentIndex(index2);
        }
        else
        {
            m_diffImageDialog->m_file2Name->setCurrentText(m_curDiffDestFile);
        }

        m_diffImageDialog->show();

        //reset the diff image file name
        m_curDiffSourceFile = "";
        m_curDiffDestFile = "";
    }
}

bool ProjectView::OpenImageFile()
{
    // Add new file
    QStringList ls = QFileDialog::getOpenFileNames(this, tr("Open image file(s)"), m_RecentImageDirOpen, m_ImageFileFilter);
    if (!ls.isEmpty())
    {
        for (int i = 0; i < ls.size(); i++)
        {
            C_Source_Image *m_dataout = NULL;
            if (Tree_AddImageFile(ls[i], 0, &m_dataout))
            {
                QFileInfo FileInfo(ls[i]);
                emit AddedImageFile(ls[i]);
                m_RecentImageDirOpen = FileInfo.path();
            }
        }
        m_saveProjectChanges = true;
        return true;
    }
    return false;
}

//=======================================================

void ProjectView::onEntered(const QModelIndex &index)
{
    Q_UNUSED(index);
}

int ProjectView::PromptSaveChanges()
{
    QMessageBox msgBox;
    QString msg;
    msg.append("The project \"");
    msg.append(m_curProjectName);
    msg.append("\" has been modified.");
    msgBox.setText(msg);
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    return msgBox.exec();
}

void ProjectView::onTree_ItemClicked(QTreeWidgetItem * item, int column)
{
    if (!item) return;
    if (m_processBusy) return;

    // There is a chance of roll over : in these cases
    // user will be clicking the mouse several time to get 
    // the correct repsonse : it is deemed ok to hangle the roll over 
    // (where ms exceeds 1000 and starts from 0)
    // if (m_elapsedTimer.msecsSinceReference() < (qint64)100)
    // {
    //     // Ignore the multiple clicks is below a set limit
    //     // this allows smooth handling of double click events
    //     return;
    // }
    // 
    // m_elapsedTimer.restart();

    Q_UNUSED(column);
    // QString itemName;
    // itemName = item->text(0);
    // 
    // // its a special control skip any click poperty updates
    // if (itemName.contains("[+]"))
    // {
    //     m_CurrentCompressedImageItem = NULL;
    //     emit UpdateData(NULL);
    //     return;
    // }
    // 
    // if (itemName.contains("(+)"))
    // {
    //     m_CurrentCompressedImageItem = NULL;
    //     emit UpdateData(NULL);
    //     return;
    // }
    // 
    // if (itemName.contains("Set:")) return;

    QVariant v = item->data(0, Qt::UserRole);
    int levelType = v.toInt();

    if (levelType == TREETYPE_ADD_IMAGE_NODE)
    {
        // Clears of selected items when user clicks on this node
        Tree_clearAllItemsSetected();
    }

    if (levelType == TREETYPE_IMAGEFILE_DATA)
    {
        m_CurrentCompressedImageItem = NULL;

        emit OnSourceImage(item->childCount());

        // view image
        QVariant v = item->data(1, Qt::UserRole);
        C_Source_Image *m_data = v.value<C_Source_Image *>();
        QString text = m_data->m_Full_Path;

        emit ViewImageFile(text, item);


        // Update the image poperty view for the item clicked
        SignalUpdateData(item, levelType);
    }
    else
    if (levelType == TREETYPE_COMPRESSION_DATA )
    {
        // view image
        QVariant v = item->data(1, Qt::UserRole);
        C_Destination_Options *m_data = v.value<C_Destination_Options *>();
        if (m_data)
        {
            QFileInfo fileinfo(m_data->m_destFileNamePath);
            QFile file(m_data->m_destFileNamePath);
            actViewImageDiff->setEnabled(file.exists());
            m_CurrentCompressedImageItem = item;
            QFile fileInfo(m_data->m_destFileNamePath);
            m_data->m_FileSize = fileInfo.size();

            if (m_data->m_FileSize > 1024000)
                m_data->m_FileSizeStr = QString().number((double)m_data->m_FileSize / 1024000, 'f', 2) + " MB";
            else
                if (m_data->m_FileSize > 1024)
                    m_data->m_FileSizeStr = QString().number((double)m_data->m_FileSize / 1024, 'f', 1) + " KB";
                else
                    m_data->m_FileSizeStr = QString().number(m_data->m_FileSize) + " Bytes";

            QFile SourcefileInfo(m_data->m_sourceFileNamePath);
            qint64 SourceImageSize = m_data->m_SourceImageSize;

            if ((m_data->m_FileSize > 0) && (SourceImageSize > 0))
            {
                double CompressionRatio = SourceImageSize / (double)m_data->m_FileSize;
                char buffer[128];
                sprintf(buffer, "%2.2f", CompressionRatio);
                m_data->m_CompressionRatio =  QString("%1 to 1").arg(buffer);
            }

            if (file.exists())
            {
                m_CurrentCompressedImageItem = item;
                emit OnDecompressImage();
                emit ViewImageFile(m_data->m_destFileNamePath, item);
            }

        }
        // Update the compression data poperty view for the item clicked
        SignalUpdateData(item,levelType);
    }
    else
    {
        m_CurrentCompressedImageItem = NULL;
        emit UpdateData(NULL);
    }

}

void ProjectView::onTree_ItemDoubleClicked(QTreeWidgetItem * item, int column)
{
    if (!item) return;

    Q_UNUSED(column);
    QString itemName;
    itemName = item->text(0);
    QVariant v = item->data(0, Qt::UserRole);
    int levelType = v.toInt();

    switch (levelType)
    {
    case TREETYPE_ADD_IMAGE_NODE: // [+] Double Click here to add files
    {
        // Clears of selected items when user clicks on this node
        Tree_clearAllItemsSetected();

        // Add new file
        if (OpenImageFile())
        {
            // Update the poperty view for the item clicked
            SignalUpdateData(item, TREETYPE_IMAGEFILE_DATA);
        }
    }
    break;
    case TREETYPE_IMAGEFILE_DATA_NODE: // STR_AddDestinationSetting
    {
        // Add new setting
        emit AddCompressSettings(item);
    }
    break;
    } // switch case

}

QTreeWidgetItem *ProjectView::Tree_FindImageItem(QString filePathName, bool includeDestination)
{
    // Make sure FileName does not contain 
    // any specialized prefixes
    // Currently Differance us used for image compare
    // 
    if (filePathName.contains(DIFFERENCE_IMAGE_TXT))
    {
        filePathName.remove(0, sizeof(DIFFERENCE_IMAGE_TXT)-1);
    }


    // Find the item and set it as selected
    QTreeWidgetItemIterator it(m_treeRootItem);
    while (*it) {

        QString Source_FilePathName = GetSourceFileNamePath(*it);
        // Match any Source file
        if (filePathName.compare(Source_FilePathName) == 0)
        {
            return (*it);
        }

        if (includeDestination)
        {
            QString Destination_FilePathName = GetDestinationFileNamePath(*it);
            // Match any Destination file
            if (filePathName.compare(Destination_FilePathName) == 0)
            {
                return (*it);
            }
        }

        ++it;
    }

    return NULL;
}

void ProjectView::SelectImageItem(QString filePathName)
{
    static QString lastfilePathName = "";

    // Already Selected on Treeview then exit
    if (lastfilePathName.compare(filePathName) == 0) return;

    QTreeWidgetItem *it = Tree_FindImageItem(filePathName, true);

    if (it) 
    {
        m_projectTreeView->setCurrentItem(it);
        // Update the poperty view for the item selected
        QVariant v = it->data(0, Qt::UserRole);
        int levelType = v.toInt();

        if (levelType == TREETYPE_IMAGEFILE_DATA) emit OnSourceImage(it->childCount());
        else
        if (levelType == TREETYPE_COMPRESSION_DATA) emit OnDecompressImage();

        lastfilePathName = GetDestinationFileNamePath(it);
        if (lastfilePathName.length() == 0)
            lastfilePathName = GetSourceFileNamePath(it);

        SignalUpdateData(it, levelType);
    }
}

void ProjectView::clearProjectTreeView()
{
    // Find the item and set it as selected
    QTreeWidgetItemIterator it(m_treeRootItem);
    while (*it) {
        DeleteItemData(*it);
        ++it;
    }

    // Clean the entire tree
    m_projectTreeView->clear();

    // add a new root node
    Tree_AddRootNode();
}

void ProjectView::keyPressEvent(QKeyEvent * event)
{
    if ((event->key() == Qt::Key_Delete) && (!m_bCompressing))
    {
        // handle the key press, perhaps giving the item text a default value
        UserDeleteItems();
        event->accept();
    }
}

QString ProjectView::GetSourceFileNamePath(QTreeWidgetItem *item)
{
    QString filePathName = "";
    if (!item) return filePathName;

    QVariant v = item->data(0, Qt::UserRole);
    int levelType = v.toInt();

    switch (levelType)
    {
    case TREETYPE_IMAGEFILE_DATA:
            {
                v = item->data(1, Qt::UserRole);
                C_Source_Image *data = v.value<C_Source_Image *>();
                if (data)
                    filePathName = data->m_Full_Path;
            }
            break;
    case TREETYPE_COMPRESSION_DATA:
            {
                v = item->data(1, Qt::UserRole);
                C_Destination_Options *data = v.value<C_Destination_Options *>();
                if (data)
                    filePathName = data->m_sourceFileNamePath;
            }
            break;
    default:
        filePathName = "";
    }

    return filePathName;
}

QString ProjectView::GetDestinationFileNamePath(QTreeWidgetItem *item)
{
    QString filePathName = "";
    if (!item) return filePathName;

    QVariant v = item->data(0, Qt::UserRole);
    int levelType = v.toInt();

    switch (levelType)
    {
        case TREETYPE_COMPRESSION_DATA:
            {
                v = item->data(1, Qt::UserRole);
                C_Destination_Options *data = v.value<C_Destination_Options *>();
                if (data)
                    filePathName = data->m_destFileNamePath;
            }
            break;
        default:
            filePathName = "";
    }
    return filePathName;
}

void ProjectView::DeleteItemData(QTreeWidgetItem *item)
{
    if (!item) return;

    QVariant v = item->data(0, Qt::UserRole);
    int levelType = v.toInt();

    switch (levelType)
    {
    case TREETYPE_IMAGEFILE_DATA:
    {
        // Get the Image Data linked to this node
        v = item->data(1, Qt::UserRole);
        C_Source_Image *data = v.value<C_Source_Image *>();
        QString filePathName = data->m_Full_Path;

        // Remove the MIP Image data
        if (m_imageloader)
            m_imageloader->clearMipImages(data->m_MipImages);

         // Remove any docked views of the file
        if (filePathName.length() > 0)
        {
            // Remove the item from the diff image drop down list
            int temp = m_ImagesinProjectTrees.indexOf(filePathName);
            if (temp != -1)
                m_ImagesinProjectTrees.removeAt(temp);

            emit DeleteImageView(filePathName);
        }

        // Delete the Data Class
        if (data)
        {
            delete data;
            data = nullptr;
        }
    }
    break;
    case TREETYPE_COMPRESSION_DATA:
    {
        v = item->data(1, Qt::UserRole);
        C_Destination_Options *data = v.value<C_Destination_Options *>();
        QString filePathName = data->m_destFileNamePath;

        // Remove the MIP Image data
        if (m_imageloader)
            m_imageloader->clearMipImages(data->m_MipImages);

        if (data)
        {
            delete data;
            data = nullptr;
        }

        // Remove any docked views of the file
        if (filePathName.length() > 0)
        {
            // Remove the item from the diff image drop down list
            int temp = m_ImagesinProjectTrees.indexOf(filePathName);
            if (temp != -1)
                m_ImagesinProjectTrees.removeAt(temp);

            emit DeleteImageView(filePathName);
        }

    }
    break;
    }

    m_saveProjectChanges = true;
}

// Return a valid image item

QTreeWidgetItem* ProjectView::GetCurrentItem(int inLevelType)
{
    // use known current 
    if (m_CurrentItem)
    {
        return m_CurrentItem;
    }

    // else use tree 
    QTreeWidgetItem *item = m_projectTreeView->currentItem();
    if (!item) return NULL;

    QVariant v = item->data(0, Qt::UserRole);
    int levelType = v.toInt();

    if ((levelType != TREETYPE_ADD_IMAGE_NODE) && (levelType != TREETYPE_IMAGEFILE_DATA_NODE) && (levelType == inLevelType))
    {
        return (item);
    }
    return (NULL);
}

// Delete an Image Item from the project view
// User is prompted to confirm the delete

void ProjectView::UserDeleteItems()
{
    QMessageBox msgBox; 
    msgBox.setText(trUtf8("Delete Selected Images")); 
    msgBox.setInformativeText(
        "<br>Choose <b>Remove</b> to remove from project<br><br>Choose <b>Delete</b> to delete the image and its compressed images (if have any) from disk permanently <br>"
        );
    QPushButton *myRemoveButton = msgBox.addButton(trUtf8("Remove"), QMessageBox::YesRole);
    QPushButton *myDeleteButton = msgBox.addButton(trUtf8("Delete"), QMessageBox::AcceptRole);
    QPushButton *myCancelButton = msgBox.addButton(trUtf8("Cancel"), QMessageBox::NoRole );
    msgBox.setDefaultButton(myCancelButton);
    msgBox.setIcon(QMessageBox::Question); 
    msgBox.exec();
    if (msgBox.clickedButton() == myRemoveButton)
    { 
        DeleteAllSeletedItems(false);
    }
    else
    if (msgBox.clickedButton() == myDeleteButton)
    {
        DeleteAllSeletedItems(true);
    }

}

QTreeWidgetItem * ProjectView::DeleteSelectedItemData(QTreeWidgetItem *item, bool RemoveFromDisk)
{
    if (!item) return (NULL);

    QVariant v = item->data(0, Qt::UserRole);
    int levelType = v.toInt();

    //QString itemName;
    //itemName = item->text(0);
    //qDebug() << "Delete: " << itemName;

    if (levelType == TREETYPE_IMAGEFILE_DATA)
    {
        // Does this item have childern
        int  childcount = item->childCount();
        if (childcount > 0)
        {
            // index starts from 0 to childcound-1
            childcount--;
    
            // Parse the child items that are of compress data types
            // in reverse order (start from botton of the three and work upwards
            // to clean the variant data items
            QTreeWidgetItem *child;
            int ChildlevelType;

            // child 0 = Root node (+) all other indexes will be Compression settings
            while (childcount >= 0)
            {
                child = item->child(childcount);
                if (child)
                {
                    v = child->data(0, Qt::UserRole);
                    ChildlevelType = v.toInt();
                    // This should always be true: We are not flagging errors if false
                    if (ChildlevelType == TREETYPE_COMPRESSION_DATA)
                    {
                        v = child->data(1, Qt::UserRole);
                        C_Destination_Options *data = v.value<C_Destination_Options *>();
                        // Remove the file
                        if (RemoveFromDisk)
                        {
                            QFile::remove(data->m_destFileNamePath);
                        }

                        // Remove the item from the treeview
                        DeleteItemData(child);
                        m_NumItems--;
                    }
                    else
                    {
                        // Remove the (+) item node 
                        DeleteItemData(child);
                        m_NumItems--;
                    }
                }
                childcount--;
            }
            m_NumItems--;
        }
    
    }
    else
    if (levelType == TREETYPE_COMPRESSION_DATA)
    {
        // Remove the file
        v = item->data(1, Qt::UserRole);
        C_Destination_Options *data = v.value<C_Destination_Options *>();
        if (RemoveFromDisk)
        {
            QFile::remove(data->m_destFileNamePath);
        }
        m_NumItems--;
    }
    
    // remove the node and its data for which the user selected the delete on
    int i = m_projectTreeView->indexOfTopLevelItem(item);
    QTreeWidgetItem *TopLevelitem = m_projectTreeView->takeTopLevelItem(i);
    if ((levelType != TREETYPE_ADD_IMAGE_NODE) && (levelType != TREETYPE_IMAGEFILE_DATA_NODE))
    {
        DeleteItemData(item);
        if (item)
        {
            delete item;
            item = NULL;
        }
    }

    m_saveProjectChanges = true;
    return TopLevelitem;
}

void ProjectView::DeleteAllSeletedItems(bool RemoveFromDisk)
{
    // Find the item and set it as selected
    QTreeWidgetItemIterator it(m_treeRootItem);
    QVariant v;
    int levelType;
    int TotalItems = m_NumItems;

    // Use TotalItems as a bug check to prevent infinate loops!
    // The itterator should loop only once through all current items in the TreeView

    while ((*it) && (TotalItems > 0)) 
    {
        v = (*it)->data(0, Qt::UserRole);
        levelType = v.toInt();

        //QString itemName;
        //itemName = (*it)->text(0);
        //qDebug() << "Delete: " << itemName;

        if ((*it)->isSelected() && (levelType != TREETYPE_ADD_IMAGE_NODE) )
        {
            DeleteSelectedItemData(*it, RemoveFromDisk);
        }
        else
            ++it;

        TotalItems--;
    }
}

QTreeWidgetItem * ProjectView::Tree_SetCurrentItem(QString FilePathName)
{
    QTreeWidgetItem * item = Tree_FindImageItem(FilePathName, true);
    if (item)
    {
        m_CurrentItem = item;
        m_projectTreeView->setCurrentItem(item);
        item->setSelected(true);
        if (m_EnableCheckedItemsView)
        {
            item->setCheckState(0, Qt::Checked);
        }
        return item;
    }

    m_CurrentItem = NULL;
    return NULL;
}

void ProjectView::saveToBatchFile()
{

    if (!AnySelectedItems()) return;


    QFileInfo fileInfo(m_curProjectFilePathName);
    QString suggestedName = fileInfo.completeBaseName();
    QString filePathName = QFileDialog::getSaveFileName(this, tr("Save selected images to batch file"), suggestedName, tr("Command Line Batch Files (*.bat)"));
    if (filePathName.length() > 0)
    {
        saveProjectFile();

        // Writing to a file
        QFile file(filePathName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            //qDebug() << "Open file for writing failed";
            return;
        }

        if (m_CompressStatusDialog)
        {
            m_CompressStatusDialog->onClearText();
        }

        compressProjectFiles(&file);
        file.close();

        if (m_AllItemsSelected)
        {
            // Reset the list of selections
            Tree_clearAllItemsSetected();
            m_AllItemsSelected = false;
        }
    }

}

void ProjectView::openContainingFolder()
{
    if (ContextMenu_ImageItem)
    {

        QVariant v = ContextMenu_ImageItem->data(0, Qt::UserRole);
        int levelType = v.toInt();
        switch (levelType)
        {
            case TREETYPE_IMAGEFILE_DATA:         // Original Image item
            {
                QVariant fv = ContextMenu_ImageItem->data(1, Qt::UserRole);
                C_Source_Image *m_data = fv.value<C_Source_Image *>();
                if (m_data)
                {
                    QDir dir(QCoreApplication::applicationDirPath());
                    QString absolute_file_pathName = dir.absoluteFilePath(m_data->m_Full_Path);
                    QFileInfo FilePath(absolute_file_pathName);
                    QDesktopServices::openUrl(QUrl::fromLocalFile(FilePath.absolutePath()));
                }
                break;
            }

            case TREETYPE_COMPRESSION_DATA:         // Compressed Image item
            {
                QVariant fv = ContextMenu_ImageItem->data(1, Qt::UserRole);
                C_Destination_Options *m_data = fv.value<C_Destination_Options *>();
                if (m_data)
                {
                    QDir dir(QCoreApplication::applicationDirPath());
                    QString absolute_file_pathName = dir.absoluteFilePath(m_data->m_destFileNamePath);
                    QFileInfo FilePath(absolute_file_pathName);
                    QDesktopServices::openUrl(QUrl::fromLocalFile(FilePath.absolutePath()));
                }
                break;
            }

        default:
                break;
        }
    }
}

void ProjectView::copyFullPath()
{
    if (ContextMenu_ImageItem)
    {
        QVariant v = ContextMenu_ImageItem->data(0, Qt::UserRole);
        int levelType = v.toInt();
        switch (levelType)
        {
            case TREETYPE_IMAGEFILE_DATA:         // Original Image item
            {
                QVariant fv = ContextMenu_ImageItem->data(1, Qt::UserRole);
                C_Source_Image *m_data = fv.value<C_Source_Image *>();
                if (m_data)
                {
                    QClipboard *clipboard = QApplication::clipboard();
                    if (clipboard)
                    {
                        QDir dir(QCoreApplication::applicationDirPath());
                        QString absolute_file_pathName = dir.absoluteFilePath(m_data->m_Full_Path);
                        // Use this if Path Only
                        QFileInfo FilePath(absolute_file_pathName);
                        QString filePath = FilePath.absolutePath();
                        filePath.replace("/", "\\");
                        clipboard->setText(filePath);
                        // else use the floowing that contains the full path and File Name with extension
                        //clipboard->setText(absolute_file_pathName);
                    }
                }
                break;
            }

            case TREETYPE_COMPRESSION_DATA:         // Compressed Image item
            {
                QVariant fv = ContextMenu_ImageItem->data(1, Qt::UserRole);
                C_Destination_Options *m_data = fv.value<C_Destination_Options *>();
                if (m_data)
                {
                    QClipboard *clipboard = QApplication::clipboard();
                    if (clipboard)
                    {
                        QDir dir(QCoreApplication::applicationDirPath());
                        QString absolute_file_pathName = dir.absoluteFilePath(m_data->m_destFileNamePath);
                        // Use this if Path Only
                        QFileInfo FilePath(absolute_file_pathName);
                        QString filePath= FilePath.absolutePath();
                        filePath.replace("/", "\\");
                        clipboard->setText(filePath);
                        // else use the floowing that contains the full path and File Name with extension
                        //clipboard->setText(absolute_file_pathName);
                    }
                }
                break;
            }

        default:
            break;
        }
    }
}

void makeFormatExtCompatible(C_Destination_Options* C_Destination_Options)
{
    C_Destination_Options::eCompression format = C_Destination_Options->m_Compression;
    QString destfilePath = C_Destination_Options->m_destFileNamePath;
    QFileInfo info(destfilePath);
    QString newDestPath;
    if (format == C_Destination_Options::eCompression::ASTC && info.suffix() == "DDS")
    {
        newDestPath = info.path() + "/" + info.completeBaseName() + ".KTX";
        C_Destination_Options->m_destFileNamePath = newDestPath;
        C_Destination_Options->m_FileInfoDestinationName = C_Destination_Options->m_compname + ".KTX";
    }
    else if (format != C_Destination_Options::eCompression::ASTC && info.suffix() == "ASTC")
    {
        newDestPath = info.path() + "/" + info.completeBaseName() + ".DDS";
        C_Destination_Options->m_destFileNamePath = newDestPath;
        C_Destination_Options->m_FileInfoDestinationName = C_Destination_Options->m_compname + ".DDS";
    }
    
}

void ProjectView::saveProjectFile()
{
    // Writing to a file
    if (!(m_curProjectFilePathName.contains(".cprj", Qt::CaseInsensitive)))
        m_curProjectFilePathName.append(QString(".cprj"));
    
    QFile file(m_curProjectFilePathName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        //qDebug() << "Open file for writing failed";
        return;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    // <PROJECT>
    xmlWriter.writeStartElement("PROJECT");

    // Parse the Project view tree
    QTreeWidgetItemIterator it(m_projectTreeView);
    while (*it) {

    QVariant v = (*it)->data(0, Qt::UserRole);
    int levelType = v.toInt();
    int childcount = (*it)->childCount();

    if (levelType == TREETYPE_IMAGEFILE_DATA)
    {
        QVariant v = (*it)->data(1, Qt::UserRole);
        C_Source_Image *m_data = v.value<C_Source_Image *>();
        QString     FilePathName;
        FilePathName = m_data->m_Full_Path;

            // <IMAGE FILE="">
            xmlWriter.writeStartElement("Image");
            xmlWriter.writeAttribute("File", FilePathName);

            if (childcount >= 1)
            {
                // now save the child elements
                for (int i = 0; i < childcount; i++)
                {
                    ++it;
                    if (*it)
                    {
                        QString   Setting = (*it)->text(0);
                        QVariant v = (*it)->data(0, Qt::UserRole);
                        int sublevelType = v.toInt();

                        // save the settings item 
                        if (sublevelType == TREETYPE_COMPRESSION_DATA)
                        {
                            v = (*it)->data(1, Qt::UserRole);
                            C_Destination_Options *data = v.value<C_Destination_Options *>();

                            // <Compression setting="">
                            xmlWriter.writeStartElement("Compression");
                            xmlWriter.writeAttribute("Setting", Setting);
                            xmlWriter.writeAttribute("Enabled", (*it)->checkState(0) == Qt::Checked ? "True" : "False");

                            if (data)
                            {
                                makeFormatExtCompatible(data);

                                xmlWriter.writeTextElement("Destination", data->m_destFileNamePath);

                                QMetaObject meta = C_Destination_Options::staticMetaObject;
                                int indexCompression = meta.indexOfEnumerator("eCompression");
                                QMetaEnum metaEnumCompression = meta.enumerator(indexCompression);
                                const char* key = metaEnumCompression.valueToKey(data->m_Compression);
                                xmlWriter.writeTextElement("fd", key);

                                xmlWriter.writeTextElement("Quality", QString::number(data->m_Quality,'g',4));

                                xmlWriter.writeTextElement("WeightR", QString::number(data->X_RED, 'g', 4));
                                xmlWriter.writeTextElement("WeightG", QString::number(data->Y_GREEN, 'g', 4));
                                xmlWriter.writeTextElement("WeightB", QString::number(data->Z_BLUE, 'g', 4));

                                xmlWriter.writeTextElement("AlphaThreshold", QString::number(data->Threshold));

                                xmlWriter.writeTextElement("BlockRate", data->m_Bitrate);
                            }
                            // </Compression>
                            xmlWriter.writeEndElement();
                        }
                    }
                }
            }

            // </IMAGE>
            xmlWriter.writeEndElement();
        }

        // last item should be a vaild one unless we ended in a 
        // child loop that was the last element in the tree if so break out were done..
        if (*it)
            ++it;
        else
            break;

    }

    // </PROJECT> 
    xmlWriter.writeEndElement();
    file.close();

    m_saveProjectChanges = false;
}

void ProjectView::saveAsProjectFile()
{
    QString filePathName = QFileDialog::getSaveFileName(this, tr("Save Project"), m_curProjectFilePathName, tr("Compress Project files (*.cprj)"));
    if (filePathName.length() == 0) return;
    setCurrentProjectName(filePathName);
    saveProjectFile();
}

bool ProjectView::loadProjectFile(QString fileToLoad)
{
    int numCompressedItems = 0;

    // Create a document to write XML
    QDomDocument document;

    // Open a file for reading
    QFile file(fileToLoad);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qDebug() << "Failed to open the file for reading.";
        return false;
    }
    else
    {
        // loading
        if (!document.setContent(&file))
        {
            //qDebug() << "Failed to load the file for reading.";
            return false;
        }
        file.close();
    }

    // Remove the old view
    clearProjectTreeView();

    // Getting root element
    QDomElement root = document.firstChildElement();

    // Loop Through Project Images 
    QDomNodeList domImages = root.elementsByTagName("Image");

    // The number of Image nodes in the file <Image ...>
    int countImages = domImages.count();
    for (int i = 0; i < countImages; i++)
    {
        QDomNode domImage = domImages.at(i);
        if (domImage.isElement())
        {
            // Get the File element  <Image File=".."/>
            QDomElement eleImage = domImage.toElement();
            QString FilePathName = eleImage.attribute("File");

            // Check if we have a file specified else igonore the node
            if (FilePathName.length() > 0)
            {
                // Add the file to our Project View
                C_Source_Image *m_dataout = NULL;
                QTreeWidgetItem * Imageitem = Tree_AddImageFile(FilePathName, 0, &m_dataout);
                if (Imageitem)
                {
                    // Loop Image settings
                    QDomNodeList domCompressions = eleImage.elementsByTagName("Compression");
                    int countCompressions = domCompressions.count();
                    for (int i = 0; i < countCompressions; i++)
                    {
                        QDomNode domCompress = domCompressions.at(i);
                        if (domCompress.isElement())
                        {
                            numCompressedItems++;

                            QDomElement eleCompress = domCompress.toElement();
                            QString Setting = eleCompress.attribute("Setting");
                            QString Enabled = eleCompress.attribute("Enabled").toUpper();
                           
                            // See also cpMainComponents - for new C_Destination_Options usage
                            C_Destination_Options *m_data = new C_Destination_Options();
                            if (m_data)
                            {
                                m_data->m_compname                  = Setting;
                                m_data->m_FileInfoDestinationName   = Setting;
                                m_data->m_sourceFileNamePath        = FilePathName;

                                if (m_dataout)
                                {
                                    m_data->m_Width     = m_dataout->m_Width;
                                    m_data->m_Height    = m_dataout->m_Height;
                                    m_data->m_HeightStr = QString().number(m_data->m_Height) + " px";
                                    m_data->m_WidthStr  = QString().number(m_data->m_Width)  + " px";
                                    m_data->m_SourceImageSize = m_dataout->m_ImageSize;
                                }

                                QDomNode child = eleCompress.firstChild();
                                while (!child.isNull()) {
                                    if (child.toElement().tagName() == "Destination") {
                                        QDomElement eleDestination = child.toElement();
                                        m_data->m_destFileNamePath = eleDestination.text();
                                        
                                        QFileInfo fileInfo(m_data->m_destFileNamePath);
                                        if (!fileInfo.isWritable())
                                        {
                                            QFileInfo fileInfo2(m_curProjectFilePathName);
                                            m_data->m_destFileNamePath = fileInfo2.dir().path();
                                            m_data->m_destFileNamePath.append(QDir::separator());
                                            m_data->m_destFileNamePath.append(fileInfo.fileName());
                                            m_data->m_destFileNamePath.replace("/", "\\");
                                        }
                                        else
                                            m_data->m_FileInfoDestinationName = Setting + "." + fileInfo.suffix();

                                    }
                                    else if (child.toElement().tagName() == "fd") {
                                        QDomElement eleFD = child.toElement();
                                        QString format = eleFD.text();
                                        QMetaObject meta = C_Destination_Options::staticMetaObject;

                                        int indexCompression = meta.indexOfEnumerator("eCompression");
                                        QMetaEnum metaEnumCompression = meta.enumerator(indexCompression);
                                        m_data->m_Compression = (C_Destination_Options::eCompression)metaEnumCompression.keysToValue(format.toLatin1().data());
                                    }
                                    else if (child.toElement().tagName() == "Quality") {
                                        QDomElement eleFD = child.toElement();
                                        QString Quality = eleFD.text();
                                        bool ok;
                                        m_data->m_Quality = Quality.toFloat(&ok);
                                        if (!ok)
                                            m_data->m_Quality = AMD_CODEC_QUALITY_DEFAULT;
                                    }
                                    else if (child.toElement().tagName() == "WeightR") {
                                        QDomElement eleFD = child.toElement();
                                        QString WeightR = eleFD.text();
                                        bool ok;
                                        m_data->X_RED = WeightR.toFloat(&ok);
                                        if (!ok)
                                            m_data->X_RED = 0.3086;
                                    }
                                    else if (child.toElement().tagName() == "WeightG") {
                                        QDomElement eleFD = child.toElement();
                                        QString WeightG = eleFD.text();
                                        bool ok;
                                        m_data->Y_GREEN = WeightG.toFloat(&ok);
                                        if (!ok)
                                            m_data->Y_GREEN = 0.6094;
                                    }
                                    else if (child.toElement().tagName() == "WeightB") {
                                        QDomElement eleFD = child.toElement();
                                        QString WeightB = eleFD.text();
                                        bool ok;
                                        m_data->Z_BLUE = WeightB.toFloat(&ok);
                                        if (!ok)
                                            m_data->Z_BLUE = 0.0820;
                                    }
                                    else if (child.toElement().tagName() == "AlphaThreshold") {
                                        QDomElement eleFD = child.toElement();
                                        QString AlphaThreshold = eleFD.text();
                                        bool ok;
                                        m_data->Threshold = AlphaThreshold.toInt(&ok);
                                        if (!ok)
                                            m_data->Threshold = 0;
                                    }
                                    else if (child.toElement().tagName() == "BlockRate") {
                                        QDomElement eleFD = child.toElement();
                                        QString BlockRate = eleFD.text();
                                        m_data->m_Bitrate = BlockRate;
                                        if (BlockRate.length() < 2) // Default back to 4x4!
                                            m_data->m_Bitrate = "8.00";
                                    }
                                    child = child.nextSibling();
                                }

                                // Always add from the TREETYPE_IMAGE_SETTING_NODE STR_AddDestinationSetting 
                                Tree_AddCompressFile(Imageitem->child(0), Setting, true, Enabled.contains("TRUE"), TREETYPE_COMPRESSION_DATA, m_data);
                            }
                        }
                    }
                }
            }
        }
    }
    
    setCurrentProjectName(fileToLoad);
    m_saveProjectChanges = false;

    emit OnProjectLoaded(numCompressedItems);

    return true;
}

void ProjectView::openProjectFile()
{
    if (!userSaveProjectAndContinue()) return;

    QString filename = QFileDialog::getOpenFileName(this, tr("Open project files"), ".", tr("Project files (*.cprj)"));
    if (filename.length() > 0)
        loadProjectFile(filename);
}

void ProjectView::removeSelectedImage()
{
    if (!m_bCompressing)
        UserDeleteItems();
}

// Prompt the user to save prio project and continue or
// cancel the current task 

bool ProjectView::userSaveProjectAndContinue()
{
    // Check if user wants t osave prior changes
    if (m_saveProjectChanges)
    {
        switch (PromptSaveChanges())
        {
        case QMessageBox::Save:
            saveProjectFile();
            break;
        case QMessageBox::Cancel:
            return false;
            break;
        }
        m_saveProjectChanges = false;
    }
    return true;
}

void ProjectView::openNewProjectFile()
{
    // Check if user wants to save prior changes
    if (!userSaveProjectAndContinue()) return;
    m_newProjectwindow->GetNewFilePathName(m_curProjectFilePathName);   
    emit OnProjectLoaded(0);
}



void ProjectView::onSetNewProject(QString &FilePathName)
{
    // Remove the old view
    clearProjectTreeView();
    setCurrentProjectName(FilePathName);
    m_curProjectFilePathName = FilePathName;
    m_saveProjectChanges = false;
    if (m_CompressStatusDialog)
        m_CompressStatusDialog->onClearText();
    m_CurrentCompressedImageItem = NULL;
    emit UpdateData(NULL);
}


extern int      g_MipLevel;
extern float    g_fProgress;

bool ProgressCallback(float fProgress, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
    // Keep Qt responsive
    QApplication::processEvents();

    // Process the CmdLine Messages

    if (g_fProgress != fProgress)
    {
        UNREFERENCED_PARAMETER(pUser1);
        UNREFERENCED_PARAMETER(pUser2);

        if ((g_CmdPrams.noprogressinfo) || (g_CmdPrams.silent)) return g_bAbortCompression;

        emit static_processmsghandler.signalProcessMessage();
        g_fProgress = fProgress;
    }

    return g_bAbortCompression;
}


void ProjectView::onSignalProcessMessage()
{
    if (m_pProgressDlg)
    {
        m_pProgressDlg->SetValue(g_fProgress);
    }
}

void ProjectView::AddSettingtoEmptyTree()
{
    int childcount = 0;
    // Parse the Project view tree
    QTreeWidgetItemIterator it(m_projectTreeView);
    cpMainComponents *temp = (cpMainComponents*)(m_parent);

    while (*it) {

        QString     name = (*it)->text(0);
        QVariant v = (*it)->data(0, Qt::UserRole);
        int levelType = v.toInt();
        childcount = (*it)->childCount();
        int miplevels = 0;


        if (levelType == TREETYPE_IMAGEFILE_DATA)
        {
            if (childcount == 1)
            {
                if (*it)
                {
                    QTreeWidgetItem * Imageitem = (*it);
                    QString   Setting = Imageitem->text(0);
                    QVariant v = (*it)->data(0, Qt::UserRole);
                    int sublevelType = v.toInt();
                    if (sublevelType == TREETYPE_IMAGEFILE_DATA)
                    {
                        if (temp)
                        {
                            temp->m_setcompressoptions->m_data.init();

                            QVariant v = Imageitem->data(1, Qt::UserRole);
                            C_Source_Image *m_imagefile = v.value<C_Source_Image *>();
                            QFileInfo fileinfo(m_imagefile->m_Name);
                            temp->m_setcompressoptions->m_data.m_sourceFileNamePath = m_imagefile->m_Full_Path;
                            temp->m_setcompressoptions->m_data.m_SourceImageSize = m_imagefile->m_ImageSize;
                            temp->m_setcompressoptions->m_data.m_SourceIscompressedFormat = CompressedFormat(m_imagefile->m_Format);
                            temp->m_setcompressoptions->m_data.m_SourceIsFloatFormat = FloatFormat(m_imagefile->m_Format);
                            temp->m_setcompressoptions->m_item = Imageitem;
                            temp->m_setcompressoptions->isNoSetting = true;   
                            break;
                        }
                    }
                }
            }
        }

        // last item should be a vaild one unless we ended in a 
        // child loop that was the last element in the tree if so break out were done..
        if (*it)
            ++it;
        else
            break;
    }

    if (temp && temp->m_setcompressoptions->isNoSetting && !(m_processFromContext))
    {
        if (temp->m_setcompressoptions->updateDisplayContent())
        {
            if (!temp->m_setcompressoptions->isVisible())
            {
                QPoint pos = QCursor::pos();
                temp->m_setcompressoptions->move(pos);
                temp->m_setcompressoptions->m_LEName->setEnabled(false);
                temp->m_setcompressoptions->setWindowFlags(Qt::Dialog);
                temp->m_setcompressoptions->exec();
                temp->m_setcompressoptions->m_LEName->setEnabled(true);
            }
        }

        if (!temp->m_setcompressoptions->isNoSetting)
        {
            g_bAbortCompression = true;
            return;
        }

        while (*it) {
        
            QString     name = (*it)->text(0);
            QVariant v = (*it)->data(0, Qt::UserRole);
            int levelType = v.toInt();
            childcount = (*it)->childCount();
            int miplevels = 0;
        
        
            if (levelType == TREETYPE_IMAGEFILE_DATA)
            {
                if (childcount == 1)
                {
                    if (*it)
                    {
                        QTreeWidgetItem * Imageitem = (*it);
                        QString   Setting = Imageitem->text(0);
                        QVariant v = (*it)->data(0, Qt::UserRole);
                        int sublevelType = v.toInt();
                        if (sublevelType == TREETYPE_IMAGEFILE_DATA)
                        {
                            if (temp)
                            {        
                                QVariant v = Imageitem->data(1, Qt::UserRole);
                                C_Source_Image *m_imagefile = v.value<C_Source_Image *>();
                                QFileInfo fileinfo(m_imagefile->m_Name);
                                temp->m_setcompressoptions->m_data.m_sourceFileNamePath = m_imagefile->m_Full_Path;
                                temp->m_setcompressoptions->m_data.m_SourceImageSize = m_imagefile->m_ImageSize;
                                temp->m_setcompressoptions->m_data.m_SourceIscompressedFormat = CompressedFormat(m_imagefile->m_Format);
                                temp->m_setcompressoptions->m_data.m_SourceIsFloatFormat = FloatFormat(m_imagefile->m_Format);
        
                                int count = Imageitem->childCount();
        
                                if (m_imagefile->m_extnum <= count)
                                    m_imagefile->m_extnum = count;
        
                                temp->m_setcompressoptions->m_extnum = m_imagefile->m_extnum++;
                                temp->m_setcompressoptions->m_data.m_Width = m_imagefile->m_Width;
                                temp->m_setcompressoptions->m_data.m_Height = m_imagefile->m_Height;
        
                                temp->m_setcompressoptions->m_data.m_compname = fileinfo.baseName();
        
                                temp->m_setcompressoptions->m_data.m_editing = false;
                                temp->m_setcompressoptions->m_item = Imageitem;
                                //emit temp->m_setcompressoptions->m_data.compressionChanged((QVariant &)temp->m_setcompressoptions->m_data.m_Compression);
                                temp->m_setcompressoptions->SaveCompressedInfo();
                            }
                            //else
                            //    emit AddCompressSettings(Imageitem);
                        }
                    }
                }
            }
        
            // last item should be a vaild one unless we ended in a 
            // child loop that was the last element in the tree if so break out were done..
            if (*it)
                ++it;
            else
                break;
        
        
        }
 
        temp->m_setcompressoptions->isNoSetting = false;
    }

}

void CompressFiles(
    QFile                *file,
    ProjectView          *ProjectView
    )
{
    C_Destination_Options setDefaultOptions;


    ProjectView->m_CompressStatusDialog->showOutput();

    ProjectView->m_pProgressDlg->SetValue(0);
    ProjectView->m_pProgressDlg->SetHeader("Processing");
    ProjectView->m_pProgressDlg->SetLabelText("");
    ProjectView->m_pProgressDlg->show();

    // Use STD vectors to hold argv ** and keep the data in scope
    typedef std::vector<char> CharArray;
    typedef std::vector<CharArray> ArgumentVector;
    ArgumentVector argvVec;
    std::vector<char *> argv;

    ProjectView->m_bCompressing = true;

    int NumberOfItemCompressed = 0;
    int NumberOfItemCompressedFailed = 0;
    int NumberOfItemsSkipped = 0;
    int childcount = 0;
    MipSet *sourceImageMipSet;

    // Parse the Project view tree
    QTreeWidgetItemIterator it(ProjectView->m_projectTreeView);
    
    while (*it) {

        QString     name = (*it)->text(0);
        QVariant v = (*it)->data(0, Qt::UserRole);
        int levelType = v.toInt();
        childcount = (*it)->childCount();
        int miplevels = 0;
       

        if (levelType == TREETYPE_IMAGEFILE_DATA)
        {

            QVariant v = (*it)->data(1, Qt::UserRole);
            C_Source_Image *m_data = v.value<C_Source_Image *>();
            QString     FilePathName;
            FilePathName = m_data->m_Full_Path;

            sourceImageMipSet = NULL;
            if (m_data->m_MipImages)
            {
                if (m_data->m_MipImages->mipset)
                {
                    miplevels           = m_data->m_MipImages->mipset->m_nMipLevels;
                    sourceImageMipSet   = m_data->m_MipImages->mipset;
                }
            }

            // Image with a setting, childcount will be at least 2 as the "add compress setting" node is counted as 1 child
            if (childcount > 1)
            {
                argvVec.clear();
                argv.clear();
                
                // now get the child elements
                for (int i = 0; i < childcount; i++)
                {
                    if (g_bAbortCompression)
                        break;

                    ++it;
                    if (*it)
                    {
                        argvVec.clear();
                        argv.clear();

                        // Push App name string
                        string app = "CompressonatorCLI.exe";
                        argvVec.push_back(CharArray(app.begin(), app.end()));
                        argvVec.back().push_back(0); // Terminate String
                        argv.push_back(argvVec.back().data());

                        // Push string
                        string SourceFile = FilePathName.toStdString();
                        argvVec.push_back(CharArray(SourceFile.begin(), SourceFile.end()));
                        argvVec.back().push_back(0); // Terminate String
                        argv.push_back(argvVec.back().data());

                        QTreeWidgetItem * Imageitem = (*it);
                        QString   Setting = Imageitem->text(0);
                        QVariant v = (*it)->data(0, Qt::UserRole);
                        int sublevelType = v.toInt();
                        // save the settings item 
                        if (sublevelType == TREETYPE_COMPRESSION_DATA)
                        {
                            v = Imageitem->data(1, Qt::UserRole);
                            C_Destination_Options *data = v.value<C_Destination_Options *>();
                            bool testitem = false;

                            //Compression setting and item is checked
                            if (ProjectView->m_EnableCheckedItemsView)
                            {
                                //qDebug() << "override checked item using m_EnableCheckedItemsView";
                                testitem = (Imageitem->checkState(0) == Qt::Checked);
                            }
                            else
                            {
                                //qDebug() << "override checked item is selected";
                                testitem = Imageitem->isSelected();
                            }

                            if (data)
                            {
                                if (!testitem)
                                    testitem = data->m_isselected;
                            }

                            //qDebug() << "testitem " << testitem;
                            if (data && testitem)
                            {
                                //qDebug() << " valid Data";
                                // Reset force compression use flag
                                data->m_isselected = false;

                                // Since we are now compressing 
                                // Indicate that the resulting files data 
                                // has been changed through compression
                                // so that image view will reload a new image
                                // This flag is also set when compression data has been edited
                                data->m_data_has_been_changed = true;

                                // make compression format and file extension compatible
                                makeFormatExtCompatible(data);

                                //"Destination" = data->m_destFileNamePath
                                string DestinationFile = data->m_destFileNamePath.toStdString();
                                QString msgCommandLine;
                                bool useWeightChannel = false;
                                bool useAlphaChannel = false;
                                QFileInfo fileInfo(data->m_destFileNamePath);
                                QDir dir(fileInfo.absoluteDir());
                                QString DestPath = dir.absolutePath();
                                if (!dir.exists()) {
                                    dir.mkpath(".");
                                }

                                ProjectView->m_pProgressDlg->SetValue(0);

                                if (ProjectView->m_CompressStatusDialog)
                                {
                                    msgCommandLine = "<b>Processing: ";
                                    msgCommandLine.append(data->m_compname);
                                    msgCommandLine.append("<\b>");
                                    ProjectView->m_CompressStatusDialog->appendText(msgCommandLine);
                                }

                                // Saving paramaters to Batch file
                                if (file != NULL)
                                {
                                    msgCommandLine = "";
                                }
                                else
                                {
                                    // Compressing
                                    msgCommandLine = "CompressonatorCLI.exe  ";
                                }

                                // Check that the paths dont contain spaces else we need to add quotes
                                // to the paths, this ensures path is ok for cut and paste if user decided
                                // to use this for a command line exec or outputs to a batch file
                                QString addqstr = " ";
                                if (QString(data->m_sourceFileNamePath).contains(" ") ||
                                    QString(data->m_destFileNamePath).contains(" ")) addqstr = "\"";

                                msgCommandLine.append(addqstr);
                                msgCommandLine.append(data->m_sourceFileNamePath);
                                msgCommandLine.append(addqstr);
                                msgCommandLine.append(" ");
                                msgCommandLine.append(addqstr);
                                msgCommandLine.append(data->m_destFileNamePath);
                                msgCommandLine.append(addqstr);
                                msgCommandLine.append(" ");

                                argvVec.push_back(CharArray(DestinationFile.begin(), DestinationFile.end()));
                                argvVec.back().push_back(0); // Terminate String
                                argv.push_back(argvVec.back().data());


                                //"-fd Format"
                                QMetaObject meta = C_Destination_Options::staticMetaObject;
                                const char* key = NULL;

                                int indexCompression = meta.indexOfEnumerator("eCompression");
                                QMetaEnum metaEnumCompression = meta.enumerator(indexCompression);
                                key = metaEnumCompression.valueToKey(data->m_Compression);
                                CMP_FORMAT cmp_format = ParseFormat((CMP_CHAR *)key);

                                //"fd" = key
                                if (key != NULL)
                                {
                                    string format = "-fd";
                                    argvVec.push_back(CharArray(format.begin(), format.end()));
                                    argvVec.back().push_back(0); // Terminate String
                                    argv.push_back(argvVec.back().data());
                                    string formatValue = key;

                                    msgCommandLine.append(" -fd ");
                                    msgCommandLine.append(key);
                                    msgCommandLine.append(" ");

                                    argvVec.push_back(CharArray(formatValue.begin(), formatValue.end()));
                                    argvVec.back().push_back(0); // Terminate String
                                    argv.push_back(argvVec.back().data());
                                }
                                
                                ////using GPU to compress
                                if (!g_useCPUEncode) 
                                {
                                    string format = key;
                                    if (format == "BC1" || format == "DXT1" || format == "BC7" || format == "BC6H")
                                    {
                                        string usegpu;
                                         msgCommandLine.append(" -EncodeWith ");
                                         usegpu = "-EncodeWith";
                                         argvVec.push_back(CharArray(usegpu.begin(), usegpu.end()));
                                         argvVec.back().push_back(0); // Terminate String
                                         argv.push_back(argvVec.back().data());

                                        if (encodewith == C_Application_Options::ImageEncodeWith::GPU_OpenCL)
                                        {
                                            msgCommandLine.append(" OpenCL ");
                                            usegpu = "OpenCL";
                                            argvVec.push_back(CharArray(usegpu.begin(), usegpu.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                        }
                                        else if (encodewith == C_Application_Options::ImageEncodeWith::GPU_Vulkan)
                                        {
                                            msgCommandLine.append(" Vulkan ");
                                            usegpu = "Vulkan";
                                            argvVec.push_back(CharArray(usegpu.begin(), usegpu.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                        }
                                        else if (encodewith == C_Application_Options::ImageEncodeWith::GPU_DirectX)
                                        {
                                            msgCommandLine.append(" DirectX ");
                                            usegpu = "DirectX";
                                            argvVec.push_back(CharArray(usegpu.begin(), usegpu.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                        }
                                    }
                                    else 
                                    {
                                        g_useCPUEncode = true;
                                    }
                                }

                                // MipLevels
                                if (miplevels > 1)
                                {
                                    msgCommandLine.append(" -miplevels ");
                                    msgCommandLine.append(QString::number(miplevels - 1));
                                    msgCommandLine.append(" ");

                                    string smiplevel = "-miplevels";
                                    argvVec.push_back(CharArray(smiplevel.begin(), smiplevel.end()));
                                    argvVec.back().push_back(0); // Terminate String
                                    argv.push_back(argvVec.back().data());

                                    smiplevel = std::to_string(miplevels - 1);
                                    argvVec.push_back(CharArray(smiplevel.begin(), smiplevel.end()));
                                    argvVec.back().push_back(0); // Terminate String
                                    argv.push_back(argvVec.back().data());

                                }

                                //=============================
                                // Quality Settings
                                //=============================
                                if (FormatSupportsQualitySetting(cmp_format))
                                {
                                    if (data->m_Quality != setDefaultOptions.m_Quality)
                                    {
                                        // User Msg
                                        QString value = QString::number(data->m_Quality, 'f', 4);
                                        msgCommandLine.append(" -Quality ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string squality = "-Quality";
                                        argvVec.push_back(CharArray(squality.begin(), squality.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        squality = value.toStdString(); //  std::to_string(data->m_Quality);
                                        argvVec.push_back(CharArray(squality.begin(), squality.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());
                                    }
                                }

                                if (FormatSupportsDXTCBase(cmp_format))
                                {
                                    //=============================
                                    // Channel Weighting
                                    //=============================
                                    if (data->X_RED != setDefaultOptions.X_RED)
                                    {
                                        // User Msg
                                        QString value = QString::number(data->X_RED, 'f', 4);
                                        if (!useWeightChannel)
                                        {
                                            msgCommandLine.append(" -UseChannelWeighting 1 ");
                                            useWeightChannel = true;

                                            // User Setting Text
                                            string suseweighChannel = "-UseChannelWeighting";
                                            argvVec.push_back(CharArray(suseweighChannel.begin(), suseweighChannel.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                            // User Setting Value
                                            suseweighChannel = "1";
                                            argvVec.push_back(CharArray(suseweighChannel.begin(), suseweighChannel.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                        }

                                        msgCommandLine.append(" -WeightR ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string sweightr = "-WeightR";
                                        argvVec.push_back(CharArray(sweightr.begin(), sweightr.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        sweightr = value.toStdString();
                                        argvVec.push_back(CharArray(sweightr.begin(), sweightr.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());
                                    }

                                    if (data->Y_GREEN != setDefaultOptions.Y_GREEN)
                                    {
                                        // User Msg
                                        QString value = QString::number(data->Y_GREEN, 'f', 4);
                                        if (!useWeightChannel)
                                        {
                                            msgCommandLine.append(" -UseChannelWeighting 1 ");
                                            useWeightChannel = true;

                                            // User Setting Text
                                            string suseweighChannel = "-UseChannelWeighting";
                                            argvVec.push_back(CharArray(suseweighChannel.begin(), suseweighChannel.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                            // User Setting Value
                                            suseweighChannel = "1";
                                            argvVec.push_back(CharArray(suseweighChannel.begin(), suseweighChannel.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                        }

                                        msgCommandLine.append(" -WeightG ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string sweightg = "-WeightG";
                                        argvVec.push_back(CharArray(sweightg.begin(), sweightg.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        sweightg = value.toStdString();
                                        argvVec.push_back(CharArray(sweightg.begin(), sweightg.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());
                                    }

                                    if (data->Z_BLUE != setDefaultOptions.Z_BLUE)
                                    {
                                        // User Msg
                                        QString value = QString::number(data->Z_BLUE, 'f', 4);
                                        if (!useWeightChannel)
                                        {
                                            msgCommandLine.append(" -UseChannelWeighting 1 ");
                                            useWeightChannel = true;

                                            // User Setting Text
                                            string suseweighChannel = "-UseChannelWeighting";
                                            argvVec.push_back(CharArray(suseweighChannel.begin(), suseweighChannel.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                            // User Setting Value
                                            suseweighChannel = "1";
                                            argvVec.push_back(CharArray(suseweighChannel.begin(), suseweighChannel.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                        }

                                        msgCommandLine.append(" -WeightB ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string sweightb = "-WeightB";
                                        argvVec.push_back(CharArray(sweightb.begin(), sweightb.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        sweightb = value.toStdString();
                                        argvVec.push_back(CharArray(sweightb.begin(), sweightb.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());
                                    }
                                }

                                // ====================================
                                // DXTC1 settings only
                                // ====================================
                                if (cmp_format == CMP_FORMAT_DXT1)
                                {
                                    if (data->Threshold != setDefaultOptions.Threshold)
                                    {
                                        // User Msg
                                        QString value = QString::number(data->Threshold);
                                        if (!useAlphaChannel)
                                        {
                                            msgCommandLine.append(" -DXT1UseAlpha 1 ");
                                            useAlphaChannel = true;

                                            // User Setting Text
                                            string susealphaChannel = "-DXT1UseAlpha";
                                            argvVec.push_back(CharArray(susealphaChannel.begin(), susealphaChannel.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                            // User Setting Value
                                            susealphaChannel = "1";
                                            argvVec.push_back(CharArray(susealphaChannel.begin(), susealphaChannel.end()));
                                            argvVec.back().push_back(0); // Terminate String
                                            argv.push_back(argvVec.back().data());
                                        }

                                        msgCommandLine.append(" -AlphaThreshold ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string sthreshold = "-AlphaThreshold";
                                        argvVec.push_back(CharArray(sthreshold.begin(), sthreshold.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        sthreshold = value.toStdString();
                                        argvVec.push_back(CharArray(sthreshold.begin(), sthreshold.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        //g_CmdPrams.CompressOptions.bDXT1UseAlpha = true;
                                        //g_CmdPrams.CompressOptions.nAlphaThreshold = data->Threshold;
                                    }
                                }

                                // ====================================
                                // ASTC Settings
                                // ====================================
                                if (cmp_format == CMP_FORMAT_ASTC)
                                {
                                    if (data->m_Bitrate != setDefaultOptions.m_Bitrate)
                                    {
                                        // User Msg
                                        //QString value = data->m_correctBitrate;
                                        QString value = data->m_Bitrate;
                                        msgCommandLine.append(" -BlockRate ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string sbitrate = "-BlockRate";
                                        argvVec.push_back(CharArray(sbitrate.begin(), sbitrate.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        sbitrate = value.toStdString(); //  std::to_string(data->m_Bitrate);
                                        argvVec.push_back(CharArray(sbitrate.begin(), sbitrate.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());
                                    }
                                }

                                // ==========================================
                                // Input HDR Settings for Float->Byte process
                                // ==========================================
                                if (data->m_SourceIsFloatFormat && !(FloatFormat(cmp_format)))
                                {
                                    if (data->m_Defog != setDefaultOptions.m_Defog)
                                    {
                                        // User Msg
                                        QString value = QString::number(data->m_Defog, 'f', 4);
                                        msgCommandLine.append(" -InDefog ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string sdefog = "-InDefog";
                                        argvVec.push_back(CharArray(sdefog.begin(), sdefog.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        sdefog = value.toStdString(); //  std::to_string(data->m_Quality);
                                        argvVec.push_back(CharArray(sdefog.begin(), sdefog.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());
                                    }

                                    if (data->m_Exposure != setDefaultOptions.m_Exposure)
                                    {
                                        // User Msg
                                        QString value = QString::number(data->m_Exposure, 'f', 4);
                                        msgCommandLine.append(" -InExposure ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string sexposure = "-InExposure";
                                        argvVec.push_back(CharArray(sexposure.begin(), sexposure.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        sexposure = value.toStdString(); //  std::to_string(data->m_Quality);
                                        argvVec.push_back(CharArray(sexposure.begin(), sexposure.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());
                                    }

                                    if (data->m_KneeLow != setDefaultOptions.m_KneeLow)
                                    {
                                        // User Msg
                                        QString value = QString::number(data->m_KneeLow, 'f', 4);
                                        msgCommandLine.append(" -InKneeLow ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string skneelow = "-InKneeLow";
                                        argvVec.push_back(CharArray(skneelow.begin(), skneelow.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        skneelow = value.toStdString(); //  std::to_string(data->m_Quality);
                                        argvVec.push_back(CharArray(skneelow.begin(), skneelow.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());
                                    }

                                    if (data->m_KneeHigh != setDefaultOptions.m_KneeHigh)
                                    {
                                        // User Msg
                                        QString value = QString::number(data->m_KneeHigh, 'f', 4);
                                        msgCommandLine.append(" -InKneeHigh ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string skneehigh = "-InKneeHigh";
                                        argvVec.push_back(CharArray(skneehigh.begin(), skneehigh.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        skneehigh = value.toStdString(); //  std::to_string(data->m_Quality);
                                        argvVec.push_back(CharArray(skneehigh.begin(), skneehigh.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());
                                    }

                                    if (data->m_Gamma != setDefaultOptions.m_Gamma)
                                    {
                                        // User Msg
                                        QString value = QString::number(data->m_Gamma, 'f', 4);
                                        msgCommandLine.append(" -Gamma ");
                                        msgCommandLine.append(value);
                                        msgCommandLine.append(" ");

                                        // User Setting Text
                                        string sgamma = "-Gamma";
                                        argvVec.push_back(CharArray(sgamma.begin(), sgamma.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());

                                        // User Setting Value
                                        sgamma = value.toStdString(); //  std::to_string(data->m_Quality);
                                        argvVec.push_back(CharArray(sgamma.begin(), sgamma.end()));
                                        argvVec.back().push_back(0); // Terminate String
                                        argv.push_back(argvVec.back().data());
                                    }
                                }


                                //===========================
                                // Exporting to Batch file
                                //===========================
                                if (file != NULL)
                                {
                                    if (msgCommandLine.length() > 0)
                                    {
                                        file->write("CompressonatorCLI.exe ");
                                        file->write(msgCommandLine.toLatin1());
                                        file->write("\n");
                                    }

                                    if (ProjectView->m_CompressStatusDialog)
                                    {
                                        ProjectView->m_CompressStatusDialog->appendText(" Done");
                                    }
                                }
                                else
                                {
                                    //======================
                                    // Compression Data
                                    //======================

                                    //Show Command line to user
                                    ProjectView->m_CompressStatusDialog->appendText(msgCommandLine);
                                    QString msg = "File: " + data->m_compname;

                                    ProjectView->m_pProgressDlg->SetLabelText(msg);

                                    emit ProjectView->OnProcessing(data->m_destFileNamePath);
                                    // Pass over the command line params
                                    if (ParseParams(argv.size(), (CMP_CHAR **)argv.data()))
                                    {

                                        // Overriding some Command Line Features 
                                        g_CmdPrams.showperformance = true;
                                        g_CmdPrams.conversion_fDuration = 0;

                                        g_CmdPrams.doDecompress = false;

                                        #ifdef SAVE_TEMP_FILE
                                        if (m_data->m_ImageSize > 0)
                                        {
                                            // Force a decompress file to be used
                                            // by default we use .bmp but if source format is EXR change destination to exr
                                            if ((m_data->m_Format == CMP_FORMAT_ARGB_32F) ||
                                                (m_data->m_Format == CMP_FORMAT_BC6H)     ||
                                                (data->m_Compression == CMP_FORMAT_BC6H)
                                                )
                                            {
                                                QFileInfo info(data->m_decompressedFileNamePath);
                                                QString croped_fileName = data->m_decompressedFileNamePath.split(".", QString::SkipEmptyParts).at(0);
                                                QString strNewName = croped_fileName + ".exr";
                                                data->m_decompressedFileNamePath = strNewName;
                                            }
                                            g_CmdPrams.DecompressFile = data->m_decompressedFileNamePath.toStdString();
                                            g_CmdPrams.doDecompress = true;
                                        }
                                        #endif

                                        // Do the Compression by loading a new MIP set
                                        if (ProcessCMDLine(&ProgressCallback, sourceImageMipSet) == 0)
                                        {

                                            if (g_bAbortCompression)
                                            {
                                                Imageitem->setIcon(0, QIcon(QStringLiteral(":/CompressonatorGUI/Images/smallGrayStone.png")));
                                            }
                                            else
                                                // Success in compression
                                                if (ProjectView->Tree_updateCompressIcon(Imageitem, data->m_destFileNamePath, true))
                                                {
                                                    // Destination File Size
                                                    QFile fileInfo(data->m_destFileNamePath);
                                                    data->m_FileSize = fileInfo.size();
                                                    if (data->m_FileSize > 1024000)
                                                        data->m_FileSizeStr = QString().number((double)data->m_FileSize / 1024000, 'f', 2) + " MB";
                                                    else
                                                        if (data->m_FileSize > 1024)
                                                            data->m_FileSizeStr = QString().number((double)data->m_FileSize / 1024, 'f', 1) + " KB";
                                                        else
                                                            data->m_FileSizeStr = QString().number(data->m_FileSize) + " Bytes";
                                                    // Add Compressoin Time
                                                    if (g_CmdPrams.conversion_fDuration > 0)
                                                    {
                                                        data->m_CompressionTime = g_CmdPrams.conversion_fDuration;
                                                        double CompressionRatio = data->m_SourceImageSize / (double)data->m_FileSize;
                                                        char buffer[128];
                                                        sprintf(buffer, "%2.2f", CompressionRatio);
                                                        data->m_CompressionRatio = QString("%1 to 1").arg(buffer);

                                                        if (g_CmdPrams.conversion_fDuration < 60)
                                                            data->m_CompressionTimeStr = QString().number((double)data->m_CompressionTime, 'f', 3) + " Sec";
                                                        else
                                                            if (g_CmdPrams.conversion_fDuration < 3600)
                                                                data->m_CompressionTimeStr = QString().number((double)data->m_CompressionTime / 60, 'f', 2) + " Min";
                                                            else
                                                                data->m_CompressionTimeStr = QString().number((double)data->m_CompressionTime / 3600, 'f', 2) + " Hrs";
                                                    }
                                                    else
                                                    {
                                                        data->m_CompressionTime = 0;
                                                        data->m_CompressionTimeStr = DESTINATION_IMAGE_NOTPROCESSED;
                                                    }

                                                    NumberOfItemCompressed++;
                                                }
                                                else
                                                {
                                                    NumberOfItemCompressedFailed++;
                                                }
                                        }
                                        else
                                        {
                                            NumberOfItemCompressedFailed++;
                                            Imageitem->setIcon(0, QIcon(QStringLiteral(":/CompressonatorGUI/Images/smallRedStone.png")));
                                        }

                                    }

                                    ProjectView->m_pProgressDlg->SetValue(0);

                                    argvVec.clear();
                                    argv.clear();
                                }
                            }
                        }
                        else
                        {
                            if (Imageitem->isSelected())
                                NumberOfItemsSkipped++;
                        }
                    }
                }
            }
        }

        // last item should be a vaild one unless we ended in a 
        // child loop that was the last element in the tree if so break out were done..
        if (*it)
            ++it;
        else
            break;

        if (g_bAbortCompression)
            break;

    }

    ProjectView->m_bCompressing = false;
    ProjectView->m_processFromContext = false;

    if (ProjectView->m_CompressStatusDialog && (file == NULL) && (!g_bAbortCompression))
    {
        if ((NumberOfItemCompressed == 0) && (NumberOfItemCompressedFailed == 0))
        {

            ProjectView->m_CompressStatusDialog->appendText("No valid image compression setting(s) are selected. A compression setting window will pop up, if not,");
            ProjectView->m_CompressStatusDialog->appendText("try:");
            ProjectView->m_CompressStatusDialog->appendText("    - adding images to the project view by \"double click here to add files...\"");
            ProjectView->m_CompressStatusDialog->appendText("      and set destination options by expand the tree view - clicking on the little arrow on the left side of the orignal image");
            ProjectView->m_CompressStatusDialog->appendText("      and double click on \"(+) Add destination settings...\"");
            ProjectView->m_CompressStatusDialog->appendText("    - Selecting images to process with Mouse click");
            ProjectView->m_CompressStatusDialog->appendText("      on destination images");

        }
        else
        {
            if (g_bAbortCompression)
                ProjectView->m_CompressStatusDialog->appendText("<b>Compression was canceled!</b>");
            else
            {
                QString Msg;
                Msg.append("====== <b>Compress: ");
                Msg.append(QString::number(NumberOfItemCompressed));
                Msg.append(" succeeded, ");
                Msg.append(QString::number(NumberOfItemCompressedFailed));
                Msg.append(" failed");
                //if (NumberOfItemsSkipped > 0)
                //{
                //    Msg.append(", ");
                //    Msg.append(QString::number(NumberOfItemsSkipped));
                //    Msg.append(" skipped ");
                //}
                Msg.append("<\b> ======");
                ProjectView->m_CompressStatusDialog->appendText(Msg);
            }
        }
    }

    ProjectView->m_pProgressDlg->hide();

    ProjectView->m_bCompressing = false;
    g_bAbortCompression = false;

}


void ProjectView::compressProjectFiles(QFile *file)
{
    if (m_CompressStatusDialog == NULL) return;

    if (m_pProgressDlg == NULL) return;

    g_bAbortCompression = false;

    emit OnCompressionStart();

    // Handle local options that must be disabled while compressing
    if (actCompressProjectFiles)
        actCompressProjectFiles->setEnabled(false);
    if (actRemoveImage)
        actRemoveImage->setEnabled(false);

    CompressFiles(file, this);

    emit OnCompressionDone();

    if (actCompressProjectFiles)
        actCompressProjectFiles->setEnabled(true);

    if (actRemoveImage)
        actRemoveImage->setEnabled(true);
}

void ProjectView::viewDiffImageFromChild()
{
    // Get the active Image view node
    if (m_CurrentCompressedImageItem)
    {
        // view image
        QVariant v = m_CurrentCompressedImageItem->data(1, Qt::UserRole);
        C_Destination_Options *m_data = v.value<C_Destination_Options *>();
        if (m_data)
        {
            QFileInfo fileinfo(m_data->m_destFileNamePath);
            QFile file(m_data->m_destFileNamePath);
            if (file.exists() && (fileinfo.suffix().length() > 0))
            {
                QTreeWidgetItem *parent = m_CurrentCompressedImageItem->parent();
                QString sourcefile = GetSourceFileNamePath(parent);

                QFileInfo fileinfo(sourcefile);
                QFile file(sourcefile);
                if (file.exists() && (fileinfo.suffix().length() > 0))
                {
                    m_curDiffSourceFile = sourcefile;
                    m_curDiffDestFile = m_data->m_destFileNamePath;

                    diffImageFiles();
                    //emit ViewImageFileDiff(m_data, sourcefile, m_data->m_destFileNamePath);
                }
            }
        }
    }
}

void ProjectView::viewImageDiff()
{

    // Get the active Image view node
    if (m_CurrentCompressedImageItem)
    {
        // view image
        QVariant v = m_CurrentCompressedImageItem->data(1, Qt::UserRole);
        C_Destination_Options *m_data = v.value<C_Destination_Options *>();
        if (m_data)
        {
            QFileInfo fileinfo(m_data->m_destFileNamePath);
            QFile file(m_data->m_destFileNamePath);
            if (file.exists() && (fileinfo.suffix().length() > 0))
            {
                QTreeWidgetItem *parent = m_CurrentCompressedImageItem->parent();
                QString sourcefile      = GetSourceFileNamePath(parent);

                QFileInfo fileinfo(sourcefile);
                QFile file(sourcefile);
                if (file.exists() && (fileinfo.suffix().length() > 0))
                {
                    emit ViewImageFileDiff(m_data, sourcefile, m_data->m_destFileNamePath);
                    // emit ViewImageFileDiff(sourcefile, m_data->m_destFileNamePath);
                }
            }
        }
    }
}

void ProjectView::Tree_selectAllChildItems(QTreeWidgetItem *item)
{
    if (item)
    {
        if (item->childCount() > 0)
        {
            for (int r = 0; r < item->childCount(); r++)
            {
                QTreeWidgetItem *child = item->child(r);
                child->setSelected(true);
            }

        }
    }
}


int  ProjectView::Tree_numSelectedtems(int &ItemsCount)
{
    ItemsCount = 0;
    int numSelected = 0;
    // Find all items and count the number selected Compression Formats
    QTreeWidgetItemIterator it(m_treeRootItem);
    QVariant v;
    int levelType;

    while (*it) {
        v = (*it)->data(0, Qt::UserRole);
        levelType = v.toInt();

        // exclude the add item count
        if ((*it)->childCount() > 1)
        {
            for (int r = 0; r < (*it)->childCount(); r++)
            {
                QTreeWidgetItem *child = (*it)->child(r);

                if (child)
                {
                    v = child->data(0, Qt::UserRole);
                    levelType = v.toInt();

                    if (child->isSelected())
                    {
                        if (levelType == TREETYPE_COMPRESSION_DATA)
                        {
                            numSelected++;
                        }
                    }
                    ItemsCount++;
                }
            }
        }

        ++it;
    }
    return numSelected;
}



int  ProjectView::Tree_numCompresstemsSelected(int &ItemsCount, int &NumCompressedItems)
{
    ItemsCount = 0;
    NumCompressedItems = 0;

    int numSelected = 0;
    // Find all items and count the number selected Compression Formats
    QTreeWidgetItemIterator it(m_treeRootItem);
    QVariant v;
    int levelType;

    while (*it) {
        v = (*it)->data(0, Qt::UserRole);
        levelType = v.toInt();

        // exclude the add item count
        if ((*it)->childCount() > 1)
        {
            ItemsCount += (*it)->childCount() - 1;

            for (int r = 0; r < (*it)->childCount(); r++)
            {
                QTreeWidgetItem *child = (*it)->child(r);

                if (child)
                {
                    v = child->data(0, Qt::UserRole);
                    levelType = v.toInt();
                    if (levelType == TREETYPE_COMPRESSION_DATA)
                    {
                        NumCompressedItems++;
                        if (child->isSelected())
                            numSelected++;
                    }

                }
            }
        }

        ++it;
    }
    return numSelected;
}



void  ProjectView::Tree_clearAllItemsSetected()
{
    // Find the item and set it as selected
    QTreeWidgetItemIterator it(m_treeRootItem);
    while (*it) {
        if (m_EnableCheckedItemsView)
        {
            (*it)->setCheckState(0, Qt::Unchecked);
        }
        (*it)->setSelected(false);
        ++it;
    }
}


void ProjectView::Tree_setAllItemsSetected()
{
    // Find the item and set it as selected
    QTreeWidgetItemIterator it(m_treeRootItem);
    while (*it) {
        if (m_EnableCheckedItemsView)
        {
            (*it)->setCheckState(0, Qt::Checked);
        }
        (*it)->setSelected(true);
        ++it;
    }
}

void ProjectView::onTreeMousePress(QMouseEvent  *event)
{
    m_CurrentItem = m_projectTreeView->m_currentItem;

    if (m_CurrentItem)
    {
        //bool SHIFT_Key = event->modifiers() & Qt::ShiftModifier;
        bool CTRL_key = event->modifiers() & Qt::ControlModifier;
        //bool MouseLeft = event->button() & Qt::LeftButton;

        if (CTRL_key)
        {
            bool selected = m_CurrentItem->isSelected();
            int childCount = m_CurrentItem->childCount();
            if (childCount > 1)
            {
                QTreeWidgetItem *childItem;
                // Vaild Index start from 0 to childCount
                while (childCount > 1)
                {
                    childItem = m_CurrentItem->child(childCount - 1);
                    if (childItem)
                        childItem->setSelected(selected);
                    childCount--;
                }
            }
        }
    }

}

void ProjectView::onTreeKeyPress(QKeyEvent  *event)
{
    Q_UNUSED(event);
    m_CurrentItem = m_projectTreeView->m_currentItem;
}


void ProjectView::onImageLoadStart()
{
    if (actCompressProjectFiles)
        actCompressProjectFiles->setEnabled(false);
    if (actRemoveImage)
        actRemoveImage->setEnabled(false);
}

void ProjectView::onImageLoadDone()
{
    if (actCompressProjectFiles)
        actCompressProjectFiles->setEnabled(true);
    if (actRemoveImage)
        actRemoveImage->setEnabled(true);
}