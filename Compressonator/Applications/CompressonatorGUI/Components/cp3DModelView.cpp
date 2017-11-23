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

#include "cp3DModelView.h"
#include "cpMainComponents.h"

//----------------------------------------

extern C_Application_Options g_Application_Options;
extern CMIPS*                g_GUI_CMIPS;

// Shared memory for all views. We keep track of the mouse last pos
// to calc deta {-1,0+1} from current mouse pos to last mouse pos
// see KeyboardMouse.cpp on how this is used to sync all views
static int g_Roll     = { 0 };
static int g_Pitch    = { 0 };
static int g_last_x   = { 0 };
static int g_last_y   = { 0 };


cp3DModelView::~cp3DModelView()
{
    // remove the plugin we used
    if (m_plugin)
    {
        m_plugin->CloseView();
        delete m_plugin;
        m_plugin = NULL;
    }
}

void cp3DModelView::resizeEvent(QResizeEvent *event)
{
    if (m_plugin && m_viewOpen)
    {
        m_plugin->ReSizeView(event->size().width(), event->size().height());
    }
}

void cp3DModelView::paintEvent(QPaintEvent *ev)
{
    if (m_plugin && m_viewOpen)
    {
        m_viewOpen = m_plugin->RenderView();
        update();
    }
}

bool cp3DModelView::eventFilter(QObject *obj, QEvent *ev)
{
   // pass the event on to the parent class if events cannot be handled by this class
   if (obj != m_newWidget)  return QObject::eventFilter(obj, ev);;
   if (!m_plugin)           return QObject::eventFilter(obj, ev);;
   if (!m_viewOpen)         return QObject::eventFilter(obj, ev);;

   if (ev->type() == QEvent::MouseMove)
   {
       MSG msg = { 0 };
       msg.hwnd = m_hwnd;
       QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
       msg.message = WM_MOUSEMOVE;

       if ((g_last_x - mouseEvent->pos().x()) == 0) g_Roll = g_Roll;
       else
           if ((g_last_x - mouseEvent->pos().x()) > 0) g_Roll++;
           else
               g_Roll--;

       if ((g_last_y - mouseEvent->pos().y()) == 0) g_Pitch = g_Pitch;
       else
           if ((g_last_y - mouseEvent->pos().y()) > 0) g_Pitch--;
           else
               g_Pitch++;

       if (g_Roll  > 1000)  g_Roll  = 1000;
       else
       if (g_Roll  < -1000) g_Roll = -1000;
       
       if (g_Pitch > 150)  g_Pitch = 150;
       else
       if (g_Pitch < -150) g_Pitch = -150;

       // msg.lParam = mouseEvent->pos().x() + (mouseEvent->pos().y() << 16);
       msg.lParam = g_Roll + (g_Pitch << 16);

       m_plugin->processMSG(&msg);

       g_last_x = mouseEvent->pos().x();
       g_last_y = mouseEvent->pos().y();

   }
   else
   if (ev->type() == QEvent::MouseButtonPress)
   {
       QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
       if (mouseEvent->button() == Qt::MouseButton::LeftButton)
       {
           MSG msg = { 0 };
           msg.hwnd = m_hwnd;
           msg.message = WM_LBUTTONDOWN;
           m_plugin->processMSG(&msg);
       }
       else
           if (mouseEvent->button() == Qt::MouseButton::RightButton)
           {
               MSG msg = { 0 };
               msg.hwnd = m_hwnd;
               msg.message = WM_RBUTTONDOWN;
               m_plugin->processMSG(&msg);
               g_Roll  = 0;
               g_Pitch = 0;
           }
   }
   else
   if (ev->type() == QEvent::MouseButtonRelease)
   {
       QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
       if (mouseEvent->button() == Qt::MouseButton::LeftButton)
       {
           MSG msg = { 0 };
           msg.hwnd = m_hwnd;
           msg.message = WM_LBUTTONUP;
           //printf("Mouse button release\n");
           m_plugin->processMSG(&msg);
       }
       else
           if (mouseEvent->button() == Qt::MouseButton::RightButton)
           {
               MSG msg = { 0 };
               msg.hwnd = m_hwnd;
               msg.message = WM_RBUTTONUP;
               m_plugin->processMSG(&msg);
           }
   }
   else
   if (ev->type() == QEvent::Wheel)
   {
       QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(ev);
       MSG msg = { 0 };
       msg.hwnd = m_hwnd;
       msg.message = WM_MOUSEWHEEL;

       if (wheelEvent->angleDelta().ry() > 0)
           msg.wParam = 10 << 16;
       else
           msg.wParam = -10 << 16;

       m_plugin->processMSG(&msg);
   }
   else
   if (ev->type() == QEvent::KeyPress) 
   {
       MSG msg = { 0 };
       msg.hwnd = m_hwnd;
       QKeyEvent * ke = (QKeyEvent*)ev;
       m_plugin->processMSG(&msg);
   }
   else
   if (ev->type() == QEvent::Paint)
   {
   }

    return QObject::eventFilter(obj, ev);
}


cp3DModelView::cp3DModelView(const QString filePathName, const QString filePathName2, const QString Title, QWidget *parent) : acCustomDockWidget(filePathName, parent)
{
    m_parent    = parent;
    if (filePathName2.length() > 0)
        m_fileName = filePathName + " " + filePathName2;
    else
        m_fileName  = filePathName;
    m_statusBar = NULL;
    m_hwnd       = 0L;
    m_plugin = NULL;

    qApp->installEventFilter(this);

    QFile f(filePathName);
    QFileInfo fileInfo(f.fileName());
    m_tabName = fileInfo.fileName();

    setWindowTitle(m_tabName);

    //===============
    // Center Widget 
    //===============
    m_newWidget = new QWidget(parent);
    if (!m_newWidget)
    {
        PrintInfo("Error: creating display widget\n");
        return;
    }

    //===============
    // Status Bar
    //===============
    m_statusBar = new QStatusBar(this);
    m_statusBar->setStyleSheet("QStatusBar{border-top: 1px outset grey; border-bottom: 1px outset grey;}");

    m_layout = new QGridLayout(m_newWidget);

    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_statusBar, 1, 0);
    m_newWidget->setLayout(m_layout);

    setWidget(m_newWidget);

    m_hwnd = reinterpret_cast<HWND> (m_newWidget->winId());

    if (g_Application_Options.getGLTFRender() == C_Application_Options::RenderModelsWith::glTF_DX12_EX)
    {
       m_plugin = reinterpret_cast<PluginInterface_3DModel *>(g_pluginManager.GetPlugin("3DMODEL_DX12_EX", "GLTF"));
       this->m_CustomTitle = Title + "glTF DX12";
       if (m_plugin)
       {
         m_plugin->TC_PluginSetSharedIO(g_GUI_CMIPS);

        if (m_hwnd > 0)
        {
            cpMainComponents *mainComponents = NULL;
            void *msgHandler = NULL;
            if (m_parent)
            {
                // assign msg handler
                QString ClassName = m_parent->metaObject()->className();
                if (ClassName.compare("cpMainComponents") == 0)
                {
                    mainComponents = (cpMainComponents *)m_parent;
                    msgHandler = mainComponents->PrintStatus;
                }
            }

            int result;
            if (filePathName2.length() > 0)
                result = m_plugin->CreateView(filePathName.toLatin1(), parent->width(), parent->height(), m_hwnd, &g_pluginManager, msgHandler, filePathName2.toLatin1(), &ProgressCallback);
            else
                 result = m_plugin->CreateView(filePathName.toLatin1(), parent->width(), parent->height(), m_hwnd, &g_pluginManager, msgHandler, NULL, &ProgressCallback);

            if (result == 0) m_viewOpen = true;
            else 
            {
                if (result == -100)
                    PrintInfo("Error: Loading glTF file failed. Note: Embedded glTF view is not supported.\n");
                if (mainComponents) 
                {
                    if (mainComponents->m_CompressStatusDialog)
                    {
                        mainComponents->m_CompressStatusDialog->showOutput();
                        mainComponents->m_CompressStatusDialog->raise();
                    }
                }
            }
        }
       }
   }
#ifdef USE_GLTF_OPENGL 
    else
    if (g_Application_Options.getGLTFRender() == C_Application_Options::RenderModelsWith::glTF_OpenGL)
        {
            m_plugin = reinterpret_cast<PluginInterface_3DModel *>(g_pluginManager.GetPlugin("3DMODEL_OPENGL", "GLTF"));
            this->m_CustomTitle = Title + "glTF OpenGL";

            if (m_plugin)
            {
                m_hwnd = (HWND)m_newWidget->winId();
                if (m_hwnd > 0)
                {
                    cpMainComponents *mainComponents = NULL;
                    void *msgHandler = NULL;
                    if (m_parent)
                    {
                        // assign msg handler
                        QString ClassName = m_parent->metaObject()->className();
                        if (ClassName.compare("cpMainComponents") == 0)
                        {
                            mainComponents = (cpMainComponents *)m_parent;
                            msgHandler = mainComponents->PrintStatus;
                        }
                    }
                    int result = m_plugin->CreateView(filePathName.toLatin1(), parent->width(), parent->height(), m_newWidget, &g_pluginManager, msgHandler, NULL, &ProgressCallback);
                    if (result == 0) m_viewOpen = true;
                }
            }
        }
#endif
    else
        this->m_CustomTitle = Title + "???";

    custTitleBar->setTitle(m_CustomTitle);
    custTitleBar->setToolTip(filePathName);
    if (Title.contains("File#"))
    {
        custTitleBar->setTitle(Title + ": " + filePathName);
    }


}

