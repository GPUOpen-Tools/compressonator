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
    m_isEditing_Compress_Options = false;
    m_PBSave->setEnabled(false);
    m_PBCancel->setEnabled(false);
    m_PBCompress->setEnabled(false);

    if (data == NULL)
    {
        m_theController->setObject(NULL, true, true);
        return;
    }

    QString m_currentClassName = data->metaObject()->className();
    bool controllerisNULL = false;


    if (m_currentClassName.compare(CLASS_COMPRESS) == 0)
    {
        // Set Editing Defaults 
        m_propQuality = m_theController->getProperty(COMPRESS_OPTIONS_QUALITY);
        if (m_propQuality == NULL)
        {
            m_theController->setObject(data, true, true);
            m_propQuality = m_theController->getProperty(COMPRESS_OPTIONS_QUALITY);
            controllerisNULL = true;
        }

        if (m_propQuality)
        {
            m_propQuality->setToolTip(STR_QUALITY_SETTING_HINT);
            m_propQuality->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propQuality->propertyManager();
            setMinMaxStep(Manager, m_propQuality, 0.0, 1.0, 0.05);
        }

        // Set Editing Defaults 
        m_propFormat = m_theController->getProperty(COMPRESS_OPTIONS_FORMAT);
        if (m_propFormat)
        {
            m_propFormat->setToolTip(STR_FORMAT_SETTING_HINT);
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propFormat->propertyManager();
            if (Manager)
            {
                // qDebug() << "Found"; // connect(Manager, SIGNAL(valueChanged(QtProperty *,QVariant &))
                // QtVariantProperty *prop = Manager->variantProperty(m_propFormat);
                // qDebug() << "Found Prop";
            }

        }

        // Set Editing Defaults for Channel Weight
        m_propChannelWeightingR = m_theController->getProperty(COMPRESS_OPTIONS_CHANNEL_WEIGHTING_R);
        if (m_propChannelWeightingR)
        {
            m_propChannelWeightingR->setToolTip(STR_CHANNELWEIGHTR_SETTING_HINT);
            m_propChannelWeightingR->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propChannelWeightingR->propertyManager();
            setMinMaxStep(Manager, m_propChannelWeightingR, 0.01, 1.0, 0.01);
        }

        m_propChannelWeightingG = m_theController->getProperty(COMPRESS_OPTIONS_CHANNEL_WEIGHTING_G);
        if (m_propChannelWeightingG)
        {
            m_propChannelWeightingG->setToolTip(STR_CHANNELWEIGHTG_SETTING_HINT);
            m_propChannelWeightingG->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propChannelWeightingG->propertyManager();
            setMinMaxStep(Manager, m_propChannelWeightingG, 0.01, 1.0, 0.01);
        }

        m_propChannelWeightingB = m_theController->getProperty(COMPRESS_OPTIONS_CHANNEL_WEIGHTING_B);
        if (m_propChannelWeightingB)
        {
            m_propChannelWeightingB->setToolTip(STR_CHANNELWEIGHTB_SETTING_HINT);
            m_propChannelWeightingB->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propChannelWeightingB->propertyManager();
            setMinMaxStep(Manager, m_propChannelWeightingB, 0.01, 1.0, 0.01);
        }

        // Set Editing Defaults 
        m_propAlphaThreshold = m_theController->getProperty(COMPRESS_OPTIONS_ALPHATHRESHOLD);
        if (m_propAlphaThreshold)
        {
            m_propAlphaThreshold->setToolTip(STR_ALPHATHRESHOLD_HINT);
            m_propAlphaThreshold->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propAlphaThreshold->propertyManager();
            setMinMaxStep(Manager, m_propAlphaThreshold, 0, 255, 1);
        }

        // Set Editing Defaults 
        m_propBitrate = m_theController->getProperty(COMPRESS_OPTIONS_BITRATE);
        if (m_propBitrate)
        {
            m_propBitrate->setToolTip(STR_BITRATE_SETTING_HINT);
            m_propBitrate->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propBitrate->propertyManager();
            setMinMaxStep(Manager, m_propBitrate, 0.00, 8.00, 0.01);
        }

        // Set Defog Editing Defaults 
        m_propDefog = m_theController->getProperty(COMPRESS_OPTIONS_DEFOG);
        if (m_propDefog)
        {
            m_propDefog->setToolTip(STR_DEFOG_SETTING_HINT);
            m_propDefog->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propDefog->propertyManager();
            setMinMaxStep(Manager, m_propDefog, 0.000, 0.010, 0.001);
        }

        m_propExposure = m_theController->getProperty(COMPRESS_OPTIONS_EXPOSURE);
        if (m_propExposure)
        {
            m_propExposure->setToolTip(STR_EXPOSURE_SETTING_HINT);
            m_propExposure->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propExposure->propertyManager();
            setMinMaxStep(Manager, m_propExposure, -10.000, 10.000, 0.125);
        }

        m_propKneeLow = m_theController->getProperty(COMPRESS_OPTIONS_KNEELOW);
        if (m_propKneeLow)
        {
            m_propKneeLow->setToolTip(STR_KNEELOW_SETTING_HINT);
            m_propKneeLow->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propKneeLow->propertyManager();
            setMinMaxStep(Manager, m_propKneeLow, -3.00, 3.00, 0.125);
        }

        m_propKneeHigh = m_theController->getProperty(COMPRESS_OPTIONS_KNEEHIGH);
        if (m_propKneeHigh)
        {
            m_propKneeHigh->setToolTip(STR_KNEEHIGH_SETTING_HINT);
            m_propKneeHigh->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propKneeHigh->propertyManager();
            setMinMaxStep(Manager, m_propKneeHigh, 3.50, 7.50, 0.125);
        }

        m_propGamma = m_theController->getProperty(COMPRESS_OPTIONS_GAMMA);
        if (m_propGamma)
        {
            m_propGamma->setToolTip(STR_GAMMA_SETTING_HINT);
            m_propGamma->setEnabled(true);
            // Set  Properties for editing
            QtVariantPropertyManager *Manager = (QtVariantPropertyManager *)m_propGamma->propertyManager();
            setMinMaxStep(Manager, m_propGamma, 1.0, 2.6, 0.2);
        }

        m_propAdaptiveColor = m_theController->getProperty(COMPRESS_OPTIONS_ADAPTIVECOLOR);
        m_propUseAlpha = m_theController->getProperty(COMPRESS_OPTIONS_USEALPHA);
        m_propNoAlpha = m_theController->getProperty(COMPRESS_OPTIONS_NOALPHA);

        m_propChannelWeight = m_theController->getProperty(CHANNEL_WEIGHTING_CLASS_NAME);
        m_propDXT1Alpha = m_theController->getProperty(DXT1_ALPHA_CLASS_NAME);
        m_propASTCBlockRate = m_theController->getProperty(ASTC_BLOCKRATE_CLASS_NAME);
        m_propHDRProperties = m_theController->getProperty(HDR_PROP_CLASS_NAME);

        m_holddata = (C_Destination_Options *)data;
        *m_C_Destination_Options << (const C_Destination_Options &)*data;
        m_data = m_C_Destination_Options;
        m_isEditing_Compress_Options = true;
        m_PBCompress->setEnabled(true);
        m_PBCompress->setText(COMPRESS_TEXT);

        if (controllerisNULL)
        {
            compressionValueChanged((QVariant &)(m_C_Destination_Options->m_Compression));
            controllerisNULL = false;
        }

        //========================
        // Monitor changes in value
        //========================
        connect(m_data, SIGNAL(compressionChanged(QVariant &)), this, SLOT(compressionValueChanged(QVariant &)));
        connect(m_data, SIGNAL(qualityChanged(QVariant &)), this, SLOT(qualityValueChanged(QVariant &)));
        connect(m_data, SIGNAL(redwChanged(QVariant &)), this, SLOT(redwValueChanged(QVariant &)));
        connect(m_data, SIGNAL(greenwChanged(QVariant &)), this, SLOT(greenwValueChanged(QVariant &)));
        connect(m_data, SIGNAL(bluewChanged(QVariant &)), this, SLOT(bluewValueChanged(QVariant &)));
        connect(m_data, SIGNAL(thresholdChanged(QVariant &)), this, SLOT(thresholdValueChanged(QVariant &)));
        connect(m_data, SIGNAL(noAlphaChannel()), this, SLOT(noAlphaChannelValue()));
        connect(m_data, SIGNAL(hasAlphaChannel()), this, SLOT(hasAlphaChannelValue()));
        connect(m_data, SIGNAL(bitrateChanged( QString &, int&, int&)), this, SLOT(bitrateValueChanged(QString &, int&, int&)));

        connect(m_data, SIGNAL(defogChanged(double&)), this, SLOT(defogValueChanged(double&)));
        connect(m_data, SIGNAL(exposureChanged(double&)), this, SLOT(exposureValueChanged(double&)));
        connect(m_data, SIGNAL(kneeLowChanged(double&)), this, SLOT(kneelowValueChanged(double&)));
        connect(m_data, SIGNAL(kneeHighChanged(double&)), this, SLOT(kneehighValueChanged(double&)));
        connect(m_data, SIGNAL(gammaChanged(double&)), this, SLOT(gammaValueChanged(double&)));
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

void CImagePropertyView::noAlphaChannelValue()
{
    if(m_propAlphaThreshold)
        m_propAlphaThreshold->setEnabled(false);
}

void CImagePropertyView::hasAlphaChannelValue()
{
    if (m_propAlphaThreshold)
        m_propAlphaThreshold->setEnabled(true);
}

//===================================================================
// Check if compression is been specified 
//===================================================================
void CImagePropertyView::compressionValueChanged(QVariant &value)
{
    bool compressedOptions = false;
    bool colorWeightOptions = false;
    bool alphaChannelOptions = false;
    bool astcbitrateOptions = false;
    bool hdrOptions = false;
    C_Destination_Options *Data = (C_Destination_Options *)m_data;
    
    C_Destination_Options::eCompression comp = (C_Destination_Options::eCompression &)value;

    switch (comp)
    {
    case C_Destination_Options::BC6H:
        compressedOptions   = true;
        colorWeightOptions  = false;
        alphaChannelOptions = false;
        astcbitrateOptions  = false;
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("Block Compression (BC) format designed to support high-dynamic range (floating point) color spaces. (e.g. .exr extension image file).");
        break;
    case C_Destination_Options::BC6H_SF:
        compressedOptions = true;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("Block Compression (BC) format designed to support high-dynamic range (signed floating point) color spaces. (e.g. .exr extension image file).");
        break;
    case C_Destination_Options::BC1:
    case C_Destination_Options::DXT1:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = true;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A four component opaque (or 1-bit alpha) compressed texture format for Microsoft DirectX10. DXT1 identical to BC1.  Four bits per pixel.");
        break;
    case C_Destination_Options::BC2:
    case C_Destination_Options::DXT3:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A four component compressed texture format with explicit alpha for Microsoft DirectX10. DXT3 identical to BC2. Eight bits per pixel.");
        break;
    case C_Destination_Options::BC3:
    case C_Destination_Options::DXT5:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A four component compressed texture format with interpolated alpha for Microsoft DirectX10. DXT5 identical to BC3. Eight bits per pixel.");
        break;
    case C_Destination_Options::BC4:
        compressedOptions = true;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
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
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A two component compressed texture format for Microsoft DirectX10. BC5 identical to ATI2N. Eight bits per pixel.");
        break;
    case C_Destination_Options::ASTC:
        compressedOptions = true;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = true;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("ASTC (Adaptive Scalable Texture Compression),lossy block-based texture compression developed with ARM.");
        astcbitrateOptions = true;
        break;
    case C_Destination_Options::BC7:
        compressedOptions   = true;
        colorWeightOptions  = false;
        alphaChannelOptions = false;
        astcbitrateOptions  = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("The latest block Compression (BC) format designed to support high-quality compression of RGB and RGBA bytes color spaces.");
        break;
    case C_Destination_Options::ATC_RGB:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A compressed RGB format.");
        break;
    case C_Destination_Options::ATC_RGBA_Explicit:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A compressed ARGB format with explicit alpha.");
        break;
    case C_Destination_Options::ATC_RGBA_Interpolated:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A compressed ARGB format with interpolated alpha.");
        break;
    case C_Destination_Options::DXT5_xGBR:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(" DXT5 with the red component swizzled into the alpha channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_RxBG:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("swizzled DXT5 format with the green component swizzled into the alpha channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_RBxG:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(" swizzled DXT5 format with the green component swizzled into the alpha channel & the blue component swizzled into the green channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_xRBG:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("swizzled DXT5 format with the green component swizzled into the alpha channel & the red component swizzled into the green channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_RGxB:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("swizzled DXT5 format with the blue component swizzled into the alpha channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_xGxR:
        compressedOptions = true;
        colorWeightOptions = true;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("two-component swizzled DXT5 format with the red component swizzled into the alpha channel & the green component in the green channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::ETC2_RGB:
    case C_Destination_Options::ETC_RGB:
        compressedOptions = true;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        if (Data) {
            if (Data->m_SourceIsFloatFormat) {
                hdrOptions = true;
            }
        }
        m_infotext->clear();
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("ETC (Ericsson Texture Compression, lossy texture compression developed with Ericsson Research.)");
        break;
    default:
        compressedOptions = false;
        colorWeightOptions = false;
        alphaChannelOptions = false;
        astcbitrateOptions = false;
        hdrOptions = false;
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
    if (m_propDefog)
        m_propDefog->setEnabled(hdrOptions);
    if (m_propExposure)
        m_propExposure->setEnabled(hdrOptions);
    if (m_propKneeLow)
        m_propKneeLow->setEnabled(hdrOptions);
    if (m_propKneeHigh)
        m_propKneeHigh->setEnabled(hdrOptions);
    if (m_propGamma)
        m_propGamma->setEnabled(hdrOptions);

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

    if (m_propHDRProperties)
    {
        m_propHDRProperties->setHidden(!hdrOptions);
    }

    if (compressedOptions)
    {
        m_infotext->append("Destination file will be <b>Compressed</b> when processed");
    }
    else
    {
        m_infotext->clear();
        m_infotext->append("Destination file will be <b>Transcoded</b> when processed");
    }

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

//===================================================================
// Check if compression is been specified 
//===================================================================
void CImagePropertyView::redwValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Red Channel Weighting</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0.01 (Lowest Weight) to 1 (Highest Weight) with default set at 0.3086");
}

//===================================================================
// Check if compression is been specified 
//===================================================================
void CImagePropertyView::greenwValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Green Channel Weighting</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0.01 (Lowest Weight) to 1 (Highest Weight) with default set at 0.6094");
}

//===================================================================
// Check if compression is been specified 
//===================================================================
void CImagePropertyView::bluewValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Blue Channel Weighting</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0.01 (Lowest Weight) to 1 (Highest Weight) with default set at 0.0820");
}

//===================================================================
// Check if compression is been specified 
//===================================================================
void CImagePropertyView::thresholdValueChanged(QVariant &value)
{
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Alpha Threshold</b> Applies only to compressed formats (with alpha channel on)");
    m_infotext->append("Value range is 1-255");
}

//===================================================================
// Check if compression is been specified 
//===================================================================
void CImagePropertyView::bitrateValueChanged(QString &actualbitrate, int&xblock, int&yblock)
{
    QString msg = "";
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

//===================================================================
// Defog value changed
//===================================================================
void CImagePropertyView::defogValueChanged(double& defog)
{
    QString msg = "";
    QString blockmsg = "";
    if (defog < 0.00 || defog > 0.01)
    {
        msg = "Invalid input. Not supported. Clamp to valid range";
        blockmsg = "Defog value should be in range of 0.0000 to 0.0100.";
    }

    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
}

//===================================================================
//exposure value changed
//===================================================================
void CImagePropertyView::exposureValueChanged(double& exposure)
{
    QString msg = "";
    QString blockmsg = "";
    if (exposure < -10.0 || exposure > 10.0)
    {
        msg = "Invalid input. Not supported. Clamp to valid range";
        blockmsg = "Exposure value supported is in range of -10.0 to 10.0.";
    }

    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
}

//===================================================================
// kneelow value changed
//===================================================================
void CImagePropertyView::kneelowValueChanged(double& kl)
{
    QString msg = "";
    QString blockmsg = "";
    if (kl < -3.00 || kl > 3.00)
    {
        msg = "Invalid input. Not supported. Clamp to valid range";
        blockmsg = "Knee Low value should be in range of -3.0 to 3.0.";
    }

    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
}

//===================================================================
// knee high value changed
//===================================================================
void CImagePropertyView::kneehighValueChanged(double& kh)
{
    QString msg = "";
    QString blockmsg = "";
    if (kh < 3.5 || kh > 7.5)
    {
        msg = "Invalid input. Not supported. Clamp to valid range";
        blockmsg = "Knee High value supported is in range of 3.5 to 7.5.";
    }

    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
}

//===================================================================
// gamma value changed
//===================================================================
void CImagePropertyView::gammaValueChanged(double& gamma)
{
    QString msg = "";
    QString blockmsg = "";
    if (gamma < 1.0 || gamma > 2.6)
    {
        msg = "Invalid input. Not supported. Clamp to valid range";
        blockmsg = "Gamma value supported is in range of 1.0 to 2.6.";
    }

    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
}


// -----------------------------------------------------------
// This call may be called too oftern for the same data 
// Optimize its calls in final production
// -----------------------------------------------------------

void CImagePropertyView::refreshView()
{
    m_theController->setObject(m_data, true, true);
}

void CImagePropertyView::setMinMaxStep(QtVariantPropertyManager* manager, QtProperty *m_prop, double min, double max, double step)
{
    if (manager)
    {
        QtVariantProperty *prop = manager->variantProperty(m_prop);
        prop->setAttribute(STR_SETTING_MINIMUM, min);
        prop->setAttribute(STR_SETTING_MAXIMUM, max);
        prop->setAttribute(STR_SETTING_SINGLESTEP, step);
    }
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