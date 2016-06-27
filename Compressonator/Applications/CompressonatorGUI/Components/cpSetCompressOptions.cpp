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
    m_propChannelWeightingR = NULL;
    m_propChannelWeightingG = NULL;
    m_propChannelWeightingB = NULL;
    m_propAlphaThreshold = NULL;
    m_propAdaptiveColor = NULL;
    m_propUseAlpha = NULL;
    m_propNoAlpha =NULL;
    m_propBitrate = NULL;
    isEditing        = false;
    isInit           = false;

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
    connect(&m_data, SIGNAL(redwChanged(QVariant &)), this, SLOT(redwValueChanged(QVariant &)));
    connect(&m_data, SIGNAL(greenwChanged(QVariant &)), this, SLOT(greenwValueChanged(QVariant &)));
    connect(&m_data, SIGNAL(bluewChanged(QVariant &)), this, SLOT(bluewValueChanged(QVariant &)));
    connect(&m_data, SIGNAL(thresholdChanged(QVariant &)), this, SLOT(thresholdValueChanged(QVariant &)));
    connect(&m_data, SIGNAL(noAlphaChannel()), this, SLOT(noAlphaChannelValue()));
    connect(&m_data, SIGNAL(hasAlphaChannel()), this, SLOT(hasAlphaChannelValue()));
    connect(&m_data, SIGNAL(bitrateChanged(QString &, int&, int&)), this, SLOT(bitrateValueChanged(QString &, int&, int&)));

    m_theController->setObject(&m_data, true);

    // Set Editing Defaults 
    m_propQuality = m_theController->getProperty(COMPRESS_OPTIONS_QUALITY);
    m_propFormat = m_theController->getProperty(COMPRESS_OPTIONS_FORMAT);
    m_propChannelWeightingR = m_theController->getProperty(COMPRESS_OPTIONS_CHANNEL_WEIGHTING_R);
    m_propChannelWeightingG = m_theController->getProperty(COMPRESS_OPTIONS_CHANNEL_WEIGHTING_G);
    m_propChannelWeightingB = m_theController->getProperty(COMPRESS_OPTIONS_CHANNEL_WEIGHTING_B);
    m_propAlphaThreshold = m_theController->getProperty(COMPRESS_OPTIONS_ALPHATHRESHOLD);
    m_propAdaptiveColor = m_theController->getProperty(COMPRESS_OPTIONS_ADAPTIVECOLOR);
    m_propUseAlpha = m_theController->getProperty(COMPRESS_OPTIONS_USEALPHA);
    m_propBitrate = m_theController->getProperty(COMPRESS_OPTIONS_BITRATE);
    m_propNoAlpha = m_theController->getProperty(COMPRESS_OPTIONS_NOALPHA);
    m_propDestImage = m_theController->getProperty(DESTINATION_IMAGE_CLASS_NAME);
    m_propChannelWeight = m_theController->getProperty(CHANNEL_WEIGHTING_CLASS_NAME);
    m_propDXT1Alpha     = m_theController->getProperty(DXT1_ALPHA_CLASS_NAME);
    m_propASTCBlockRate = m_theController->getProperty(ASTC_BLOCKRATE_CLASS_NAME);

    //=================================
    // Text View for help and Hints
    //=================================

    m_infotext = new QTextBrowser(this);
    // Always show min Two lines of text and Max to 5 lines at font size 16
    m_infotext->setMinimumHeight(40);
    m_infotext->setMaximumHeight(100);
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
    QString extension = "DDS";
    bool compressedOptions = false;
    bool colorWeightOptions = false;
    bool alphaChannelOptions = false;
    bool astcbitrateOptions = false;

    m_fileFormats->clear();

    if (m_propQuality)
        m_propQuality->setEnabled(true);
    if (m_propChannelWeightingR)
        m_propChannelWeightingR->setEnabled(true);
    if (m_propChannelWeightingG)
        m_propChannelWeightingG->setEnabled(true);
    if (m_propChannelWeightingB)
        m_propChannelWeightingB->setEnabled(true);
    if (m_propAlphaThreshold)
        m_propAlphaThreshold->setEnabled(true);
    if (m_propAdaptiveColor)
        m_propAdaptiveColor->setEnabled(true);
    if (m_propUseAlpha)
        m_propUseAlpha->setEnabled(true);
    if (m_propNoAlpha)
        m_propNoAlpha->setEnabled(true);
    if (m_propBitrate)
        m_propBitrate->setEnabled(true);

    switch (comp)
    {
    case C_Destination_Options::BC6H:
        compressedOptions = true;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_fileFormats->setCurrentIndex(0);
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("Block Compression (BC) format designed to support high-dynamic range (floating point) color spaces. (e.g. .exr extension image file).");
        break;             
    case C_Destination_Options::BC1:
    case C_Destination_Options::DXT1:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = true;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A four component opaque (or 1-bit alpha) compressed texture format for Microsoft DirectX10. DXT1 identical to BC1.  Four bits per pixel.");
        break;
    case C_Destination_Options::BC3:
    case C_Destination_Options::DXT5:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A four component compressed texture format with interpolated alpha for Microsoft DirectX10. DXT5 identical to BC3. Eight bits per pixel.");
        break;
    case C_Destination_Options::BC2:
    case C_Destination_Options::DXT3:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A four component compressed texture format with explicit alpha for Microsoft DirectX10. DXT3 identical to BC2. Eight bits per pixel.");
        break;
    case C_Destination_Options::BC4:
        compressedOptions = true;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A single component compressed texture format for Microsoft DirectX10. Identical to ATI1N. Four bits per pixel.");
        break;
    case C_Destination_Options::BC5:
    case C_Destination_Options::ATI2N:
    case C_Destination_Options::ATI2N_XY:
    case C_Destination_Options::ATI2N_DXT5:
        compressedOptions = true;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A two component compressed texture format for Microsoft DirectX10. BC5 identical to ATI2N. Eight bits per pixel.");
        break;
    case C_Destination_Options::ATC_RGB:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A compressed RGB format.");
        break;
    case C_Destination_Options::ATC_RGBA_Explicit:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A compressed ARGB format with explicit alpha.");
        break;
    case C_Destination_Options::ATC_RGBA_Interpolated:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A compressed ARGB format with interpolated alpha.");
        break;
    case C_Destination_Options::DXT5_xGBR:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(" DXT5 with the red component swizzled into the alpha channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_RxBG:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("swizzled DXT5 format with the green component swizzled into the alpha channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_RBxG:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(" swizzled DXT5 format with the green component swizzled into the alpha channel & the blue component swizzled into the green channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_xRBG:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("swizzled DXT5 format with the green component swizzled into the alpha channel & the red component swizzled into the green channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_RGxB:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("swizzled DXT5 format with the blue component swizzled into the alpha channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_xGxR:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("two-component swizzled DXT5 format with the red component swizzled into the alpha channel & the green component in the green channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::BC7:
        compressedOptions = true;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("The latest block Compression (BC) format designed to support high-quality compression of RGB and RGBA bytes color spaces.");
        break;
    case C_Destination_Options::ETC_RGB:
    case C_Destination_Options::ETC2_RGB:
        compressedOptions = true;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_fileFormats->addItem("DDS");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("ETC (Ericsson Texture Compression, lossy texture compression developed with Ericsson Research.)");
        break;
    case C_Destination_Options::ASTC:
        compressedOptions = true;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = true;
        extension = "KTX";
        m_fileFormats->addItem("ASTC");
        m_fileFormats->addItem("KTX");
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("ASTC (Adaptive Scalable Texture Compression),lossy block-based texture compression developed with ARM.");
        break;
    default:
        m_fileFormats->addItems(m_AllFileTypes);
        if (m_propQuality)
            m_propQuality->setEnabled(false);
        if (m_propChannelWeightingR)
            m_propChannelWeightingR->setEnabled(false);
        if (m_propChannelWeightingG)
            m_propChannelWeightingG->setEnabled(false);
        if (m_propChannelWeightingB)
            m_propChannelWeightingB->setEnabled(false);
        if (m_propAlphaThreshold)
            m_propAlphaThreshold->setEnabled(false);
        if (m_propAdaptiveColor)
            m_propAdaptiveColor->setEnabled(false);
        if (m_propUseAlpha)
            m_propUseAlpha->setEnabled(false);
        if (m_propNoAlpha)
            m_propNoAlpha->setEnabled(false);
        if (m_propBitrate)
            m_propBitrate->setEnabled(false);

        break;
    }

    if (m_propQuality)
        m_propQuality->setEnabled(compressedOptions);
    if (m_propChannelWeightingR)
        m_propChannelWeightingR->setEnabled(colorWeightOptions);
    if (m_propChannelWeightingG)
        m_propChannelWeightingG->setEnabled(colorWeightOptions);
    if (m_propChannelWeightingB)
        m_propChannelWeightingB->setEnabled(colorWeightOptions);
    if (m_propAlphaThreshold)
        m_propAlphaThreshold->setEnabled(alphaChannelOptions);
    if (m_propAdaptiveColor)
        m_propAdaptiveColor->setEnabled(colorWeightOptions);
    if (m_propUseAlpha)
        m_propUseAlpha->setEnabled(alphaChannelOptions);
    if (m_propNoAlpha)
        m_propNoAlpha->setEnabled(alphaChannelOptions);
    if (m_propBitrate)
        m_propBitrate->setEnabled(astcbitrateOptions);

    if (m_propDXT1Alpha)
    {
        m_propDXT1Alpha->setHidden(!alphaChannelOptions);
    }

    if (m_propASTCBlockRate)
    {
        m_propASTCBlockRate->setHidden(!astcbitrateOptions);
    }

    if (m_propChannelWeight)
    {
        m_propChannelWeight->setHidden(!colorWeightOptions);
    }
   
    int i = m_fileFormats->findText(extension);
    m_fileFormats->setCurrentIndex(i);

    if (compressedOptions)
        m_infotext->append("Destination file will be <b>Compressed</b> when processed");
    else
        m_infotext->append("Destination file will be <b>Transcoded</b> when processed");
}


//===================================================================
// Check if compression quality value changed
//===================================================================
void CSetCompressOptions::qualityValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Quality</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0 (Poor Quality) to 1 (High Quality) with default set at 0.05");
}

//===================================================================
// Check if red weight value changed
//===================================================================
void CSetCompressOptions::redwValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Red Channel Weighting</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0.01 (Lowest Weight) to 1 (Highest Weight) with default set at 0.3086");
}

//===================================================================
// Check if green weight value changed
//===================================================================
void CSetCompressOptions::greenwValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Green Channel Weighting</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0.01 (Lowest Weight) to 1 (Highest Weight) with default set at 0.6094");
}

//===================================================================
// Check if blue weight value changed
//===================================================================
void CSetCompressOptions::bluewValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Blue Channel Weighting</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0.01 (Lowest Weight) to 1 (Highest Weight) with default set at 0.0820");
}

//===================================================================
// Check if alpha threshold value changed
//===================================================================
void CSetCompressOptions::thresholdValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Alpha Threshold</b> Applies only to compressed formats (with alpha channel on)");
    m_infotext->append("Value range is 1-255");
}

//===================================================================
// Check if alpha is selected 
//===================================================================
void CSetCompressOptions::noAlphaChannelValue()
{
    if (m_propAlphaThreshold)
        m_propAlphaThreshold->setEnabled(false);
}

void CSetCompressOptions::hasAlphaChannelValue()
{
    if (m_propAlphaThreshold)
        m_propAlphaThreshold->setEnabled(true);
}

//===================================================================
// Check if compression is been specified 
//===================================================================
void CSetCompressOptions::bitrateValueChanged(QString &actualbitrate, int&xblock, int&yblock)
{
    QString msg="";
    QString blockmsg = "";
    if (xblock == -1 && yblock == -1)
    {
        msg = "Invalid input. Not supported.";
        blockmsg = "Value changed to default bit rate 8.00 (4x4).";
    }
    else
    {
        msg = "The <b>closet bit rate</b> is " + actualbitrate;
        blockmsg = "<b>Block number</b> is (XxY): " + QString::number(xblock) + "x" + QString::number(yblock);
    }
    
    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
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
    //m_data.m_settoUseOnlyBC6 = false;
    QFileInfo fi(m_data.m_sourceFileNamePath);
    QString ext = fi.suffix().toUpper();

    // Update Property Managed Settings and content view
    m_theController->setObject(&m_data, true, true);
    m_theController->update();

    // Destination FileName
    QString FileName;
    QFileInfo fileinfo;
    if (isInit)
    {
        fileinfo.setFile(m_data.m_destFileNamePath);
        m_DestinationFolder->setText(m_destFilePath);
        // Destination File Name
        FileName = fileinfo.baseName();
        if (FileName.length() <= 0) FileName = m_data.m_compname;
    }
    else
    {
        fileinfo.setFile(m_data.m_sourceFileNamePath);
        FileName = m_data.m_compname;
        // Destination Folder
        QDir dir(fileinfo.absoluteDir());
        QString DestFolder = dir.absolutePath();
        if ((DestFolder.length() <= 1) && DestFolder.contains("."))
        {
            DestFolder = "./Results";
        }
        m_DestinationFolder->setText(DestFolder);
        isInit = true;
    }

    //m_DestinationFile->setText(FileName);

    m_PBSaveSettings->setEnabled(true);

    if (m_propQuality)
    {
        m_propQuality->setToolTip(STR_QUALITY_SETTING_HINT);
        m_propQuality->setEnabled(true);
        // Set  Properties for editing
        QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propQuality->propertyManager();
        setMinMaxStep(Manager, m_propQuality, 0.0, 1.0, 0.05);
    }

    if (m_propFormat)
    {
        m_propFormat->setToolTip(STR_FORMAT_SETTING_HINT);
    }

    if (m_propChannelWeightingR)
    {
        m_propChannelWeightingR->setToolTip(STR_CHANNELWEIGHTR_SETTING_HINT);
        m_propChannelWeightingR->setEnabled(true);
        // Set  Properties for editing
        QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propChannelWeightingR->propertyManager();
        setMinMaxStep(Manager, m_propChannelWeightingR, 0.01, 1.0, 0.01);
    }

    if (m_propChannelWeightingG)
    {
        m_propChannelWeightingG->setToolTip(STR_CHANNELWEIGHTG_SETTING_HINT);
        m_propChannelWeightingG->setEnabled(true);
        // Set  Properties for editing
        QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propChannelWeightingG->propertyManager();
        setMinMaxStep(Manager, m_propChannelWeightingG, 0.01, 1.0, 0.01);
    }

    if (m_propChannelWeightingB)
    {
        m_propChannelWeightingB->setToolTip(STR_CHANNELWEIGHTB_SETTING_HINT);
        m_propChannelWeightingB->setEnabled(true);
        // Set  Properties for editing
        QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propChannelWeightingB->propertyManager();
        setMinMaxStep(Manager, m_propChannelWeightingB, 0.01, 1.0, 0.01);
    }

    if (m_propBitrate)
    {
        m_propBitrate->setToolTip(STR_BITRATE_SETTING_HINT);
        m_propBitrate->setEnabled(true);
        // Set  Properties for editing
        QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propBitrate->propertyManager();
        setMinMaxStep(Manager, m_propBitrate, 0.00, 8.00, 0.01);
    }

    if (m_propAlphaThreshold)
    {
        m_propAlphaThreshold->setToolTip(STR_ALPHATHRESHOLD_HINT);
        m_propAlphaThreshold->setEnabled(true);
        // Set  Properties for editing
        QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propAlphaThreshold->propertyManager();
        setMinMaxStep(Manager, m_propAlphaThreshold, 0, 255, 1);
    }

    if (m_propDestImage)
    {
        m_propDestImage->setHidden(true);
    }

    if (m_propDXT1Alpha)
    {
        m_propDXT1Alpha->setHidden(true);
    }

    if (m_propASTCBlockRate)
    {
        m_propASTCBlockRate->setHidden(true);
    }

    return true;
}

void CSetCompressOptions::setMinMaxStep(QtVariantPropertyManager* manager, QtProperty *m_prop, double min, double max, double step)
{
    if (manager)
    {
        QtVariantProperty *prop = manager->variantProperty(m_prop);
        prop->setAttribute(STR_SETTING_MINIMUM, min);
        prop->setAttribute(STR_SETTING_MAXIMUM, max);
        prop->setAttribute(STR_SETTING_SINGLESTEP, step);
    }
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

    QString ImageExt = m_fileFormats->currentText();

    // Path+FileName+Ext
    m_data.m_FileInfoDestinationName = m_data.m_compname + "." + ImageExt;

    // Path
    QString Path = m_DestinationFolder->text();
    QDir dir(Path);
    QString finalPath = dir.absolutePath();

    // Path+FileName
    finalPath.append("/");
    finalPath.append(m_data.m_compname);

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