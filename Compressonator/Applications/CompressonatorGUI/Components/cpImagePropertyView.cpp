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

#include "cpImagePropertyView.h"

#define COMPRESS_TEXT       "     Process     "
#define SAVECOMP_TEXT       " Save && Process "
#define CLASS_COMPRESS      "C_Destination_Options"

static int msgcnt = 0;

CImagePropertyView::CImagePropertyView(const QString title, QWidget *parent) : 
    QDockWidget(parent)
    // acCustomDockWidget(title, parent)
{
    // if (custTitleBar)
    // {
    //     custTitleBar->setTitle(title);
    //     custTitleBar->setButtonCloseEnabled(false);
    //     custTitleBar->setButtonToolBarEnabled(false);
    // }
    
    setWindowTitle(title);
  
    m_browser = NULL;

    m_newWidget   = new QWidget(parent);
    m_layoutV     = new QVBoxLayout(this);

    //=======================
    // Object Viewer & Editor
    //=======================
    m_data = NULL;
    m_C_Destination_Options = new C_Destination_Options();

    // Create a Object Controller and use a temp data class to populate a view
    m_theController = new ObjectController(this, true, false);
    m_browser = m_theController->getTreeBrowser();
    if (m_browser)
    {
        m_browser->setHeaderVisible(false);
        m_browser->setResizeMode(QtTreePropertyBrowser::ResizeToContents); // follow this comment Note#1
        connect(m_browser, SIGNAL(currentItemChanged(QtBrowserItem *)), this, SLOT(oncurrentItemChanged(QtBrowserItem *)));
    }
    m_layoutV->addWidget(m_theController);

    //Spacer
    QWidget* spacerV = new QWidget(this);
    spacerV->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    //=================================
    // Text View for help and Hints
    //=================================

    m_infotext= new QTextBrowser(this);
    // Always show min Two lines of text and Max to 5 lines at font size 16
    m_infotext->setMinimumHeight(32);
    m_infotext->setMaximumHeight(96);
    m_infotext->setReadOnly(true);
    m_infotext->setAcceptRichText(true);
    m_layoutV->addWidget(m_infotext);

    //====================
    // Horizontal Buttons
    //=====================
    m_layoutHButtons = new QHBoxLayout(this);
    m_PBSave        = new QPushButton("Save",this);
    m_PBSave->setToolTip("Save changes, this will delete the current image file and its settings");
    m_PBCompress = new QPushButton(COMPRESS_TEXT, this);
    m_PBCompress->setToolTip("Process the image with the current displayed settings");
    m_PBCancel = new QPushButton("Revert", this);
    m_PBCancel->setToolTip("Restores the old settings, original image file is not restored from prior changes!");

    m_PBSave->setEnabled(false);
    m_PBCancel->setEnabled(false);
    m_PBCompress->setEnabled(false);

    connect(m_PBCancel,     SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(m_PBSave,       SIGNAL(clicked()), this, SLOT(onSave()));
    connect(m_PBCompress,   SIGNAL(clicked()), this, SLOT(onCompress()));


    //====================
    // Layout on Widget
    //=====================
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_layoutHButtons->addWidget(spacer);
    m_layoutHButtons->addWidget(m_PBSave);
    m_layoutHButtons->addWidget(m_PBCompress);
    m_layoutHButtons->addWidget(m_PBCancel);
    m_layoutV->addLayout(m_layoutHButtons);

    m_newWidget->setLayout(m_layoutV);
    setWidget(m_newWidget);
}

// -----------------------------------------------------------
// Signaled when items focus has changed on th property view
// ----------------------------------------
void CImagePropertyView::oncurrentItemChanged(QtBrowserItem *item)
{
    if (!item) return;
    m_infotext->clear();

    if (!m_isEditing_Compress_Options) return;

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
        m_infotext->append("Sets destinations image quality");
        m_infotext->append("For low values quality will be poor and the time to process the image will be short.");
        m_infotext->append("Subsequently higher values will increase the quality and processing time");
    }
}



// -----------------------------------------------------------
// This call may be called too oftern for the same data 
// Optimize its calls in final production
// -----------------------------------------------------------

void CImagePropertyView::OnUpdateData(QObject *data)
{

    if (data == NULL)
    {
        m_theController->setObject(NULL, true, true);
        return;
    }

    QString m_currentClassName = data->metaObject()->className();
    
    m_isEditing_Compress_Options = false;

    m_PBSave->setEnabled(false);
    m_PBCancel->setEnabled(false);
    m_PBCompress->setEnabled(false);

    if (m_currentClassName.compare(CLASS_COMPRESS) == 0)
    {
        m_holddata = (C_Destination_Options *)data;
        *m_C_Destination_Options << (const C_Destination_Options &)*data;
        m_data = m_C_Destination_Options;
        m_isEditing_Compress_Options = true;
        m_PBCompress->setEnabled(true);
        m_PBCompress->setText(COMPRESS_TEXT);

        //========================
        // Monitor changes in value
        //========================
        connect(m_data, SIGNAL(compressionChanged(QVariant &)), this, SLOT(compressionValueChanged(QVariant &)));
        connect(m_data, SIGNAL(qualityChanged(QVariant &)), this, SLOT(qualityValueChanged(QVariant &)));

        // Set Editing Defaults 
        m_propQuality = m_theController->getProperty(COMPRESS_OPTIONS_QUALITY);
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

        // Set Editing Defaults 
        m_propFormat = m_theController->getProperty(COMPRESS_OPTIONS_FORMAT);
        if (m_propFormat)
        {
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propFormat->propertyManager();
            if (Manager)
            {
               // qDebug() << "Found"; // connect(Manager, SIGNAL(valueChanged(QtProperty *,QVariant &))
               // QtVariantProperty *prop = Manager->variantProperty(m_propFormat);
               // qDebug() << "Found Prop";
            }

        }

    }
    else
    {
        msgcnt = 0;
        m_data = data;
    }

    m_theController->setObject(m_data, true, true);

    // This should always be calld after the setObject
    // since data is alway changed on first display
    if (m_isEditing_Compress_Options)
    {
        connect(m_C_Destination_Options, SIGNAL(dataChanged()), this, SLOT(onCompressDataChanged()));
        m_C_Destination_Options->m_data_has_been_changed = false;
    }

    m_infotext->clear();

}



//===================================================================
// Check if compression is been specified 
//===================================================================
void CImagePropertyView::compressionValueChanged(QVariant &value)
{
    QMessageBox msgBox;
    bool compressedOptions = false;
    C_Destination_Options *Data = (C_Destination_Options *)m_data;

    C_Destination_Options::eCompression comp = (C_Destination_Options::eCompression &)value;

    m_infotext->clear();
    m_infotext->append("<b>Format</b>");

    switch (comp)
    {
    case C_Destination_Options::BC6H:
        if (Data)
        {
            // if (!Data->m_settoUseOnlyBC6)
            // {
            //     if (comp == C_Destination_Options::BC6H)
            //     {
            //         Data->m_Compression = C_Destination_Options::BC7;
            //         // Avoid displaying the message if thedata gets refreshed multiple times
            //         if (msgcnt == 0)
            //         {
            //             msgBox.setText("BC6H compression is supported only for HDR image file formats\nSetting will be reset to BC7");
            //             msgBox.exec();
            //             msgcnt++;
            //             if (m_browser)
            //                 m_browser->update();
            //         }
            //     }
            // }
        }
        compressedOptions = true;
        break;
    case C_Destination_Options::BC1:
    case C_Destination_Options::BC2:
    case C_Destination_Options::BC3:
    case C_Destination_Options::BC4:
    case C_Destination_Options::BC5:
    case C_Destination_Options::BC7:
    // Enable when Qualified
    // case C_Destination_Options::ASTC:
    case C_Destination_Options::ATC_RGB:
    case C_Destination_Options::ATC_RGBA_Explicit:
    case C_Destination_Options::ATC_RGBA_Interpolated:
    case C_Destination_Options::ATI2N:
    case C_Destination_Options::ATI2N_XY:
    case C_Destination_Options::ATI2N_DXT5:
    case C_Destination_Options::DXT3:
    case C_Destination_Options::DXT5:
    case C_Destination_Options::DXT5_xGBR:
    case C_Destination_Options::DXT5_RxBG:
    case C_Destination_Options::DXT5_RBxG:
    case C_Destination_Options::DXT5_xRBG:
    case C_Destination_Options::DXT5_RGxB:
    case C_Destination_Options::DXT5_xGxR:
    case C_Destination_Options::ETC_RGB:
        compressedOptions = true;
        break;
    default:
        compressedOptions = false;
        break;
    }

    if (m_propQuality)
        m_propQuality->setEnabled(compressedOptions);

    if (Data)
    {
        if (Data->m_settoUseOnlyBC6)
        {
            if ((comp != C_Destination_Options::BC6H) && (compressedOptions))
            {
                Data->m_Compression = C_Destination_Options::BC6H;
            }
        }
    }


    if (compressedOptions)
        m_infotext->append("Destination file will be <b>Compressed</b> when processed");
    else
        m_infotext->append("Destination file will be <b>Transcoded</b> when processed");

}


//===================================================================
// Check if compression is been specified 
//===================================================================
void CImagePropertyView::qualityValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Quality</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0 (Poor Quality) to 1 (High Quality) with default set at 0.05");
}


// -----------------------------------------------------------
// This call may be called too oftern for the same data 
// Optimize its calls in final production
// -----------------------------------------------------------

void CImagePropertyView::refreshView()
{
    m_theController->setObject(m_data, true, true);
}


void CImagePropertyView::onCompressDataChanged()
{
    if (m_C_Destination_Options->m_data_has_been_changed)
    {
        m_PBSave->setEnabled(true);
        m_PBCancel->setEnabled(true);
        m_PBCompress->setText(SAVECOMP_TEXT);
    }
}


void CImagePropertyView::closeEvent(QCloseEvent * event)
{
    hide();
    event->ignore();
}

void CImagePropertyView::onCancel()
{
    *m_C_Destination_Options << (const C_Destination_Options &)*m_holddata;
    refreshView();
    m_PBSave->setEnabled(false);
    m_PBCancel->setEnabled(false);
    m_PBCompress->setText(COMPRESS_TEXT);

}

void CImagePropertyView::onSave()
{
    if (m_isEditing_Compress_Options)
    {
        m_PBSave->setEnabled(false);
        m_PBCancel->setEnabled(false);
        m_PBCompress->setText(COMPRESS_TEXT);
        *m_holddata << *m_C_Destination_Options;
        emit saveSetting(&m_C_Destination_Options->m_destFileNamePath);
        msgcnt = 0;
    }
}

void CImagePropertyView::onCompress()
{
    if (m_isEditing_Compress_Options)
    {
        *m_holddata << *m_C_Destination_Options;
        msgcnt = 0;

        m_PBSave->setEnabled(false);
        m_PBCancel->setEnabled(false);
        m_PBCompress->setText(COMPRESS_TEXT);
        m_PBCompress->setEnabled(false);

        emit compressImage(&m_C_Destination_Options->m_destFileNamePath);
    }
}

void CImagePropertyView::onCompressionStart()
{
        m_PBCompress->setEnabled(false);
}

void CImagePropertyView::onCompressionDone()
{
        m_PBCompress->setEnabled(true);
}


CImagePropertyView::~CImagePropertyView()
{
    if (m_C_Destination_Options)
    {
        delete m_C_Destination_Options;
        m_C_Destination_Options = NULL;
    }
}



void CImagePropertyView::onImageLoadStart()
{
    m_PBCompress->setEnabled(false);
}

void CImagePropertyView::onImageLoadDone()
{
    m_PBCompress->setEnabled(true);
}


void CImagePropertyView::onSourceImage(int childCount)
{
    Q_UNUSED(childCount);
   //reserved for future feature -> m_PBCompress->setEnabled(childCount> 1);
}