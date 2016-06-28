/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDER_QGRAPHICSHELPERGL3_H
#define QT3D_RENDER_QGRAPHICSHELPERGL3_H

#include <Qt3DRenderer/private/qgraphicshelperinterface_p.h>
#include <QtCore/qscopedpointer.h>

#ifndef QT_OPENGL_ES_2

QT_BEGIN_NAMESPACE

class QOpenGLFunctions_3_2_Core;
class QOpenGLExtension_ARB_tessellation_shader;

namespace Qt3D {
namespace Render {

class QGraphicsHelperGL3 : public QGraphicsHelperInterface
{
public:
    QGraphicsHelperGL3();

    // QGraphicHelperInterface interface
    void initializeHelper(QOpenGLContext *context, QAbstractOpenGLFunctions *functions) Q_DECL_OVERRIDE;
    void drawElementsInstanced(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void *indices, GLsizei instances, GLint baseVertex = 0,  GLint baseInstance = 0) Q_DECL_OVERRIDE;
    void drawArraysInstanced(GLenum primitiveType, GLint first, GLsizei count, GLsizei instances) Q_DECL_OVERRIDE;
    void drawElements(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void *indices, GLint baseVertex = 0) Q_DECL_OVERRIDE;
    void drawArrays(GLenum primitiveType, GLint first, GLsizei count) Q_DECL_OVERRIDE;
    void setVerticesPerPatch(GLint verticesPerPatch) Q_DECL_OVERRIDE;
    void useProgram(GLuint programId) Q_DECL_OVERRIDE;
    QVector<ShaderUniform> programUniformsAndLocations(GLuint programId) Q_DECL_OVERRIDE;
    QVector<ShaderAttribute> programAttributesAndLocations(GLuint programId) Q_DECL_OVERRIDE;
    QVector<ShaderUniformBlock> programUniformBlocks(GLuint programId) Q_DECL_OVERRIDE;
    void vertexAttribDivisor(GLuint index, GLuint divisor) Q_DECL_OVERRIDE;
    void blendEquation(GLenum mode) Q_DECL_OVERRIDE;
    void alphaTest(GLenum mode1, GLenum mode2) Q_DECL_OVERRIDE;
    void depthTest(GLenum mode) Q_DECL_OVERRIDE;
    void depthMask(GLenum mode) Q_DECL_OVERRIDE;
    void cullFace(GLenum mode) Q_DECL_OVERRIDE;
    void frontFace(GLenum mode) Q_DECL_OVERRIDE;
    void enableAlphaCoverage() Q_DECL_OVERRIDE;
    void disableAlphaCoverage() Q_DECL_OVERRIDE;
    GLuint createFrameBufferObject() Q_DECL_OVERRIDE;
    void releaseFrameBufferObject(GLuint frameBufferId) Q_DECL_OVERRIDE;
    void bindFrameBufferObject(GLuint frameBufferId) Q_DECL_OVERRIDE;
    GLuint boundFrameBufferObject() Q_DECL_OVERRIDE;
    bool checkFrameBufferComplete() Q_DECL_OVERRIDE;
    void bindFrameBufferAttachment(QOpenGLTexture *texture, const Attachment &attachment) Q_DECL_OVERRIDE;
    bool supportsFeature(Feature feature) const Q_DECL_OVERRIDE;
    void drawBuffers(GLsizei n, const int *bufs) Q_DECL_OVERRIDE;
    void bindFragDataLocation(GLuint shader, const QHash<QString, int> &outputs) Q_DECL_OVERRIDE;
    void bindUniform(const QVariant &v, const ShaderUniform &description) Q_DECL_OVERRIDE;
    void bindUniformBlock(GLuint programId, GLuint uniformBlockIndex, GLuint uniformBlockBinding) Q_DECL_OVERRIDE;
    void bindBufferBase(GLenum target, GLuint bindingIndex, GLuint buffer) Q_DECL_OVERRIDE;
    void buildUniformBuffer(const QVariant &v, const ShaderUniform &description, QByteArray &buffer) Q_DECL_OVERRIDE;
    uint uniformByteSize(const ShaderUniform &description) Q_DECL_OVERRIDE;
    void enableClipPlane(int clipPlane) Q_DECL_OVERRIDE;
    void disableClipPlane(int clipPlane) Q_DECL_OVERRIDE;
    GLint maxClipPlaneCount() Q_DECL_OVERRIDE;
    void enablePrimitiveRestart(int primitiveRestartIndex) Q_DECL_OVERRIDE;
    void disablePrimitiveRestart() Q_DECL_OVERRIDE;

private:
    QOpenGLFunctions_3_2_Core *m_funcs;
    QScopedPointer<QOpenGLExtension_ARB_tessellation_shader> m_tessFuncs;
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // !QT_OPENGL_ES_2

#endif // QT3D_RENDER_QGRAPHICSHELPERGL3_H
