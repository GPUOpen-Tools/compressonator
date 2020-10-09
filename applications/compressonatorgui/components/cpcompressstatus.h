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
/// \version 2.20
//
//=====================================================================

#ifndef _CPCOMPRESSSTATUS_H
#define _CPCOMPRESSSTATUS_H


#include <qstring.h>
#include <qimage.h>
#include <qfileinfo.h>
#include <qtextbrowser.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qmenu.h>

#include "compressonator.h"
#include "common.h"
#include "pluginmanager.h"
#include "accustomdockwidget.h"

class CompressStatusDialog :
    public QDockWidget
// public acCustomDockWidget
{
    Q_OBJECT

  public:
    CompressStatusDialog(const QString title,QWidget *parent);
    ~CompressStatusDialog();

    void appendText(QString text);
    void showOutput();
    void hideOutput();

  private:
    QTextBrowser  *m_textBrowser;
    QPushButton   *m_btnClearText;
    QWidget       *m_newWidget;
    QHBoxLayout   *m_layoutH;
    QVBoxLayout   *m_layoutV;
    QWidget       *m_parent;
    QAction       *m_clear;
    QMenu         *m_menu;

  protected:
    void closeEvent(QCloseEvent *event);

  public slots:
    void onClearText();
    void onShowContextMenu(const QPoint&);
};


#endif