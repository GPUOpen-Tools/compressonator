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

#include "gltf_ogldevice.h"

static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));

static PFNGLGENQUERIESPROC        glGenQueries        = reinterpret_cast<PFNGLGENQUERIESPROC>(wglGetProcAddress("glGenQueries"));
static PFNGLBEGINQUERYPROC        glBeginQuery        = reinterpret_cast<PFNGLBEGINQUERYPROC>(wglGetProcAddress("glBeginQuery"));
static PFNGLENDQUERYPROC          glEndQuery          = reinterpret_cast<PFNGLENDQUERYPROC>(wglGetProcAddress("glEndQuery"));
static PFNGLGETQUERYOBJECTIVPROC  glGetQueryObjectiv  = reinterpret_cast<PFNGLGETQUERYOBJECTIVPROC>(wglGetProcAddress("glGetQueryObjectiv"));
static PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv = reinterpret_cast<PFNGLGETQUERYOBJECTUIVPROC>(wglGetProcAddress("glGetQueryObjectuiv"));

static float g_Roll  = 0;
static float g_Pitch = 0;
static int   m_lastMouseWheelDelta, m_mouseWheelDelta = 0;

glTF_OGLDevice::glTF_OGLDevice(CMODEL_DATA model[MAX_NUM_OF_NODES], DWORD width, DWORD height, void* pluginManager, void* msghandler, QWidget* parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    if (glGenQueries && glBeginQuery && glEndQuery && glGetQueryObjectiv && glGetQueryObjectuiv)
        m_haveTimerExtnsions = true;

    m_width  = width;
    m_height = height;

    for (int i = 0; i < MAX_NUM_OF_NODES; i++)
        m_model[i] = &model[i];

    m_pitch           = g_Pitch;
    m_roll            = g_Roll;
    m_last_mouse_xpos = 0.0f;
    m_last_mouse_ypos = 0.0f;

    UI.xTrans = 0.0f;
    UI.yTrans = 0.0f;
    UI.zTrans = float(m_mouseWheelDelta);

    UI.xRotation   = g_Roll;
    UI.yRotation   = g_Pitch;
    UI.zRotation   = 0.0f;
    UI.fill        = true;
    UI.m_showimgui = false;

    // imGui default window size and pos
    m_imgui_win_size.x = 200;
    m_imgui_win_size.y = 250;
    m_imgui_win_pos.x  = 10;
    m_imgui_win_pos.y  = 10;

    m_core        = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    m_xRot        = 0.0f;
    m_yRot        = 0.0f;
    m_zRot        = 0.0f;
    m_program     = 0;
    m_transparent = false;

    m_meshdata.MeshLoaded = false;
    m_meshdata.LoadData(model[0].m_meshData[0]);
    m_meshdata.calc_bounds(m_width, m_height, model[0].m_meshData[0]);

    // Scale the view acording to bounding box
    UI.xTrans = (m_meshdata.m_centerx);
    UI.yTrans = -(m_meshdata.m_centery);
    UI.zTrans = -(max(max(m_meshdata.m_extentx, m_meshdata.m_extenty), m_meshdata.m_extentz) * 2.0f);

    // Dafault vertical sync is off
    m_bVerticalSync = false;
    VerticalSync(m_bVerticalSync);

    // Default view is normal
    m_fullScreen = false;

    // Camera Position
    m_cameraPos[0] = -(m_meshdata.m_centerx);
    m_cameraPos[1] = (m_meshdata.m_centery);
    m_cameraPos[2] = -(max(max(m_meshdata.m_extentx, m_meshdata.m_extenty), m_meshdata.m_extentz) * 2.0f);
}

glTF_OGLDevice::~glTF_OGLDevice()
{
    cleanup();
}

void glTF_OGLDevice::cleanup()
{
    if (m_program == 0)
        return;
    makeCurrent();
    m_MeshVbo.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

//===================
// Vertex Shader
//
// SHADER CODE - Attributes
// vertex
// normal
//===================
static const char* vertexShaderSource1 = GLSL(
    120, attribute vec4 vertex; attribute vec3 normal; varying vec3 vert; varying vec3 vertNormal; uniform mat4 projMatrix; uniform mat4 mvMatrix;
    uniform mat3 normalMatrix;
    void         main() {
        vert        = vertex.xyz;
        vertNormal  = normalMatrix * normal;
        gl_Position = projMatrix * mvMatrix * vertex;
    });

// Use for Code in GLObjViewer
static const char* vertexShaderSource = GLSL(
    110, attribute vec4 vertex; attribute vec3 normal; varying vec3 normal1;

    varying vec3 vert;
    varying vec3 vertNormal;
    uniform mat4 projMatrix;
    uniform mat4 mvMatrix;
    uniform mat3 normalMatrix;

    void main() {
        vert       = vertex.xyz;
        vertNormal = normalMatrix * normal;
        normal1    = normalize(vertNormal);

        gl_Position = projMatrix * mvMatrix * vertex;
    });

//===================
// Fragment Shader
//===================
static const char* fragmentShaderSource = GLSL(
    110, uniform bool useFill;

    varying highp vec3 vert;
    varying highp vec3 vertNormal;
    void               main() {
        vec3       n = normalize(vertNormal);
        highp vec3 L = normalize(gl_LightSource[0].position.xyz - vert);
        vec3       R = normalize(-reflect(L, vertNormal));
        vec3       E = normalize(-vert);

        float nDotL = max(0.0, dot(n, L));
        float nDotH = max(0.0, dot(R, E));
        float power = pow(nDotH, 0.3 * gl_FrontMaterial.shininess);

        vec4 ambient = gl_FrontLightProduct[0].ambient;

        vec4 diffuse = gl_FrontLightProduct[0].diffuse * nDotL;
        diffuse      = clamp(diffuse, 0.0, 1.0);

        vec4 specular = gl_FrontLightProduct[0].specular * power;
        specular      = clamp(specular, 0.0, 1.0);

        highp vec4 basecolor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        if (useFill)
            basecolor = gl_FrontLightModelProduct.sceneColor;

        gl_FragColor   = basecolor + ambient + diffuse + specular;
        gl_FragColor.a = 1.0f;
    });

// =================================== Done Shader Code ========================================

/*! Generate display list for checkered floor geometry */
void glTF_OGLDevice::DrawPlane()
{
    float MinPos         = -20.0f;
    float MaxPos         = 20.0f;
    float MajorGridSteps = 0.1f;

    m_world.setToIdentity();
    m_world.translate(0, 0, 0);

    // Do Rotation
    m_world.rotate(m_xRot / 16.0f, 1, 0, 0);   // Along X
    m_world.rotate(-m_yRot / 16.0f, 0, 1, 0);  // Along Y
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);   // Along Z

    glColor3f(0.3f, 0.3f, 0.3f);

    glBegin(GL_LINES);
    for (GLfloat i = MinPos; i <= MaxPos; i += MajorGridSteps)
    {
        glVertex3f(i, 0, MaxPos);
        glVertex3f(i, 0, MinPos);
        glVertex3f(MaxPos, 0, i);
        glVertex3f(MinPos, 0, i);
    }
    glEnd();
}

void glTF_OGLDevice::SetLighting(void)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_NORMALIZE);

    // -------------------------------------------
    // Lighting parameters:
    // -------------------------------------------
    GLfloat light_pos[]     = {m_lightPos_x, m_lightPos_y, m_lightPos_z, 1.0f};
    GLfloat light_ambient[] = {0.8f, 0.8f, 0.8f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

    if (UI.fill)
    {
        GLfloat light_diffuse[]  = {0.8f, 0.8f, 0.8f, 1.0f};
        GLfloat light_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    }
}

void glTF_OGLDevice::setupVertexAttribs()
{
    m_MeshVbo.bind();
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);                                             // Vertices
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));  // Normals
    m_MeshVbo.release();
}

void glTF_OGLDevice::clear_vertex_buffer_object()
{
    makeCurrent();
    m_MeshVbo.destroy();
    doneCurrent();
}

void glTF_OGLDevice::setup_vertex_buffer_object()
{
    m_MeshVbo.create();
    m_MeshVbo.bind();
    m_MeshVbo.allocate(m_meshdata.constVertexData(), m_meshdata.count() * sizeof(GLfloat));
    m_MeshVbo.release();
}

void glTF_OGLDevice::initializeGL()
{
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc   = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc     = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_useFillLoc      = m_program->uniformLocation("useFill");

    // Create a vertex array object
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    setup_vertex_buffer_object();

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();

    // Set camera position
    m_camera.setToIdentity();
    m_camera.translate(m_cameraPos[0], m_cameraPos[1], m_cameraPos[2]);
    m_program->release();

    // Default light Position
    m_lightPos_x = 0;
    m_lightPos_y = 0;
    m_lightPos_z = 70;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);

    int numVertices = (int)m_meshdata.m_basemesh.vertices.size();

    bool isCW = true;
    for (int i = 0; i < numVertices; i++)
    {
        if (m_meshdata.m_basemesh.vertices[i].nz > 0)
            isCW = false;
    }

    if (isCW)
    {
        glFrontFace(GL_CW);

        // Update light Z Position
        m_lightPos_z = -70;
    }
    else
    {
        glFrontFace(GL_CCW);

        // Update light Z Position
        m_lightPos_z = 70;
    }

    SetLighting();

    //if (UI.m_showimgui)
    //{
    QImGUI_OpenGLWindowWrapper* window = new QImGUI_OpenGLWidgetWindowWrapper((QWidget*)this);
    if (window)
        m_ImGuiRenderer.initialize(window);
    //}

    m_frameCount = 0;
    m_frameRate  = 0.0f;
    m_elapsedTimer.start();
    DeviceReady = true;
}

//===============
// Frame Render
//===============

void glTF_OGLDevice::paintGL()
{
    if (!DeviceReady)
        return;

    GLuint query;
    GLuint elapsed_time;

    //==================================
    // Calculate Frame Rates
    //==================================
    m_frameCount++;

    // Get the number of ms since last timer reset
    qint64 elapsedTime = m_elapsedTimer.elapsed();
    if (elapsedTime > 1000)
    {
        m_elapsedTimer.restart();
        m_frameRate = 1000.0f / m_frameCount;

        if (m_frameRateMin > m_frameRate)
            m_frameRateMin = m_frameRate;

        m_frameCount = 0;
    }

    // Do a timer query one everytime the frame counter get reset
    // see above the the rate (Set to 1 Second)
    if (m_haveTimerExtnsions && (m_frameCount == 0))
    {
        // now measure accurate timing value
        glGenQueries(1, &query);
        glBeginQuery(GL_TIME_ELAPSED, query);
    }

    //==================================
    // set Scene view port
    //==================================
    glViewport(0, 0, m_width, m_height);

    //=====================
    // Clear Buffer
    //=====================
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Set lighting
    SetLighting();

    // Set camera
    m_camera.setToIdentity();
    m_camera.translate(m_cameraPos[0], m_cameraPos[1], m_cameraPos[2]);

    //==================================
    // Optional user Settings and Stats
    //==================================
    if (UI.m_showimgui)
    {
        static double RenderingTime;
        m_ImGuiRenderer.newFrame();
        {
            ImGui::Begin("Stats");  //, NULL, ImGuiWindowFlags_ShowBorders);
            m_imgui_win_size = ImGui::GetWindowSize();
            m_imgui_win_pos  = ImGui::GetWindowPos();

            if (ImGui::CollapsingHeader("Info", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Resolution       : %ix%i", m_width, m_height);
                ImGui::Text("Load Time  %d ms", m_model[0]->m_LoadTime);
            }

            if (ImGui::CollapsingHeader("Profiler", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("%d vertices  %d indices", m_meshdata.vertexCount(), m_meshdata.indexCount());
                ImGui::Text("Frame Rate %.3f ms/frame", m_frameRate);
                /***
                ImGui::Text("Min        %.3f ms/frame", m_frameRateMin);
                ***/
                ImGui::Text("Draw Time  %.3f ms", m_draw_time);
            }

            if (ImGui::CollapsingHeader("Options", ImGuiTreeNodeFlags_DefaultOpen))
            {
                /* Disabled feature for V3.0 will be enabled in V3.1
                static int lod = 0;
                static int last_lod = 0;
                ImGui::SliderInt("LOD", &lod, 0, 9);
                if (last_lod != lod)
                {
                    last_lod = lod;
                    CMP_Mesh mesh = m_meshdata.optimize(m_meshdata.m_basemesh, lod);
                    m_meshdata.LoadIndex(mesh);
                }
                */

                ImGui::Checkbox("Fill", &UI.fill);

                if (ImGui::Checkbox("VSync", &m_bVerticalSync))
                {
                    VerticalSync(m_bVerticalSync);
                }

                // if (ImGui::Checkbox("Full Screen", &m_fullScreen))
                // {
                //     SetFullScreen(m_fullScreen);
                // }

                // ImGui::SliderFloat("Light x", &m_lightPos_x, 0.0f, 70.0f);
                // ImGui::SliderFloat("Light y", &m_lightPos_y, 0.0f, 70.0f);
                // ImGui::SliderFloat("Light z", &m_lightPos_z, 0.0f, 70.0f);
                // //
                // ImGui::SliderFloat("Camera x", &m_cameraPos[0], -1.0f, 1.0f);
                // ImGui::SliderFloat("Camera y", &m_cameraPos[1], -1.0f, 1.0f);
                // ImGui::SliderFloat("Camera z", &m_cameraPos[2], -1.0f, 1.0f);
            }
            ImGui::End();
        }
    }

    m_xRot = (UI.xRotation * 100);
    m_yRot = (UI.yRotation * 100);

    m_world.setToIdentity();

    // Do translation
    m_world.translate(UI.xTrans, UI.yTrans, UI.zTrans);

    // Do Rotation
    m_world.rotate(m_xRot / 16.0f, 1, 0, 0);   // Along X
    m_world.rotate(-m_yRot / 16.0f, 0, 1, 0);  // Along Y
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);   // Along Z

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    QMatrix3x3 normalMatrix = m_world.normalMatrix();

    m_program->bind();
    m_program->setUniformValue(m_useFillLoc, UI.fill);
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    if (UI.fill)
        glDrawElements(GL_TRIANGLES, m_meshdata.indexCount(), GL_UNSIGNED_INT, m_meshdata.constIndexData());
    else
        glDrawElements(GL_LINES, m_meshdata.indexCount(), GL_UNSIGNED_INT, m_meshdata.constIndexData());

    // DrawPlane();

    m_program->release();

    if (UI.m_showimgui)
    {
        m_ImGuiRenderer.Draw();
    }

    update();

    if (m_haveTimerExtnsions && (m_frameCount == 0))
    {
        glEndQuery(GL_TIME_ELAPSED);

        // retrieving the recorded elapsed time
        // wait until the query result is available
        int done     = 0;
        int downtime = 100000;
        while (!done && downtime)
        {
            glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &done);
            downtime--;
        }

        // get the query result
        glGetQueryObjectuiv(query, GL_QUERY_RESULT, &elapsed_time);

        // Scale the time to milli seconds
        m_draw_time = (elapsed_time / 1000000.0);
    }
}

void glTF_OGLDevice::resizeView(int width, int height)
{
    if (height == 0)
        height = 1;
    m_width  = width;
    m_height = height;
    setFixedSize(width, height);

    float aspect = (width / height);

    m_proj.setToIdentity();

    m_proj.perspective(45.0f, GLfloat(width) / GLfloat(height), 0.1f, 10000.0f);
}

//--------------------------------------------------------------------------------------
// OnCreate
//--------------------------------------------------------------------------------------

int glTF_OGLDevice::OnCreate()
{
    return 0;
}

//--------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------
void glTF_OGLDevice::OnDestroy()
{
    DeviceReady = false;
}

//--------------------------------------------------------------------------------------
// OnRender
//--------------------------------------------------------------------------------------
void glTF_OGLDevice::OnRender()
{
}

void glTF_OGLDevice::OnResize(DWORD width, DWORD height)
{
    resizeView(width, height);
}

//--------------------------------------------------------------------------------------
// OnEvent from Host Application
//--------------------------------------------------------------------------------------
bool glTF_OGLDevice::OnEvent(MSG msg)
{
    static bool Mouse_RBD = false;
    // Always update mouse pos when mouse clicked.
    if (msg.message == WM_LBUTTONDOWN)
    {
        m_mouse_press_xpos = (signed short)(msg.wParam);
        m_mouse_press_ypos = (signed short)(msg.wParam >> 16);
    }

    // Check if mouse is inside imGUI window
    if (UI.m_showimgui)
    {
        if ((m_mouse_press_xpos > m_imgui_win_pos.x) && (m_mouse_press_xpos < m_imgui_win_pos.x + m_imgui_win_size.x + 15))
        {
            if ((m_mouse_press_ypos > m_imgui_win_pos.y) && (m_mouse_press_ypos < m_imgui_win_pos.y + m_imgui_win_size.y + 15))
            {
                return true;
            }
        }
    }

    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();

    // Process Events for Mouse based Mesh pocessing
    switch (msg.message)
    {
    case WM_RBUTTONUP: {
        if (keyMod.testFlag(Qt::ShiftModifier))
        {
            UI.zTrans    = 0.0f;
            UI.xRotation = 0.0f;
            UI.yRotation = 0.0f;
        }
        Mouse_RBD = false;
        break;
    }
    case WM_RBUTTONDOWN: {
        float mouse_x = (signed short)(msg.lParam);
        float mouse_y = (signed short)(msg.lParam >> 16);
        m_lastPos.setX(mouse_x);
        m_lastPos.setY(mouse_y);
        Mouse_RBD = true;
        break;
    }
    case WM_MOUSEWHEEL: {
        float m_DeltaScale = keyMod.testFlag(Qt::ShiftModifier) ? 1.0f : 5.0f;
        m_mouseWheelDelta  = (signed short)(msg.wParam >> 16);
        UI.zTrans += float(m_mouseWheelDelta) / m_DeltaScale;
        break;
    }
    case WM_MOUSEMOVE: {
        if (Mouse_RBD)
        {
            float mouse_x = (signed short)(msg.lParam);
            float mouse_y = (signed short)(msg.lParam >> 16);
            float dx      = mouse_x - m_lastPos.x();
            float dy      = mouse_y - m_lastPos.y();

            UI.yTrans -= (dy * 0.01);
            UI.xTrans += (dx * 0.01);
            m_lastPos.setX(mouse_x);
            m_lastPos.setY(mouse_y);
        }
        else
        {
            m_pitch = (signed short)(msg.lParam);
            m_roll  = (signed short)(msg.lParam >> 16);

            g_Roll       = m_roll;
            g_Pitch      = m_pitch;
            UI.yRotation = m_pitch;
            UI.xRotation = m_roll;
        }

        break;
    }

    case WM_COMMAND: {
        int show = (int)(msg.lParam);
        ShowOptions(show ? true : false);
        break;
    }
    }

    return true;
}

//--------------------------------------------------------------------------------------
// SetFullScreen
//--------------------------------------------------------------------------------------
void glTF_OGLDevice::SetFullScreen(bool fullscreen)
{
    if (fullscreen)
        this->showFullScreen();
    else
        this->showNormal();
}

void glTF_OGLDevice::ShowOptions(bool onshow)
{
    UI.m_showimgui = onshow;
}

void glTF_OGLDevice::VerticalSync(bool enable)
{
    if (wglSwapIntervalEXT)
    {
        wglSwapIntervalEXT(enable ? 1 : 0);
        m_bVerticalSync = enable;
    }
}
