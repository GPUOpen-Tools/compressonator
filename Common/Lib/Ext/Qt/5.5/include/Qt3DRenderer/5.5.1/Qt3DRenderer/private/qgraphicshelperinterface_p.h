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

#ifndef QT3D_RENDER_QGRAPHICSHELPERINTERFACE_H
#define QT3D_RENDER_QGRAPHICSHELPERINTERFACE_H

#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QVector>
#include <Qt3DRenderer/private/shadervariables_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

struct Attachment;

class QGraphicsHelperInterface
{
public:
    enum Feature {
        MRT = 0,
        Tessellation,
        UniformBufferObject,
        BindableFragmentOutputs,
        PrimitiveRestart
    };

    virtual ~QGraphicsHelperInterface() {}
    virtual void    initializeHelper(QOpenGLContext *context, QAbstractOpenGLFunctions *functions) = 0;
    virtual void    drawElementsInstanced(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void * indices, GLsizei instances, GLint baseVertex = 0, GLint baseInstance = 0) = 0;
    virtual void    drawArraysInstanced(GLenum primitiveType, GLint first, GLsizei count, GLsizei instances) = 0;
    virtual void    drawElements(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void * indices, GLint baseVertex = 0) = 0;
    virtual void    drawArrays(GLenum primitiveType, GLint first, GLsizei count) = 0;
    virtual void    setVerticesPerPatch(GLint verticesPerPatch) = 0;
    virtual void    useProgram(GLuint programId) = 0;
    virtual QVector<ShaderUniform> programUniformsAndLocations(GLuint programId) = 0;
    virtual QVector<ShaderAttribute> programAttributesAndLocations(GLuint programId) = 0;
    virtual QVector<ShaderUniformBlock> programUniformBlocks(GLuint programId) = 0;
    virtual void    vertexAttribDivisor(GLuint index, GLuint divisor) = 0;
    virtual void    blendEquation(GLenum mode) = 0;
    virtual void    alphaTest(GLenum mode1, GLenum mode2) = 0;
    virtual void    depthTest(GLenum mode) = 0;
    virtual void    depthMask(GLenum mode) = 0;
    virtual void    cullFace(GLenum mode) = 0;
    virtual void    frontFace(GLenum mode) = 0;
    virtual void    enableAlphaCoverage() = 0;
    virtual void    disableAlphaCoverage() = 0;
    virtual GLuint  createFrameBufferObject() = 0;
    virtual void    releaseFrameBufferObject(GLuint frameBufferId) = 0;
    virtual void    bindFrameBufferObject(GLuint frameBufferId) = 0;
    virtual GLuint  boundFrameBufferObject() = 0;
    virtual bool    checkFrameBufferComplete() = 0;
    virtual void    bindFrameBufferAttachment(QOpenGLTexture *texture, const Attachment &attachment) = 0;
    virtual bool    supportsFeature(Feature feature) const = 0;
    virtual void    drawBuffers(GLsizei n, const int *bufs) = 0;
    virtual void    bindFragDataLocation(GLuint shader, const QHash<QString, int> &outputs) = 0;
    virtual void    bindUniform(const QVariant &v, const ShaderUniform &description) = 0;
    virtual void    bindUniformBlock(GLuint programId, GLuint uniformBlockIndex, GLuint uniformBlockBinding) = 0;
    virtual void    bindBufferBase(GLenum target, GLuint index, GLuint buffer) = 0;
    virtual void    buildUniformBuffer(const QVariant &v, const ShaderUniform &description, QByteArray &buffer) = 0;
    virtual uint    uniformByteSize(const ShaderUniform &description) = 0;
    virtual void    enableClipPlane(int clipPlane) = 0;
    virtual void    disableClipPlane(int clipPlane) = 0;
    virtual GLint   maxClipPlaneCount() = 0;
    virtual void    enablePrimitiveRestart(int primitiveRestartIndex) = 0;
    virtual void    disablePrimitiveRestart() = 0;
};


} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QGRAPHICSHELPERINTERFACE_H
