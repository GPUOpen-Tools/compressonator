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

#ifndef QT3D_RENDER_RENDERSHADER_H
#define QT3D_RENDER_RENDERSHADER_H

#include <QVector>
#include <Qt3DRenderer/private/quniformvalue_p.h>
#include <Qt3DRenderer/private/shadervariables_p.h>
#include <Qt3DCore/qbackendnode.h>

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;
class QMutex;

namespace Qt3D {

class QShaderProgram;

namespace Render {

class ShaderManager;
class AttachmentPack;

typedef uint ProgramDNA;

class Q_AUTOTEST_EXPORT RenderShader : public QBackendNode
{
public:
    RenderShader();
    ~RenderShader();

    void cleanup();

    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;
    void updateUniforms(QGraphicsContext *ctx, const QUniformPack &pack);
    void setFragOutputs(const QHash<QString, int> &fragOutputs);

    QVector<QString> uniformsNames() const;
    QVector<QString> attributesNames() const;
    QVector<QString> uniformBlockNames() const;
    QVector<QByteArray> shaderCode() const;

    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;
    bool isLoaded() const;
    ProgramDNA dna() const;

    QVector<ShaderUniform> uniforms() const;
    QVector<ShaderAttribute> attributes() const;
    QVector<ShaderUniformBlock> uniformBlocks() const;

    QHash<QString, ShaderUniform> activeUniformsForBlock(int blockIndex) const;
    ShaderUniformBlock uniformBlock(int blockIndex);
    ShaderUniformBlock uniformBlock(const QString &blockName);

private:
    QOpenGLShaderProgram *m_program;

    QOpenGLShaderProgram *createProgram(QGraphicsContext *context);
    QOpenGLShaderProgram *createDefaultProgram();

    QVector<QString> m_uniformsNames;
    QVector<ShaderUniform> m_uniforms;

    QVector<QString> m_attributesNames;
    QVector<ShaderAttribute> m_attributes;

    QVector<QString> m_uniformBlockNames;
    QVector<ShaderUniformBlock> m_uniformBlocks;
    QHash<int, QHash<QString, ShaderUniform> > m_blockIndexToShaderUniforms;

    QHash<QString, int> m_fragOutputs;

    QVector<QByteArray> m_shaderCode;

    bool m_isLoaded;
    ProgramDNA m_dna;
    QMutex *m_mutex;

    void updateDNA();

    // Private so that only GraphicContext can call it
    void initializeUniforms(const QVector<ShaderUniform> &uniformsDescription);
    void initializeAttributes(const QVector<ShaderAttribute> &attributesDescription);
    void initializeUniformBlocks(const QVector<ShaderUniformBlock> &uniformBlockDescription);

    void initialize(const RenderShader &other);

    QOpenGLShaderProgram *getOrCreateProgram(QGraphicsContext *ctx);
    friend class QGraphicsContext;
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERSHADER_H
