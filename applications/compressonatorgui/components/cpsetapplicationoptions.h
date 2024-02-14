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

#ifndef _SETAPPLICATIONOPTIONS_H
#define _SETAPPLICATIONOPTIONS_H

#include "cpprojectdata.h"
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qtgroupboxpropertybrowser.h"
#include "objectcontroller.h"
#include <QtWidgets>

class CSetApplicationOptions : public QDialog
{
    Q_OBJECT

public:
    CSetApplicationOptions(const QString title, QWidget* parent);
    ~CSetApplicationOptions();

    void                   SaveSettings(QString SettingsFile, QSettings::Format Format);
    void                   LoadSettings(QString SettingsFile, QSettings::Format Format);
    void                   UpdateViewData();
    QTextBrowser*          m_infotext;
    QtTreePropertyBrowser* m_browser;

signals:
    void OnAppSettingHide();

public slots:
    void onClose();
    void oncurrentItemChanged(QtBrowserItem* item);
    void onImageEncodeChanged(QVariant& value);
    void onImageViewDecodeChanged(QVariant& value);
    void onLogResultsChanged(QVariant& value);

private:
    // Common for all
    QWidget* m_newWidget;

    QVBoxLayout*      m_layoutV;
    const QString     m_title;
    QWidget*          m_parent;
    QPushButton*      m_PBClose;
    ObjectController* m_theController;
    QtProperty*       m_propAppOptions;
    QtProperty*       m_propAnalysisTable;
};

extern C_Application_Options g_Application_Options;
extern bool                  g_useCPUDecode;
extern bool                  g_useCPUEncode;
extern MIPIMAGE_FORMAT       g_gpudecodeFormat;

#endif