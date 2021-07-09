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

#include "cpimagepropertyview.h"

#define COMPRESS_TEXT "     Process     "
#define SAVECOMP_TEXT " Save && Process "
#define CLASS_COMPRESS "C_Destination_Options"

static int msgcnt = 0;

CImagePropertyView::CImagePropertyView(const QString title, QWidget* parent)
    : QDockWidget(parent) {
    setWindowTitle(title);

    m_browser   = NULL;
    m_newWidget = new QWidget(parent);
    m_layoutV   = new QVBoxLayout();

    //=======================
    // Object Viewer & Editor
    //=======================
    m_holddata              = NULL;
    m_data                  = NULL;
    m_C_Destination_Options = new C_Destination_Options();

    // Create a Object Controller and use a temp data class to populate a view
    m_theController = new ObjectController(this, true, false);
    m_browser       = m_theController->getTreeBrowser();
    if (m_browser) {
        m_browser->setHeaderVisible(false);
        m_browser->setResizeMode(QtTreePropertyBrowser::ResizeToContents);  // follow this comment Note#1
        connect(m_browser, SIGNAL(currentItemChanged(QtBrowserItem*)), this, SLOT(oncurrentItemChanged(QtBrowserItem*)));
    }

    m_layoutV->addWidget(m_theController);
    m_C_Destination_Options->m_controller = m_theController;

    //Spacer
    QWidget* spacerV = new QWidget(this);
    spacerV->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    //=================================
    // Text View for help and Hints
    //=================================

    m_infotext = new QTextBrowser(this);
    // Always show min Two lines of text and Max to 5 lines at font size 16
    m_infotext->setMinimumHeight(32);
    m_infotext->setMaximumHeight(96);
    m_infotext->setReadOnly(true);
    m_infotext->setAcceptRichText(true);
    m_layoutV->addWidget(m_infotext);

    //====================
    // Horizontal Buttons
    //=====================
    m_layoutHButtons = new QHBoxLayout();
    m_PBSave         = new QPushButton("Save", this);
    m_PBSave->setToolTip("Save changes, this will delete the current image file and its settings");
    m_PBCompress = new QPushButton(COMPRESS_TEXT, this);
    m_PBCompress->setToolTip("Process the image with the current displayed settings");
    m_PBCancel = new QPushButton("Revert", this);
    m_PBCancel->setToolTip("Restores the old settings, original image file is not restored from prior changes!");

    m_PBSave->setEnabled(false);
    m_PBCancel->setEnabled(false);
    m_PBCompress->setEnabled(false);

    connect(m_PBCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(m_PBSave, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(m_PBCompress, SIGNAL(clicked()), this, SLOT(onCompress()));

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
void CImagePropertyView::oncurrentItemChanged(QtBrowserItem* item) {
    if (!item)
        return;
    m_infotext->clear();

    if (!m_isEditing_Compress_Options)
        return;

    QtProperty* treeItem = item->property();
    QString     text;
    text = treeItem->propertyName();
    m_infotext->append("<b>" + text + "</b>");

    if (text.compare(COMPRESS_OPTIONS_FORMAT) == 0) {
        m_infotext->append("Sets destination image format");
    } else if (text.compare(COMPRESS_OPTIONS_QUALITY) == 0) {
        m_infotext->append("Sets destinations image quality");
        m_infotext->append("For low values quality will be poor and the time to process the image will be short.");
        m_infotext->append("Subsequently higher values will increase the quality and processing time");
    }
    else if (text.compare(COMPRESS_OPTIONS_REFINE_STEPS) == 0) {
        m_infotext->append("Sets added steps to increase image quality");
        m_infotext->append("Set 1 for low-frequency images, these images typically have solid or gradient colors per block");
        m_infotext->append("Set 2 for processing mixed low + high-frequency images, these images typically contain edges or mixed colors per block");
    }
    else if (text.compare(COMPRESS_OPTIONS_ALPHATHRESHOLD) == 0) {
        m_infotext->append("Punch through alpha for BC1");
        m_infotext->append("To enable set a value > 0");
        m_infotext->append("Blocks with alpha below the threshold will be set transparent");
    }
#ifdef USE_ENABLEHQ
    else if (text.compare(COMPRESS_OPTIONS_HIGHQUALITY) == 0) {
        m_infotext->append("Enables high-quality encoding to improve overall image quality");
        m_infotext->append("This will option will considerably increase processing time");
    }
#endif
}

void CImagePropertyView::Init_C_Destiniation_Data_Controller() {
    if (m_theController == NULL) {
        return;
    }

    // Inheritance Map Class layout starts at C_Destination_Options
    // Enable Format and Quality only for Images
    m_propFormat = m_theController->getProperty(COMPRESS_OPTIONS_FORMAT);
    if (m_propFormat)
        m_propFormat->setHidden(true);

    m_propQuality = m_theController->getProperty(COMPRESS_OPTIONS_QUALITY);
    if (m_propQuality) {
        m_propQuality->setHidden(true);
    }
#ifdef USE_ENABLEHQ
    m_propEnableHQ = m_theController->getProperty(COMPRESS_OPTIONS_HIGHQUALITY);
    if (m_propEnableHQ) {
        m_propEnableHQ->setHidden(true);
    }
#endif
    // C_Destination_Image - always enabled common to both Mesh and Image Data
    m_propWidth     = m_theController->getProperty(COMPRESS_OPTIONS_WIDTH);
    m_propHeight    = m_theController->getProperty(COMPRESS_OPTIONS_HEIGHT);
    m_propCompRatio = m_theController->getProperty(COMPRESS_OPTIONS_COMP_RATIO);
    m_propCompTime  = m_theController->getProperty(COMPRESS_OPTIONS_COMP_TIME);

    // Mesh_Optimizer_Settings - default hidden
    m_propMeshOptimizerSettings = m_theController->getProperty(MESH_OPTIMIZER_SETTING_CLASS_NAME);
    if (m_propMeshOptimizerSettings) {
        m_propMeshOptimizerSettings->setHidden(true);
    }

    // Mesh_Compression_Settings - default hidden
    m_propMeshCompressionSettings = m_theController->getProperty(MESH_COMPRESSION_SETTINGS_CLASS_NAME);
    if (m_propMeshCompressionSettings) {
        m_propMeshCompressionSettings->setHidden(true);
    }

    // Channel_Weighting - default hidden
    m_propChannelWeight = m_theController->getProperty(CHANNEL_WEIGHTING_CLASS_NAME);
    if (m_propChannelWeight) {
        m_propChannelWeight->setHidden(true);
        m_propChannelWeightingR = m_theController->getProperty(COMPRESS_OPTIONS_CHANNEL_WEIGHTING_R);
        m_propChannelWeightingG = m_theController->getProperty(COMPRESS_OPTIONS_CHANNEL_WEIGHTING_G);
        m_propChannelWeightingB = m_theController->getProperty(COMPRESS_OPTIONS_CHANNEL_WEIGHTING_B);
    }

    // DXT1_Alpha - default hidden
    m_propDXT1Alpha = m_theController->getProperty(DXT1_ALPHA_CLASS_NAME);
    if (m_propDXT1Alpha) {
        m_propDXT1Alpha->setHidden(true);
        m_propAlphaThreshold = m_theController->getProperty(COMPRESS_OPTIONS_ALPHATHRESHOLD);
        m_propAdaptiveColor  = m_theController->getProperty(COMPRESS_OPTIONS_ADAPTIVECOLOR);
        m_propBitrate        = m_theController->getProperty(COMPRESS_OPTIONS_BITRATE);
    }

    // C_CODEC_BlockRate - default hidden
    m_propCodecBlockRate = m_theController->getProperty(CODEC_BLOCK_CLASS_NAME);
    if (m_propCodecBlockRate) {
        m_propCodecBlockRate->setHidden(true);
    }

    // C_Refine - default hidden : Enabled for BC1 
    m_propRefine = m_theController->getProperty(REFINE_CLASS_NAME);
    if (m_propRefine) {
        m_propRefine->setHidden(true);
    }

    // C_Input_HDR_Image_Properties - default hidden
    m_propHDRProperties = m_theController->getProperty(HDR_PROP_CLASS_NAME);
    if (m_propHDRProperties) {
        m_propHDRProperties->setHidden(true);
        m_propDefog    = m_theController->getProperty(COMPRESS_OPTIONS_DEFOG);
        m_propExposure = m_theController->getProperty(COMPRESS_OPTIONS_EXPOSURE);
        m_propKneeLow  = m_theController->getProperty(COMPRESS_OPTIONS_KNEELOW);
        m_propKneeHigh = m_theController->getProperty(COMPRESS_OPTIONS_KNEEHIGH);
        m_propGamma    = m_theController->getProperty(COMPRESS_OPTIONS_GAMMA);
    }
}

// -----------------------------------------------------------
// This call may be called too oftern for the same data
// Optimize its calls in final production
// -----------------------------------------------------------

void CImagePropertyView::OnUpdateData(QObject* data) {
    m_isEditing_Compress_Options = false;
    m_PBSave->setEnabled(false);
    m_PBCancel->setEnabled(false);
    m_PBCompress->setEnabled(false);

    if (m_theController == NULL)
        return;

    if (data == NULL) {
        m_theController->setObject(NULL, true, true);
        return;
    }

    QString m_currentClassName = data->metaObject()->className();
    bool    controllerisNULL   = false;

    if (m_currentClassName.compare(CLASS_COMPRESS) == 0) {
        C_Destination_Options* DestinationOptions = reinterpret_cast<C_Destination_Options*>(data);

        m_holddata = (C_Destination_Options*)data;
        *m_C_Destination_Options << (const C_Destination_Options&)*data;
        m_data = m_C_Destination_Options;
        m_theController->setObject(m_data, true, true);

        m_holddata->setController(m_theController);
        Init_C_Destiniation_Data_Controller();

        //========================
        // Model Data Settings
        //========================
        if (DestinationOptions->m_isModelData) {
            // Hide Some unrelavent Settings
            if (m_propWidth)
                m_propWidth->setHidden(true);
            if (m_propHeight)
                m_propHeight->setHidden(true);
            if (m_propCompRatio)
                m_propCompRatio->setHidden(true);
            if (m_propCompTime)
                m_propCompTime->setHidden(true);

            if (m_propMeshOptimizerSettings) {
                m_propMeshOptimizerSettings->setHidden(false);
                m_holddata->disable_mesh_optimization_setting(DestinationOptions->m_Do_Mesh_Optimization != C_Destination_Options::eMeshOptimization::UserOpt);
                connect(m_data, SIGNAL(onMesh_Optimization(QVariant&)), this, SLOT(onMesh_Optimization(QVariant&)));
            }

            if (m_propMeshCompressionSettings) {
                QFileInfo fi(DestinationOptions->m_modelSource);
                QString   m_modelext = fi.suffix().toUpper();
                m_propMeshCompressionSettings->setHidden(m_modelext.compare("OBJ") != 0 && m_modelext.compare("GLTF") != 0);
                m_holddata->disable_mesh_compression_settings(DestinationOptions->m_Do_Mesh_Compression == C_Destination_Options::eMeshCompression::NoComp);
                connect(m_data, SIGNAL(onMesh_Compression(QVariant&)), this, SLOT(onMesh_Compression(QVariant&)));
            }
        }

        //========================
        // Image Data Settings
        //========================
        else {
            //calling function to initialize the setting class properly according to compress format
            compressionValueChanged((QVariant&)(m_C_Destination_Options->m_Compression));
            controllerisNULL = false;

            // Show relavent Settings
            if (m_propFormat)
                m_propFormat->setHidden(false);
            if (m_propQuality) {
#ifdef USE_ENABLEHQ
                if (m_propEnableHQ) m_propEnableHQ->setHidden(true); // v4.2 feature set to false when ready
#endif
                m_propQuality->setHidden(false);
                QtVariantPropertyManager* Manager = (QtVariantPropertyManager*)m_propQuality->propertyManager();
                setMinMaxStep(Manager, m_propQuality, 0.0, 1.0, 0.01, 2);
            }
            if (m_propWidth)
                m_propWidth->setHidden(false);
            if (m_propHeight)
                m_propHeight->setHidden(false);
            if (m_propCompRatio)
                m_propCompRatio->setHidden(false);
            if (m_propCompTime)
                m_propCompTime->setHidden(false);

            //========================
            // Monitor changes in value
            //========================
            connect(m_data, SIGNAL(compressionChanged(QVariant&)), this, SLOT(compressionValueChanged(QVariant&)));
            connect(m_data, SIGNAL(qualityChanged(QVariant&)), this, SLOT(qualityValueChanged(QVariant&)));
            connect(m_data, SIGNAL(redwChanged(QVariant&)), this, SLOT(redwValueChanged(QVariant&)));
            connect(m_data, SIGNAL(greenwChanged(QVariant&)), this, SLOT(greenwValueChanged(QVariant&)));
            connect(m_data, SIGNAL(bluewChanged(QVariant&)), this, SLOT(bluewValueChanged(QVariant&)));
            connect(m_data, SIGNAL(thresholdChanged(QVariant&)), this, SLOT(thresholdValueChanged(QVariant&)));
            connect(m_data, SIGNAL(bitrateChanged(QString&, int&, int&)), this, SLOT(bitrateValueChanged(QString&, int&, int&)));

            connect(m_data, SIGNAL(defogChanged(double&)), this, SLOT(defogValueChanged(double&)));
            connect(m_data, SIGNAL(exposureChanged(double&)), this, SLOT(exposureValueChanged(double&)));
            connect(m_data, SIGNAL(kneeLowChanged(double&)), this, SLOT(kneelowValueChanged(double&)));
            connect(m_data, SIGNAL(kneeHighChanged(double&)), this, SLOT(kneehighValueChanged(double&)));
            connect(m_data, SIGNAL(gammaChanged(double&)), this, SLOT(gammaValueChanged(double&)));
        }  // mage Data Settings

        m_PBCompress->setEnabled(true);
        m_PBCompress->setText(COMPRESS_TEXT);
        m_isEditing_Compress_Options = true;
        connect(m_C_Destination_Options, SIGNAL(dataChanged()), this, SLOT(onCompressDataChanged()));
    } else {
        msgcnt                       = 0;
        m_data                       = data;
        m_isEditing_Compress_Options = false;
        m_theController->setObject(m_data, true, true);
        if (m_currentClassName.compare("C_Global_Process_Settings") == 0) {
            if (m_propQuality == NULL)
                m_propQuality = m_theController->getProperty("Set Quality");
            if (m_propQuality) {
                if (g_Application_Options.m_ImageEncode == C_Application_Options::ImageEncodeWith::GPU_HW) {
                    m_propQuality->setHidden(true);
#ifdef USE_ENABLEHQ
                    if (m_propEnableHQ) m_propEnableHQ->setHidden(true);
#endif
                }
                else {
                    m_propQuality->setHidden(false);
#ifdef USE_ENABLEHQ
                    if (m_propEnableHQ) m_propEnableHQ->setHidden(false);
#endif
                    QtVariantPropertyManager* Manager = (QtVariantPropertyManager*)m_propQuality->propertyManager();
                    setMinMaxStep(Manager, m_propQuality, 0.0, 1.0, 0.01, 2);
                }
            }
        }
    }

    m_infotext->clear();
}

void CImagePropertyView::onMesh_Optimization(QVariant& value) {
    if (!m_holddata)
        return;
    C_Destination_Options::eMeshOptimization comp = (C_Destination_Options::eMeshOptimization&)value;
    m_holddata->disable_mesh_optimization_setting(comp != C_Destination_Options::eMeshOptimization::UserOpt);
}

void CImagePropertyView::onMesh_Compression(QVariant& value) {
    if (!m_holddata)
        return;
    C_Destination_Options::eMeshCompression comp = (C_Destination_Options::eMeshCompression&)value;
    m_holddata->disable_mesh_compression_settings(comp == C_Destination_Options::eMeshCompression::NoComp);
}

//===================================================================
// Check if compression is been specified
//===================================================================
void CImagePropertyView::compressionValueChanged(QVariant& value) {
    bool Quality_Settings     = false;
    bool Channel_Weights      = false;
    bool DXT1_Alpha           = false;
    bool Codec_BlockRate      = false;
    bool Hide_HDR_Image_Properties = true;

    m_infotext->clear();

    C_Destination_Options::eCompression comp = (C_Destination_Options::eCompression&)value;
    switch (comp) {
    case C_Destination_Options::BC6H:
        Quality_Settings     = true;
        Hide_HDR_Image_Properties = false;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(
            "Block Compression (BC) format designed to support high-dynamic range (floating point) color spaces. (e.g. .exr extension image file).");
        break;
    case C_Destination_Options::BC6H_SF:
        Quality_Settings     = true;
        Hide_HDR_Image_Properties = false;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(
            "Block Compression (BC) format designed to support high-dynamic range (signed floating point) color spaces. (e.g. .exr extension image "
            "file).");
        break;
    case C_Destination_Options::BC1:
    case C_Destination_Options::DXT1:
        Quality_Settings = true;
        Channel_Weights  = true;
        DXT1_Alpha       = true;
        if (m_propRefine)
            m_propRefine->setHidden(false);
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(
            "A four component opaque (or 1-bit alpha) compressed texture format. DXT1 identical to BC1.  Four bits per "
            "pixel.");
        break;
    case C_Destination_Options::BC2:
    case C_Destination_Options::DXT3:
        Quality_Settings = true;
        Channel_Weights  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(
            "A four component compressed texture format with explicit alpha. DXT3 identical to BC2. Eight bits per pixel.");
        break;
    case C_Destination_Options::BC3:
    case C_Destination_Options::DXT5:
        Quality_Settings = true;
        Channel_Weights  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(
            "A four component compressed texture format with interpolated alpha. DXT5 identical to BC3. Eight bits per "
            "pixel.");
        break;
    case C_Destination_Options::BC4:
    case C_Destination_Options::BC4_S:
        Quality_Settings = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A single component compressed texture. BC4 identical to ATI1N. Four bits per pixel. BC4_S is used for signed components");
        break;
    case C_Destination_Options::BC5:
    case C_Destination_Options::BC5_S:
    case C_Destination_Options::ATI2N:
    case C_Destination_Options::ATI2N_XY:
    case C_Destination_Options::ATI2N_DXT5:
        Quality_Settings = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A two component compressed texture format. BC5 identical to ATI2N. Eight bits per pixel. BC5_S is used for signed components");
        break;
    case C_Destination_Options::ASTC:
        Quality_Settings = true;
        Codec_BlockRate  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("ASTC (Adaptive Scalable Texture Compression),lossy block-based texture compression developed with ARM.");
        break;
    case C_Destination_Options::BC7:
        Quality_Settings = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("The latest block Compression (BC) format designed to support high-quality compression of RGB and RGBA bytes color spaces.");
        break;
    case C_Destination_Options::ATC_RGB:
        Quality_Settings = true;
        Channel_Weights  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A compressed RGB format.");
        break;
    case C_Destination_Options::ATC_RGBA_Explicit:
        Quality_Settings = true;
        Channel_Weights  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A compressed ARGB format with explicit alpha.");
        break;
    case C_Destination_Options::ATC_RGBA_Interpolated:
        Quality_Settings = true;
        Channel_Weights  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("A compressed ARGB format with interpolated alpha.");
        break;
    case C_Destination_Options::DXT5_xGBR:
        Quality_Settings = true;
        Channel_Weights  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(" DXT5 with the red component swizzled into the alpha channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_RxBG:
        Quality_Settings = true;
        Channel_Weights  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(
            " swizzled DXT5 format with the green component swizzled into the alpha channel & the blue component swizzled into the green channel. "
            "Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_xRBG:
        Quality_Settings = true;
        Channel_Weights  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(
            "swizzled DXT5 format with the green component swizzled into the alpha channel & the red component swizzled into the green channel. "
            "Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_RGxB:
        Quality_Settings = true;
        Channel_Weights  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("swizzled DXT5 format with the blue component swizzled into the alpha channel. Eight bits per pixel.");
        break;
    case C_Destination_Options::DXT5_xGxR:
        Quality_Settings = true;
        Channel_Weights  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append(
            "two-component swizzled DXT5 format with the red component swizzled into the alpha channel & the green component in the green channel. "
            "Eight bits per pixel.");
        break;
    case C_Destination_Options::ETC2_RGB:
    case C_Destination_Options::ETC_RGB:
        Quality_Settings = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("ETC (Ericsson Texture Compression, lossy texture compression developed with Ericsson Research.)");
        break;
#ifdef USE_APC
    case C_Destination_Options::APC:
        Quality_Settings = true;
        Codec_BlockRate  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("The latest block Compression (APC format designed to support high compression");
        break;
#endif
#ifdef USE_GTC
    case C_Destination_Options::GTC:
        Quality_Settings = true;
        Codec_BlockRate  = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("The latest block Compression (GTC) format designed to support high-speed compression");
        break;
#endif
#ifdef USE_BASIS
    case C_Destination_Options::BASIS:
        Quality_Settings = true;
        m_infotext->append("<b>Format Description</b>");
        m_infotext->append("The latest block Compression format designed to support high-speed compression");
        break;
#endif
    }

    if (m_propQuality) {
#ifdef USE_ENABLEHQ
        if (m_propEnableHQ) m_propQuality->setHidden(!Quality_Settings);
#endif
        if (m_C_Destination_Options->m_globalSetting_qualityEnabled) {
            m_propQuality->setEnabled(false);
#ifdef USE_ENABLEHQ
            if (m_propEnableHQ) m_propEnableHQ->setEnabled(true); // v4.2 feature set to false when ready
#endif
        } else {
            if (g_Application_Options.m_ImageEncode == C_Application_Options::ImageEncodeWith::GPU_HW) {
                m_propQuality->setEnabled(false);
#ifdef USE_ENABLEHQ
                if (m_propEnableHQ) m_propEnableHQ->setEnabled(false);
#endif
            }
            else {
                m_propQuality->setEnabled(Quality_Settings);
#ifdef USE_ENABLEHQ
                if (m_propEnableHQ) m_propEnableHQ->setEnabled(Quality_Settings);
#endif
            }
        }
    }

    if (m_propDXT1Alpha) {
        m_propDXT1Alpha->setHidden(!DXT1_Alpha);
    }

    if (m_propChannelWeight) {
        m_propChannelWeight->setHidden(!Channel_Weights);
    }

    if (m_propCodecBlockRate) {
        m_propCodecBlockRate->setHidden(!Codec_BlockRate);
    }

    if (m_propHDRProperties) {
        m_propHDRProperties->setHidden(Hide_HDR_Image_Properties);
    }

    if (Quality_Settings) {
        m_infotext->append("Destination file will be <b>Compressed</b> when processed");
    } else {
        m_infotext->append("Destination file will be <b>Transcoded</b> when processed");
    }
}

//===================================================================
// Check if compression is been specified
//===================================================================
void CImagePropertyView::qualityValueChanged(QVariant& value) {
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Quality</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0 (Poor Quality) to 1 (High Quality) with default set at 0.05");
}

//===================================================================
// Check if compression is been specified
//===================================================================
void CImagePropertyView::redwValueChanged(QVariant& value) {
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Red Channel Weighting</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0.01 (Lowest Weight) to 1 (Highest Weight) with default set at 0.3086");
}

//===================================================================
// Check if compression is been specified
//===================================================================
void CImagePropertyView::greenwValueChanged(QVariant& value) {
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Green Channel Weighting</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0.01 (Lowest Weight) to 1 (Highest Weight) with default set at 0.6094");
}

//===================================================================
// Check if compression is been specified
//===================================================================
void CImagePropertyView::bluewValueChanged(QVariant& value) {
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Blue Channel Weighting</b> Applies only to compressed formats");
    m_infotext->append("Value range is 0.01 (Lowest Weight) to 1 (Highest Weight) with default set at 0.0820");
}

//===================================================================
// Check if compression is been specified
//===================================================================
void CImagePropertyView::thresholdValueChanged(QVariant& value) {
    Q_UNUSED(value);
    m_infotext->clear();
    m_infotext->append("<b>Alpha Threshold</b> Applies only to compressed formats (with alpha channel on)");
    m_infotext->append("Value range is 1-255");
}

//===================================================================
// Check if compression is been specified
//===================================================================
void CImagePropertyView::bitrateValueChanged(QString& actualbitrate, int& xblock, int& yblock) {
    QString msg      = "";
    QString blockmsg = "";
    if (xblock == -1 && yblock == -1) {
        msg      = "Invalid input. Not supported.";
        blockmsg = "Value changed to default bit rate 8.00 (4x4).";
    } else {
        msg      = "The <b>closet bit rate</b> is " + actualbitrate;
        blockmsg = "<b>Block number</b> is (XxY): " + QString::number(xblock) + "x" + QString::number(yblock);
    }

    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
}

//===================================================================
// Defog value changed
//===================================================================
void CImagePropertyView::defogValueChanged(double& defog) {
    QString msg      = "";
    QString blockmsg = "";
    if (defog < 0.00 || defog > 0.01) {
        msg      = "Invalid input. Not supported. Clamp to valid range";
        blockmsg = "Defog value should be in range of 0.0000 to 0.0100.";
    }

    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
}

//===================================================================
//exposure value changed
//===================================================================
void CImagePropertyView::exposureValueChanged(double& exposure) {
    QString msg      = "";
    QString blockmsg = "";
    if (exposure < -10.0 || exposure > 10.0) {
        msg      = "Invalid input. Not supported. Clamp to valid range";
        blockmsg = "Exposure value supported is in range of -10.0 to 10.0.";
    }

    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
}

//===================================================================
// kneelow value changed
//===================================================================
void CImagePropertyView::kneelowValueChanged(double& kl) {
    QString msg      = "";
    QString blockmsg = "";
    if (kl < -3.00 || kl > 3.00) {
        msg      = "Invalid input. Not supported. Clamp to valid range";
        blockmsg = "Knee Low value should be in range of -3.0 to 3.0.";
    }

    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
}

//===================================================================
// knee high value changed
//===================================================================
void CImagePropertyView::kneehighValueChanged(double& kh) {
    QString msg      = "";
    QString blockmsg = "";
    if (kh < 3.5 || kh > 7.5) {
        msg      = "Invalid input. Not supported. Clamp to valid range";
        blockmsg = "Knee High value supported is in range of 3.5 to 7.5.";
    }

    m_infotext->clear();
    m_infotext->append(msg);
    m_infotext->append(blockmsg);
}

//===================================================================
// gamma value changed
//===================================================================
void CImagePropertyView::gammaValueChanged(double& gamma) {
    QString msg      = "";
    QString blockmsg = "";
    if (gamma < 1.0 || gamma > 2.6) {
        msg      = "Invalid input. Not supported. Clamp to valid range";
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

void CImagePropertyView::refreshView() {
    m_theController->setObject(m_data, true, true);
}

void CImagePropertyView::setMinMaxStep(QtVariantPropertyManager* manager, QtProperty* m_prop, double min, double max, double step, int  decimals) {
    if (manager) {
        QtVariantProperty* prop = manager->variantProperty(m_prop);
        if (prop) {
            prop->setAttribute(STR_SETTING_MINIMUM, min);
            prop->setAttribute(STR_SETTING_MAXIMUM, max);
            prop->setAttribute(STR_SETTING_SINGLESTEP, step);
            prop->setAttribute(STR_SETTING_DECIMALS,decimals);
        }
    }
}

void CImagePropertyView::onCompressDataChanged() {
    if (m_C_Destination_Options->m_data_has_been_changed) {
        m_PBSave->setEnabled(true);
        m_PBCancel->setEnabled(true);
        m_PBCompress->setText(SAVECOMP_TEXT);
    }
}

void CImagePropertyView::closeEvent(QCloseEvent* event) {
    hide();
    event->ignore();
}

void CImagePropertyView::onCancel() {
    *m_C_Destination_Options << (const C_Destination_Options&)*m_holddata;
    refreshView();
    m_PBSave->setEnabled(false);
    m_PBCancel->setEnabled(false);
    m_PBCompress->setText(COMPRESS_TEXT);
}

void CImagePropertyView::onSave() {
    if (m_isEditing_Compress_Options) {
        m_PBSave->setEnabled(false);
        m_PBCancel->setEnabled(false);
        m_PBCompress->setText(COMPRESS_TEXT);
        *m_holddata << *m_C_Destination_Options;
        emit saveSetting(&m_C_Destination_Options->m_destFileNamePath);
        msgcnt = 0;
    }
}

void CImagePropertyView::onCompress() {
    if (m_isEditing_Compress_Options) {
        *m_holddata << *m_C_Destination_Options;
        msgcnt = 0;
        if (m_holddata) {
            if (m_holddata->m_isModelData) {
                if (m_holddata->getDo_Mesh_Optimization() == m_holddata->NoOpt && m_holddata->getDo_Mesh_Compression() == m_holddata->NoComp) {
                    QMessageBox msgBox;
                    msgBox.setText("No process setting detected: Please select at least one mesh process (Optimize and/or Compress) option.");
                    msgBox.exec();
                    return;
                }
            }
        }

        m_PBSave->setEnabled(false);
        m_PBCancel->setEnabled(false);
        m_PBCompress->setText(COMPRESS_TEXT);
        m_PBCompress->setEnabled(false);

        emit compressImage(&m_C_Destination_Options->m_destFileNamePath);
    }
}

void CImagePropertyView::onCompressionStart() {
    m_PBCompress->setEnabled(false);
}

void CImagePropertyView::onCompressionDone() {
    m_PBCompress->setEnabled(true);
}

CImagePropertyView::~CImagePropertyView() {
    if (m_C_Destination_Options) {
        delete m_C_Destination_Options;
        m_C_Destination_Options = NULL;
    }
}

void CImagePropertyView::onImageLoadStart() {
    m_PBCompress->setEnabled(false);
}

void CImagePropertyView::onImageLoadDone() {
    m_PBCompress->setEnabled(true);
}

void CImagePropertyView::onSourceImage(int childCount) {
    Q_UNUSED(childCount);
    //reserved for future feature -> m_PBCompress->setEnabled(childCount> 1);
}