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

#include "cpSetCompressOptions.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"


#define DROPDOWN_FILEEXT_WIDTH  60

extern void GetSupportedFileFormats(QList<QByteArray> &g_supportedFormats);

#ifdef USE_TREEVIEW_PROP
QTreeWidgetItem* CSetCompressOptions::AddRootItem(int col, QString itemtext, bool checkable)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(m_trewwview);
    treeItem->setText(col, itemtext);
    if (checkable)
    {
        treeItem->setFlags(treeItem->flags() | Qt::ItemIsUserCheckable);
        treeItem->setCheckState(0, Qt::Unchecked);
    }
    treeItem->addChild(treeItem);
    return treeItem;
}

QTreeWidgetItem* CSetCompressOptions::AddChildItem(QTreeWidgetItem *parent, int col, QString itemtext, bool checkable)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(parent);
    treeItem->setText(col, itemtext);
    if (checkable)
    {
        treeItem->setFlags(treeItem->flags() | Qt::ItemIsUserCheckable);
        treeItem->setCheckState(0, Qt::Unchecked);
    }
    treeItem->addChild(treeItem);
    return treeItem;
}
#endif


CSetCompressOptions::CSetCompressOptions(const QString title, QWidget *parent) : QDialog(parent), //QDockWidget(title, parent),
    m_title(title),
    m_parent(parent)
{
    changeSelf        = false;
    m_propQuality    = NULL;
    isEditing        = false;

    setWindowTitle(title);
    Qt::WindowFlags flags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    setWindowFlags(flags);

    //--------------------------------
    // Destination Name
    //--------------------------------
    QLabel *LName = new QLabel(tr("Name"),this);
    m_LEName = new QLineEdit(this);
    m_LEName->setEchoMode(QLineEdit::Normal);
    QObject::connect(m_LEName, SIGNAL(textChanged(QString)), this, SLOT(onNameTextChanged(QString)));
    QObject::connect(m_LEName, SIGNAL(editingFinished()), this, SLOT(onNameEditingFinished()));

    
    //=========================================================
    // Combo box for type of files supported by plugin manager 
    //=========================================================
    m_fileFormats = new QComboBox(this);
    m_fileFormats->setMinimumWidth(DROPDOWN_FILEEXT_WIDTH);
    QList<QByteArray> m_supportedFormats;
    GetSupportedFileFormats(m_supportedFormats);
    
    m_AllFileTypes.clear();
    
    QList<QByteArray>::Iterator i;
    for (i = m_supportedFormats.begin(); i != m_supportedFormats.end(); ++i)
    {
        QByteArray fformat = (*i);
        QString item = fformat;
        m_AllFileTypes.append(item);
    }
    
    // Default output format
    m_fileFormats->addItem("DDS");
    m_fileFormats->addItem("KTX");

    // =================================================
    // Destination File 
    // =================================================
    QLabel *m_lFolder = new QLabel(this);
    m_lFolder->setText("Folder");
    m_DestinationFolder = new QLineEdit(this);
    m_PBDestFileFolder  = new QPushButton("...",this);
    m_PBDestFileFolder->setMaximumWidth(30);
    QObject::connect(m_PBDestFileFolder, SIGNAL(clicked()), this, SLOT(onDestFileFolder()));

    m_DestinationFile = new QLineEdit();
    QObject::connect(m_DestinationFile, SIGNAL(editingFinished()), this, SLOT(onDestinationFileEditingFinished()));

    // =================================================
    // Object Browser
    // =================================================
    m_theController = new ObjectController(this, true);
    QtTreePropertyBrowser *browser = m_theController->getTreeBrowser();
    if (browser)
    {
        browser->setHeaderVisible(false);
        browser->SetBrowserClick(true);
        browser->setResizeMode(QtTreePropertyBrowser::ResizeToContents); // follow this comment Note#1
        connect(browser, SIGNAL(currentItemChanged(QtBrowserItem *)), this, SLOT(oncurrentItemChanged(QtBrowserItem *)));
    }


    connect(&m_data, SIGNAL(compressionChanged(QVariant &)), this, SLOT(compressionValueChanged(QVariant &)));
    connect(&m_data, SIGNAL(qualityChanged(QVariant &)), this, SLOT(qualityValueChanged(QVariant &)));

    m_theController->setObject(&m_data, true);

    // Set Editing Defaults 
    m_propQuality = m_theController->getProperty(COMPRESS_OPTIONS_QUALITY);
    m_propDestImage = m_theController->getProperty(DESTINATION_IMAGE_CLASS_NAME);

    //=================================
    // Text View for help and Hints
    //=================================

    m_infotext = new QTextBrowser(this);
    // Always show min Two lines of text and Max to 5 lines at font size 16
    m_infotext->setMinimumHeight(32);
    m_infotext->setMaximumHeight(96);
    m_infotext->setReadOnly(true);
    m_infotext->setAcceptRichText(true);

    //================
    // Buttons
    //================
    m_PBSaveSettings = new QPushButton("Save",this);
    m_PBCancel       = new QPushButton("Cancel",this);
    QObject::connect(m_PBSaveSettings, SIGNAL(clicked()), this, SLOT(PBSaveCompressSetting()));
    QObject::connect(m_PBCancel, SIGNAL(clicked()), this, SLOT(onPBCancel()));


#ifdef USE_TREEVIEW_PROP
    m_theControllerOptions = new ObjectController(this, true);
    QtTreePropertyBrowser *browser = m_theController->getTreeBrowser();
    if (browser)
    {
        browser->setHeaderVisible(false);
        browser->SetBrowserClick(true);
        browser->setResizeMode(QtTreePropertyBrowser::ResizeToContents); // follow this comment Note#1
    }

    m_trewwview = new QTreeWidget();
    m_trewwview->setHeaderHidden(true);

    // ASTC setup
    QTreeWidgetItem *parentItem;
    AddRootItem(0, "Compression subMode", false);
    AddRootItem(0, "Texture type and block Dimensions", false);
    parentItem = AddRootItem(0, "Additional options", false);
    AddChildItem(parentItem, 0, "Built-in error-weighting options", true);
    AddChildItem(parentItem, 0, "Low-Level error-weighting options", true);
    AddChildItem(parentItem, 0, "Low-Level performace quality tradeoff options", true);
    AddChildItem(parentItem, 0, "Other options", true);
    AddRootItem(0, "Mipmap generation", true);
#endif


    QGroupBox *GBDestinationFile = new QGroupBox(tr("Destination"));

    m_VlayoutWindow         = new QVBoxLayout(this);
    m_HlayoutName           = new QHBoxLayout(this);
    m_HlayoutDestination    = new QHBoxLayout(this);
    m_HlayoutButtons        = new QHBoxLayout(this);
    m_VlayoutDestination    = new QVBoxLayout(this);

    m_HlayoutName->addWidget(LName);
    m_HlayoutName->addWidget(m_LEName);
    m_HlayoutName->addWidget(m_fileFormats);

    m_HlayoutDestination->addWidget(m_lFolder);
    m_HlayoutDestination->addWidget(m_DestinationFolder);
    m_HlayoutDestination->addWidget(m_PBDestFileFolder);
    
    m_VlayoutDestination->addLayout(m_HlayoutName);
    m_VlayoutDestination->addLayout(m_HlayoutDestination);

    GBDestinationFile->setLayout(m_VlayoutDestination);


    m_HlayoutButtons->addStretch();
    m_HlayoutButtons->addWidget(m_PBSaveSettings);
    m_HlayoutButtons->addWidget(m_PBCancel);

    m_VlayoutWindow->addWidget(m_theController);
    m_VlayoutWindow->addWidget(m_infotext);
    m_VlayoutWindow->addWidget(GBDestinationFile);
    m_VlayoutWindow->addLayout(m_HlayoutButtons);

    setLayout(m_VlayoutWindow);

}

//===================================================================
// update FileType Combobox this event is not called
// when setText is used on a LineEdit
//===================================================================
bool CSetCompressOptions::updateFileFormat(QFileInfo &fileinfo)
{
    QString filetype = fileinfo.suffix();
    filetype = filetype.toUpper();
    int index = m_fileFormats->findText(filetype);
    if (index != -1)
    {
        m_fileFormats->setCurrentIndex(index);
        return true;
    }

    return false;
}


//==========================
// Destination NAME editing
//==========================

void    CSetCompressOptions::onNameEditingFinished()
{
    m_data.m_compname = m_LEName->displayText();
    m_data.m_FileInfoDestinationName = m_data.m_compname;
    m_theController->update();
}

void CSetCompressOptions::onNameTextChanged(QString text)
{
}


//===================================================================
// Check if compression is been specified 
//===================================================================

void CSetCompressOptions::compressionValueChanged(QVariant &value)
{
    C_Destination_Options::eCompression comp = (C_Destination_Options::eCompression &)value;
    QMessageBox msgBox;
    bool ok = false;
    QString Extension = m_fileFormats->currentText();
    bool compressedOptions = false;

    m_infotext->clear();
    m_infotext->append("<b>Format</b>");

    m_fileFormats->clear();

    if (m_propQuality)
        m_propQuality->setEnabled(true);

    //if (m_data.m_settoUseOnlyBC6)
    //{
    //    if (comp != C_Destination_Options::BC6H)
    //    {
    //        msgBox.setText("Only BC6H compression is supported for EXR files");
    //        msgBox.exec();
    //        m_data.m_Compression = C_Destination_Options::BC6H;
    //    }
    //}

    switch (comp)
    {
    case C_Destination_Options::BC6H:
                                compressedOptions = true;
                                //ok = (Extension.compare("DDS") == 0) | (Extension.compare("KTX") == 0);
                                //ok = (Extension.compare("EXR") == 0) || ok;
                                //if (ok == false)
                                //{
                                //    msgBox.setText("Extension " + Extension + " is not supported by BC6H format\nit will be changed to DDS");
                                //    msgBox.exec();
                                //}
                                //else
                                //{
                                //    if (!m_data.m_settoUseOnlyBC6)
                                //    {
                                //        msgBox.setText("Only BC6H compression is supported for EXR files\nSetting will be reset to BC7");
                                //        msgBox.exec();
                                //        m_data.m_Compression = C_Destination_Options::BC7;
                                //    }
                                //}
                                m_fileFormats->addItem("DDS");
                                m_fileFormats->addItem("KTX");
                                m_fileFormats->setCurrentIndex(0);
                                break;


    case C_Destination_Options::BC1:
    case C_Destination_Options::BC3:
    case C_Destination_Options::BC7:
    case C_Destination_Options::DXT3:
    case C_Destination_Options::DXT5:
                                compressedOptions = true;
                                //ok = (Extension.compare("DDS") == 0) | (Extension.compare("KTX") == 0);
                                //if (ok == false)
                                //{
                                //    msgBox.setText("Extension " + Extension + " is not supported by BCn compression formats\nit will be changed to DDS");
                                //    msgBox.exec();
                                //    Extension = "DDS";
                                //}
                                m_fileFormats->addItem("DDS");
                                m_fileFormats->addItem("KTX");
                                break;

    case C_Destination_Options::BC2:
    case C_Destination_Options::BC4:
    case C_Destination_Options::BC5:
    case C_Destination_Options::ATC_RGB:
    case C_Destination_Options::ATC_RGBA_Explicit:
    case C_Destination_Options::ATC_RGBA_Interpolated:
    case C_Destination_Options::ATI2N:
    case C_Destination_Options::ATI2N_XY:
    case C_Destination_Options::ATI2N_DXT5:
    case C_Destination_Options::DXT5_xGBR:
    case C_Destination_Options::DXT5_RxBG:
    case C_Destination_Options::DXT5_RBxG:
    case C_Destination_Options::DXT5_xRBG:
    case C_Destination_Options::DXT5_RGxB:
    case C_Destination_Options::DXT5_xGxR:
    case C_Destination_Options::ETC_RGB:
                                compressedOptions = true;
                                //ok = (Extension.compare("DDS") == 0) | (Extension.compare("KTX") == 0);
                                //if (ok == false)
                                //{
                                //    msgBox.setText("Extension " + Extension + " is not supported by BCn compression formats\nit will be changed to DDS");
                                //    msgBox.exec();
                                //    Extension = "DDS";
                                //}
                                m_fileFormats->addItem("DDS");
                                m_fileFormats->addItem("KTX");
                                break;

    // Enable when qualified  
    // case C_Destination_Options::ASTC:
    //                             ok = (Extension.compare("DDS") == 0) | (Extension.compare("ASTC") == 0) | (Extension.compare("KTX") == 0);
    //                             if (ok == false)
    //                             {
    //                                 msgBox.setText("Extension " + Extension + " is not supported by ASTC compression format \nit will be changed to ASTC");
    //                                 msgBox.exec();
    //                                 Extension = "ASTC";
    //                             }
    //                             m_fileFormats->addItem("ASTC");
    //                             m_fileFormats->addItem("DDS");
    //                             m_fileFormats->addItem("KTX");
    //                             break;

    default:
        m_fileFormats->addItems(m_AllFileTypes);
        if (m_propQuality)
            m_propQuality->setEnabled(false);

        break;
    }

    int i = m_fileFormats->findText(Extension);
    m_fileFormats->setCurrentIndex(i);

    if (compressedOptions)
        m_infotext->append("Destination file will be <b>Compressed</b> when processed");
    else
        m_infotext->append("Destination file will be <b>Transcoded</b> when processed");
}


//===================================================================
// Check if compression is been specified 
//===================================================================
void CSetCompressOptions::qualityValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Quality</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0 (Poor Quality) to 1 (High Quality) with default set at 0.05");
}

// -----------------------------------------------------------
// Signaled when items focus has changed on th property view
// ----------------------------------------
void CSetCompressOptions::oncurrentItemChanged(QtBrowserItem *item)
{
    if (!item) return;
    m_infotext->clear();

    QtProperty *treeItem = item->property();
    QString text;
    text = treeItem->propertyName();
    m_infotext->append("<b>" + text + "</b>");

    if (text.compare(COMPRESS_OPTIONS_FORMAT) == 0)
    {
        m_infotext->append("Sets destination image format");
    }
    else
        if (text.compare(COMPRESS_OPTIONS_QUALITY) == 0)
        {
            m_infotext->append(tr("Sets destinations image quality"));
            m_infotext->append(tr("For low values quality will be poor and the time to process the image will be short."));
            m_infotext->append(tr("Subsequently higher values will increase the quality and processing time"));
        }
}

CSetCompressOptions::~CSetCompressOptions()
{
}

void CSetCompressOptions::resetData()
{
    m_data << m_dataOriginal;
}


//===================================================================
// Called just before the display of this editor 
// normally called prior to Adding or Editing data
//===================================================================

bool CSetCompressOptions::updateDisplayContent()
{
    isEditing = true;

    m_dataOriginal << m_data;

    // Compression Name
    m_LEName->clear();
    m_LEName->insert(m_data.m_compname);
    m_data.m_FileInfoDestinationName = m_data.m_compname;

    // Check source file extension for special cases
    m_data.m_settoUseOnlyBC6 = false;
    QFileInfo fi(m_data.m_sourceFileNamePath);
    QString ext = fi.suffix().toUpper();
    if (ext.compare("EXR") == 0)
    {
        m_data.m_settoUseOnlyBC6 = true;
        m_data.m_Compression = C_Destination_Options::BC6H;
    }

    
    // Update Property Managed Settings and content view
    m_theController->setObject(&m_data, true, true);
    m_theController->update();

    // Destination FileName
    QString FileName;
    QFileInfo fileinfo;
    if (m_data.m_editing)
    {
        fileinfo.setFile(m_data.m_destFileNamePath);
        // Destination File Name
        FileName = fileinfo.baseName();
        if (FileName.length() <= 0) FileName = m_data.m_compname;
    }
    else
    {
        fileinfo.setFile(m_data.m_sourceFileNamePath);
        FileName = m_data.m_compname;
    }

    //m_DestinationFile->setText(FileName);

    // Destination Folder
    QDir dir(fileinfo.absoluteDir());
    QString DestFolder = dir.absolutePath();
    if ((DestFolder.length() <= 1) && DestFolder.contains("."))
    {
        DestFolder = "./Results";
    }
    m_DestinationFolder->setText(DestFolder);

    m_PBSaveSettings->setEnabled(true);

    if (m_propQuality)
    {
        m_propQuality->setToolTip(STR_QUALITY_SETTING_HINT);
        m_propQuality->setEnabled(true);
        // Set  Properties for editing
        QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propQuality->propertyManager();
        if (Manager)
        {
            QtVariantProperty *prop = Manager->variantProperty(m_propQuality);
            prop->setAttribute(STR_QUALITY_SETTING_MINIMUM,    0.0);
            prop->setAttribute(STR_QUALITY_SETTING_MAXIMUM,    1.0);
            prop->setAttribute(STR_QUALITY_SETTING_SINGLESTEP, 0.05);
        }
    }

    if (m_propDestImage)
    {
        m_propDestImage->setHidden(true);
    }

    return true;
}



void CSetCompressOptions::onPBCancel()
{
    resetData();
    hide();
    isEditing = false;

    // Obtain the Parent and its data
    // and reset the num of extension counts for that image
    QTreeWidgetItem *parent = m_item->parent();
    if (parent)
    {
        // Varify its root
        QVariant v = parent->data(0, Qt::UserRole);
        int ParentlevelType = v.toInt();
        if (ParentlevelType == TREETYPE_IMAGEFILE_DATA)
        {
            QVariant v = parent->data(1, Qt::UserRole);
            C_Source_Image *m_imagefile = v.value<C_Source_Image *>();
            if (m_imagefile)
                m_imagefile->m_extnum--;
        }
    }

}

void CSetCompressOptions::PBSaveCompressSetting()
{
    m_data.m_compname = m_LEName->displayText();
    m_data.m_FileInfoDestinationName = m_data.m_compname;

    // Path
    QString Path = m_DestinationFolder->text();
    QDir dir(Path);
    QString finalPath = dir.absolutePath();

    // Path+FileName
    finalPath.append("/");
    finalPath.append(m_data.m_compname);

    // Path+FileName+Ext
    QString ImageExt = m_fileFormats->currentText();

    if (m_fileFormats->currentIndex() >= 0 && !(ImageExt.isEmpty()))
    {
        finalPath.append(".");
        finalPath.append(ImageExt);
    }
    else
        finalPath.append(".DDS");

    m_data.m_destFileNamePath = finalPath;

    // check if user change the file name or path
    if (m_data.m_destFileNamePath.compare(m_dataOriginal.m_destFileNamePath) != 0)
                    m_data.m_data_has_been_changed = true;

    emit SaveCompressSettings(m_item, m_data);
    hide();
    isEditing = false;
}



// update FileType Combobox this event is not called 
// when setText is used on a LineEdit
void CSetCompressOptions::onDestinationFileEditingFinished()
{
    QFileInfo fileinfo(m_DestinationFile->text());

    // Update the FileType combobox
    updateFileFormat(fileinfo);

    // Strip extension from filename
    m_DestinationFile->setText(fileinfo.baseName());

}

void CSetCompressOptions::onDestFileFolder()
{
    QString fileFolder = QFileDialog::getExistingDirectory(this, tr("Destination Folder"), m_DestinationFolder->text());
    if (fileFolder.length() > 0)
    {
        m_DestinationFolder->setText(fileFolder);
    }
}