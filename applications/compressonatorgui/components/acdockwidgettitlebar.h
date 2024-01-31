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
/// \file file acDockWidgetTitleBar.h
/// \version 2.21
//
//=====================================================================

#ifndef ACDOCKWIDGETTITLEBAR_H
#define ACDOCKWIDGETTITLEBAR_H

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

class acDockWidgetTitlebar : public QWidget
{
    Q_OBJECT

private:
    QWidget*     parent;
    QPoint       cursor;
    QLabel*      m_label;
    QHBoxLayout* m_layout;

public:
    acDockWidgetTitlebar(QWidget* parent);
    ~acDockWidgetTitlebar();

    void    setButtonCloseEnabled(bool enabled);
    void    setButtonMinMaxEnabled(bool enabled);
    void    setButtonToolBarEnabled(bool enabled);
    void    setButtonToolBarShow(bool isShowing);
    void    setTitle(QString title);
    QString getTitle();
    bool    m_close;

public slots:
    void OnToolBarClicked();
    void onMaximize();
    void onNormal();
    void onMinimize();
    void onClose();

Q_SIGNALS:
    void ToolBarCliked();
    void OnAboutToClose(QString& Title);

protected:
    QWidget* m_parent;

    bool m_ButtonCloseEnabled;
    bool m_buttonToolBarEnabled;

    QPushButton* m_buttonClose;
    QPushButton* m_buttonMaximize;
    QPushButton* m_buttonMinimize;
    QPushButton* m_buttonNormal;

    QPushButton* m_buttonToolBar;
    QIcon*       m_ToolBarIcon;

    // QWidget Reimplementations
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
};

#endif