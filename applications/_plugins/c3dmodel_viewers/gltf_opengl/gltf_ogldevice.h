// Copyright(c) 2017-2024 Advanced Micro Devices, Inc.All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef _GLTF_OGLDEVICE_H
#define _GLTF_OGLDEVICE_H

#include "userinterface.h"
#include "pluginmanager.h"
#include "compressonator.h"

#include "QtWidgets/qwidget.h"

// -- Qt Open GL Support
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector2D>
#include <QEvent>
#include <QOpenGLBuffer>
#include <QPropertyAnimation>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QElapsedTimer>
#include <QPainter>
#include <QOpenGLVertexArrayObject>
#include <QApplication>

#include <imgui_opengl.h>
#include "imgui.h"

#include "modeldata.h"
#include "misc.h"

#define MAX_NUM_OF_NODES 2

#include <QtOpenGL/qgl.h>  // Uses Native Window Context
#include "imgui_openglrenderer.h"

#include "meshdata.h"
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <qopenglext.h>

// Used for in app Shader Code
#ifndef GLSL
#define GLSL(version, A) "#version " #version "\n" #A
#endif

// Used to enable or disable Vertical Sync
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC)(GLint);

struct TimeStamp
{
    std::string m_label;
    float       m_milliseconds;
};

class glTF_OGLDevice : public QGLWidget
{
public:
    PluginManager* m_pluginManager;
    bool           DeviceReady = false;

    glTF_OGLDevice(CMODEL_DATA model[MAX_NUM_OF_NODES], DWORD width, DWORD height, void* pluginManager, void* msghandler, QWidget* parent);
    ~glTF_OGLDevice();

    int OnCreate();

    void OnDestroy();
    void OnRender();
    bool OnEvent(MSG msg);
    void OnResize(DWORD Width, DWORD Height);

    void SetFullScreen(bool fullscreen);
    void ShowOptions(bool onshow);

    bool isTransparent()
    {
        return m_transparent;
    }
    void setTransparent(bool t)
    {
        m_transparent = t;
    }

    void setup_vertex_buffer_object();
    void clear_vertex_buffer_object();

    void DrawPlane();

protected:
    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;

private:
    void resizeView(int w, int h);

    // Contains Mesh Data loaded from File
    CMODEL_DATA* m_model[MAX_NUM_OF_NODES];

    // Scene window size
    int m_width;
    int m_height;

    // current imGUI window position and size
    ImVec2 m_imgui_win_size;
    ImVec2 m_imgui_win_pos;

    // Mouse Event and Data
    int   m_mouse_press_xpos, m_mouse_press_ypos;
    float m_roll;
    float m_pitch;
    float m_last_mouse_xpos;
    float m_last_mouse_ypos;

    float rotateX = 0;
    float rotateY = 0;
    float rotateZ = 0;

    ImGuiRenderer m_ImGuiRenderer;

    // User IO interface settings
    UserInterface UI;

    // Code for Timing
    int           m_frameCount;
    float         m_frameRate;
    float         m_frameRateMin = FLT_MAX;
    QElapsedTimer m_elapsedTimer;

    void cleanup();
    void setupVertexAttribs();

    bool m_core;
    int  m_xRot;
    int  m_yRot;
    int  m_zRot;

    QPoint   m_lastPos;
    MeshData m_meshdata;

    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_normalMatrixLoc;
    int m_useFillLoc;

    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;

    bool m_transparent;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer            m_MeshVbo;
    QOpenGLShaderProgram*    m_program;

    // VSync
    bool m_bVerticalSync;
    void VerticalSync(bool enable);

    // Full Screen Mode
    bool m_fullScreen;

    // Lighting
    float m_lightPos_x;
    float m_lightPos_y;
    float m_lightPos_z;
    void  SetLighting(void);

    // Camera
    float m_cameraPos[3];

    // GPU Timer
    double m_draw_time          = 0;
    bool   m_haveTimerExtnsions = false;
};

#endif
