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

#include "cp3dmodelview.h"
#include "cpmaincomponents.h"
#include <gltf/cmp_gltfcommon.h>
#include <gltf/tiny_gltf2.h>
#include <gltf/tiny_gltf2_utils.h>
//----------------------------------------

extern C_Application_Options g_Application_Options;
extern CMIPS*                g_GUI_CMIPS;
extern bool                  isDX12Supported();

// Shared memory for all views. We keep track of the mouse last pos
// to calc deta {-1,0+1} from current mouse pos to last mouse pos
// see KeyboardMouse.cpp on how this is used to sync all views
static int g_Roll  = {0};  //x
static int g_Yaw   = {0};  //y
static int g_Pitch = {0};  //z

static int g_last_x = {0};
static int g_last_y = {0};

static winMsgHandler static_winMsgHandler;

#ifdef _WIN32
// this is the main message handler for the calls that uses createNativeWindowView
// it will emit signals to Qt connected classes via the winMsgHandler
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MSG msg;

    msg.hwnd    = hWnd;
    msg.message = message;
    msg.wParam  = wParam;
    msg.lParam  = lParam;
    emit static_winMsgHandler.signalMessage(msg);

    return DefWindowProc(hWnd, message, wParam, lParam);
}

WId createNativeWindowView(const char* name, int width, int height)
{
    WId result = 0;

    WNDCLASSEX windowClass;

    // init window class
    ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
    windowClass.cbSize        = sizeof(WNDCLASSEX);
    windowClass.style         = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = WindowProc;
    windowClass.hInstance     = GetModuleHandle(NULL);
    windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = TEXT("WindowClass1");
    RegisterClassEx(&windowClass);

    RECT windowRect = {0, 0, width, height};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);  // adjust the size

    // const HWND hwnd =CreateWindowEx(
    //          0,
    //          reinterpret_cast<const wchar_t *>(name.utf16()),
    //          L"NativeCtrl",
    //          WS_OVERLAPPEDWINDOW,
    //          CW_USEDEFAULT,
    //          CW_USEDEFAULT,
    //          CW_USEDEFAULT,
    //          CW_USEDEFAULT,
    //          0,
    //          0,
    //          GetModuleHandle(NULL),
    //          NULL);

    HWND hwnd = CreateWindowEx(NULL,
                               TEXT("WindowClass1"),  // name of the window class
                               TEXT("Native Window"),
                               WS_OVERLAPPEDWINDOW,
                               100,
                               100,
                               width,
                               height,
                               NULL,                   // we have no parent window, NULL
                               NULL,                   // we aren't using menus, NULL
                               windowClass.hInstance,  // application handle
                               NULL);                  // used with multiple windows, NULL

    if (hwnd)
    {
        result = WId(hwnd);
    }
    else
    {
        qErrnoWarning("Cannot create window \"%s\"", qPrintable(name));
    }

    return result;
}
#endif

bool cpRenderWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(result);
    Q_UNUSED(eventType);

#ifdef _WIN32
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_ACTIVATE)
    {
    }
    if (msg->message == WM_CREATE)
    {
    }
#endif
    return false;
}

void cpRenderWindow::resizeEvent(QResizeEvent* event)
{
    if (m_plugin && m_viewOpen)
    {
        m_plugin->OnReSizeView(event->size().width(), event->size().height());
    }
}

void cpRenderWindow::paintEvent(QPaintEvent* ev)
{
    (void)ev;

    if (m_plugin && m_viewOpen)
    {
        m_viewOpen = m_plugin->OnRenderView();
        update();
    }
}
static void qNormalizeAngle(int& angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void cpRenderWindow::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != g_Roll)
    {
        g_Roll = angle;
    }
}

void cpRenderWindow::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != g_Yaw)
    {
        g_Yaw = angle;
    }
}

void cpRenderWindow::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != g_Pitch)
    {
        g_Pitch = angle;
    }
}

void cpRenderWindow::SetManualRenderFlip(int mode)
{
#ifdef _WIN32
    if (m_plugin)
    {
        MSG msg                = {0};
        msg.hwnd               = m_hwnd;
        msg.message            = WM_KEYDOWN;
        m_manual3DViewFlipMode = mode;
        msg.wParam             = 1;  // Signal that we want to set a Manual Mode setting
        msg.lParam             = mode;
        m_plugin->processMSG(&msg);
    }
#endif
}

// Keypressed event for the Qt render window
void cpRenderWindow::keyPressEvent(QKeyEvent* event)
{
#ifdef _WIN32
    if (m_plugin)
    {
        // Process key press if 3D model view is in manual mode
        if (m_manual3DViewFlipMode == 1)
        {
            //printf("%x ", event->key());
            MSG msg     = {0};
            msg.hwnd    = m_hwnd;
            msg.message = WM_KEYDOWN;
            // Tracks user flip mode, always starts with original then flips
            // for now we only have two rendered models overlaid in one view.
            if (event->key() == QT_KEY_SPACE)
            {
                m_viewingOriginalModel = m_viewingOriginalModel ? false : true;
                msg.wParam             = 2;  // Signal we want to flip view
                msg.lParam             = m_viewingOriginalModel ? 0 : 1;
                m_plugin->processMSG(&msg);
                emit signalModelKeyPressed(event->key());
            }
        }
    }
#endif
}

// This event handler has "Global" scope and is triggered throughout the app
// Mouse events, paint .. are filtered for the active Rendered Windows
bool cpRenderWindow::eventFilter(QObject* obj, QEvent* ev)
{
    // pass the event on to the parent class if events cannot be handled by this class
    if (obj != this)
        return QObject::eventFilter(obj, ev);
    if (!m_plugin)
        return QObject::eventFilter(obj, ev);

    if (!m_viewOpen)
        return QObject::eventFilter(obj, ev);
#ifdef _WIN32
    static bool LeftButtonPressed  = false;
    static bool RightButtonPressed = false;

    if (ev->type() == QEvent::MouseMove)
    {
        MSG msg                 = {0};
        msg.hwnd                = m_hwnd;
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(ev);

        if (LeftButtonPressed)
        {
            msg.message = WM_MOUSEMOVE;

            if ((g_last_x - mouseEvent->pos().x()) == 0)
                g_Roll = g_Roll;
            else if ((g_last_x - mouseEvent->pos().x()) > 0)
                g_Roll++;
            else
                g_Roll--;

            if ((g_last_y - mouseEvent->pos().y()) == 0)
                g_Pitch = g_Pitch;
            else if ((g_last_y - mouseEvent->pos().y()) > 0)
                g_Pitch--;
            else
                g_Pitch++;

            if (g_Roll > 1000)
                g_Roll = 1000;
            else if (g_Roll < -1000)
                g_Roll = -1000;

            if (g_Pitch > 150)
                g_Pitch = 150;
            else if (g_Pitch < -150)
                g_Pitch = -150;

            msg.lParam = g_Roll + (g_Pitch << 16);
            msg.wParam = mouseEvent->pos().x() + (mouseEvent->pos().y() << 16);

            m_plugin->processMSG(&msg);

            g_last_x = mouseEvent->pos().x();
            g_last_y = mouseEvent->pos().y();
        }
        else if (RightButtonPressed)
        {
            msg.message = WM_MOUSEMOVE;
            msg.lParam  = mouseEvent->pos().x() + (mouseEvent->pos().y() << 16);
            m_plugin->processMSG(&msg);
            g_last_x = mouseEvent->pos().x();
            g_last_y = mouseEvent->pos().y();
        }
    }
    else if (ev->type() == QEvent::MouseButtonPress)
    {
        if (m_usingWindowProc)
            return QObject::eventFilter(obj, ev);
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(ev);
        g_last_x                = mouseEvent->pos().x();
        g_last_y                = mouseEvent->pos().y();

        if (mouseEvent->button() == Qt::MouseButton::LeftButton)
        {
            MSG msg     = {0};
            msg.hwnd    = m_hwnd;
            msg.message = WM_LBUTTONDOWN;
            msg.lParam  = g_last_x + (g_last_y << 16);
            m_plugin->processMSG(&msg);
            LeftButtonPressed = true;
        }
        else if (mouseEvent->button() == Qt::MouseButton::RightButton)
        {
            MSG msg     = {0};
            msg.hwnd    = m_hwnd;
            msg.message = WM_RBUTTONDOWN;
            msg.lParam  = g_last_x + (g_last_y << 16);
            m_plugin->processMSG(&msg);
            g_Roll             = 0;
            g_Pitch            = 0;
            RightButtonPressed = true;
        }
    }
    else if (ev->type() == QEvent::MouseButtonRelease)
    {
        if (m_usingWindowProc)
            return QObject::eventFilter(obj, ev);
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(ev);
        if (mouseEvent->button() == Qt::MouseButton::LeftButton)
        {
            MSG msg     = {0};
            msg.hwnd    = m_hwnd;
            msg.message = WM_LBUTTONUP;
            //printf("Mouse button release\n");
            msg.lParam = g_last_x + (g_last_y << 16);
            m_plugin->processMSG(&msg);
            LeftButtonPressed = false;
        }
        else if (mouseEvent->button() == Qt::MouseButton::RightButton)
        {
            MSG msg     = {0};
            msg.hwnd    = m_hwnd;
            msg.message = WM_RBUTTONUP;
            msg.lParam  = g_last_x + (g_last_y << 16);
            m_plugin->processMSG(&msg);
            RightButtonPressed = false;
        }
    }
    else if (ev->type() == QEvent::Wheel)
    {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(ev);
        MSG          msg        = {0};
        msg.hwnd                = m_hwnd;
        msg.message             = WM_MOUSEWHEEL;

        if (wheelEvent->angleDelta().ry() > 0)
            msg.wParam = 10 << 16;
        else
            msg.wParam = -10 << 16;

        m_plugin->processMSG(&msg);
    }
    else if (ev->type() == QEvent::Paint)
    {
    }
#endif
    return QObject::eventFilter(obj, ev);
}

#ifdef _WIN32
void cpRenderWindow::localMessage(MSG& msg)
{
    if (!m_usingWindowProc)
        return;

    static bool LeftButtonPressed  = false;
    static bool RightButtonPressed = false;
    int         xPos;
    int         yPos;

    switch (msg.message)
    {
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        switch (msg.message)
        {
        case WM_RBUTTONDOWN:
            g_last_x           = LOWORD(msg.lParam);
            g_last_y           = HIWORD(msg.lParam);
            RightButtonPressed = true;
            break;
        case WM_RBUTTONUP:
            RightButtonPressed = false;
            break;
        case WM_LBUTTONDOWN:
            g_last_x          = LOWORD(msg.lParam);
            g_last_y          = HIWORD(msg.lParam);
            LeftButtonPressed = true;
            break;
        case WM_LBUTTONUP:
            LeftButtonPressed = false;
            break;
        }

        msg.lParam = g_last_x + (g_last_y << 16);
        m_plugin->processMSG(&msg);
        break;
    case WM_MOUSEMOVE:
        xPos = LOWORD(msg.lParam);
        yPos = HIWORD(msg.lParam);
        if (LeftButtonPressed)
        {
            if ((g_last_x - xPos) == 0)
                g_Roll = g_Roll;
            else if ((g_last_x - xPos) > 0)
                g_Roll++;
            else
                g_Roll--;

            if ((g_last_y - yPos) == 0)
                g_Pitch = g_Pitch;
            else if ((g_last_y - yPos) > 0)
                g_Pitch--;
            else
                g_Pitch++;

            if (g_Roll > 1000)
                g_Roll = 1000;
            else if (g_Roll < -1000)
                g_Roll = -1000;

            if (g_Pitch > 150)
                g_Pitch = 150;
            else if (g_Pitch < -150)
                g_Pitch = -150;

            msg.lParam = g_Roll + (g_Pitch << 16);
            msg.wParam = xPos + (yPos << 16);

            m_plugin->processMSG(&msg);

            g_last_x = xPos;
            g_last_y = yPos;
        }
        else if (RightButtonPressed)
        {
            msg.lParam = xPos + (yPos << 16);
            m_plugin->processMSG(&msg);
            g_last_x = xPos;
            g_last_y = yPos;
        }
        break;
    }
}
#endif
//===========================================================

void cp3DModelView::Clean3DModelView()
{
    // remove the Viewer plugin we used first
    if (m_plugin)
    {
        m_plugin->CloseView();
        delete m_plugin;
        m_plugin = NULL;
    }

    // remove the data loaded plugin we used last
    if (m_plugin_loader)
    {
        delete m_plugin_loader;
        m_plugin_loader = NULL;
    }
}

cp3DModelView::~cp3DModelView()
{
    Clean3DModelView();
}

void cp3DModelView::setManualViewFlip(int mode)
{
    if (m_renderview)
    {
        m_renderview->SetManualRenderFlip(mode);
    }
}

void cp3DModelView::OnToolBarClicked()
{
    if (m_toolBar->isVisible())
        m_toolBar->hide();
    else
        m_toolBar->show();
}

void cp3DModelView::OnShowOptions()
{
    m_showViewOptions ^= 1;
    m_OptionsButton->setText(!m_showViewOptions ? SHOW_VIEW_OPTIONS : HIDE_VIEW_OPTIONS);
#ifdef _WIN32
    MSG msg     = {0};
    msg.hwnd    = m_hwnd;
    msg.message = WM_COMMAND;
    msg.lParam  = m_showViewOptions;
    m_plugin->processMSG(&msg);
#endif
}

// This is only signaled when we are viewing a 3D model diff
void cp3DModelView::OnModelViewKeyPressed(int key)
{
    if (!m_renderview)
        return;
    if (!custTitleBar)
        return;
    if (key == QT_KEY_SPACE)
    {
        if (m_renderview->m_viewingOriginalModel)
            custTitleBar->setTitle("3D Model Diff View: Original");
        else
            custTitleBar->setTitle("3D Model Diff View: Processed");
    }
}

cp3DModelView::cp3DModelView(const QString filePathName, const QString filePathName2, const QString Title, QWidget* parent)
    : acCustomDockWidget(filePathName, parent)
{
    m_parent        = parent;
    m_fileName      = filePathName;
    m_statusBar     = NULL;
    m_hwnd          = 0L;
    m_plugin        = NULL;
    m_plugin_loader = NULL;

    //qApp->installEventFilter(this);

    QFile     f(filePathName);
    QFileInfo fileInfo(f.fileName());
    m_tabName = fileInfo.fileName();

    setWindowTitle(m_tabName);

    //===============
    // Main Widget
    //===============
    m_newWidget = new QWidget(parent);
    if (!m_newWidget)
    {
        PrintInfo("Error: creating main widget\n");
        return;
    }

    connect(custTitleBar, SIGNAL(ToolBarCliked()), this, SLOT(OnToolBarClicked()));

    //===============
    // Tool Bar
    //===============
    m_toolBar = new QToolBar("Tools");
    m_toolBar->setStyleSheet("QToolBar{spacing: 0px;} QToolButton {width:15px;} ");
    m_toolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolBar->setMaximumHeight(25);

    m_showViewOptions = false;
    if (g_Application_Options.m_GLTFRenderWith != C_Application_Options::RenderModelsWith::glTF_Vulkan)
    {
        m_OptionsButton = new QPushButton(!m_showViewOptions ? SHOW_VIEW_OPTIONS : HIDE_VIEW_OPTIONS);
        if (m_OptionsButton)
        {
            m_OptionsButton->setStatusTip(tr("Toggle Display Options"));
            connect(m_OptionsButton, SIGNAL(released()), this, SLOT(OnShowOptions()));
        }

        m_toolBar->addWidget(m_OptionsButton);
    }

    m_toolBar->hide();

    //===============
    // Status Bar
    //===============
    m_statusBar = new QStatusBar(this);
    m_statusBar->setStyleSheet("QStatusBar{border-top: 1px outset grey; border-bottom: 1px outset grey;}");
    m_statusBar->setMaximumHeight(25);
    m_statusBar->hide();

    // Layout Toobar
    QHBoxLayout* hlayout2 = new QHBoxLayout;
    hlayout2->setSpacing(0);
    hlayout2->setMargin(0);
    hlayout2->setContentsMargins(0, 0, 0, 0);
    hlayout2->addWidget(m_toolBar, 0);

    void*             msgHandler     = NULL;
    cpMainComponents* mainComponents = NULL;
    if (m_parent)
    {
        // assign msg handler
        QString ClassName = m_parent->metaObject()->className();
        if (ClassName.compare("cpMainComponents") == 0)
        {
            mainComponents = (cpMainComponents*)m_parent;
            msgHandler     = (void*)(mainComponents->PrintStatus);
        }
    }

    m_layout = new QGridLayout(m_newWidget);

    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addLayout(hlayout2, 0, 0);

    try
    {
        // Load the Model Data
        QFileInfo  fileInfo(filePathName);
        QString    EXT   = fileInfo.suffix();
        QByteArray ba    = EXT.toUpper().toLatin1();
        char*      c_ext = ba.data();

        if (((strcmp(c_ext, "GLTF") == 0) || (strcmp(c_ext, "BIN") == 0)))
        {
            m_isviewingDX12 = true;
            if (g_Application_Options.getGLTFRender() == C_Application_Options::RenderModelsWith::glTF_OpenGL)
                g_Application_Options.setGLTFRender(C_Application_Options::RenderModelsWith::glTF_DX12_EX);
        }
        else
        {
            m_isviewingDX12 = false;
            if (g_Application_Options.getGLTFRender() == C_Application_Options::RenderModelsWith::glTF_DX12_EX ||
                g_Application_Options.getGLTFRender() == C_Application_Options::RenderModelsWith::glTF_Vulkan)
                g_Application_Options.setGLTFRender(C_Application_Options::RenderModelsWith::glTF_OpenGL);
        }

        std::string srcglTFfile1 = filePathName.toStdString();
        std::string srcglTFfile2 = filePathName2.toStdString();

        // for gltf case: detect if is gltf draco the call decode before loading
        if (((strcmp(c_ext, "GLTF") == 0) || (strcmp(c_ext, "BIN") == 0)))
        {
            if ((strcmp(c_ext, "BIN") == 0))
            {
                if (filePathName2.contains(".gltf") || filePathName2.contains(".GLTF"))
                {
                    srcglTFfile1 = "";
                }
            }

            if (isGLTFDracoFile(srcglTFfile1))
            {
                std::size_t dotPos      = srcglTFfile1.rfind('.');
                std::string tempdstFile = srcglTFfile1.substr(0, dotPos) + "_tmpdecoded.glTF";
                if (!decompressglTFfile(srcglTFfile1, tempdstFile, g_CmdPrams.use_Draco_Encode, g_CmdPrams.CompressOptions))
                {
                    if (mainComponents)
                    {
                        if (mainComponents->m_CompressStatusDialog)
                        {
                            mainComponents->m_CompressStatusDialog->showOutput();
                            mainComponents->m_CompressStatusDialog->raise();
                        }
                    }
                    PrintInfo("Error: Decoding glTF file [%s] failed. Only single \"meshes\" decode support for now.\n", srcglTFfile1.c_str());
                    return;
                }

                srcglTFfile1 = tempdstFile;
            }

            if (isGLTFDracoFile(srcglTFfile2))
            {
                std::size_t dotPos      = srcglTFfile2.rfind('.');
                std::string tempdstFile = srcglTFfile2.substr(0, dotPos) + "_tmpdecoded.glTF";
                if (!decompressglTFfile(srcglTFfile2, tempdstFile, g_CmdPrams.use_Draco_Encode, g_CmdPrams.CompressOptions))
                {
                    if (mainComponents)
                    {
                        if (mainComponents->m_CompressStatusDialog)
                        {
                            mainComponents->m_CompressStatusDialog->showOutput();
                            mainComponents->m_CompressStatusDialog->raise();
                        }
                    }
                    PrintInfo("Error: Decoding glTF file [%s] failed. Only single \"meshes\" decode support for now.\n", srcglTFfile2.c_str());
                    return;
                }

                srcglTFfile2 = tempdstFile;
            }
        }

        // We will only use GLTF loader when not using OpenGL
        if (((strcmp(c_ext, "GLTF") == 0) || (strcmp(c_ext, "BIN") == 0)) &&
            (g_Application_Options.getGLTFRender() != C_Application_Options::RenderModelsWith::glTF_OpenGL))
            m_plugin_loader = reinterpret_cast<PluginInterface_3DModel_Loader*>(g_pluginManager.GetPlugin("3DMODEL_LOADER", "GLTF"));
        else if (strcmp(c_ext, "DRC") == 0)
            m_plugin_loader = reinterpret_cast<PluginInterface_3DModel_Loader*>(g_pluginManager.GetPlugin("3DMODEL_LOADER", "DRC"));
        else
            m_plugin_loader = reinterpret_cast<PluginInterface_3DModel_Loader*>(
                g_pluginManager.GetPlugin("3DMODEL_LOADER", g_Application_Options.getUseAssimp() ? "ASSIMP" : "OBJ"));

        if (m_plugin_loader)
        {
            m_plugin_loader->TC_PluginSetSharedIO(g_GUI_CMIPS);
            int result = 0;
            if ((strcmp(c_ext, "BIN") == 0))
            {
                if (filePathName2.contains(".gltf") || filePathName2.contains(".GLTF"))
                    result = m_plugin_loader->LoadModelData(srcglTFfile2.c_str(), "", &g_pluginManager, msgHandler, &ProgressCallback);
                else
                {
                    if (mainComponents)
                    {
                        if (mainComponents->m_CompressStatusDialog)
                        {
                            mainComponents->m_CompressStatusDialog->showOutput();
                            mainComponents->m_CompressStatusDialog->raise();
                        }
                    }
                    PrintInfo("Error: Please add related gltf file then view bin file from there. View bin alone is not supported!\n");
                    return;
                }
            }
            else
            {
                if (strcmp(c_ext, "DRC") == 0)
                {
                    CMP_DracoOptions DracoOptions;
                    DracoOptions.output = "";
                    result              = m_plugin_loader->LoadModelData(
                        srcglTFfile1.c_str(), srcglTFfile2.size() > 0 ? srcglTFfile2.c_str() : "", &g_pluginManager, &DracoOptions, &ProgressCallback);
                }
                else
                {
                    result = m_plugin_loader->LoadModelData(
                        srcglTFfile1.c_str(), srcglTFfile2.size() > 0 ? srcglTFfile2.c_str() : "", &g_pluginManager, msgHandler, &ProgressCallback);
                }
            }

            if (result != 0)
            {
                if (mainComponents)
                {
                    if (mainComponents->m_CompressStatusDialog)
                    {
                        mainComponents->m_CompressStatusDialog->showOutput();
                        mainComponents->m_CompressStatusDialog->raise();
                    }
                }
                PrintInfo("Error: Failed in loading model data.\n");
                return;
            }

            char* viewer_type = nullptr;

            switch (g_Application_Options.getGLTFRender())
            {
            case C_Application_Options::RenderModelsWith::glTF_DX12_EX:
                viewer_type = "DX12_EX";
                if (isDX12Supported())
                {
                    m_plugin = reinterpret_cast<PluginInterface_3DModel*>(g_pluginManager.GetPlugin("3DMODEL_VIEWER", viewer_type));
                }
                else
                {
                    PrintInfo("Error: Selected Viewer type is not supported!\n");
                    return;
                }
                break;
            case C_Application_Options::RenderModelsWith::glTF_OpenGL:
                viewer_type = "OPENGL";
                m_plugin    = reinterpret_cast<PluginInterface_3DModel*>(g_pluginManager.GetPlugin("3DMODEL_VIEWER", viewer_type));
                break;
            case C_Application_Options::RenderModelsWith::glTF_Vulkan:
                viewer_type = "VULKAN";
                m_plugin    = reinterpret_cast<PluginInterface_3DModel*>(g_pluginManager.GetPlugin("3DMODEL_VIEWER", viewer_type));
                break;
            default: {
                if (mainComponents)
                {
                    if (mainComponents->m_CompressStatusDialog)
                    {
                        mainComponents->m_CompressStatusDialog->showOutput();
                        mainComponents->m_CompressStatusDialog->raise();
                    }
                }
                PrintInfo("Error: Selected Viewer type is not supported!\n");
                return;
            }
            }

            this->m_CustomTitle = Title;
            if (m_plugin)
            {
                m_plugin->TC_PluginSetSharedIO(g_GUI_CMIPS);
                void* data = m_plugin_loader->GetModelData();

                if (data && m_isviewingDX12)
                {
                    if ((strcmp(c_ext, "BIN") == 0))
                        ((CMP_GLTFCommon*)data)->isBinFile = true;
                    else
                        ((CMP_GLTFCommon*)data)->isBinFile = false;
                }

                void* result = NULL;
                if (strcmp(viewer_type, "VULKAN") == 0)
                {
                    result = m_plugin->CreateView(data, parent->width(), parent->height(), NULL, &g_pluginManager, msgHandler, &ProgressCallback);
                    if (result)
                    {
                        // Create Empty window
                        m_renderview = new cpRenderWindow();
                        if (!m_renderview)
                        {
                            if (mainComponents)
                            {
                                if (mainComponents->m_CompressStatusDialog)
                                {
                                    mainComponents->m_CompressStatusDialog->showOutput();
                                    mainComponents->m_CompressStatusDialog->raise();
                                }
                            }
                            PrintInfo("Error: creating render widget.\n");
                            return;
                        }

#ifdef _WIN32
                        WId wid = createNativeWindowView("Vulkan Viewer", parent->width(), parent->height());
                        if (wid)
                        {
                            connect(&static_winMsgHandler, SIGNAL(signalMessage(MSG&)), m_renderview, SLOT(localMessage(MSG&)));
                            m_renderview->m_usingWindowProc = true;
                            connect(m_renderview, SIGNAL(signalModelKeyPressed(int)), this, SLOT(OnModelViewKeyPressed(int)));

                            HWND hwnd = reinterpret_cast<HWND>(wid);
                            if (m_plugin->ShowView(hwnd) != NULL)
                            {
                                QVBoxLayout* widgetLayout  = new QVBoxLayout();
                                QWindow*     foreignWindow = QWindow::fromWinId(wid);
                                QWidget*     newWindow     = QWidget::createWindowContainer(foreignWindow, m_renderview);
                                newWindow->installEventFilter(m_renderview);
                                widgetLayout->addWidget(newWindow);
                                m_renderview->setplugin(m_plugin);
                                m_renderview->setLayout(widgetLayout);
                                m_viewOpen = true;
                            }
                            else
                            {
                                m_viewOpen = false;
                            }

                            m_renderview->setView(m_viewOpen);
                        }
                        else
                        {
                            m_renderview->setplugin(m_plugin);
                            m_viewOpen = false;
                            m_renderview->setView(false);
                        }
#endif
                    }
                }
                else
                {
                    m_renderview = new cpRenderWindow();

                    if (!m_renderview)
                    {
                        if (mainComponents)
                        {
                            if (mainComponents->m_CompressStatusDialog)
                            {
                                mainComponents->m_CompressStatusDialog->showOutput();
                                mainComponents->m_CompressStatusDialog->raise();
                            }
                        }
                        PrintInfo("Error: creating render widget.\n");
                        return;
                    }

                    connect(m_renderview, SIGNAL(signalModelKeyPressed(int)), this, SLOT(OnModelViewKeyPressed(int)));

                    m_renderview->setplugin(m_plugin);
#ifdef _WIN32
                    m_hwnd = reinterpret_cast<HWND>(m_renderview->winId());
#endif
                    m_renderview->setHwnd(m_hwnd);
                    try
                    {
                        result = m_plugin->CreateView(data, parent->width(), parent->height(), m_renderview, &g_pluginManager, msgHandler, &ProgressCallback);
                    }
                    catch (...)
                    {
                        if (m_renderview)
                        {
                            delete m_renderview;
                            m_renderview = nullptr;
                        }
                        if (m_plugin)
                        {
                            m_plugin->CloseView();
                        }
                        throw;
                    }

                    m_viewOpen = true;
                    m_renderview->setView(true);
                }

                if (result)
                {
                    m_layout->addWidget(m_renderview, 1, 0);
                }
                else
                {
                    if (mainComponents)
                    {
                        if (mainComponents->m_CompressStatusDialog)
                        {
                            mainComponents->m_CompressStatusDialog->showOutput();
                            mainComponents->m_CompressStatusDialog->raise();
                        }
                    }
                    PrintInfo("Error: Viewer load failed.\n");
                    return;
                }
            }
        }
        else
        {
            if (mainComponents)
            {
                if (mainComponents->m_CompressStatusDialog)
                {
                    mainComponents->m_CompressStatusDialog->showOutput();
                    mainComponents->m_CompressStatusDialog->raise();
                }
            }
            PrintInfo("Error: File format not supported.\n");
            return;
        }
    }
    catch (const char* error)
    {
        if (mainComponents)
        {
            if (mainComponents->m_CompressStatusDialog)
            {
                mainComponents->m_CompressStatusDialog->showOutput();
                mainComponents->m_CompressStatusDialog->raise();
            }
        }
        Clean3DModelView();
        PrintInfo("Error: %s", error);
    }

    m_layout->addWidget(m_statusBar, 2, 0);
    m_newWidget->setLayout(m_layout);

    setWidget(m_newWidget);

    // This is used by cpMainWindow to manage dock view add and delete
    QString DockTitle = m_CustomTitle + " " + filePathName;
    custTitleBar->setTitle(DockTitle);
    custTitleBar->setToolTip(filePathName);
}
