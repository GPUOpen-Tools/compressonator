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

#include "cpsetapplicationoptions.h"
#include "qtbuttonpropertybrowser.h"
#include "qteditorfactory.h"
#include "qtgroupboxpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qttreepropertybrowser.h"
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMap>
#include <QScrollArea>

CSetApplicationOptions::CSetApplicationOptions(const QString title, QWidget* parent)
    : m_title(title)
    , m_parent(parent) {
    setWindowTitle(title);
    Qt::WindowFlags flags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    setWindowFlags(flags);

    m_propAnalysisTable = NULL;

    // Widget to be placed onto this DockWidget

    m_theController = new ObjectController(this, true);
    m_layoutV       = new QVBoxLayout(this);

    m_browser = m_theController->getTreeBrowser();
    if (m_browser) {
        m_browser->setHeaderVisible(false);
        m_browser->SetBrowserClick(true);
        m_browser->setResizeMode(QtTreePropertyBrowser::ResizeToContents);  // follow this comment Note#1
        connect(m_browser, SIGNAL(currentItemChanged(QtBrowserItem*)), this, SLOT(oncurrentItemChanged(QtBrowserItem*)));
    }

    connect(&g_Application_Options, SIGNAL(ImageViewDecodeChanged(QVariant&)), this, SLOT(onImageViewDecodeChanged(QVariant&)));
    connect(&g_Application_Options, SIGNAL(LogResultsChanged(QVariant&)), this, SLOT(onLogResultsChanged(QVariant&)));
    connect(&g_Application_Options, SIGNAL(ImageEncodeChanged(QVariant&)), this, SLOT(onImageEncodeChanged(QVariant&)));
    m_theController->setObject(&g_Application_Options, true);
    m_layoutV->addWidget(m_theController);

    m_infotext = new QTextBrowser(this);
    // Always show min Two lines of text and Max to 5 lines at font size 16
    m_infotext->setMinimumHeight(32);
    m_infotext->setMaximumHeight(96);
    m_infotext->setReadOnly(true);
    m_infotext->setAcceptRichText(true);
    m_layoutV->addWidget(m_infotext);
    setMinimumWidth(400);

    // Buttons
    m_PBClose                 = new QPushButton("Close");
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_PBClose);
    m_layoutV->addLayout(buttonLayout);

    connect(m_PBClose, SIGNAL(pressed()), this, SLOT(onClose()));

    setLayout(m_layoutV);
}

void CSetApplicationOptions::onImageViewDecodeChanged(QVariant& value) {
    C_Application_Options::ImageDecodeWith decodewith = (C_Application_Options::ImageDecodeWith&)value;

    if (decodewith == C_Application_Options::ImageDecodeWith::CPU)
        g_useCPUDecode = true;
    else
        g_useCPUDecode = false;

    if (decodewith == C_Application_Options::ImageDecodeWith::GPU_OpenGL)
        g_gpudecodeFormat = MIPIMAGE_FORMAT::Format_OpenGL;
    else if (decodewith == C_Application_Options::ImageDecodeWith::GPU_DirectX)
        g_gpudecodeFormat = MIPIMAGE_FORMAT::Format_DirectX;
    else if (decodewith == C_Application_Options::ImageDecodeWith::GPU_Vulkan)
        g_gpudecodeFormat = MIPIMAGE_FORMAT::Format_Vulkan;
    else
        g_gpudecodeFormat = MIPIMAGE_FORMAT::Format_QImage;
}

void CSetApplicationOptions::onImageEncodeChanged(QVariant& value) {
    g_Application_Options.m_ImageEncode = (C_Application_Options::ImageEncodeWith&)value;

    if (g_Application_Options.m_ImageEncode == C_Application_Options::ImageEncodeWith::CPU)
        g_useCPUEncode = true;
    else
        g_useCPUEncode = false;

    QtProperty *m_qproperty;

    m_qproperty = m_theController->getProperty(APP_Use_GPU_To_Generate_MipMaps);
    if (m_qproperty) {
        m_qproperty->setEnabled((g_Application_Options.m_ImageEncode == C_Application_Options::ImageEncodeWith::GPU_HW));
    }

    m_qproperty = m_theController->getProperty(APP_Use_SRGB_Frames_While_Encoding);
    if (m_qproperty) {
        m_qproperty->setEnabled((g_Application_Options.m_ImageEncode == C_Application_Options::ImageEncodeWith::GPU_HW));
    }
}

void CSetApplicationOptions::onLogResultsChanged(QVariant& value) {
    m_propAnalysisTable = m_theController->getProperty(APP_Show_Analysis_Results_Table);
    if (m_propAnalysisTable) {
        m_propAnalysisTable->setEnabled((bool&)value);
    }
}

void CSetApplicationOptions::onClose() {
    g_useCPUDecode = (g_Application_Options.m_ImageViewDecode == C_Application_Options::ImageDecodeWith::CPU);
    g_useCPUEncode = (g_Application_Options.m_ImageEncode == C_Application_Options::ImageEncodeWith::CPU);
    emit OnAppSettingHide();
    close();
}

CSetApplicationOptions::~CSetApplicationOptions() {
}

// -----------------------------------------------------------
// Signaled when items focus has changed on th property view
// ----------------------------------------
void CSetApplicationOptions::oncurrentItemChanged(QtBrowserItem* item) {
    if (!item)
        return;
    m_infotext->clear();

    QString text;

    QtProperty* treeItem = item->property();
    if (!treeItem)
        return;

    text = treeItem->propertyName();
    text.replace(QString("_"), QString(" "));

    if (text.compare(APP_Decompress_image_views_using) == 0) {
        m_infotext->append("<b>Compressed image views</b>");
        m_infotext->append("For compressed images this option selects how images are decompressed for viewing.");
        m_infotext->append("<b>Note:</b>");
        m_infotext->append("For ASTC, GPU Decompress will not work until hardware supports it");
        m_infotext->append("For ETCn, GPU Decompress with DirectX is not supported");
        m_infotext->append("For HDR image view, decode with OpenGL is not supported. It may appear darker.");
    }
    if (text.compare(APP_compress_image_using) == 0) {
        m_infotext->append("<b>Compressed image</b>");
        m_infotext->append("For compressed images this option selects how images are compressed either with CPU,HPC,GPU_DirectX, GPU_OpenCL or GPU_HW.");
        m_infotext->append("HPC runs codecs optimized for vector extensions and SPMD processing on CPU.");
        m_infotext->append(
            "GPU_OpenCL or GPU_DirectX options compiles shaders on first use and takes time to process, subsequent runs will load built shaders");
        m_infotext->append("if your GPU device has changed since last use, delete all .cmp files in plugins compute folder to rebuild the shaders");
        m_infotext->append(
            "GPU_HW loads textures onto your Prinmary Graphics Hardware and then compresses it using its supported GL Extensions, If the extension is not "
            "supported processing will revert to using the CPU");
        m_infotext->append("<b>Note:</b>");
        m_infotext->append(
            "Only BC1 to BC7 format are supported for HPC and GPU, if you choose other formats or a shader compile fails on first use, processing with "
            "generalized CPU instructions will be used");
    } else if (text.compare(APP_Reload_image_views_on_selection) == 0) {
        m_infotext->append("<b>Reload image views</b>");
        m_infotext->append("Refreshes image cache views when an image is processed or a setting has changed\n");
    } else if (text.compare(APP_Load_recent_project_on_startup) == 0) {
        m_infotext->append("<b>Load recent project</b>");
        m_infotext->append("Reloads the last project session each time the application is started");
    } else if (text.compare(APP_Close_all_image_views_prior_to_process) == 0) {
        m_infotext->append("<b>Close all image views prior to processing</b>");
        m_infotext->append("This will free up system memory, to avoid out of memory issues when processing large files");
    } else if (text.compare(APP_Mouse_click_on_icon_to_view_image) == 0) {
        m_infotext->append("<b>Mouse click on icon to view image</b>");
        m_infotext->append("Mouse click on icons will display a image view, clicking on the items text will update the Properties page only");
    } else if (text.compare(APP_Set_Image_Diff_Contrast) == 0) {
        m_infotext->append("<b>Set Image Diff Contrast</b>");
        m_infotext->append(
            "Sets the contrast of pixels for image view diff, default is 20.0 using 1.0 returns pixels to original diff contrast, min is 1 max is 200");
    } else if (text.compare(APP_Set_Number_of_Threads) == 0) {
        m_infotext->append("<b>Set Number of Threads</b>");
        m_infotext->append("Sets the number of threads to use for texture compression, max is 128 threads distributed over multiple cores");
        m_infotext->append("Default 0 sets auto detection, where the total threads = number of processor cores, if auto detection fails default = 8");
        int  processors = CMP_NumberOfProcessors();
        char buff[128];
        sprintf(buff, "Max number of processors [%d]", processors);
        m_infotext->append(buff);
        m_infotext->append("<b>Restart the application for the new settings to take effect<b>");
    } else if (text.compare(APP_Use_GPU_To_Generate_MipMaps) == 0) {
        m_infotext->append("<b>Use GPU To Generate MipMaps</b>");
        m_infotext->append(
            "Generates max number of MipMap levels for the image using the GPU hardware, this option is only enabled when EncodeWith is set to GPU_HW");
    } else if (text.compare(APP_Use_SRGB_Frames_While_Encoding) == 0) {
        m_infotext->append("<b>Use SRGB Frames While Encoding</b>");
        m_infotext->append(
            "Encoded frames will use SRGB frame buffer when encoding with GPU hardware, this option is only enabled when EncodeWith is set to GPU_HW");
    } else if (text.compare(APP_Show_MSE_PSNR_SSIM_Results) == 0) {
        m_infotext->append("<b>Show MSE, PSNR and SSIM_Results</b>");
        m_infotext->append("Show these values after processing compressed images");
    } else if (text.compare(APP_Show_Analysis_Results_Table) == 0) {
        m_infotext->append("<b>Show Analysis Results Table</b>");
        m_infotext->append("Show all Process Times, PSNR and SSIM results for compressed images in a table view");
    } else if (text.compare(APP_Render_Models_with) == 0) {
        m_infotext->append("<b>Selects how to render 3DModels files</b>");
    }
}

void CSetApplicationOptions::UpdateViewData() {
    m_theController->setObject(&g_Application_Options, true, true);
    g_useCPUDecode = g_Application_Options.m_ImageViewDecode == C_Application_Options::ImageDecodeWith::CPU;
    g_useCPUEncode = g_Application_Options.m_ImageEncode == C_Application_Options::ImageEncodeWith::CPU;
}

void CSetApplicationOptions::SaveSettings(QString SettingsFile, QSettings::Format Format) {
    QSettings settings(SettingsFile, Format);
    QVariant  var;
    QString   name;

    int count      = g_Application_Options.metaObject()->propertyCount();
    int parent_cnt = g_Application_Options.metaObject()->superClass()->propertyCount();

    for (int i = 0; i < parent_cnt; ++i) {
        if (g_Application_Options.metaObject()->superClass()->property(i).isStored(&g_Application_Options)) {
            var  = g_Application_Options.metaObject()->superClass()->property(i).read(&g_Application_Options);
            name = g_Application_Options.metaObject()->superClass()->property(i).name();
            settings.setValue(name, var);
        }
    }

    for (int i = 0; i < count; ++i) {
        if (g_Application_Options.metaObject()->property(i).isStored(&g_Application_Options)) {
            var  = g_Application_Options.metaObject()->property(i).read(&g_Application_Options);
            name = g_Application_Options.metaObject()->property(i).name();
            settings.setValue(name, var);
        }
    }
}

void CSetApplicationOptions::LoadSettings(QString SettingsFile, QSettings::Format Format) {
    QSettings settings(SettingsFile, Format);
    QVariant  var;
    QString   name;
    int       count      = g_Application_Options.metaObject()->propertyCount();
    int       parent_cnt = g_Application_Options.metaObject()->superClass()->propertyCount();

    for (int i = 0; i < parent_cnt; ++i) {
        if (g_Application_Options.metaObject()->superClass()->property(i).isStored(&g_Application_Options)) {
            name = g_Application_Options.metaObject()->superClass()->property(i).name();
            var  = g_Application_Options.metaObject()->superClass()->property(i).read(&g_Application_Options);
            var  = settings.value(name, var);
            name.replace(QString("_"), QString(" "));
            // ENum bug in Write need to fix this, for now we will use this
        }
    }

    for (int i = 0; i < count; ++i) {
        if (g_Application_Options.metaObject()->property(i).isStored(&g_Application_Options)) {
            name = g_Application_Options.metaObject()->property(i).name();
            var  = g_Application_Options.metaObject()->property(i).read(&g_Application_Options);
            var  = settings.value(name, var);
            name.replace(QString("_"), QString(" "));
            if (name.compare(APP_compress_image_using) == 0) {
                int                                    value  = var.value<int>();
                C_Application_Options::ImageEncodeWith encode = (C_Application_Options::ImageEncodeWith)value;
                g_Application_Options.setImageEncode(encode);
            }
            if (name.compare(APP_Decompress_image_views_using) == 0) {
                int                                    value      = var.value<int>();
                C_Application_Options::ImageDecodeWith decodeWith = (C_Application_Options::ImageDecodeWith)value;
                g_Application_Options.setImageViewDecode(decodeWith);
            } else if (name.compare(APP_Render_Models_with) == 0) {
                int                                     value  = var.value<int>();
                C_Application_Options::RenderModelsWith render = (C_Application_Options::RenderModelsWith)value;
                g_Application_Options.setGLTFRender(render);
            } else
                g_Application_Options.metaObject()->property(i).write(&g_Application_Options, var);
        }
    }
}
