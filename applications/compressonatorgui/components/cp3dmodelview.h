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

#include "compressonator.h"
#include "common.h"
#include "accustomdockwidget.h"
#include "plugininterface.h"

#define HIDE_VIEW_OPTIONS "Hide View Options"
#define SHOW_VIEW_OPTIONS "Show View Options"

//
// This Class is defined as Static
// to enable message handling via emit signals calls from static & global functions
//
class winMsgHandler : public QObject {
    Q_OBJECT

  public:
    winMsgHandler() {};

#ifdef _WIN32
  Q_SIGNALS:
    void signalMessage(MSG& msg);
#else
  Q_SIGNALS:
    void signalMessage(QString& msg);
#endif
};

class cpRenderWindow : public QWidget {
    Q_OBJECT

  public:
    bool m_showViewOptions;
    WId  m_wid;
    bool m_usingWindowProc;
    bool m_viewingOriginalModel;

    cpRenderWindow() {
        m_plugin          = NULL;
        m_viewOpen        = false;
        m_wid             = 0L;
        m_usingWindowProc = false;
        m_manual3DViewFlipMode = 0;
        m_viewingOriginalModel = true;
        this->setFocusPolicy(Qt::ClickFocus);
        qApp->installEventFilter(this);
    }

    void setplugin(PluginInterface_3DModel* plugin) {
        m_plugin = plugin;
    }

    void setView(bool viewOpen) {
        m_viewOpen = viewOpen;
    }

    void setHwnd(HWND hwnd) {
        m_hwnd = hwnd;
    }

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    // Effective only when watching a 3DModel Diff
    void SetManualRenderFlip(int mode);

  Q_SIGNALS:
    void signalModelKeyPressed(int key);

#ifdef _WIN32
  public slots:
    void localMessage(MSG& msg);
#endif

  private:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result);

    void paintEvent(QPaintEvent* ev);
    bool eventFilter(QObject* obj, QEvent* ev);
    void resizeEvent(QResizeEvent*);
    void  keyPressEvent(QKeyEvent *event);

    HWND                     m_hwnd;  // Handle to the window created for rendering the glTF views
    PluginInterface_3DModel* m_plugin;
    bool                     m_viewOpen;
    int                      m_manual3DViewFlipMode;
};

class cp3DModelView : public acCustomDockWidget {
    Q_OBJECT
  public:
    int m_showViewOptions;
    cp3DModelView(const QString filePathName, const QString filePathName2, const QString Title, QWidget* parent);
    ~cp3DModelView();
    void Clean3DModelView();
    void setManualViewFlip(int mode);

  private:
    HWND m_hwnd;  // Handle to the window created for rendering the glTF views

    // Common for all
    QWidget* m_newWidget;

    QWidget* m_renderViewQt;

    cpRenderWindow* m_renderview;
    QGridLayout*    m_layout;
    QWidget*        m_parent;
    QStatusBar*     m_statusBar;
    bool            m_viewOpen;

    // ToolBar
    QToolBar*    m_toolBar;
    QPushButton* m_OptionsButton;

    PluginInterface_3DModel*        m_plugin;
    PluginInterface_3DModel_Loader* m_plugin_loader;
    bool                            m_isviewingDX12;

  Q_SIGNALS:
    void UpdateData(QObject* data);
    void OnSetScale(int value);

  public slots:
    void OnToolBarClicked();  // Hook into the CustomeWidgets TitleBars On Tool Button Clicked events
    void OnShowOptions();     // Toggles Overlayed Display Options in 3D Model Viewers
    void OnModelViewKeyPressed(int key);
};

extern PluginManager g_pluginManager;

#endif  // _3DMODELVIEW_H
