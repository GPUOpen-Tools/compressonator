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

#ifndef _3DMODELVIEW_H
#define _3DMODELVIEW_H

#include <QtWidgets>
#include <QGridLayout>
#include <QFile>
#include <qtimer.h>

#include "acCustomDockWidget.h"
#include "PluginInterface.h"

class cp3DModelView : public acCustomDockWidget
{
    Q_OBJECT
public:

    cp3DModelView(const QString filePathName, const QString Title, QWidget *parent);
    ~cp3DModelView();

private:
    HWND                m_hwnd;     // Handle to the window created for rendering the glTF views

    // Common for all
    QWidget            *m_newWidget;
    QGridLayout        *m_layout;
    QWidget            *m_parent;
    QStatusBar         *m_statusBar;
    bool                m_viewOpen;

    PluginInterface_3DModel *m_plugin;

    void paintEvent(QPaintEvent *ev);
    bool eventFilter(QObject *obj, QEvent *ev);

Q_SIGNALS:
    void UpdateData(QObject *data);
    void OnSetScale(int value);
};

extern PluginManager g_pluginManager;

#endif // _3DMODELVIEW_H