/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_RENDER_QSKYBOXENTITY_P_H
#define QT3D_RENDER_QSKYBOXENTITY_P_H

#include <Qt3DCore/private/qentity_p.h>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QTranslateTransform;
class QTextureCubeMap;
class QShaderProgram;
class QSkyboxEntity;
class QTextureImage;
class QCuboidMesh;
class QRenderPass;
class QTechnique;
class QTransform;
class QParameter;
class QMaterial;
class QEffect;

class QSkyboxEntityPrivate : public QEntityPrivate
{
    QSkyboxEntityPrivate();

    void init();
    void reloadTexture();

    Q_DECLARE_PUBLIC(QSkyboxEntity)

    QEffect *m_effect;
    QMaterial *m_material;
    QTextureCubeMap *m_skyboxTexture;
    QShaderProgram *m_gl3Shader;
    QShaderProgram *m_gl2es2Shader;
    QTechnique *m_gl2Technique;
    QTechnique *m_es2Technique;
    QTechnique *m_gl3Technique;
    QRenderPass *m_gl2RenderPass;
    QRenderPass *m_es2RenderPass;
    QRenderPass *m_gl3RenderPass;
    QCuboidMesh *m_mesh;
    QTransform *m_transform;
    QTranslateTransform *m_translate;
    QParameter *m_textureParameter;
    QTextureImage *m_posXImage;
    QTextureImage *m_posYImage;
    QTextureImage *m_posZImage;
    QTextureImage *m_negXImage;
    QTextureImage *m_negYImage;
    QTextureImage *m_negZImage;
    QString m_extension;
    QString m_baseName;
    QVector3D m_position;
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QSKYBOXENTITY_P_H

