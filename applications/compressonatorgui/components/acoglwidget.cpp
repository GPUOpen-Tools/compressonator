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

#include "acoglwidget.h"
#include <math.h>
#include "qopengltexture.h"

#define GL_COMPRESSED_RED_RGTC1 0x8DBB  // Also known as: DXT_BC5, LATC, RGTC, 3Dc, ATI2
#define GL_COMPRESSED_SIGNED_RED_RGTC1 0x8DBC
#define GL_COMPRESSED_RG_RGTC2 0x8DBD
#define GL_COMPRESSED_SIGNED_RG_RGTC2 0x8DBE

#define GL_COMPRESSED_RGBA_BPTC_UNORM 0x8E8C  // Also known as: DXT_BC7
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM 0x8E8D
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT 0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT 0x8E8F

extern PluginManager g_pluginManager;

acOGLWidget::acOGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , geometries(0)
{
}

acOGLWidget::~acOGLWidget()
{
    // Make sure the context is current when deleting the buffers.
    makeCurrent();
    delete geometries;
    doneCurrent();
}

#if 0
GLenum  acOGLWidget::MIP2OLG_Format(MipSet *m_MipSet) {
    GLenum m_GLnum;
    switch (m_MipSet->m_format) {
    case AMD_TC_FORMAT_BC7:
        m_GLnum = GL_COMPRESSED_RGBA_BPTC_UNORM;
        break;
    case AMD_TC_FORMAT_BC6H:
        m_GLnum = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
        break;
    default:
        m_GLnum = GL_INVALID_ENUM;
    }
    return m_GLnum;
}

bool  acOGLWidget::loadMIPImage(MipSet *m_MipSet, CMIPS *cMips, int MIPLevel = 0) {
    // Get First <IP Level
    MipLevel* pInMipLevel = cMips->GetMipLevel(m_MipSet, MIPLevel, 0);
    if (!pInMipLevel) return false;

    BYTE *pData = pInMipLevel->m_pbData;
    if (!pData) return false;

    GLenum m_GLnum = MIP2OLG_Format(m_MipSet);

    if (m_GLnum == GL_INVALID_ENUM) return false;

    glCompressedTexImage2D( GL_TEXTURE_2D,
                            0,
                            m_GLnum,
                            (unsigned int)pInMipLevel->m_nWidth,
                            (unsigned int)pInMipLevel->m_nHeight,
                            0,
                            pInMipLevel->m_dwLinearSize,
                            pData);

    m_ImageH = m_MipSet->m_nHeight;
    m_ImageW = m_MipSet->m_nWidth;

    return true;
}
#endif

//not in used- the function below not working
//used qopenglwidget grabframebuffer for offscreen render
void acOGLWidget::drawOffscreen()
{
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(0);

    QWindow window;
    window.setSurfaceType(QWindow::OpenGLSurface);
    window.setFormat(format);
    window.create();

    QOpenGLContext context;
    context.setFormat(format);
    if (!context.create())
        qFatal("Cannot create the requested OpenGL context!");
    context.makeCurrent(&window);

    //make sure it is current for painting
    if (!m_isInitialized)
    {
        initializeGL();
        resizeGL(width(), height());
    }
    if (!m_fbo || m_fbo->width() != width() || m_fbo->height() != height())
    {
        //allocate additional? FBO for rendering or resize it if widget size changed
        delete m_fbo;
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        m_fbo = new QOpenGLFramebufferObject(width(), height(), format);
        resizeGL(width(), height());
    }

    //bind FBO and paintGL to render image
    m_fbo->bind();
    paintGL();
    //grab the content of the framebuffer rendered
    QImage image = m_fbo->toImage();
    image.save(QString("test_framebufferobj.png"));
    m_fbo->release();

    //bind default framebuffer
    m_fbo->bindDefault();
    doneCurrent();
}

void acOGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    //check opengl version
    QString version = QString((const char*)glGetString(GL_VERSION));
    m_major_ver     = version.left(version.indexOf(".")).toInt();
    m_minor_ver     = version.mid(version.indexOf(".") + 1, 1).toInt();

    qDebug() << m_major_ver << m_minor_ver;

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

    //! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
    //! [2]

    geometries = new GeometryEngine;
}

//! [3]
void acOGLWidget::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/AMDCompressGUI/Shaders/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/AMDCompressGUI/Shaders/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}
//! [3]

//! [4]
void acOGLWidget::initTextures()
{
    glEnable(GL_TEXTURE_2D);

    // Typical Texture Generation
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//! [4]
void acOGLWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture[0]);
    //! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);
    //! [6]

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("texture", 0);

    // Draw cube geometry
    geometries->drawCubeGeometry(&program);
}
//![4]

//! [5]
void acOGLWidget::resizeGL(int w, int h)
{
    w = m_ImageW;
    h = m_ImageH;
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]
