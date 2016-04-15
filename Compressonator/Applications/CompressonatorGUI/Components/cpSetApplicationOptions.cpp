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

    m_theController->setObject(&g_Application_Options, true);
    m_layoutV->addWidget(m_theController);

    m_infotext = new QTextBrowser(this);
    // Always show min Two lines of text and Max to 5 lines at font size 16
    m_infotext->setMinimumHeight(32);
    m_infotext->setMaximumHeight(96);
    m_infotext->setReadOnly(true);
    m_infotext->setAcceptRichText(true);
    m_layoutV->addWidget(m_infotext);
    setMinimumWidth(350);

    // Buttons
    m_PBClose = new QPushButton("Close");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_PBClose);
    m_layoutV->addLayout(buttonLayout);

    connect(m_PBClose, SIGNAL(pressed()), this, SLOT(onClose()));

    setLayout(m_layoutV);
}

void CSetApplicationOptions::onClose()
{
    g_useCPUDecode = (g_Application_Options.m_ImageViewDecode == g_Application_Options.ImageViewDecode::CPU);
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
        m_infotext->append("For compressed images this option selects how images are decompressed for viewing");
    }
    else
    if (text.compare(APP_Reload_image_views_on_selection) == 0)
    {
        m_infotext->append("<b>Reload image views</b>");
        m_infotext->append("Refreshes image cache views when an image is processed or a setting has changed\n");
    }
    else
    if (text.compare(APP_Load_recent_project_on_startup) == 0)
    {
        m_infotext->append("<b>Load recent project</b>");
        m_infotext->append("Reloads the last project session each time the application is started");
    }
    else
        if (text.compare(APP_Close_all_image_views_prior_to_process) == 0)
        {
            m_infotext->append("<b>Close all image views prior to processing</b>");
            m_infotext->append("This will free up system memory, to avoid out of memory issues when processing large files");
        }

}

void CSetApplicationOptions::UpdateViewData()
{
    m_theController->setObject(&g_Application_Options, true, true);
    g_useCPUDecode = g_Application_Options.m_ImageViewDecode == g_Application_Options.ImageViewDecode::CPU;
}

void CSetApplicationOptions::SaveSettings(QString SettingsFile, QSettings::Format Format)
{
    QSettings settings(SettingsFile, Format);
    QVariant var;
    QString  name;

    int count = g_Application_Options.metaObject()->propertyCount();
    for (int i = 0; i<count; ++i) {
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
    for (int i = 0; i<count; ++i) {
        if (g_Application_Options.metaObject()->property(i).isStored(&g_Application_Options)) {
            name = g_Application_Options.metaObject()->property(i).name();
            var = g_Application_Options.metaObject()->property(i).read(&g_Application_Options);
            var = settings.value(name, var);
            name.replace(QString("_"), QString(" "));
            // ENum bug in Write need to fix this, for now we will use this
            if (name.compare(APP_Decompress_image_views_using) == 0)
            {
                int value = var.value<int>();
                C_Application_Options::ImageViewDecode decode = (C_Application_Options::ImageViewDecode) value;
                g_Application_Options.setImageViewDecode(decode);
            }
            else
                g_Application_Options.metaObject()->property(i).write(&g_Application_Options, var);
        }
    }

}
