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

#ifndef QT3D_RENDER_QNORMALDIFFUSESPECULARMAPMATERIAL_P_H
#define QT3D_RENDER_QNORMALDIFFUSESPECULARMAPMATERIAL_P_H

#include <Qt3DRenderer/private/qmaterial_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QEffect;
class QAbstractTextureProvider;
class QTechnique;
class QParameter;
class QShaderProgram;
class QRenderPass;
class QParameterMapping;

class QNormalDiffuseSpecularMapMaterial;

class QNormalDiffuseSpecularMapMaterialPrivate : public QMaterialPrivate
{
public:
    QNormalDiffuseSpecularMapMaterialPrivate();

    void init();

    QEffect *m_normalDiffuseSpecularEffect;
    QAbstractTextureProvider *m_diffuseTexture;
    QAbstractTextureProvider *m_normalTexture;
    QAbstractTextureProvider *m_specularTexture;
    QParameter *m_ambientParameter;
    QParameter *m_diffuseParameter;
    QParameter *m_normalParameter;
    QParameter *m_specularParameter;
    QParameter *m_shininessParameter;
    QParameter *m_lightPositionParameter;
    QParameter *m_lightIntensityParameter;
    QParameter *m_textureScaleParameter;
    QTechnique *m_normalDiffuseSpecularGL3Technique;
    QTechnique *m_normalDiffuseSpecularGL2Technique;
    QTechnique *m_normalDiffuseSpecularES2Technique;
    QRenderPass *m_normalDiffuseSpecularGL3RenderPass;
    QRenderPass *m_normalDiffuseSpecularGL2RenderPass;
    QRenderPass *m_normalDiffuseSpecularES2RenderPass;
    QShaderProgram *m_normalDiffuseSpecularGL3Shader;
    QShaderProgram *m_normalDiffuseSpecularGL2ES2Shader;

    Q_DECLARE_PUBLIC(QNormalDiffuseSpecularMapMaterial)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QNORMALDIFFUSESPECULARMAPMATERIAL_P_H

