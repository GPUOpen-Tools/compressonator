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

#include "cpSetApplicationOptions.h"

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


CSetApplicationOptions::CSetApplicationOptions(const QString title, QWidget *parent) : 
m_title(title),
m_parent(parent)
{
    setWindowTitle(title);
    Qt::WindowFlags flags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    setWindowFlags(flags);

    // Widget to be placed onto this DockWidget

    m_theController = new ObjectController(this, true);
    m_layoutV       = new QVBoxLayout(this);

    m_browser = m_theController->getTreeBrowser();
    if (m_browser)
    {
        m_browser->setHeaderVisible(false);
        m_browser->SetBrowserClick(true);
        m_browser->setResizeMode(QtTreePropertyBrowser::ResizeToContents); // follow this comment Note#1
        connect(m_browser, SIGNAL(currentItemChanged(QtBrowserItem *)), this, SLOT(oncurrentItemChanged(QtBrowserItem *)));
    }

    connect(&g_Application_Options, SIGNAL(ImageViewDecodeChanged(QVariant &)), this, SLOT(onImageViewDecodeChanged(QVariant &)));
#ifdef USE_COMPUTE
    connect(&g_Application_Options, SIGNAL(ImageEncodeChanged(QVariant &)), this, SLOT(onImageEncodeChanged(QVariant &)));
#endif
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
    m_PBClose = new QPushButton("Close");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_PBClose);
    m_layoutV->addLayout(buttonLayout);

    connect(m_PBClose, SIGNAL(pressed()), this, SLOT(onClose()));

    setLayout(m_layoutV);
}

void CSetApplicationOptions::onImageViewDecodeChanged(QVariant &value)
{
    C_Application_Options::ImageDecodeWith decodewith= (C_Application_Options::ImageDecodeWith &)value;

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

C_Application_Options::ImageEncodeWith encodewith = C_Application_Options::ImageEncodeWith::GPU_OpenCL;
void CSetApplicationOptions::onImageEncodeChanged(QVariant &value)
{
    encodewith = (C_Application_Options::ImageEncodeWith &)value;

    if (encodewith == C_Application_Options::ImageEncodeWith::CPU)
        g_useCPUEncode = true;
    else 
        g_useCPUEncode = false;

}

void CSetApplicationOptions::onClose()
{
    g_useCPUDecode = (g_Application_Options.m_ImageViewDecode == g_Application_Options.ImageDecodeWith::CPU);
#ifdef USE_COMPUTE
    g_useCPUEncode = (g_Application_Options.m_ImageEncode == g_Application_Options.ImageEncodeWith::CPU);
#endif
    emit OnAppSettingHide();
    close();
}

CSetApplicationOptions::~CSetApplicationOptions()
{
}


// -----------------------------------------------------------
// Signaled when items focus has changed on th property view
// ----------------------------------------
void CSetApplicationOptions::oncurrentItemChanged(QtBrowserItem *item)
{
    if (!item) return;
    m_infotext->clear();


    QString text;

    QtProperty *treeItem = item->property();
    if (!treeItem) return;

    text = treeItem->propertyName();
    text.replace(QString("_"), QString(" "));

    if (text.compare(APP_Decompress_image_views_using) == 0)
    {
        m_infotext->append("<b>Compressed image views</b>");
        m_infotext->append("For compressed images this option selects how images are decompressed for viewing.");
        m_infotext->append("<b>Note:</b>");
        m_infotext->append("For ASTC, GPU Decompress will not work until hardware supports it");
        m_infotext->append("For ETCn, GPU Decompress with DirectX is not supported");
        m_infotext->append("For HDR image view, decode with OpenGL is not supported. It may appear darker.");
    }
#ifdef USE_COMPUTE    
    if (text.compare(APP_compress_image_using) == 0)
    {
        m_infotext->append("<b>Compressed image</b>");
        m_infotext->append("For compressed images this option selects how images are compressed either with CPU or GPU.");
        m_infotext->append("<b>Note:</b>");
        m_infotext->append("Only BC1, BC7 format are supported with GPU Compress, if you choose other format under GPU Compress, they will be compressed with CPU.");
    }

    else
#endif
    if (text.compare(APP_Reload_image_views_on_selection) == 0)
    {
        m_infotext->append("<b>Reload image views</b>");
        m_infotext->append("Refreshes image cache views when an image is processed or a setting has changed\n");
    }
    else if (text.compare(APP_Load_recent_project_on_startup) == 0)
    {
        m_infotext->append("<b>Load recent project</b>");
        m_infotext->append("Reloads the last project session each time the application is started");
    }
    else if (text.compare(APP_Close_all_image_views_prior_to_process) == 0)
    {
        m_infotext->append("<b>Close all image views prior to processing</b>");
        m_infotext->append("This will free up system memory, to avoid out of memory issues when processing large files");
    }
    else if (text.compare(APP_Mouse_click_on_icon_to_view_image) == 0)
    {
        m_infotext->append("<b>Mouse click on icon to view image</b>");
        m_infotext->append("Mouse click on icons will display a image view, clicking on the items text will update the Properties page only");
    }
    else if (text.compare(APP_Set_Image_Diff_Contrast) == 0)
    {
        m_infotext->append("<b>Set Image Diff Contrast</b>");
        m_infotext->append("Sets the contrast of pixels for image view diff, default is 20.0 using 1.0 returns pixels to original diff contrast, min is 1 max is 200");
    }

#ifdef USE_3DVIEWALLAPI
    else
    if (text.compare(APP_Render_Models_with) == 0)
    {
        m_infotext->append("<b>Selects how to render 3DModels files</b>");
    }
#endif

}

void CSetApplicationOptions::UpdateViewData()
{
    m_theController->setObject(&g_Application_Options, true, true);
    g_useCPUDecode = g_Application_Options.m_ImageViewDecode == g_Application_Options.ImageDecodeWith::CPU;
#ifdef USE_COMPUTE
    g_useCPUEncode = g_Application_Options.m_ImageEncode == g_Application_Options.ImageEncodeWith::CPU;
#endif
}

void CSetApplicationOptions::SaveSettings(QString SettingsFile, QSettings::Format Format)
{
    QSettings settings(SettingsFile, Format);
    QVariant var;
    QString  name;

    int count = g_Application_Options.metaObject()->propertyCount();
    int parent_cnt= g_Application_Options.metaObject()->superClass()->propertyCount();

    for (int i = 0; i < parent_cnt; ++i) 
    {
        if (g_Application_Options.metaObject()->superClass()->property(i).isStored(&g_Application_Options)) {
            var = g_Application_Options.metaObject()->superClass()->property(i).read(&g_Application_Options);
            name = g_Application_Options.metaObject()->superClass()->property(i).name();
            settings.setValue(name, var);
        }
    }

    for (int i = 0; i<count; ++i) 
    {
        if (g_Application_Options.metaObject()->property(i).isStored(&g_Application_Options)) {
            var = g_Application_Options.metaObject()->property(i).read(&g_Application_Options);
            name = g_Application_Options.metaObject()->property(i).name();
            settings.setValue(name, var);
        }
    }
}

void CSetApplicationOptions::LoadSettings(QString SettingsFile, QSettings::Format Format)
{
    QSettings settings(SettingsFile, Format);
    QVariant var;
    QString  name;
    int count = g_Application_Options.metaObject()->propertyCount();
    int parent_cnt = g_Application_Options.metaObject()->superClass()->propertyCount();

    for (int i = 0; i < parent_cnt; ++i)
    {
        if (g_Application_Options.metaObject()->superClass()->property(i).isStored(&g_Application_Options)) {
            name = g_Application_Options.metaObject()->superClass()->property(i).name();
            var = g_Application_Options.metaObject()->superClass()->property(i).read(&g_Application_Options);
            var = settings.value(name, var);
            name.replace(QString("_"), QString(" "));
            // ENum bug in Write need to fix this, for now we will use this
           
        }
    }

    for (int i = 0; i<count; ++i) {
        if (g_Application_Options.metaObject()->property(i).isStored(&g_Application_Options)) {
            name = g_Application_Options.metaObject()->property(i).name();
            var = g_Application_Options.metaObject()->property(i).read(&g_Application_Options);
            var = settings.value(name, var);
            name.replace(QString("_"), QString(" "));
#ifdef USE_COMPUTE
            if (name.compare(APP_compress_image_using) == 0)
            {
                int value = var.value<int>();
                C_Application_Options::ImageEncodeWith encode = (C_Application_Options::ImageEncodeWith) value;
                g_Application_Options.setImageEncode(encode);
            }
#endif
            if (name.compare(APP_Decompress_image_views_using) == 0)
            {
                int value = var.value<int>();
                C_Application_Options::ImageDecodeWith decodeWith = (C_Application_Options::ImageDecodeWith) value;
                g_Application_Options.setImageViewDecode(decodeWith);
            }
#ifdef USE_3DVIEWALLAPI
            else
            if (name.compare(APP_Render_Models_with) == 0)
            {
                int value = var.value<int>();
                C_Application_Options::RenderModelsWith render = (C_Application_Options::RenderModelsWith) value;
                g_Application_Options.setGLTFRender(render);
            }
#endif
            else
                g_Application_Options.metaObject()->property(i).write(&g_Application_Options, var);
        }
    }

}
