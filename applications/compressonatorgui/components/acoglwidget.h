//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================

#ifndef __ACOGLWIDGET_H
#define __ACOGLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include "pluginmanager.h"
#include <QWidget>
#include <qdebug.h>

#if defined(_WIN32)
#include <gl/GL.h>
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "accustomgraphics.h"

#include "geometryengine.h"

class GeometryEngine;
// ----------------------------------------------------------------------------------
// Class Name:          acOGLWidget
// General Description: This class represent an image item widget that is based on OpenGL
// Creation Date (MM/DD):       11/16/2015
// ----------------------------------------------------------------------------------

class acOGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

  public:
    int ID;

    explicit acOGLWidget(QWidget *parent = 0);
    virtual ~acOGLWidget();
#if 0
    GLenum  MIP2OLG_Format(MipSet *m_MipSet);
    bool loadMIPImage(MipSet *m_MipSet, CMIPS *cMips, int MIPLevel);
#endif
    void drawOffscreen();  //not in used- just for testing

    // OpenGL Version
    int m_major_ver;
    int m_minor_ver;


  protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

    void initShaders();
    void initTextures();


    GLfloat  m_ImageH;
    GLfloat  m_ImageW;

  private:
    GLuint texture[1];
    QOpenGLShaderProgram program;
    GeometryEngine *geometries;
    QMatrix4x4 projection;
    //for drawOffscreen() testing
    QOpenGLFramebufferObject* m_fbo = nullptr;
    bool m_isInitialized = false;
};

#endif
