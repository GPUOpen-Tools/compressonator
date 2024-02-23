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
/// \file acCustomDockWidget.h
/// \version 2.21
//
//=====================================================================

#ifndef ACCUSTOMDOCKWIDGET_H
#define ACCUSTOMDOCKWIDGET_H

#include <qdebug.h>
#include <qdockwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qevent.h>
#include <qtoolbutton.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qproxystyle.h>
#include <qstyleoption.h>
#include <qpainter.h>

#include "acdockwidgettitlebar.h"

class acCustomDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    acCustomDockWidget(const QString title, QWidget* parent = nullptr);

    acDockWidgetTitlebar* custTitleBar;
    QString               m_CustomTitle;
    QWidget*              m_CentralWidget;
    QString               m_tabName;
    QString               m_fileName;
    int                   m_type = 0;  // Identify what type of docked view it is (Image, 3DModel etc..)

public slots:
    void OntopLevelChanged(bool topLevel);
};

#endif
