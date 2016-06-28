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

#ifndef QT3D_RENDER_QNORMALDIFFUSESPECULARMAPMATERIAL_H
#define QT3D_RENDER_QNORMALDIFFUSESPECULARMAPMATERIAL_H

#include <Qt3DRenderer/qmaterial.h>
#include <QColor>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QNormalDiffuseSpecularMapMaterialPrivate;

class QT3DRENDERERSHARED_EXPORT QNormalDiffuseSpecularMapMaterial : public QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(QAbstractTextureProvider *diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(QAbstractTextureProvider *normal READ normal WRITE setNormal NOTIFY normalChanged)
    Q_PROPERTY(QAbstractTextureProvider *specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(float textureScale READ textureScale WRITE setTextureScale NOTIFY textureScaleChanged)

public:
    explicit QNormalDiffuseSpecularMapMaterial(QNode *parent = 0);
    ~QNormalDiffuseSpecularMapMaterial();

    QColor ambient() const;
    QAbstractTextureProvider *diffuse() const;
    QAbstractTextureProvider *normal() const;
    QAbstractTextureProvider *specular() const;
    float shininess() const;
    float textureScale() const;

    void setAmbient(const QColor &ambient);
    void setDiffuse(QAbstractTextureProvider *diffuse);
    void setNormal(QAbstractTextureProvider *normal);
    void setSpecular(QAbstractTextureProvider *specular);
    void setShininess(float shininess);
    void setTextureScale(float textureScale);

Q_SIGNALS:
    void ambientChanged();
    void diffuseChanged();
    void normalChanged();
    void specularChanged();
    void shininessChanged();
    void textureScaleChanged();

protected:
    QNormalDiffuseSpecularMapMaterial(QNormalDiffuseSpecularMapMaterialPrivate &dd, QNode *parent = 0);

private:
    Q_DECLARE_PRIVATE(QNormalDiffuseSpecularMapMaterial)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QNORMALDIFFUSESPECULARMAPMATERIAL_H
